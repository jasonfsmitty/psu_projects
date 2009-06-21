#include <iostream.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "disk.h"

//---------------------------------------------------------------------------
// class disk - implementation. 
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constructor and destructor
// 
// Create a file of required size, and fill it with zeros.
//--------------------------------------------------------------------------- 
disk::disk ()
{
  char buf[SECTORSIZE] ;
  num_track = TRACKS ;
  num_sector = SECTORS ;

  if ( -1 == (diskfile = open ( "./diskfile" , O_RDWR | O_CREAT | O_TRUNC ,
                                0600 ) ) )
    {
      cerr << "Could not open file, Exiting" << endl ;
      exit ( 1 ) ;
    }

  memset ( buf , '\0' , SECTORSIZE ) ;

  for ( int i = 0 ; i <  SECTORS * TRACKS ; i ++ )
    write ( diskfile , buf , SECTORSIZE ) ;
  lseek ( diskfile , 0 , SEEK_SET ) ;

}


disk::~disk ( ) 
{
  close ( diskfile ) ;
}


//---------------------------------------------------------------------------
// disk_read ( int track_number , int sector_number , char *buf ) 
//
// Seek to required block
// Read a block and copy into the buffer
//
//---------------------------------------------------------------------------
int disk::disk_read ( int track_number , int sector_number , char *buf )
{
  int count ;
  if ( track_number > num_track-1 || sector_number > num_sector-1 )
    {
      cerr << "Attempt to read track " << track_number << " Sector " 
	   << sector_number << ", Exiting" << endl ;
      exit (1) ;
    }
  int logical_sector = track_number * SECTORS + sector_number ;
  lseek ( diskfile , logical_sector * SECTORSIZE , SEEK_SET ) ;
  count = read ( diskfile , buf , SECTORSIZE ) ;
  return count ;
}

//---------------------------------------------------------------------------
// disk_write ( int track_number , int sector_number , char *buf )
//
// Seek to required block
// Copy from the buffer to the file
//
//---------------------------------------------------------------------------
int disk::disk_write ( int track_number , int sector_number , char *buf )
{
  int count ;
  if ( track_number > num_track-1 || sector_number > num_sector-1 )
    {
      cerr << "Attempt to read track " << track_number << " Sector " 
	   << sector_number << ", Exiting" << endl ;
      exit (1) ;
    }
  int logical_sector = track_number * SECTORS + sector_number ;
  lseek ( diskfile , logical_sector * SECTORSIZE , SEEK_SET ) ;
  count = write ( diskfile , buf , SECTORSIZE ) ;
  return count ;
}


