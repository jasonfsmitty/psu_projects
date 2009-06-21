/*
	IntQueue.h

	Jason F Smith

	My own queue implementation for integers.

*/

#ifndef INT_QUEUE_H
#define INT_QUEUE_H

#include <stdio.h>

#ifdef DEBUG
#	define QUEUE_DEBUG
#	pragma message ("INFO - Integer queue class debug: ON")
#else
#	pragma message ("INFO - Integer queue class debug: OFF")
#endif

#define		MAX_INT_QUEUE_SIZE			64

//////////////////////////////////////////////////////////////////
class IntQueue{
	protected:
		int buffer[ MAX_INT_QUEUE_SIZE ];
		int start, end;
		int numItems;
		int limit;

	public:
		///////////////////////////////////////////////////////
		IntQueue( void ){
			memset( (void *)buffer, 0, sizeof( int[MAX_INT_QUEUE_SIZE] ) );
			start=end=numItems=0;
			limit = MAX_INT_QUEUE_SIZE;
		};

		///////////////////////////////////////////////////////
		~IntQueue( void ){ };

		///////////////////////////////////////////////////////
		void Reset( void )
		{
			start=end=numItems=0;
		};

		///////////////////////////////////////////////////////
		void Push( int add )
		{

			buffer[end++] = add;
			if( end>=MAX_INT_QUEUE_SIZE )
				end=0;
			if( (++numItems)>limit ){
				numItems--;
				if( (++start)>=MAX_INT_QUEUE_SIZE )
					start=0;
			}

		};


		///////////////////////////////////////////////////////
		int Pop( void ){
			int temp = -1;
			if( numItems>0 ){
				temp = buffer[start++];
				numItems--;
				if( start>=MAX_INT_QUEUE_SIZE )
					start=0;
			}
#if defined( QUEUE_DEBUG )
			else {
				cout <<"DEBUG - Invalid IntQueue::Pop() - no items in queue\n";
			}
#endif // QUEUE_DEBUG
			return temp;
		};


		///////////////////////////////////////////////////////
		bool IsFull( void ) { return (numItems==limit); };
		///////////////////////////////////////////////////////
		bool IsEmpty( void ) { return (numItems==0); };
		///////////////////////////////////////////////////////
		int  NumItems( void ){ return numItems; };
		
		///////////////////////////////////////////////////////
		int  Peek( void ){ return buffer[start]; };

		///////////////////////////////////////////////////////
		int operator[]( int index ){
			if( index>=numItems || index<0 ){
#if defined( QUEUE_DEBUG )
				cout <<"DEBUG - Invalid index in IntQueue::[](int)\n";
#endif // QUEUE_DEBUG
				return -1;
			}

			if( (start+index)<MAX_INT_QUEUE_SIZE ){
				return buffer[ start+index ];
			} else {
				return buffer[ start + index - MAX_INT_QUEUE_SIZE ];
			}
		};


		///////////////////////////////////////////////////////
		void SetLimit( int maxItems ){
			if( maxItems>0 && maxItems<MAX_INT_QUEUE_SIZE )
				limit=maxItems;
		};


		///////////////////////////////////////////////////////
		int GetLimit( void ){ return limit; };
};

//////////////////////////////////////////////////////////
#ifdef QUEUE_DEBUG
#undef QUEUE_DEBUG
#endif
//////////////////////////////////////////////////////////

#endif // !INT_QUEUE_H
