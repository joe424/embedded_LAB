#include "includes.h"

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */

OS_STK        Task1Stk[TASK_STK_SIZE];                /* Task #1    task stack                         */
OS_STK        Task2Stk[TASK_STK_SIZE];                /* Task #2    task stack                         */
OS_STK        Task3Stk[TASK_STK_SIZE];                /* Task #3    task stack                         */

        void  Task1(void *data);
        void  Task2(void *data);
        void  Task3(void *data);
static  void  TaskStartCreateTasks(void);

void main (void)
{
    PC_DispClrScr(DISP_FGND_WHITE);                        /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    OSTaskCreate(Task1, (void *)0, &Task1Stk[TASK_STK_SIZE - 1], 0);

    OSStart();                                             /* Start multitasking                       */
}

static  void  TaskStartCreateTasks (void)
{
    OSTaskCreate(Task2, (void *)0, &Task2Stk[TASK_STK_SIZE - 1], 1);
    // OSTaskCreate(Task3, (void *)0, &Task3Stk[TASK_STK_SIZE - 1], 2);
}

void  Task1 (void *data)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    INT16S     key;
    int start, end, toDelay, c = 1, p = 3, tmpc, printlog = 0;
    OS_TCB    *ptcb;

    data = data;                                         /* Prevent compiler warning                 */

    OS_ENTER_CRITICAL();                                   /* Install uC/OS-II's clock tick ISR        */
    PC_VectSet(0x08, OSTickISR);
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();
    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    TaskStartCreateTasks();                                /* Create all other tasks                   */

    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = c;
    OSTCBCur->period = p;
    OS_EXIT_CRITICAL();
    start = OSTimeGet();

    while(1)
    {
        do{
            OS_ENTER_CRITICAL();
            tmpc = OSTCBCur->compTime;
            OS_EXIT_CRITICAL();
        }while(tmpc > 0);

        OS_ENTER_CRITICAL();
        if(*(OSTCBCur->ptr_count) >= 1000){
            int i;
            for(i=0; i<*(OSTCBCur->ptr_count); i++){
                printf("%lu    %s    %hhu    %hhu\n", 
                        OSTCBCur->ptr_struct[i].cur_time,
                        OSTCBCur->ptr_struct[i].event,
                        OSTCBCur->ptr_struct[i].from,
                        OSTCBCur->ptr_struct[i].to);
            }
            *(OSTCBCur->ptr_count) = 0;
            printlog = 1;
        }
        OS_EXIT_CRITICAL();

        end = OSTimeGet();
        toDelay = p - (end - start);
        if(toDelay < 0 && printlog){
            printf("(p1) deadline missed cause by printlog!\n");
            printlog = 0;
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = OSTimeGet();
        }else if(toDelay < 0 && !printlog){
            printf("(p1) deadline missed!\n");
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = OSTimeGet();
        }else{
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = start + p;
            OSTimeDly(toDelay);
        }
    }
}

void Task2(void *data)
{
    int start, end, toDelay, c = 3, p = 6, tmpc, printlog = 0;

    data = data;

    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = c;
    OSTCBCur->period = p;
    OS_EXIT_CRITICAL();
    start = OSTimeGet();

    while(1)
    {
        do{
            OS_ENTER_CRITICAL();
            tmpc = OSTCBCur->compTime;
            OS_EXIT_CRITICAL();
        }while(tmpc > 0);

        OS_ENTER_CRITICAL();
        if(*(OSTCBCur->ptr_count) >= 1000){
            int i;
            for(i=0; i<*(OSTCBCur->ptr_count); i++){
                printf("%lu    %s    %hhu    %hhu\n", 
                        OSTCBCur->ptr_struct[i].cur_time,
                        OSTCBCur->ptr_struct[i].event,
                        OSTCBCur->ptr_struct[i].from,
                        OSTCBCur->ptr_struct[i].to);
            }
            *(OSTCBCur->ptr_count) = 0;
            printlog = 1;
        }
        OS_EXIT_CRITICAL();

        end = OSTimeGet();
        toDelay = p - (end - start);
        if(toDelay < 0 && printlog){
            printf("(p2) deadline missed cause by printlog!\n");
            printlog = 0;
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = OSTimeGet();
        }else if(toDelay < 0 && !printlog){
            printf("(p2) deadline missed!\n");
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = OSTimeGet();
        }else{
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = start + p;
            OSTimeDly(toDelay);
        }
    }
}

void Task3(void *data)
{
    int start, end, toDelay, c = 4, p = 9, tmpc, printlog = 0;

    data = data;

    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = c;
    OSTCBCur->period = p;
    OS_EXIT_CRITICAL();
    start = OSTimeGet();

    while(1)
    {
        do{
            OS_ENTER_CRITICAL();
            tmpc = OSTCBCur->compTime;
            // printf("1: %d %d %hhu %hhu\n", tmpc, OSTCBCur->period, OSTCBCur->OSTCBPrio, OSTCBCur->OSTCBStat);
            OS_EXIT_CRITICAL();
        }while(tmpc > 0);

        OS_ENTER_CRITICAL();
        if(*(OSTCBCur->ptr_count) >= 1000){
            int i;
            for(i=0; i<*(OSTCBCur->ptr_count); i++){
                printf("%lu    %s    %hhu    %hhu\n", 
                        OSTCBCur->ptr_struct[i].cur_time,
                        OSTCBCur->ptr_struct[i].event,
                        OSTCBCur->ptr_struct[i].from,
                        OSTCBCur->ptr_struct[i].to);
            }
            *(OSTCBCur->ptr_count) = 0;
            printlog = 1;
        }
        OS_EXIT_CRITICAL();

        end = OSTimeGet();
        toDelay = p - (end - start);
        if(toDelay < 0 && printlog){
            printf("(p3) deadline missed cause by printlog!\n");
            printlog = 0;
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = OSTimeGet();
        }else if(toDelay < 0 && !printlog){
            printf("(p3) deadline missed!\n");
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = OSTimeGet();
        }else{
            OS_ENTER_CRITICAL();
            OSTCBCur->compTime = c;
            OS_EXIT_CRITICAL();
            start = start + p;
            OSTimeDly(toDelay);
        }
    }
}