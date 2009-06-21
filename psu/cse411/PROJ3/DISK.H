//---------------------------------------------------------------------------
// Class disk - the disk simulator
// 
//---------------------------------------------------------------------------

#ifndef DISK_H
#define DISK_H

#define SECTORS 40
#define TRACKS  9
#define SECTORSIZE 512

class disk 
{
  public :

  disk () ; 

  ~disk () ;

  int disk_read  ( int track_number , int sector_number , char *buf ) ;
  int disk_write ( int track_number , int sector_number , char *buf ) ;

  private :

  int diskfile ;
  int num_track , num_sector ;

} ;
#endif
