/*---------------------------------------------------------------------------+
	linklist2.h

	Jason F Smith

	A double linked list template.  I didn't like my old one, so I wrote
	this one rather quickly.  Maybe if I use it again I'll add more to it.
 +---------------------------------------------------------------------------*/

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <iostream.h>
#include <stddef.h>

#define DEBUG

////////////////////////////////////////////////////////////
#if defined( DEBUG )
#	define LINKLIST_DEBUG
#	pragma message ("INFO - LinkList template debug: ON")
#else
#	pragma message ("INFO - LinkList template debug: OFF")
#endif
////////////////////////////////////////////////////////////

template <class T>
struct Node {
	T		data;

	Node*	next;
	Node*	prev;
}; // struct Node

/////////////////////////////////////////////////////////////
template <class T>
class LinkList {
	protected:
		Node<T> *head;
		Node<T> *tail;

		Node<T> *current;

		int size;
	
	public:
		LinkList( void );
		~LinkList( void );

		int DeleteAll( void );
		bool DeleteCurrent( void );
		bool Delete( int index );

		bool MoveForward( void );
		bool MoveBackward( void );

		T& operator[]( int index );

		T& GetCurrent( void );

		int GetSize( void );
		bool SetCurrent( int index );

		bool MoveToTail( void );
		bool MoveToHead( void );

		bool InsertBefore( T add );
		bool InsertAfter( T add );

		bool InsertAtTail( T add );
		bool InsertAtHead( T add );

		bool IsEmpty( void );

};	// class LinkList<T>
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//  now on to the actual code...
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
LinkList<T>::LinkList( void )
{
	head = tail = current = NULL;
	size = 0;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
LinkList<T>::~LinkList( void )
{
	DeleteAll();
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
int LinkList<T>::DeleteAll( void )
{
	int i;
	while( head ){
		current = head;
		head = head->next;
		delete current;
		i++;
	}

	// now set up the variables correctly
	head = tail = current = NULL;
	size = 0;
	return i;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::Delete( int index )
{
	if( SetCurrent( index ) ){
		return DeleteCurrent();
	}
	return false;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::DeleteCurrent( void )
{
	if( !current ){
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - error deleting current (LinkList::DeleteCurrent), no current\n";
#endif // LINKLIST_DEBUG
		return false;
	}

	if( current->prev )
		current->prev->next = current->next;
	else // no previous must be head
		head = current->next;

	if( current->next )
		current->next->prev = current->prev;
	else // no next, must be at tail
		tail = current->prev;

	delete current;
	current = head;
	size--;

	return true;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::MoveForward( void )
{
	if( !current || !current->next ){
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - could not move forward - LinkedList::MoveForward()\n";
#endif // LINKLIST_DEBUG
		return false;
	}

	current = current->next;
	return true;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::MoveBackward( void )
{
	if( !current || !current->prev ){
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - could not move backward - LinkList::MoveBackward()\n";
#endif // LINKLIST_DEBUG
		return false;
	}
	current = current->prev;
	return true;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
T& LinkList<T>::operator[]( int index )
{
	static T dummy;

	if( SetCurrent(index) )
		return (current->data);

#if defined( LINKLIST_DEBUG )
	cout <<"DEBUG - could not use [] operator, invalid call to SetCurrent() - LinkeList::operator[]()\n";
#endif // LINKLIST_DEBUG

	return dummy;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
T& LinkList<T>::GetCurrent( void )
{
	static T dummy;

	if( !current ){
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - could not return current, (none to return) - LinkList::GetCurrent()\n";
#endif // LINKLIST_DEBUG
		return dummy;
	}

	return (current->data);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
int LinkList<T>::GetSize( void )
{
	return size;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::SetCurrent( int index )
{
	if( index<0 || index>=size )
	{
#if defined( LINKLIST_DEBUG )
		if( index<0 )
			cout <<"DEBUG - could not set current, index<0 - LinkList::SetCurrent()\n";
		else 
			cout <<"DEBUG - could not set current, index>=size - LinkList::SetCurrent()\n";
#endif // LINKLIST_DEBUG
		return false;
	}

	current=head;
	for( int i=0; i<index; i++ )
		current = current->next;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::MoveToTail( void )
{
	return ((current=tail)!=NULL);
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::MoveToHead( void )
{
	return ((current=head)!=NULL);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::InsertBefore( T add )
{
	Node<T>* temp;

	if( !current || (current==head) )
		return InsertAtHead();

	temp = new Node<T>;
	if( !temp )
	{
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - could not insert node (new failed) - LinkList::InsertBefore()\n";
#endif // LINKLIST_DEBUG
		return false;
	}

	memcpy( (void*)&(temp->data), (void*)&add, sizeof( T ) );
	//temp->data = add;

	temp->next = current;
	temp->prev = current->prev;
	current->prev->next = temp;
	current->prev = temp;
	

	current = temp;
	size++;
	return true;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::InsertAfter( T add )
{
	Node<T>* temp;
	if( !current || (current==tail) )
		return InsertAtTail();

	temp = new Node<T>;
	if( !temp )
	{
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - could not insert node (new failed) - LinkList::InsertAfter()\n";
#endif // LINKLIST_DEBUG
		return false;
	}

	memcpy( (void*)&(temp->data), (void*)&add, sizeof( T ) );
	//temp->data = add;

	temp->prev = current;
	temp->next = current->next;
	current->next->prev = temp;
	current->next = temp;
	
	current = temp;
	size++;
	return true;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::InsertAtTail( T add )
{
	Node<T>* temp;

	temp = new Node<T>;
	if( temp==NULL )
	{
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - could not insert node (new failed) - LinkList::InsertAtTail()\n";
#endif // LINKLIST_DEBUG
		return false;
	}

	memcpy( (void*)&(temp->data), (void*)&add, sizeof( T ) );
	//temp->data = add;

	if( tail==NULL ){
		head = tail = temp;
		temp->next=NULL;
		temp->prev=NULL;
		size=1;
	} else {
		tail->next = temp;
		temp->prev = tail;
		temp->next = NULL;
		tail = temp;
		size++;
	}

	current = temp;
	return true;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::InsertAtHead( T add )
{
	Node<T>* temp;

	temp = new Node<T>;
	if( !temp )
	{
#if defined( LINKLIST_DEBUG )
		cout <<"DEBUG - could not insert node (new failed) - LinkList::InsertAtHead()\n";
#endif // LINKLIST_DEBUG
		return false;
	}

	memcpy( (void*)&(temp->data), (void*)&add, sizeof( T ) );
	//temp->data = add;

	if( head!=NULL ){
		head->prev = temp;
		temp->next = head;
		temp->prev = NULL;
		head = temp;
		size++;
	} else {
		head = tail = temp;
		temp->prev = NULL;
		temp->next = NULL;
		size=1;
	}

	current = temp;
	return true;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
template <class T>
bool LinkList<T>::IsEmpty( void )
{
	return (size==0);
}

/////////////////////////////////////////////
/////////////////////////////////////////////
#ifdef LINKLIST_DEBUG
#undef LINKLIST_DEBUG
#endif
/////////////////////////////////////////////

#endif // ! LINKED_LIST_H
