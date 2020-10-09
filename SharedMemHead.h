#ifndef SHAREDMEMHEAD_H
#define SHAREDMEMHEAD_H

extern void *createSHMem(int size, int *errorLog, int *shmid);
extern void destroySharedMem(int shmid, void *Ptr);

#endif
