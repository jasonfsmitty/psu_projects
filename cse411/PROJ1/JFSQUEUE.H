/*
	Jason F. Smith  4588
	Mike Schreiber  5259
	CSE 411    Project 1
*/

#ifndef _JFS_QUEUE_H
#define _JFS_QUEUE_H

#include <stdio.h>

struct PCB;

struct q_node{
	PCB *data;
	q_node *ptr;
};

class JFS_Queue{
	private:
   		q_node 	*q_head;
     		q_node 	*q_tail;
		int 	q_size;
	public:
   		JFS_Queue( void );

		void 	enqueue( PCB* add );
		PCB* 	dequeue( void );

		int  	get_size( void );
		int 	is_empty( void );
};

JFS_Queue::JFS_Queue( void )
{
	q_head=NULL;
	q_tail=NULL;
	q_size=0;
}

void JFS_Queue::enqueue( PCB* add )
{
	// Initialize the node to be entered into the queue.
	q_node* temp;
	temp= new q_node;
	temp->ptr=NULL;
	temp->data=add;

	//Deteremine where it is to go.
	if( q_head==NULL )
	{
   		q_head=temp;
      		q_tail=temp;
   	}
   	else{
   		q_tail->ptr=temp;
      		q_tail=temp;
   	}
   	// Increase the queue size.
   	q_size++;
}

PCB* JFS_Queue::dequeue( void )
{
	q_node *temp;
   	PCB*   out;

   	if( q_size==0 )
   		return NULL;

   	temp=q_head;
   	q_head=q_head->ptr;
   	q_size--;//At this point, the queue is size-1

   	out=temp->data;
   	delete temp;
   	return out;
}

int JFS_Queue::get_size( void )
{
	return q_size;
}

int JFS_Queue::is_empty( void )
{
	if( q_size==0 )
   	return 1;
   	return 0;
}

#endif
