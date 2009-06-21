/* Last Update on Feb 27 */

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
      printf("\nThe first 10 characters of small buffer are  %s",temp_buffer);
    }

  for (i = 0; i < NUM_BUFS; i++)
    {
      my_read(lbuf[i], 10, temp_buffer);
      printf("\nThe first 10 characters of large buffer are %s",temp_buffer);
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

  dump_out();
  for (i = 0; i < NUM_BUFS; i++)
    {
      sbuf[i] = (char *)my_malloc(SMALL_BUF_SIZE);
      lbuf[i] = (char *)my_malloc(LARGE_BUF_SIZE);
      printf("The buffer small pointers is %x and large pointer is %x.\n",(unsigned)sbuf[i],(unsigned)lbuf[i] );
      dump_out();
    }
 
  for (i = 0; i < NUM_BUFS; i++)
    {
      memset(temp_buffer, 'A'+i, LARGE_BUF_SIZE);
      my_write(sbuf[i], SMALL_BUF_SIZE, temp_buffer);
      my_write(lbuf[i], LARGE_BUF_SIZE, temp_buffer);
      my_read(lbuf[i], LARGE_BUF_SIZE, temp_buffer);
    }

  read_start();

  my_read(lbuf[NUM_BUFS-1], LARGE_BUF_SIZE, temp_buffer);
  my_write(lbuf[0], LARGE_BUF_SIZE, temp_buffer);

  my_read(lbuf[NUM_BUFS - 1], LARGE_BUF_SIZE, temp_buffer);
  my_write(lbuf[1]+5, 3, temp_buffer);

  my_read(sbuf[NUM_BUFS-1], SMALL_BUF_SIZE, temp_buffer);
  my_write(sbuf[0], SMALL_BUF_SIZE, temp_buffer);

  my_read(sbuf[NUM_BUFS - 1], SMALL_BUF_SIZE, temp_buffer);
  my_write(sbuf[1]+5, 3, temp_buffer);

  read_start();

  for (i = 0; i < NUM_BUFS; i++)
    {
      memset(temp_buffer, 'Z'-i, LARGE_BUF_SIZE);
      my_write(sbuf[i] + 5, SMALL_BUF_SIZE - 5, temp_buffer);
      my_read(sbuf[i], SMALL_BUF_SIZE, temp_buffer);
      my_write(lbuf[i] + 5, LARGE_BUF_SIZE - 5, temp_buffer);
    }

  read_start();

  for (i = 0; i < NUM_BUFS; i++) {
    my_free((void *) sbuf[i]);
    my_free((void *) lbuf[i]);
  }
  dump_out();

//  my_terminate();
}
