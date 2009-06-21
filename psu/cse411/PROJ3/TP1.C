#include <iostream.h>
#include "filesys.h"

//---------------------------------------------------------------------------
// test program 1 
// 
// Checks the basic functions - create, delete, open, close, read, write, seek 
// * Creates a data pattern in buffer
// * writes to a file
// * Reads data from the file, and checks if the data read back was the same
//
//--------------------------------------------------------------------------- 
main ( int argc , char **argv ) 
{
  char buf[1000] , ch ;
  int i , fp , result ;   
   
  // Init the filesystem
  fs_init () ;

  // Fill up the buffer with data
  for ( i = 0 ; i < 1000 ; i ++ ) buf[i] =  'A' + (i % 52) ;

  // Create a file
  result = fs_create ( "myfile1.txt" ) ; 
  if ( result == 0 ) 
   { cout << "File creation failed" << endl ; exit(1) ; }

  // Write 510 bytes into the file
  fp = fs_open ( "myfile1.txt" , "w" ) ;
  if ( fp == -1 )
   { cout << "First file open failed" << endl ; exit (1) ; }
  fs_write ( fp , buf , 510 ) ;
  fs_close ( fp ) ;

  // Write 490 bytes into the file
  fp = fs_open ( "myfile1.txt" , "rw" ) ;
  if ( fp == -1 )
   { cout << "Second file open failed" << endl ; exit (1) ; }
  fs_seek(fp, 510);
  fs_write ( fp , buf + 510 , 490 ) ;
  fs_close ( fp ) ; 

  // Open file in read mode
  fp = fs_open ( "myfile1.txt" , "r" ) ;
  if ( fp == -1 ) 
   { cout << "Third file open failed" << endl ; exit (1) ; }

  
  // Read all the characters back, check for consistency 
  for ( i = 0 ; i < 1000 ; i ++ )
   { 
     fs_read ( fp , &ch , 1 ) ;
     if ( ch != buf[i] ) 
       { cout << "Data corrupted at offset " << i << endl ; exit ( 1 ) ; }
   }

  // Delete the file 
  fs_close ( fp ) ;
  fs_delete ( "myfile1.txt" ) ;

  // Try opening it again - should be an error
  fp = fs_open ( "myfile1.txt" , "r" ) ;
  if ( fp != -1 )
   { cout << "Opened a deleted file !" << endl ; exit (1) ; }

  // OK, you passed the test
  fs_shutdown ( ) ;
  cout << "Passed Test1 " << endl ;
}
