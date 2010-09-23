/*****************************************************************************
    Control the swapping of images

   Author: Alec Thomas
   Update: Nov '94
   Comments:
	Caching idea from Eric, and initially from "Operating System Design"
*****************************************************************************/

#include "include.h"
#include <malloc.h>
#include "image.h"
#include "chkerror.h"


// Global variables
IMAGE *image;
int IMAGE::cachememory = 2048 * 1024;			// 2M image cache

IMAGE::IMAGE()
/* ============
	Image constructor
*/
{
	lib = ::lib;
	diskimage = lib->retrieve_image( "MISC_DISK" );
	diskw = ((int*)diskimage)[ 0 ];
	diskh = ((int*)diskimage)[ 1 ];
	list = new IMAGENAME[ MAXIMAGE ];
	memset( list, 0, sizeof( IMAGENAME ) * MAXIMAGE );
	accesshistory = new int[ MAXIMAGE ];
	memset( accesshistory, -1, sizeof( int ) * MAXIMAGE );
	maximages = cachesize = accessedimages = overflow = 0;
}

void IMAGE::set( int index, char *name )
/* ======================================
	Set an image's name, make list of image width's and height's so we
	don't have to swap in to perform checks on image bounding boxes
*/
{
int l, t, x, y, len, newlen;
LIBLIST *ll = NULL;
char buf[ 64 ], buf2[ 64 ];
static int oldlen = 0, w[ 512 ], h[ 512 ];
	
	strcpy( list[ index ].name, name );
	list[ index ].list = NULL;
	if ( index >= maximages ) maximages = index + 1;
	lib->push();
	if ( G_oldmode == 0 )
	{
		newlen = lib->entrypercent( 20, index );
		for ( ; oldlen != newlen; oldlen++ ) cout << ".";
	}
	if ( lib->seek( list[ index ].name ) == -1 )
	{
		strcpy( buf, "could not retrieve image " );
		strcat( buf, list[ index ].name );
		strcat( buf, " (index " );
		sprintf( buf2, "%i", index );	
		strcat( buf, buf2 );
		strcat( buf, ")" );
		ERROR( buf );
	}
	ll = lib->examine( list[ index ].name );
	for ( l = len = 0; ; l++ )
	{
		fread( &w[ l ], 4, 1, lib->io );
		fread( &h[ l ], 4, 1, lib->io );
		if ( ( len += w[ l ] * h[ l ] + 8 ) >= ll->length )
		{
			CHECK( len > ll->length, "image set exceeds library entry length" );
			l++;
			break;
		}
		fseek( lib->io, w[ l ] * h[ l ], SEEK_CUR );
	}
	list[ index ].w = new int[ l ];
	list[ index ].h = new int[ l ];
	list[ index ].indices = l;
	list[ index ].size = ll->length;
	memcpy( list[ index ].w, w, l * sizeof( int ) );
	memcpy( list[ index ].h, h, l * sizeof( int ) );
	lib->pop();
}

#ifdef __linux__
extern "C" int remove_accessed( int index, int *list, int entries );
#endif

#ifdef __WATCOMC__
int remove_accessed( int index, int *list, int entries );
/* ======================================================
	Remove a reference to an index in the access history list
*/
#pragma aux remove_accessed = \
		"cmp ecx, 0" \
		"je quit" \
		"repne scasd" \
		"jecxz quit" \
		"mov esi, edi" \
		"sub edi, 4" \
		"rep movsd" \
		"inc ecx" \
	"quit:" \
	modify [esi] \
	parm [eax] [edi] [ecx] \
	value [ecx]
#endif

void IMAGE::mark_accessed( int index )
/* ===================================
	Mark an image as having been accessed
*/
{
	if ( accesshistory[ accessedimages - 1 ] == index ) return;
	if ( !remove_accessed( index, accesshistory, accessedimages ) )
		accessedimages++;
	accesshistory[ accessedimages - 1 ] = index;
	CHECK( accessedimages == MAXIMAGE, "too many images in cache access list" );
}

int IMAGE::swap_in( int index )
/* =============================
	Swap in an image, checking and adjusting cache if necessary
*/
{
char *data, *p;
int l, t, x, y;
LIBLIST *ll = NULL;
char buf[ 64 ], buf2[ 64 ];
VPAGESTRUCT *oldpage;
	
	if ( list[ index ].list != NULL ) return FALSE;
	lib->push();
	oldpage = V_screen;
	V_screen = V_VGAscreen;
	x = 320 - diskw; y = 200 - diskh;
	diskback = V_get( x, y, diskw, diskh );
	V_put( x, y, diskimage, MASK );
	if ( cachesize + list[ index ].size > cachememory )
	{ // swap out images, because the cache is too big
		while ( cachesize + list[ index ].size > cachememory )
		{
			if ( accessedimages == 0 )
			{
				strcpy( buf, "image " );
				strcat( buf, list[ index ].name );
				strcat( buf, " exceeds cache limit, increase cache size" );
				ERROR( buf );
			}
			swap_out( accesshistory[ 0 ] );
			memmove( accesshistory, &accesshistory[ 1 ], accessedimages * sizeof( int ) );
			accessedimages--;
		}
		overflow++;
	}
	if ( ( data = lib->retrieve( list[ index ].name ) ) == NULL )
	{
		strcpy( buf, "could not retrieve image " );
		strcat( buf, list[ index ].name );
		strcat( buf, " (index " );
		sprintf( buf2, "%i", index );
		strcat( buf, buf2 );
		strcat( buf, ")" );
		ERROR( buf );
	}
	ll = lib->examine( list[ index ].name );
	list[ index ].list = new char*[ list[ index ].indices ];
	p = data;
	for ( l = 0; p < data + ll->length; l++ )
	{
		list[ index ].list[ l ] = p;
		p += ((int*)p)[ 0 ] * ((int*)p)[ 1 ] + 8;
	}
	if ( l != list[ index ].indices )
	{
		strcpy( buf, "image indices do not match for image " );
		strcat( buf, list[ index ].name );
		strcat( buf, " (index " );
		sprintf( buf2, "%i", index );
		strcat( buf, buf2 );
		strcat( buf, ")" );
		ERROR( buf );
	}
	lib->pop();
	V_put( x, y, diskback, MOV );
	free( diskback );
	V_screen = oldpage;
	cachesize += list[ index ].size;
	return TRUE;
}

int IMAGE::swap_out( int index )
/* ==============================
	Swap out an image
*/
{
	if ( list[ index ].list == NULL || list[ index ].unswappable ) return FALSE;
	delete list[ index ].list[ 0 ];
	delete list[ index ].list;
	list[ index ].list = NULL;
	cachesize -= list[ index ].size;
	return TRUE;
}

void IMAGE::reset_cache()
/* ======================
	Swap out all images in cache and reset cache size
*/
{
int l, c;

	for ( l = c = 0; l != maximages; l++ )
		if ( list[ l ].unswappable )
			c += list[ l ].size;
		else
			swap_out( l );
	CHECK( cachesize != c, "cache inconsistency on reset" );
}

int IMAGE::find( char *name )
{
int l;

	for ( l = 0; l != maximages; l++ )
		if ( !strcmp( name, list[ l ].name ) ) return l;
	return -1;
}

int IMAGE::add_image( char *name, char **imagelist, int indices )
/* ============================================
	Add a user defined image to the list
*/
{
	strcpy( list[ maximages ].name, name );
	set_image( maximages, imagelist, indices );
	maximages++;
	return maximages - 1;
}

void IMAGE::set_image( int index, char **imagelist, int indices )
/* ========================================================
	Set a specific image's data
*/
{
int l;

	if ( list[ index ].list != NULL )
	{
		delete list[ index ].w;
		delete list[ index ].h;
	}
	list[ index ].list = imagelist;
	list[ index ].indices = indices;
	list[ index ].size = 0;
	list[ index ].unswappable = TRUE;
	list[ index ].w = new int[ indices ];
	list[ index ].h = new int[ indices ];
	for ( l = 0; l != indices; l++ )
	{
		list[ index ].w[ l ] = ((int*)imagelist[ l ])[ 0 ];
		list[ index ].h[ l ] = ((int*)imagelist[ l ])[ 1 ];
	}
}

void IMAGE::put( fixed x, fixed y, int listindex, int index, unsigned mode )
{
	mark_accessed( listindex );
	if ( list[ listindex ].list == NULL ) swap_in( listindex );
	V_put( x.integer(), y.integer(), list[ listindex ].list[ index ], mode );
}

void IMAGE::put( fixed x, fixed y, int listindex, unsigned mode )
{
	mark_accessed( listindex );
	if ( list[ listindex ].list == NULL ) swap_in( listindex );
	V_put( x.integer(), y.integer(), *list[ listindex ].list, mode );
}

