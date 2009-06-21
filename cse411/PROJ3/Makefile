cc = g++
CFLAGS = 

all : tp1 tp2 tp3

tp1 :  	filesys.o disk.o tp1.o
	@echo Linking tp1
	$(cc) -o tp1 filesys.o disk.o tp1.o
	@echo Done
    
tp2 :   filesys.o disk.o tp2.o
	@echo Linking tp2
	$(cc) -o tp2 filesys.o disk.o tp2.o
	@echo Done

tp3 :   filesys.o disk.o tp3.o
	@echo Linking tp3
	$(cc) -o tp3 filesys.o disk.o tp3.o
	@echo Done

filesys.o : filesys.c 
	@echo Compiling filesys.c
	$(cc) -g -c $(CFLAGS) filesys.c 

tp1.o : tp1.c
	@echo Compiling tp1.c
	$(cc) -g -c $(CFLAGS) tp1.c

tp2.o : tp2.c
	@echo Compiling tp2.c
	$(cc) -g -c $(CFLAGS) tp2.c

tp3.o : tp3.c
	@echo Compiling tp3.c
	$(cc) -g -c $(CFLAGS) tp3.c

disk.o : disk.c
	@echo Compiling disk.c
	$(cc) -g -c $(CFLAGS) disk.c

clean :
	rm -f *.o tp1 tp2 tp3

submit :
	@echo Submitting...
	tar cvf 411.tar filesys.c filesys.h Makefile tp1.c tp2.c tp3.c disk.c disk.h
	/home/users1/cg411/bin/turnin 3 411.tar

