#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <vector>

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
	
	
};

void schedTrains(char lastDir) { // changed to linked list, take head as additional input
	// maybe keep track f num of E, W, e, w and use single queue
	/*
	if high > 0 use high queues
	else use low queues
	
	if the size is 1, sent to main
	
	otherwise if west or east is zero, send the non-zero one
	
	otherwise, send the one that was not last sent
	
	*/
	
}// e and w set or high and low set?

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
        //cout << in << endl;
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
	
	for(Train x: trainList)
		cout << "," << x.getInfo();
	cout << '\n';
	
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