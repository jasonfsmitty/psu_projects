/*
	Jason F. Smith  4588
	Mike Schreiber  5259

	CSE 411 Project 1
*/

#ifndef  SEM_T
#define  SEM_T
struct sem_t{
	void* blocked;
	int pending_signals;
};
#endif

extern void t_init ()  ;
extern void t_shutdown () ;
extern int t_create ( void (*func)(int) , int val , int pri) ;
extern int t_terminate () ;
extern void t_yield () ;

extern int sem_init( sem_t *sp, unsigned int sem_count );
extern void sem_wait( sem_t *sp );
extern void sem_signal( sem_t *sp );
extern void sem_destroy( sem_t *sp );
