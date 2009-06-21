//	Mike Schreiber		Jason F. Smith		CSE 411

/*
  PROGRAM : mem_manager.cc
  File with code for the memory manager
  Last Updated : Mar 2, 2:30 pm
  */


#include <iostream.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "mymalloc.h"

#define		Page_ptr	(Page*)

extern "C"{
void*	malloc( int );
void	free( char* );
}

// already defined as extern in mymalloc.h==========================================

	int		num_pm_pages; // number of physical pages in memory
	int		num_vm_pages; // number of virtual pages in implementation.
	double		total_size;

	char*		filename="swapfile.dat";
	int		swapfile_id; // file number descripter
	char*		mem_start;
	mem_node*	usedList;
	Page*		vm_pages;
	Page**		pm_pages;
	int		pm_index;	

struct mem_node{
	char*		address; // The address returned from the appropriate mymalloc() call
	int		size;	 // The number of bytes allocated.
	mem_node*	next;	//used for linked list implementation.
};


void	error( char* problem );

//====================================================================================

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Interface function definitions
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

// size is the virtual memory size in megabytes
// phy_pages is the number of pages in the physical memory
int   my_init(int size, int phy_pages)
{
	int i;

	mem_start=(char*)malloc( phy_pages*PAGESIZE );  // Allocate enough memory
					// to simulate the physical memory.
	if( mem_start==NULL )
		return -1;

	total_size= size*pow(2,20);	// store the total number of bytes in virtual mem.
	usedList=NULL;
	
	//initialize the virtual memory variables.
	num_pm_pages=phy_pages;
	num_vm_pages=total_size/PAGESIZE;

	pm_pages=new (Page*)[ num_pm_pages ];	// allocate an array of pointers to the pages in PM.
	vm_pages=new Page[ num_vm_pages ]; // allocate an array of the size of virtual mem
	
	for( i=0; i<num_vm_pages; i++)
	{
		vm_pages[i].norm_ref_bits=vm_pages[i].add_ref_bits=CLEAR_BITS;
		vm_pages[i].index=i;
		vm_pages[i].vm_address=mem_start + i*PAGESIZE;
	}

	for( i=0; i<num_pm_pages; i++ )
		pm_pages[i]=NULL; // set all pm pages to empty
	pm_index=-1;


	// open up the swapfile so that we have disk space to place the virtual pages not in PM.
	swapfile_id=open( filename, O_CREAT | O_RDWR, 0700 ); // open for input/output

	(void)sigset(SIGALRM,sighandler);	//Alarm Interrupt
	ualarm(TIMER,0);
	return 1;
}


// size is the memory requested in BYTES
void *my_malloc(int size)
{
	mem_node*	temp;
	mem_node*	ptr;
	char*		address;
	char		found=0;
	int	 	i;

	address=mem_start;

	// increase size until it is of a power of 2
	for( int i=0; i<=100; i++ )
	{
		if( pow(2,i) >= size )
		{
			size=pow(2,i);
			break;
		}
	}

	if( size > total_size )
		return NULL;	

	while( found==0 )
	{
		found=1;
		temp=usedList;
		while( temp!=NULL )
		{
			if(  (temp->address >= address && temp->address < address+size )
			|| ( temp->address+temp->size >= address && temp->address+temp->size < address )
			|| ( temp->address < address && temp->address+temp->size > address ))
			{
				found=0;
				temp=NULL;
				address+=size;
			}
			if( temp!=NULL )
				temp=temp->next;
		}// end of while( temp!=NULL ) loop
	}// end of while( !found)

	// now we have an address that does not interfer with any of the currently allocated chunks of memory.
	// Test to see if it is within our virtual memory address space though.

	if( (int)address > total_size+(int)mem_start || (int)(address+size) >= total_size+(int)mem_start )
		return NULL; // There is no space to store it.

	temp= new mem_node; // create a new node to enter the data into
	temp->address=address;
	temp->size=size;
	temp->next=NULL;

	//insert into the linked list
	if( usedList==NULL )
		usedList=temp;
	else
	{
		ptr=usedList;
		while( ptr->next!=NULL )
			ptr=ptr->next;
		ptr->next=temp;
	}
	return temp->address;
}

int   my_free(void *ptr)
{
	mem_node*	temp;
	mem_node*	t;

	temp=usedList;
	t=NULL;
	while( temp!=NULL)
	{
		// check item
		if( ptr >= temp->address  && ptr < temp->address+temp->size )
		{
			// delete the node from the list
			if( t==NULL )// then first node in list
			{
				usedList=usedList->next;
				delete	temp;
			}
			else// somewhere else in list
			{
				t->next=temp->next;
				delete	temp;
			}
			return 1; // for success.
		}
		t=temp; // t is the previous node pointer
		temp=temp->next;
	}
	error( "Did not find the memory to free in a my_free() call." );
	return -1;
}

void  dump_out(void)
{
	float		i;
	double		j;
	double		free, used;
	mem_node*	temp;

	temp=usedList;
	used=0;
	while( temp!=NULL )
	{
		used+=temp->size;
		temp=temp->next;
	}
	free=total_size-used;

	printf("\n\n   ***** Memory System Dump *****\n");

	for( i=total_size, j=0; i>=1; i=i/2, j++ )
	{
		temp=usedList;
		used=0;
		while( temp!=NULL )
		{
			if( temp->size==i )
				used++;
			temp=temp->next;
		}
		printf(" Size:  %f \t\tList[%f]\n   used:  % \t\t free: ",i, j, used );
		if( free-i >= 0 )
		{
			printf("1\n");
			free-=i;
		}
		else
			printf("0\n");
	}

}


void error( char* problem )
{
	printf("\n\nThere was an error found in the program:\n  %s\n",problem );
}

void my_terminate( void )
{
	free( mem_start);
	close( swapfile_id);
}
