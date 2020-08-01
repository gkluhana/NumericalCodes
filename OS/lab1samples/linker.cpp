#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>

using namespace std;


//helper function used for debugging, not used in final submission
void printFile(char* filepath);

//Pass Functions
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
void lastoffset(string& line,int& offset);

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
    // // Print File for debugging, not required in submission, should be commented out
    // printFile(filepath);

    //Obtain file name from command line
    char* filepath = argv[1];

    // Initialize variables to be passed between passes
    
    int machineSize = 512;
    char* symbolTable[machineSize];     //stores symbols from Definition List
    int symModTable[machineSize];       //stores module number of ith symbol in Symbol Table
    int valueTable[machineSize];        //stores defined value if ith symbol in Symbol Table
    int baseAddTable[machineSize];      //stores base address of ith module

    //Run Parser Passes
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
    inFile.open(FileName);

    //File not opened for some reason    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
   
    // get Token Variables
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
    char* instrtype;

    //Initiate Parsing
    getline(inFile,line);               //get first line in line
    buffer = strdup(line.c_str());      //put the line string to buffer char*
    baseAddTable[0]=0;

    //Pass 1 loop
    while (!inFile.eof()) { 

        //DEFLIST

        //get Defcount
        if (moduleNum==1){defcount = readIntFirst(buffer, inFile, line, linenumber, offset);}
        else {defcount = readInt(buffer, inFile, line, linenumber, offset);} 
        
        //No more modules, do nothing, just break out of loop
        if(defcount==-1){
        break;  
        }

        //Too Many Definitions
        if (defcount >= 16){
        parseerror(4,linenumber,offset); 
        exit(1);}

        //Total Number of Definitions
        totaldef +=defcount;

        //Parse Deflist
        for (int i=0;i<defcount;i++){      
            symbol = readSym(buffer, inFile, line, linenumber, offset);  
            value  = readInt(buffer, inFile, line, linenumber, offset);      
            if(value==-1){
                parseerror(0,linenumber,offset);
                exit(1);
            }
         storeSymbol(symbolTable,symbol,valueTable, value, symModTable, moduleNum, machineSize);
        }


        //USELIST
        //get UseCount
        usecount = readInt(buffer, inFile, line, linenumber, offset);

        //Unable to find usecount
        if(usecount==-1){
            parseerror(0,linenumber,offset);
            exit(1);
        }

        //Too many uses
        if (usecount >= 16){
            parseerror(5,linenumber,offset); 
            exit(1);
        }

        // Parse Uselist

        for (int i=0;i<usecount;i++){        
            symbol = readSym(buffer, inFile, line, linenumber, offset);            
        }


        //INSTRLIST

        //get InstrCount
        instrcount = readInt(buffer, inFile, line, linenumber, offset);

        //Unable to find InstrCount
        if(instrcount==-1){
            parseerror(0,linenumber,offset);
            exit(1);
        }

        //Total number of Instructions so far
        totalinst += instrcount;
    
        //Too many instructions
        if(totalinst>machineSize){
            parseerror(6,linenumber,offset);
            exit(1);
        }
        
        //Address in definition exceeds size of module
        for(int i = totaldef - defcount; i<totaldef;i++){
            
            if(valueTable[i]>instrcount){
                printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n",moduleNum,symbolTable[i], valueTable[i],instrcount-1);
                valueTable[i]=0;
            }
        }
        
        //Parse Instruction List
        for (int i=0;i<instrcount;i++){     
            readIAER(buffer, inFile, line, linenumber, offset);     
            }
        //Increment Module Number
        moduleNum++;

        //Store base Address of Module
        baseAdd += instrcount; 
        baseAddTable[moduleNum]= baseAdd;

    }
    //Close File after completing pass 1.
    inFile.close();

    //Print Symbol Table to screen
    printSymbolTable(symbolTable,baseAddTable,symModTable,valueTable, machineSize);
}

//Prints File to help with debugging, disabled for submission
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
    if(inFile){
        while (getline(inFile,line)) {          
            cout << line << endl;               
        }
    }
    inFile.close();
    cout << endl;
}

//Function gets defcount for First Module
int readIntFirst(char* buffer,istream& inFile,string& line, int& linenumber, int& offset){
    char* tok;
    int result;
                    
    // put next token in tok
    // if fails, return with -1 indicating failure to Pass function
    string templine;
    tok = strtok(buffer, " \n \t");

    if (tok==NULL){
            while (tok==NULL){ 
                getline(inFile,templine);   
                linenumber++;
                buffer = strdup(templine.c_str());
                tok = strtok(buffer, " \n \t");
                if(tok==NULL && inFile.eof()){
                linenumber--;
                lastoffset(line,offset);
                return -1;
                }
                line = templine;
            }
            
    }
    offset = line.find(tok) +1;

    //If token is not an Int
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

//Reads all other Ints and checks for validity,
//Returns -1 to indicate no more tokens to Pass1
int readInt(char* buffer,istream& inFile,string& line, int& linenumber,int& offset){

    char* tok;
    int result;
    string templine;
    tok = strtok(NULL, " \n \t");

    if (tok==NULL){
            while (tok==NULL){ 
                getline(inFile,templine);   
                linenumber++;
                buffer = strdup(templine.c_str());
                tok = strtok(buffer, " \n \t");
                if(tok==NULL && inFile.eof()){
                linenumber--;
                lastoffset(line,offset);
                return -1;
                }
                line = templine;
            }
            
    }
    offset = line.find(tok) +1;

    //If token is not an Int

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
    string templine;
    
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ //unless you find a token
                getline(inFile,templine);   //get the next line
                linenumber++;
                buffer = strdup(templine.c_str());
                tok = strtok(buffer, " \n \t");
                if(tok==NULL && inFile.eof()){
                linenumber--;
                lastoffset(line,offset);
                parseerror(1,linenumber,offset);
                exit(1);
                }
                line = templine;
            }
            
    }
    offset = line.find(tok) +1;

    // Symbol Too large
    if(strlen(tok)>16){
        parseerror(3, linenumber, offset);
        exit(1);
    }

    //Symbol not valid [a-Z][a-Z0-9]*
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
    string templine;
   
    //get IAER token
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ 
                getline(inFile,templine);
                linenumber++;
                buffer = strdup(templine.c_str());
                tok = strtok(buffer, " \n \t");
                if(tok==NULL && inFile.eof()){
                linenumber--;
                lastoffset(line,offset);
                parseerror(2,linenumber,offset);
                exit(1);
                }
                line = templine;
            }

            
    }
    offset = line.find(tok) +1;

    //Is not IAER
    if(tok[0]=='I'||tok[0]=='A'||tok[0]=='E'||tok[0]=='R'){}
    else{
        lastoffset(line,offset);
        parseerror(2, linenumber, offset);
        exit(1);
    }

    //get address token

    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ 
                getline(inFile,templine);
                linenumber++;
                buffer = strdup(templine.c_str());
                tok = strtok(buffer, " \n \t");
                if(inFile.eof()){
                linenumber--;
                lastoffset(line,offset);
                parseerror(0,linenumber,offset);
                exit(1);
                }
                line = templine;
            }
            
    }

    //is not int
    for (int i=0; i<strlen(tok);i++){
        if (!isdigit(tok[i])){
            parseerror(2,linenumber,offset);
            exit(1);
        }
    }
}

//Stores Symbol, Symbol's Value, Symbol's Module Number in arrays
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
        //No more symbols
        if(symbolTable[i]==NULL){break;} 

        //check if index is a dupicate, if yes, skip
        for(int j = 0; j<= machineSize; j++){
            if(duplicateIndex[j]==0){ break;}   //no more indexes
            if(i==duplicateIndex[j]){isDuplicate=true;} //found i
        }

        //Print it if it's not a duplicate
        if(!isDuplicate){
        cout << symbolTable[i] << "=" << valueTable[i]+baseAdd[symModTable[i]]; //print symbols and value
        //reset Duplicate indicator
        isDuplicate = false;

        //Find other dupicates in symbolTable and put these indices in duplicateIndex array
        for(int j = i + 1; j<= machineSize;j++){ 
            if(symbolTable[j]==NULL){break;} 
            if(equalchars(symbolTable[j],symbolTable[i])){  
                isDuplicate = true; 
                for(int k = 0; k<= machineSize; k++){
                    if(duplicateIndex[k]==0){
                    duplicateIndex[k]=j;
                    break;
                    }
                }
            }

        //Duplicate encountered
        if(isDuplicate){cout << " Error: This variable is multiple times defined; first value used";}
        }
        //reset Duplicate Indicator
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

//When file runs out of tokens prematurely, lastoffset corrects offset value
void lastoffset(string& line,int& offset){
    char* tok;
    char* buffer;
    buffer = strdup(line.c_str());
    tok = strtok(buffer,"");
    if(tok){
    offset = strlen(tok)+1;}
    else{offset =1;}
}

//Second Pass
void Pass2(char* filepath,int& machineSize, char* symbolTable[], int symModTable[], int valueTable[], int baseAddTable[]){
    //Clean Duplicates in Symbol Table
    cleanduplicates(symbolTable,machineSize);
    
    cout << "Memory Map"<< endl;
    
    //Open File
    const char* FileName = filepath;     
    string line;
    ifstream inFile;
    inFile.open(FileName); 
    
    //File not opened for some reason
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

    //Pass 2 variables
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

    // Initiate First Pass
    getline(inFile,line);               //get first line in line
    buffer = strdup(line.c_str());      //put the line string to buffer char*

    //Global Use list initialized to NULL
    char* uselist[machineSize];
    for (int i = 0 ; i<machineSize; i ++){
            uselist[i]=NULL;
        }

    //Global list of referenced symbols initialized to NULL 
    char* used[machineSize];
        
    for (int i = 0 ; i<machineSize; i++){
        used[i]=NULL;
    }


    while (!inFile.eof()) {

        //DEFLIST

        //get Defcount
        if (moduleNum==1){defcount = readIntFirst2(buffer, inFile, line, linenumber, offset);}
        else {defcount = readInt2(buffer, inFile, line, linenumber, offset);}
        
        //Total Definitions
        totaldef +=defcount;

        //Parse Deflist
        for (int i=0;i<defcount;i++){ 
            symbol = readSym2(buffer, inFile, line, linenumber, offset);  
            value  = readInt2(buffer, inFile, line, linenumber, offset);  

        }


        //USELIST

        //get usecount
        usecount = readInt2(buffer, inFile, line, linenumber, offset);

        //Local Temporary list of symbols in uselist of current module
        char* curuselist[usecount];
        for (int i = 0 ; i<usecount; i++){
            curuselist[i]=NULL;
        }

        // Local temporary indicators for symbols used from current module
        // 0 : not used, 1: used
        int usebit[usecount];
        for (int i = 0 ; i<usecount; i++){
            usebit[i]=0;
        }

        //Parse Uselist
        for (int i=0;i<usecount;i++){        
            symbol = readSym2(buffer, inFile, line, linenumber, offset);  //readSym checks tok valid, puts in symbol table?
            storeSymbol2(uselist, curuselist, symbol, machineSize);
        }
 
        //INSTRLIST

        //get instrcount
        instrcount = readInt2(buffer, inFile, line, linenumber, offset);
      
        //total instructions
        totalinst += instrcount;
        
        //Parse InstList and Print Instructions
        for (int i=0;i<instrcount;i++){     
            
            //Print Instruction Number

            cout << std::setw(3) << std::setfill('0') << instnum << ": ";
            //Read and Print Instruction
            readIAER2(buffer, inFile, line, linenumber, offset, machineSize, 
                baseAddTable,moduleNum, symbolTable, curuselist, 
                usecount,valueTable,symModTable,usebit, used);     
                instnum++;
            }

        //Rule 7: Symbol listed in uselist but not referenced   
        for(int i=0;i<usecount; i++){
            if(usebit[i]==0){
                printf("Warning: Module %d: %s appeared in the uselist but was not actually used\n",moduleNum,curuselist[i]); 
            }
        }

        //Increment Module Number
        moduleNum++;

    }
    //Pass 2 Complete
    
    //Check if any symbol in symbol table is not in global list of used symbols
    //Rule 4: Symbol defined but not used
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

    //Close File
    inFile.close();
   
}

//Reads Defcount for First Module
int readIntFirst2(char* buffer,istream& inFile,string& line, int& linenumber, int&offset){
    char* tok;
    int result;
                    
    //get Token

    tok = strtok(buffer, " \n \t"); 
    if (tok==NULL){
            
            while (tok==NULL){ 
                getline(inFile,line);   
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

}

//Reads Ints 
int readInt2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset){

    char* tok;
    int result;
    tok = strtok(NULL, " \n \t");
    //get token
    if (tok==NULL){
            
            while (tok==NULL){
                getline(inFile,line);
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
    
    result = atoi (tok);

    return result;
}     

//Read Symbols
char* readSym2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset){

    char* tok;
    // get token
    tok = strtok(NULL, " \n \t");
    if (tok==NULL){
            while (tok==NULL){ 
                getline(inFile,line);
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
    return tok;
}

//Parses Instruction list and Prints Instructions
void readIAER2(char* buffer,istream& inFile,string& line, int& linenumber, int& offset, 
    int machineSize,int baseAddTable[], int moduleNum, char* symbolTable[],char* curuselist[],
    int usecount, int valueTable[], int symModTable[], int usebit[], char* used[]){

    char* tok;
    char* type;
    int errcode;

    // get token
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
    

    //Address String manipulation variables
    string address = tok;
    string operand;
    string opcode;
    
    //if Address is less than 4 digits
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
    
    //Resolve Relative Addresses
    if (*type =='R'){

        // Relative address exceeds size of Module
        if(atoi(operand.c_str())>(baseAddTable[moduleNum+1]-baseAddTable[moduleNum])){
            operand = "000";
            errcode = 4;
        }

        intoperand = (atoi(operand.c_str())+baseAddTable[moduleNum]);
        sprintf(temp, "%d", intoperand);
        operand = temp;
    }

    //Resolve External Address

    if(*type == 'E'){
        int index = atoi(operand.c_str());

        // External  address is greater than uselist
        if((index)>=usecount){ 
            errcode = 5;
        }

        else{

            symbol = curuselist[index]; //find symbol
            
            //Store symbol in used list 
            for (int i=0; i<machineSize;i++){
                if(!used[i]){
                    used[i]= symbol;
                    break;
                }
            }


            //Find symbol in symbol table
            usebit[index] = 1;
            for(int i = 0; i<machineSize; i++){
                
                //Symbol not defined
                if(symbolTable[i]==NULL){
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
    }


    // Rule 10: Illegal Immediate Value
    if(*type=='I'&& strlen(tok)>4){
        errcode = 1;
    }
    //Rule 11: Illegal Opcode
    else if(strlen(tok)>4){
        errcode = 2;
    }
    //Rule 8: Absolute address exceeds machine size
    else if(*type=='A'&& atoi(operand.c_str())>512){
        operand = "000";
        errcode = 3;
    }
    
    //Print Instruction (and Error)
    printInstr(errcode, opcode, operand,symbol);
}

//Prints Instructions or errors
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

//Stores symbols in global and temporary uselists
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

//Checks if two symbols are equal
bool equalchars(char* first, char* second){

    for (int k = 0; k<16;k++){
        if(!(first[k]==second[k])){return false;} //if some chars not equal. break
        
        if((k+1)>strlen(first)&&(k+1)>strlen(second)){return true;} //if 
    }
}

//Cleans duplicates from symbol Table
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