#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <list>
#include  <queue>
#include <bits/stdc++.h>


using namespace std;


//Global Variables



//Structure Declarations
struct fileRead{
    string line;
    ifstream inFile;
    char *tok;
    char *buffer;
    int ofs;
    int count;
    char* filepath;
};

struct io{

};
//Scheduler Class
class Scheduler{
	
	public:
	list<io*> ioqueue;
	// virtual frame_t* select_victim_frame(vector<Process> &plist)=0;
};

//Scheduler Sub Classes

//FIFO
class FIFO: public Scheduler{
	
	public:
	// frame_t* select_victim_frame(vector<Process> &plist);
};
//FIFO function

// frame_t* FIFO::select_victim_frame(vector<Process> &plist){
//  	frame_t *tempframe = flist.front();
//  	flist.pop_front();
// 	flist.push_back(tempframe);
// 	return tempframe;
// }


//SSTF
//LOOK
//CLOOK
//FLOOK

//Function Declarations
void printFile(char* filepath);
void readFile(fileRead *inputfile, vector<Process> &plist);

//Main Function
int main(int argc, char** argv) {
char* inputfilepath;
    char* randfilepath
    char *svalue = NULL;
    opterr = 0;
    filepath =0;
    //Extract Options from Input
    while ((c = getopt (argc, argv, "s:")) != -1)
	    switch (c)
	      {
	        case 's':
	        svalue = optarg;
	        break;
	      case '?':
	        if (isprint (optopt))
	          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	        else
	          fprintf (stderr,
	                   "Unknown option character `\\x%x'.\n",
	                   optopt);
	        return 1;
	      default:
	        abort ();
	      }
	    

      for (index = optind; index < argc; index++){
           if (filepath==0){inputfilepath=argv[index]; filepath++; break;}
      }
      
}
//Simulation

//Helper Functions
//Prints Input File for debugging
void printFile(char* filepath){
	cout << "Printing File" <<endl;
    string line;
    ifstream inFile;
    inFile.open(filepath);
    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    char *tok;
    char *buffer;
    getline(inFile,line);

    if(!inFile.eof()){        
		while (getline(inFile,line)){
			cout<<line<<endl;
			}
	}
	cout << endl;
}
