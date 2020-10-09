CC=gcc
CFLAGS=-I.
DEPS=MainPrg.h CustomerHead.h MsgManageHead.h ParamReader.h SemManageHead.h SharedMemHead.h TellerHead.h TimeManageHead.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CLFAGS)

part_1: MainProgs.c MsgManageFile.c ParameterReader.c SemManageFile.c SharedMemFile.c CustomerFile.c TellerFile.c TimeManageFile.c
	$(CC) -o part_1 MainProgs.c MsgManageFile.c ParameterReader.c SemManageFile.c SharedMemFile.c CustomerFile.c TellerFile.c TimeManageFile.c
