#include "disk.h"
#include <stdio.h>

main () 
{
  int j,count, t=4, s=3 ;
  char buf [SECTORSIZE] ;
  
  // Create a disk
  disk *mydisk = new disk () ;

  for(j=0;j<SECTORSIZE;j++)
	buf[j]='M';  
  // Assign values to t (track) and s(sector) 

  // Read and write - a sector at a time
  count = mydisk->disk_write ( t , s , buf ) ;
  count = mydisk->disk_read  ( t , s , buf ) ;
  printf("%s \n", buf);

  // Cleanup
  delete mydisk ;

}
