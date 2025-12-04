/****************************************************************************

                 IoT & I-System Research Center, CST, BUPT

            Project : All
              Group : Operating System
               Item : OS support system software
               File : eOSedu.h
        Description : system startup header file

****************************************************************************/

/*
modification history
--------------------
10a,21apr2001, jkuang ,Original Version
10b,29jul2018, jkuang, update eos.h to eosedu.h
*/

#ifndef _E_OSEDU_H_

#define _E_OSEDU_H_

#include <vxworks.h>
#include <msgqlib.h>

/***************************************************************************/
/*   eOSeduMemory management definations:                                  */
/***************************************************************************/
/*-------------------------------------------------------------------------*/
/*    OC_MM_SBUFNUITNUM  : small constant-len buffer pool unit number      */
/*    OC_MM_SBUFUNITSIZE : small constant-len buffer pool unit size        */
/*    OC_MM_LBUFNUITNUM  : large constant-len buffer pool unit number      */
/*    OC_MM_LBUFUNITSIZE : large constant-len buffer pool unit size        */
/*-------------------------------------------------------------------------*/
#define OC_MM_SBUFNUITNUM      100
#define OC_MM_SBUFUNITSIZE     24
#define OC_MM_LBUFNUITNUM      50
#define OC_MM_LBUFUNITSIZE     256

/*===========================================================================
    Task Control
===========================================================================*/

/*-------------------------------------------------------------------------*/
/*    Define OS-level task runing status code                              */
/*-------------------------------------------------------------------------*/
#define TRS_NOCREATED      0x00
#define TRS_NOSTARTED      0x01
#define TRS_RUNNING        0x02
#define TRS_DELETED        0x03

/*-------------------------------------------------------------------------*/
/*      Define OS-level task startup command                               */
/*-------------------------------------------------------------------------*/
#define TSC_DELETE         0x00
#define TSC_INIT           0x01
#define TSC_REINIT         0x02

/*---------------------------------------------------------------------------
      Task Creat default tags
---------------------------------------------------------------------------*/
#define OC_DEFAULT_TARGS   0,0,0,0,0,0,0,0,0,0

/***************************************************************************/
/*    task code                                                            */
/***************************************************************************/
#define OC_TC_idle         0x01           /* idle deamon task              */
#ifdef SYSMANTASK
#define OC_TC_SYSMAN       0x02           /* system management task        */
#define OC_TC_1stAPP       0x03
#else
#define OC_TC_1stAPP       0x02           /* first application task code   */
#endif
/***************************************************************************/
/*      task control structure                                 */
/***************************************************************************/
typedef struct _TASK_CONFTAB
{
   UINT32        run_state;            /* running status of a task         */
   UINT32        task_ID;              /* IDentifier of task               */
   MSG_Q_ID      msgboxID;             /* message box ID of a task         */
   void*         taskDataPtr;          /* task/process data pointer        */
   int*          instDataPtr;          /* Instance Data Array Pointer      */
   UINT32        tMaxInstance;         /* Max instance number of task      */
}  TASK_CONFTAB;

extern TASK_CONFTAB  Task_CT[];          /* task configuration table       */
/*---------------------------------------------------------------------------
      Task_CT member process
---------------------------------------------------------------------------*/
#define TASK_STATE(tcode)   (Task_CT[tcode].run_state)
#define TASK_ID(tcode)      (Task_CT[tcode].task_ID) 
#define TASK_MBOX(tcode)    (Task_CT[tcode].msgboxID) 
#define TASK_DATA(tcode)    (Task_CT[tcode].taskDataPtr)
#define TASK_INSTDATA(tcode)(Task_CT[tcode].instDataPtr)
#define TASK_MAXINST(tcode) (Task_CT[tcode].tMaxInstance)


extern int Task_code_MAX(void);
/*===========================================================================
    SDL Task configruation
===========================================================================*/
/*---------------------------------------------------------------------------
      Task & msgQ configruation strycture for SDL tasks
---------------------------------------------------------------------------*/
typedef struct _SDL_TASK_MSQ_CFG
{
  int    tCode;       /* task code(index) in Task_CT. -1 -> End of table   */
  int    tPriority;   /* task priority. -1 -> default                      */
  int    tStack;      /* task stack size. -1 -> default                    */
  int    qMaxmsg;     /* msgQueue MAX message number. -1 -> default        */
  UINT32 qFlag;       /* msgQueue flag                                     */ 
} SDL_TASK_MSQ_CFG;

#define SDL_TASK_MSQ_CONFIG_END -1

extern SDL_TASK_MSQ_CFG SDLtaskmsgqcfgtbl[];

#define OC_SDL_TPRIO        128
#define OC_SDL_TOPTION      0
#define OC_SDL_TSTACK       4096
#define OC_SDL_TARGS        1,2,3,0,0,0,0,0,0,0

#define OC_SDL_QMAXMSG      20
#define OC_SDL_QMSGLEN      OC_DEFAULT_QMSGLEN
#define OC_SDL_QFLAG        (MSG_Q_FIFO)

/*===========================================================================
    Message
===========================================================================*/
#define OC_DEFAULT_QMSGLEN 20                   /* default message length  */

/***************************************************************************/
/*     General OS message format :                                         */
/*                                                                         */
/*     mType         type of message, can be one of following:             */
/*                      MT_ALARM       : General alarm message             */
/*                      MT_MAINTAIN    : Maintaince message                */
/*                      MT_SYSCON      : system console IO message         */
/*                      MT_OSTIMER     : from Timer task when timeout      */
/*                      MT_INTR        : Interrupt message from ISR        */
/*                      MT_DLINK       : net-datalink message              */
/*                      MT_APP         : Application layer message         */
/*                      MT_OTHER       : Other system message              */
/*     mLenTag       message length tag, 2 options:                        */
/*                      MLT_SHORTMSG   : message length <= 12 bytes        */
/*                      MLT_LONGMSG    : message length >  12 bytes        */
/*     mSendID       message sender ID(TaskID or Mudule N0.)               */
/*     mRecvID       message receiver ID(TaskID or Mudule N0.)             */
/*     mCode         message general code inside one CPU system            */
/*     mCTRL         message control word                                  */
/*     mSendInst     message sender instance ID                            */
/*     mRecvInst     message receiver instance ID                          */
/*     mLen          buffer length when mlenTag  == MLT_LONGMSG.           */
/*     pktptr        can be one of following options:                      */
/*                    . 4 byte message body, when mlenTag == MLT_SHORTMSG  */
/*                    . a pointer of a message body buffer, when mlenTag   */
/*                      == MLT_LONGMSG.                                    */
/***************************************************************************/
typedef struct _MESSAGE
{
    UINT8   mType;
    UINT8   mLenTag;
    UINT8   mSendID;
    UINT8   mRecvID;
    UINT16  mCode;
    UINT16  mCTRL;
    UINT16  mSendInst;
    UINT16  mRecvInst;
    UINT32  mLen;
    void *  pktptr;
} MESSAGE;

/*---------------------------------------------------------------------------
   mType
---------------------------------------------------------------------------*/
#define MT_ALARM        0x80
#define MT_MAINTAIN     0x81
#define MT_SYSCON       0x82
#define MT_OSTIMER      0x01
#define MT_INTR         0x02
#define MT_DLINK        0x03
#define MT_APP          0x04
#define MT_SDL          0x05

#define MT_OTHER        0x06

/*---------------------------------------------------------------------------
   mLenTag
---------------------------------------------------------------------------*/
#define MLT_SHORTMSG    0
#define MLT_LONGMSG     1


/***************************************************************************/
/*     OS Timer Messages structure                                         */
/*                                                                         */
/*     mType         must equal to MT_OSTIMER                              */
/*     mlenTag       must equal to MLT_SHORTMSG                            */
/*     mSubTp        can be one of following values:                       */
/*                      MST_OT_TMOUT   : message is timeout timer message  */
/*                      MST_OT_PERIOD  : message is periodic timer message */
/*     mRecvID       message receiver ID(TaskID or Mudule N0.)             */
/*     mCode         message general code inside one CPU system            */
/*     mCTRL         message control word                                  */
/*     userTag       2 byte tag set by user when timer create or start     */
/*     mRecvInst     message receiver instance ID                          */
/*     info          2 byte info set by user when timer created            */
/*     otID          ID of the timer which send the message                */
/*                                                                         */
/***************************************************************************/
typedef struct _OSTIMER_MESSAGE
{
    UINT8   mType;
    UINT8   mLenTag;
    UINT8   mSubTp;
    UINT8   mRecvID;
    UINT16  mCode;
    UINT16  mCTRL;
    UINT16  userTag;
    UINT16  otID;
    UINT32  mRecvInst;
    UINT32  info;
}  OSTIMER_MESSAGE;

#define OSTIMER_MSG(x) ((OSTIMER_MESSAGE *)&(x))

/*---------------------------------------------------------------------------
   mSubTp
---------------------------------------------------------------------------*/
#define MST_OT_TMOUT    0x00
#define MST_OT_PERIOD   0x01


/***************************************************************************/
/*      Interrupt Messages structure                                       */
/*                                                                         */
/*     mType         must equal to MT_INTR                                 */
/*     mlenTag       must equal to MLT_SHORTMSG                            */
/*     mSendID       message sender ID(TaskID or Mudule N0.)               */
/*     mRecvID       message receiver ID(TaskID or Mudule N0.)             */
/*     mCode         message general code inside one CPU system            */
/*     mCTRL         message control word                                  */
/*     portNo        physical port No.# which cause the interrupt          */
/*     mRecvInst     message receiver instance ID                          */
/*     mLen          buffer length when mlenTag  == MLT_LONGMSG.           */
/*     pktptr        can be one of following options:                      */
/*                    . 4 byte message body, when mlenTag == MLT_SHORTMSG  */
/*                    . a pointer of a message body buffer, when mlenTag   */
/*                      == MLT_LONGMSG                                     */
/*                                                                         */
/***************************************************************************/
typedef struct _INTR_MESSAGE
{
    UINT8   mType;
    UINT8   mLenTag;
    UINT8   mSendID;
    UINT8   mRecvID;
    UINT16  mCode;
    UINT16  mCTRL;
    UINT16  portNo;
    UINT16  mRecvInst;
    UINT32  mLen;
    void *  pktptr;
}   INTR_MESSAGE;

#define INTR_MSG(x) ((INTR_MESSAGE *)&(x))

/***************************************************************************/
/*     SDL & non-SDL Messages structure (same struct)                      */
/*                                                                         */
/*     mType         must equal to MT_SDL or MT_NONSDL                     */
/*     mlenTag       must equal to MLT_LONGMSG                             */
/*     lnkCode       not used, set to 0x00                                 */
/*     mSubtype      not used, set to 0x00                                 */
/*     pktSize       size of message body pointed by pktptr                */
/*     sTask         task code which send the message                      */
/*     rsvd          4 byte reserved                                       */
/*     pktptr        pointer of a message body buffer                      */
/*                                                                         */
/***************************************************************************/
typedef struct _SDL_MESSAGE
{
    unsigned char  mType;
    unsigned char  mlenTag;
    unsigned char  lnkCode;
    unsigned char  mSubtype;
    unsigned short pktSize;
    unsigned short sTask;
    unsigned long  rsvd, rsvd1;
    void *         pktptr;
}   SDL_MESSAGE;
typedef SDL_MESSAGE NONSDL_MESSAGE;

#define SDL_MSG(x)      ((SDL_MESSAGE *)&(x))
#define NONSDL_MSG(x)   ((NOSDL_MESSAGE *)&(x))



/***************************************************************************/
/*     Alarming Messages structure                                         */
/*                                                                         */
/*     mType         must equal to MT_ALARM                                */
/*     mlenTag       must equal to MLT_SHORTMSG                            */
/*                                                                         */
/***************************************************************************/


/***************************************************************************/
/*     Maintaince  Messages structure                                      */
/*                                                                         */
/*     mType         must equal to MT_MAINTAIN                             */
/*     mlenTag       must equal to MLT_SHORTMSG                            */
/*                                                                         */
/***************************************************************************/


/***************************************************************************/
/*     System console I/O Messages structure                               */
/*                                                                         */
/*     mType         must equal to MT_SYSCON                               */
/*     mlenTag       must equal to MLT_LONGMSG                             */
/*                                                                         */
/***************************************************************************/




/*===========================================================================
    eOSTimer
===========================================================================*/
/*---------------------------------------------------------------------------
  eOSTimer message code
---------------------------------------------------------------------------*/
#define MC_TIMEOUT     0x1

//---------------------------------------------------------------
//  Timer ID
//---------------------------------------------------------------
#define TIMER_ID    UINT16              /* eOStimer ID typedef             */
#define OSTIMER_ID  TIMER_ID

/****************************************************************************
      Timer attrib
****************************************************************************/
#define OT_TA_TMOUT        MST_OT_TMOUT
#define OT_TA_PERIOD       MST_OT_PERIOD



/***************************************************************************/
/*    System Call code, used in error alarming & processing                */
/*    VxWorks system call code                                             */
/***************************************************************************/
#define  EC_GNRL_QFAIL     0xff000003
#define  EC_GNRL_MTYPE     0xff000001
#define  EC_GNRL_INIT      0xff000002

/***************************************************************************/
/*    ROOT task system call code & error code                              */
/***************************************************************************/
/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call code for ROOT                           */
/*-------------------------------------------------------------------------*/
#define  OSEC_RT_TCREATE   0x8001         /* _Create_Task()                */
#define  OSEC_RT_TRESTART  0x8002         /* _Restart_Task()               */

/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call error code for ROOT                     */
/*-------------------------------------------------------------------------*/
#define  EC_RT_TCREATE     0x80000100     /* error when create task        */
#define  EC_RT_TSTART      0x80000200     /* error when start task         */
#define  EC_RT_TRESTART    0x80000300     /* error when restart task       */
#define  EC_RT_TSETCODE    0x80000400     /* error when set task code      */


/***************************************************************************/
/*    OSTimer task system call code & error code                           */
/***************************************************************************/
/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call code for OSTimer                        */
/*-------------------------------------------------------------------------*/
#define  OSEC_OT_TMCREATE  0x8801         /* CreateTimer()                 */
#define  OSEC_OT_TMSTART   0x8802         /* StartTimer()                  */
#define  OSEC_OT_TMSTOP    0x8803         /* StopTimer()                   */
#define  OSEC_OT_TMDELETE  0x8804         /* KillTimer()                   */

/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call error code for OSTimer                  */
/*-------------------------------------------------------------------------*/
#define  EC_OT_INIT        0x88000003     /* task init fatal error         */
#define  EC_OT_ATQFAIL     0x88000004     /* active timer queue fail       */
#define  EC_OT_NOTIMER     0x88000101     /* no free OSTimer in OTCB tab   */
#define  EC_OT_PARAM       0x88000102     /* parameter  value  invalid     */
#define  EC_OT_OTBUSY 	   0x88000103	  /* OSTimer task busy             */
#define  EC_OT_IDINV       0x88000201     /* ostimer ID invalid            */
#define  EC_OT_STATUS      0x88000202     /* ostimer-CB status error       */
#define  EC_OT_TIDINV      0x88000203     /* ostimer invalid task ID       */



/***************************************************************************/
/*    System manager task system call code & error code                    */
/***************************************************************************/
/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call code for system manager                 */
/*-------------------------------------------------------------------------*/
#define  OSEC_SM_PTMRMSG   0x8901         /* smpTimerMsg()                 */
#define  OSEC_SM_PINTMSG   0x8902         /* smpIntrMsg()                  */

/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call error code for system manager           */
/*-------------------------------------------------------------------------*/
#define  EC_SM_MTYPE       0x89000001     /* receive message type error    */
#define  EC_SM_QFAIL       0x89000002     /* Sysman input queue failure    */
#define  EC_SM_INIT        0x89000003     /* task init fatal error         */
#define  EC_SM_OTMRID      0x89000101     /* sysman Ostimer ID error       */
#define  EC_SM_SUBTYPE     0x89000201     /* message subtype error         */

/***************************************************************************/
/*    system console task system call code & error code                    */
/***************************************************************************/
/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call code for system console                 */
/*-------------------------------------------------------------------------*/
#define  OSEC_SC_PSCONMSG  0x8c01         /* scpSysconMsg()                */
#define  OSEC_SC_CONOUT    0x8c02         /* con_output()                  */
#define  OSEC_SC_DISPSIG   0x8c03         /* DisplaySignal()               */

/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call error code for system console           */
/*-------------------------------------------------------------------------*/
#define  EC_SC_MTYPE       0x8c000001     /* invalid message type          */
#define  EC_SC_QFAIL       0x8c000002     /* SYSCON¡¡input queue failure   */
#define  EC_SC_INIT        0x8c000003     /* task init fatal error         */
#define  EC_SC_SIGPTR      0x8c000101     /* invalid signal pointer        */
#define  EC_SC_SUBTYPE     0x8c000102     /* invalid SYSCON msg subtype    */
#define  EC_SC_SIGLEN      0x8c000103     /* invalid signal size to disp   */
#define  EC_SC_NOTASK      0x8c000201     /* SYSCON task no running        */
#define  EC_SC_CONOUT      0x8c000202     /* error in func con_output()    */
#define  EC_SC_SIGCPY      0x8c000303     /* signal copy failed            */
#define  EC_SC_SEND        0x8c000304     /* send message to SYSCON error  */

/***************************************************************************/
/*    Memory manager system call code & error code                         */
/***************************************************************************/
/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call code for memory manager                 */
/*-------------------------------------------------------------------------*/
#define  OSEC_MM_INIT      0x9001         /* Init_MemManager()             */
#define  OSEC_MM_MALLOC    0x9002         /* memAlloc()                    */
#define  OSEC_MM_MFREE     0x9003         /* memFree()                     */

/*-------------------------------------------------------------------------*/
/*    VxWorks extender system call error code for memory manager           */
/*-------------------------------------------------------------------------*/
#define  EC_MM_INIT        0x90000101     /* error when init memman        */
#define  EC_MM_REINIT      0x90000102     /* error when reinit memman      */
#define  EC_MM_NULLPTR     0x90000201     /* a null-pointer                */
#define  EC_MM_NOBUF       0x90000202     /* no free buffer to allocate    */
#define  EC_MM_GETBUF      0x90000203     /* error when get buffer         */
#define  EC_MM_RETBUF      0x90000301     /* error when return buufer      */


/****************************************************************************
  SDL task process errcode
****************************************************************************/
#define  PSCC_QCREATE      0x10           /* msgQcreate()                  */
#define  PSCC_QSEND        0x11           /* msgQsend()                    */
#define  PSCC_QRECEIVE     0x12           /* msgQreceive()                 */
#define  PSCC_TCREATE      0x20           /* taskSpawn()                   */

/*-------------------------------------------------------------------------*/
/*     Common call code for SDL application task                           */
/*-------------------------------------------------------------------------*/
#define  SDLEC_ENV         0x9101
#define  SDLEC_TASK        0x9102
#define  SDLEC_SIGOUT      0x9103
#define  SDLEC_PROCESS     0x9104
#define  SDLEC_TIMER       0x9105
#define  SDLEC_SIGIN       0x9106
#define  SDLEC_PROCEDURE   0x9107
/*=========================================================================*/
/*    Define SDL application task Run Time Error Code                      */
/*=========================================================================*/
#define  EC_SDL_QFAIL      0x91000003     /* SDL task input queue fail     */
/*-------------------------------------------------------------------------*/
/*    ENV task error code                                                  */
/*-------------------------------------------------------------------------*/
#define  EC_SDL_ENVCREATE  0x91000101     /* ENV task create failure       */
#define  EC_SDL_ENVMSG     0x91000102     /* ENV receive a invalid message */
#define  EC_SDL_ENVQFAIL   0x91000103     /* ENV task input queue failure  */
/*-------------------------------------------------------------------------*/
/*    Application Task creation                                            */
/*-------------------------------------------------------------------------*/
#define  EC_SDL_APPCREATE  0x91000201     /* APP task create failure       */
#define  EC_SDL_APPSUSPEND 0x91000202     /* APP task suspend              */
/*-------------------------------------------------------------------------*/
/*    Process creation & stop                                              */
/*-------------------------------------------------------------------------*/
#define  EC_SDL_STAALLOC   0x91000301     /* startup sig mem alloc error   */
#define  EC_SDL_PRSALLOC   0x91000302     /* xPrsNode mem alloc error      */
#define  EC_SDL_VARALLOC   0x91000303     /* local var mem alloc error     */
#define  EC_SDL_STARTOUT   0x91000304     /* startup signal output failure */
#define  EC_SDL_MAXINST    0x91000305     /* process has max instance      */
#define  EC_SDL_INSTEXIST  0x91000306     /* Instance Already Exist        */
/*-------------------------------------------------------------------------*/
/*    Signal sending                                                       */
/*-------------------------------------------------------------------------*/
#define  EC_SDL_SIGALLOC   0x91000401     /* signal memory allocation error*/
#define  EC_SDL_INVRECEIVER 0x91000402    /* invalid receiver              */
#define  EC_SDL_TASKNO     0x91000403     /* invalid task No               */
#define  EC_SDL_PID        0x91000404     /* invalid task LocalPID         */
#define  EC_SDL_SIGOUT     0x91000405     /* signal send failure           */
/*-------------------------------------------------------------------------*/
/*    Signal receive                                                       */
/*-------------------------------------------------------------------------*/
#define  EC_SDL_SIGIN      0x91000501     /* signal receive failure        */
#define  EC_SDL_SIGTYPE    0x91000502     /* invalid signal type           */
#define  EC_SDL_STATE      0x91000503     /* invalid state                 */
/*-------------------------------------------------------------------------*/
/*    Timer                                                                */
/*-------------------------------------------------------------------------*/
#define  EC_SDL_TMSET      0x91000601     /* New timer set failure         */
#define  EC_SDL_TMRESET    0x91000602     /* timer reset failure           */
#define  EC_SDL_TMMISSING  0x91000603     /* timer can't be found          */
#define  EC_SDL_TMALLOC    0x91000604     /* timer signal memory alloc err */
/*-------------------------------------------------------------------------*/
/*    Procedure                                                            */
/*-------------------------------------------------------------------------*/
#define  EC_SDL_PRDALLOC    0x91000701    /* procedure node mem alloc error*/

/****************************************************************************
      prototype of eOSedu functions
****************************************************************************/
extern int Init_eOS_system(ULONG SUnitNum, ULONG SUnitSize, 
                           ULONG LUnitNum, ULONG LUnitSize);

/****************************************************************************
      prototype of OSTimer system call functions
****************************************************************************/
extern UINT32 eCreateTimer(TIMER_ID*,UINT32,UINT32,UINT32,UINT32,
                           UINT32,UINT32,UINT32);
extern UINT32 eStartTimer(TIMER_ID,UINT32,UINT32);
extern UINT32 eStopTimer(TIMER_ID);
extern UINT32 eKillTimer(TIMER_ID);
extern UINT32 eStopTaskAllTimer(UINT32);
extern UINT32 eKillTaskAllTimer(UINT32);
extern UINT32 eGetTimeStamp(void);
extern UINT32 eGetTimeFreeNum(void);


/***************************************************************************
    prototypes of memory management functions
***************************************************************************/
extern UINT32 memAlloc(void ** buf_addr, unsigned length);
extern UINT32 memFree(void * buf_addr);
extern UINT32 memInfo(UINT32 * sBupPoolFree, UINT32 * lBupPoolFree);

/***************************************************************************/
/*    global data structures defined in ROOT.C                             */
/***************************************************************************/
extern UINT32 idle_counter;
extern UINT32 LocalModuleNo;

/**************************************************************************/
/*      prototype of functions in ROOT.C                                  */
/**************************************************************************/
extern UINT32 _Create_Task(UINT32  t_code,
                           char *  t_name,
                           UINT32  t_prio,
                           UINT32  t_option,
                           UINT32  t_stack,
                           int*    t_args,
                           void (* t_entry_addr)());
extern UINT32 _Restart_Task(UINT32 t_code, int* t_args);
extern void ErrAlarm(unsigned errcode,unsigned Param1,unsigned Param2);

#define OC_SUSPEND_ERR_TASK  0
#define Error_Alarm(x,y) ErrAlarm(x,y,0)
#define ConsoleOut(x)  printf(x)


#endif
