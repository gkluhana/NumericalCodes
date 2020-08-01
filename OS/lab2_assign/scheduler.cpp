#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;



//Process State
enum Process_State {
  STATE_CREATED,
  STATE_READY,
  STATE_RUNNING,
  STATE_BLOCKED,
  STATE_PREMPTED,
}; 
//State Transition 
enum State_Transition {
  
  TRANS_TO_READY,
  TRANS_TO_RUN,
  TRANS_TO_BLOCK,
  TRANS_TO_PREEMPT,
  DONE,
};

//Class Definitions
//Process Class
class Process {
    
    int AT, TC, CB, IO, PRIO, FT,TT,IT,CW;
    Process_State PS;
    int TS;
    int TIPS;
    int remaining_burst;
    int rem_exec_time;
    bool incomplete;
    int PID;
    int PRIO_dynamic;
    bool expired;
    


    public:
    void set_values (int,int,int,int,Process_State, int, int);
    int state_ts(); 
    void timeInPrevState(int);
    int getAT();    
    int getCB();
    int getTC();
    int getIO();
    int getPID();
    int getPS();
    int get_prev_time();
    int get_PRIO();
    int get_PRIO_dynamic();
    int rem_time();
    void set_rem_time(int);
    void set_PRIO(int);
    void set_stats(int);
    int getFT();
    int getTT();
    int getIT();
    int getCW();
    void setPS(Process_State ps){
        PS = ps;
    }
    void setIT(int);
    void setCW(int);
    void set_remaining_burst(int,bool);
    bool is_incomplete();
    bool is_expired();
    int get_remaining_burst();
    void decrease_PRIO_dyn();
    void reset_PRIO_dynamic();
    void make_active();
    void make_expire();
};
//Process Functions
void Process::set_values (int a, int t, int c, int i, Process_State ps, int ts, int pid) {
    
    AT=a;
    TC=t;
    CB=c;
    IO=i;
    PS= ps;
    TS = ts;
    PID = pid;
    rem_exec_time=TC;
    TIPS = 0;
    IT = 0;
    CW = 0;
    expired= false;
    // cout << "AT, TC, CB, IO set to" << a<< t << c << i<<endl;
}
//Last TimeStamp
int Process::state_ts(){
    
    return TS;
}
//Time in Previous State
void Process::timeInPrevState(int tips){
    
    TIPS = tips;
    TS = TS+tips;
}
int Process::getAT(){
    
    return AT;
}
int Process::getCB(){
    
    return CB;
}
int Process::getTC(){
    
    return TC;
}
int Process::getIO(){
    
    return IO;
}
int Process::rem_time(){
    
    return rem_exec_time;
}
int Process::getPID(){
    
    return PID;
}
int Process::get_prev_time(){
    
    return TIPS;
}
int Process::getPS(){
    
    return PS;
}
int Process::get_PRIO_dynamic(){
    
    return PRIO_dynamic;
}
int Process::get_PRIO(){
    
    return PRIO;
}
void Process::set_rem_time(int exec_time){
    
    rem_exec_time = rem_exec_time - exec_time; 
}
void Process::set_PRIO(int prio){
    
    PRIO = prio;
    PRIO_dynamic = PRIO-1;
}
void Process::decrease_PRIO_dyn(){
    PRIO_dynamic--;
    if(PRIO_dynamic==-1){expired=true;PRIO_dynamic=PRIO-1;}
}
void Process::reset_PRIO_dynamic(){

    PRIO_dynamic=PRIO-1;
}
void Process::set_stats(int ft){
    
    FT = ft;
    TT= ft-AT;
}
int Process::getFT(){
    
    return FT;
}
int Process::getTT(){
    
    return TT;
}
int Process::getIT(){
    
    return IT;
}
int Process::getCW(){
    
    return CW;
}
void Process::setIT(int it){
    
    IT = IT + it;
}
void Process::setCW(int cw){

    CW = CW + cw;
}
void Process::set_remaining_burst(int rb,bool inc){
    
    remaining_burst=rb;
    incomplete=inc;
}
int Process::get_remaining_burst(){

    return remaining_burst;
}
bool Process::is_incomplete(){
    
    return incomplete;
}
bool Process::is_expired(){
    
    return expired;
}
void Process::make_active(){
    
    expired=false;
}
void Process::make_expire(){
    
    expired= true;
}


//Event Class
class Event {
    
    int TIMESTAMP;
    Process* PROC;
    State_Transition ST;

    public:
    void set_values (int,Process*,State_Transition);
    State_Transition transition();
    Process* evtProcess();
    int evtTimeStamp();
};

//Event Functions 
void Event::set_values (int ts,Process* p, State_Transition st) {
    
    TIMESTAMP=ts;
    PROC = p;
    ST = st;   
}
State_Transition Event::transition(){
    
    return ST;
}
Process* Event::evtProcess(){
    
    return PROC;
}
int Event::evtTimeStamp(){
    
    return TIMESTAMP;
}


//Event/Process List Structure
struct node
  {
    Event *evt;
    Process *proc;
    node *next;
  };    
class list{
    node *head, *tail;
    public:
    list()
    {
      head=NULL;
      tail=NULL;
    }
    //Inserts at end

    //Event Functions
    void createEventNode(Event *new_event)
    {


      //Create Node
      node *temp=new node;
      temp->evt=new_event;
      temp->next=NULL;
      
      
      //Empty List
      if(head==NULL)
      {
        head=temp;
        tail=temp;
        temp=NULL;
        return;
      }

      //Find Node Placement in List
      node *prev = NULL;
      node *curr = head;

      while(curr->evt->evtTimeStamp()<=new_event->evtTimeStamp()){
        prev=curr;
        curr=curr->next;

        //Tail also crossed
        if (curr==NULL){break;}
      }

      //Head Not Crossed
      if(curr==head){
        head = temp;
        head->next = curr;
        temp = NULL;
      }
      //Tail Crossed
      else if (curr==NULL){
        tail = temp;
        prev->next = temp;
        temp=NULL;
      }
      //Middle
      else{
        prev->next=temp;
        temp->next=curr;
        temp=NULL;
      }
      
    }
    void createEventNode(int ts,Process* p, State_Transition st)
    {
      Event* new_event = new Event;
      new_event->set_values(ts, p, st);

      //Create Node
      node *temp=new node;
      temp->evt=new_event;
      temp->next=NULL;
      
      
      //Empty List
      if(head==NULL)
      {
        head=temp;
        tail=temp;
        temp=NULL;
        return;
      }

      //Find Node Placement in List
      node *prev = NULL;
      node *curr = head;

      while(curr->evt->evtTimeStamp()<=new_event->evtTimeStamp()){
        prev=curr;
        curr=curr->next;

        //Tail also crossed
        if (curr==NULL){break;}
      }

      //Head Not Crossed
      if(curr==head){
        head = temp;
        head->next = curr;
        temp = NULL;
      }
      //Tail Crossed
      else if (curr==NULL){
        tail = temp;
        prev->next = temp;
        temp=NULL;
      }
      //Middle
      else{
        prev->next=temp;
        temp->next=curr;
        temp=NULL;
      }
    }

    //Create Insert Event At Particular Location

    //Inserts at Front
    void insertEventFront(Event *new_event)
    {
        node *temp=new node;
        temp->evt=new_event;
        temp->next=head; //takes care of NULL head case
        head=temp;
    }

    //Remove from Front and return
    Event* get_event(){

        if(!head){return NULL;}
        else {node *temp=new node;
        temp = head;
        head = head->next;
        return temp->evt;}
    }
    int get_next_event_time(){
        // cout << "getting next event time!" <<endl;
        if(!head){return NULL;}
        return head->evt->evtTimeStamp();
    }

    //Process Functions
    //creates and adds at end
    void createProcessNode(Process *p)
    {
      node *temp=new node;
      temp->proc= p;
      temp->next=NULL;
      if(head==NULL)
      {
        head=temp;
        tail=temp;
        temp=NULL;
      }
      else
      { 
        tail->next=temp;
        tail=temp;
      }
    }

    //Inserts at Front
    void insertProcessFront(Process *p)
    {
        node *temp=new node;
        temp->proc=p;
        temp->next=head; //takes care of NULL head case
        head=temp;
    }

    void insertProcessShortest(Process *p)
    {
        //Create Node
      node *temp=new node;
      temp->proc=p;
      temp->next=NULL;
      
      
      //Empty List
      if(head==NULL)
      {
        head=temp;
        tail=temp;
        temp=NULL;
        return;
      }

      //Find Node Placement in List
      node *prev = NULL;
      node *curr = head;

      while(curr->proc->rem_time()<= p->rem_time()){
        prev=curr;
        curr=curr->next;

        //Tail also crossed
        if (curr==NULL){break;}
      }

      //Head Not Crossed
      if(curr==head){
        head = temp;
        head->next = curr;
        temp = NULL;
      }
      //Tail Crossed
      else if (curr==NULL){
        tail = temp;
        prev->next = temp;
        temp=NULL;
      }
      //Middle
      else{
        prev->next=temp;
        temp->next=curr;
        temp=NULL;
      }

    }

    //Remove from Front and return
    Process* popProcessFront(){
        if (head==NULL){return NULL;}
        node *temp=new node;
        temp = head;
        head = head->next;
        return temp->proc;
    }


    //Remove from End and return



    //Round Robin Return

    //PRIO return
};    


// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// Schedulers

//Scheduler Base Class
class Scheduler{

    public:
    virtual Process* get_next_process()=0;
    virtual void add_to_queue(Process*)=0;
};

// Scheduler subclasses
// FCFS/FIFO

class FCFS: public Scheduler{
    //Create RUN/READY Queue
    list RunQueue;
    public:
    Process* get_next_process();
    void add_to_queue(Process*);
};
//FCFS/FIFO functions
Process* FCFS::get_next_process(){
    
    // cout << "getting next process!" << endl;
    return RunQueue.popProcessFront();
}
void FCFS::add_to_queue(Process *proc){
    
    // cout << " added process" << proc -> getPID() << " to runqueue" << endl;
    RunQueue.createProcessNode(proc);
}

// LCFS/LIFO
class LCFS:public Scheduler{
    list RunQueue;
    public:
    Process* get_next_process();
    void add_to_queue(Process*);
};
//LCFS/LIFO functions
Process* LCFS::get_next_process(){
   
    return RunQueue.popProcessFront();  
}
void LCFS::add_to_queue(Process *proc){
    
    RunQueue.insertProcessFront(proc);
}

//SJF
class SJF: public Scheduler{
    list RunQueue;
    public:
    Process* get_next_process();
    void add_to_queue(Process*);
};
Process* SJF::get_next_process(){

    return RunQueue.popProcessFront();
}
void SJF::add_to_queue(Process *proc){
    
    RunQueue.insertProcessShortest(proc);
}

//RR
class RR: public Scheduler{
    list RunQueue;
    public:
    Process* get_next_process();
    void add_to_queue(Process*);
};
Process* RR::get_next_process(){

    return RunQueue.popProcessFront();
}
void RR::add_to_queue(Process *proc){
    
    RunQueue.createProcessNode(proc);
}

// PRIO
class PRIO: public Scheduler{
    
    list prio[2][4];
    int active;
    int expired;
    public:
    Process* get_next_process();
    void add_to_queue(Process*);
    void initialize_prio();  
};  
Process* PRIO::get_next_process(){
    Process* proc;
    // cout << "trying to get process from active q # " << active <<endl;
    //Search Active Queue for Process
    for (int i=3;i>-1;i--){
     if(proc=prio[active][i].popProcessFront()){return proc;}
    }

    //Active Queue was empty, Switch Active and Expired
    int temp=0;
    if(proc==NULL){
        temp = expired;
        expired = active;
        active= temp;
    }
    // cout << "trying to get process from active q # " << active <<endl;
    //Search Active Queue for Process Again
    for (int i=3;i>-1;i--){
     if(proc=prio[active][i].popProcessFront()){return proc;}
    }

    //Active Queue still empty
    if(proc==NULL){return NULL;}
}

void PRIO::add_to_queue(Process *proc){

    if (proc->is_expired()){
        // cout << "process added to expired q # "<< expired <<endl;
        prio[expired][proc->get_PRIO_dynamic()].createProcessNode(proc);
        proc->make_active();
    }
    else{
        // cout << "process added to active q # "<< active << "at PRIO level "<<proc->get_PRIO_dynamic() <<endl;
        prio[active][proc->get_PRIO_dynamic()].createProcessNode(proc);
    }

}
void PRIO::initialize_prio(){
    active=0;
    expired=1;
}


//File Read Structure
struct fileRead{
    string line;
    ifstream inFile;
    char *tok;
    char *buffer;
    int ofs;
    int count;
    char* filepath;
};
struct IOinfo{
    int num_IO;
    int timestamp_on;
    int timestamp_off;
    bool in_IO;
    int IO_time;
};

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// Main Program Function Declarations
void printFile(char* filepath);
int readFile(fileRead *inputfile, list& Event_Queue, fileRead* randfile);
void Simulation(list& event_queue, fileRead* randfile, Process* summary[],char sched_type, int timequantum,IOinfo& io_util);
int myrandom(int burst, fileRead *randfile);
void printSummary(Process* summary[], int num_processes, IOinfo io_util);



//MAIN
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
    // printFile(inputfilepath);  

    // Event queue
    list Event_Queue;
    
    // Structs for input files
    fileRead *inputfile = new fileRead;
    inputfile->inFile.open(inputfilepath);

    fileRead *randfile = new fileRead;
    randfile->filepath = randfilepath;
    randfile->inFile.open(randfile->filepath);
    getline(randfile->inFile,randfile->line);
    randfile->count = atoi(strdup(randfile->line.c_str()));

    //Read File and return number of processes
    int num_processes=readFile(inputfile, Event_Queue, randfile);
    IOinfo io_util; 
    //Final Process Array for Summary
    Process* Summary[num_processes];

    //Simulation
    Simulation(Event_Queue,randfile, Summary,sched_type, timequantum, io_util);
    // Print output
    printSummary(Summary,num_processes, io_util);
}
//Main Simulation
void Simulation(list& Event_Queue,fileRead* randfile, Process* summary[], char sched_type, int timequantum, IOinfo& io_util){
    //Code to Create Scheduler according to specification
    Scheduler* scheduler;
    FCFS fcfs;
    LCFS lcfs;
    SJF sjf;
    RR rr;
    PRIO prio;
    // cout << "Time Quantum is" << timequantum << endl;

    switch(sched_type){
        case 'F':
        scheduler = &fcfs;
        cout <<"FCFS"<<endl;
        break;
        case 'L':
        scheduler = &lcfs;
        cout <<"LCFS"<<endl;
        break;
        case 'S':
        scheduler = &sjf;
        cout << "SJF"<<endl;
        break;
        case 'R':
        scheduler = &rr;
        cout << "RR " <<timequantum<< endl;
        break;
        case 'P':
        scheduler = &prio;
        prio.initialize_prio();
        cout << "PRIO " <<timequantum<< endl;
        break;
    }

    Event* evt;
    int CURRENT_TIME; 
    bool CALL_SCHEDULER=false;
    Process *CURRENT_RUNNING_PROCESS=NULL;
    int randCB;
    int randIO;

    // cout << "reached here!" <<endl;
    io_util.IO_time=0;
    io_util.num_IO=0;
    io_util.in_IO=false;
    // cout << "reached here!" <<endl;
    
    while((evt)=Event_Queue.get_event()){

        

        Process *proc = evt -> evtProcess();  //get process from Event
        CURRENT_TIME = evt-> evtTimeStamp();  //get timestamp from Event
        evt->evtProcess()-> timeInPrevState(CURRENT_TIME-proc->state_ts());
        
        // printf("%d %d %d: ", CURRENT_TIME, proc->getPID(), proc->get_prev_time());
        
        switch(evt -> transition()){ // which state to transition to?
            case TRANS_TO_READY:

                //If Process coming from Blocked State
                if (proc->getPS()==STATE_BLOCKED){
                    proc->reset_PRIO_dynamic();
                    proc->make_active();
                    io_util.num_IO--;
                    if(io_util.num_IO==0){
                    // cout << "ready Num of Processes in IO are"<<io_util.num_IO << " at time" << CURRENT_TIME << endl; 
                    io_util.timestamp_off=CURRENT_TIME;
                    io_util.IO_time=(io_util.timestamp_off - io_util.timestamp_on)+io_util.IO_time;
                    }
                }
                
                //add to Run Queue
                scheduler->add_to_queue(proc);
                proc-> setPS(STATE_READY);
                // cout <<" TRANS_TO_READY "<< endl;

                CALL_SCHEDULER = true; // conditional on wheter something is run
            break;

            case TRANS_TO_RUN:
                proc->setCW(proc->get_prev_time());
                
                proc-> setPS(STATE_RUNNING);
                
                //Donot compute CB if incomplete
                if(proc->is_incomplete()){
                    randCB = proc->get_remaining_burst();
                    proc->set_remaining_burst(0,false);
                }
                else{randCB = myrandom(proc->getCB(),randfile);}
                // cout << "remaining burst is" << proc->get_remaining_burst()<<endl;
                // cout << "randCB is" << randCB << endl;

                // cout << "CB is "<< randCB << endl;

                //Check Remaining Execution Time for Process
                if(proc->rem_time() <= randCB){
                    // proc->set_remaining_burst(0,false);
                    randCB = proc->rem_time();
                }

                if(randCB<=timequantum){
                    
                    proc->set_remaining_burst(0,false);

                    if(proc->rem_time() == randCB){
                    // Event Done
                    Event_Queue.createEventNode(CURRENT_TIME+randCB,proc,DONE);
                    // cout <<" TRANS_TO_RUN "<< " cb="<<randCB <<" rem="<< proc->rem_time()<<" prio="<< proc->get_PRIO_dynamic() <<endl;
                    }
                    else{
                    // Create Event to Block
                    Event_Queue.createEventNode(CURRENT_TIME+randCB,proc,TRANS_TO_BLOCK);
                    }
                    // cout << "Created Event to Block"<< endl;
                }
                else{
                    //Create Event to Preempt

                    proc->set_remaining_burst(randCB-timequantum,true);
                    Event_Queue.createEventNode(CURRENT_TIME+timequantum,proc,TRANS_TO_PREEMPT);
                    // cout<<"Created Event to Premept"<<endl;
                }
                // cout <<" TRANS_TO_RUN "<< " cb="<<randCB <<" rem="<< proc->rem_time()<<" prio=" <<proc->get_PRIO_dynamic() <<endl;
                CALL_SCHEDULER = true;
            break;
            case TRANS_TO_BLOCK:
                //Another Process comes to IO 
                io_util.num_IO++;
                if(io_util.num_IO==1){io_util.timestamp_on=CURRENT_TIME;}
                // cout << "blocked Num of Processes in IO are"<<io_util.num_IO << " at time" << CURRENT_TIME << endl; 
                // cout <<io_util.timestamp_on <<endl;
                CURRENT_RUNNING_PROCESS = NULL;

              //create an event for when process becomes ready again
                randIO = myrandom(proc->getIO(),randfile);
                Event_Queue.createEventNode(CURRENT_TIME+randIO,proc,TRANS_TO_READY);
                
                //Change Process parameters
                proc->set_rem_time(proc->get_prev_time());
                proc-> setIT(randIO);
                proc-> setPS(STATE_BLOCKED);
                // cout<< " TRANS_TO_BLOCK "<< " ib=" << randIO << " rem="<< proc->rem_time()<< endl;
                
                CALL_SCHEDULER = true;
                break;
            case TRANS_TO_PREEMPT: 
               //add to runqueue (no event is generated)
                proc->decrease_PRIO_dyn();
                proc->set_rem_time(proc->get_prev_time());
                proc-> setPS(STATE_PREMPTED);
                // cout <<" TRANS_TO_PREEMPT "<< " cb=" << proc->get_remaining_burst() << " rem="<< proc->rem_time()<<" prio=" <<proc->get_PRIO_dynamic() <<endl;                
                
                CURRENT_RUNNING_PROCESS = NULL;
                scheduler->add_to_queue(proc);
                CALL_SCHEDULER = true;
                break;  
            case DONE:
                // cout << " Done" << endl;
                summary[proc->getPID()] = proc;
                proc->set_stats(CURRENT_TIME);
                CURRENT_RUNNING_PROCESS = NULL;
                CALL_SCHEDULER = true;
        }


        //remove current event object from memory;
        delete evt;
        evt = NULL;

        if(CALL_SCHEDULER){
            // cout << "Scheduler Called!" <<endl;
            //If there are events with the same timestamp as current timestamp,
            //process them all first, before heading to scheduler

            if(Event_Queue.get_next_event_time() == CURRENT_TIME){
                // cout << "more right now, continuing!" <<endl;
                continue; // process next event from Event queue
            }


            //why? because all current timestamp events have been processed
            //scheduler doesn't need to be scheduled
            CALL_SCHEDULER = false;
            // cout << "no more with same time stamp!" <<endl;
            //There's no running process... scheduler will get next process and make it running

            if(CURRENT_RUNNING_PROCESS==NULL){
                // cout << "current running is NULL!" <<endl;
                CURRENT_RUNNING_PROCESS = scheduler->get_next_process();
                // cout <<" CURRENT_RUNNING_PROCESS is" << CURRENT_RUNNING_PROCESS->getPID() <<endl;
                //nothing in run queue

                if(CURRENT_RUNNING_PROCESS==NULL){
                    continue;
                }

            // create event to make process runnable for same time
                Event_Queue.createEventNode(CURRENT_TIME,CURRENT_RUNNING_PROCESS,TRANS_TO_RUN);
            }
        }
     }     
}
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// OTHER FUNCTIONS

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

//Reads Processes and creates Events
int readFile(fileRead *inputfile, list& Event_Queue,fileRead* randfile){
    
    
    if (!inputfile->inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    int cb;
    int pid=0;
    int prio;
    // int linenumber = 1;
    // Get line and Print line
    if(inputfile->inFile){
        while (getline(inputfile->inFile,inputfile->line)) {          
            
            //GET PROCESS INFORMATION
            inputfile->buffer = strdup(inputfile->line.c_str());
            int a,t,c,i;
            for (int j =0; j < 4; j++){
                if(j==0){a = atoi(strtok(inputfile->buffer, " "));}
                else if(j==1){t = atoi(strtok(NULL, " "));}
                else if(j==2){c = atoi(strtok(NULL, " "));}
                else if(j==3){i = atoi(strtok(NULL, " "));}
                }

            //CREATE PROCESS AND SEND TO EVENT Q
            Process *proc = new Process;
            prio = myrandom(4,randfile);

            proc-> set_PRIO(prio);
            proc->set_values(a,t,c,i, STATE_CREATED,a,pid++);

            
            Event *evt = new Event;
            evt->set_values(proc->getAT(),proc,TRANS_TO_READY);
            Event_Queue.createEventNode(evt);
            // cout << endl;
        }               
    inputfile->inFile.close();

    }
    return pid;
}
   
// Random number code for CB/IO Bursts
int myrandom(int burst, fileRead *randfile){
    // cout <<"myrandom was called" <<endl;

    //get next random number according to ofs value
    getline(randfile->inFile,randfile->line);
    int randval = atoi(strdup(randfile->line.c_str()));
    int val = 1 + (randval % burst);
    randfile -> ofs++;

    // cout << "randval is" << randval << " val is" << val << endl;

    //fix Offset for next time
    if(randfile->ofs==randfile->count-1){
        randfile->ofs=0;
        randfile->inFile.close();
        randfile->inFile.open(randfile->filepath);
        getline(randfile->inFile,randfile->line);
    } 

    return val;
}
// Print Summary Function
void printSummary(Process* Summary[], int num_processes,IOinfo io_util){
    Process *proc;
    int end_time=0;
    double sum_cpu=0;
    double cpu_perccentage=0;
    double sum_io = 0;
    double io_time = io_util.IO_time;
    
    double sum_tt = 0;
    double ave_tt = 0;
    double sum_cw = 0;
    double ave_cw = 0;

    for(int i=0; i<num_processes;i++){
        proc = Summary[i];
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",proc->getPID(),proc->getAT(),
            proc->getTC(),proc->getCB(),proc->getIO(),proc->get_PRIO(),proc->getFT(),proc->getTT(),proc->getIT(),proc->getCW());
        if (proc->getFT() >end_time){end_time=proc->getFT();}
        sum_cpu = sum_cpu + proc->getTC();
        sum_io = sum_io + proc-> getIT();
        sum_tt = sum_tt + proc ->getTT();
        sum_cw = sum_cw + proc -> getCW();
    }
    cpu_perccentage = (sum_cpu/end_time)*100;
    double io_percentage = (io_time/end_time)*100;
    ave_tt = (sum_tt/num_processes);
    ave_cw = (sum_cw/num_processes);
    // cout << num_processes << end_time << num_processes/end_time<<endl;
    double throughput = (((double)num_processes)/(double)end_time)*100;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",end_time,cpu_perccentage,io_percentage,ave_tt,ave_cw,throughput);
}