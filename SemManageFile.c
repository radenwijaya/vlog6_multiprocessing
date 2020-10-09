#include "SemManageHead.h"
#include <time.h>
#include <errno.h>

const int cCREATESEMERROR = 100;
const int cSETSEMVALERROR = 101;
const int cGETSEMVALERROR = 102;
const int cSEMCLEANUPERROR = 103;
const int cSEMOPERROR = 104;
const int cSEMTIMEOUT = 105;

typedef struct
  {
    int tv_sec;
    long int tv_nsec;
  } timespec;

int createSemaphore(key_t key, int *errorLog)
//this will create semaphore and return semaphore
//if this process failed, then will return -1
//with error log <> 1
{
  int semid;
  *errorLog=0;

  if (key!=IPC_PRIVATE)
    semid=semget(key, 1, IPC_CREAT|0600);
  else
    semid=semget(IPC_PRIVATE, 1, IPC_CREAT|0600);

  if (semid==-1)
    {
      *errorLog=cCREATESEMERROR;
      semid=*errorLog;
    }
    
  return semid;
}

int setSemValue(int semid, int value, int *errorLog)
{
  *errorLog=0;

  if (semctl(semid, 0, SETVAL, value)==-1)
    {
      *errorLog=cSETSEMVALERROR;
    }

  return *errorLog;
}

int getSemValue(int semid, int *errorLog)
{
  int value;
  *errorLog=0;
  value=semctl(semid, 0, GETVAL);

  if (value==-1)
    {
      *errorLog=cGETSEMVALERROR;
      value=*errorLog;
    }

  return value;
}

int removeSemaphore(int semid, int *errorLog)
{
  *errorLog=0;
  if (semctl(semid, 0, IPC_RMID)==-1)
    *errorLog=cSEMCLEANUPERROR;

  return *errorLog;
}

int runSemaphore(int semid, sembuf semaphore)
{
  int result = 0;
  if (semop(semid, (struct sembuf*) &semaphore, 1)==-1)
    result=cSEMOPERROR;

  return result;
}

int timedLockSemaphore(int semid, int timeout)
{
  int result = 0;
  sembuf semaphore;

  semaphore.sem_num=0;
  semaphore.sem_op=-1;
  semaphore.sem_flg=0;

  timespec ts;
  ts.tv_sec=timeout;

  if (semtimedop(semid, (struct sembuf*) &semaphore, 1,
      (struct timespec*) &ts)==-1)
    if (errno==EAGAIN)
      result=cSEMTIMEOUT;

  return result;
}

int lockSemaphore(int semid)
{
  sembuf semaphore;

  semaphore.sem_num=0;
  semaphore.sem_op=-1;
  semaphore.sem_flg=0;

  return runSemaphore(semid, semaphore);  
}

int unlockSemaphore(int semid)
{
  sembuf semaphore;

  semaphore.sem_num=0;
  semaphore.sem_op=1;
  semaphore.sem_flg=0;

  return runSemaphore(semid, semaphore);  
}
