#include "MsgManageHead.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>

const int cCANTCREATEMESSAGE = 200;
const int cCANTSENDMESSAGE = 201;
const int cCANTDESTROYMESSAGE = 202;

int createMessageQueue(int mtype, int *errorLog)
{
  int msgqid;
  CustomerQueueMsg msgbuf;
  msgbuf.mtype=mtype;
  *errorLog=0;

  msgqid=msgget(IPC_PRIVATE, IPC_CREAT|0600);

  if (msgqid==-1)
    msgqid=*errorLog=cCANTCREATEMESSAGE;

  return msgqid;
}

int sendMessages(int msgqid, CustomerQueueMsg msgbuf, int *errorLog)
{
  int result;
  *errorLog=0;

  result=msgsnd(msgqid, &msgbuf, sizeof(CustomerQueueMsg), 0);
  if (result==-1)
    *errorLog=result=cCANTSENDMESSAGE;

  return result;
}

int sendMessage(int msgqid, int mtype, 
                int CustomerNo, char *TransType,
                int *errorLog)
{
  CustomerQueueMsg msgbuf;
  msgbuf.mtype=mtype;
  strcpy(msgbuf.mtext, TransType);
  msgbuf.customerNo=CustomerNo;

  return sendMessages(msgqid, msgbuf, errorLog);
}

CustomerQueueMsg receiveMessage(int msgqid)
{
  CustomerQueueMsg msgbuf;
  msgrcv(msgqid, &msgbuf, sizeof(msgbuf), 0, IPC_NOWAIT|MSG_NOERROR);

  return msgbuf;
}

int destroyMessageQueue(int msgqid, int *errorLog)
{
  int result;
  *errorLog=0;

  if (msgctl(msgqid, IPC_RMID, 0)==-1)
    result=*errorLog=cCANTDESTROYMESSAGE;

  return result;
}

//to get the number of message queue
int getMessageQueueNumber(int messageQueueID)
{
  struct msqid_ds msqid_ds;
  msgctl(messageQueueID, IPC_STAT, &msqid_ds);

  return msqid_ds.msg_qnum;
}
