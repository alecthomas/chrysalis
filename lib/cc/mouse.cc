#ifdef __WATCOMC__
#include <i86.h>
#endif

#ifdef __X11__
#include <mitshm.h>
#endif

#ifdef __SVGA__
#include <vga.h>
#include <vgamouse.h>
#include <stdlib.h>
#endif

#include <string.h>
#include <stdio.h>
#include "general.h"
#include "mouse.h"

int Mouse::active = 0;

Mouse::Mouse( int w, int h )
/* ==========================
	Initialise the mouse driver
*/
{
	active++;
	if ( active > 1 ) return;
	failed = false;
	width = w; height = h;

#ifdef __WATCOMC__
union REGPACK rs;

	memset( &rs, 0, sizeof( union REGPACK ) );
	intr( 0x33, &rs );
	if ( rs.w.ax != 0xffff ) 
	{
		failed = true;
		return;
	}
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
#endif

#ifdef __SVGA__
int rv;

	rv = ( mouse_init( "/dev/mouse", vga_getmousetype(), MOUSE_DEFAULTSAMPLERATE ) == 0 );
	if ( !rv ) 
	{
		failed = true;
		return;
	}
	mouse_setxrange( 0, w );
	mouse_setyrange( 0, h );
#endif
	x = lastX = y = lastY = 0;
	button = lastButton = 0;
}

Mouse::~Mouse()
/* =============
	Kill the mouse
*/
{
	active--;
	if ( active ) return;
#ifdef __SVGA__
	mouse_close();
#endif
}

void Mouse::update()
/* ==================
	Call this baby once per frame, it gets mouse positions, and button information, etc.
*/
{
	lastButton = button;
	lastX = x; lastY = y;

#ifdef __WATCOMC__
	union REGPACK rs;

	memset( &rs, 0, sizeof( union REGPACK ) );
	rs.w.ax = 3;
	intr( 0x33, &rs );
	button = rs.w.bx;

	memset( &rs, 0, sizeof( union REGPACK ) );
	rs.w.ax = 3;
	intr( 0x33, &rs );
	x = rs.w.cx >> 1;
	y = rs.w.dx;
#endif

#ifdef __X11__
	updateKeyboard();
	button = Xmousebuttons;
	x = Xmousex; y = Xmousey;
#endif

#ifdef __SVGA__
	int nb;
	mouse_update();
	x = mouse_getx();
	y = mouse_gety();

	// Fucking linux makes the middle button == 1, right button == 2 and left button
	// == 4....stoopid stoopid, so i'll have to convert them
	mouse_update();
	nb = mouse_getbutton();
	button = 0;
	if ( nb & 4 ) button |= 1;
	if ( nb & 2 ) button |= 4;
	if ( nb & 1 ) button |= 2;
#endif
}


void Mouse::setPosition( int x, int y )
/* =====================================
	Set the position of the mouse cursor
*/
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
