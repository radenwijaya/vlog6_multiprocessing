#ifndef MSGMANAGEHEAD_H
#define MSGMANAGEHEAD_H

#include <time.h>

extern const int cCANTCREATEMESSAGE;
extern const int cCANTSENDMESSAGE;
extern const int cCANTDESTROYMESSAGE;

typedef struct
  {
    long mtype;
    char mtext[1];
    int customerNo;
    time_t arrivalTime;
  } CustomerQueueMsg;

extern int createMessageQueue(int mtype, int *errorLog);
extern int sendMessages(int msgqid, CustomerQueueMsg msgbuf, int *errorLog);
extern int sendMessage(int msgqid, int mtype, 
                       int CustomerNo, char *TransType, int *errorLog);
extern CustomerQueueMsg receiveMessage(int msgqid);
extern int destroyMessageQueue(int msgqid, int *errorLog);
int getMessageQueueNumber(int messageQueueID);

#endif
