#include <stdio.h>
#include <stdlib.h>
#include "411_threads.h"

sem_t s;
int resource = 0;

void read_function ( int val )
{
  long i, j ;
  int res_before, res_after;
  for ( i = 0 ; i < 100 ; i++ ) 
    {
      sem_wait(&s);
            
      printf ( "I am thread %d\n" , val ) ;
      res_before = resource;
      printf("The value of stored variable is %d\n", resource);
      for (j = 0; j < 10000000; j++);
      t_yield();
      res_after = resource;
      if (res_before!= res_after){
	fprintf(stderr, "error in the semaphore program \n");
	exit(-1);
      }
      sem_signal(&s);
      
    }
  t_terminate () ;
}

void write_function ( int val )
{
  long i, j ;
  for ( i = 0 ; i < 100 ; i++ ) 
    {
      sem_wait(&s);
            
      printf ( "I am thread %d\n" , val ) ;

      resource = rand()%100;      
      printf("The value of stored variable is changed to %d\n", resource);
      for (j = 0; j < 10000000; j++);
      
      sem_signal(&s);
      
      t_yield () ;
    }
  t_terminate () ;
}

/*---------------------------------------------------------------------------
// main () 
// 
//-------------------------------------------------------------------------*/
main ()
{

  int i ;

  t_init () ;
  sem_init(&s, 1);
  
  t_create ( write_function , 1 , 1 ) ;
  t_create ( write_function , 2 , 1 ) ;
  t_create ( read_function , 11 , 1 ) ;
  t_create ( read_function , 12 , 1 ) ;
  t_create ( write_function , 3 , 1 ) ;
  t_create ( write_function , 4 , 1 ) ;
  t_create ( read_function , 13 , 1 ) ;
  t_create ( read_function , 14 , 1 ) ;
  
  for ( i = 0 ; i < 150000 ; i++ ) 
    {
      printf ( "I am main\n" ) ;
      t_yield () ;
    }
  sem_destroy(&s);
  t_shutdown () ;
}

