# Last Updated : Mar 2, 2:30 pm
cc = g++

all : tp0 tp1 tp2

tp0 : mem_manager.o tp0.o second_chance_vm.o
	@echo Linking tp0
	$(cc) -o tp0 mem_manager.o tp0.o second_chance_vm.o
	@echo Done

tp1 : mem_manager.o tp1.o second_chance_vm.o
	@echo Linking tp1
	$(cc) -o tp1 mem_manager.o tp1.o second_chance_vm.o
	@echo Done
    
tp2 : mem_manager.o tp2.o add_ref_vm.o
	@echo Linking tp2
	$(cc) -o tp2 mem_manager.o tp2.o add_ref_vm.o
	@echo Done

mem_manager.o : mem_manager.cpp mymalloc.h
	@echo Compiling mem_manager.cpp
	$(cc) -g -c mem_manager.cpp

second_chance_vm.o : second_chance_vm.cpp mymalloc.h
	@echo Compiling second_chance_vm.cpp
	$(cc) -g -c second_chance_vm.cpp

add_ref_vm.o : add_ref_vm.cpp mymalloc.h
	@echo Compiling add_ref_vm.cpp
	$(cc) -g -c add_ref_vm.cpp
 
tp0.o : tp0.c mymalloc.h
	@echo Compiling tp0.c
	$(cc) -g -c tp0.c

tp1.o : tp1.c mymalloc.h
	@echo Compiling tp1.c
	$(cc) -g -c tp1.c

tp2.o : tp2.c mymalloc.h
	@echo Compiling tp2.c
	$(cc) -g -c tp2.c

clean :
	rm -f *.o tp? core *~ *dat

submit :
	@echo Submitting...
	tar cvf 411.tar mem_manager.cpp mymalloc.h second_chance_vm.cpp add_ref_vm.cpp  
	/home/users1/cg411/bin/turnin 2 411.tar
