#ifndef SCHEDUELER_H
#define SCHEDUELER_H

#include<stdint.h>
#include<stdlib.h>

#define STACK_SIZE      128
#define MAX_TASK_COUNT  5


typedef struct tcb_
{
    unsigned long int stack[STACK_SIZE];
    unsigned long int* _sp;
    struct tcb_ * next;

}tcb;


/*Prototypes*/
void enable_interrupts(void);
void disable_interrupts(void);
void trigger_pendSV(void);
void systick_handler(void);
void create_task(void* task);
void pendSV_handler(void);

void start_scheduler(void *task);

void init_systick(void);

#endif