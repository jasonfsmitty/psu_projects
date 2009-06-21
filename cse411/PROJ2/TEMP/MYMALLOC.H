/*
  The header file for the second project of CSE 411
  Last Updated : Mar 2, 2:30 pm


		Jason F. Smith
		Mike Schreiber
		CSE 411
 */ 

#ifndef	_MYMALLOC_H
#define _MYMALLOC_H


#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

#include <fstream.h>

/* function prototypes */
int   my_init(int, int);
void *my_malloc(int);
int   my_free(void *);
void  my_read(const char*, int, char *);
void  my_write(char *, int, const char*);
void  sighandler(int);

/*
  dump_out is not a part of the interface.
  it has been added for explanatory purposes to show how the memory
  manager allocates memory.

  It is used only in tp0.c
*/

void dump_out(void);

/*
  The pagesize to be used by the VM system
  */
# define PAGESIZE 4096


//  Global Data added by Jason and Mike...

// Constant values used when testing/setting the valid, dirty, and reference bits of a page.
#define		CLEAR_BITS	0x00
#define		VALID_BIT	0x01
#define		DIRTY_BIT	0x02
#define		REF_BIT		0x04
#define		NOT_REF_BIT	0x0b	// used to clear the ref bit
#define		ACCESSED	0x08 // if the page has never been accessed, then it is not read in 1-accessed, 0-not accessed

#define		SET_BITS	0x0d
#define		NOT_VALID	0x0e

#define		TIMER		100

 		struct 		mem_node;	// Keeps track of allocated memory location and size (instead of headers)
struct Page		// Used to contain information on a page (Valid, dirty and ref bits, addresses )
{
	unsigned char		add_ref_bits;
	unsigned char		norm_ref_bits;
	char*		vm_address;
	char*		pm_address;
	int		index;
	unsigned int	order;
};

extern		int		num_pm_pages;
extern		int		num_vm_pages;

extern		mem_node*	usedList;	// A linked list of the mem_node structures
						//  this tracks the allocated memory of the system.
extern		int		pm_index;
extern		double		total_size;	// The total size of the memory block (in bytes).
extern		char*		mem_start;	// The starting address of the memory we will be using.

//typedef		Page_ptr	Page*;

extern		Page**	pm_pages;	// Points to the pages currently in physical memory
extern		Page*	vm_pages;	// Dynamically allocated array that contains info on each page in Virtual Memory.

//extern		fstream		swapfile;
extern		char*		filename;
extern		int		swapfile_id;

extern		void		error( char* problem );// default error messaging function.

#endif
