#include "SharedMemHead.h"
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>

void *createSHMem(int size, int *errorLog, int *shmid)
//exports shared memory address and shared memory
//id
{
  *errorLog=0;
  void *shAddr;

  *shmid = shmget(IPC_PRIVATE, size, IPC_CREAT|0600);
  
  if (*shmid==-1)
    *errorLog=1;
  else
    {
      shAddr=shmat(*shmid, NULL, 0);
      if ((intptr_t) shAddr==-1)
        *errorLog=2;
    }  

  return shAddr;
}

void destroySharedMem(int shmid, void *Ptr)
//detach/destroy shared memory for 1 process that uses
{
  shmdt(Ptr);
  shmctl(shmid, IPC_RMID, NULL);
//  shmdt(Ptr);
}

