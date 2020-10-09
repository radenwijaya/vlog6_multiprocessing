#include "MainPrg.h"
#include <stdio.h>
#include "MsgManageHead.h"
#include "TimeManageHead.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "SemManageHead.h"

const char *customerFileName = "customer_file";

int readCustomerFile(FILE *customerFile, tCustomer *NextCustomer)
{
  int cNo, success=0;
  char cType[9];

  if (EOF!=fscanf(customerFile, "%d %s", &cNo, cType))
    {
      NextCustomer->customerNo=cNo;
      NextCustomer->Transaction=cType[0];
      NextCustomer->arrivalTime=time(NULL);
//only character w or d is enough to differentiate between
//deposit or withdrawal
    }
  else
    success=-1;

  return success;
}

void writeCustomerLog(tCustomer Customer, int fileAccessSemID)
{
  int errorLog;
  FILE *logFile;
  char *buf;

  lockSemaphore(fileAccessSemID);

  logFile=fopen(logFileName, "a");

//  logFile=stdout; // for debug

//if cannot open for output then set output to standard output
  if (logFile==NULL) logFile=stdout;
 
  fprintf(logFile, "-------------------------------------\n");
  fprintf(logFile, "Customer %d: ", Customer.customerNo);

  switch (Customer.Transaction)
    {
      case 'd' : fprintf(logFile, "Deposit\n"); break;
      case 'w' : fprintf(logFile, "Withdrawal\n"); break;
    }

  fprintf(logFile, "Arrival time: ");
  buf=formatTime(Customer.arrivalTime);
  fprintf(logFile, buf);
  free(buf);
  fprintf(logFile, "\n-------------------------------------\n\n");

  fclose(logFile);

  unlockSemaphore(fileAccessSemID);
}

int Customer(int ReadInterval, tShVariables *ShVar, int *ChildPIDs)
{
  FILE *customerFile;
  tCustomer nextCustomer;
  CustomerQueueMsg custBuf;
  int success = 0;
  int errorLog = 0;
  int waitLoop;

  int runtime=0; //this is used to approximate the run time of the customer

  customerFile=fopen("customer_file", "r");
 
  if (customerFile!=NULL)
    {
      while (readCustomerFile(customerFile, &nextCustomer)==0)  
        {
          custBuf.mtype=1;
          custBuf.mtext[0]=nextCustomer.Transaction;
          custBuf.customerNo=nextCustomer.customerNo;
          custBuf.arrivalTime=nextCustomer.arrivalTime;

          writeCustomerLog(nextCustomer, ShVar->fileAccessSemID); 

          sendMessages(ShVar->messageQueueID, custBuf, &errorLog);

          lockSemaphore(ShVar->getMessageSemID);
//critical section

 //         ShVar->noOfMsgQueue++;
//increment the number of message in the queue

//child process can go now
          if (getMessageQueueNumber(ShVar->messageQueueID)==1)
            unlockSemaphore(ShVar->waitMessageSemID);

//end of critical section
          unlockSemaphore(ShVar->getMessageSemID);

//put the customer in the shared memory 

          sleep(ReadInterval);
        }

    }
  else
    {
      success=-1;      
    } 

//  lockSemaphore(ShVar->parentDieSemID);//just try to lock
//if the semaphore can be released, then parents can be killed
//or just exit as normal
//  unlockSemaphore(ShVar->parentDieSemID);

//this is used to notify whether there were more customer or not
//it is no need to mutually exclusive, since the other file just
//reading
  ShVar->moreCustomer=0;

//  fprintf(logFile, "parent die %d\n", ShVar->moreCustomer);

  for (waitLoop=0; waitLoop<cMAXTELLER; waitLoop++)
    waitpid(ChildPIDs[waitLoop], NULL, 0);
//wait until each of the child die

  fclose(customerFile);

  return success;
}
