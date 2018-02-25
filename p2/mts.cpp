#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <chrono>

using namespace std;

const int E = 0;
const int W = 1;
const int e = 2;
const int w = 3;

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

	char getDirection() {
		return direction;
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

vector<Train> schedTrains(vector<Train> trList, char lastDir, Train tr) { // changed to linked list, take head as additional input
	// maybe keep track f num of E, W, e, w and use single queue
	char dir = tr.getDirection();
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

void startTrain(Train t) {
	//usleep(t.loadTime * 100000);
	this_thread::sleep_for(chrono::milliseconds(t.loadTime*100));
	//printf("%02d:%02d:%04.1f",h,m,s);
	cout << "Done sleeping " << double(t.loadTime) / 10.0 << " seconds" << endl;
}

string elapsedTime(chrono::time_point<chrono::system_clock> start) {
	chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
	int timeSpan = chrono::duration_cast<chrono::milliseconds>(end-start).count();
	int hours = timeSpan / 3600000;
	int minutes = timeSpan / 60000;
	double seconds = timeSpan / 1000.0;
	char t[10];
	sprintf(t, "%02d:%02d:%04.1f",hours,minutes,seconds);
	// format this into a string somehow?
	return t;
}

int main(int argc, char* argv[]) {
	if(argc != 2) {
		cerr << "usage: ./mts.exe <train_text_file>.txt" << endl;
		return 0;
	}
	
	ifstream trainFile;
	trainFile.open(argv[1]);
	string in;
	int num = 0;
	vector<pthread_t> trainTracks[4];
	vector<Train> trainList;
	
	while(getline(trainFile, in)) {
		istringstream iss(in);

        string d, l, c;
        getline(iss,d,' ');
        getline(iss,l,' ');
        getline(iss,c,' ');
		Train newTrain(num++,d,l,c);
        trainList.push_back(newTrain);

		cout << num << " loop" << endl;
		
	}
	trainFile.close();

	chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
	
	for(Train x: trainList)
		startTrain(x);
	
	cout << elapsedTime(start) << " and some text" << '\n';
	
	// create thread per train for loading
	
	// some indicator for when done loading, if no mutex, load to main thread
	// if mutex, load into other data struct in processing thread and sort upon unlock
	
	// eventually we will be done.
	
	
	cout << "Hello world! I'm using " << argv[1] << endl;
	return 0;
}
 

// http://en.cppreference.com/w/cpp/thread/condition_variable
// https://stackoverflow.com/questions/43614634/stdthread-how-to-wait-join-for-any-of-the-given-threads-to-complete
// http://www.cplusplus.com/reference/thread/thread/