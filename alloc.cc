#include "include.h"
#include "data.h"
#include <stdlib.h>

#define MEMORY_SIGNATURE 	0x89abcdef
// Space reserved before each allocation for the signature. It must be a
// multiple of the platform's maximum alignment so that returned pointers stay
// suitably aligned.
#define MEMORY_HEADER		16

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

static void *allocate( size_t mem, const char *negative )
{
char *tmp;

	CHECK( (long)mem < 0, negative );
	CHECK( ( tmp = (char*)malloc( mem + MEMORY_HEADER ) ) == NULL, "out of memory" );
	tmp += MEMORY_HEADER;
	((unsigned int*)tmp)[ -1 ] = MEMORY_SIGNATURE;
	return tmp;
}

static void release( void *mem, const char *corrupt )
{
	if ( mem == NULL ) return;
	CHECK( ((unsigned int*)mem)[ -1 ] != MEMORY_SIGNATURE, corrupt );
	free( (char*)mem - MEMORY_HEADER );
}

void *operator new( size_t mem )
{
	return allocate( mem, "attempt to allocate negatively sized pointer" );
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

void *operator new []( size_t mem )
{
	return allocate( mem, "attempt to allocate negatively sized array" );
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

void operator delete( void *mem ) noexcept
{
	release( mem, "memory signature corruption detected on pointer deletion" );
}

void operator delete []( void *mem ) noexcept
{
	release( mem, "memory signature corruption detected on array deletion" );
}

// Modern compilers call the sized forms, which must also use this allocator.
void operator delete( void *mem, size_t ) noexcept
{
	release( mem, "memory signature corruption detected on pointer deletion" );
}

void operator delete []( void *mem, size_t ) noexcept
{
	release( mem, "memory signature corruption detected on array deletion" );
}
