#include "TellerHead.h"
#include "MainPrg.h"
#include "MsgManageHead.h"
#include <stdio.h>
#include "TimeManageHead.h"

const int cTELLERTIMEDOUT = 301;
const int cTELLERNONEXTCUSTOMER = 302;
const int cTELLERNOMORECUSTOMER = 303;
const int cTELLERTAKEMODE = 1;
const int cTELLERFINISHMODE = 2;
const int cTELLERDIE = 3;
const char *logFileName = "simulation_log";


time_t initTeller(tShVariables *ShVar)
{
  lockSemaphore(ShVar->noOfProcessSemID);

//critical section

//    lockSemaphore(ShVar->parentDieSemID);//parents can't die

  ShVar->noOfChildProcess++;
//end of critical section

//  printf("number of process %d\n", ShVar->noOfChildProcess);

  unlockSemaphore(ShVar->noOfProcessSemID);

  return time(NULL);
}

void doneTeller(tShVariables *ShVar, time_t startTime, int ownProcessNo)
{
  int teller;
  FILE *logFile;
  char *buf;

  lockSemaphore(ShVar->fileAccessSemID);

  lockSemaphore(ShVar->noOfProcessSemID);

//critical section
  logFile=fopen(logFileName, "a");
//  logFile=stdout; for debug only
  if (logFile==NULL) logFile=stdout;
//give output to stdout log file cannot be used
  
  ShVar->noOfChildProcess--;//decrease the number of child

  fprintf(logFile, "Teller-%d Termination:\n", ownProcessNo);
  fprintf(logFile, "----------------------\n");

  fprintf(logFile, "Start time: ");
  buf=formatTime(startTime);
  fprintf(logFile, buf);
  free(buf);

  fprintf(logFile, "\nTermination time: ");
  buf=formatTime(time(NULL));
  fprintf(logFile, buf);
  free(buf);

  fprintf(logFile, "\n\n");

  if (ShVar->noOfChildProcess==0)//after all of the child has gone
    {
      fprintf(logFile, "Teller Statistic\n");
      fprintf(logFile, "----------------\n");

      for (teller=0; teller<cMAXTELLER; teller++)
        fprintf(logFile, "Teller-%d running time: %d CPU second(s)\n",
                teller+1, ShVar->TellerRunTime[teller]);

      fprintf(logFile, "Number of customer(s) %d of customer(s)\n", 
              ShVar->NoOfCustomer);
      fprintf(logFile, "Average waiting time: %.2f second(s)\n",
              (float) ShVar->WaitingTime/(float) ShVar->NoOfCustomer);
      fprintf(logFile, "Average turn around time: %.2f second(s)\n\n",
              (float) ShVar->TurnaroundTime/(float) ShVar->NoOfCustomer);
    }

//    unlockSemaphore(ShVar->parentDieSemID);//now parents can die;

//end of critical section
  fclose(logFile);

  unlockSemaphore(ShVar->noOfProcessSemID);

  unlockSemaphore(ShVar->fileAccessSemID);  
}

void writeTellerLog(tCustomer Customer, int fileAccessSemID, 
                      int ownProcessNo, int mode)
{
  FILE *logFile;
  char *buf;

  lockSemaphore(fileAccessSemID);

  logFile=fopen(logFileName, "a");
//  logFile=stdout;
  if (logFile==NULL) logFile=stdout;
 
  fprintf(logFile, "Customer statistic for Teller-%d\n", ownProcessNo);
  fprintf(logFile, "-------------------------------\n");
  fprintf(logFile, "Customer %d\n", Customer.customerNo);
  fprintf(logFile, "Arrival time: ");
  buf=formatTime(Customer.arrivalTime);
  fprintf(logFile, buf);
  free(buf);

  if (mode==cTELLERTAKEMODE)
    {
      fprintf(logFile, "\nResponse time: ");
    }
  else
    {
      fprintf(logFile, "\nCompletion time: ");
    }

  buf=formatTime(time(NULL));
  fprintf(logFile, buf);
  free(buf);

  fprintf(logFile, "\n\n");

  fclose(logFile);

  unlockSemaphore(fileAccessSemID);
}

int getMessages(tShVariables *ShVar, int remActiveTime, 
                tCustomer *nextCustomer, int ownProcessNo)
{
  int result = 0;
  CustomerQueueMsg custBuf;

  time_t startTime=time(NULL);

  do 
    {
      if (getMessageQueueNumber(ShVar->messageQueueID)>0)
        {
          custBuf=receiveMessage(ShVar->messageQueueID);
          nextCustomer->customerNo=custBuf.customerNo;
          nextCustomer->Transaction=custBuf.mtext[0];
          nextCustomer->arrivalTime=custBuf.arrivalTime;

          result=1;
        }
      else
        {
          timedLockSemaphore(ShVar->waitMessageSemID, remActiveTime);
          if (remActiveTime<timeElapsed(startTime))
            result=2;
        }
    }
  while (result==0);

  return result;
}

void Teller(int ownProcessNo, int timeOut, int TimeWithdraw, 
            int TimeDeposit, tShVariables *ShVar)
{
  tCustomer nextCustomer;
  int res;
  time_t startTime;
  int remActiveTime, custProcessed, die, prevRemTime, runTime;

  startTime=initTeller(ShVar);

  remActiveTime=timeOut;

  do
    {
      custProcessed=false;

//checking if rem active time<0 however there were customer that has not been
//served
      if (remActiveTime<0)
        remActiveTime=prevRemTime;
      else
        prevRemTime=remActiveTime;
 
      res=getMessages(ShVar, remActiveTime, &nextCustomer, ownProcessNo);

//fprintf(logFile, "\n%d\n", nextCustomer.customerNo);
 
      switch (res)
        {
          case 1: //success
            {
              writeTellerLog(nextCustomer, ShVar->fileAccessSemID, 
                             ownProcessNo, cTELLERTAKEMODE);

              lockSemaphore(ShVar->getMessageSemID);

              ShVar->NoOfCustomer++;
              ShVar->WaitingTime=ShVar->WaitingTime+
                timeElapsed(nextCustomer.arrivalTime);

              unlockSemaphore(ShVar->getMessageSemID);

              if (nextCustomer.Transaction=='d')
                sleep(TimeDeposit);
              else
                sleep(TimeWithdraw);

              writeTellerLog(nextCustomer, ShVar->fileAccessSemID, 
                             ownProcessNo, cTELLERFINISHMODE);

              lockSemaphore(ShVar->getMessageSemID);

              ShVar->TurnaroundTime=ShVar->TurnaroundTime+
                timeElapsed(nextCustomer.arrivalTime);

              unlockSemaphore(ShVar->getMessageSemID);

              custProcessed=true;
              break;
            }
          case 2:
            {
//  fprintf(logFile, "false, %d", ShVar->noOfChildProcess);             
//              lockSemaphore(ShVar->noOfProcessSemID);
 
              if ((ShVar->noOfChildProcess>1) || 
                  (ShVar->moreCustomer==0))//not the last teller
                die=true;
              else
                die=false;
              break;

//              unlockSemaphore(ShVar->noOfProcessSemID);
            } 
        }

//  fprintf(logFile, "HERE IT IS %d\n", res);

//calculate the remaining active time for teller
      remActiveTime=timeOut-timeElapsed(startTime);

//if there were no more customer or time to die.
//no more customer happens when more customer flag is 0,
//customer has been processed and there were no message in the
//queue.
//time to die when remaining active time is <= 0 and there were
//another teller active
      lockSemaphore(ShVar->noOfProcessSemID);

      if ((((ShVar->moreCustomer==0) && 
            (custProcessed==true)) &&
           (getMessageQueueNumber(ShVar->messageQueueID)==0)) ||
           ((remActiveTime<=0) && (ShVar->noOfChildProcess>1)))
{
//fprintf(stdout, "DIE %d\n", ownProcessNo); //for debug only
       die=true;
}

      unlockSemaphore(ShVar->noOfProcessSemID);
    }
  while (die!=1);

  ShVar->TellerRunTime[ownProcessNo-1]=timeElapsed(startTime);

  doneTeller(ShVar, startTime, ownProcessNo);
}
