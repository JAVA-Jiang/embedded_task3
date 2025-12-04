/****************************************************************************

                 IoT & I-System Research Center, CST, BUPT

            Project : Demo
              Group : Operating System
               Item : Demo for embedded project
               File : eduAppcfg.h
        Description : application config header file

****************************************************************************/

/*
modification history 
--------------------
10a,21apr2001, jkuang ,Original Version
10b,29jul2018, jkuang, update appcfg.h to eduAppcfg.h
*/

#ifndef     __APPCFG_H__

#define     __APPCFG_H__

#include "eOSedu.h"

/***************************************************************************/
/*    application task code                                                */
/*                                                                         */
/*    NOTE: The application task code range should between from            */
/*          OC_TC_1stAPP to OC_TC_num - 1, defined in "syscfg.h"           */
/* EXAMPLE:                                                                */
/*          #define APP_TC_app_one  OC_TC_1stAPP                           */
/*          #define APP_TC_app_two  OC_TC_1stAPP + 1                       */
/***************************************************************************/
/* a two task demo --------------------------------------------------------*/
#define APP_TC_sender     OC_TC_1stAPP
#define APP_TC_receiver   OC_TC_1stAPP + 1
#define APP_TC_nm         OC_TC_1stAPP + 2
/* a two task demo --------------------------------------------------------*/



/***************************************************************************/
/*   Application task parameters definations                               */
/*                                                                         */
/*   OC_xx_TNAME    - Task Name string                                     */
/*   OC_xx_TPRIO    - Task priority                                        */
/*   OC_xx_TOPTION  - Task option word                                     */
/*   OC_xx_TSTACK   - Task STACK size (bytes)                              */
/*   OC_xx_TARGS    - Task input arguments                                 */
/*                                                                         */
/*   OC_xx_QMAXMSG  - message box Q max message number                     */
/*   OC_xx_QMSGLEN  - message box Q length(bytes) of every message         */
/*   OC_xx_QFLAG    - message box Q flag                                   */ 
/***************************************************************************/
/*   EXAMPLE:                                                              */
/*-------------------------------------------------------------------------*/
/*            App_one Parameters                                           */
/*-------------------------------------------------------------------------*/
/*            #define OC_AppOne_TNAME        "AppOne"                      */
/*            #define OC_AppOne_TPRIO        150                           */
/*            #define OC_AppOne_TOPTION      0                             */
/*            #define OC_AppOne_TSTACK       0x1000                        */
/*            #define OC_AppOne_TARGS        1,2,3,0,0,0,0,0,0,0           */
/*                                                                         */
/*            #define OC_AppOne_QMAXMSG      40                            */
/*            #define OC_AppOne_QMSGLEN      OC_DEFAULT_QMSGLEN            */
/*            #define OC_AppOne_QFLAG        (MSG_Q_FIFO)                  */
/***************************************************************************/
/* a two task demo --------------------------------------------------------*/
/*-----------------------------------------------------------*/
/*   sender task params                                      */
/*-----------------------------------------------------------*/
#define OC_Sender_TNAME        "Sender"
#define OC_Sender_TPRIO        150
#define OC_Sender_TOPTION      0
#define OC_Sender_TSTACK       0x1000
#define OC_Sender_TARGS        OC_DEFAULT_TARGS

#define OC_Sender_QMAXMSG      20
#define OC_Sender_QMSGLEN      OC_DEFAULT_QMSGLEN
#define OC_Sender_QFLAG        (MSG_Q_FIFO)
/*-----------------------------------------------------------*/
/*   receiver task params                                      */
/*-----------------------------------------------------------*/
#define OC_Receiver_TNAME      "Receiver"
#define OC_Receiver_TPRIO      140
#define OC_Receiver_TOPTION    0
#define OC_Receiver_TSTACK     0x1000
#define OC_Receiver_TARGS      OC_DEFAULT_TARGS

#define OC_Receiver_QMAXMSG    20
#define OC_Receiver_QMSGLEN    OC_DEFAULT_QMSGLEN
#define OC_Receiver_QFLAG      (MSG_Q_FIFO)
/*-----------------------------------------------------------*/
/*   net manager task params                                      */
/*-----------------------------------------------------------*/
#define OC_nm_TNAME             "NM"
#define OC_nm_TPRIO             161
#define OC_nm_TOPTION           0
#define OC_nm_TSTACK            0x1000
#define OC_nm_TARGS             OC_DEFAULT_TARGS

#define OC_nm_QMAXMSG           20
#define OC_nm_QMSGLEN           OC_DEFAULT_QMSGLEN
#define OC_nm_QFLAG             (MSG_Q_FIFO)

/* a two task demo --------------------------------------------------------*/



/***************************************************************************/
/*      creation macro for every specified task                            */
/*                                                                         */
/*   EXAMPLE:                                                              */
/*            #define Create_AppOne_Task(args) \                           */
/*                                             _Create_Task(\              */
/*                                             APP_TC_app_one,\            */
/*                                             OC_AppOne_TNAME,\           */
/*                                             OC_AppOne_TPRIO,\           */
/*                                             OC_AppOne_TOPTION,\         */
/*                                             OC_AppOne_TARGS,\           */
/*                                             args,\                      */
/*                                             App_One_task_entry)         */
/***************************************************************************/
/* a two task demo --------------------------------------------------------*/
/*-----------------------------------------------------------*/
/*   sender task creation macro                              */
/*-----------------------------------------------------------*/
#define Create_Sender_Task(args) \
                                  _Create_Task(\
                                  APP_TC_sender,\
                                  OC_Sender_TNAME,\
                                  OC_Sender_TPRIO,\
                                  OC_Sender_TOPTION,\
                                  OC_Sender_TSTACK,\
                                  args,\
                                  Sender_task)
/*-----------------------------------------------------------*/
/*   receiver task creation macro                            */
/*-----------------------------------------------------------*/
#define Create_Receiver_Task(args) \
                                  _Create_Task(\
                                  APP_TC_receiver,\
                                  OC_Receiver_TNAME,\
                                  OC_Receiver_TPRIO,\
                                  OC_Receiver_TOPTION,\
                                  OC_Receiver_TSTACK,\
                                  args,\
                                  Receiver_task)
/*-----------------------------------------------------------*/
/*   net manager task creation macro                            */
/*-----------------------------------------------------------*/
#define Create_NM_Task(args) \
                                  _Create_Task(\
                                  APP_TC_nm,\
                                  OC_nm_TNAME,\
                                  OC_nm_TPRIO,\
                                  OC_nm_TOPTION,\
                                  OC_nm_TSTACK,\
                                  args,\
                                  NM_task)
/* a two task demo --------------------------------------------------------*/


extern void Sender_task(void);
extern void Receiver_task(void);
extern void NM_task(void);

extern SEM_ID   syncBSem;

#endif
