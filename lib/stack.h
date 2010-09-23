/*****************************************************************************
	Template for a FIFO queue...or, you could call it a stack :)

	Author: Alec Thomas
	Update: Sep '95
	Comments:
*****************************************************************************/

#ifndef __Stack_H__
#define __Stack_H__

template <class E> class Stack {
		class SEL {
			public :
				inline SEL( E *e ) { element = e; next = NULL; }
				E *element;
				SEL *next;
		};
		SEL *head, *current;
		int itemcount;
	public :
		inline Stack()
		{
			head = current = NULL;
			itemcount = 0;
		}
		inline ~Stack()
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
				head = new SEL( e );
				return;
			} else
			{
				temp = new SEL( e );
				temp->next = head;
				head = temp;
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
			return head->element;
		}
		inline int items() 
		{ 
			return itemcount; 
		}
};
#endif
