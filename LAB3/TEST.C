#include "includes.h"

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        3        /* Number of identical tasks                          */

OS_EVENT *R1, *R2;
INT8U err1, err2;

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_TCB        *ptcb;

void  Task1(void *data); 
void  Task2(void *data);                      /* Function prototypes of tasks                  */
void  Task3(void *data);
static  void  TaskStartCreateTasks(void);

void  main (void)
{
    print_log_iter = 0;
    mutex_owner = -1;
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    R1 = OSMutexCreate(1, &err1);
    R2 = OSMutexCreate(2, &err2);

    OSTaskCreate(Task1, (void *)0, &TaskStk[0][TASK_STK_SIZE - 1], 3); 

    OSStart();                                             /* Start multitasking                       */
}

static  void  TaskStartCreateTasks (void)
{
    OSTaskCreate(Task2, (void *)0, &TaskStk[1][TASK_STK_SIZE - 1], 4); 
    OSTaskCreate(Task3, (void *)0, &TaskStk[2][TASK_STK_SIZE - 1], 5); 
}

void  Task1 (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

    int start;      /*the start time    */
    int end;        /*the end time      */
    int toDelay;
    int i;
    int lockR1, lockR2;

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    TaskStartCreateTasks();
    pdata = pdata;

    lockR1 = 0;
    lockR2 = 0;
    if(A_TASK1 != 0)
        OSTimeDly(A_TASK1);
    start = OSTimeGet();
    while(1){ /* the task who has highest priority print the log */
        if(print_log_iter >= PRINT_LOG_NUMS){
            for(i=0; i<PRINT_LOG_NUMS; i++){
                if(LOG[i][4] == -1){ /* preemptive/complete log */
                    printf("%d   ", LOG[i][0]);
                    printf((LOG[i][1] == 0) ? "Complete   " : "Preemptive   ");
                    printf("%d   ", LOG[i][2]);
                    printf("%d   \n", LOG[i][3]);
                }else if(LOG[i][4] == -2 || LOG[i][4] == -3){ /* block log */
                    printf("%d   ", LOG[i][0]);
                    printf((LOG[i][4] == -2) ? "lock   " : "unlock   "); /* -2: lock, -3: unlock */
                    printf("R%d  (Prio=", LOG[i][1]);
                    printf("%d changes to=", LOG[i][2]);
                    printf("%d)\n", LOG[i][3]);
                }else{ /* deadline miss log */
                    printf("%d    ", LOG[i][0]);
                    printf("task%d deadline missed!\n", LOG[i][4]);
                }
            }
            PC_DOSReturn(); /* exit after printing */
        }

        while(OSTCBCur->compTime > 0){
            if(OSTCBCur->compTime == 4 && !lockR1){
                lockR1 = 1;
                OSMutexPend(R1, 0, &err1);
            }
            if(OSTCBCur->compTime == 2 && !lockR2){
                lockR2 = 1;
                OSMutexPend(R2, 0, &err2);
            }
        }
        OSMutexPost(R2);
        lockR2 = 0;
        OSMutexPost(R1);
        lockR1 = 0;
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start += OSTCBCur->period;
        // if (PC_GetKey(&key) == TRUE) {
        //     if (key == 0x1B)          
        //         PC_DOSReturn();       
        // }
        if(toDelay > 0)
            OSTimeDly(toDelay);
        OSTCBCur->compTime = C_TASK1;
    }
}

void  Task2 (void *pdata)
{
    int start;      /*the start time    */
    int end;        /*the end time      */
    int toDelay;
    int lockR2;
    pdata = pdata;

    lockR2 = 0;
    if(A_TASK2 != 0)
        OSTimeDly(A_TASK2);
    start = OSTimeGet();
    while(1){
        while(OSTCBCur->compTime > 0){
            if(OSTCBCur->compTime == 4 && !lockR2){
                lockR2 = 1;
                OSMutexPend(R2, 0, &err2);
            }
        }
        OSMutexPost(R2);
        lockR2 = 0;
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start += OSTCBCur->period;
        if(toDelay > 0)
            OSTimeDly(toDelay);
        OSTCBCur->compTime = C_TASK2;
    }
}
void  Task3 (void *pdata)
{
    int start;      /*the start time    */
    int end;        /*the end time      */
    int toDelay;
    int lockR1;
    pdata = pdata;

    lockR1 = 0;
    if(A_TASK3 != 0)
        OSTimeDly(A_TASK3);
    start = OSTimeGet();
    while(1){
        while(OSTCBCur->compTime > 0){
            if(OSTCBCur->compTime == 7 && !lockR1){
                lockR1 = 1;
                OSMutexPend(R1, 0, &err1);
            }
        }
        OSMutexPost(R1);
        lockR1 = 0;
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start += OSTCBCur->period;
        if(toDelay > 0)
            OSTimeDly(toDelay);
        OSTCBCur->compTime = C_TASK3;
    }
}
