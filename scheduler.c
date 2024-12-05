#include"scheduler.h"

tcb TCB[MAX_TASK_COUNT];
uint8_t task_count = 0;
uint8_t current_task_num = 0;
uint8_t next_task_num = 1;

volatile unsigned long int *current_pc;
volatile unsigned long int *current_sp;
volatile unsigned long int *psp_value;

void enable_interrupts()
{
	// Enables all interrupts
	__asm volatile("CPSIE I");
}

void disable_interrupts()
{
	// Disables all interrupts
	__asm volatile("CPSID I");
}

void trigger_pendSV(void)
{
    __asm volatile(".thumb");                                       //Set to thumb mode explicitly

    __asm volatile("LDR     r0, =0xE000ED04\n");                    // Load the address of ICSR into R0
    __asm volatile("LDR     r1, [r0]\n");                           // Load the current value of ICSR into R1
    __asm volatile("ORR     r1, r1, #0x10000000\n");                // Set the PENDSVSET bit (bit 28)
    __asm volatile("STR     r1, [r0]\n");                           // Store the updated value back into ICSR
}

void systick_handler(void)
{
    trigger_pendSV();
}

void start_scheduler(void *task)
{
    unsigned long int *_psp = (unsigned long int*)&(TCB[0]._sp);
    
    disable_interrupts();

    __asm volatile("MSR PSP, %0" : : "r" (_psp));                   // Set PSP to the address of the task's stack
    __asm volatile("MRS R1, CONTROL");                              //Storing CONTROL register values to R0
    __asm volatile("ORR R1, R1, #2");                               //Setting SPSEL to 1 to use PSP instead of MSP
    __asm volatile("MSR CONTROL, R1");
    enable_interrupts();
    __asm volatile("BX R0");
}

void create_task(void* task)
{
    disable_interrupts();

    unsigned long int* __psp;
    TCB[task_count].next = &TCB[0];
    if(task_count != 0)
    {
        TCB[task_count - 1].next = &TCB[task_count];
    }

    if(task_count < (MAX_TASK_COUNT - 1))
    {
        __psp = (unsigned long int *)&TCB[task_count + 1].stack;
        --__psp;                                                        //To move PSP to the beginning of stack
        --__psp;                                                        //To move PSP to the beginning of stack

        *(--__psp) = (1U << 24);                                        //Set thumb bit of XPSR;
        *(--__psp) = (unsigned long int*)task;                          //Setting the PC 
        *(--__psp) = 0xFFFFFFFDu;                                       //Loading LR with EXC_RETURN to return to Thread using PSP 

        *(--__psp) = 0x12121212u;                                       // Dummy R12
        *(--__psp) = 0x03030303u;                                       // Dummy R3
        *(--__psp) = 0x02020202u;                                       // Dummy R2
        *(--__psp) = 0x01010101u;                                       // Dummy R1
        *(--__psp) = 0x00000000u;                                       // Dummy R0
        *(--__psp) = 0x11111111u;                                       // Dummy R11
        *(--__psp) = 0x10101010u;                                       // Dummy R10
        *(--__psp) = 0x09090909u;                                       // Dummy R9
        *(--__psp) = 0x08080808u;                                       // Dummy R8
        *(--__psp) = 0x07070707u;                                       // Dummy R7
        *(--__psp) = 0x06060606u;                                       // Dummy R6
        *(--__psp) = 0x05050505u;                                       // Dummy R5
        *(--__psp) = 0x04040404u;                                       // Dummy R4
        
        TCB[task_count]._sp = __psp;
        task_count++;
    }
    enable_interrupts();
} 

void pendSV_handler()
{
    __asm volatile("CPSID I");
    __asm volatile("PUSH {LR}");                                        //Saving PR in the main stack to use this to exit 
    __asm volatile("mrs r0, PSP");                                      //Getting current PSP
    __asm volatile("stmdb r0!, {r4-r11}");                              //Storing r4-r11 of the previous task to its stack using its psp


    if(task_count <= 0)//No task or 1 task
    {
        //Do nothing
    }
    else
    {
        __asm volatile(".thumb");                                       //Set to thumb mode explicitly
        __asm volatile(
            // "MRS r0, PSP\n"                                          // Move the value of MSP into register r0
            "MOV %0, r0"                                                // Move the value of r0 (MSP) to the C variable psp_value
            : "=r" (psp_value)                                          // Output constraint: store r0 into psp_value
            :                                                           // No input operands
            : "r0"                                                      // Clobber list: r0 is modified
        );

        TCB[current_task_num]._sp = (uint32_t*) psp_value;              //Storing the PSP of the previous task to it's TCB
        current_task_num = (current_task_num + 1) % task_count;
        current_sp = TCB[current_task_num]._sp;                         //Storing the new task's stack pointer to current_sp variable

        __asm volatile("ldr r1, =current_sp");                          // Load the address of current_sp into r1
        __asm volatile("ldr r0, [r1]");                                 // Dereference current_sp and load the value into r0; i.e. psp is loaded into the r0 register
        __asm volatile("msr PSP, r0");                                  //Loaded PSP with r0

        __asm volatile("ldmia.w	r0!, {r4-r11}");                        //Load  registers from the PSP stack (r4 to r11)
        __asm volatile("msr PSP, r0");                                  //Loaded PSP with r0              
        __asm volatile("POP {LR}");
    	__asm volatile("CPSIE I");                                      //Enabling interrupt
        __asm volatile("bx	lr");
    }
}