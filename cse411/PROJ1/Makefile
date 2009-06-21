cc = g++

all : 	tp1 tp2 tp3 tp4

tp1 :
	@echo Compiling tp1
	$(cc) -o tp1 411_thr_sch.cpp tp1.c
	@echo Done
    
tp2 :
	@echo Compiling tp2
	$(cc) -o tp2 411_thr_sch.cpp tp2.c
	@echo Done

tp3 :
	@echo Compiling tp3
	$(cc) -o tp3 411_thr_sch.cpp tp3.c
	@echo Done

tp4 :
	@echo Compiling tp4
	$(cc) -o tp4 411_thr_sch.cpp tp4.c
	@echo Done

clean :
	rm -f *.o tp1 tp2 tp3 tp4

submit :
	@echo Tarring the files...
	tar cvf 411temp.tar 411_thr_sch.cpp 411_threads.h jfsqueue.h Makefile
	@chmod 600 411temp.tar
	@echo Submitting...
	/home/users1/cg411/bin/turnin 1 411temp.tar

