/*
		linklist.h

		Jason F Smith

		I wrote this a long long time ago, but it is pretty solid code...
		    just not optimized for speed.

*/

#ifndef LINK_LIST_H
#define LINK_LIST_H

#include <stddef.h>     // For NULL
#include <iostream.h>

#ifndef TRUE
#define	TRUE		1
#define FALSE		0
#endif

//----------------------------------------------------------------------
//  SPECIFICATION
//  This module exports a template class for linked lists
//----------------------------------------------------------------------
template <class T>
struct Node {
	 T 		data;
	 Node    *link;        // Pointer to next node
};

template <class T>
class LinkList {
public:
	 bool IsEmpty() const;
		  // POST: FCTVAL == (list is empty)

	 bool IsFull() const;
		  // POST: FCTVAL == (list is full)

	 void Reset();
		  // PRE:  NOT IsEmpty()
		  // POST: List cursor is at front of list

	 bool EndOfList() const;
		  // POST: FCTVAL == (list cursor is beyond end of list)

	 void Advance();
		  // PRE:  NOT IsEmpty()  &&  NOT EndOfList()
		  // POST: List cursor has advanced to next record

	 T CurrentRec() const;
		  // PRE:  NOT IsEmpty()  &&  NOT EndOfList()
		  // POST: FCTVAL == record at list cursor

	 void InsertBefore( /* in */ T someRec );
		  // PRE:  Assigned(someRec)  &&  NOT IsFull()
		  // POST: someRec inserted before list cursor
		  //          (at back, if EndOfList())
		  //    && This is the new current record

	 void InsertAfter( /* in */ T someRec );
		  // PRE:  Assigned(someRec)  &&  NOT IsEmpty()
		  //    && NOT IsFull()  &&  NOT EndOfList()
		  // POST: someRec inserted after list cursor
		  //    && This is the new current record

	 void Delete();
		  // PRE:  NOT IsEmpty()  &&  NOT EndOfList()
		  // POST: Record at list cursor deleted
		  //    && Successor of deleted record is now the current record

	 LinkList();
		  // Constructor
		  // POST: Empty list created  &&  EndOfList()

	 ~LinkList();
		  // Destructor
		  // POST: List destroyed
protected:
	 Node<T> *head;
	 Node<T> *currPtr;
};

//----------------------------------------------------------------------
//  IMPLEMENTATION
//  This module exports a linked list template class
//  for maintaining lists
//  List representation: a singly linked list of dynamic nodes.
//----------------------------------------------------------------------

// Private members of class:
//    Node<T> *head;
//    Node<T> *currPtr;
//
// CLASSINV:
//     (head != NULL) --> head points to front of list
//  && (head == NULL)  --> list is empty
//  && (currPtr != NULL) --> currPtr points to currently accessed node
//  && (currPtr == NULL)  --> list cursor is beyond end of list

template <class T>
Node<T> *PrevPtr( Node<T>*, Node<T>* ); // Prototype for auxiliary function

template <class T>
LinkList<T>::LinkList()
	 //..................................................................
	 // Constructor
	 // POST: head == NULL  &&  currPtr == NULL
	 //..................................................................
{
	 head = currPtr = NULL;
}

template <class T>
LinkList<T>::~LinkList()
	 //..................................................................
	 // Destructor
	 // POST: All free store for list has been deallocated
	 //..................................................................
{
	 currPtr = head;
	 while ( !EndOfList() )
		  Delete();
}

template <class T>
bool LinkList<T>::IsEmpty() const
	 //..................................................................
	 // POST: FCTVAL == (head == NULL)
	 //..................................................................
{
	 return (head == NULL);
}

template <class T>
bool LinkList<T>::IsFull() const
	 //..................................................................
	 // POST: FCTVAL == (there is no room on the free store for
	 //                  another PersonNode object)
	 //..................................................................
{
		 Node<T> *newPtr;
		 newPtr = new Node<T>;
		 if( !newPtr )
			return TRUE;
		 else {
			delete newPtr;
			return FALSE;
		 }
}

template <class T>
void LinkList<T>::Reset()
	 //..................................................................
	 // PRE:  head != NULL
	 // POST: currPtr == head
	 //..................................................................
{
	 currPtr = head;
}

template <class T>
bool LinkList<T>::EndOfList() const
	 //..................................................................
	 // POST: FCTVAL == (currPtr == NULL)
	 //..................................................................
{
	 return (currPtr == NULL);
}

template <class T>
void LinkList<T>::Advance()
	 //..................................................................
	 // PRE:  head != NULL  &&  currPtr != NULL
	 // POST: currPtr == (currPtr<entry>)->link
	 //..................................................................
{
	 currPtr = currPtr->link;
}

template <class T>
T LinkList<T>::CurrentRec() const
	 //..............................................................
	 // PRE:  head != NULL  &&  currPtr != NULL
	 // POST: FCTVAL.name == currPtr->name
	 //    && FCTVAL.age == currPtr->age
	 //..................................................................
{
	 T rec;

	 rec = currPtr->data;
	 return rec;
}

template <class T>
void LinkList<T>::InsertBefore( /* in */ T someRec )
	 //..................................................................
	 // PRE:  Assigned(someRec)
	 // POST: (currPtr<entry> == NULL) --> New node is at end of list
	 //    && (currPtr<entry> != NULL) --> New node inserted before
	 //                                    *(currPtr<entry>)
	 //    && currPtr == pointer to new node
	 //..................................................................
{
	 Node<T> *newPtr = new Node<T>;
	 // ASSERT: Storage for node is now on free store and
	 //         its base address is in newPtr

	 newPtr->data = someRec;
	 newPtr->link = currPtr;
	 if (currPtr == head)
		  head = newPtr;
	 else {
		  Node<T> *p = PrevPtr(currPtr, head);
		  p->link = newPtr;
	 }
	 currPtr = newPtr;
}

template <class T>
void LinkList<T>::InsertAfter( /* in */ T someRec )
	 //..................................................................
	 // PRE:  Assigned(someRec)  &&  head != NULL  &&  currPtr != NULL
	 // POST: New node inserted after *(currPtr<entry>)
	 //    && currPtr == pointer to new node
	 //..................................................................
{
	 Node<T> *newPtr = new Node<T>;
	 newPtr->data = someRec;

	 newPtr->link = currPtr->link;
	 currPtr->link = newPtr;
	 currPtr = newPtr;
}

template <class T>
void LinkList<T>::Delete()
	 //..................................................................
	 // PRE:  head != NULL  &&  currPtr != NULL
	 // POST: *(currPtr<entry>) deleted from list (and name string
	 //       deleted from free store)
	 //    && currPtr points to node after deleted node (or == NULL if
	 //       last node was deleted)
	 //..................................................................
{
	 Node<T> *temp = currPtr;
	 if (currPtr == head)
		  head = currPtr = head->link;
	 else {
		  Node<T> *p = PrevPtr(currPtr, head);
		  p->link = currPtr = currPtr->link;
	 }
	 delete temp;
	 // ASSERT: Free store for node deallocated
}

template <class T>
Node<T> *PrevPtr( /* in */ Node<T> *somePtr,
					  /* in */ Node<T> *head    )
	 //..................................................................
	 // PRE:  somePtr != head
	 // POST: FCTVAL == pointer to node before *somePtr
	 //..................................................................
{
	 Node<T> *p = head;
	 while (p->link != somePtr)  // INV (prior to test):
										  //    *p is a node before *somePtr
		  p = p->link;
	 return p;
}

#endif
