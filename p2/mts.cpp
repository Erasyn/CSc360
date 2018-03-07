#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std;

class Train {
	public:
	char direction;
	int loadTime;
	int crossTime;
    string info;
	int id;
	
	//Default Constructor
	Train() {}
	//Parametrized Constructor
	Train(int id, string d, string l, string c) {
        info = d + " " + l + " " + c;
		this->id = id;
		direction = d.c_str()[0];
		loadTime = stoi(l);
		crossTime = stoi(c);
	}

	//Destructor -- TODO
	~Train() {}

    string getInfo() {
        return info;
    }

	string getDirection() {
		if( direction == 'e' or direction == 'E')
			return "East";
		else
			return "West";
	}
};

// Declare globals after object
int E, W, e, w = 0;
vector<Train> schedule;
char last = 'W';

chrono::time_point<chrono::system_clock> start;
condition_variable cv;
mutex mx;
mutex ms;

string elapsedTime(chrono::time_point<chrono::system_clock> start) {
	chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
	int timeSpan = chrono::duration_cast<chrono::milliseconds>(end-start).count();
	int hours = timeSpan / 3600000;
	int minutes = (timeSpan % 3600000)/ 60000;
	double seconds = (timeSpan % 3600000 % 60000) / 1000.0;
	char t[10];
	sprintf(t, "%02d:%02d:%04.1f",hours,minutes,seconds);
	return t;
}

void schedTrains(Train tr) {
	char dir = tr.direction;
	int diff = 0;
	if(dir == 'E') {
		diff = E;
		schedule.insert(schedule.begin()+diff,tr);
		E++;
	} else if(dir == 'W') {
		diff = E + W;
		schedule.insert(schedule.begin()+diff,tr);
		W++;
	} else if(dir == 'e') {
		diff = E + W + e;
		schedule.insert(schedule.begin()+diff,tr);
		e++;
	} else if(dir == 'w') {
		diff = E + W + e + w;
		schedule.insert(schedule.begin()+diff,tr);
		w++;
	}	
}

int find(char p) {
	for(int i = 0;i < schedule.size(); i++)
		if(schedule[i].direction == p)
			return i;
}

Train getNextTrain(char lastDir) {
	int idx;
	if(lastDir == 'W' || lastDir == 'w') {
		if(E > 0) {
			idx = find('E');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			E--;
			last = 'e';
			return t;
		}
		if(W > 0) {
			idx = find('W');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			last = 'w';
			W--;
			return t;
		}
		if(e > 0) {
			idx = find('e');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			e--;
			last = 'e';
			return t;
		}
		if(w > 0) {
			idx = find('w');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			last = 'w';
			w--;
			return t;
		}
	} else {
		if(W > 0) {
			idx = find('W');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			last = 'w';
			W--;
			return t;
		} if(E > 0) {
			idx = find('E');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			last = 'e';
			E--;
			return t;
		} if(w > 0) {
			idx = find('w');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			last = 'w';
			w--;
			return t;
		} if(e > 0) {
			idx = find('e');
			Train t = schedule[idx];
			schedule.erase(schedule.begin()+idx);
			last = 'e';
			e--;
			return t;
		}
	}
}

void loadTrain(Train t) { // loads train

	this_thread::sleep_for(chrono::milliseconds((t.loadTime*100)+(t.id)));
	start += chrono::milliseconds(t.id);
	printf("%s Train %2d is ready to go %s\n",
		elapsedTime(start).c_str(),t.id,t.getDirection().c_str());
}

void crossMain(Train t) { // cross main track

	printf("%s Train %2d is ON the main track going %s\n",
		elapsedTime(start).c_str(),t.id,t.getDirection().c_str());

	this_thread::sleep_for(chrono::milliseconds(t.crossTime*100));
	
	printf("%s Train %2d is OFF the main track after going %s\n",
		elapsedTime(start).c_str(),t.id,t.getDirection().c_str());
}

void startTrain(Train t) {
	
	unique_lock<mutex> lck(ms);
	cv.wait(lck);
	lck.unlock();
	
	loadTrain(t);
	// lock schedule
	unique_lock<mutex> lks(ms);
	// Do scheduling stuff.
	schedTrains(t);
	if(schedule.size() == 1) {
		lks.unlock();
		this_thread::sleep_for(chrono::milliseconds(10));
	} else
		cv.wait(lks);
	// All threads will end up waiting - a notify one to wake one who will pop.
	unique_lock<mutex> lk(mx); // critical main track
	crossMain(getNextTrain(last));
	lk.unlock();
	cv.notify_one();
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		cerr << "usage: ./mts.exe <train_text_file>.txt" << endl;
		return 0;
	}
	ifstream trainFile;
	trainFile.open(argv[1]);
	string in;
	int id = 0;
	vector<Train> trainList;
	
	while(getline(trainFile, in)) {
		istringstream iss(in);

        string d, l, c;
        getline(iss,d,' ');
        getline(iss,l,' ');
        getline(iss,c,' ');
		Train newTrain(id++,d,l,c);
        trainList.push_back(newTrain);
	}
	trainFile.close();
	int num_threads = id;

	thread trains[num_threads];
	
	unique_lock<mutex> lck(ms);
	for(int i = 0; i < num_threads; i++) {
		trains[i] = thread(startTrain, trainList[i]);
	}
	lck.unlock();
	this_thread::sleep_for(chrono::milliseconds(1));
	start = chrono::system_clock::now();
	cv.notify_all();
	
	for(int i = 0; i < num_threads; i++)
		trains[i].join();
	
	return 0;
}