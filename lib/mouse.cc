#ifdef __WATCOMC__
#include <i86.h>
#endif

#ifdef __linux__
#include <stdlib.h>
#ifdef __SVGA__
#include <vga.h>
#include <vgamouse.h>
#endif
#ifdef __X11__
#include "mitshm.h"
#endif
#endif

#include <string.h>
#include <stdio.h>
#include "general.h"
#include "mouse.h"

#ifdef __SVGA__
void M_deinit()
{
	REPORT( "unhooking mouse handler" );
	mouse_close();
}
#endif

int M_init( int w, int h )
{
#ifdef __WATCOMC__
union REGPACK rs;

  REPORT( "using device 0x33" );
  memset( &rs, 0, sizeof( union REGPACK ) );
  intr( 0x33, &rs );
  if ( rs.w.ax != 0xffff )
	  return FALSE;
  memset( &rs, 0, sizeof( union REGPACK ) );
  rs.w.ax = 7;
  rs.w.cx = 0;
  rs.w.dx = w << 1;
  intr( 0x33, &rs );
  memset( &rs, 0, sizeof( union REGPACK ) );
  rs.w.ax = 8;
  rs.w.cx = 0;
  rs.w.dx = h;
  intr( 0x33, &rs );
  return TRUE;
#endif

#ifdef __SVGA__
int rv;

	REPORT( "using device /dev/mouse" );
	atexit( M_deinit );
	rv = ( mouse_init( "/dev/mouse", vga_getmousetype(), MOUSE_DEFAULTSAMPLERATE ) == 0 );
	if ( rv == FALSE ) return FALSE;
	mouse_setxrange( 0, w );
	mouse_setyrange( 0, h );
	return rv;
#endif
}

void M_getposition( int &x, int &y )
{
#ifdef __WATCOMC__
union REGPACK rs;

  memset( &rs, 0, sizeof( union REGPACK ) );
  rs.w.ax = 3;
  intr( 0x33, &rs );
  x = rs.w.cx >> 1;
  y = rs.w.dx;
#endif

#ifdef __SVGA__
	mouse_update();
	x = mouse_getx();
	y = mouse_gety();
#endif

#ifdef __X11__
	static unsigned int dummy;
	x = Xmousex; y = Xmousey;
#endif
}


void M_setposition( int x, int y )
{
#ifdef __WATCOMC__
union REGPACK rs;

  memset( &rs, 0, sizeof( union REGPACK ) );
  rs.w.ax = 4;
  rs.w.cx = x << 1;
  rs.w.dx = y;
  intr( 0x33, &rs );
#endif

#ifdef __SVGA__
	mouse_setposition( x, y );
#endif

#ifdef __X11__
	setMouse( x, y );
#endif
}

void M_pressed( int &b )
{
#ifdef __WATCOMC__
union REGPACK rs;

  memset( &rs, 0, sizeof( union REGPACK ) );
  rs.w.ax = 3;
  intr( 0x33, &rs );
  b = rs.w.bx;
#endif

#ifdef __SVGA__
// Fucking linux makes the middle button == 1, right button == 2 and left button
// == 4....stoopid stoopid, so i'll have to convert them
int nb;

	mouse_update();
	nb = mouse_getbutton();
	b = 0;
	if ( nb & 4 ) b |= 1;
	if ( nb & 2 ) b |= 4;
	if ( nb & 1 ) b |= 2;
#endif

#ifdef __X11__
	b = Xmousebuttons;
#endif
}

