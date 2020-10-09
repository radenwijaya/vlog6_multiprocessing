#ifndef SEMMANAGEHEAD_H
#define SEMMANAGEHEAD_H

/*
 * define _GNU_SOURCE for semtimedop to compile
 * semtimedop is not standard System V
 */
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

typedef struct 
    {
      unsigned short sem_num;
      short sem_op;
      short sem_flg;
    } sembuf;

extern const int cCREATESEMERROR;
extern const int cSETSEMVALERROR;
extern const int cGETSEMVALERROR;
extern const int cSEMCLEANUPERROR;
extern const int cSEMOPERROR;

extern int createSemaphore(key_t key, int *errorLog);
 
extern int setSemValue(int semid, int value, int *errorLog);

extern int getSemValue(int semid, int *errorLog);

extern int removeSemaphore(int semid, int *errorLog);

extern int lockSemaphore(int semid);

extern int unlockSemaphore(int semid);

extern int timedLockSemaphore(int semid, int timeout);

#endif
