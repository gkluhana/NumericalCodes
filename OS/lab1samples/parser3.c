#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

char* getToken();
char* getToken(char* buffer);
void printFile();
int main() {
    printFile();
    string line;
    ifstream inFile;
    inFile.open("input-11");
    
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
            
            tok = getToken(buffer);
            cout << "offset is " << line.find(tok) + 1<< endl;
            while (tok != NULL)
                {
                    printf (" tok is %s\n",tok);
                    tok = getToken();
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

    return 0;
}

char* getToken(char* buffer){

	return strtok(buffer, " \n \t");
}

char* getToken(){
    
    return strtok(NULL, " \n \t");
}

void printFile(){
	string line;
    ifstream inFile;
	inFile.open("input-11");
    
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
