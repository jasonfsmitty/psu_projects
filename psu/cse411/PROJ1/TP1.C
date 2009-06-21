#include <stdio.h>
#include "411_threads.h"

/*---------------------------------------------------------------------------
// void thread_func(int val)
//
// The user function - body of thread.
//-------------------------------------------------------------------------*/
void thread_function ( int val )
{
  int i ;
  for ( i = 0 ; i < 10 ; i++ ) 
    {
      printf ( "I am thread %d\n" , val ) ;
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
  t_create ( thread_function , 1 , 1 ) ;
  t_create ( thread_function , 2 , 1 ) ;
  t_create ( thread_function , 3 , 1 ) ;
  t_create ( thread_function , 4 , 1 ) ;
  
  for ( i = 0 ; i < 15 ; i++ ) 
    {
      printf ( "I am main\n" ) ;
      t_yield () ;
    }

  t_shutdown () ;
}

