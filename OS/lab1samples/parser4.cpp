#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;

void printFile(char* filepath);

//Pass Functions, should also accept filename Path to open and close File
void Pass1(char* filepath, int& machineSize, char* symbolTable[], int symModTable[], int valueTable[], int baseAddTable[]);
void Pass2(char* filepath, int& machineSize, char* symbolTable[], int symModTable[], int valueTable[], int baseAddTable[]);

//Pass 1 Functions
int readIntFirst(char* buffer,istream& inFile,string& line, int& linenumber, int&offset);     //accepts token, returns int or prints error
int readInt(char* buffer,istream& inFile,string& line, int& linenumber, int& offset);     //accepts token, returns int or prints error
char* readSym(char* buffer,istream& inFile,string& line, int& linenumber, int& offset);
void readIAER(char* buffer,istream& inFile,string& line, int& linenumber, int& offset);
void storeSymbol(char* symbolTable[],char* symbol, int valueTable[],int value,int symModTable[],int moduleNum, int machineSize);
void printSymbolTable(char* symbolTable[],int baseAdd[],int symModTable[],int valueTable[],int machineSize);
void parseerror(int errcode, int linenumber, int offset);


//Pass 2 Functions
int readIntFirst2(char* buffer,istream& inFile,string& line, int& linenumber, int&offset);     //accepts token, returns int or prints error
int readInt2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset);     //accepts token, returns int or prints error
char* readSym2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset);
void readIAER2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset, 
    int machineSize,int baseAddTable[], int moduleNum, char* symbolTable[],char* curuselist[],
     int usecount, int valueTable[], int symModTable[], int usebit[], char* used[]);
void printInstr(int errcode = 0, string opcode = "", string operand="", char* sym = NULL);
void storeSymbol2(char* uselist[],char* curuselist[], char* symbol, int machineSize);
void cleanduplicates(char* symbolTable[],int machineSize);
bool equalchars(char* first, char* second);
int main(int argc, char** argv) {
    
    char* filepath = argv[1];
    // Initialize variables to be passed between passes
    // printFile(filepath);
    int machineSize = 512;
    char* symbolTable[machineSize];
    int symModTable[machineSize];
    int valueTable[machineSize];
    int baseAddTable[machineSize];

    Pass1(filepath, machineSize, symbolTable, symModTable, valueTable, baseAddTable);
    Pass2(filepath,machineSize, symbolTable, symModTable, valueTable, baseAddTable);
    return 0;
}

//First Pass
void Pass1(char* filepath, int& machineSize, char* symbolTable[], int symModTable[], int valueTable[], int baseAddTable[]){
    //Open File
    const char* FileName = filepath;    
    string line;
    ifstream inFile;
    inFile.open(FileName);    // 
    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
   
    char *tok;
    char *buffer;
    
    int linenumber = 1;
    int offset = 1;

    //Pass 1 variables
    int moduleNum = 1;
    int defcount = 0; //resets on each module
    int totaldef = 0; //does not reset
    int usecount = 0; //resets on each module
    int instrcount = 0; // resets on each module
    int totalinst = 0; //does not reset
    int baseAdd = 0; // increases accourding to instrcount
    int address = 0;
    
    char* symbol;
    int value;



    bool parseError = false;    //if needed

    char* instrtype;
    getline(inFile,line);        //get first line in line
    buffer = strdup(line.c_str());      //put the line string to buffer char*
    baseAddTable[0]=0;

    while (!inFile.eof()) { //if File hasn't ended

        //DEFLIST
        if (moduleNum==1){defcount = readIntFirst(buffer, inFile, line, linenumber, offset);}
        else {defcount = readInt(buffer, inFile, line, linenumber, offset);} //send tok to readInt to verify input
        
        if(defcount==-1){
        break;
        }

        if (defcount >= 16){
        parseerror(4,linenumber,offset); 
        exit(1);}

        totaldef +=defcount;
        for (int i=0;i<defcount;i++){       //read defcount line
            symbol = readSym(buffer, inFile, line, linenumber, offset);          //readSym checks tok valid, puts in symbol table?
                                            //readSym should also check if it's also been defined
                                            //if it's already been defined, use first value, warning message
            value  = readInt(buffer, inFile, line, linenumber, offset);             //checks if Int is valid
            if(value==-1){
                parseerror(0,linenumber,offset);
                exit(1);
            }
         storeSymbol(symbolTable,symbol,valueTable, value, symModTable, moduleNum, machineSize);
            
        }
        //USELIST
        usecount = readInt(buffer, inFile, line, linenumber, offset);
        if(usecount==-1){
            parseerror(0,linenumber,offset);
            exit(1);
        }
        if (usecount >= 16){
            parseerror(5,linenumber,offset); 
            exit(1);
        }

        for (int i=0;i<usecount;i++){        
            symbol = readSym(buffer, inFile, line, linenumber, offset);            
        }
        //INSTRLIST

        instrcount = readInt(buffer, inFile, line, linenumber, offset);

        if(instrcount==-1){
            parseerror(0,linenumber,offset);
            exit(1);
        }


        totalinst += instrcount;
  
        if(totalinst>machineSize){
            parseerror(6,linenumber,offset);
            exit(1);
        }
  
        //RULE 5 Warning Check
        for(int i = totaldef - defcount; i<totaldef;i++){
            
            if(valueTable[i]>instrcount){
                printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n",moduleNum,symbolTable[i], valueTable[i],instrcount-1);
                valueTable[i]=0;
            }
        }
        

        //check for total instr numbers
        for (int i=0;i<instrcount;i++){     
            readIAER(buffer, inFile, line, linenumber, offset);     
                                            //if it's already been defined, use first value, warning message
            }
        moduleNum++;
        baseAdd += instrcount; //check one off errors
        baseAddTable[moduleNum]= baseAdd;

    }
    inFile.close();



    // if file had syntax error, should display warnings or parse error
    // otherwise move to pass2 , give symbol table info to pass2 and printFile
    // pass 1 should return something which contains symbol table and base address info
    // pass2 could return a struct with a char array for symbol table, int array for instruction set
    // cout << "File was closed." << endl; 

    printSymbolTable(symbolTable,baseAddTable,symModTable,valueTable, machineSize);
}
//Prints Test File to help with debugging, disable for submission
void printFile(char* filepath){
    string line;
    ifstream inFile;
    inFile.open(filepath);
    
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

//gets first Module Defcount
int readIntFirst(char* buffer,istream& inFile,string& line, int& linenumber, int& offset){
    char* tok;
    int result;
                    
    // put next token in tok
    tok = strtok(buffer, " \n \t"); //there's no new token in current line
    if (tok==NULL){
            
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                return -1;
                }
            }
            
    }
    offset = line.find(tok) +1;
    // cout << "tok is " << tok[0] << tok[1] << endl;
    // // check if tok is int
    // cout << "tok lenght is " << strlen(tok) << endl; 
    for (int i=0; i<strlen(tok);i++){
        if (!isdigit(tok[i])){
            parseerror(0,linenumber,offset);
            exit(1);
            return 0;
        }
    }
    result = atoi (tok);
    
    return result;
}

//Reads Ints and checks for validity
int readInt(char* buffer,istream& inFile,string& line, int& linenumber,int& offset){
    char* tok;
    int result;
    // put next token in tok
    tok = strtok(NULL, " \n \t");
 
    if (tok==NULL){
            
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                return -1;
                }
            }
            
    }
    offset = line.find(tok) +1;
    // cout << "tok is " << tok[0] << tok[1] << endl;
    // // check if tok is int
    // cout << "tok lenght is " << strlen(tok) << endl; 
    for (int i=0; i<strlen(tok);i++){
        if (!isdigit(tok[i])){
            parseerror(0,linenumber,offset);
            exit(1);
        }
    }
    result = atoi (tok);

    return result;
}

//Reads Symbols and checks for validity
char* readSym(char* buffer,istream& inFile,string& line, int& linenumber, int& offset){
    char* tok;
    // put next token in tok
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                parseerror(1,linenumber,offset);
                exit(1);
                }
            }
            
    }
    offset = line.find(tok) +1;
    // cout << "tok is " << tok[0] << tok[1] << endl;
    // // check if tok is int
    // cout << "tok lenght is " << strlen(tok) << endl; 
    if(strlen(tok)>16){
        parseerror(3, linenumber, offset);
        exit(1);
    }


    if(!isalpha(tok[0])){
        parseerror(1,linenumber, offset);
        exit(1);
    }
    for (int i=1; i<strlen(tok);i++){
        if (!isalnum(tok[i])){
            parseerror(1,linenumber,offset);
            exit(1);
        }
    }
    
    return tok;
}

//Reads Addresses and checks for validity
void readIAER(char* buffer,istream& inFile,string& line, int& linenumber,int& offset){
    char* tok;
    // put next token in tok
    // get next token and offset, do not change offset 
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                parseerror(2,linenumber,offset);
                exit(1);
                }
            }

            
    }
    offset = line.find(tok) +1;
    //Is not IAER
    if(tok[0]=='I'||tok[0]=='A'||tok[0]=='E'||tok[0]=='R'){}
    else{
        parseerror(2, linenumber, offset);
        exit(1);
    }

    //get address token
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                parseerror(0,linenumber,offset);
                exit(1);
                }
            }
            
    }
    for (int i=0; i<strlen(tok);i++){
        if (!isdigit(tok[i])){
            parseerror(2,linenumber,offset);
            exit(1);
        }
    }
}
//stores Symbol in relevant arrays
void storeSymbol(char* symbolTable[],char* symbol, int valueTable[],int value,int symModTable[],int moduleNum, int machineSize){
    bool stored=false;
    for (int i = 0; i < machineSize; i++ ){
        if (symbolTable[i]==NULL){
            symbolTable[i] = symbol;
            valueTable[i] = value;
            symModTable[i] = moduleNum;
            stored = true;
            break;
        }
        if (stored){break;}
    }    
}
//Prints Symbol Table
void printSymbolTable(char* symbolTable[],int baseAdd[],int symModTable[],int valueTable[],int machineSize){
    cout << "Symbol Table" <<endl;
    int duplicateIndex[machineSize];
    bool isDuplicate = false;
    char* symbol1;
    char* symbol2;

    for (int i = 0 ; i<= machineSize; i++){
        if(symbolTable[i]==NULL){break;} // no more symbols

        //check if index is a dupicate, if yes, skip
        for(int j = 0; j<= machineSize; j++){
            if(duplicateIndex[j]==0){ break;}   //no more indexes
            if(i==duplicateIndex[j]){isDuplicate=true;} //found i
        }

        if(!isDuplicate){
        cout << symbolTable[i] << "=" << valueTable[i]+baseAdd[symModTable[i]]; //print symbols and value
        isDuplicate = false;

        //find other dupicates in symbolTable and put these indices in duplicateIndex
        for(int j = i + 1; j<= machineSize;j++){ //search after index i
            if(symbolTable[j]==NULL){break;} //if out of symbols, break
            if(equalchars(symbolTable[j],symbolTable[i])){ //if current symbol equal to some other symbol 
                isDuplicate = true;
                for(int k = 0; k<= machineSize; k++){
                    if(duplicateIndex[k]==0){
                    duplicateIndex[k]=j;
                    break;
                    }
                }
            }

        if(isDuplicate){cout << " Error: This variable is multiple times defined; first value used";}
        
        }
        isDuplicate = false;
        }
    cout << endl;
    }
    cout << endl;
}
//Prints Parse Error
void parseerror(int errcode, int linenumber, int offset){
    static const char* errstr[] = {
        "NUM_EXPECTED",
        "SYM_EXPECTED",
        "ADDR_EXPECTED",
        "SYM_TOO_LONG",
        "TOO_MANY_DEF_IN_MODULE",
        "TOO_MANY_USE_IN_MODULE",
        "TOO_MANY_INSTR",
    };
    printf("Parse Error line %d offset %d: %s\n",linenumber,offset,errstr[errcode]);
    }



void Pass2(char* filepath,int& machineSize, char* symbolTable[], int symModTable[], int valueTable[], int baseAddTable[]){
    cleanduplicates(symbolTable,machineSize);
    cout << "Memory Map"<< endl;
    
    //Open File
    
    const char* FileName = filepath;     //This should be a full path later on
    string line;
    ifstream inFile;
    inFile.open(FileName);    // 
    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    
    // Initializing Variables
    //Parse variables
    
    char *tok;
    char *buffer;
    int linenumber = 1;
    int offset = 1;

    //Pass 1 variables
    int moduleNum = 1;
    int defcount = 0; //resets on each module
    int totaldef = 0; //does not reset
    int usecount = 0; //resets on each module
    int instrcount = 0; // resets on each module
    int totalinst = 0; //does not reset
    int instnum = 0; // used for printing
    int baseAdd = 0; // increases accourding to instrcount
    int address = 0;
    char* symbol;
    int value;
    char* instrtype;
    getline(inFile,line);               //get first line in line
    buffer = strdup(line.c_str());      //put the line string to buffer char*

    char* uselist[machineSize];
    for (int i = 0 ; i<machineSize; i ++){
            uselist[i]=NULL;
        }
    char* used[machineSize];
        
        for (int i = 0 ; i<machineSize; i++){
            used[i]=NULL;
        }
    while (!inFile.eof()) { //if File hasn't ended

        //DEFLIST
        if (moduleNum==1){defcount = readIntFirst2(buffer, inFile, line, linenumber, offset);}
        else {defcount = readInt2(buffer, inFile, line, linenumber, offset);} //send tok to readInt to verify input
        
        totaldef +=defcount;
        for (int i=0;i<defcount;i++){       //read defcount line
            symbol = readSym2(buffer, inFile, line, linenumber, offset);  //readSym checks tok valid, puts in symbol table?
            value  = readInt2(buffer, inFile, line, linenumber, offset);  //checks if Int is valid

        }


        //USELIST
        usecount = readInt2(buffer, inFile, line, linenumber, offset);

        char* curuselist[usecount];
        for (int i = 0 ; i<usecount; i++){
            curuselist[i]=NULL;
        }
        int usebit[usecount];
        
        for (int i = 0 ; i<usecount; i++){
            usebit[i]=0;
        }

        for (int i=0;i<usecount;i++){        
            symbol = readSym2(buffer, inFile, line, linenumber, offset);  //readSym checks tok valid, puts in symbol table?
            storeSymbol2(uselist, curuselist, symbol, machineSize);
        }
 
        //INSTRLIST
        instrcount = readInt2(buffer, inFile, line, linenumber, offset);
      
        totalinst += instrcount;
        


        //Print Instruction
        for (int i=0;i<instrcount;i++){     
            //Print Instruction Number

            cout << std::setw(3) << std::setfill('0') << instnum << ": ";
            //Read and Print Instruction
            readIAER2(buffer, inFile, line, linenumber, offset, machineSize, 
                baseAddTable,moduleNum, symbolTable, curuselist, 
                usecount,valueTable,symModTable,usebit, used);     
                instnum++;
            }
        //Rule 7
        for(int i=0;i<usecount; i++){
            if(usebit[i]==0){
                printf("Warning: Module %d: %s appeared in the uselist but was not actually used\n",moduleNum,curuselist[i]); 
            }
        }


        moduleNum++;

    }
    bool notused = true;
    for(int i=0; i< machineSize; i++){
        if(symbolTable[i]==NULL){break;}
        for (int j=0; j<totalinst;j++){
            if(used[j]==NULL){break;}      
            else if(equalchars(symbolTable[i],used[j])){notused=false;break;}
        }
        if(notused){printf("Warning: Module %d: %s was defined but never used\n",symModTable[i],symbolTable[i]);}
        notused=true;
    }


    inFile.close();
   
}
int readIntFirst2(char* buffer,istream& inFile,string& line, int& linenumber, int&offset){
    char* tok;
    int result;
                    
    // put next token in tok
    tok = strtok(buffer, " \n \t"); //there's no new token in current line
    if (tok==NULL){
            
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                return -1;
                }
            }
            
    }
    offset = line.find(tok) +1;
    for (int i=0; i<strlen(tok);i++){
        if (!isdigit(tok[i])){
            exit(1);
            return 0;
        }
    }
    result = atoi (tok);
    
    return result;

}     //accepts token, returns int or prints error
int readInt2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset){

    char* tok;
    int result;
    // put next token in tok
    tok = strtok(NULL, " \n \t");
 
    if (tok==NULL){
            
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                return -1;
                }
            }
            
    }
    offset = line.find(tok) +1;
    for (int i=0; i<strlen(tok);i++){
        if (!isdigit(tok[i])){
            exit(1);
        }
    }
    result = atoi (tok);

    return result;

}     //accepts token, returns int or prints error
char* readSym2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset){

    char* tok;
    // put next token in tok
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                exit(1);
                }
            }
            
    }
    offset = line.find(tok) +1;
    if(strlen(tok)>16){
        exit(1);
    }


    if(!isalpha(tok[0])){
        exit(1);
    }

    for (int i=1; i<strlen(tok);i++){
        if (!isalnum(tok[i])){
            exit(1);
        }
    }
    
    return tok;
}
void readIAER2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset, 
    int machineSize,int baseAddTable[], int moduleNum, char* symbolTable[],char* curuselist[],
     int usecount, int valueTable[], int symModTable[], int usebit[], char* used[]){

    char* tok;
    char* type;
    int errcode;

    // put next token in tok
    // get next token and offset, do not change offset 
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                exit(1);
                }
            }

            
    }
    offset = line.find(tok) +1;
    

    //Is not IAER
    if(tok[0]=='I'||tok[0]=='A'||tok[0]=='E'||tok[0]=='R'){}
    else{
        exit(1);
    }

    //get address token
    type = tok;
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            
            while (tok==NULL){ //unless you find a token
                getline(inFile,line);   //get the next line
                linenumber++;
                buffer = strdup(line.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                exit(1);
                }
            }
            
    }
    for (int i=0; i<strlen(tok);i++){
        if (!isdigit(tok[i])){
            exit(1);
        }
    }

    //String manipulation variables
    string address = tok;
    string operand;
    string opcode;
    if(strlen(tok)<4){
        operand = address;
        opcode = "";
    }
    else{
        operand = address.substr(1);
        opcode = address.substr(0,1);}
    int intoperand;
    char temp[3];
    char* symbol;
    // cout << "tok is " << address.substr(1) << endl;
    
    //Resolve Relative Addresses
    if (*type =='R'){

        if(atoi(operand.c_str())>(baseAddTable[moduleNum+1]-baseAddTable[moduleNum])){
            operand = "000";
            errcode = 4;
        }
    //     int intoperand;
        intoperand = (atoi(operand.c_str())+baseAddTable[moduleNum]);
    //     char* temp;
        sprintf(temp, "%d", intoperand);
        operand = temp;
    }

    //Resolve External Address
    if(*type == 'E'){
        int index = atoi(operand.c_str());
        if((index)>=usecount){ // if address is greater than uselist

            errcode = 5;
        }
        else{
            symbol = curuselist[index]; //find symbol

            //store symbol in used list 
            for (int i=0; i<machineSize;i++){
                if(!used[i]){
                    used[i]= symbol;
                    // cout << "symbol " << symbol << "stored in position" << i << endl;
                    break;
                }
            }


            //try to find symbol in symbol table
            usebit[index] = 1;
            for(int i = 0; i<machineSize; i++){
                if(symbolTable[i]==NULL){      //out of Symbols  
                    errcode = 6;
                    
                    break;
                }
                else if(equalchars(symbolTable[i],symbol)){ //if you found the symbol in symbol talbe
                    intoperand = valueTable[i]+baseAddTable[symModTable[i]];
                    sprintf(temp, "%d", intoperand);
                    operand = temp;
                    break; 
                }
            }

        }
        // External address resolution here
    }


    // Rule 10 Error
    if(*type=='I'&& strlen(tok)>4){
        errcode = 1;
    }
    //Rule 11 Error
    else if(strlen(tok)>4){
        errcode = 2;
    }
    else if(*type=='A'&& atoi(operand.c_str())>512){
        operand = "000";
        errcode = 3;
    }
    //Rule 8 Error
    
    // else if (*type=='I'&& strlen(tok)>4)

    printInstr(errcode, opcode, operand,symbol);
}


void printInstr(int errcode, string opcode, string operand, char* sym){
    // errcodes: 1-6
  switch (errcode){
        case 1:
        cout << "9999";
        cout << " Error: Illegal immediate value; treated as 9999";
        cout << endl;
        break;
        case 2:
        cout << "9999";
        cout << " Error: Illegal opcode; treated as 9999";
        cout << endl;
        break;
        case 3:
        cout << opcode << operand;
        cout << " Error: Absolute address exceeds machine size; zero used";
        cout << endl;
        break;
        case 4:
        cout << opcode <<std::setw(3) << std::setfill('0') << operand;
        cout << " Error: Relative address exceeds module size; zero used";
        cout << endl;
        break;
        case 5:
        cout << opcode <<std::setw(3) << std::setfill('0') << operand;
        cout << " Error: External address exceeds length of uselist; treated as immediate";
        cout << endl;
        break;
        case 6:
        cout << opcode <<std::setw(3) << std::setfill('0') << operand;
        printf(" Error: %s is not defined; zero used",sym);
        cout << endl;
        break;

        default:
        if(opcode == ""){
            cout << std::setw(4) << std::setfill('0') << operand << endl;
        }
        else{
        cout << opcode <<std::setw(3) << std::setfill('0') << operand << endl;}
    }
}

void storeSymbol2(char* uselist[],char* curuselist[], char* symbol, int machineSize){

    //Store in Current Use List
    for (int i=0; i < 16; i++){
        if(curuselist[i]==NULL){
            curuselist[i]=symbol;
            break;

        }
    }

    //Store in Global Use List
        for (int i = 0; i < machineSize; i++ ){
            if (uselist[i]==NULL){
                uselist[i] = symbol;
                break;
            }
        }
}    

bool equalchars(char* first, char* second){

    for (int k = 0; k<16;k++){
        if(!(first[k]==second[k])){return false;} //if some chars not equal. break
        
        if((k+1)>strlen(first)&&(k+1)>strlen(second)){return true;} //if 
    }
}

void cleanduplicates(char* symbolTable[],int machineSize){
    char* symbolTable2[machineSize];
    bool stored = false;
    for (int i=0;i<machineSize; i++){
        if(symbolTable[i]==NULL){break;}    //out of symbols in original symboltable
        for (int j=0;j<machineSize;j++){ //search for symbol in symboltable2
            if(j==0){symbolTable2[j]=symbolTable[i];break;}
            else if(equalchars(symbolTable[i],symbolTable2[j])){stored=true;break;}//if found, stored
            if(symbolTable2[j+1]==NULL){symbolTable2[j]=symbolTable[i];}
        }

    }
    symbolTable = symbolTable2;
}