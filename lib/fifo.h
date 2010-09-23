/*****************************************************************************
	Template for a FIFO queue...or, you could call it a stack :)

	Author: Alec Thomas
	Update: Sep '95
	Comments:
*****************************************************************************/

#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdlib.h>

template <class E> class FIFO {
		class SEL {
			public :
				inline SEL( E *e ) { element = e; next = NULL; }
				E *element;
				SEL *next;
		};
		SEL *head, *tail, *current;
		int itemcount;
	public :
		inline FIFO()
		{
			head = tail = current = NULL;
			itemcount = 0;
		}
		inline ~FIFO()
		{
		SEL *temp;

			temp = head;
			while ( head != NULL ) 
			{
				head = head->next;
				delete temp;
				temp = head;
			}
		}
		inline void push( E *e )
		{
		SEL *temp;

			if ( e == NULL ) return;
			itemcount++;
			if ( head == NULL )
			{ // start a new fifo
				tail = head = new SEL( e );
				return;
			} else
			{
				temp = new SEL( e );
				tail->next = temp;
				tail = temp;
			}
		}
		inline E *pop()
		{
		SEL *temp;
		E *e;

			if ( head == NULL ) return NULL;
			itemcount--;
			temp = head;
			head = head->next;
			e = temp->element;
			delete temp;
			return e;
		}
		inline bool exist( E *e )
		{
		SEL *s = head;

			while ( s != NULL )
			{
				if ( e == s->element ) return true;
				s = s->next;
			}
			return false;
		}
		inline E *reset()
		{
			return ( current = head )->element;
		}
		inline E *next()
		{
			if ( current == NULL || current->next == NULL ) return NULL;
			return ( current = current->next )->element;
		}
		inline E *peek()
		{
			return tail->element;
		}
		inline int items() 
		{ 
			return itemcount; 
		}
};
#endif
