#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bitset>
#include <vector>
#include <list>
#include  <queue>
#include <bits/stdc++.h>

using namespace std;

vector<int> randvals;
int ofs=0;
int myrandom(int size);
unsigned long long cost;
unsigned long ctx_switches=0;
unsigned long inst_count = 0;



struct fileRead{
    string line;
    ifstream inFile;
    char *tok;
    char *buffer;
    int ofs;
    int count;
    char* filepath;
};

struct pte_t{
	unsigned int framenumber: 7;
	unsigned int present: 1;
	unsigned int referenced:1;
	unsigned int write_p:1;
	unsigned int modified:1;
	unsigned int paged_out:1;
	unsigned int created:1;
	unsigned int f_mapped:1;
	unsigned int info : 19;
};

struct frame_t{
	int fid;
	int pid;
	unsigned int vaddr: 6;
	unsigned int mapped:1;
};
vector<frame_t> frame_table;

struct vma_t{

	int s_page, e_page, write_p, f_mapped;
};
struct age_bit{
	unsigned int age:32;
};

struct pstats_t{
	unsigned long  unmaps:64;
	unsigned long  maps:64;
	unsigned long  ins:64;
	unsigned long  outs:64;
	unsigned long  fins:64;
	unsigned long  fouts:64;
	unsigned long  zeros:64;
	unsigned long  segv:64;
	unsigned long  segprot:64;
};

class Process{
	public:
	int vma_count;
	int pid;
	vector<pte_t> ptable;
	vector<age_bit> age_vector;
	list<vma_t> vmalist;
	pstats_t pstats;
};

class Pager{
	public:
	list<frame_t*> flist;
	virtual frame_t* select_victim_frame(vector<Process> &plist)=0;
};


//Pager Subclasses

//FIFO
class FIFO: public Pager{
	
	public:
	frame_t* select_victim_frame(vector<Process> &plist);
};
//FIFO function

frame_t* FIFO::select_victim_frame(vector<Process> &plist){
 	// cout << "SIZE" << flist.size() << endl;
 	
 	// cout << "FIFO selecting frame" << endl;
 	frame_t *tempframe = flist.front();
 	// cout << "SIZE" << flist.size() << endl;
 	flist.pop_front();
 	// cout << "SIZE" << flist.size() << endl;
	flist.push_back(tempframe);
	return tempframe;
}

//Second Chance
class SC: public Pager{
		
	public:
	frame_t* select_victim_frame(vector<Process> &plist);
};
//SC function
frame_t* SC::select_victim_frame(vector<Process> &plist){
 	// cout << "SIZE" << flist.size() << endl;
 	// cout << "back ref " << flist.front()->vaddr << endl; 
 	while(true){
 	if(!plist[flist.front()->pid].ptable[flist.front()->vaddr].referenced){
	 	// cout << "FIFO selecting frame" << endl;
	 	// cout << "Proc " << proc->pid << " page " << flist.front()->vaddr << " referenced = "
	 		// <<plist[flist.front()->pid].ptable[flist.front()->vaddr].referenced << endl;

	 	frame_t *tempframe = flist.front();
	 	
	 	// cout << "SIZE" << flist.size() << endl;
	 	flist.pop_front();
	 	// cout << "SIZE" << flist.size() << endl;
		flist.push_back(tempframe);
		return tempframe;}
	else{
		// cout << "Proc " << proc->pid << " page " << flist.front()->vaddr << " referenced = "
	 		// <<proc->ptable[flist.front()->vaddr].referenced << endl;
		plist[flist.front()->pid].ptable[flist.front()->vaddr].referenced=0;
		frame_t *tempframe = flist.front();
		// cout << "frame " << flist.front()->fid << " moved to back " <<endl;
		flist.pop_front();
		flist.push_back(tempframe);
	}
	}

}
//Clock
class CLOCK: public Pager{
	list<frame_t*>::iterator hand;
	bool middle;
	public:
	
	frame_t* select_victim_frame(vector<Process> &plist);
};
frame_t* CLOCK::select_victim_frame(vector<Process> &plist){
 	if(!middle){
 		hand = flist.begin();
 		middle=1;
 	}
 	while(true){
 	if(!plist[(*hand)->pid].ptable[(*hand)->vaddr].referenced){
	 	
	 	frame_t *tempframe = *hand;
	 	hand++;
	 	if(hand==flist.end()){
	 		hand=flist.begin();
	 	}

		return tempframe;}
	else{
		plist[(*hand)->pid].ptable[(*hand)->vaddr].referenced=0;
		hand++;
	 	if(hand==flist.end()){
	 		hand=flist.begin();
	 	}
	}
	}

}
//Aging
class AGING: public Pager{
	
	bool middle;
	public:
	
	frame_t* select_victim_frame(vector<Process> &plist);
};

frame_t* AGING::select_victim_frame(vector<Process> &plist){
 	Process *proc;
 	//Move age counter ref bits to the right and add ref bit
 	unsigned int ref = 2147483648;

 	// cout << "ref is " << ref;
 	for(int i =0; i<plist.size(); i++){
 		proc = &plist[i];
 		for (int j=0; j<proc->age_vector.size();j++){
 			proc->age_vector[j].age =proc->age_vector[j].age >> 1;

 			if(proc->ptable[j].referenced){
 				// cout << "REF BIT WAS ADDED in " <<j << endl;
 				proc->age_vector[j].age = proc->age_vector[j].age + ref;
 				proc->ptable[j].referenced=0;
 			}
 		}
 	}
 	frame_t* lowest_counter = flist.front(); 
 	unsigned int min =  plist[lowest_counter->pid].age_vector[lowest_counter->vaddr].age;
 	//find frame with lowest counter
 	
 	for (list<frame_t*>::iterator it=flist.begin(); it != flist.end(); ++it){
 		// cout << "age is " << plist[(*it)->pid].age_vector[(*it)->vaddr].age;
 		// cout << "min is " << min<<endl;
 		if(plist[(*it)->pid].age_vector[(*it)->vaddr].age < min){
 			lowest_counter = *it;
 			min = plist[lowest_counter->pid].age_vector[lowest_counter->vaddr].age;
 		
 		}
 	}

 	return lowest_counter;
}
//NRU
class NRU: public Pager{
	bool middle;
	int cycle;
	public:
	
	frame_t* select_victim_frame(vector<Process> &plist);
};

frame_t* NRU::select_victim_frame(vector<Process> &plist){
	//Make Classes
	if(!middle){cycle = 1; middle = true;}
	cout << "cycle is " << cycle << endl;
	
	//Reset Ref Bits if 10th Cycle
	

	vector< vector<frame_t*> > array;
	int classnum = 0;

    for (int i = 0; i < 4; i++) {
        array.push_back(vector<frame_t*>());
	}

	frame_t *frame;
	for (int i=0; i<frame_table.size();i++){
		if(plist[(frame_table[i]).pid].ptable[(frame_table[i]).vaddr].referenced &&
		plist[(frame_table[i]).pid].ptable[(frame_table[i]).vaddr].modified){
			classnum=3;
		}
		else if(plist[(frame_table[i]).pid].ptable[(frame_table[i]).vaddr].referenced &&
			!plist[(frame_table[i]).pid].ptable[(frame_table[i]).vaddr].modified){
			classnum=2;
		}
		else if (!plist[(frame_table[i]).pid].ptable[(frame_table[i]).vaddr].referenced &&
			plist[(frame_table[i]).pid].ptable[(frame_table[i]).vaddr].modified){
			classnum=1;
		}
		else{
			classnum=0;
		}
		frame = &frame_table[i];
		array[classnum].push_back(frame);
		cout << "frame " << frame->fid << " pushed to class " << classnum<< endl;
	}
	frame = NULL;
	if(!array[0].empty()){
		frame = array[0][myrandom(array[0].size())];
		cout << "Taken from class" << 0 <<endl;
	}
	else if(!array[1].empty()){
		frame = array[1][myrandom(array[1].size())];
		cout << "Taken from class" << 1 <<endl;
	}
	else if(!array[2].empty()){
		frame = array[2][myrandom(array[2].size())];
		cout << "Taken from class" << 2 <<endl;
	}
	else if(!array[3].empty()){
		frame = array[3][myrandom(array[3].size())];
		cout << "Taken from class" << 3 <<endl;
	}
	
	
	if(cycle==10){
		
		for (int i=0; i<frame_table.size();i++){
			plist[(frame_table[i]).pid].ptable[(frame_table[i]).vaddr].referenced=0;
		}
	} 
	cycle++;
	if(cycle > 10){
		cycle=1;
	}

	return frame;
	// 	array.push_back(vector<int>());
	// }

	//Start from 0, if empty, move ahead
	
	//From first non-empty class, pick array[class][randvals[ofs]%(num_pages_in_class)]

}

//Random
class RAND: public Pager{
	public:
	
	frame_t* select_victim_frame(vector<Process> &plist);
};

frame_t* RAND::select_victim_frame(vector<Process> &plist){
	int framenum = myrandom(frame_table.size());
	frame_t *frame = &frame_table[framenum];
	return frame;
}




//Function Declarations

void printFile(char* filepath);
void readFile(fileRead *inputfile, vector<Process> &plist);
void readFileRand(char* randfilepath);


void getTok(fileRead *inputfile);
void getTok2(fileRead *inputfile);

void initialize_ptable(vector<pte_t> &table);
void initialize_ftable(int num_frames);
frame_t *get_frame(int &freelist, Pager &pager, bool &freelist_empty, vector<Process> &plist, int num_frames);
void create_pte(Process *proc, int vpage);

void Simulation(fileRead *inputfile, vector<Process> &plist, char pager_type);
frame_t get_frame();

void printProcess(vector<Process> &plist);
void printPTable(vector<Process> &plist);
void printFTable();
void printCost();
int main(int argc, char** argv) {
    //Obtain file name from command line
    char* inputfilepath;
    char* randfilepath;

    int vflag = 0;
    char *avalue = NULL;
    char *fvalue = NULL;
    char *ovalue = NULL;
    int index;
    int filepath=0;
    int c;
    bool O;
    bool P;
    bool F;
    bool S;
    opterr = 0;

    //Extract Options from Input
    while ((c = getopt (argc, argv, "f:a:o:")) != -1)
	    switch (c)
	      {
	      
	      case 'f':
	        fvalue = optarg;
	        break;
	      case 'a':
	        avalue = optarg;
	        break;
	        case 'o':
	        ovalue = optarg;
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
           if (filepath==0){inputfilepath=argv[index]; filepath++; continue;}
           if (filepath==1){randfilepath=argv[index]; break;}
      }
      
    fileRead *inputfile = new fileRead;
    inputfile->inFile.open(inputfilepath);
    char pager_type =avalue[0];

    string numframes = fvalue;
    int num_frames = atoi(fvalue);

    string options = ovalue;
    cout << options << endl;
    for (int i=0; i<strlen(ovalue); i++){
    	if(options[i]=='F'){
    		F=true;
    	}
    	if(options[i]=='P'){
    		P=true;
    	}
    	if(options[i]=='O'){
    		O=true;
    	}
    	if(options[i]=='S'){
    		S=true;
    	}
    }
    cout << O << P << F << S << endl;
    // cout << "Pager Type is " << pager_type << " numframes are " <<num_frames <<endl;
    // Print File for debugging, not required in submission, should be commented out
    // printFile(inputfilepath);  

    // Store Random Numbers to Vector
    readFileRand(randfilepath);

    // Create Process List
    vector<Process> plist;

    readFile(inputfile, plist);

    // Pick Paging Algorithm


    // Create Global Frame Table
    num_frames = 32;
    initialize_ftable(num_frames);
    // Run Simulation & Print Output
    Simulation(inputfile, plist, pager_type);
    // Print Summary
    if(P){printPTable(plist);}
    if(F){printFTable();}
    
    if(S){
		printProcess(plist);
    	printCost();}
    //Testing plist
    // cout << plist.front().vmalist.front().e_page<<endl;
    // cout << plist.back().vmalist.back().e_page<<endl;
}


void printCost(){
	printf("TOTALCOST %lu %lu %llu\n", ctx_switches, inst_count, cost);
}


// Simulation Code
void Simulation(fileRead *inputfile, vector<Process> &plist, char pager_type){
	// cout << endl << "Simulation Started " <<endl;
	char *inst_type;
	int inst=0;
	Process *current_process;
	int vpage;
	
	pte_t *pte;
	int freelist = 0;
	bool freelist_empty=false;
	int num_frames = frame_table.size();
	// cout << "NUM is " << num_frames;
	

	Pager *pager;
	FIFO fifo;
	SC sc;
	CLOCK ck;
	AGING ag;
	NRU nru;
	RAND r;
	switch(pager_type){
		case 'f':
		pager = &fifo;
		// cout << "f";
		break;
		case 's':
		pager = &sc;
		break;
		case 'c':
		pager = &ck;
		break;
		case 'a':
		pager = &ag;
		break;
		case 'n':
		pager = &nru;
		break;
		case 'r':
		pager = &r;
		break;
		
	}

	while (getline(inputfile-> inFile,inputfile->line)){
	
		getTok2(inputfile);
		if (*inputfile->tok=='#'){continue;}
		if ((*inputfile->tok!='c')&&(*inputfile->tok!='r')&&(*inputfile->tok!='w'))
			{break;}

		
		inst_type = inputfile->tok;
		inputfile->tok = strtok(NULL," \n \t" );
		inst = atoi(inputfile->tok);
		cout << inst_count++ << " ==> ";
		cout << inst_type << " ";
		cout << inst << endl;
		vpage = inst;
		
		//Context Switch
		if(*inst_type=='c'){
			current_process = &plist[inst];
			ctx_switches++;
			cost = cost + 121;
			continue;
		}

		frame_t *newframe;
		pte = &current_process->ptable[vpage];

		if(!pte->created){
			create_pte(current_process, vpage);			
		}


		// Page Fault Exception, Allocate frame
		if (pte->created && !pte->present){
			// cout << "Page Fault" <<endl;
			newframe = get_frame(freelist, *pager, freelist_empty, plist, num_frames);
			
			//UNMAP 

			if(freelist_empty){
			// cout << "Old pid:vaddr " << newframe->pid << newframe->vaddr <<endl;
			// plist[newframe->pid].ptable[newframe->vaddr].present = 0;
			// plist[newframe->pid].ptable[newframe->vaddr].referenced = 0;
			// cout << " Proc " << newframe->pid << " vpage " << newframe->vaddr <<
					// " referenced = " <<plist[newframe->pid].ptable[newframe->vaddr].referenced <<endl; 
			if(plist[newframe->pid].ptable[newframe->vaddr].modified){
				// cout << newframe->vaddr;
				if(plist[newframe->pid].ptable[newframe->vaddr].f_mapped){
					cout << " FOUT" << endl;
					plist[newframe->pid].pstats.fouts++;
					cost = cost + 2500;
				}
				else{cout << " OUT " <<endl;
				plist[newframe->pid].pstats.outs++;
				cost = cost+3000;
				}
				plist[newframe->pid].ptable[newframe->vaddr].paged_out=1;
				plist[newframe->pid].ptable[newframe->vaddr].modified=0;
				// cout << " Proc " << newframe->pid << " vpage " << newframe->vaddr <<
					// " paged_out = " <<plist[newframe->pid].ptable[newframe->vaddr].paged_out <<endl; 
			}
			}

			if(	current_process->ptable[vpage].paged_out==0&&
				current_process -> ptable[vpage].f_mapped==0){
				cout << " ZERO" <<endl;
				current_process->pstats.zeros++;
				cost = cost + 150;
			}

			else if(current_process->ptable[vpage].f_mapped){
				cout << " FIN " << endl;
				current_process->pstats.fins++;
				cost = cost + 2500;
			}
			else if(current_process->ptable[vpage].paged_out){
				cout << " IN " <<endl;
				current_process->pstats.ins++;
				cost = cost + 3000;
			}
			
			//Map vpage

			cout << " MAP " << newframe->fid << endl;

			newframe->pid = current_process->pid;
			newframe->vaddr = vpage;
			plist[current_process->pid].pstats.maps++;

			current_process -> ptable[vpage].framenumber =newframe->fid;
			current_process -> ptable[vpage].present =1;
			current_process -> ptable[vpage].referenced=0;
			current_process -> ptable[vpage].modified=0;

			cost = cost + 400;
		}
		//Execute Instruction
			if(*inst_type=='w'){
				
				current_process -> ptable[vpage].referenced=1;

				if(current_process -> ptable[vpage].write_p){
					cout << " SEGPROT "<<endl;
					current_process->pstats.segprot++;
					cost = cost + 300;
				}
				else{
					current_process -> ptable[vpage].modified=1;
				}
				cost = cost + 1;
			}
			else if(*inst_type=='r'){

				current_process -> ptable[vpage].referenced=1;
				cost = cost +1;
			}	  	

	}
}

frame_t *get_frame(int &freelist, Pager &pager, bool &freelist_empty, vector<Process> &plist, int num_frames){
	cout << "NUM" << num_frames << endl; 
	if (freelist<num_frames){
		frame_table[freelist].fid = freelist;
		
		frame_t *tempframe = &frame_table[freelist];
		tempframe->mapped=1;
		pager.flist.push_back(tempframe);
		// cout << "here" << endl;
		freelist++;
		// cout << "SIZE" << pager.flist.size() <<endl;
		return tempframe;
	}
	// else {return NULL;}
	else {
		freelist_empty = true;
		// cout << "SIZE" << pager.flist.size() <<endl;
		// cout <<"reached here!-!" <<endl; 
		frame_t *tempframe = pager.select_victim_frame(plist);
		cout << " UNMAP " << tempframe->pid << ":" << tempframe->vaddr<<endl;
		plist[tempframe->pid].pstats.unmaps++;
		plist[tempframe->pid].ptable[tempframe->vaddr].present=0;
		cost = cost +400;
		return tempframe; }

	// frame_t *frame = allocate _frame_free_list();
	// if(frame == NULL) frame = THE_PAGER -> determine_victim_frame()
	// return frame;
}

void printProcess(vector<Process> &plist){
	for (int i =0;i<plist.size();i++){
	printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",i,plist[i].pstats.unmaps,
			plist[i].pstats.maps,plist[i].pstats.ins,plist[i].pstats.outs,
			plist[i].pstats.fins,plist[i].pstats.fouts,plist[i].pstats.zeros,plist[i].pstats.segv,
			plist[i].pstats.segprot);
	}
}
void printPTable(vector<Process> &plist){
	for (int p = 0;p<plist.size();p++){
	printf("PT[%d]: ",p);
		for(int i=0;i<64;i++){
			if(plist[p].ptable[i].present){
				cout<< " " << i << ":";
				if(plist[p].ptable[i].referenced){
					cout << "R";
				}
				else {cout << "-";}
				if(plist[p].ptable[i].referenced){
					cout << "M";
				}
				else {cout << "-";}
				if(plist[p].ptable[i].paged_out){
					cout << "S";
				}
				else {cout << "-";}
			}
			else if(!plist[p].ptable[i].created){
				cout << " *";
			}
			else{
				cout << " #" ;
			}
		}cout << endl;
	}
}
void printFTable(){
	cout << "FT: ";
	for (int i=0;i<frame_table.size();i++){
		if(!frame_table[i].mapped){cout << "* "; continue;}
		cout << frame_table[i].pid <<":"
		<< frame_table[i].vaddr <<" ";
	}
	cout <<endl;
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

void readFile(fileRead* inputfile, vector<Process> &plist){
	// cout << "Reading File" << endl;
	
    if (!inputfile->inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int num_proc;
    int vma_count;
    Process *proc;
    vma_t *vma;

    if(!inputfile->inFile.eof()){        
		// Get Num of Process
		getTok(inputfile);
	        // Skip lines with #
	    while (*(inputfile->tok)=='#'){
	    	getTok(inputfile);
	    }
	    num_proc = atoi(inputfile->tok);
	    // cout << "num_proc = "<<num_proc<<endl;
	    
	    // Read Processes
	   

	    for(int i =0;i < num_proc; i++){
	    	proc = new Process;
	    	proc->pid = i;
	    	// Get Vma Count 
	    	getTok(inputfile);
	        // Skip lines with #
		    while (*(inputfile->tok)=='#'){
	    		getTok(inputfile);
		    }
		    vma_count = atoi(inputfile->tok);
		    proc->vma_count = vma_count;
		    // cout << "vma_count = " <<proc->vma_count<< endl;

		    //Read VMAs
		    for (int j= 0; j<vma_count; j++){
		    	getTok(inputfile);
		    	
		    	vma = new vma_t;
		    	
		    	vma->s_page = atoi(inputfile->tok);
		    	vma->e_page = atoi(strtok(NULL, " \n \t"));
		    	vma->write_p = atoi(strtok(NULL, " \n \t"));
		    	vma->f_mapped = atoi(strtok(NULL, " \n \t"));


		    	// cout << inputfile->line <<endl;
		    	proc->vmalist.push_back(*vma);
		    	//Code to create vma and enter to list
		    }
		    initialize_ptable(proc->ptable);
		    //age-vector Initialize
		    proc->age_vector.resize(64);
		    age_bit *newbit;
		    for(int p=0;p<64;p++ ){
		    	proc->age_vector[p].age =0;
		    }
		    proc->pstats.maps=0;
		    proc->pstats.unmaps=0;
		    proc->pstats.ins=0;
		    proc->pstats.outs=0;
		    proc->pstats.fins=0;
		    proc->pstats.fouts=0;
		    proc->pstats.zeros=0;
		    proc->pstats.segv=0;
		    proc->pstats.segprot=0;

		    plist.push_back(*proc);
	    }
	}

}
void readFileRand(char* randfilepath) {
	// cout << "Readhing Random Num File" << endl;
	string line;
	ifstream inFile;
    inFile.open(randfilepath);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    char *tok;
    char *buffer;
    getline(inFile,line);
    int randval;

    if(!inFile.eof()){        
		while (getline(inFile,line)){
			
			buffer = strdup(line.c_str());
			tok = strtok(buffer, " \n \t");
			// cout << tok;
			randval = atoi(tok);
			randvals.push_back(randval);
		}
	}
	// cout << endl;
}

int myrandom(int size){
	int val = randvals[ofs] % size;
		// cout << "randval is " << randvals[ofs]<< "offset is " << ofs <<endl;

	ofs++;
	if(ofs==randvals.size()){
		ofs=0;
	}
	// cout << "random num is " << val<< "offset is " << ofs <<endl;
	// cout << "size was " << size <<endl;
	return val;
}

void getTok(fileRead *inputfile){
	getline(inputfile->inFile,inputfile->line);
	inputfile->buffer = strdup(inputfile->line.c_str());
	inputfile->tok = strtok(inputfile->buffer, " \n \t");
}

void getTok2(fileRead *inputfile){
	inputfile->buffer = strdup(inputfile->line.c_str());
	inputfile->tok = strtok(inputfile->buffer, " \n \t");
}

void create_pte(Process *proc, int vpage){
	//find in vmalist
	int start = 0;
	int end = 0;
  	for (list<vma_t>::iterator it=proc->vmalist.begin(); it != proc->vmalist.end(); ++it){
    start = it->s_page;
    end = it-> e_page;
		if(vpage>=start && vpage<=end){

			proc->ptable[vpage].write_p=it->write_p;
			proc->ptable[vpage].f_mapped=it->f_mapped;
			proc->ptable[vpage].created=1;
			return;
		}
	}

	cout << " SEGV " << endl;
	proc->pstats.segv++;
	cost = cost + 240;
	// modify bits

}

void initialize_ptable(vector<pte_t> &ptable){
	pte_t *pte;
	for (int i=0;i<64;i++){
		pte = new pte_t;
		pte -> framenumber=0;
		pte -> present=0;
		pte -> referenced=0;
		pte -> write_p=0;
		pte -> modified=0;
		pte -> paged_out=0;
		pte -> created=0;
		pte -> f_mapped=0;
		pte -> info =0;
		ptable.push_back(*pte);
	}
	ptable.resize(64);
}	

void initialize_ftable(int num_frames){
	frame_t *frame;
	for (int i=0;i<num_frames;i++){
		frame = new frame_t;
		frame->fid = i;
		// cout << "i is "<< i << endl;
		frame->vaddr= 0;
		frame->pid = 0;
		frame->mapped=0;
		frame_table.push_back(*frame);
	}
	// frame_table.resize(num_frames);
}