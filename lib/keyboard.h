#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "keymappings.h"

#ifdef __linux__
#include <stdio.h>

#ifdef __SVGA__
#include <vgakeyboard.h>
#endif

#ifdef __X11__
#include "mitshm.h"
#endif
#endif

extern volatile char
  _keys[ 128 ],
  _keypressed,
  _keyreleased,
  _keycount;
 
extern const char
  keytable[],
  shiftkeytable[];

extern void (*userKBhandler)();

char K_convertascii( char key );
void K_init( void (*userhandler)() );
void K_deinit();

#ifdef __WATCOMC__
void __interrupt K_handler();

inline char K_index( int index )
{
	return _keys[ index ];
}

inline char K_pressed()
{
	return _keypressed;
}

inline char K_released()
{
	return _keyreleased;
}

inline char K_count()
{
	return _keycount;
}
#endif

#ifdef __X11__
inline char K_index( int index )
{
	updateKeyboard();
	return queryKeyboard( index );
}

inline char K_pressed()
{
	updateKeyboard();
	return Xkeypressed;
}

inline char K_released()
{
	updateKeyboard();
	return Xkeyreleased;
}

inline char K_count()
{
	return 0;
}

inline void K_clearpressed()
{
	Xkeypressed = 0;
}

inline void K_clearreleased()
{
	Xkeyreleased = 0;
}
#endif

#ifdef __SVGA__
void K_handler( int scancode, int press );

inline char K_index( int index )
{
	keyboard_update();
	return _keys[ index ];
}

inline char K_pressed()
{
	keyboard_update();
	return _keypressed;
}

inline char K_released()
{
	keyboard_update();
	return _keyreleased;
}

inline char K_count()
{
	keyboard_update();
	return _keycount;
}

#endif

#if defined( __SVGA__ ) || defined( __WATCOMC__ )
inline void K_clearpressed()
{
	_keypressed = 0;
}

inline void K_clearreleased()
{
	_keyreleased = 0;
}
#endif

#endif
