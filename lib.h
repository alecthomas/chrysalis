/*****************************************************************************
    Data library header file

  Author: Alec Thomas
  Update: Nov '94
  Comment:
*****************************************************************************/

#ifndef __LIB_H__
#define __LIB_H__

class LIB;

#include <stdio.h>

#define LIB_SIGNATURE 0x5242494c
#define LIB_MAXENTRIES	4096

typedef struct {
	int
		offset, length;
		char name[ 64 ];
	FILE *io;
} LIBLIST;

typedef struct {
	int offset;
	FILE *io;
} LIBSTACK;

class LIB {
		LIBLIST **list;
		int _insert_( LIBLIST *ll );
		int stackpos;
		LIBSTACK stack[ 16 ];
	public :
		FILE *io, *baseio;
		int entries, size, valid, created;
		LIB();
		~LIB();
		int mergelib( char *filename );
		void push();
		void pop();
		int add( char *dataname, char *filename );
		int add( char *dataname, char *buffer, int len );
		int seek( char *dataname );
		int create( char *filename ) { return ( ( io = baseio = fopen( filename, "w+b" ) ) == NULL ); }
		char *retrieve( char *dataname );
		char *retrieve_image( char *dataname );
		LIBLIST *examine( char *dataname );
		LIBLIST *examine( int index ) { return list[ index ]; }
		int entrypercent( int maxpercent, int curentry );
		int positionpercent( int maxpercent );
};

// Global definitions
extern LIB *lib;

#endif
