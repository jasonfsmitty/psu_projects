#include <stdio.h>
#include "411_threads.h"

/*---------------------------------------------------------------------------
// Test Program #2
// This program tests the scheduler
//-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
// void thread_func(int val)
//
// The user function - body of thread.
//-------------------------------------------------------------------------*/
void thread_function ( int val )
{
  for ( ; ; ) 
    {
      printf ( "I am thread %d\n" , val ) ;
    }
  /* Never reached */
}

/*---------------------------------------------------------------------------
// main () 
// 
//-------------------------------------------------------------------------*/
main ()
{

  t_init () ;
  t_create ( thread_function , 1 , 1 ) ;
  t_create ( thread_function , 2 , 1 ) ;
  t_create ( thread_function , 3 , 1 ) ;
  t_create ( thread_function , 4 , 1 ) ;
  
  for ( ; ; ) 
    {
      printf ( "I am main\n" ) ;
    }

  /* Never reached */
}

