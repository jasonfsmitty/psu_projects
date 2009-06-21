//	Jason F. Smith		Mike Schreiber		CSE 411

// PROGRAM NAME : add_ref_vm.cc
// contains code which implements the virtual memory 
// Page replacement uses Additional reference bits algorithm
// Last updated : Mar 2, 2:30 pm

#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "mymalloc.h"


long int	remainder;

#define		enable(A)	sigrelse(SIGALRM)//ualarm(remainder,0)
#define		disable(A)	sighold(SIGALRM)//(remainder=ualarm(0,0))


unsigned int		order=0;

#define	FIRST_IN	0xf000
#define	SECOND_IN	0x0f00
#define	THIRD_IN	0x00f0
#define	LAST_IN		0x000f

void	pg_translate( const int index );

void sighandler(int sig)
{
	switch(sig){
		case SIGALRM: 
			for(int i=0; i<num_pm_pages; i++)
			{
		
				if(pm_pages[i] != NULL)
				{
					pm_pages[i]->used =0; ( pm_pages[i]->used >> 1);

					pm_pages[i]->add_ref_bits = (((pm_pages[i]->norm_ref_bits &REF_BIT)<<5)|(pm_pages[i]->norm_ref_bits >> 1 ));
					pm_pages[i]->norm_ref_bits = (pm_pages[i]->norm_ref_bits | ACCESSED | DIRTY_BIT | VALID_BIT );
					printf(" Now Ref bit for VM page %d is %d \n",pm_pages[i]->index,(int)(pm_pages[i]->add_ref_bits) );
				}
			}//for
		break;
	default :
		break;

	};//switch	
	ualarm(TIMER,0);
	
}


void  my_read(const char *src, int size, char *dest)
{
	int index= ( src - mem_start)/PAGESIZE;
	printf("\nReading from %d\n", index );

	disable(SIGALRM);
	pg_translate( index );
	enable(SIGALRM);

	src=( vm_pages[index].pm_address+(src-mem_start)%PAGESIZE);

	for( int i=0; i< size; i++ )
		dest[i]=*(src+i);
}

void  my_write(char *dest, int size, const char* src)
{

	int index=( dest - mem_start )/PAGESIZE;
	printf("\nWriting to %d\n",index);

	disable(SIGALRM);
	pg_translate( index );
	enable(SIGALRM);

	dest=( vm_pages[index].pm_address+(dest-mem_start)%PAGESIZE);

	for( int i=0; i < size; i++ )
		*(dest+i)=*(src+i);
	vm_pages[index].norm_ref_bits = 0x0f;
}


void	pg_translate( const int vm_index )
{

	vm_pages[vm_index].used++;

	for( int w=0; w<num_pm_pages; w++ )
	{
		if( pm_pages[w]!=NULL )
			pm_pages[w]->add_ref_bits= ( pm_pages[w]->add_ref_bits/2 );
	}

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
		for( int k=0; k<num_pm_pages; k++ )
		//time to pick the page to evict;
		{

			if( (int)(pm_pages[k]->add_ref_bits) < (int)(pm_pages[pm_index]->add_ref_bits) )
			{
				pm_index=k;
			}
			else if( (int)(pm_pages[k]->add_ref_bits) == (int)(pm_pages[pm_index]->add_ref_bits) )
				pm_index=k;

/*
			if( pm_pages[k]->used < pm_pages[pm_index]->used)
				pm_index=k;

			else if( pm_pages[k]->add_ref_bits < pm_pages[pm_index]->add_ref_bits )
			{
				pm_index=k;
			}
			else if( pm_pages[k]->add_ref_bits == pm_pages[pm_index]->add_ref_bits )
			{
				if( pm_pages[k]->order > pm_pages[pm_index]->order )
					pm_index=k;
			}

*/
		}		

		// as of this point, pm_index points to a page to evict
		cout<<"Victim page "<<pm_pages[pm_index]->index<<" in frame "<<pm_index<<endl; 
		if( (pm_pages[pm_index]->norm_ref_bits & DIRTY_BIT) == DIRTY_BIT )
		// then the page need written to disk
		{
			// write the page to the disk.
			printf("Writing page to disk...\n");

			lseek( swapfile_id, (long int)(pm_pages[pm_index]->vm_address-mem_start),SEEK_SET );
			write( swapfile_id, (char*)(pm_index*PAGESIZE+mem_start), PAGESIZE );

		}
		else // do not write to disk, but becomes an empty page
		{
			printf("\n Replacing page %d without writing to disk...\n\n",pm_pages[pm_index]->index );

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
		lseek( swapfile_id, (long int)(vm_index*PAGESIZE), SEEK_SET );
		read( swapfile_id, pm_index*PAGESIZE+mem_start, PAGESIZE );
	}

	vm_pages[vm_index].norm_ref_bits=( VALID_BIT | ACCESSED | REF_BIT );
	vm_pages[vm_index].add_ref_bits=0x0080;


	for( int j=0; j<num_pm_pages; j++)
	{
		if( pm_pages[j]!=NULL )
		{
			pm_pages[j]->order++;
//			pm_pages[j]->add_ref_bits=(pm_pages[j]->add_ref_bits >> 1 );
		}
	}
	vm_pages[vm_index].order=0;
	vm_pages[vm_index].used=1;

}
