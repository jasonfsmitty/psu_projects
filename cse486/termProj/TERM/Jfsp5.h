/*******************************************************************************

		This is a sorted list using the linked list template 'linklist.h'
      	Jason F. Smith				CSE120			Project 5

*******************************************************************************/
//==============================================================================
//			Specification file (jfsp5.h)
//==============================================================================

#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <iostream.h>
#include <stdlib.h>
#include "bool.h"
#include "linklist.h"

template <class T>
class SortedList : private LinkList<T>
{
	private:
   	int size;
      	//keeps track of the number of elements in the list

	public:
   	SortedList();
      	//Constructor
         //	POST: LinkList() is called,
         //       size==0

      SortedList( const SortedList<T>& orig);
      	//Copy Constructor
         //POST: THIS == orig

      ~SortedList();
      	//Destructor
         //POST: All of the dynamic data nodes are deleted

   	void Insert( T& add);
      	//Adds an element to the list (places it in the correct place
         //PRE: Assigned (add)
         //POST: list == (list + add)

      void Display( T output);
      	//Displays a singe element of the list
         //PRE: Assigned output
         //POST: output == (output)

      void Display(void);
      	//Displays the sorted list
         //POST: output == (list)

      void Remove(void);
      	//Removes the first element from the list and deletes it
         //PRE: list is NOT Empty
         //POST: The first element is removed

      void delNode( int index);
      	//Deletes the node which coresponds to the array index
         //PRE: Assigned (index)  &&  index<size
         //POST: list[i] is deleted, size--

      void delCurrNode(void);
      	//Deletes teh node at which currPtr is at
         //PRE: currPtr!=NULL
         //POST: size--
         //      list-=(currPtr)

      void delet(void);
      	//Deletes the entire list.
         //POST: list == isEmpty()

      int GetSize(void);
      	//POST: FCTVAL == size

      T operator[](int index);
      	//uses the braces to access the list as if it was an array
         //PRE: index<size
         //POST: FCTVAL == list[index]

      void operator=(/*in*/ SortedList<T>& orig);
      	//Equals operator.
         //POST: (THIS == orig) && (currPtr==head)
         //NOTE:The function deletes any extra nodes in THIS list(if necessary)
         //     or adds (any neccesary) nodes if size!=orig.size

      friend ostream& operator<<(ostream& os, SortedList<T>& output);
      	//Overloaded operator for output
         //POST: Display && FCTVAL == os

      Boolean Empty(void);
      	//POST: FCTVAL == (size==0)

      Boolean Full(void);
      	//POST: no more memory left!!


};

/*******************************************************************************
		Implimintation file (jfsp5.h continued)
*******************************************************************************/

template <class T>
SortedList<T>::SortedList() : LinkList<T>()
	//Constructor
   //	POST: LinkList() is called,
   //       size==0
{
   size=0;
}


template <class T>
SortedList<T>::SortedList( const SortedList<T>& orig) : LinkList<T>()
	//Copy Constructor
   //POST: THIS == orig
{
   T temp;
	for(int i=(orig.size-1), size=0; i>=0; i--, size++)
   {
   	temp=orig[i];
   	InsertBefore( temp );
   }
   Reset();
}


template <class T>
SortedList<T>::~SortedList()
	//Destructor
   //POST: All of the dynamic data nodes are deleted
{
	//NOTHING TO DO!!
}


template <class T>
void SortedList<T>::Insert( T& add)
      	//Adds an element to the list (places it in the correct place)
         //PRE: Assigned (add)
         //POST: list == (list + add)
{
	if(IsFull())
   	return;

   Reset();

   while( !EndOfList() )
   {
   	if( CurrentRec() > add )
      	break;
      else
      	Advance();
   }

   InsertBefore( add );
   size++;

}


template <class T>
void SortedList<T>::Display( T output)
	//Displays a singe element of the list
   //PRE: Assigned output
   //POST: output == (output)
{
	try                        //Tries to do what's inside the block
	{
		cout <<output          //A runtime error occurs when the << operator
		     <<endl;           //has a problem cout'ing the data T
   }
   catch(...)                 //Catches ALL errors from the try block
   {                          //and dislays an error message before
   									//terminating the program.
   	cout <<"\n\nError encountered displaying the data."
           <<"\nCheck to make sure the << operator is properly defined"
           <<"\n  for whatever data type you are using.";
      exit(1);
   }

}


template <class T>
void SortedList<T>::Display(void)
	//Displays the sorted list
   //POST: output == (list)
{
	for( Reset() ; !EndOfList(); Advance() )
   {
   	Display( CurrentRec() );
      if( !EndOfList() )
      	cout <<" ";               //Adds a white space between elements
   }
   Reset();
}


template <class T>
void SortedList<T>::Remove(void)
	//Removes the first element from the list and deletes it
   //PRE: list is NOT Empty
   //POST: The first element is removed
{
	if( IsEmpty() || EndOfList() )
   	return;

   Reset();
   Delete();
   size--;
}


template <class T>
void SortedList<T>::delNode( int index)
	//Deletes the node which coresponds to the array-type index[0, 1, 2...size-1]
   //PRE: Assigned (index)  &&  index<size
   //POST: list[i] is deleted, size--
{
   if(index>=size)
   	return;

   Reset();
   for(int i=0; i<index; i++)
   	Advance();

   Delete();
   size--;

   Reset();
   return;
}


template <class T>
void SortedList<T>::delCurrNode(void)
	//Deletes teh node at which currPtr is at
   //PRE: currPtr!=NULL
   //POST: size--
   //      list-=(currPtr)
{
	if( IsEmpty() || EndOfList() )
   	return;
   else{
   	Delete();
      size--;
   }
}

template <class T>
void SortedList<T>::delet(void)
	//Deletes the entire list.
   //POST: list == isEmpty()
{
	int origSize=size;
	for(int i=0; i<origSize; i++)
   	Remove();
}

template <class T>
int SortedList<T>::GetSize(void)
  	//POST: FCTVAL == size
{
	return size;
}


template <class T>
T SortedList<T>::operator[](int index)
   //uses the braces to access the list as if it was an array
   //PRE: index<size
	//POST: FCTVAL == list[index]
{
	if( index<0 || index>=size)
   {
   	cerr <<"\nInvalid [] index of "<<index<<", exiting program.";
      exit(1);
   }
   Reset();
   for(int i=0; i<index; i++)
   	Advance();

   return CurrentRec();
}


template <class T>
void SortedList<T>::operator=(/*in*/ SortedList& orig)
	//Equals operator.
   //POST: (THIS == orig) && (currPtr == head)
   //NOTE:The function deletes any extra nodes in THIS list(if necessary)
   //     or adds (any neccesary) nodes if size!=orig.size
{
	int i;
	// Adds any extra nodes
	for(i=size; i<orig.size; i++)
   {
      InsertBefore( orig[0] );
      size++;
   }

	// Delete any extra nodes
	for(i=orig.size; i<size; i++)
   {
      Reset();
      Delete();
      size--;
   }

	// Set all of the data elements equal

   Reset();
   orig.Reset();
	for(i=0; i<orig.size; i++)
   {
   	InsertBefore( orig.CurrentRec() );
      size++;
   }

	Reset();
}


template <class T>
ostream& operator<<(ostream& os, SortedList<T>& output)
	//Overloaded operator for output
   //POST: Display && FCTVAL == os
{
	output.Display();
   return os;
}


template <class T>
Boolean SortedList<T>::Empty(void)
	//POST: FCTVAL == (size==0)
{
	return (IsEmpty());
}


template <class T>
Boolean SortedList<T>::Full(void)
 	//POST: no more memory left!!
{
	return (IsFull());
}

#endif
