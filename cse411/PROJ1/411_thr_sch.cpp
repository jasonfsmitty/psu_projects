/*******************************************************************************
		Jason F. Smith 4588			Mike Schreiber 5259
		       CSE 411	 			Project 1	
********************************************************************************/
#include <iostream.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <ucontext.h>

#include "jfsqueue.h"

#define	  MIN_PRIORITY           0
#define   MAX_PRIORITY          10
#define   MAIN_PRIORITY          1

#define   ALARM_TIME  		1 	// for a one second interrupt
#define   ALARM_OFF		0	// value to turn the alarm off

static void signal_handler( int sig );
void t_error( char* problem );


//void enable( void ) {};
//void disable( void ) {};

#define  enable(); 	alarm(ALARM_TIME);
#define	 disable();	alarm(ALARM_OFF);


struct PCB{
	ucontext_t *data;  // The context of the thread
	int id;		   // The unique thread id
	int priority;     //  The priority for the scheduling
};


class PCB_tracker{
	private:
	int size;
	JFS_Queue levels[ MAX_PRIORITY - MIN_PRIORITY + 1 ];
   public:
   		PCB_tracker( void );
		~PCB_tracker( void );

		void add_PCB( PCB *add );

		PCB* get_next( void );

		void clear_all( void );

		int get_size( void );
		int is_empty( void );
};


struct sem_t{
	PCB_tracker blocked;
	int pending_signals;
};


PCB_tracker 	*pcb_ready=NULL;	// The main ready queue
int 		main_id;		// The id of the main function.
int 		next_thread_id=0;	// keep track of ids given out
PCB* 		current=NULL; 		// the current thread


//*******************************************************************
void t_init()
{
	
	if( pcb_ready!=NULL )//Library already initialized.
		t_error("t_init: The library is already initialized or needs shutdown first.");

	pcb_ready=new PCB_tracker;

   	current=new PCB;
	current->priority=MAIN_PRIORITY;//the minimun priority of our system
	main_id=next_thread_id++;
	current->id=main_id;

	current->data=new ucontext_t;
	getcontext( current->data );

	(void) sigset( SIGALRM, signal_handler ); // Alarm interrupt.
	(void) sigset( SIGINT,  signal_handler ); // Control C interrupt.
	enable();

	alarm( ALARM_OFF );//only one thread right now.
}

//*******************************************************************
void t_shutdown()
{
	disable();

	if( current->id !=main_id )
		t_error( "t_shutdown: The current thread is the not thread which initialized the library." );

	if( pcb_ready==NULL )
		t_error( "t_shutdown: The library is already shutdown or not it is initialized.");

	pcb_ready->clear_all();
	delete pcb_ready;
   	pcb_ready=NULL;
   	if( current!=NULL )    
	{
   		delete current->data;
      		delete current;
		current=NULL;
	}
	enable();
	alarm( ALARM_OFF );  //turn off the alarm
}

//*******************************************************************
int t_create( void (*func)(int), int val, int pri)
{
	disable();

	PCB* temp;
	int  unique_thread_id;

	ucontext_t *n;  // the next context
	ucontext_t *c;  // the current context.

   	if( pcb_ready==NULL )
   		t_error( "t_create: The library must be initialized first." );

	temp=new PCB;

	if( pri<MIN_PRIORITY )
   		pri=MIN_PRIORITY;
   	else if( pri>MAX_PRIORITY )
   		pri=MAX_PRIORITY;

   	temp->priority=pri;
   	temp->id=next_thread_id++;
   	unique_thread_id=temp->id;

	temp->data=new ucontext_t;
   	getcontext( temp->data );
   	makecontext( temp->data, (void(*)(...))func, 2, val );

   	c=current->data;   //so we can save the current context.
   	pcb_ready->add_PCB( current );

   	current=temp;
   	n=current->data;  //we have the next thread, get the context.

	enable();
   	swapcontext( c, n );

   	return (unique_thread_id);
}

//*******************************************************************
int t_terminate()   
{
	disable();

	if( pcb_ready==NULL )
		t_error( "t_terminate called: no library exists." );

	if( pcb_ready->is_empty() )
	   	return 0;

	delete current->data;
	delete current;
	if( ( current=pcb_ready->get_next() ) == NULL )//the next thread.
		t_error( "t_terminate: The library could not find another thread to switch to." );

	enable();
	setcontext( current->data );

	return 0; //Never reached, hopefully.
}

//*******************************************************************
void t_yield( void )    
{
	disable();

	ucontext_t *c, *n;
  	PCB *temp;

	if( pcb_ready==NULL )
		t_error( "t_yield: Library is not initialized yet." );
	if ( pcb_ready->is_empty() )
		return;		//There are no other threads to jump to.
	if( (temp=pcb_ready->get_next() )==NULL )
		t_error("t_yield: The library could not find another thread to switch to." );

	n=temp->data;
	c=current->data;
	pcb_ready->add_PCB( current );// Put it back into the queue.

	current=temp;

	enable();

	swapcontext( c, n );
}


//*******************************************************************
int sem_init( sem_t *sp, unsigned int sem_count )
{
	disable();
	if ( pcb_ready==NULL )// the thread library was not initialized.
		t_error( "sem_init: The thread library must be initialized first." );
	(*sp).pending_signals=sem_count;;
	enable();
}

//*******************************************************************
void sem_wait( sem_t *sp)
{
	disable();

	ucontext_t  *c;
	if( pcb_ready==NULL )
			t_error( "sem_wait: The semaphore must be initialized first." );

	(*sp).pending_signals--;
	if( ( (*sp).pending_signals)<0 )   {
		c=current->data;
		(*sp).blocked.add_PCB( current );	
		current=pcb_ready->get_next();
		swapcontext( c, current->data );
	}

	enable();
}

//*******************************************************************
void sem_signal( sem_t *sp )
{
	disable();

	if( pcb_ready==NULL )
		t_error( "sem_signal: The thread library must be initialized first." );

	ucontext_t *c;
	
	(*sp).pending_signals++;
	if( (*sp).pending_signals<=0 )  {	
	
		c=current->data;
		pcb_ready->add_PCB( current );
		current=(*sp).blocked.get_next();		
		swapcontext( c, current->data );
	}
	enable();
}

//*******************************************************************
void sem_destroy( sem_t *sp )
{
	disable();
	(*sp).blocked.clear_all();
	enable();
}

//*******************************************************************
void t_error( char* problem )
{
	cout 	<<"\n\nThread Library error in...\n"<<problem
        	<<"\nExiting...\n\n";
	exit(1);
}

//*******************************************************************
static void signal_handler( int sig )
{
	switch( sig)	
	{
		case SIGALRM:
			t_yield();
			break;
		case SIGINT:
			if( pcb_ready==NULL )
			{
				cout <<"\n\n  Control-C interrupt, exiting...\n\n";			
				exit(1);
			}
			else if( main_id != current->id )
				t_terminate();
			else
				t_shutdown();
			break;
		default:
			break;
	};
}

//*******************************************************************
PCB_tracker::PCB_tracker( void )
{
	size=0;
}

PCB_tracker::~PCB_tracker( void )
{
	clear_all();
}

void PCB_tracker::add_PCB( PCB* add )
{
	int p= (add->priority)- MIN_PRIORITY;
	levels[p].enqueue( add );
	size++;
}

PCB* PCB_tracker::get_next( void )
{
	if( size==0)
		return NULL;
	size--;// should find a PCB

	for( int i=MAX_PRIORITY-MIN_PRIORITY; i>=0; i-- )
	{
		if( levels[i].get_size() > 0 )
			return ( levels[i].dequeue() );
	}
	return NULL;//error in the size probably
}

void PCB_tracker::clear_all( void )
{
	PCB *temp;
	for( int i=0; i<=MAX_PRIORITY-MIN_PRIORITY; i++ )
	{
   		while( !levels[i].is_empty() )
		{
      			temp=levels[i].dequeue();
			delete temp->data;
        		delete temp;
      		}
	}
	size=0;
}

int PCB_tracker::get_size( void )
{
	return size;
}

int PCB_tracker::is_empty( void )
{
	if( size==0)
   		return 1;
	return 0;
}

/*********************************************************************
 **	Jason F. Smith           Mike Schreiber           	    ** 
 **	       CSE 411           Project 1           		    **	
 *********************************************************************/
