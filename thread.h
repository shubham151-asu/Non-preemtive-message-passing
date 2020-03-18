#include"queue.h"


int stacksize  = 8192;
struct tcb_t *ReadyQ;
struct tcb_t *Curr_Thread = NULL;

int counter = 0;


void init_readyQ(){
	 initQueue(&ReadyQ);
}


void start_thread(void (*function),int id1,int id2)
{
	void *stack = (void*)malloc(sizeof(int)*stacksize);
	struct tcb_t *TCB = (tcb_t*)malloc(sizeof(struct tcb_t));
	init_TCB(TCB, function , stack, stacksize,id1,id2);
	TCB->thread_id = ++counter; // assigning counter value to each thread
	AddQueue(&ReadyQ,TCB);
}


void yield()
{
   tcb_t *Prev_Thread; 
   //display(&ReadyQ);
   AddQueue(&ReadyQ, Curr_Thread); 
   //display(&ReadyQ);
   Prev_Thread = Curr_Thread;
   //rotateQueue(&ReadyQ); 
   Curr_Thread = DelQueue(&ReadyQ);
   //if (Curr_Thread->thread_id!=100)
   swapcontext(&(Prev_Thread->context),&(Curr_Thread->context));
   //else
   //	AddQueue(&ReadyQ, Curr_Thread);	
}

void run()
{  
    //display(&ReadyQ);	 
    Curr_Thread = DelQueue(&ReadyQ);
    //printf("in run\n");
    //printf("Executing thread %d\n",Curr_Thread->thread_id);
    //display(&ReadyQ);
    ucontext_t parent; 
    getcontext(&parent);   
    swapcontext(&parent, &(Curr_Thread->context));// start the first thread
} 
