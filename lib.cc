/*****************************************************************************
  Library manager class

   Author: Alec and Eric
   Update: Nov' 94
   Comments:
*****************************************************************************/


#include "include.h"
#include <iostream>

#include <general.h>
#include <graphics.h>

#include "lib.h"
#include "chkerror.h"

// Global variables
LIB *lib;

LIB::LIB()
/*  =======================
	Initialise a data library. 
*/
{
	stackpos = 0;
	created = FALSE;
	baseio = NULL;
	valid = TRUE;
	list = new LIBLIST*[ LIB_MAXENTRIES ];
	memset( list, 0, sizeof( LIBLIST* ) * LIB_MAXENTRIES );
	entries = 0;
}

LIB::~LIB()
/*  ========
	Library destructor closes all files from library
*/
{
int l;

	for ( l = 0; l != entries; l++ )
		fclose( list[ l ]->io );
	delete list;
}

void LIB::push()
/* ==============
	Save current library file pointer position
*/
{
	stack[ stackpos ].offset = ftell( io );
	stack[ stackpos ].io = io;
	stackpos++;
}

void LIB::pop()
/* =============
	Restore last saved file pointer position
*/
{
	if ( !stackpos ) return;
	stackpos--;
	io = stack[ stackpos ].io;
	fseek( io, stack[ stackpos ].offset, SEEK_SET );
}

int LIB::_insert_( LIBLIST *ll )
/* ==============================
	Used internally by the library functions to insert a new record.
	Inserts in sorted order.
*/
{
int l = 0, l1, overwrite = FALSE;
	
	if ( entries )
		for ( l = 0; l != entries; l++ )
			if ( strcmp( ll->name, list[ l ]->name ) <= 0 )
			{
				if ( !strcmp( ll->name, list[ l ]->name ) )
				{ // over-write entry with identical name
					overwrite = TRUE;
					delete list[ l ];
					break;
				}
				for ( l1 = entries; l1 != l; l1-- )
					list[ l1 ] = list[ l1 - 1 ];
				break;
			}
	list[ l ] = new LIBLIST;
	memcpy( list[ l ], ll, sizeof( LIBLIST ) );
	// increase entries if not over-writing another one
	if ( !overwrite ) entries++;
	return TRUE;
}

int LIB::add( char *dname, char *filename )
/* ============================
	Reads "filename" in from disk and adds it to the LIBrary file as well as
	the record list.
*/
{
FILE *in;
char *buffer;
int len, rv;
	
	if ( ( in = fopen( filename, "rb" ) ) == NULL ) return FALSE;
	fseek( in, 0, SEEK_END );
	len = ftell( in );
	fseek( in, 0, SEEK_SET );
	if ( ( buffer = new char[ len ] ) == NULL )
	{
		fclose( in );
		return FALSE;
	}
	fread( buffer, 1, len, in );
	fclose( in );
	rv = add( dname, buffer, len );
	delete buffer;
	return rv;
}

int LIB::add( char *filename, char *buffer, int len )
/* ===================================================
	Adds raw data to the library disk and records.
*/
{
LIBLIST l, *existinglib;
int pos, sig, s;
	
	strcpy( l.name, filename );
	l.io = io;
	l.length = len;
	l.offset = sizeof( LIBLIST );
	if ( ( s = seek( filename ) ) == -1 )
	{ // append new entry
		fseek( io, 0, SEEK_END );
		l.offset += ftell( io );
	} else
	{ // overwrite existing entry
		fseek( io, -sizeof( LIBLIST ), SEEK_CUR );
		existinglib = lib->examine( s );
		l.offset += ftell( io );
		if ( existinglib->length > len )
			l.length = existinglib->length;
	}
	if ( !_insert_( &l ) ) return FALSE;
	sig = LIB_SIGNATURE;
	fwrite( &sig, 4, 1, io );
	fwrite( &l, 1, sizeof( LIBLIST ) - 4, io );
	fwrite( buffer, 1, l.length, io );
	return TRUE;
}

int LIB::seek( char *filename )
/* =============================
	Uses a binary search to find the records. Returns -1 if it couldn't find
	the record or the index number if it could.
*/
{
int pos = 0, posjump = entries, dir, linear = FALSE;
	
	if ( !entries ) return -1;
	while ( posjump != 0 )
	{
		dir = strcmp( filename, list[ pos ]->name );
		if ( dir == 0 ) // found the item
		{
			io = list[ pos ]->io;
			fseek( io, list[ pos ]->offset, SEEK_SET );
			io = list[ pos ]->io;
			return pos;
		}
		if ( posjump <= 2 && !linear )
		{
			linear = TRUE;
			posjump = 4;
		}
		if ( linear )
			posjump--;
		else
			posjump /= 2;
		pos += dir < 0 ? -posjump : +posjump;
		if ( pos >= entries ) pos = entries - 1;
		if ( pos < 0 ) pos = 0;
	}
	return -1;
}

char *LIB::retrieve( char *filename )
/*  ==================================
	Retrieve a record from the LIBrary.
*/
{
int pos;
char *buffer;
	
	if ( ( pos = seek( filename ) ) == -1 ) return NULL;
	if ( ( buffer = new char[ list[ pos ]->length ] ) == NULL ) return NULL;
	if ( fread( buffer, list[ pos ]->length, 1, io ) != 1 )
	{
		delete buffer;
		return NULL;
	}
	return buffer;
}

char *LIB::retrieve_image( char *filename )
/*  =======================================
	Retrieve an image from the LIBrary.
*/
{
int pos, cpos;
	
	if ( ( pos = seek( filename ) ) == -1 ) return NULL;
	return G_loadimage( io );
}

LIBLIST *LIB::examine( char *name )
/*     ========================
	Return information about library entry "name".
*/
{
	int l;
	
	if ( ( l = seek( name ) ) == -1 ) return NULL;
	return list[ l ];
}

int LIB::mergelib( char *filename )
/* =================================
	Add a new library file to program
*/
{
LIBLIST tl;
int sig, startentries = entries, oldlen = 0, newlen;

	if ( ( io = fopen( filename, "r+b" ) ) == NULL )
		return FALSE;
	if ( baseio == NULL ) baseio = io;
	do {
		newlen = lib->positionpercent( 20 );
		for ( ; oldlen != newlen; oldlen++ ) cout << ".";
		cout.flush();
		if ( fread( &sig, 4, 1, io ) != 1 ) break;
		if ( sig != LIB_SIGNATURE )
		{
			fclose( io );
			return FALSE;
		}
		if ( fread( &tl, 1, sizeof( LIBLIST ) - 4, io ) != sizeof( LIBLIST ) - 4 ) break;
		tl.io = io;
		if ( !_insert_( &tl ) )
		{
			fclose( io );
			return FALSE;
		}
		fseek( io, tl.offset + tl.length, SEEK_SET );
	} while ( TRUE );
	return TRUE;
}

int LIB::entrypercent( int maxpercent, int curentry )
/* ===================================================
	Return position in whole library index as a percentage
*/
{
	return ( maxpercent * curentry ) / entries;
}

int LIB::positionpercent( int maxpercent )
/* ========================================
	Return position in current file as a percentage
*/
{
static FILE *lastio = NULL;
static int lastiolength;

	if ( io != lastio )
	{ // new file, calculate size
		lastio = io;
		push();
		fseek( io, 0, SEEK_END );
		lastiolength = ftell( io );
		pop();
	}
	return ( maxpercent * ftell( io ) ) / lastiolength;
}
