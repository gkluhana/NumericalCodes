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


//Global Variables

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
	vector <io_t*> ioqueue_inactive;	//only used in FLOOK
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
	// c
	if(ioqueue.empty()){
		return NULL;}
	io_t *closest = ioqueue[0]; //closest to arm
	int distance = closest->track_accessed - track;	//initialize min distance 
	if (distance < 0) {distance = distance* -1;}	//absolute distance
	
	int tempdistance =0;
	int index =0; //index of consideration 
	int remove_index=0;	//index to be removed

	//loop over ioqueue
	for (vector<io_t*>::iterator it=ioqueue.begin(); it!=ioqueue.end();++it){
		tempdistance = (*it)->track_accessed - track;
		if(tempdistance<0) {tempdistance = tempdistance * (-1);}
		
		//same as previous min, compare IDS
		if(tempdistance==distance){
			if(closest->id < (*it)->id){}
			else{closest = *it; remove_index=index;}
		}

		//less than prev min
		else if(tempdistance<distance){
			distance=tempdistance; 
			closest = *it; 
			remove_index=index;}
		index++;
	}
	//remove from queue and return
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
	if(ioqueue.empty()){
		return NULL;}
	io_t *closest1 = ioqueue[0];
	io_t *closest2 = ioqueue[0];

	int tempdistance =0;

	//init mins to some large value
	int mindistance1 =1000000000;
	int mindistance2 =1000000000;
	
	int index =0; 
	int remove_index1=0;		//for same direction
	int remove_index2=0;		//for other direction
	
	bool request_direction; 	//direction of consideration
	bool same_direction=false;	//if found in same direction, true
	
	for (vector<io_t*>::iterator it=ioqueue.begin(); it!=ioqueue.end();++it){
		
		//Compute Current Request Distance
		tempdistance = (*it)->track_accessed - track;
		
		//If zero, at same point, set mindistance1 to 0, set new closest
		if(tempdistance==0){
			mindistance1=0;
			if(closest1->id < (*it)->id){}
			else{closest1 = *it; remove_index1=index;}
		}

		//Find current request direction
		if(tempdistance<0){request_direction=false;}
		else if(tempdistance>0){request_direction=true;}
	
		//Make current distance positive
		if(tempdistance<0) {tempdistance = tempdistance * (-1);}
		
		//If direction Same as current direction, compare with min distance 1
		if(request_direction==direction){
			same_direction = true;
			if(tempdistance==mindistance1){
				if(closest1->id < (*it)->id){}
				else{closest1 = *it; remove_index1=index;}
			}
			else if(tempdistance<mindistance1){mindistance1=tempdistance; closest1 = *it; remove_index1=index;}
		}
		//If different direcctions, compare with min distance 2, in case same direction not found at all
		else{
				if(tempdistance==mindistance2){
				if(closest2->id < (*it)->id){}
				else{closest2 = *it; remove_index2=index;}
			}
			else if(tempdistance<mindistance2){mindistance2=tempdistance; closest2 = *it; remove_index2=index;}
		}
		index++;
	}

	//Decide which value to return
	if(same_direction){
			ioqueue.erase(ioqueue.begin()+remove_index1);
			return closest1;

		}
		else{
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
	
	if(ioqueue.empty()){
		return NULL;}
	direction = true;
	io_t *closest1 = ioqueue[0];
	io_t *closest2 = ioqueue[0];

	int tempdistance1 =0;
	int tempdistance2 =0;

	int mindistance1 =100000000;
	int mindistance2 =100000000;

	int index =0; 
	int remove_index1=0;
	int remove_index2=0;

	bool request_direction; 
	bool same_direction=false;
	//Loop over ioqueue
	for (vector<io_t*>::iterator it=ioqueue.begin(); it!=ioqueue.end();++it){
	
		//Compute Current Request Distance from arm, and zero (for wraparound)
		tempdistance1 = (*it)->track_accessed - track;
		tempdistance2 = (*it)->track_accessed;

		//if current distance from arm is zero, choose this io, check id if any other io
		// also present
		if(tempdistance1==0){
			mindistance1=0;
			if(closest1->id < (*it)->id){}
			else{closest1 = *it; remove_index1=index;}
		}

		//Find current request direction
		if(tempdistance1<0){request_direction=false;}
		else if(tempdistance1>0){request_direction=true;}
		
		//Make tempdistance positive
		if(tempdistance1<0) {tempdistance1 = tempdistance1 * (-1);}
		
		//If Direction Same as current direction, compare with min distance 1
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

	//Decide which to return
	if(same_direction){
			ioqueue.erase(ioqueue.begin()+remove_index1);
			return closest1;

		}
		else{
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
//FLOOK Function
io_t* FLOOK::strategy(){
	vector <io_t*> ioqueue_temp;
	
	//If queue is empty, switch queues
	if(ioqueue.empty()){
		ioqueue_temp = ioqueue;
		ioqueue = ioqueue_inactive;
		ioqueue_inactive = ioqueue_temp;
		}

	//Same as LOOK Function from here
	//if queue is still empty
		if(ioqueue.empty()){
		return NULL;}


	io_t *closest1 = ioqueue[0];
	io_t *closest2 = ioqueue[0];
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

		//Compute Current Request Distance
		tempdistance = (*it)->track_accessed - track;

		//If at same point, compare ids with any other at same point, and select
		if(tempdistance==0){
			mindistance1=0;
			if(closest1->id < (*it)->id){}
			else{closest1 = *it; remove_index1=index;}
		}

		//Find current request direction
		if(tempdistance<0){request_direction=false;}
		else if(tempdistance>0){request_direction=true;}

		//Make tempdistance positive
		if(tempdistance<0) {tempdistance = tempdistance * (-1);}
		
		//If Same as current direction, compare with min distance 1
		if(request_direction==direction){
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
	//Decide which to select
	if(same_direction){
			ioqueue.erase(ioqueue.begin()+remove_index1);
			return closest1;

		}
		else{
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
	int curr_time=1;
	int movement=0;
	int tot_wait_time=0;
	int index_request=0;
	bool active = false;
	bool completed = false;
	bool pending = false;
	io_t *current_io; 

	//Initialize Scheduler
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

	while(true){
		if (iorequests[index_request]==NULL){}
		//Add io to queue if arrived	
		else if(iorequests[index_request]->time_issued==curr_time){
			scheduler->insert(iorequests[index_request]);
			index_request++;
		}
	
		//If there is a current io, and it is on current track, completed
		if(current_io){
			if(current_io->track_accessed == track){
				completed = true;
			}
		}
		
		//Active and completed IO
		if(active && completed){
			current_io->end_time = curr_time;
			current_io->turnaround =  current_io->end_time - current_io->time_issued;
			completed = true;
			current_io = NULL;
			active = false;
		}
		//Active and not Completed IO
		else if(active && !completed){	
			if(direction){
				track++;
				movement++;
			}
			else{
				track--;
				movement++;
			}

		}
		//No active IO
		if (!active){	
			current_io = scheduler->strategy();

			//if Still No IO
			if(!current_io){
				
				//If requests are remaining, increment time and continue loop
				if(iorequests.size() > index_request){
					// curr_time = iorequests[index_request]->time_issued;
					curr_time++;
					continue;}
				break;
			}
			//IO found
			else{
				current_io->start_time = curr_time;
				
				current_io-> wait_time = curr_time - current_io->time_issued;
				if(max_wait_time <current_io-> wait_time){
					max_wait_time = current_io-> wait_time;
				}

				completed = false;
				active = true;

				//Next IO somewhere else
				if(current_io->track_accessed < track){direction =false;}
				else if(current_io->track_accessed > track) {direction = true;} 
				
				//If next io at same point, do not move arm, continue loop
				else{continue;}

				//Move Arm
				if(direction){
					track++;
					movement++;
				}
				else{
					track--;
					movement++;
				}
			}
		}
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

//Reads and creates request queue
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
			index++;
			requests++;

			}
			iorequests.resize(index);
			
	}

}

//Prints Standard Output
void printOutput(){
	for (int i=0; i<iorequests.size();i++){
	
		printf("%5d: %5d %5d %5d\n", iorequests[i]->id, iorequests[i]->time_issued,
					 iorequests[i]->start_time, iorequests[i]->end_time);
		total_turnaround = total_turnaround + iorequests[i]->turnaround;
		
		total_wait_time = total_wait_time + iorequests[i]->wait_time;
	}
}

//Prints SUM Line
void printSummary(){
	avg_turnaround = total_turnaround/ requests;
	avg_wait_time = total_wait_time / requests;
	 printf("SUM: %d %d %.2lf %.2lf %d\n", total_time, total_movement, avg_turnaround,
			 avg_wait_time,	max_wait_time);
}
