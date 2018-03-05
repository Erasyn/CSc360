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
	~Train() {
		//cout << "Train " << id << " Destructor" << endl;
	}

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
vector<Train> scheduleW;
char last = 'W';
int load = 0;

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

void ins(int diff, Train tr) {
	/*if(diff != 0) {
		while(schedule[diff+1].loadTime == tr.loadTime && schedule[diff+1].id > tr.id) {
			cout << "id" << schedule[diff].id << endl;
			diff--;	
		}
		cout << "sorter?" << endl;
		//diff--;
	}*/
	schedule.insert(schedule.begin()+diff,tr);
}

void schedTrains(Train tr) {
	char dir = tr.direction;
	int diff = 0;
	if(dir == 'E') {
		diff = E;
		ins(diff, tr);
		E++;
	} else if(dir == 'W') {
		diff = E + W;
		ins(diff,tr);
		W++;
	} else if(dir == 'e') {
		diff = E + W + e;
		ins(diff,tr);
		e++;
	} else if(dir == 'w') {
		diff = E + W + e + w;
		ins(diff,tr);
		w++;
	}	
}

int find(char p) {
	for(int i = 0;i < schedule.size(); i++)
		if(schedule[i].direction == p)
			return i;
	return -1;
}

char checkLoad(char lastDir, int lastLoad) {
	int small;
	int eL,wL;
	if(E > 0 && W > 0) {
		eL = schedule[find('E')].loadTime;
		wL = schedule[find('W')].loadTime;
		//small = min(schedule[find('E')].loadTime, schedule[find('W')].loadTime);
		if(eL < wL) {
			return 'w';
		} else if(eL > wL){
			return 'e';
		}
	}
	//cout << e << " and " << w << endl;
	if(e > 0 && w > 0 && E == 0 && W == 0) {
		//cout <<"??"<<endl;
		eL = schedule[find('e')].loadTime;
		wL = schedule[find('w')].loadTime;
		//small = min(schedule[find('e')].loadTime, schedule[find('w')].loadTime);
		if(eL < wL) {
			//cout << "eL " << eL << " and wL " << wL << endl;
			return 'w';
		} else if(eL > wL) {
			return 'e';
		}
	}
	return last;
}

Train getNextTrain(char lastDir, int lastLoad) {
	int idx;
	lastDir = checkLoad(lastDir,lastLoad);
	//cout << "last " << last << endl;
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
		//cout << "wat" << endl;
		if(e > 0) {
			//cout << "in here" << endl;
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
		//cout << "no" << endl;
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

	this_thread::sleep_for(chrono::milliseconds(t.loadTime*100));
	
	printf("%s Train %2d is ready to go %s\n",
		elapsedTime(start).c_str(),t.id,t.getDirection().c_str());
}

int crossMain(Train t) { // cross main track

	printf("%s Train %2d is ON the main track going %s\n",
		elapsedTime(start).c_str(),t.id,t.getDirection().c_str());

	this_thread::sleep_for(chrono::milliseconds(t.crossTime*100));
	
	printf("%s Train %2d is OFF the main track after going %s\n",
		elapsedTime(start).c_str(),t.id,t.getDirection().c_str());
	return t.loadTime;
}

void startTrain(Train t) {
	// http://en.cppreference.com/w/cpp/thread/condition_variable
	loadTrain(t);
	// lock schedule
	unique_lock<mutex> lks(ms);
	// Do scheduling stuff.
	schedTrains(t);
	if(schedule.size() == 1) {
		lks.unlock();
		this_thread::sleep_for(chrono::milliseconds(20));
	} else
		cv.wait(lks);
	//cout << schedule.size() << endl;
	// All threads will end up waiting - a notify one to wake one who will pop.
	unique_lock<mutex> lk(mx); // critical main track
	Train next = getNextTrain(last,load);
	load = crossMain(next);
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

		cout << id << " loop" << endl;
		
	}
	trainFile.close();
	int num_threads = id;

	thread trains[num_threads];
	start = chrono::system_clock::now();
	
	for(int i = num_threads-1; i >= 0; i--) {
		trains[i] = thread(startTrain, trainList[i]);
	}
	
	cout << elapsedTime(start) << " and some text" << '\n';
	
	// create thread per train for loading
	
	// some indicator for when done loading, if no mutex, load to main thread
	// if mutex, load into other data struct in processing thread and sort upon unlock
	
	// eventually we will be done.
	
	for(int i = 0; i < num_threads; i++)
		trains[i].join();
	for (auto x: schedule) {
        cout << ' ' << x.getInfo();
    }
	//cout << endl << "s " << schedule.size();
    
	cout << "Hello world! I'm using " << argv[1] << endl;
	return 0;
}

// http://en.cppreference.com/w/cpp/thread/condition_variable
// https://stackoverflow.com/questions/43614634/stdthread-how-to-wait-join-for-any-of-the-given-threads-to-complete
// http://www.cplusplus.com/reference/thread/thread/
// https://stackoverflow.com/questions/36602080/c11-thread-hangs-on-locking-mutex