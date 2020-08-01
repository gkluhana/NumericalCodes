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
 // int max_track = 0;
 int queue_index=0; 
 int total_time=0;
 int total_movement=0;
 double total_turnaround=0;
 double total_wait_time=0;
 double avg_wait_time=0;
 double avg_turnaround=0;
 int max_wait_time=0;
 int track=0;
 bool direction = true; // false = -ve, true = +ve
 double requests=0;


//Global Variables



//Structure Declarations

struct io_t{
	int id;
	int time_issued;
	int track_accessed;
	double wait_time;
	int start_time;
	int end_time;
	double turnaround;
};

vector<io_t*> iorequests;
//Scheduler Class
class Scheduler{

	public:
	vector <io_t*> ioqueue;
	vector <io_t*> ioqueue_inactive;
	virtual io_t* strategy()=0;
	virtual void insert(io_t* io)=0;
};


//Scheduler Sub Classes

//FIFO
class FIFO: public Scheduler{
	public:
	io_t* strategy();
	void insert(io_t* io);
};
//FIFO function

io_t* FIFO::strategy(){
 	if(ioqueue.empty()){
 		return NULL;}
 	io_t *tempio = ioqueue.back();
 	ioqueue.pop_back();
	return tempio;
}
void FIFO::insert(io_t* io){
	ioqueue.insert(ioqueue.begin(),io);
}

//SSTF
class SSTF: public Scheduler{
	public:
	io_t* strategy();
	void insert(io_t* io);
};
//SSTF function
io_t* SSTF::strategy(){
	// cout << "Scheduling" << endl;
	if(ioqueue.empty()){
		return NULL;}
	io_t *closest = ioqueue[0]; 
	int distance = 10000000;
	int tempdistance =0;
	int index =0; 
	int remove_index=0;
	//loop over iorequests
	for (vector<io_t*>::iterator it=ioqueue.begin(); it!=ioqueue.end();++it){
		tempdistance = (*it)->track_accessed - track;
		if(tempdistance<0) {tempdistance = tempdistance * (-1);}
		if(tempdistance==distance){
			if(closest->id < (*it)->id){}
			else{closest = *it; remove_index=index;}
		}
		else if(tempdistance<distance){distance=tempdistance; closest = *it; remove_index=index;}
		index++;
	}
	ioqueue.erase(ioqueue.begin()+remove_index);
	
	return closest;
}
void SSTF::insert(io_t* io){
	ioqueue.insert(ioqueue.begin(),io);
}
//LOOK
class LOOK: public Scheduler{
	public:
	io_t* strategy();
	void insert(io_t* io);
};
//LOOK Function
io_t* LOOK::strategy(){
	// cout << "Look Scheduling" << endl;
	if(ioqueue.empty()){
		return NULL;}
	io_t *closest1 = ioqueue[0];
	io_t *closest2 = ioqueue[0];
	int distance = 10000000;
	int tempdistance =0;
	int mindistance1 =100000000;
	int mindistance2 =100000000;
	int index =0; 
	int remove_index1=0;
	int remove_index2=0;
	bool request_direction; 
	bool same_direction=false;
	//Check if io's in current direction
	for (vector<io_t*>::iterator it=ioqueue.begin(); it!=ioqueue.end();++it){
		//	

		// cout << "considering request " << (*it)-> id << endl;

		//Compute Current Request Distance
		tempdistance = (*it)->track_accessed - track;
		// cout << "temp distance " << tempdistance <<endl;
		//Find current request direction
		if(tempdistance<0){request_direction=false;}
		else if(tempdistance>0){request_direction=true;}
		// cout << "req direction " << request_direction <<endl;
		// cout << "direction" << direction <<endl;
		//Make tempdistance positive
		if(tempdistance<0) {tempdistance = tempdistance * (-1);}
		
		//If Same as current direction, compare with min distance 1
		if(tempdistance==0){
			mindistance1=0;
			if(closest1->id < (*it)->id){}
			else{closest1 = *it; remove_index1=index;}
		}

		if(request_direction==direction){
			// cout << "same " <<endl;
			same_direction = true;
			if(tempdistance==mindistance1){
				if(closest1->id < (*it)->id){}
				else{closest1 = *it; remove_index1=index;}
			}
			else if(tempdistance<mindistance1){mindistance1=tempdistance; closest1 = *it; remove_index1=index;}
		}
		//If different direcctions, compare with min distance 2
		else{
				if(tempdistance==mindistance2){
				if(closest2->id < (*it)->id){}
				else{closest2 = *it; remove_index2=index;}
			}
			else if(tempdistance<mindistance2){mindistance2=tempdistance; closest2 = *it; remove_index2=index;}
		}
		index++;
	}
	// cout << same_direction <<endl;
	if(same_direction){
			// cout <<closest1->id<<" issue " << closest1->track_accessed <<" " << track << endl; 
			// cout << "remove index " << remove_index1<< endl;
			ioqueue.erase(ioqueue.begin()+remove_index1);
			return closest1;

		}
		else{
			// cout << " NEGATIVE IO " << closest2->id << " At Track " << closest2->track_accessed << " head at " << track << endl; 

			ioqueue.erase(ioqueue.begin()+remove_index2);
			return closest2;
	}
}
void LOOK::insert(io_t* io){
	ioqueue.insert(ioqueue.begin(),io);
}

//CLOOK
class CLOOK: public Scheduler{
	public:
	io_t* strategy();
	void insert(io_t* io);
};
//CLOOK Function
io_t* CLOOK::strategy(){
	// cout << "Look Scheduling" << endl;
	if(ioqueue.empty()){
		return NULL;}
	direction = true;
	io_t *closest1 = ioqueue[0];
	io_t *closest2 = ioqueue[0];
	int distance = 10000000;
	int tempdistance1 =0;
	int tempdistance2 =0;
	int mindistance1 =100000000;
	int mindistance2 =100000000;
	int index =0; 
	int remove_index1=0;
	int remove_index2=0;
	bool request_direction; 
	bool same_direction=false;
	//Check if io's in current direction
	for (vector<io_t*>::iterator it=ioqueue.begin(); it!=ioqueue.end();++it){
		//	

		// cout << "considering request " << (*it)-> id << endl;

		//Compute Current Request Distance
		tempdistance1 = (*it)->track_accessed - track;
		tempdistance2 = (*it)->track_accessed;
		// cout << "temp distance " << tempdistance <<endl;
		//Find current request direction
		if(tempdistance1<0){request_direction=false;}
		else if(tempdistance1>0){request_direction=true;}
		// cout << "req direction " << request_direction <<endl;
		// cout << "direction" << direction <<endl;
		//Make tempdistance positive
		if(tempdistance1<0) {tempdistance1 = tempdistance1 * (-1);}
		
		//If Same as current direction, compare with min distance 1
		if(tempdistance1==0){
			mindistance1=0;
			if(closest1->id < (*it)->id){}
			else{closest1 = *it; remove_index1=index;}
		}

		if(request_direction==direction){
			// cout << "same " <<endl;
			same_direction = true;
			if(tempdistance1==mindistance1){
				if(closest1->id < (*it)->id){}
				else{closest1 = *it; remove_index1=index;}
			}
			else if(tempdistance1<mindistance1){mindistance1=tempdistance1; closest1 = *it; remove_index1=index;}
		}
		//If different direcctions, compare with min distance 2
		else{
				if(tempdistance2==mindistance2){
				if(closest2->id < (*it)->id){}
				else{closest2 = *it; remove_index2=index;}
			}
			else if(tempdistance2<mindistance2){mindistance2=tempdistance2; closest2 = *it; remove_index2=index;}
		}
		index++;
	}
	// cout << same_direction <<endl;
	if(same_direction){
			// cout <<closest1->id<<" issue " << closest1->track_accessed <<" " << track << endl; 
			// cout << "remove index " << remove_index1<< endl;
			ioqueue.erase(ioqueue.begin()+remove_index1);
			return closest1;

		}
		else{
			// cout <<closest2->id<<" issue " << closest2->track_accessed <<" " << track << endl; 
			ioqueue.erase(ioqueue.begin()+remove_index2);
			return closest2;
	}
}
void CLOOK::insert(io_t* io){
	ioqueue.insert(ioqueue.begin(),io);
}

//FLOOK
class FLOOK: public Scheduler{
	public:
	io_t* strategy();
	void insert(io_t* io);
};
//LOOK Function
io_t* FLOOK::strategy(){
	vector <io_t*> ioqueue_temp;
	// cout << "Look Scheduling" << endl;
	
	//If queue is empty, switch queues
	if(ioqueue.empty()){
		ioqueue_temp = ioqueue;
		ioqueue = ioqueue_inactive;
		ioqueue_inactive = ioqueue_temp;
		}

	//if queue is still empty
		if(ioqueue.empty()){
		return NULL;}


	io_t *closest1 = ioqueue[0];
	io_t *closest2 = ioqueue[0];
	int distance = 10000000;
	int tempdistance =0;
	int mindistance1 =100000000;
	int mindistance2 =100000000;
	int index =0; 
	int remove_index1=0;
	int remove_index2=0;
	bool request_direction; 
	bool same_direction=false;
	//Check if io's in current direction
	for (vector<io_t*>::iterator it=ioqueue.begin(); it!=ioqueue.end();++it){
		//	

		// cout << "considering request " << (*it)-> id << endl;

		//Compute Current Request Distance
		tempdistance = (*it)->track_accessed - track;
		// cout << "temp distance " << tempdistance <<endl;
		//Find current request direction
		if(tempdistance<0){request_direction=false;}
		else if(tempdistance>0){request_direction=true;}
		// cout << "req direction " << request_direction <<endl;
		// cout << "direction" << direction <<endl;
		//Make tempdistance positive
		if(tempdistance<0) {tempdistance = tempdistance * (-1);}
		
		//If Same as current direction, compare with min distance 1
		if(tempdistance==0){
			mindistance1=0;
			if(closest1->id < (*it)->id){}
			else{closest1 = *it; remove_index1=index;}
		}

		if(request_direction==direction){
			// cout << "same " <<endl;
			same_direction = true;
			if(tempdistance==mindistance1){
				if(closest1->id < (*it)->id){}
				else{closest1 = *it; remove_index1=index;}
			}
			else if(tempdistance<mindistance1){mindistance1=tempdistance; closest1 = *it; remove_index1=index;}
		}
		//If different direcctions, compare with min distance 2
		else{
				if(tempdistance==mindistance2){
				if(closest2->id < (*it)->id){}
				else{closest2 = *it; remove_index2=index;}
			}
			else if(tempdistance<mindistance2){mindistance2=tempdistance; closest2 = *it; remove_index2=index;}
		}
		index++;
	}
	// cout << same_direction <<endl;
	if(same_direction){
			// cout <<closest1->id<<" issue " << closest1->track_accessed <<" " << track << endl; 
			// cout << "remove index " << remove_index1<< endl;
			ioqueue.erase(ioqueue.begin()+remove_index1);
			return closest1;

		}
		else{
			// cout <<closest2->id<<" issue " << closest2->track_accessed <<" " << track << endl;

			ioqueue.erase(ioqueue.begin()+remove_index2);
			return closest2;
	}
}
void FLOOK::insert(io_t* io){
	ioqueue_inactive.insert(ioqueue_inactive.begin(),io);
}

//Function Declarations
void printFile(char* filepath);
void readFile(char *filepath);
void simulate(char sched_type);
void printSummary();
void printOutput();
//Main Function
int main(int argc, char** argv) {
	char* inputfilepath;
    char *svalue = NULL;
    opterr = 0;
    int filepath =0;
    int c;
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
	    

      inputfilepath=argv[optind];
      // printFile(inputfilepath);
      

      char sched_type =svalue[0];
      readFile(inputfilepath);

      simulate(sched_type);

      printOutput();
      printSummary();

}
//Simulation
void simulate(char sched_type){
	// cout << "Simulation Started" <<endl;
	int curr_time=1;

	int movement=0;
	int tot_wait_time=0;
	
	int index_request=0;
	
	bool active = false;
	
	bool running = true;
	bool completed = false;
	bool pending = false;
	io_t *current_io; 

	Scheduler *scheduler;
	FIFO fifo;
	SSTF sstf;
	LOOK look;
	CLOOK clook;
	FLOOK flook;
	switch(sched_type){
		case 'i':
		scheduler = &fifo;
		break;
		case 'j':
		scheduler = &sstf;
		break;
		case 's':
		scheduler = &look;
		break;
		case 'c':
		scheduler = &clook;
		break;
		case 'f':
		scheduler = &flook;
		break;
	}

	while(running){
		//Add io to queue if arrived

		// cout << curr_time << endl;
		// cout  << curr_time << ": "<<endl;
		
		if (iorequests[index_request]==NULL){}
		else if(iorequests[index_request]->time_issued==curr_time){
			// cout << curr_time << ": "<<iorequests[index_request]->id<< " Add "<<iorequests[index_request]->track_accessed  << endl;
	
			scheduler->insert(iorequests[index_request]);
			// cout << "error her" << endl;
			index_request++;
		}
	
		
		if(current_io){
			if(current_io->track_accessed == track){
				completed = true;
			}
		}
		
		//Active and completed IO
		if(active && completed){
			// cout << "Active and Completed!" <<endl;
			

			current_io->end_time = curr_time;

			current_io->turnaround =  current_io->end_time - current_io->time_issued;
			// cout << curr_time << ": "<<current_io->id << "finish " << current_io->turnaround <<endl;
			completed = true;

			current_io = NULL;
			active = false;
		}
		else if(active && !completed){
			// cout << "Active and NOT Completed!" <<endl;
			
			if(direction){
			track++;
			movement++;
			}
			else{
			track--;
			movement++;
			}

		}
		if (!active){

			// cout << "Not Active" <<endl;
			
			current_io = scheduler->strategy();
			if(!current_io){
				// cout << "Simulation ended" << endl;
				// cout << "here!" << endl;
	
				if(iorequests.size() > index_request){

					curr_time = iorequests[index_request]->time_issued;

					continue;}
				break;
			}
	
			current_io->start_time = curr_time;
			
			current_io-> wait_time = curr_time - current_io->time_issued;
			if(max_wait_time <current_io-> wait_time){
				max_wait_time = current_io-> wait_time;
			}

			completed = false;
			active = true;

			if(current_io->track_accessed < track){direction =false;}
			else if(current_io->track_accessed > track) {direction = true;} 
			else{continue;}

			if(direction){
			track++;
			movement++;
			}
			else{
			track--;
			movement++;
			}
		}
		// cout << track << endl;
		// cout << current_io->track_accessed << endl;
		curr_time++;

	}
	total_time = curr_time;
	total_movement = movement;

}
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

void readFile(char *filepath){
	// cout << "Reading File" << endl;
	ifstream inFile;
	inFile.open(filepath);

    
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    string line;
    char *tok;
    char *buffer;
    getline(inFile,line);
    int index = 0;
    io_t *io;
    if(!inFile.eof()){        
		while (getline(inFile,line)){
			
			buffer = strdup(line.c_str());
			tok = strtok(buffer, " \n \t");
			if(*tok=='#'){continue;}
			//
			io = new io_t;
			iorequests.push_back(io);
			iorequests[index]->id=index;
			iorequests[index]->time_issued = atoi(tok);
			iorequests[index]->track_accessed = atoi(strtok(NULL, " \n \t"));
			// if(iorequests[index]->track_accessed > max_track){max_track=iorequests[index]->track_accessed;}
			index++;
			requests++;

			// cout << iorequests[index - 1]->id <<endl;
			// cout << iorequests[index - 1]->time_issued;
			// cout << " " << iorequests[index - 1]->track_accessed <<endl;
			// cout << max_track << endl;
			}
			iorequests.resize(index);
			
	}
	// cout << endl;

}
void printOutput(){
	for (int i=0; i<iorequests.size();i++){
	
		printf("%5d: %5d %5d %5d\n", iorequests[i]->id, iorequests[i]->time_issued,
					 iorequests[i]->start_time, iorequests[i]->end_time);
		total_turnaround = total_turnaround + iorequests[i]->turnaround;
		
		// cout << "turnaround "  << iorequests[i]->turnaround <<endl;
		// cout << "compute "  << iorequests[i]-> end_time - iorequests[i]->time_issued <<endl; 
		total_wait_time = total_wait_time + iorequests[i]->wait_time;
		// cout << "wait_time "  << total_wait_time <<endl;
	}
}

void printSummary(){
	// loop over iorequest queue and compute stats
	// cout << "iorequests " << requests <<endl;
	avg_turnaround = total_turnaround/ requests;
	avg_wait_time = total_wait_time / requests;
	 printf("SUM: %d %d %.2lf %.2lf %d\n", total_time, total_movement, avg_turnaround,
			 avg_wait_time,	max_wait_time);
}
