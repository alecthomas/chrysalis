/*****************************************************************************
    Control the swapping of images

   Author: Alec Thomas
   Update: Nov '94
   Comments:
*****************************************************************************/

#ifndef __IMAGE_H__
#define __IMAGE_H__

#define MAXIMAGE        4096

class IMAGE;

// Global variables
extern IMAGE *image;

#include <string.h>
#include <virtual.h>
#include "lib.h"
#include "chkerror.h"
#include "queue.h"

typedef struct {
	char
		**list,
		name[ 64 ];
	int *w, *h, indices, size, reversed, unswappable;
} IMAGENAME;

class IMAGE {
		LIB *lib;
		IMAGENAME *list;
		int maximages, diskw, diskh, cachesize, *accesshistory, accessedimages,
			overflow;
		char *diskimage, *diskback;
		void mark_accessed( int index );
	public :
		static int cachememory;
		IMAGE();
		int swap_in( int index );
		int swap_out( int index );
		void reset_cache();
		void set( int index, char *name );
		int find( char *name );
		inline int getw( int listindex, int index ) { return list[ listindex ].w[ index ]; }
		inline int getw( int listindex ) { return getw( listindex, 0 ); }
		inline int geth( int listindex, int index ) { return list[ listindex ].h[ index ]; }
		inline int geth( int listindex ) { return geth( listindex, 0 ); }
		inline void setunswappable( int listindex ) { list[ listindex ].unswappable = TRUE; }
		inline int getentries( int listindex ) { return list[ listindex ].indices; }
		inline char **get_data( int listindex )
		{
			mark_accessed( listindex );
			if ( list[ listindex ].list == NULL ) swap_in( listindex );
			return list[ listindex ].list;
		}
		inline char *get_data( int listindex, int index )
		{
			mark_accessed( listindex );
			if ( list[ listindex ].list == NULL ) swap_in( listindex );
			return list[ listindex ].list[ index ];
		}
		void put( fixed x, fixed y, int listindex, int index, unsigned mode );
		void put( fixed x, fixed y, int listindex, unsigned mode );
		inline int cache_used() { return cachesize == 0 ? 0 : cachesize * 100 / cachememory; }
		inline int cache_overflow() { return overflow; }
		inline int cache_active_images() { return accessedimages; }
		int in_cache( int index ) { return list[ index ].list != NULL; }
		int get_size( int index ) { return list[ index ].size; }
		inline int images() { return maximages; }
		inline char *name( int listindex ) { return list[ listindex ].name; }
		int add_image( char *name, char **imagelist, int indices );
		void set_image( int index, char **imagelist, int indices );
};
#endif
