/****************************************************************************

                 IoT & I-System Research Center, CST, BUPT

            Project : All
              Group : Operating System
               Item : OS support system software
               File : EduRoot.c
        Description : system root function file

****************************************************************************/

/*
modification history 
--------------------
10a,21apr2001, jkuang ,Original Version
10b,29jul2018, jkuang, update root.c to EduRoot.c
*/

#include <taskLib.h>
#include "eOSedu.h"

/***************************************************************************/
extern int Init_Application_Tasks(void);

LOCAL void Task_Idle(void);
LOCAL void Task_System_Manage(int sCommand);

/*-------------------------------------------------------------------------*/
/*   system idel task Parameter                                            */
/*-------------------------------------------------------------------------*/
#define OC_idle_TNAME      "idle"
#define OC_idle_TPRIO      254
#define OC_idle_TOPTION    0
#define OC_idle_TSTACK     0x200
#define OC_idle_TARGS      OC_DEFAULT_TARGS

#define Create_idle_Task(args) \
                               _Create_Task(\
                               OC_TC_idle,\
                               OC_idle_TNAME,\
                               OC_idle_TPRIO,\
                               OC_idle_TOPTION,\
                               OC_idle_TSTACK,\
                               args,\
                               Task_Idle)

#ifdef SYSMANTASK
/*-------------------------------------------------------------------------*/
/*    System Manager task Parameter                                        */
/*-------------------------------------------------------------------------*/
#define OC_SM_TNAME        "SysMan"
#define OC_SM_TPRIO        200
#define OC_SM_TOPTION      0
#define OC_SM_TSTACK       0x1000
#define OC_SM_TARGS        1,2,3,0,0,0,0,0,0,0

#define OC_SM_QMAXMSG      50
#define OC_SM_QMSGLEN      OC_DEFAULT_QMSGLEN
#define OC_SM_QFLAG        (MSG_Q_FIFO)

#define Create_Sysman_Task(args)  \
                               _Create_Task(\
                               OC_TC_SYSMAN,\
                               OC_SM_TNAME,\
                               OC_SM_TPRIO,\
                               OC_SM_TOPTION,\
                               OC_SM_TSTACK,\
                               args,\
                               Task_System_Manage)
#endif /* SYSMANTASK */

/***************************************************************************/
/*      Global datas                                                       */
/***************************************************************************/
UINT32 idle_counter;                     /* idle task running counter      */
UINT32 LocalModuleNo;                    /* module no.# of local processor */

UINT8  smanSuspendErrTask;

UINT32 uCPU;                              /* recent CPU util               */
UINT32 usBufPool;                         /* recent s-buffer pool util     */
UINT32 ulBufPool;                         /* recent l-buffer pool util     */
UINT32 uOSTimer;                          /* recent OS timer util          */


/**************************************************************************/
/*      Function  : root  -  root task                                    */
/*                  system root task initializtion all system             */
/**************************************************************************/
int root(void)
{
   int tID;
   
   /*---------------------------------------------------------------------*/
   /*   Init system                                                       */
   /*---------------------------------------------------------------------*/
   if(Init_eOS_system(OC_MM_SBUFNUITNUM, OC_MM_SBUFUNITSIZE,
                      OC_MM_LBUFNUITNUM, OC_MM_LBUFUNITSIZE))
     return -1;
 
#ifdef SYSMANTASK
   /*---------------------------------------------------------------------*/
   /*      Creat System manager Task                                      */
   /*---------------------------------------------------------------------*/
   {
     int args[10] = {OC_SM_TARGS};
     if(Create_Sysman_Task(&args[0]))
     {
       return -1;
     }
#ifdef OC_DEBUG
     ConsoleOut("SysMem Task Initialed!\n");
#endif
   };
#endif /* SYSMANTASK */

   /*---------------------------------------------------------------------*/
   /*   start new idle task                                               */
   /*---------------------------------------------------------------------*/
   tID = taskNameToId(OC_idle_TNAME);
   if(tID > 0)
      taskDelete(tID);
   {
     int args[10] = {OC_idle_TARGS};
     if(Create_idle_Task(&args[0]))
     {
       return -1;
     }
#ifdef OC_DEBUG
     ConsoleOut("Idle Task Initialed!\n");
#endif
   };
 
   /*-----------------------------------------------------------------------
     User task initial process 
   -----------------------------------------------------------------------*/
   if(Init_Application_Tasks())
   {
     return -1;
   }
#ifdef OC_DEBUG
   ConsoleOut("Application Tasks Initialed!\n");
#endif
   
   return 0;
}

/***************************************************************************/
/*                                                                         */
/*      Function : _Create_Task                                            */
/*               - create a task and start it                              */
/*                                                                         */
/*          Type : OS level system call                                    */
/*   Input param :                                                         */
/*               t_code - code of task to create (defined in "syscfg.h")   */
/*               t_name - name of task                                     */
/*               t_prio - priority of task                                 */
/*             t_option - option word of task                              */
/*              t_stack - stack size of task                               */
/*               t_regs - 10 arguments array pointer of task               */
/*         t_entry_addr - task's start entry address                       */
/*        Output :                                                         */
/*           success - 0                                                   */
/*              fail - an error code                                       */
/*                                                                         */
/***************************************************************************/
UINT32 _Create_Task(UINT32  t_code,
                    char *  t_name,
                    UINT32  t_prio,
                    UINT32  t_option,
                    UINT32  t_stack,
                    int*    t_args,
                    void (* t_entry_addr)())
{
   int tID;

   /*----------------------------------------------------------------------*/
   /*          Create task                                                 */
   /*----------------------------------------------------------------------*/
   tID = taskSpawn(t_name, t_prio, t_option, t_stack, (FUNCPTR)t_entry_addr,
                   t_args[0],t_args[1],t_args[2],t_args[3],t_args[4],
                   t_args[5],t_args[6],t_args[7],t_args[8],t_args[9]);
   if (tID < 0)
   {
      Error_Alarm(PSCC_TCREATE,errno);
      return EC_RT_TCREATE|t_code;
   }

   /*------------------------------------------------------------------------
     Initial Task_CT record of specified task
   ------------------------------------------------------------------------*/
   TASK_ID(t_code)       = tID;
   TASK_STATE(t_code)    = TRS_NOSTARTED;
   TASK_INSTDATA(t_code) = NULL;
   return 0;
}


/***************************************************************************/
/*                                                                         */
/*      Function : _Restart_Task                                           */
/*                - Restart a task (only called by system manager)         */
/*                                                                         */
/*          Type : OS level system call                                    */
/*   Input param :                                                         */
/*               t_code - code of task to be restart                       */
/*               t_regs - 10 arguments array pointer of task               */
/*        Output :                                                         */
/*              success - return 0                                         */
/*                 fail - return a value not 0                             */
/*                                                                         */
/***************************************************************************/
UINT32 _Restart_Task(UINT32 t_code, int* t_args)
{
   UINT32 error_code;
   
   if((error_code = taskRestart(TASK_ID(t_code))))
   {
     Error_Alarm(errno,0);
     return EC_RT_TRESTART|t_code;
   }
   return 0;
}


/***************************************************************************/
/*                                                                         */
/*      Function   : Task_Idle                                             */
/*                 - entry of idle task                                    */
/*                                                                         */
/*      Task name  : idle                                                  */
/*      Startup Paramter:     none                                         */
/*                                                                         */
/***************************************************************************/
LOCAL void Task_Idle(void)
{
  int i;

  TASK_STATE(OC_TC_idle) = TRS_RUNNING;
  idle_counter = 0;
  FOREVER
  {
    for (i=0; i<100; i++);
    idle_counter++;             /* increase idle task counter            */
/*      printf("idle_counter = %d\n",idle_counter);
*/
  }
}

#ifdef SYSMANTASK
/***************************************************************************/
/*                                                                         */
/*      Function   : Task_System_Manage   - System Management Task entry   */
/*                                                                         */
/*      Task name  : Sman                                                  */
/*      Startup Paramter:                                                  */
/*                sCommand -  0 : delete Task                              */
/*                            1 : first task startup initialize            */
/*                            2 : re-initialize when task is running       */
/*                                                                         */
/***************************************************************************/
LOCAL void Task_System_Manage(int sCommand)
{
   int msglen;
   char aMsg[OC_DEFAULT_QMSGLEN];
   UINT32 cnt = 0;

   if(!(TASK_MBOX(OC_TC_SYSMAN) = msgQCreate(OC_SM_QMAXMSG,OC_SM_QMSGLEN,OC_SM_QFLAG)))
   {
     Error_Alarm(PSCC_QCREATE,errno);
     ConsoleOut("SysManager Task exit!");
     taskDelete(TASK_ID(OC_TC_SYSMAN));
   };
   
   smanSuspendErrTask = OC_SUSPEND_ERR_TASK;
   idle_counter = 0;                      /* clear idle counter            */
   TASK_STATE(OC_TC_SYSMAN) = TRS_RUNNING;           /*set sysman task running flag   */

   FOREVER
   {
     msglen = msgQReceive(TASK_MBOX(OC_TC_SYSMAN),&aMsg[0],OC_DEFAULT_QMSGLEN,WAIT_FOREVER);
     if(msglen < 0)
     { 
       Error_Alarm(PSCC_QRECEIVE, errno);
       /*
       printf("\none second TIMEOUT! errno:%x(%d)",errno,cnt);
       printErrno(errno);*/
       cnt++;
     }
   }
}
#endif

/***************************************************************************/
/*                                                                         */
/*      Function : ErrAlarm                                                */
/*               - System error alarming.                                  */
/*                                                                         */
/*          Type : OS level system call                                    */
/*   Input param :                                                         */
/*        errcode - system call return errcode                             */
/*         param1 - alarming parameter 1                                   */
/*         param2 - alarming parameter 2                                   */
/*        Output :                                                         */
/*                none                                                     */
/*                                                                         */
/***************************************************************************/
void ErrAlarm(unsigned errcode,unsigned Param1,unsigned Param2)
{
  char tName[15];
  char* tMptr = &tName[0];
  int  tID = taskIdSelf();
  
  if(smanSuspendErrTask)
    taskSuspend(tID);

  if(!(tMptr = taskName(tID)))
  {
    sprintf(tMptr,"N/A");
  }
/*  printf("ALARM TN:%s EC:0x%x PM1:%d PM2:%d\n",tMptr,errcode,Param1,Param2); */
  logMsg("ALARM TN:%s EC:0x%x PM1:%d PM2:%d\n",tMptr,errcode,Param1,Param2,0,0,0);
}



