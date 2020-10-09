#include <unistd.h>
#include <stdlib.h>
//#include "SharedMemHead.h"
//#include "SemManageHead.h"

#ifndef MAINHEADER_H
#define MAINHEADER_H

#include <time.h>

extern const int false;
extern const int true;

extern const int cPARAMETERVALID;
extern const int cINCORRECTPARAMETER;
extern const int cPARAMETERNOTINTEGER;
extern const char *logFileName;

const int cMAXTELLER;

//this is used to record the customer information
typedef struct
  {
    int customerNo;
    char Transaction;
    time_t arrivalTime;
//later add time for time management
  } tCustomer;

typedef struct
  {
//independent variables
    int noOfChildProcess;//number of active child process
//    int noOfMsgQueue;//to count the number of customer in the queue
    int moreCustomer;//to check whether more customer still in the file

//statistic data
    int TellerRunTime[3];
    int NoOfCustomer;
    int WaitingTime;
    int TurnaroundTime;

//message things
    int messageQueueID;//to catch the message queue ID address

//semaphore ID things
    int waitMessageSemID;//sem ID to order the child to wait until new
                         //data in the queue is sent
    int getMessageSemID;//sem ID to mark the mutual exclusion of receiving data
                        //actually do not need to use this
                        //however to make nicer output
    int noOfProcessSemID;//the semaphore ID to lock/unlock no Of Process
    int fileAccessSemID;//the semaphore ID to ensure mutex file access
  } tShVariables;

#endif
