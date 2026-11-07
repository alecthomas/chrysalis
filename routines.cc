/*
	Portable C port of assembler.S, the low-level routines used by the game.

	TBL_translucent lookups use (colour << 8) | background, matching the
	original, which relied on the table being 64K aligned and loaded the
	colour into AH and the background into AL.
*/

#include <string.h>
#include "include.h"
#include "morph.h"

extern "C" {

int remove_accessed( int index, int *list, int entries )
{
int l;

	for ( l = 0; l < entries; l++ )
		if ( list[ l ] == index )
		{
			// The original reports a match in the final slot as "not found".
			// Callers never reach this case because they check the final
			// slot first.
			if ( l == entries - 1 ) return 0;
			memmove( &list[ l ], &list[ l + 1 ], ( entries - l - 1 ) * sizeof( int ) );
			return 1;
		}
	return 0;
}

void __blit4__( char *targ, char *src, int len )
{
int l;

	for ( l = 0; l < len; l++ )
		if ( src[ l ] ) targ[ l ] = src[ l ];
}

int count_pixels( char *image, int size )
{
int l, count = 0;

	for ( l = 8; l < size; l++ )
		if ( image[ l ] ) count++;
	return count;
}

void calc_points( MORPH::PIXEL *list, char *image, int w, int h )
{
int x, y;
unsigned char *p = (unsigned char *)image + 8;

	for ( y = 0; y != h; y++ )
		for ( x = 0; x != w; x++, p++ )
			if ( *p )
			{
				list->x = x;
				list->y = y;
				list->colour = *p;
				list++;
			}
}

void morph_points_to_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, int length )
{
int l, step;

	for ( l = 0; l != length; l++ )
	{
		list[ l ].x += list2[ l ].x;
		list[ l ].y += list2[ l ].y;
		for ( step = 0; step != 3 && ( list[ l ].colour & 0x0f ) != 0x0f; step++ )
			list[ l ].colour++;
	}
}

void morph_points_switch( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length )
{
int l;

	for ( l = 0; l != length; l++ )
	{
		list[ l ].x += list2[ l ].x;
		list[ l ].y += list2[ l ].y;
		list[ l ].colour = list3[ l ].colour | 0x0f;
	}
}

void morph_points_from_white( MORPH::PIXEL *list, MORPH::PIXEL *list2, MORPH::PIXEL *list3, int length )
{
int l, step;

	for ( l = 0; l != length; l++ )
	{
		list[ l ].x += list2[ l ].x;
		list[ l ].y += list2[ l ].y;
		for ( step = 0; step != 3 &&
			(unsigned)list[ l ].colour > (unsigned)list3[ l ].colour; step++ )
			list[ l ].colour--;
	}
}

void dump_points( MORPH::PIXEL *ms, char **ytable, int length )
{
int l;

	for ( l = 0; l != length; l++ )
		ytable[ (unsigned)ms[ l ].y >> 8 ][ (unsigned)ms[ l ].x >> 8 ] = ms[ l ].colour;
}

void calc_vectors( MORPH::PIXEL *list, MORPH::PIXEL *source, MORPH::PIXEL *target, MORPH::PIXEL *vector, int pixels )
{
int l;

	for ( l = 0; l != pixels; l++ )
	{
		list[ l ].x = source[ l ].x << 8;
		vector[ l ].x = ( target[ l ].x - source[ l ].x ) << 5;
		list[ l ].y = source[ l ].y << 8;
		vector[ l ].y = ( target[ l ].y - source[ l ].y ) << 5;
		list[ l ].colour = source[ l ].colour;
	}
}

static inline void translucentpixel( unsigned char *p, int colour )
{
	*p = TBL_translucent[ ( ( colour & 0xff ) << 8 ) | *p ];
}

// Shades a box of h + 1 rows on the 320 pixel wide screen. The top and left
// edges are lit (colour + 2), the bottom and right edges shadowed (colour - 2).
void translucentbox( char *ptr, int w, int h, int colour )
{
unsigned char *row = (unsigned char *)ptr;
int x, y;

	if ( w == 0 || h == 0 ) return;
	for ( x = 0; x != w; x++ ) translucentpixel( row + x, colour + 2 );
	for ( y = 1; y != h; y++ )
	{
		row += 320;
		translucentpixel( row, colour + 2 );
		for ( x = 1; x < w - 1; x++ ) translucentpixel( row + x, colour );
		translucentpixel( row + w - 1, colour - 2 );
	}
	row += 320;
	for ( x = 0; x != w; x++ ) translucentpixel( row + x, colour - 2 );
}

void PLAYER_copy_reversed_image( char *source, char *target )
{
int w = ((int*)source)[ 0 ], h = ((int*)source)[ 1 ], x, y;

	((int*)target)[ 0 ] = w;
	((int*)target)[ 1 ] = h;
	source += 8;
	target += 8;
	for ( y = 0; y != h; y++, source += w, target += w )
		for ( x = 0; x != w; x++ )
			target[ w - 1 - x ] = source[ x ];
}

}
