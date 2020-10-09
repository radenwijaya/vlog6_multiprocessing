# vlog6_multiprocessing

I will discuss on how to use fork to spawn child process and making your application multiprocessing capable.  In addition I will also discuss some IPC (Inter Process Communication) protocol such as semaphore, message queue and shared memory variable.

Multiprocessing programming, while difficult to do, is really rewarding on this day and age where desktop computer have multicore CPU.  Multiprocessing can optimise the program you made, and in theory, made the application performance scale to the number of CPU core in the computer.

However multiprocessing must be enabled manually at programming level, that means non multiprocessing or multithreading application will only able to access one CPU core.  Dividing the which task processed by which process is a decision that must be made by programmer.  No operating system or programming language, at the moment, can decide that for you.

To compile it you can choose to compile and type all the required parameters manually or simply run make and your computer will compile according to the included makefile.  The makefile will produce an executable named part_1 to run the software.

Non Source Code file:
1. customer_file contains the list of customer that will be arriving and their intention
2. simulation_log contains the output of the simulation
3. makefile, contains the make information

The source code was made using VI, intended to be compiled on GNU C and run on Linux system. It might or might not run on other system UNIX or its clone. 

Disclaimer: The source code, while tested and does satisfy my needs, does contains some bug and not very high performance.  I advise that you should not use it in production.  You should only use the source code for studying purposes only.

Video available in https://www.youtube.com/watch?v=UTp6YMPCACE
