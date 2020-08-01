#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bitset>
#include <vector>

using namespace std;



// Data Structures to try
// free lists for physical frames, holes and pages
// holes in VMAS
// struct for pte and bit structures
// struct for frame table

// classes for pagers
// random number reader from file, store in vector this time

// file reader for input files
// file printer, edit to ignore lines with #

// Read instruction set, keep track of instructions
// input options invocations  (lab2)

// figure out 32-bit information
// figure out 
struct fileRead{
    string line;
    ifstream inFile;
    char *tok;
    char *buffer;
    int ofs;
    int count;
    char* filepath;
};



struct pte_tag{
	unsigned int framenumber: 7;
	unsigned int state: 5;
	unsigned int info : 20;
};

struct fte_tag{
	int pid;
	unsigned int vaddr: 6;
};

struct vma_tag{
	int s_page, e_page, write_p, f_mapped;
};


class Pager{};

void printFile(char* filepath);
void readFile(fileRead *inputfile);
void getTok(fileRead *inputfile);

int main(int argc, char** argv) {
    //Obtain file name from command line
    char* inputfilepath;
    char* randfilepath;

    int vflag = 0;
    char *svalue = NULL;
    int index;
    int filepath=0;
    int c;
    opterr = 0;


    while ((c = getopt (argc, argv, "vs:")) != -1)
    switch (c)
      {
      case 'v':
        vflag = 1;
        break;
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

    // Exctract Sched_type and Time Quantum
    // char sched_type = svalue[0];
    // string tq = svalue;
    // int timequantum;
    // if (strlen(svalue)==1){timequantum=10000;}
    // else{tq.erase(0,1); 
    // timequantum = atoi(tq.c_str());

    // }
    

      for (index = optind; index < argc; index++){
           if (filepath==0){inputfilepath=argv[index]; filepath++; continue;}
           if (filepath==1){randfilepath=argv[index]; break;}
      }
      
    fileRead *inputfile = new fileRead;
    inputfile->inFile.open(inputfilepath);

    // Print File for debugging, not required in submission, should be commented out
    printFile(inputfilepath);  
    readFile(inputfile);

    //bitField testing
    pte_tag pte = {0,1,2};
    // pte.framenumber = 127;
    cout << pte.framenumber <<endl;    
    cout << ++pte.framenumber <<endl;
    cout << sizeof(pte) <<endl;
  	vector<pte_tag> ptable;
  	ptable.push_back(pte);
  	cout << ptable.at().info<<endl;


    fte_tag fte = {0};
    fte.pid = 3;
    fte.vaddr = 63;
    cout <<fte.vaddr++ <<endl;
    cout <<fte.vaddr <<endl;
    
}

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

void readFile(fileRead* inputfile){
	cout << "Reading File" << endl;
	
    if (!inputfile->inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int num_proc;
    
    
    if(!inputfile->inFile.eof()){        
		// Get Num of Process
		getTok(inputfile);
	        // Skip lines with #
	    while (*(inputfile->tok)=='#'){
	    	getTok(inputfile);
	    }
	    num_proc = atoi(inputfile->tok);
	    cout << "num_proc = "<<num_proc<<endl;
	    
	    // Read Processes
	    int vma_count;

	    for(int i =0;i < num_proc; i++){
	    	// Get Vma Count 
	    	getTok(inputfile);
	        // Skip lines with #
		    while (*(inputfile->tok)=='#'){
	    		getTok(inputfile);
		    }
		    vma_count = atoi(inputfile->tok);
		    cout << "vma_count = " <<vma_count<< endl;

		    //Read VMAs
		    for (int j= 0; j<vma_count; j++){
		    	getTok(inputfile);

		    	cout << inputfile->line <<endl;
		    	//Code to create vma and enter to list

		    }

	    }
	    inputfile->inFile.close();
	}
}

void getTok(fileRead *inputfile){
	getline(inputfile->inFile,inputfile->line);
	inputfile->buffer = strdup(inputfile->line.c_str());
	inputfile->tok = strtok(inputfile->buffer, " \n \t");
}
