/****************************************************************************

                 IoT & I-System Research Center, CST, BUPT

            Project : Demo
              Group : Operating System
               Item : Demo for embedded project
               File : eduDemo.c
        Description : a two tasks DEMO file

****************************************************************************/

/*
modification history 
--------------------
10a,21apr2001, jkuang ,Original Version
10b,29jul2018, jkuang, update demo.c to eduDemo.c
*/


#include <vxWorks.h>
#include <msgQLib.h>
#include <taskLib.h>
#include <logLib.h>
#include "eduappcfg.h"
#include "eduappmsgcode.h"

#define RECV_INSTANCE_NUM    4	//Receiver Instance Number

void ResourceShow(void);
void memPoolTest(void);

/***************************************************************************/
/*                                                                         */
/*      Function   : NM task                                               */
/*                                                                         */
/*      Task name  : NM                                                    */
/*      Startup Paramter:  NONE                                            */
/*                                                                         */
/***************************************************************************/
void NM_task(void)
{
   int     msglen;
   MESSAGE aMsgGot;
   UINT8   cnt = 0;
   UINT32  retcode;
	 
// NM Task's MesQ Created in Init_Application_Tasks() of eduAppRoot.c
/*
   if(!(TASK_MBOX(APP_TC_nm) = msgQCreate(OC_nm_QMAXMSG,
                               OC_nm_QMSGLEN,OC_nm_QFLAG)))
   {
     Error_Alarm(errno,0);
#ifdef OC_DEBUG     
     ConsoleOut("NM Task exit!");
#endif
     TASK_STATE(APP_TC_nm) = TRS_DELETED;
     taskDelete(TASK_ID(APP_TC_nm));
   };
*/
  TASK_STATE(APP_TC_nm) = TRS_RUNNING;

	/*------------------------------------------------------------
		 Task sync control
	------------------------------------------------------------*/
  while(cnt < (APP_TC_nm - OC_TC_1stAPP))
  {
    msglen = msgQReceive(TASK_MBOX(APP_TC_nm), (char*)&aMsgGot, 
                         OC_nm_QMSGLEN, WAIT_FOREVER);
    if(aMsgGot.mCode != MC_TASK_SYNC) continue;
    cnt++;
//    logMsg("NM Msg Recved :%d\n",cnt,0,0,0,0,0); 
  }

  semFlush(syncBSem);  //Active Other App Tasks
  /*------------------------------------------------------------------------*/
  /*                     Main loop                                          */
  /*------------------------------------------------------------------------*/   
  FOREVER
  {
    msglen = msgQReceive(TASK_MBOX(APP_TC_nm), (char*)&aMsgGot, 
                         OC_nm_QMSGLEN, WAIT_FOREVER);
		/*------------------- general message check --------------------------*/
		if(msglen < 0)
		{
			Error_Alarm(errno,0);
			continue;
		}
			
		if(msglen < OC_nm_QMSGLEN)
			continue;
		
		if(aMsgGot.mRecvID != APP_TC_nm)
		{
			if(aMsgGot.mLenTag != MLT_SHORTMSG)
				memFree(aMsgGot.pktptr);
			continue;
		}

			/*------------------- state process --------------------------*/
			 switch(aMsgGot.mCode)
			{
				case MC_NM_REPORT:
					//mem test
					memPoolTest();
					//Show eOS Resource
					ResourceShow();
 				  break;
				default:
#ifdef OC_DEBUG     
						ConsoleOut("NM got a UNKNOW message\n");
#endif 
			}

		/*----------------- Dealloc memory of long message --------------*/
		if(aMsgGot.mLenTag != MLT_SHORTMSG)
			memFree(aMsgGot.pktptr);
  }
  
}
  
/***************************************************************************/
/***************************************************************************/
typedef struct _SENDER_CTLTBL
{
  TIMER_ID  TimerA;              /* timer id */
  TIMER_ID  TimerB;
  TIMER_ID  TimerC;
 
  UCHAR     tAact,tBact,tCact,resvd;
} SENDER_CTLTBL;
/***************************************************************************/
/*                                                                         */
/*      Function   : SENDER task                                           */
/*                                                                         */
/*      Task name  : Sender                                                */
/*      Startup Paramter:  NONE                                            */
/*                                                                         */
/***************************************************************************/
void Sender_task(void)
{
   int     msglen;
   MESSAGE aMsg = {MT_APP,0,APP_TC_sender};
   MESSAGE aMsgGot;
   SENDER_CTLTBL* SenderDataP;
   UCHAR   recv_inst;		//Receiver Instance Idx
   UINT32  retcode;

   if(!(TASK_MBOX(APP_TC_sender) = msgQCreate(OC_Sender_QMAXMSG,
                                   OC_Sender_QMSGLEN,OC_Sender_QFLAG)))
   {
     Error_Alarm(errno,0);
#ifdef OC_DEBUG     
     ConsoleOut("SENDER Task exit!");
#endif
     TASK_STATE(APP_TC_sender) = TRS_DELETED;
     taskDelete(TASK_ID(APP_TC_sender));
   };
   TASK_STATE(APP_TC_sender) = TRS_RUNNING;
   
	 
	 ResourceShow();

	 //Get Task Resource
   memAlloc((void**)&TASK_INSTDATA(APP_TC_sender), sizeof(SENDER_CTLTBL));
   SenderDataP = (SENDER_CTLTBL*)TASK_INSTDATA(APP_TC_sender);

   SenderDataP->tAact = 1;
   SenderDataP->tBact = 0;
   SenderDataP->tCact = 1;
   retcode = eCreateTimer(&SenderDataP->TimerA, APP_TC_sender, OT_TA_PERIOD,
                            SenderDataP->tAact, 2000, 0, 0, 0xEEEE);
   retcode = eCreateTimer(&SenderDataP->TimerB, APP_TC_sender, OT_TA_TMOUT,
                            SenderDataP->tBact, 100, 0xFF, 0, 0xEEEE);
   retcode = eCreateTimer(&SenderDataP->TimerC, APP_TC_sender, OT_TA_TMOUT,
                            SenderDataP->tCact, 300, 0xAA, 0, 0xEEEE);

   recv_inst = RECV_INSTANCE_NUM - 1;

   /*------------------------------------------------------------
      Task sync control
   ------------------------------------------------------------*/
   aMsg.mLenTag = MLT_SHORTMSG;
   aMsg.mRecvID = APP_TC_nm;
   aMsg.mCode   = MC_TASK_SYNC;
   aMsg.mCTRL   = 0;
   if(msgQSend(TASK_MBOX(APP_TC_nm),(char*)&aMsg, OC_nm_QMSGLEN, 
               NO_WAIT, MSG_PRI_NORMAL))
   {
   	 Error_Alarm(errno,0);
   }
   
   semTake(syncBSem, WAIT_FOREVER);

   /*------------------------------------------------------------------------*/
   /*                     Main loop                                          */
   /*------------------------------------------------------------------------*/   
   FOREVER
   {
     msglen = msgQReceive(TASK_MBOX(APP_TC_sender), (char*)&aMsgGot, 
                          OC_Sender_QMSGLEN, WAIT_FOREVER);
     if(msglen < 0)
     {
         Error_Alarm(errno,0);
         continue;
     };
     if(msglen < OC_Sender_QMSGLEN)
       continue;

     if(aMsgGot.mRecvID != APP_TC_sender)
     {
       if(aMsgGot.mLenTag != MLT_SHORTMSG)
         memFree(aMsgGot.pktptr);
       continue;
     }
     /*----------------------- message process ------------------------------*/
     switch(aMsgGot.mCode)
     {
       case MC_TIMEOUT:
            #ifdef OC_DEBUG
              ConsoleOut("SEBDER TIMEOUT\n");
            #endif

//						ResourceShow();

            //TimerA Process
            if(OSTIMER_MSG(aMsgGot)->otID == SenderDataP->TimerA)
            {
              eStartTimer(SenderDataP->TimerB, 0, 0xFF);
              SenderDataP->tBact = 1;

              aMsg.mLenTag = MLT_SHORTMSG;
              aMsg.mRecvID = APP_TC_receiver;
              aMsg.mCode   = MC_aSTRING;
              aMsg.mCTRL   = 0;
              aMsg.mSendInst   = 0;
              recv_inst = (recv_inst + 1) % RECV_INSTANCE_NUM;
              aMsg.mRecvInst   = recv_inst;
              if(msgQSend(TASK_MBOX(APP_TC_receiver), (char*)&aMsg,
                 OC_Receiver_QMSGLEN, NO_WAIT, MSG_PRI_NORMAL))
              {
                Error_Alarm(errno, 0);
              }
              break;
            }

            //TimerB Process
            if(OSTIMER_MSG(aMsgGot)->otID == SenderDataP->TimerB)
            {
              SenderDataP->tBact = 0;

//              printf("Timer B :%d\n",tmr_cnt);

              break;
            }

            //TimerC Process
            if(OSTIMER_MSG(aMsgGot)->otID == SenderDataP->TimerC)
            {
              eStartTimer(SenderDataP->TimerC, 0, 0xFF);

              //Send MC_NM_REPORT to NM

							aMsg.mLenTag = MLT_SHORTMSG;
              aMsg.mRecvID = APP_TC_nm;
              aMsg.mCode	 = MC_NM_REPORT;
              aMsg.mCTRL	 = 0;
              aMsg.mSendInst	 = 0;
              aMsg.mRecvInst	 = 0;
              if(msgQSend(TASK_MBOX(APP_TC_nm), (char*)&aMsg,
                    	 OC_nm_QMSGLEN, NO_WAIT, MSG_PRI_NORMAL))
              {
	              Error_Alarm(errno, 0);
              }

            }
            break;

       case MC_HOT:
            logMsg("Msg from Recvr: %d timestamp: 0x%x\n",aMsgGot.mSendInst,eGetTimeStamp(),0,0,0,0);
            if(SenderDataP->tBact)
            {
              eStopTimer(SenderDataP->TimerB);
              SenderDataP->tBact = 0;
            }
            break;

       default:
#ifdef OC_DEBUG     
            ConsoleOut("SENDER got a UNKNOW message\n");
#endif 
            break;
     }/* switch */
     /*----------------- Dealloc memory of long message --------------*/
     if(aMsgGot.mLenTag != MLT_SHORTMSG)
       memFree(aMsgGot.pktptr);
   }
}


/***************************************************************************
***************************************************************************/

typedef struct _RECV_CTLTBL
{
  int       state;              /* instance state */
  int       idx;                /* instance index */
  TIMER_ID  Timer;              /* onstance timer id */
  UINT16    rsvd;
} RECV_CTLTBL;

#define RECEIVER_DATA_P ((RECV_CTLTBL*)TASK_INSTDATA(APP_TC_receiver))

static MESSAGE *msg_recvr; /* message pointer for sending out of receiver */
/***************************************************************************
***************************************************************************/
int Recvr_state_0(RECV_CTLTBL* Act_ctP, MESSAGE *aMsgGot)
{
   int nextstate = 0;

   switch(aMsgGot->mCode)
   {
     case MC_aSTRING:
          eStartTimer(Act_ctP->Timer, 0, 0xFF);
          nextstate = 1;
          logMsg("inst:%d\n", Act_ctP->idx, 0,0,0,0,0);
//					printf("inst:%d\n", Act_ctP->idx);
          break;
     default:
     	  break;
   }/* Switch */
   return nextstate;
}

/***************************************************************************
***************************************************************************/
int Recvr_state_1(RECV_CTLTBL* Act_ctP, MESSAGE *aMsgGot)
{
  int nextstate = 1;

  switch(aMsgGot->mCode)
  {
     case MC_TIMEOUT:
          if(((OSTIMER_MESSAGE *)aMsgGot)->otID == Act_ctP->Timer)
          {
            /* Send a message back to SENDER */
            msg_recvr->mLenTag     = MLT_LONGMSG;
            msg_recvr->mRecvID     = APP_TC_sender;
            msg_recvr->mCode       = MC_HOT;
            msg_recvr->mSendInst   = Act_ctP->idx;
            msg_recvr->mRecvInst   = 0;
            memAlloc(&(msg_recvr->pktptr), OC_MM_SBUFUNITSIZE);
            msg_recvr->mLen = OC_MM_SBUFUNITSIZE;
            if(msgQSend(TASK_MBOX(APP_TC_sender),(char*)msg_recvr, OC_Sender_QMSGLEN,
                          NO_WAIT, MSG_PRI_NORMAL))
            {
              memFree(msg_recvr->pktptr);
							Error_Alarm(errno,0);
            }
            nextstate = 0;
          }
          break;
     default:
     	  break;
   }/* Switch */
   return nextstate;
}


/***************************************************************************/
/*                                                                         */
/*      Function   : RECEIVER task                                         */
/*                                                                         */
/*      Task name  : Receiver                                              */
/*      Startup Paramter:  NONE                                            */
/*                                                                         */
/***************************************************************************/
void Receiver_task(void)
{
   int msglen, i;
   RECV_CTLTBL *Act_ctP;
   MESSAGE aMsgGot;
   UINT32  retcode;

   if(!(TASK_MBOX(APP_TC_receiver) = msgQCreate(OC_Receiver_QMAXMSG,
                                       OC_Receiver_QMSGLEN,
                                       OC_Receiver_QFLAG)))
   {
     Error_Alarm(errno,0);
#ifdef OC_DEBUG     
     ConsoleOut("RECEIVER Task exit!");
#endif
     TASK_STATE(APP_TC_receiver) = TRS_DELETED;
     taskDelete(TASK_ID(APP_TC_receiver));
   };
   TASK_STATE(APP_TC_receiver) = TRS_RUNNING;

	 //Get Task Resource
   /*------------------------------------------------------------
      instance initial
   ------------------------------------------------------------*/
   memAlloc((void**)&TASK_INSTDATA(APP_TC_receiver), 
                 sizeof(RECV_CTLTBL) * RECV_INSTANCE_NUM);
   Act_ctP = (RECV_CTLTBL*)TASK_INSTDATA(APP_TC_receiver);
   
   for(i=0; i<RECV_INSTANCE_NUM; i++)
   {
     Act_ctP->state = 0;
     Act_ctP->idx   = i;
		 retcode = eCreateTimer(&(Act_ctP->Timer),
		 	         APP_TC_receiver, OT_TA_TMOUT, 0, 500, 0xAA, i, 0xEEEE);
  	 if(retcode)
     {
       printf("!!! Task%d Instance%d CreatTimer Failure!", APP_TC_receiver, i);
     }
     Act_ctP++;
   }

   /*------------------------------------------------------------
      initial message header for sending out
   ------------------------------------------------------------*/
   memAlloc((void**)&msg_recvr, OC_Receiver_QMSGLEN);

   msg_recvr->mType   = MT_APP;
   msg_recvr->mSendID = APP_TC_receiver;
   msg_recvr->mCTRL   = 0;
   msg_recvr->mLen    = 0;
   msg_recvr->pktptr  = NULL;
   /*------------------------------------------------------------
      Task sync control
   ------------------------------------------------------------*/
   msg_recvr->mLenTag   = MLT_SHORTMSG;
   msg_recvr->mRecvID   = APP_TC_nm;
   msg_recvr->mCode     = MC_TASK_SYNC;
   msg_recvr->mSendInst = -1;
   msg_recvr->mRecvInst = 0;
     
   if(msgQSend(TASK_MBOX(APP_TC_nm), (char*)msg_recvr, OC_nm_QMSGLEN, 
               NO_WAIT, MSG_PRI_NORMAL))
   {
       Error_Alarm(errno,0);
   }
   semTake(syncBSem, WAIT_FOREVER);

   /*------------------------------------------------------------------------*/
   /*                     Main loop                                          */
   /*------------------------------------------------------------------------*/   
   FOREVER
   {
     msglen = msgQReceive(TASK_MBOX(APP_TC_receiver), (char*)&aMsgGot,
                          OC_Receiver_QMSGLEN, WAIT_FOREVER);
     /*------------------- general message check --------------------------*/
     if(msglen < 0)
     {
       Error_Alarm(errno,0);
       continue;
     }
     
     if(msglen < OC_Receiver_QMSGLEN)
       continue;

     if(aMsgGot.mRecvID != APP_TC_receiver)
     {
       if(aMsgGot.mLenTag != MLT_SHORTMSG)
         memFree(aMsgGot.pktptr);
       continue;
     }
     /*------------------- instance check --------------------------*/
     if(aMsgGot.mRecvInst < RECV_INSTANCE_NUM)
       Act_ctP = &(RECEIVER_DATA_P[aMsgGot.mRecvInst]);
     else
       continue;

     /*------------------- state process --------------------------*/
     switch(Act_ctP->state)
     {
       case 0:
       	    Act_ctP->state = Recvr_state_0(Act_ctP,&aMsgGot);
       	    break;
       case 1:
       	    Act_ctP->state = Recvr_state_1(Act_ctP,&aMsgGot);
       	    break;
       default:
       	    printf("Receiver state error!\n");
       	    Act_ctP->state = 0;
     }
     /*----------------- Dealloc memory of long message --------------*/
     if(aMsgGot.mLenTag != MLT_SHORTMSG)
       memFree(aMsgGot.pktptr);
   }
}


//****************************************************************
void ResourceShow(void)
{
  UINT32 sBupPoolFree, lBupPoolFree;
	
	memInfo(&sBupPoolFree, &lBupPoolFree);
//	printf("memPool Free - Short: %u; Long:%u; Timer Free:%u\n",
//		      sBupPoolFree, lBupPoolFree, eGetTimeFreeNum());
  logMsg("memPool Free - Short: %u; Long:%u; Timer Free:%u\n", 
		      sBupPoolFree, lBupPoolFree, eGetTimeFreeNum(),0,0,0);
}


//****************************************************************
void *memRoot = 0;  //mem block chain root pointer
UINT32 memAllocating = 1;  //Initial set mem alloc process 

void memPoolTest(void)
{
  UINT32 sBupPoolFree, lBupPoolFree;
	UINT32 *memTmp;  //mem block tmp pointer
	
	if(memAllocating)  //Alloc
	{
		if(memInfo(&sBupPoolFree, &lBupPoolFree))
	  {
	    memAlloc((void**)&memTmp, 20);
		  *memTmp = (UINT32)memRoot;
		  memRoot = memTmp;
		}
		else
		{
		  memAllocating = 0;  //Set mem free process
	  }
	}
	else  //Free
	{
	  if(memRoot)
	  {
	    memTmp = memRoot;
		  memRoot = (void*)*memTmp;
      memFree((void *)memTmp);
	  }
		else
		{
		  memAllocating = 1;  //Set mem alloc process
		}
	}
	
}


