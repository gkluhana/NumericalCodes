#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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


void printFile(char* filepath);

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
    char sched_type = svalue[0];
    string tq = svalue;
    int timequantum;
    if (strlen(svalue)==1){timequantum=10000;}
    else{tq.erase(0,1); 
    timequantum = atoi(tq.c_str());
    }
    

      for (index = optind; index < argc; index++){
           if (filepath==0){inputfilepath=argv[index]; filepath++; continue;}
           if (filepath==1){randfilepath=argv[index]; break;}
      }
      

    // Print File for debugging, not required in submission, should be commented out
    printFile(inputfilepath);  

}

//Prints Input File for debugging
void printFile(char* filepath){
    string line;
    ifstream inFile;
    inFile.open(filepath);
    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    char *tok;
    char *buffer;
    int linenumber = 1;
    size_t found;
    if(!inFile.eof()){
        while (getline(inFile,line)) {          
            cout << line << endl;               
        }
    }

    inFile.close();
}


