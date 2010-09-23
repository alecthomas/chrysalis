#ifndef __VIRTUAL_H
#define __VIRTUAL_H
#include <stdio.h>
#include <string.h>
#include "graphics.h"
#include "general.h"

#ifdef __X11__
#include "mitshm.h"
#endif

/*******************************************************************************
  function: The following defines are valid values that can be passed to
            V_BLIT (and indirectly through V_PUT)
      date:  31 Aug '94
*******************************************************************************/
#define MOV                (1 << 24)
#define MASK               (2 << 24)
#define XOR                (3 << 24)
#define OR                 (4 << 24)
#define AND                (5 << 24)

/*                         => NOTE <=
  This put mode requires the  TRANSLUCENCY table be initialised
*/

#define TRANSLUCENT        (6 << 24)

/*                               => NOTE <=
  These put-modes require the RGB_UTILS be linked in and initialised
*/
#define TINT( C, F )       (7 << 24) | (C) | ((F) << 8)
#define TINTBACK( C, F )   (8 << 24) | (C) | ((F) << 8)

#define SINGLE( C )        (9 << 24) | (unsigned char)(C)
#define SOLID( C )         (10 << 24) | (unsigned char)(C)
#define FADE( C )          (11 << 24) | (signed char)(C)+15
#define ALIAS              (12 << 24)
#define SHADE( C )         (13 << 24) | (signed char)(C)+15
#define TRANSLUCENTFADE( C ) ( 14 << 24 ) | (signed char)(C)+15
#define FLIPPED		   (64 << 24)
#define REVERSED	   (32 << 24)
#define OUTLINE            (127 << 24)

#define FULLTINT           7
#define HALFTINT           4
#define NOTINT             0

/*******************************************************************************
      name:  VPAGESTRUCT
  function:  Information about the VIRTUAL pages, including the PTR to the
             start of the page, and a table of pointers pointing to the start
             of each row of the page
      date:  31 Aug '94
*******************************************************************************/
typedef struct {
  char *ptr, **ytable;
  int w, h, *leftedge, *rightedge;
} VPAGESTRUCT;

// make them "C" externals so that the assembler routines can access them
extern "C" {
/*******************************************************************************
      name:  PAGESTRUCT's
  function:  V_SCREEN and V_VGASCREEN are initialised by V_INIT and are the
             default
      date:  31 Aug '94
*******************************************************************************/
extern VPAGESTRUCT *V_screen, *V_VGAscreen, *V_savescreen;

extern char
  *TBL_translucent,
  *TBL_shade;
}
/*******************************************************************************
      name:  V_INIT
  function:  Initialises the virtual page driver, setting up default pages, edge
             lists, etc.
      date:  31 Aug '94
*******************************************************************************/
int V_init( void );

/*******************************************************************************
      name: V_PUT
  function: Puts an image down on a virtual page, using V_BLIT
      date:  31 Aug '94
*******************************************************************************/
void V_put( int x, int y, char *image, unsigned mode );

/*******************************************************************************
      name: V_GET
  function: Gets an image from a virtual page
      date:  31 Aug '94
*******************************************************************************/
char *V_get( int &x, int &y, int w, int h );

/*******************************************************************************
      name: V_TEXTWIDTH
  function: returns the width of the text, using the current font
      date:  31 Aug '94
*******************************************************************************/
int V_textwidth( char *text );

/*******************************************************************************
      name: V_PRINT
  function: print the current font
      date:  31 Aug '94
*******************************************************************************/
void V_print( int x, int y, char *text, int mode );

/*******************************************************************************
      name: V_COPYBOX
  function: Copies a portion of SOURCE page to TARGET page, not copying portions
            of the screen
      date:  31 Aug '94
*******************************************************************************/
void V_copybox( VPAGESTRUCT *source, int x, int y, int w, int h,
  VPAGESTRUCT *target, int tx, int ty, int mode );

/*******************************************************************************
      name: V_CREATEPAGE
  function: Create a new virtual page
      date:  31 Aug '94
*******************************************************************************/
VPAGESTRUCT *V_createpage( int w, int h );

/*******************************************************************************
      name: V_CREATEPAGE
  function: Create a virtual page as an alias for an image
      date:  31 Aug '94
*******************************************************************************/
VPAGESTRUCT *V_createpage( char *image );

/*******************************************************************************
      name: V_DELETEPAGE
  function: Delete a virtual page
      date:  31 Aug '94
*******************************************************************************/
void V_deletepage( VPAGESTRUCT *page );

/*******************************************************************************
      name: V_FBOX
  function: Draw a filled box
      date:  31 Aug '94
*******************************************************************************/
void V_fbox( int x, int y, int w, int h, char colour );

/*******************************************************************************
      name: V_BOX
  function: Draw a box outline
      date:  31 Aug '94
*******************************************************************************/
void V_box( int x, int y, int w, int h, char colour );

void V_circle( int x, int y, int radius, char colour );
void V_line( int x, int y, int x1, int y1, char colour );

inline char V_getpixel( int x, int y )
/* ==========================================
	Read a pixel from the screen
*/
{
	if ( x < 0 || x >= V_screen->w || y < 0 || y >= V_screen->h ) return 0;
	return V_screen->ytable[ y ][ x ];
}

inline int V_putpixel( int x, int y, char colour )
/* ==========================================
	Draw a single pixel....THE most primitive of all graphics primitives :)
*/
{
	if ( x < 0 || x >= V_screen->w || y < 0 || y >= V_screen->h ) return 0;
	V_screen->ytable[ y ][ x ] = colour;
	return 1;
}

void V_loadtranslucency( FILE *handle );

/*******************************************************************************
      name: V_CLEARPAGE
  function: Clear the current virtual page to COLOUR
      date:  31 Aug '94
*******************************************************************************/
inline void V_clearpage( char colour )
{
  memset( V_screen->ptr, 0, V_screen->w * V_screen->h );
}

/*******************************************************************************
      name: V_SHOWPAGE
  function: Copy the current virtual page to the video screen, while
            simultaneously clearing the virtual page to black (to take advantage
            of 16-bit VGA wait states)
      date:  31 Aug '94
*******************************************************************************/
inline void V_showpage()
{
  if ( V_screen->ptr != G_screen ) memcpy( G_screen, V_screen->ptr, V_screen->w * V_screen->h );
#ifdef __X11__
	updateScreen();
#endif
}

// External assembley routines
extern "C" {
#ifdef __WATCOMC__
  void _cdecl V_blit( char *source, char *target, unsigned len, unsigned mode );
#endif
#ifdef __linux__
  void V_blit( char *source, char *target, unsigned len, unsigned mode );
#endif
}

/*******************************************************************************
      name: V_SETPAGE
  function: Set the active page to PAGE
      date:  31 Aug '94
*******************************************************************************/
inline void V_setpage( VPAGESTRUCT *page )
{
  V_savescreen = V_screen;
  V_screen = page;
}

/*******************************************************************************
      name: V_RESTOREPAGE
  function: Restore the page to the previously SAVED page (saved by SETPAGE)
      date:  31 Aug '94
*******************************************************************************/
inline void V_restorepage()
{
  V_screen = V_savescreen;
}
#endif
