//	Jason F. Smith			Mike Schreiber		CSE 411


// PROGRAM NAME : second_chance_vm.cc
// contains code which implements the virtual memory 
// Last updated : Mar 2, 2:30 pm

#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "mymalloc.h"



void	sighandler(int sig){};

void	pg_translate( const int index );
// returns the physical address of the virtual address

void  my_read(const char *src, int size, char *dest)
{
	int index= ( src - mem_start)/PAGESIZE;
	pg_translate( index );

	src=( vm_pages[index].pm_address+(src-mem_start)%PAGESIZE);

	for( int i=0; i< size; i++ )
		dest[i]=*(src+i);
}

void  my_write(char *dest, int size, const char* src)
{
	int index=( dest - mem_start )/PAGESIZE;
	
	pg_translate( index );

	dest=( vm_pages[index].pm_address+(dest-mem_start)%PAGESIZE);

	for( int i=0; i < size; i++ )
		*(dest+i)=*(src+i);
	vm_pages[index].norm_ref_bits = 0x0f;
}


void	pg_translate( const int vm_index )
{

	if(  (vm_pages[vm_index].norm_ref_bits & VALID_BIT)==VALID_BIT )
	// it is alread in PM, so return.
	{
		// set reference bit
		vm_pages[vm_index].norm_ref_bits=( vm_pages[vm_index].norm_ref_bits | REF_BIT );
		return;
	}

	// otherwise we need to evict a page to make room
	if( pm_pages[(pm_index+1)%num_pm_pages]!=NULL ) 
		// if == NULL, then the page is empty
		// otherwise, we need to find a page to evict
	{
		while( (pm_pages[pm_index]->norm_ref_bits & REF_BIT) == REF_BIT )
		{
			pm_pages[pm_index]->norm_ref_bits=( pm_pages[pm_index]->norm_ref_bits & NOT_REF_BIT );
			// the reference bit is set to zero for second chance.
			pm_index= (pm_index+1)%num_pm_pages;
		}
		// as of this point, pm_index points to a page to evict

		if( (pm_pages[pm_index]->norm_ref_bits & DIRTY_BIT) == DIRTY_BIT )
		// then the page need written to disk
		{
			// write the page to the disk.
			cout <<"\n Writing page "<<pm_pages[pm_index]->index<<" to disk...\n\n";
			lseek( swapfile_id, (long int)(pm_pages[pm_index]->vm_address), SEEK_SET );
			write( swapfile_id, (char*)(pm_index*PAGESIZE+mem_start), PAGESIZE );

		}
		else // do not write to disk, but becomes an empty page
		{
			cout <<"\n Replacing page "<<pm_pages[pm_index]->index<<" without writing to disk...\n\n";
		}
		// the page is no longer valid, so turn off it's valid bit
		pm_pages[pm_index]->norm_ref_bits= ( pm_pages[pm_index]->norm_ref_bits & NOT_VALID );
		// along with not being valid, set the page pointer to NULL
		pm_pages[pm_index]=NULL;

	}// end of evict page to make room
	else
		pm_index= (pm_index+1)%num_pm_pages;

	// We now have a page cleared in PM and the entry in pm_pages[] is set to NULL
	
	// set the new value of pm_pages[pm_index]
	pm_pages[pm_index]= &vm_pages[vm_index];
	vm_pages[vm_index].pm_address = (char*) ( ( pm_index * PAGESIZE)+mem_start);
	

	//check if the new page need read in from the disk
	if( (vm_pages[vm_index].norm_ref_bits & ACCESSED ) == ACCESSED ) // then it needs read in from file
	{
		lseek( swapfile_id, (long int)(vm_index*PAGESIZE+mem_start), SEEK_SET );
		read( swapfile_id, pm_index*PAGESIZE+mem_start, PAGESIZE );
	}

	vm_pages[vm_index].norm_ref_bits=( VALID_BIT | ACCESSED | REF_BIT );

}

