#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

void getToken(string line, ifstream& inFile);
void printFile();
int main() {
    printFile();
    string line;
    ifstream inFile;
	inFile.open("input-1");
	char* tok;
    getToken(line, inFile);
    return 0;
}

void getToken(string line, ifstream& inFile){
	    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    // cout << "File was opened.\n";
    char *tok;
    char *buffer;
    int linenumber = 1;
    size_t found;

    if(inFile){
	    while (getline(inFile,line)) {	        
			buffer = strdup(line.c_str());
			
			tok = strtok(buffer, " \n \t");
			cout << "offset is " << line.find(tok) + 1 << endl;

			while (tok != NULL)
				{
				    printf (" tok is %s\n",tok);
				    tok = strtok (NULL, " \n \t");
				    if (tok == NULL) {break;}
				    else {found =line.find(tok) + 1; cout << "offset is " << found << endl;}
				}

			cout << "line is "<< line << endl;	
			cout << "line number is " << linenumber++ << endl << endl;
			
		}
		cout << "File ended at line " << --linenumber << " offset " <<found << endl;
	}
    inFile.close();
    // cout << "File was closed." << endl; 

}

void printFile(){
	string line;
    ifstream inFile;
	inFile.open("input-1");
    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    // cout << "File was opened.\n";
    char *tok;
    char *buffer;
    int linenumber = 1;
    size_t found;
    if(inFile){
	    while (getline(inFile,line)) {	        
			cout << line << endl;				
		}
	}
    inFile.close();
    cout << endl;
    // cout << "File was closed." << endl; 

}

int readInt();
char* readSym();
