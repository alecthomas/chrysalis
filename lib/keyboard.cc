/*******************************************************************************
  Keyboard handling routines

  Author: Alec Thomas
  Updates: Dec 23 '94
  Comments: Extended keys set the SECOND bit of the appropriate keys[] index.
    For example, if you pressed the gray right arrow, the right arrow index would
    contain a 2. If you pressed the gray and non-gray right arrows, it would
    contain (1 | 2). The "keypressed"/"keyreleased" variables contain the key
    value with the high bit set if it's a gray key.
*******************************************************************************/


#ifdef __WATCOMC__
#include <conio.h>
#include <dos.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "keyboard.h"
#include "general.h"

volatile char
  _keys[ 128 ],
  _keypressed,
  _keyreleased,
  _keycount;

const char keytable[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
	0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};
const char shiftkeytable[] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
	0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

#ifdef __WATCOMC__
void (__interrupt __far *old_kb)();
#endif

void (*userKBhandler)();

#ifdef __WATCOMC__
void __interrupt K_handler()
{
char key, extendedkey = 0;

  key = inp( 0x60 );
  if ( key == 0xe0 || key == 0xe1 )
    extendedkey = key;
  else
    if ( extendedkey )
    {
      if ( key & 0x80 ) // key released
      {
        _keys[ key & 0x7f ] &= ~2;
        _keyreleased = key | 0x80;
        if ( _keycount ) _keycount--;
      } else
      {
        _keycount++;
        _keys[ key & 0x7f ] |= 2;
        _keypressed = key | 0x80;
      }
      extendedkey = FALSE;
    } else
    {
      if ( key & 0x80 ) // key released
      {
        _keys[ key & 0x7f ] &= ~1;
        _keyreleased = key & 0x7f;
        if ( _keycount ) _keycount--;
      } else
      {
        _keys[ key & 0x7f ] |= 1;
        _keypressed = key & 0x7f;
        _keycount++;
      }
    }
  key = inp( 0x61 ) | 0x80;
  outp( 0x61, key );
  outp( 0x61, key & 0x7f );
  outp( 0x20, 0x20 );
  if ( userKBhandler != NULL ) userKBhandler();
}
#endif

#ifdef __SVGA__
char __K_linux_extended_conversion__[] = 
	{
		71,		// 102 (home)
		72,		// 103 (up)
		73,		// 104 (pgup)
		75,		// 105 (left)
		77,		// 106 (right)
		79,		// 107 (end)
		80,		// 108 (down)
		81,		// 109 (pgdn)
		82,		// 110 (ins)
		83		// 111 (del)
	};
void K_handler( int scancode, int press )
{
	char key, extendedkey = 0;

	// convert linux vgakeyboard stuff to raw data
	key = scancode | ( (!press) << 7 ); 
	if ( ( key & 0x7f ) >= 102 && ( key & 0x7f ) <= 111 )
	{
		key = __K_linux_extended_conversion__[ ( key & 0x7f ) - 102 ] | ( key & 0x80 );
		if ( key & 0x80 ) // key released
		{
			_keys[ key & 0x7f ] &= ~2;
			_keyreleased = key;
			if ( _keycount ) _keycount--;
		} else
		{
			_keycount++;
			_keys[ key & 0x7f ] |= 2;
			_keypressed = key;
		}
		extendedkey = FALSE;
	} else
	{
		if ( key & 0x80 ) // key released
		{
			_keys[ key & 0x7f ] &= ~1;
			_keyreleased = key & 0x7f;
			if ( _keycount ) _keycount--;
		} else
		{
			_keys[ key & 0x7f ] |= 1;
			_keypressed = key & 0x7f;
			_keycount++;
		}
	}
	if ( userKBhandler != NULL ) userKBhandler();
}
#endif

void K_deinit()
{
	REPORT( "unhooking handler, returning to cooked mode" );
#ifdef __WATCOMC__
char *p = (char*)0x400;

// fixup held down shifts, ctrls, etc.
  p[ 0x17 ] &= 0xf0; p[ 0x18 ] &= 0xfc; p[ 0x96 ] &= 0xf3;
  _dos_setvect( 0x09, old_kb );
#endif
#ifdef __SVGA__
	keyboard_close();
#endif
  _keypressed = _keyreleased = _keycount = 0;
  memset( (void*)&_keys, 0, 128 );
}

void K_init( void (*userhandler)() )
{
	REPORT( "installing raw keyboard handler" );
#ifdef __WATCOMC__
  old_kb=_dos_getvect( 0x09 );
  _dos_setvect( 0x09, K_handler );
#endif
#ifdef __SVGA__
	if ( keyboard_init() == -1 )
		ERROR( "could not init keyboard driver" );
	keyboard_seteventhandler( K_handler );
#endif
#ifdef __X11__
	initKeyboard();
	atexit( K_deinit );
#endif
  atexit( K_deinit );
  userKBhandler = userhandler;
}

char K_convertascii( char key )
{
	if ( K_index( kbLSHIFT ) || K_index( kbRSHIFT ) )
		return shiftkeytable[ key ];
	return keytable[ key ];
}
