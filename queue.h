#include"tcb.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


// Circular Doubly linked list to store tcb of threads in the ReadyQ and Semphore Queues

void initQueue(struct tcb_t **head_ref)
{

	tcb_t *dummy = (tcb_t*)malloc(sizeof(struct tcb_t));
	dummy->thread_id = 100;
	dummy->next = dummy->prev = dummy;
	*head_ref = dummy;
	
}


void AddQueue(struct tcb_t **head_ref,struct tcb_t *node)
{
	tcb_t *temp = *head_ref;
	if (temp->next==temp)
	{
		node->next = *head_ref;
		node->prev = *head_ref;
		*head_ref = node;
		return;
	}
	else
	{

		while(temp->next->thread_id!=100)
		    temp = temp->next;

		node->prev = temp;	
		temp->next->prev = node;
		node->next = temp->next;
		temp->next = node;

	}	

}

void rotateQueue(struct tcb_t **head_ref){
    tcb_t *temp = *head_ref;
    if(temp->next==temp)
            printf("Queue empty\n");
    else
    {
     *head_ref = temp->next;
    }
}


tcb_t *DelQueue(struct tcb_t **head_ref)
{
	tcb_t *temp = *head_ref;
	if(temp->next==temp)
	{
		printf("here");
		return *head_ref;
	}
	else 
	{
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		*head_ref = temp->next;
		return temp;
	}


}

void display(struct tcb_t **head_ref){
        tcb_t *temp = *head_ref;
	if(temp->next==temp){
		printf("Queue empty\n");
		return;
	}
	
        while(1){
                printf("thread_id = %d\n",temp->thread_id);
                temp = temp->next;
		sleep(1);
		if(temp->thread_id==100)
			break;
        }
	

}

