#include <malloc.h>
#include <string.h>
#include "virtual.h"
#include "graphics.h"
#include "general.h"

extern "C" {
VPAGESTRUCT
  *V_screen,
  *V_VGAscreen,
  *V_savescreen;

char
  *TBL_translucent = NULL,
  *TBL_leftedge,
  *TBL_rightedge,
  *TBL_shade = NULL;
}

char *V_get( int &x, int &y, int w, int h )
{
int l;
char *image;

  clip( image, x, y, w, h, V_screen->w, V_screen->h );
  if ( ( image = (char*)malloc( w * h + 8 ) ) == NULL ) return NULL;
  ((int*)image)[ 0 ] = w; ((int*)image)[ 1 ] = h;
  for ( l = 0; l != h; l++ )
    memcpy( image + 8 + l * w, V_screen->ytable[ l + y ] + x, w );
  return image;
}

// Should be pretty fast, uses the MIGHTY V_BLIT yay, yay!!
void V_fbox( int x, int y, int w, int h, char colour )
{
int l;
char *i;

  if ( !clip( i, x, y, w, h, V_screen->w, V_screen->h ) ) return;
  for ( l = 0; l != h; l++ )
    V_blit( i, V_screen->ytable[ l + y ] + x, w, SOLID( colour ) );
}

// Definitely not optimised, but hey, you shouldn't be using V_BOX in hard-core
// areas anyway :ù)
void V_box( int x, int y, int w, int h, char colour )
{
  V_fbox( x, y, w, 1, colour );
  V_fbox( x, y, 1, h, colour );
  V_fbox( x + w - 1, y, 1, h, colour );
  V_fbox( x, y + h - 1, w, 1, colour );
}

void V_copybox( VPAGESTRUCT *source, int x, int y, int w, int h,
  VPAGESTRUCT *target, int tx, int ty, int mode )
{
int l;

  if ( x + w - 1 < 0 || y + h - 1 < 0 || tx + w - 1 < 0 || ty + h - 1 < 0 ) return;
  if ( x + w - 1 > source->w ) w -= ( x + w - 1 ) - source->w;
  if ( y + h - 1 > source->h ) h -= ( y + h - 1 ) - source->h;
  if ( tx + w - 1 > target->w ) w -= ( tx + w - 1 ) - target->w;
  if ( ty + h - 1 > target->h ) h -= ( ty + h - 1 ) - target->h;
  if ( x < 0 )
    { w += x; x = 0; }
  if ( y < 0 )
    { h += y; y = 0; }
  if ( tx < 0 )
    { w += tx; tx = 0; }
  if ( ty < 0 )
    { h += ty; ty = 0; }
  if ( h <= 0 || w <= 0 ) return;
  for ( l = 0; l != h; l++ )
    V_blit( source->ytable[ y + l ] + x, target->ytable[ ty + l ] + tx, w, mode );
}

VPAGESTRUCT *V_createpage( int w, int h )
{
VPAGESTRUCT *t;
int l;

  t = (VPAGESTRUCT*)malloc( sizeof( VPAGESTRUCT ) );
  t->w = w; t->h = h;
  t->ptr = (char*)malloc( w * h );
  t->ytable = (char**)malloc( sizeof( char* ) * h );
  t->leftedge = (int*)malloc( sizeof( int ) * h );
  t->rightedge = (int*)malloc( sizeof( int ) * h );
  for ( l = 0; l != h; l++ ) t->ytable[ l ] = t->ptr + l * w;
  memset( t->ptr, 0, w * h );
  return t;
}

VPAGESTRUCT *V_createpage( char *image )
{
VPAGESTRUCT *t;
int l;

  t = (VPAGESTRUCT*)malloc( sizeof( VPAGESTRUCT ) );
  t->w = ((int*)image)[ 0 ]; t->h = ((int*)image)[ 1 ];
  t->ptr = image + 8;
  t->ytable = (char**)malloc( sizeof( char* ) * t->h );
  t->leftedge = (int*)malloc( sizeof( int ) * t->h );
  t->rightedge = (int*)malloc( sizeof( int ) * t->h );
  for ( l = 0; l != t->h; l++ ) t->ytable[ l ] = t->ptr + l * t->w;
  return t;
}

void V_deletepage( VPAGESTRUCT *page )
{
  free( page->ptr );
  free( page->ytable );
  free( page->leftedge );
  free( page->rightedge );
  free( page );
}

// Puts an image, should be about as fast as it gets (clips, tricky puts, etc.)
void V_put( int x, int y, char *image, unsigned mode )
{
int l, w, copyw, h;

  w = copyw = ((int*)image)[ 0 ];
  h = ((int*)image)[ 1 ];
  if ( !clip( image, x, y, copyw, h, V_screen->w, V_screen->h ) ) return;
  if ( ( mode >> 24 ) == TRANSLUCENT && TBL_translucent == NULL ) mode = MASK;
  for ( l = 0; l != h; l++ ) 
    V_blit( image + l * w, V_screen->ytable[ l + y ] + x, copyw, mode );
}

// Initialise virtual pages, etc.
int V_init( void )
{
int l, l1, l2;

  V_screen = (VPAGESTRUCT*)malloc( sizeof( VPAGESTRUCT ) );
  V_screen->w = G_screenwidth; V_screen->h = G_screenheight;
  V_screen->ptr = G_screen;
  V_screen->ytable = (char**)malloc( sizeof( char* ) * V_screen->h );
  for ( l = 0; l != V_screen->h; l++ ) V_screen->ytable[ l ] = V_screen->ptr +
    l * V_screen->w;
  V_VGAscreen = V_screen;
// generate shade table
  TBL_shade = (char*)malloc( 32 * 256 + 65536);
  TBL_shade += 65535;
  l = (int)TBL_shade; l &= ~0xffff; TBL_shade = (char*)l; // make table 256 byte aligned
  for ( l = 0; l != 16; l++ )
    for ( l1 = 0; l1 != 16; l1++ )
      for ( l2 = 0; l2 != 32; l2++ )
        TBL_shade[ ( l2 << 8 ) + ( ( l << 4 ) + l1 ) ] =
          l2 - 15 + l1 <= 0 ?
          ( l << 4 ) : l2 - 15 + l1 > 15 ?
          ( l << 4 ) + 15 :
          ( l << 4 ) + l2 - 15 + l1;
  return TRUE;
}

void V_print( int x, int y, char *text, int mode )
{
  if ( mode == OUTLINE )
    while ( text[ 0 ] )
    {
      V_put( x + 1, y, font[ text[ 0 ] ], SHADE( -2 ) );
      V_put( x - 1, y, font[ text[ 0 ] ], SHADE( +2 ) );
      V_put( x, y + 1, font[ text[ 0 ] ], SHADE( -2 ) );
      V_put( x, y - 1, font[ text[ 0 ] ], SHADE( +2 ) );
      V_put( x, y, font[ text[ 0 ] ], MASK );
      x += ((int*)font[ text[ 0 ] ])[ 0 ] + 1;
      text++;
    }
  else
    while ( text[ 0 ] )
    {
      V_put( x, y, font[ text[ 0 ] ], mode );
      x += ((int*)font[ text[ 0 ] ])[ 0 ];
      text++;
    }
}

int V_textwidth( char *text )
{
int t, w = 0;

  while ( text[ 0 ] )
  {
    t = ((int*)font[ text[ 0 ] ])[ 0 ];
    w += t;
    text++;
  }
  return w;
}

void V_loadtranslucency( FILE *handle )
{
int l;

	TBL_translucent = new char[ 65536 * 2 ];
	// align buffer on 256 byte boundary
	TBL_translucent += 65535; l = (int)TBL_translucent; l &= ~0xffff; TBL_translucent = (char*)l;
	fread( TBL_translucent, 65536, 1, handle );
}

void V_line( int x1, int y1, int x2, int y2, char colour )
/* ======================================================
	Draw a line (I'm a thief, I ripped this directly from the PCGPE...and
	I'm proud of it) :)
*/
{
int i, deltax, deltay, numpixels, dinc1, dinc2, xinc1, xinc2,
	yinc1, yinc2;
register int d, x, y;

	/* Calculate deltax and deltay for initialisation */
	deltax = x2 - x1;
	if ( deltax < 0 ) deltax = -deltax;
	deltay = y2 - y1;
	if ( deltay < 0 ) deltay = -deltay;

	/* Initialize all vars based on which is the independent variable */
	if ( deltax >= deltay )
	{ /* x is independent variable */
		numpixels = deltax + 1;
		d = ( deltay << 1 ) - deltax;
		dinc1 = deltay << 1;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = xinc2 = yinc2 = 1;
		yinc1 = 0;
	} else
	{ /* y is independent variable */
		numpixels = deltay + 1;
		d = (deltax << 1) - deltay;
		dinc1 = deltax << 1;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = yinc1 = yinc2 = 1;
	};

	/* Make sure x and y move in the right directions */
	if ( x1 > x2 )
	{
		xinc1 = - xinc1;
		xinc2 = - xinc2;
	}
	if ( y1 > y2 )
	{
		yinc1 = - yinc1;
		yinc2 = - yinc2;
	}

	/* Start drawing at <x1, y1> */
	x = x1;
	y = y1;

	/* Draw the pixels */
	for ( i = 1; i != numpixels; i++ )
	{
		V_putpixel( x, y, colour );
		if ( d < 0 )
		{
			d += dinc1;
			x += xinc1;
			y += yinc1;
		} else
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
	}
}

void V_circle( int x1, int y1, int radius, char colour )
/* ====================================================
	Draw a circle :) how hard can it be, right? Once again, I ripped this
	directly from PCGPE!
*/
{
int d, x, y;

	d = 3 - ( radius << 1 );
	x = 0;
	y = radius;

	do {
		V_putpixel( x1 + x, y1 + y, colour );
		V_putpixel( x1 + x, y1 - y, colour );
		V_putpixel( x1 - x, y1 + y, colour );
		V_putpixel( x1 - x, y1 - y, colour );
		V_putpixel( x1 + y, y1 + x, colour );
		V_putpixel( x1 + y, y1 - x, colour );
		V_putpixel( x1 - y, y1 + x, colour );
		V_putpixel( x1 - y, y1 - x, colour );
		if ( d < 0 ) 
		{
			d += ( x << 2 ) + 6;
		} else
		{
			d += ( ( x - y ) << 2 ) + 10;
			y--;
		}
		x++;
	} while ( x <= y );
}
