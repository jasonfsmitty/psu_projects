/* 
   Code for testing the memory allocation and the virtual memory 
   subsystem
   Compiled with second_chance_vm.o and executes the second chance
   page replacement algorithm

   Last Update : Mar 2, 2:45 lm
*/ 

#include "mymalloc.h"
 
#define PHYSICAL_MEM_LIMIT 4
#define MEMORY_SIZE 1 /* The size of memory in MB */

#define NUM_BUFS 10
#define SMALL_BUF_SIZE 400
#define LARGE_BUF_SIZE 4000


/* global variable declarations */
char *sbuf[NUM_BUFS], *lbuf[NUM_BUFS];
char temp_buffer[LARGE_BUF_SIZE];

void read_start(void)
{
  int i;
  temp_buffer[10] = '\0';

  for (i = 0; i < NUM_BUFS; i++)
    {
      my_read(sbuf[i], 10, temp_buffer);
      printf("\nThe first 10 characters of small buffer %d are %s",
	     i, temp_buffer);
    }

  for (i = 0; i < NUM_BUFS; i++)
    {
      my_read(lbuf[i], 10, temp_buffer);
      printf("\nThe first 10 characters of large buffer %d are %s",
	     i, temp_buffer);
    }
  printf("\n");
}

main()
{
  int i, j;

  my_init(MEMORY_SIZE, PHYSICAL_MEM_LIMIT);
	/* The MEMORY SIZE is in megabytes */
	/* PHYSICAL MEMORY SIZE is in number of system pages */
	/* The page size on Sparc 4 and Sparc 5 is 4Kb */

  // dump_out();
  for (i = 0; i < NUM_BUFS; i++)
    {
      sbuf[i] = (char *)my_malloc(SMALL_BUF_SIZE);
      lbuf[i] = (char *)my_malloc(LARGE_BUF_SIZE);
      printf("The buffer small pointers is %d, and large pointer is
%d\n", (unsigned)sbuf[i], (unsigned)lbuf[i]);
      // dump_out();
    }

 
  // exit(1);
  for (i = 0; i < NUM_BUFS; i++)
    {
      memset(temp_buffer, 'A'+i, LARGE_BUF_SIZE);
      my_write(sbuf[i], SMALL_BUF_SIZE, temp_buffer);
      my_write(lbuf[i], LARGE_BUF_SIZE, temp_buffer);
    }

  read_start();


  // cout << "end of the read_start routine \n";
  my_read(lbuf[NUM_BUFS-1], LARGE_BUF_SIZE, temp_buffer);
  my_write(lbuf[0], LARGE_BUF_SIZE, temp_buffer);

  my_read(lbuf[NUM_BUFS - 1], LARGE_BUF_SIZE, temp_buffer);
  my_write(lbuf[1]+5, 3, temp_buffer);

  my_read(sbuf[NUM_BUFS-1], SMALL_BUF_SIZE, temp_buffer);
  my_write(sbuf[0], SMALL_BUF_SIZE, temp_buffer);

  my_read(sbuf[NUM_BUFS - 1], SMALL_BUF_SIZE, temp_buffer);
  my_write(sbuf[1]+5, 3, temp_buffer);

  read_start();


  for (i = 0; i < NUM_BUFS; i++) {
    my_free((void *) sbuf[i]);
    my_free((void *) lbuf[i]);
  }

  my_terminate();
}
