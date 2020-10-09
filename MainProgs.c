#include <stdio.h>
#include "MainPrg.h"
#include "ParamReader.h"
#include "TellerHead.h"
#include "CustomerHead.h"
#include "MsgManageHead.h"
#include "SemManageHead.h"
#include "SharedMemHead.h"

const int false = 0;
const int true = 1;

const int cMAXTELLER = 3;

void errorHandler(int errType)
{
  switch (errType)
    {
      case 1 : 
        { 
          fprintf(stderr, "Can not invoke fork\n");
          break;
        }
      case 2 : 
        {
          fprintf(stderr, "Can not read customer_file\n");
          break;
        }
      default : break; //no error, do nothing
    }
}

int RunProcess(int ReadInterval, int* LiveTime, 
                int TimeWithdraw, int TimeDeposit)
{
  int ProcessNo, ChildPID, ownProcessNo, timeOut;
  int ChildPIDs [cMAXTELLER]; //storing for the child's PID
  int ParentProcess = true;
//this is the for indiacting the parent process

  int errorLog = 0;//for error checking

//this section is used for initiaizing the shared variables
  tShVariables *ShVar;
  void *ShVAttach;
  int ShVAddress;

  FILE *logFile;
  logFile=fopen(logFileName, "w");
  if (logFile==NULL) logFile=stdout;

  fprintf(logFile, "Starting simulation\n");
  fprintf(logFile, "-------------------\n\n");

  fclose(logFile);

  ShVAttach=createSHMem(sizeof(tShVariables), &errorLog, &ShVAddress);
  ShVar=(tShVariables *) ShVAttach;

  ShVar->noOfChildProcess=0;
//  ShVar->noOfMsgQueue=0;
  ShVar->moreCustomer=true;
//end of initiaizing shared variables

//initiaising message Queue ID
  ShVar->messageQueueID=createMessageQueue(1, &errorLog);

//this is for semaphore is for controling child to wait the parents 
  ShVar->waitMessageSemID = createSemaphore(IPC_PRIVATE, &errorLog);
  setSemValue(ShVar->waitMessageSemID, 1, &errorLog);

//this semaphore is for indicating where to get message
  ShVar->getMessageSemID = createSemaphore(IPC_PRIVATE, &errorLog);
  setSemValue(ShVar->getMessageSemID, 1, &errorLog);

//this is for semaphore to manage no of process process 
  ShVar->noOfProcessSemID = createSemaphore(IPC_PRIVATE, &errorLog);
  setSemValue(ShVar->noOfProcessSemID, 1, &errorLog);

//this is for semaphore to manage writing file access
  ShVar->fileAccessSemID = createSemaphore(IPC_PRIVATE, &errorLog);
  setSemValue(ShVar->fileAccessSemID, 1, &errorLog);

/*  cout << "parent die sem " << ShVar->parentDieSemID << endl;
  cout << "semaphore value " << ShVar->noOfProcessSemID << endl;*/
  
  ownProcessNo=0;//this is for indicating the own process no
//0 means parents, 1..3 means childs

//lock the semaphore here so that the child will be willing to
//wait for the parents to put the message in
  lockSemaphore(ShVar->waitMessageSemID);

//initiaisation for the loop
  ProcessNo=1; 
  while ((ProcessNo<=cMAXTELLER) && (ParentProcess) 
         && (errorLog==0))
   {

//while the parent process is running, no error happens, and
//number of process is < max teller+1
      ChildPID=fork();

      switch (ChildPID)
        {
          case -1 : 
            {
              errorLog=1; 
              break;
            }
          case 0 :
            {
//the child process goes here
              ParentProcess=false;

              ownProcessNo=ProcessNo;
              timeOut=LiveTime[ProcessNo-1];
              break;
            }
          default :
            {
//the parent process
              ChildPIDs[ProcessNo-1]=ChildPID;
            }
        }

      ProcessNo++;
    } 
  
  if ((errorLog==0) && (ownProcessNo==0)) //parent process will go here
    {
      if (Customer(ReadInterval, ShVar, ChildPIDs)==-1) 
        errorLog=2;

      destroyMessageQueue(ShVar->messageQueueID, &errorLog);
      removeSemaphore(ShVar->waitMessageSemID, &errorLog);
      removeSemaphore(ShVar->getMessageSemID, &errorLog);
      removeSemaphore(ShVar->noOfProcessSemID, &errorLog);
      removeSemaphore(ShVar->fileAccessSemID, &errorLog);
      destroySharedMem(ShVAddress, ShVAttach);
    }
  else
    { //child process will go here
      Teller(ownProcessNo, timeOut, TimeWithdraw, 
             TimeDeposit, ShVar);
//      cout << "Child die" << endl;
    }

  return errorLog;
}

int main(int argc, char** argv)
{
  int ReadInterval, TimeWithdraw, TimeDeposit;
  int LiveTime [cMAXTELLER];
  int ParamCheck;

  ParamCheck=getParameter(&ReadInterval, LiveTime, 
                          &TimeWithdraw, &TimeDeposit, 
                          argc, argv);

  if (ParamCheck==cPARAMETERVALID)
    {
      errorHandler(RunProcess(ReadInterval, LiveTime, 
                      TimeWithdraw, TimeDeposit));      
    }
  else
    writeParameterError(ParamCheck);

  return 0;
}
