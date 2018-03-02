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
vector<Train> scheduleE;
vector<Train> scheduleW;
char last = 'w';

chrono::time_point<chrono::system_clock> start;
condition_variable cv;
mutex mx;

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

template<typename T>
void pop_front(vector<T>& vec) {
    assert(!vec.empty());
    vec.erase(vec.begin());
}

void sortEqs() {
	
}

void schedTrains(char lastDir, Train tr) {
	// maybe keep track f num of E, W, e, w and use single queue
	//if(schedule.empty() /*&& check mutex lock*/); // No scheduled trains, so just send to main track.
	//	return tr;
	// remember the case of >2 equal loadTimes.
	//int len = schedule.size();
	bool eqLoad = false;
	char dir = tr.direction;
	int lt = tr.loadTime;
	//if(lt == schedule.back().loadTime) {}
	//	eqLoad = true;
	if(dir == 'E') { // last dir was w?
		if(eqLoad) {
			for(int i = len-1; tr.loadTime == scheduleE[i]; i--);
			while()
		}
		//schedule.insert(tr);
		E++;
	} else if(dir == 'W') {
		W++;
	} else if(dir == 'e') {
		e++;
	} else if(dir == 'w') {
		w++;
	}
	//return schedule.pop();
	/*
	if high > 0 use high queues
	else use low queues
	
	if the size is 1, sent to main
	
	otherwise if west or east is zero, send the non-zero one
	
	otherwise, send the one that was not last sent
	
	*/
	
} // e and w set or high and low set?

Train getNextTrain(char lastDir) { // next train to cross. Give last dir, choose appropriate q.
	// first do load times.
	// if eq, then do by dir. This should naturally lead.
	if(scheduleE.front().loadTime == scheduleW.front().loadTime)
	
	if(lastDir == 'w' || lastDir == 'W') {
		Train = scheduleW.front(); // erase it?
		return
	}
}

void loadTrain(Train t) { // loads train

	this_thread::sleep_for(chrono::milliseconds(t.loadTime*100));
	
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
	// http://en.cppreference.com/w/cpp/thread/condition_variable
	//cv.wait(lk, []{return true;});
	loadTrain(t);
	// lock schedule
	// Do scheduling stuff.
	// schedTrains(tr);
	
	//if(!mx.try_lock()) { cout << "uh" << endl;}
	// All threads will end up waiting - a notify one to wake one who will pop.
	unique_lock<mutex> lk(mx); // critical main track
	// pop thing from schefule for cross
	// cout << schedule.pop() << endl;
	crossMain(t);
	lk.unlock();
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
	
	for(int i = 0; i < num_threads; i++) {
		trains[i] = thread(startTrain, trainList[i]);
	}
	
	cout << elapsedTime(start) << " and some text" << '\n';
	
	// create thread per train for loading
	
	// some indicator for when done loading, if no mutex, load to main thread
	// if mutex, load into other data struct in processing thread and sort upon unlock
	
	// eventually we will be done.
	
	for(int i = 0; i < num_threads; i++)
		trains[i].join();
	cout << "Hello world! I'm using " << argv[1] << endl;
	return 0;
}

// http://en.cppreference.com/w/cpp/thread/condition_variable
// https://stackoverflow.com/questions/43614634/stdthread-how-to-wait-join-for-any-of-the-given-threads-to-complete
// http://www.cplusplus.com/reference/thread/thread/
// https://stackoverflow.com/questions/36602080/c11-thread-hangs-on-locking-mutex
// https://stackoverflow.com/questions/30411790/how-to-use-different-comparators-for-a-priority-queue-depending-on-conditions