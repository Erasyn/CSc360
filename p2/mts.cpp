#include <iostream>
#include <fstream>
#include <string>
#include <thread>

using namespace std;




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
	// if mutex, load into other data struct in processing thread
	
	// eventually we will be done.
	
	
	cout << "Hello world! I'm using " << argv[1] << endl;
	return 0;
}


// http://en.cppreference.com/w/cpp/thread/condition_variable
// https://stackoverflow.com/questions/43614634/stdthread-how-to-wait-join-for-any-of-the-given-threads-to-complete
// http://www.cplusplus.com/reference/thread/thread/