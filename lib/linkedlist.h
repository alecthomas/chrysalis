/*****************************************************************************
	Template for linked list

	Author: Eric Sambach,Alec Thomas
	Update: Nov '94
	Comments:
*****************************************************************************/

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <stddef.h>

template <class Q> class LinkedList {
		class QLIST {
			public :
				inline QLIST( Q *o )
				/*====================
					Initialise a link in the list
				*/
				{
					object = o;
					prev = NULL; next = NULL;
				}
				QLIST *prev, *next;
				Q *object;
			};
		QLIST *head, *tail, *current;
		int count;
	public :
		inline LinkedList()
		/* ======= 
			Constructor, initialse head/tail/current pointers
		*/
		{
			count = 0;
			head = tail = current = NULL;
		}
		inline ~LinkedList()
		/* ===========
			Destructor for LinkedList list template, delete all objects
		*/
		{
			QLIST *temp;
			
			temp = head;
			while ( temp != NULL )
			{
				head = head->next;
				delete temp;
				temp = head;
			}
		}
		inline Q *operator ++( int )
		{
			return next();
		}
		inline operator Q* ()
		{
			return retrieve();
		}
		inline void insert( Q *object )
		/* ===================================
			Insert an object into the queue
		*/
		{
		QLIST *list;
			
			if ( object == NULL ) return;
			count++;
			if ( head == NULL )
			{ // empty list
				head = new QLIST( object );
				head->next = head->prev = NULL;
				tail = current = head;
				return;
			}
			// insert at start of list
			list = new QLIST( object );
			list->next = head;
			list->prev = NULL;
			head->prev = list;
			head = list;
			return;
		}
		inline void remove( Q *o )
		/* =====================
			Delete an object from the queue
		*/
		{
			QLIST *list;
			
			if ( head == NULL ) return;
			if ( head->object == o )
			{ // removing from the start of the list
				if ( current == head )
					current = head->next;
				list = head;
				head = head->next;
				if ( head != NULL )
					head->prev = NULL;
				else
					tail = NULL;
				delete list;
				count--;
				return;
			}
			if ( tail->object == o )
			{ // removing from the end of the list
				if ( current == tail )
					current = NULL;
				list = tail;
				tail = tail->prev;
				tail->next = NULL;
				delete list;
				count--;
				return;
			}
			list = head; while ( list->next != NULL && list->object != o ) list = list->next;
			if ( list->object == o )
			{ // remove from middle of the list
				if ( current == list )
					current = list->next;
				list->next->prev = list->prev;
				list->prev->next = list->next;
				count--;
				delete list;
			}
		}
		inline void remove()
		/* ==========
			Remove current object from queue
		*/
		{
			QLIST *list;
			
			if ( current == NULL ) return;
			count--;
			if ( current == head )
			{ // removing from head
				head = head->next;
				if ( head != NULL )
					head->prev = NULL;
				else
					tail = NULL;
				delete current;
				current = head;
				return;
			}
			if ( current == tail )
			{ // removing from tail
				tail = tail->prev;
				tail->next = NULL;
				delete current;
				current = NULL;
				return;
			}
			// removing from anywhere in list
			list = current;
			current->next->prev = current->prev;
			current->prev->next = current->next;
			current = current->next;
			delete list;
		}
		inline int exists( Q *object )
		/* ======================
			Returns TRUE if object is in queue
		*/
		{
		QLIST *list;

			list = head;
			while ( list != NULL && object != list->object ) list = list->next;
			if ( list != NULL )
				return 1;
			return 0;
		}
		inline int items()
		/* ===========
			Return the number of items in the queue
		*/
		{
			return count;
		}
		inline int at_tail()
		/* =============
			Return true if at tail
		*/
		{
			return current == tail;
		}
		inline int at_head()
		/* =============
			Return true if at head
		*/
		{
			return current == head;
		}
		inline Q *reset_end()
		/* ==============
			Reset to end of queue
		*/
		{
			if ( tail == NULL ) return NULL;
			return ( current = tail )->object;
		}
		inline Q *prev()
		/* =========
			Previous item
		*/
		{
			if ( current == NULL || current->prev == NULL ) return NULL;
			return ( current = current->prev )->object;
		}
		inline Q *reset()
		/*  =========
			Restart the list
		*/
		{
			if ( head == NULL ) return NULL;
			return ( current = head )->object;
		}
		inline Q *next()
		/*========
			Move to the next object and return it's pointer
		*/
		{
			if ( current == NULL || current->next == NULL ) return NULL;
			return ( current = current->next )->object;
		}
		inline Q *retrieve()
		/*============
			Return current object **NOTE: if at end of list, won't return NULL**
		*/
		{
			if ( current == NULL ) return NULL;
			return current->object;
		}
};

#endif
