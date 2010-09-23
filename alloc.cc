#include "include.h"
#include "data.h"
#include <malloc.h>

#define MEMORY_SIGNATURE 	0x89abcdef

void checkheap()
{
#ifdef __WATCOMC__
	if ( flags.debug )
		cout << "checking heap (" << _heapchk() << ")" << endl;
	if ( _heapchk() == _HEAPOK )
	{
		if ( flags.debug ) cout << "heap ok" << endl;
		return;
	}
	if ( _heapchk() != _HEAPEMPTY ) cout << "error: heap corrupt" << endl;
#endif
#ifdef __linux__
#endif
}

void *operator new( unsigned int mem )
{
void *tmp;

	CHECK( (int)mem < 0, "attempt to allocate negatively sized pointer" );
	CHECK( ( tmp = malloc( mem + 4 ) ) == NULL, "out of memory" );
	((unsigned int*)tmp)[ 0 ] = MEMORY_SIGNATURE;
	return (char*)tmp + 4;
}

//void *operator new( unsigned int mem, void *ptr )
//{
//void *tmp;
//
//	CHECK( (int)mem < 0, "attempt to reallocate negatively sized pointer" );
//	CHECK( ( tmp = realloc( ptr, mem + 4 ) ) == NULL, "out of memory" );
//	((unsigned int*)tmp)[ 0 ] = MEMORY_SIGNATURE;
//	return (char*)tmp + 4;
//}

void *operator new []( unsigned int mem )
{
void *tmp;

	CHECK( (int)mem < 0, "attempt to allocate negatively sized array" );
	CHECK( ( tmp = malloc( mem + 4 ) ) == NULL, "out of memory" );
	((unsigned int*)tmp)[ 0 ] = MEMORY_SIGNATURE;
	return (char*)tmp + 4;
}

//void *operator new []( unsigned int mem, void *ptr )
//{
//void *tmp;
//
//	CHECK( (int)mem < 0, "attempt to reallocate negatively sized pointer" );
//	CHECK( ( tmp = realloc( ptr, mem + 4 ) ) == NULL, "out of memory" );
//	((unsigned int*)tmp)[ 0 ] = MEMORY_SIGNATURE;
//	return (char*)tmp + 4;
//}

void operator delete( void *mem )
{
	CHECK( ((unsigned int*)mem)[ -1 ] != MEMORY_SIGNATURE, "memory signature corruption detected on pointer deletion" );
	free( (char*)mem - 4 );
}

void operator delete []( void *mem )
{
	CHECK( ((unsigned int*)mem)[ -1 ] != MEMORY_SIGNATURE, "memory signature corruption detected on array deletion" );
	free( (char*)mem - 4 );
}
