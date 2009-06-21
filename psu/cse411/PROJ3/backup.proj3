/*==============================================================================

        Jason F Smith  4588
	Mike Schreiber  5259
        CSE 411  Project #3
	
==============================================================================*/

#include <iostream.h>
#include <string.h>
#include "filesys.h"
#include "disk.h"


#define  BLOCKSIZE   SECTORSIZE

// defines for the FAT entries====================================
#define  UNUSED         0
#define  END_OF_FILE    -1

// defines for the directory entries  ============================
#define  NO_BLOCKS     -1
#define  MAX_NAME      20

// defines for the type of access allowed on an open file=========
#define    READ         2
#define    WRITE        4  

// A structure to contain the data of each file in the directory==
struct Dir_Entry{
       char         filename[ MAX_NAME ];
       int          start_block;
       int          size;
       Dir_Entry    *next;
};


// A data structure for an entry in the open file table =================
struct Open_File_Entry{
       int fd;                  // the file descripter
       int cursor_offset;       // The current cursor offset into the file
       int block_num;           // the block number the cursor is in
       char mode;               // The access mode the file has been opened as
       Dir_Entry* dir_entry;
       Open_File_Entry *next;
};

// Global Variables ==========================================================
// Array for the FAT  
int fat [ TRACKS*SECTORS ];

Open_File_Entry* open_files;  // the open file 'table'
Dir_Entry *root_dir;   // the linked list directory structure.  Every file has an entry here.
disk my_disk; // the disk used by the file system.

void write_fat_dir( void );
// saves the FAT and the root directory whenever there is a change.

//========================================================================
//     Initialize the disk:  Return 1 for success, 0 for failure
int fs_init ( )
{
    root_dir=NULL;    
    open_files=NULL;
    for( int i=2; i<TRACKS*SECTORS; i++ )
         fat[i]=UNUSED;
    fat[0]=END_OF_FILE; // reserved for the FAT
    fat[1]=END_OF_FILE; // root directory.

    write_fat_dir();

    return 1;
}
//========================================================================
//      Shutdown the file system: 1 -success, 0 - failure
int fs_shutdown ( )
{
   write_fat_dir();

   Open_File_Entry* temp;
   while( open_files!=NULL ){
       temp=open_files;
       open_files=temp->next;
       delete temp;
   }
   Dir_Entry* t;
   while( root_dir!=NULL ){
       t=root_dir;
       root_dir=t->next;
       delete t;
   }
}

//========================================================================
//   Create a file of name fname   1 - success, 0 - failure
int fs_create ( char *fname )
{
    Dir_Entry* temp;
    temp=root_dir;
    while( temp!=NULL ){
        if( strcmp( temp->filename, fname ) == 0 )
            return 0;
        temp=temp->next;
    }
    temp=new Dir_Entry;
    strcpy( temp->filename, fname );
    temp->next=root_dir;
    temp->size=0;
    temp->start_block=NO_BLOCKS;
    root_dir=temp;

    write_fat_dir();

    return 1;
}

//=========================================================================
//   Delete a file of name fname.   1 - success,  0 - failure
int fs_delete ( char *fname)
{
     Dir_Entry *temp, *t;
     Open_File_Entry *t2;
     // check that the file isn't open.
     t2=open_files;
     while( t2!=NULL ){
         if( strcmp( t2->dir_entry->filename, fname )==0 ){
             cout <<" Error deleting, file is open.";
             return 0;
         }
         t2=t2->next;
     }
     temp=root_dir;
     while( temp!=NULL ){
         if( strcmp( temp->filename, fname )==0 )
             break;
         temp=temp->next;
     }
     if( temp==NULL )
         return 0;  // could not find the file to delete
     if( temp==root_dir ){
        root_dir=temp->next;
     }
     else{
        t=root_dir;
        while( t->next != temp ){
            t=t->next;
        }
        t->next=temp->next;
     }
     if( temp->size>0 ){
         int i;
         int index=temp->start_block;
         while( index!=END_OF_FILE ){
            i=index;
            index=fat[i];
            fat[i]=UNUSED;
         }
     }// end of deleting disk blocks

     write_fat_dir();

     delete temp;
}

//=========================================================================
//  Open a file with the specified mode.       fd - success (positive), -1 for failure
//  Valid values for mode are: "r", "w", "rw", and "a"
int fs_open ( char *fname , char *mode )
{
    static int file_desc=1;
    int index;

    Dir_Entry *temp;
    temp=root_dir;
    while( temp!=NULL ){
         if( strcmp( temp->filename, fname )==0 )
             break;
         temp=temp->next;
    }
    if( temp==NULL )
         return -1;
    Open_File_Entry *o;
    o=new Open_File_Entry;
    o->fd = file_desc++;
    o->dir_entry=temp;
    if( mode[0]=='a' ){
        o->mode = WRITE;
        o->cursor_offset = o->dir_entry->size;
        // set the block num to the correct block
        index=o->dir_entry->start_block;
        int i;
        for( i=1; i*BLOCKSIZE < o->cursor_offset; i++ ){
             index=fat[index];
        }
        o->block_num=index;
    }
    else if( mode[0]=='r' ){
        o->mode = (READ);
        if( mode[1]=='w' )
            o->mode = READ | WRITE;
        o->cursor_offset=0;
    }
    else if( mode[0]=='w' ){
        o->cursor_offset=0;
        o->mode=WRITE;
	
        int index, i;
        index=o->dir_entry->start_block;
        while( index != NO_BLOCKS ){ // than delete the used blocks
            i=index;
            index=fat[i];
            fat[i]=UNUSED;
        }// end of delete the used blocks

        o->dir_entry->start_block=NO_BLOCKS;
        o->dir_entry->size=0;
    	write_fat_dir();
    }
    else{
       delete o;
       return -1;
    }

    // Enter it into the list of open files
    o->next=open_files;
    o->block_num=o->dir_entry->start_block; // not initialized yet.
    open_files=o;
    
    return o->fd;
}

//=========================================================================
//   Close the file.   1 - success,  0 - failure
int fs_close ( int fhandle )
{ // remove the open file from the list of open files
     Open_File_Entry *temp;
     temp=open_files;
     while( temp!=NULL ){
         if( temp->fd==fhandle ){
              // delete the entry from the table
              if( temp==open_files )
                   open_files=temp->next;
              else{
                   Open_File_Entry *t;
                   t=open_files;
                   while( t->next!=temp ) // find the previous entry
                          t=t->next;
                   // the previous entry is found, skip over temp
                   t->next=temp->next;
              }// end of delete from linked list
              delete temp;
              return 1; //success
         }//end of if found
     }// end of while !found
     // The file could not be found, that's an error..
     return 0;
}

//=========================================================================
//  Read the file.    Returns number of bytes read from the file.
int fs_read ( int fhandle , char *buf , int nbytes )
{

     char buffer[BLOCKSIZE];
     Open_File_Entry *entry;
     int count=0;  // number of bytes read
     // find the entry in the table
     entry=open_files;
     while( entry!=NULL ){
          if( entry->fd=fhandle )
              break;
          entry=entry->next;
     }//end of while !found loop
     if( entry==NULL ){
         cout <<" Error: File not opened.\n";
         return 0;
     }

     // at this point, the open file entry is found and pointed to by
     //  entry.  Need to check if a read is allowed
     if( !(entry->mode&READ) ){
	 cout <<"Error: read not allowed.\n";
         return 0;  // not able to read from the file!!
     }

     //read the bytes from the file
     // get to the correct block of the file
     if( entry->dir_entry->size <= entry->cursor_offset ){ 
	 //cout <<"Error: End of file.\n";
         return 0;
     }
     // find the track and sector number

     int track = ( entry->block_num) / SECTORS;
     int sector = ( entry->block_num ) % SECTORS;
     my_disk.disk_read( track, sector, buffer );

     for( count=0; count<nbytes && entry->cursor_offset < entry->dir_entry->size; count++ ){
          buf[count]=buffer[(entry->cursor_offset++)%BLOCKSIZE];

          if( entry->cursor_offset%BLOCKSIZE==0 ){ // it's pointing past the current block
               entry->block_num = fat[ entry->block_num ];
               if( entry->block_num == END_OF_FILE )
                  return ( count+1 );
               track = entry->block_num / SECTORS;
               sector = entry->block_num % SECTORS;
               my_disk.disk_read( track, sector, buffer );
          }
     }// end of read in data

     return (count);
}

//=========================================================================
//  write to the specified file.   Returns number of bytes written.
int fs_write ( int fhandle , char *buf , int nbytes )
{
     int track, sector, count;
     int index, i;
     char buffer[BLOCKSIZE];
     Open_File_Entry *entry;
     entry=open_files;
     while( entry!=NULL ){
          if( entry->fd=fhandle )
              break;
          entry=entry->next;
     }//end of while !found loop
     if( entry==NULL ){
	 //cout <<" Error, attempted a write to an unopened file.\n";
         return 0;
     }
     if( !(entry->mode&WRITE) ){
	 //cout <<" Error, attempted to write to a file not opened in write mode.\n";
         return 0;
     }

     if(entry->dir_entry->size%BLOCKSIZE==0){
         for(index=0; index<(TRACKS*SECTORS); index++ ) {
             if( fat[index] == UNUSED)
                 break;
         }//for
         if(index >= (TRACKS*SECTORS))
             return 0;

         if(entry->block_num==NO_BLOCKS){//if no blocks are assigned yet

             entry->dir_entry->start_block=index;
             entry->block_num=index;
             fat[index]=END_OF_FILE;
         }// end of first block
         else{ // need another block for the file

             fat[ entry->block_num ]=index; // next block
             entry->block_num=index;
             fat[index]=END_OF_FILE;
	     write_fat_dir();	
         }// end of adding a block
     }//if
     track = ( entry->block_num) / SECTORS;
     sector = ( entry->block_num ) % SECTORS;

     my_disk.disk_read( track, sector, buffer );

     for( count=0; count<nbytes && entry->cursor_offset <= entry->dir_entry->size; count++ ){
          buffer[(entry->cursor_offset++)%BLOCKSIZE]=buf[count];
          if( entry->cursor_offset > entry->dir_entry->size )
              entry->dir_entry->size=entry->cursor_offset;
          if( entry->cursor_offset%BLOCKSIZE==0 ){ // it's pointing past the current block
               if( fat[ entry->block_num ] == END_OF_FILE ){ // need to get another block to write to
                   for(index=0; index<(TRACKS*SECTORS); index++ ) {
                        if( fat[index] == UNUSED)
                             break;
                   }//for
                   if(index >= (TRACKS*SECTORS))
                        return (count+1); // could not find another block
                   fat[ entry->block_num ] = index; // set the next pointer of the fat list
                   entry->block_num = index;
                   fat[index]=END_OF_FILE;
		   write_fat_dir();
               }
               else { // more block in the file already
                   entry->block_num = fat[ entry->block_num ];
               }
               my_disk.disk_write( track, sector, buffer ); // write the buffer back to the disk
               track = entry->block_num / SECTORS;
               sector = entry->block_num % SECTORS;
               my_disk.disk_read( track, sector, buffer );
          }
     }// end of read in data
     my_disk.disk_write( track, sector, buffer );

     return (count);
}
//=========================================================================
//  Seek to the specified offset of a file.  Returns offset on success, -1 failure.
int fs_seek ( int fhandle , int offset )
{
     	Open_File_Entry *entry;
     	int index;

     	entry=open_files;
     	while( entry!=NULL ){
         	if( entry->fd=fhandle )
             		break;
        	entry=entry->next;
     	}//end of while !found loop
     	if( entry==NULL )
        	return (-1);// the entry could not be found
     	if( !(entry->mode&READ) )
     		return (-1);

     	if( entry->dir_entry->size < offset )
		offset = entry->dir_entry->size;

     	entry->cursor_offset=offset;
     	index=entry->dir_entry->start_block;
     	int i;
     	for( i=1; i*BLOCKSIZE < entry->cursor_offset; i++ ){
		index=fat[index];
     	}
     	entry->block_num=index;
     	return offset;
}


//====================================================================
//  Save the FAT and root directory data to the disk, needed whenever
//  there is a change.
void write_fat_dir( void )
{
	char buf[BLOCKSIZE]; //buffer used to write to the disk
	int i;
	char *c;
	c=(char *)fat; //the fat is an integer array
	
	// write the FAT to the disk
	for(i=0; i<(TRACKS*SECTORS*2) && i<BLOCKSIZE; i++)
		buf[i]=c[i];
	my_disk.disk_write(0, 0, buf);

	Dir_Entry *temp;
	temp=root_dir;
	char *b;
	b=buf;

	while(temp != NULL)
	{
  		strcpy(b,temp->filename);
  		b += MAX_NAME;
  		*(b++)=(char)((temp->start_block & 0xff00)>>8);
  		*(b++)=(char)(temp->start_block & 0x00ff);
  		*(b++)=(char)((temp->size & 0xff00)>>8);  
  		*(b++)=(char)(temp->size & 0x00ff);
  		if((b-buf+MAX_NAME+4) > BLOCKSIZE)
  		{
     			my_disk.disk_write(0, 1, buf);
     			b=buf;

  		}//if
  		temp=temp->next;
	}//while
  	my_disk.disk_write(0, 1, buf);
}
//  End of file  =====================================
