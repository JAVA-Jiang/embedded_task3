/****************************************************************************

                 IoT & I-System Research Center, CST, BUPT

            Project : Demo
              Group : Operating System
               Item : Demo for embedded project
               File : eduAppRoot.c
        Description : a two tasks DEMO App Root file

****************************************************************************/

/*
modification history 
--------------------
10a,21apr2001, jkuang ,Original Version
10b,29jul2018, jkuang, update AppRoot.c to eduAppRoot.c
*/

#include <vxWorks.h>
#include <msgQLib.h>
#include <taskLib.h>
#include <logLib.h>
#include "eduappcfg.h"


SEM_ID syncBSem;                          /* semaphorefor task startup sync */
/***************************************************************************/
/*                                                                         */
/*       Function : Init_Application_Tasks                                 */
/*                - application initializtion                              */
/*                                                                         */
/*           Type : internal used function                                 */
/*      Called by : root()                                                 */
/*    Input Param : none                                                   */
/*         Output :                                                        */
/*                  0 -> OK                                                */     
/*                 -1 -> ERROR                                             */     
/*                                                                         */
/***************************************************************************/
int Init_Application_Tasks(void)
{
   /*initial logTask*/
   logInit(1,50);
   taskPrioritySet(taskNameToId("tLogTask"),160);
   
   /*sync semaphore create for task startup sync*/
   syncBSem = semBCreate(SEM_EMPTY,SEM_Q_PRIORITY);

   TASK_MBOX(APP_TC_nm) = msgQCreate(OC_nm_QMAXMSG,
                                     OC_nm_QMSGLEN, OC_nm_QFLAG);
   /*----------------------------------------------------------------------*/
   /* EXAMPLE:                                                             */
   /*    {                                                                 */
   /*      int args[10] = {OC_AppOne_TARGS};                               */
   /*      if(Create_AppOne_Task(&args[0]))                                */
   /*      {                                                               */
   /*	     return -1;                                                    */
   /*      }                                                               */
   /*    };                                                                */
   /*----------------------------------------------------------------------*/

   /* a two task demo --------------------------------------------------------*/
   {
     int args[10] = {OC_nm_TARGS};
     if(Create_NM_Task(&args[0]))
     {
       return -1;
     }
#ifdef OC_DEBUG
     ConsoleOut("Demo NM Task Initialed!\n");
#endif
   };

   {
     int args[10] = {OC_Sender_TARGS};
     if(Create_Sender_Task(&args[0]))
     {
       return -1;
     }
#ifdef OC_DEBUG
     ConsoleOut("Demo SENDER Task Initialed!\n");
#endif
   };

   {
     int args[10] = {OC_Receiver_TARGS};
     if(Create_Receiver_Task(&args[0]))
     {
       return -1;
     }
#ifdef OC_DEBUG
     ConsoleOut("Demo RECEIVER Task Initialed!\n");
#endif
   };

   /* a two task demo --------------------------------------------------------*/
  
  return 0;
}
