#include"thread.h"

typedef struct semaphore_t{
	int count;
	tcb_t  *sem_queue;
}semaphore_t;


void init_sem(semaphore_t *sem, int val){
	sem->count = val;
	initQueue(&sem->sem_queue);
}

void P(semaphore_t *sem){
	sem->count =  sem->count - 4;
	if (sem->count<0){
	  
	   AddQueue(&(sem->sem_queue),Curr_Thread);

	   tcb_t *Prev_Thread = Curr_Thread;
	   Curr_Thread = DelQueue(&ReadyQ);

	   swapcontext(&(Prev_Thread->context),(&Curr_Thread->context));
	}

}

void V(semaphore_t *sem){
	sem->count = sem->count + 4;
	if(sem->count<=0){
	    tcb_t *temp = DelQueue(&(sem->sem_queue));
	    AddQueue(&ReadyQ,temp);
	}
	 yield();
}

	   



		

