#include <iostream.h>
#include <unistd.h>

#include <setjmp.h>
#include <signal.h>


void handler( int sig )
{
	cout <<"called handler...\n";
	switch( sig ){
		case SIGALRM:
		cout <<"recieved an alarm interrupt\n";
		exit(1);
		break;
		default:
			cout <<"Recieved some other interrupt.";
			break;
	};
	exit(1);
}

void test_function()
{

	alarm(5);
	int sigmask( SIGALRM );
	int sigblock( SIGALRM);
	int sigsetmask( SIGALRM );

	for(  int i =0; i<60; i++ )
	{
		cout <<"In test loop...\n";
		sleep(1);
		cout <<"After sleep...\n";
}
	
}

int main()
{
	(void) sigset( SIGALRM, handler );
	
	
	test_function();
	(void ) sigset( SIGALRM, handler );
	cout <<"passed the test function...\n";

	alarm(1);
	for( int t=0; t<20; t++ )
			sleep(1);

	cout <<"Exited without and alarm.";

	return 0;
}
