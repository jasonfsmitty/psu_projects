/* 
   Code which tests the memory allocation subsystem only 
   Contains a call to function dump_out which allows examining
   information about the Buddy System

   Last Updated : Mar 2, 2:30 pm
*/
#include "mymalloc.h"
 
#define PHYSICAL_MEM_LIMIT 4
#define MEMORY_SIZE 1 /* The size of memory in MB */

#define NUM_BUFS 5
#define SMALL_BUF_SIZE 32
#define LARGE_BUF_SIZE 1024

char *sbuf[NUM_BUFS], *lbuf[NUM_BUFS];
char temp_buffer[LARGE_BUF_SIZE];



main()
{
  int i, j;

  my_init(MEMORY_SIZE, PHYSICAL_MEM_LIMIT);
  dump_out();
  for (i = 0; i < NUM_BUFS; i++) {
    sbuf[i] = (char *)my_malloc(SMALL_BUF_SIZE);
    lbuf[i] = (char *)my_malloc(LARGE_BUF_SIZE);
    printf("The buffer small pointers is %x, and large pointer is %x\n", (unsigned)sbuf[i], (unsigned)lbuf[i]);
  }
  dump_out();
  
  for (i = 0; i < NUM_BUFS; i++) {
    my_free((void *) sbuf[i]);
    my_free((void *) lbuf[i]);
  }
  dump_out();

  my_terminate();
}
