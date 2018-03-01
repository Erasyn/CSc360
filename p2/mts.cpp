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

const int E = 0;
const int W = 1;
const int e = 2;
const int w = 3;

chrono::time_point<chrono::system_clock> start;
condition_variable cv;
mutex mx;

class Train {
	public:
	char direction;
	int loadTime;
	int crossTime;
    string info;
	int id;
	
	//Default Constructor
	Train() {
		
	}
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
	
	int getLoadTime() {
		return loadTime;
	}
	
	int getCrossTime() {
		return crossTime;
	}
	
	int getId() {
		return id;
	}
	
};

string elapsedTime(chrono::time_point<chrono::system_clock> start) {
	chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
	int timeSpan = chrono::duration_cast<chrono::milliseconds>(end-start).count();
	int hours = timeSpan / 3600000;
	int minutes = timeSpan / 60000;
	double seconds = timeSpan / 1000.0;
	char t[10];
	sprintf(t, "%02d:%02d:%04.1f",hours,minutes,seconds);
	return t;
}

vector<Train> schedTrains(vector<Train> trList, char lastDir, Train tr) { // changed to linked list, take head as additional input
	// maybe keep track f num of E, W, e, w and use single queue
	char dir = tr.direction;
	int lastLoadTime = trList.back().getLoadTime();
	if(dir == 'E') {

	} else if(dir == 'W') {

	} else if(dir == 'e') {

	} else if(dir == 'w') {

	}
	/*
	if high > 0 use high queues
	else use low queues
	
	if the size is 1, sent to main
	
	otherwise if west or east is zero, send the non-zero one
	
	otherwise, send the one that was not last sent
	
	*/
	
} // e and w set or high and low set?

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
	//if(!mx.try_lock()) { cout << "uh" << endl;}
	unique_lock<mutex> lk(mx); // critical main track
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