#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

char* getToken();
char* getToken(char*& buffer,istream& inFile,string& line, int& linenumber);
void printFile();
void Pass1();
int readInt(char*& buffer,istream& inFile,string& line, int& linenumber);     //accepts token, returns int or prints error
char* readSym(char*& buffer,istream& inFile,string& line, int& linenumber);
char* readIAER(char*& buffer,istream& inFile,string& line, int& linenumber);
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
    const char* FileName = "input-1";     //This should be a full path later on
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
    //Parse variables
    char *tok;
    char *buffer;
    
    int linenumber = 1;
    size_t found;

    //Pass 1 variables
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
    

    bool parseError = false;    //if needed

    char* instrtype;
    getline(inFile,line);        //get first line in line
    buffer = strdup(line.c_str());      //put the line string to buffer char*


    if(inFile){             //File has been successfully opened
        // new module 
        while (!inFile.eof()) { //if File hasn't ended
                        
            //Get Next Token
            defcount = readInt(buffer, inFile, line, linenumber); //send tok to readInt to verify input
            // check defcount number <16, report error

            for (int i=0;i<defcount;i++){       //read defcount line
                symbol = readSym(buffer, inFile, line, linenumber);          //readSym checks tok valid, puts in symbol table?
                                                //readSym should also check if it's also been defined
                                                //if it's already been defined, use first value, warning message
                value  = readInt(buffer, inFile, line, linenumber);             //checks if Int is valid
                storeSymbol(symbolTable,symbol,valueTable, value, symModTable, moduleNum);
            }

            usecount = readInt(buffer, inFile, line, linenumber);
            //check usecount number <16, report error 

            for (int i=0;i<usecount;i++){        
                tok = getToken();
                symbol = readSym(buffer, inFile, line, linenumber);          //readSym checks tok valid, puts in symbol table?
                                                //readSym should also check if it's also been defined
                                                //if it's already been defined, use first value, warning message
            
            }

            instrcount = readInt(buffer, inFile, line, linenumber);
            //check for total instr numbers

            for (int i=0;i<instrcount;i++){     
                tok = getToken();
                instrtype = readIAER(buffer, inFile, line, linenumber);     
                                                //if it's already been defined, use first value, warning message
                tok = getToken();
                address = readInt(buffer, inFile, line, linenumber);       //instruction Address
            }
            cout<< "Created Module " << moduleNum++ << "with Base Address " << baseAdd;
            baseAdd += instrcount; //check one off errors

        
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
//gets first Token
char* getToken(char* buffer){
    // if bu
    char* tok;
    return strtok(buffer, " \n \t");
}



int readInt(istream& inFile,string& line,char*& buffer,int& linenumber){
    char* tok;
    int result;

    // put next token in tok
    if (strtok(NULL, " \n \t")==NULL){
            while (getline(inFile,line)&& tok==NULL){
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
            }
            if(tok==NULL){//end of file reached, print parseerror 1 }
            }
    }
    else {tok = strtok(NULL, " \n \t");}

    // check if tok is int

    if ( isdigit(*tok) ){
        result = atoi(tok);
        if (result >= 16){//parse error 5
            return 0;}
        else {return result;}
    }
    else{//NUM expected parse error
        return 0;}
    }

char* readSym(char*& buffer,istream& inFile,string& line, int& linenumber){
    char* tok;
    int result;

    // put next token in tok
    if (strtok(NULL, " \n \t")==NULL){
            while (getline(inFile,line)&& tok==NULL){
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
            }
            if(tok==NULL){//end of file reached, print parseerror 1 }
            }
    }
    else {tok = strtok(NULL, " \n \t");}

    // check if tok is int

    if ( isdigit(*tok) ){
        result = atoi(tok);
        if (result >= 16){//parse error 5
            return 0;}
        else {return tok;}
    }
    else{//NUM expected parse error
        return 0;}
    }

char* readIAER(char*& buffer,istream& inFile,string& line, int& linenumber){
    char* tok;
    int result;

    // put next token in tok
    if (strtok(NULL, " \n \t")==NULL){
            while (getline(inFile,line)&& tok==NULL){
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
            }
            if(tok==NULL){//end of file reached, print parseerror 1 }
            }
    }
    else {tok = strtok(NULL, " \n \t");}

    // check if tok is int

    if ( isdigit(*tok) ){
        result = atoi(tok);
        if (result >= 16){//parse error 5
            return 0;}
        else {return tok;}
    }
    else{//NUM expected parse error
        return 0;}
    }

// so what does readInt do? it checks if its an int, if not, calls parse error
// Num expected error code


// char* readSym(char* tok);
//checks if symbol is valid, is correct length, symbol name is correct


// void storeSymbol(char* symbolTable[],char* symbol, int valueTable[],int value,int symModTable[],int moduleNum);
// void printSymbolTable(char* symbolTable[],int baseAdd[],int symModTable[]);
