#include<ucontext.h>
#include<string.h>

typedef struct tcb_t{
	struct tcb_t *next;
	struct tcb_t *prev;
	int thread_id;
	ucontext_t context;
}tcb_t;

void init_TCB (tcb_t *TCB, void *function, void *stackP, int stack_size,int id1,int id2)
{

    memset(TCB, '\0', sizeof(struct tcb_t));       // wash, rinse

    getcontext(&TCB->context);              // have to get parent context, else snow forms on hell

    TCB->context.uc_stack.ss_sp = stackP;

    TCB->context.uc_stack.ss_size = (size_t) stack_size;

    makecontext(&TCB->context, function, 2,id1,id2);// context is now cooked

}
