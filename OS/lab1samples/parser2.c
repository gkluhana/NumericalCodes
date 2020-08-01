#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

char* getToken();
char* getToken(char* buffer);
void printFile();
void Pass1();
int readInt(char* tok);
char* readSym(char* tok);
void storeSymbol(char* symbolTable[],char* symbol, int valueTable[],int value,int symModTable[],int moduleNum);
void printSymbolTable(char* symbolTable[],int baseAdd[],int symModTable[]);

int main() {
    printFile();
//    getToken();
    Pass1();        //Should take path for file

//    printOutput();
    return 0;
}

void Pass1(){
    //Open File
    const char* FileName = "input-2";         //This should be a full path later on
    string line;
    ifstream inFile;
    inFile.open(FileName);    // 
    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    //Read First character , defcount, read first module

    // cout << "File was opened.\n";
    // Initializing Variables
    char *tok;
    char *buffer;
    int linenumber = 1;
    size_t found;
    int moduleNum = 0;
    int defcount = 0; //resets on each module
    int usecount = 0; //resets on each module
    int instrcount = 0; // does not reset
    int baseAdd = 0; // increases accourding to instrcount
    int address = 0;
    char* symbolTable[512];
    char* symbol;
    int value;
    int symModTable[512];
    int valueTable[512];
    int baseAddTable[512];

    char* instrtype;


    if(inFile){             //File has been successfully opened
        while (getline(inFile,line)) {          //while you can get a line, 
            buffer = strdup(line.c_str());      //put the line string to buffer char*
            
            //Get Next Token
            tok = getToken(buffer);     //get the first token
            defcount = readInt(tok);                 //send tok to readInt to verify input
            // check defcount number <16, report error

            for (int i=0;i<defcount;i++){                //read defcount line
                tok = getToken();
                symbol = readSym(tok);          //readSym checks tok valid, puts in symbol table?
                                                //readSym should also check if it's also been defined
                                                //if it's already been defined, use first value, warning message
                tok = getToken(); 
                value  = readInt(tok);             //checks if Int is valid
                storeSymbol(symbolTable,symbol,valueTable, value, symModTable, moduleNum);
            }

            tok = getToken();
            usecount = readInt(tok);
            //check usecount number <16, report error 

            for (int i=0;i<usecount;i++){                //read defcount line
                tok = getToken();
                symbol = readSym(tok);          //readSym checks tok valid, puts in symbol table?
                                                //readSym should also check if it's also been defined
                                                //if it's already been defined, use first value, warning message
            
            }

            tok = getToken();
            instrcount = readInt(tok);
            //check for total instr numbers

            for (int i=0;i<instrcount;i++){      //read defcount line
                tok = getToken();
                instrtype = readSym(tok);       //readSym checks tok valid, puts in symbol table?
                                                //readSym should also check if it's also been defined
                                                //if it's already been defined, use first value, warning message
                tok = getToken();
                address = readInt(tok);       //instruction Address
            }
            cout<< "Created Module " << moduleNum++ << "with Base Address " << baseAdd;
            baseAdd += instrcount; //check one off errors

            //Next Module
            while (tok!=NULL){      //read next module                        
                //Get Next Token
                tok = getToken();     //get the first token
                defcount = readInt(tok);                 //send tok to readInt to verify input
                // check defcount number <16, report error

                for (int i=0;i<defcount;i++){                //read defcount line
                    tok = getToken();
                    symbol = readSym(tok);          //readSym checks tok valid, puts in symbol table?
                                                    //readSym should also check if it's also been defined
                                                    //if it's already been defined, use first value, warning message
                    tok = getToken();
                    value  = readInt(tok);             //checks if Int is valid
                    storeSymbol(symbolTable,symbol,valueTable, value, symModTable, moduleNum);
                }

                tok = getToken();
                usecount = readInt(tok);
                //check usecount number <16, report error 

                for (int i=0;i<usecount;i++){                //read defcount line
                    tok = getToken();
                    symbol = readSym(tok);          //readSym checks tok valid, puts in symbol table?
                                                    //readSym should also check if it's also been defined
                                                    //if it's already been defined, use first value, warning message
                
                }

                tok = getToken();
                instrcount = readInt(tok);
                //check for total instr numbers

                for (int i=0;i<instrcount;i++){      //read defcount line
                    tok = getToken();
                    instrtype = readSym(tok);       //readSym checks tok valid, puts in symbol table?
                                                    //readSym should also check if it's also been defined
                                                    //if it's already been defined, use first value, warning message
                    tok = getToken();
                    address = readInt(tok);       //instruction Address


                }

            }
        cout << "File ended at line " << --linenumber << " offset " <<found << endl;
        }
    }
    inFile.close();


    //Done with file, should have symbol table and base addresses of modules
    // if file had syntax error, should display warnings or parse error
    // if parse error exists, do not do any more work on the file, pass parse error info to print file
    // some thing like, if parse error: printOutput has an overloaded version with parse errors
    // otherwise move to pass2 , give symbol table info to pass2 and printFile
    // pass 1 should return something which contains symbol table and base address info
    // pass1 could return a struct with a char array for symbol table, and an int array for base addresses, 
    //another for absolute addresses for defined symbols 
    // pass1 needs to print symbol table
    // pass2 could return a struct with a char array for symbol table, int array for instruction set

    // cout << "File was closed." << endl; 

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

char* getToken(char* buffer){

    return strtok(buffer, " \n \t");
}

char* getToken(){
    
    return strtok(NULL, " \n \t");
}
//parse error code


// int readInt(char* tok);
// so what does readInt do? it checks if its an int, if not, calls parse error
// Num expected error code


// char* readSym(char* tok);
//checks if symbol is valid, is correct length, symbol name is correct


// void storeSymbol(char* symbolTable[],char* symbol, int valueTable[],int value,int symModTable[],int moduleNum);
// void printSymbolTable(char* symbolTable[],int baseAdd[],int symModTable[]);
