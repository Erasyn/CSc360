#include <iostream>
#include <fstream>
#include <string>
#include <thread>

using namespace std;


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
		cerr << "usage: ./mts.exe <train_text_file>" << endl;
		return 0;
	}
	ifstream trainIn;
	trainIn.open(argv[1]);
	string in;
	//vector<string> parsed;
	while(!trainIn.eof()) {
		getline(trainIn, in, ' ');
		// some data structure to store them.
		cout << in << endl;
	}
	trainIn.close();
	
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