#include "includes.h"

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        3        /* Number of identical tasks                          */

#define C_TASK0 1
#define P_TASK0 3
#define C_TASK1 3
#define P_TASK1 6
#define C_TASK2 4
#define P_TASK2 9

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */

void  Task0(void *data); 
void  Task1(void *data);                      /* Function prototypes of tasks                  */
void  Task2(void *data);
static  void  TaskStartCreateTasks(void);

void  main (void)
{
    print_log_iter = 0;
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    OSTaskCreate(Task0, (void *)0, &TaskStk[0][TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}

static  void  TaskStartCreateTasks (void)
{
    OSTaskCreate(Task1, (void *)0, &TaskStk[1][TASK_STK_SIZE - 1], 1);
    // OSTaskCreate(Task2, (void *)0, &TaskStk[2][TASK_STK_SIZE - 1], 2);  
}

void  Task0 (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

    INT16S key;
    int start;      /*the start time    */
    int end;        /*the end time      */
    int toDelay;
    int i;

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    TaskStartCreateTasks();

    pdata = pdata;
    start = OSTimeGet();
    OSTCBCur->compTime = C_TASK0;
    OSTCBCur->realCompTime = C_TASK0;
    OSTCBCur->period = P_TASK0;
    OSTCBCur->deadline = P_TASK0;
    while(1){ /* the task who has highest priority print the log */
        if(print_log_iter >= PRINT_LOG_NUMS){
            for(i=0; i<PRINT_LOG_NUMS; i++){
                if(LOG[i][4] == -1){
                    printf("%d   ", LOG[i][0]);
                    printf((LOG[i][1] == 0) ? "Complete   " : "Preemptive   ");
                    printf("%d   ", LOG[i][2]);
                    printf("%d   \n", LOG[i][3]);
                }else{
                    printf("%d    ", LOG[i][0]);
                    printf("task%d deadline missed!\n", LOG[i][4]);
                }
            }
            PC_DOSReturn(); /* exit after printing */
        }
        
        while(OSTCBCur->compTime > 0){}
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start += OSTCBCur->period;

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B)                               /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
        }
        if(toDelay > 0)
            OSTimeDly(toDelay);
        OSTCBCur->compTime = C_TASK0;
    }
}

void  Task1 (void *pdata)
{
    int start;      /*the start time    */
    int end;        /*the end time      */
    int toDelay;
    pdata = pdata;

    start = OSTimeGet();
    OSTCBCur->compTime = C_TASK1;
    OSTCBCur->realCompTime = C_TASK1;
    OSTCBCur->period = P_TASK1;
    OSTCBCur->deadline = P_TASK1;
    while(1){
        while(OSTCBCur->compTime > 0){}
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start += OSTCBCur->period;
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
    pdata = pdata;

    start = OSTimeGet();
    OSTCBCur->compTime = C_TASK2;
    OSTCBCur->realCompTime = C_TASK2;
    OSTCBCur->period = P_TASK2;
    OSTCBCur->deadline = P_TASK2;
    while(1){
        while(OSTCBCur->compTime > 0){}
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start += OSTCBCur->period;
        if(toDelay > 0)
            OSTimeDly(toDelay);
        OSTCBCur->compTime = C_TASK2;
    }
}
