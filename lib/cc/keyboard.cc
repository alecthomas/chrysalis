#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <cc/keyboard.h>
#ifdef __SVGA__
#include <vgakeyboard.h>
#endif
#ifdef __X11__
#include <mitshm.h>
#endif

char Keyboard::keyState[ 128 ];
Keyboard::keyEvent Keyboard::eventList[ 256 ];
int Keyboard::readPos = 0, Keyboard::writePos = 0, Keyboard::active = 0;
bool Keyboard::failed = false;

char Keyboard::keyTable[ 128 ] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
	0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

char Keyboard::shiftKeyTable[ 128 ] = {
	0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
	0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

#ifdef __SVGA__
static char __linux_extended_key_conversion__[] = 
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
#endif

Keyboard::Keyboard()
{
	active++;
	if ( active > 1 ) return;
#ifdef __SVGA__
	if ( keyboard_init() == -1 ) 
	{
		failed = true;
		return;
	}
	keyboard_seteventhandler( KeyboardEventHandler );
#endif
#ifdef __X11__
	initKeyboard();
#endif
	memset( eventList, 0, sizeof( eventList ) );
	readPos = writePos = 0;
}

Keyboard::~Keyboard()
{
	active--;
	if ( active ) return;
#ifdef __SVGA__
	keyboard_close();
#endif
}

#ifdef __SVGA__
void KeyboardEventHandler( int scancode, int press )
{
char key, extendedkey = 0;
char _keyreleased = 0, _keypressed = 0;

	// convert linux vgakeyboard stuff to raw data
	key = scancode | ( (!press) << 7 ); 
	if ( ( key & 0x7f ) >= 102 && ( key & 0x7f ) <= 111 )
	{
		key = __linux_extended_key_conversion__[ ( key & 0x7f ) - 102 ] | ( key & 0x80 );
		if ( key & 0x80 ) // key released
		{
			Keyboard::keyState[ key & 0x7f ] &= ~2;
			_keyreleased = key & 0x7f;
		} else
		{
			Keyboard::keyState[ key & 0x7f ] |= 2;
			_keypressed = key & 0x7f;
		}
		extendedkey = false;
	} else
	{
		if ( key & 0x80 ) // key released
		{
			Keyboard::keyState[ key & 0x7f ] &= ~1;
			_keyreleased = key & 0x7f;
		} else
		{
			Keyboard::keyState[ key & 0x7f ] |= 1;
			_keypressed = key & 0x7f;
		}
	}
	if ( _keypressed )
	{
		Keyboard::eventList[ Keyboard::writePos ].press = true;
		Keyboard::eventList[ Keyboard::writePos ].scanCode = _keypressed;
		Keyboard::eventList[ Keyboard::writePos ].ascii = Keyboard::scanCodeToASCII( _keypressed );
	} else
	{
		Keyboard::eventList[ Keyboard::writePos ].press = false;
		Keyboard::eventList[ Keyboard::writePos ].scanCode = _keyreleased;
		Keyboard::eventList[ Keyboard::writePos ].ascii = Keyboard::scanCodeToASCII( _keyreleased );
	}
	++Keyboard::writePos &= 0xff;
}
#endif

unsigned Keyboard::scanCodeToASCII( unsigned scanCode )
{
	if ( keyState[ kbLSHIFT ] || keyState[ kbRSHIFT ] ) return shiftKeyTable[ scanCode ];
	return keyTable[ scanCode ];
}

void Keyboard::update()
{
#ifdef __SVGA__
	keyboard_update();
#endif
#ifdef __X11__
	updateKeyboard();
	memcpy( keyState, keymap, 128 );
	if ( Xkeypressed )
	{
		Keyboard::eventList[ Keyboard::writePos ].press = true;
		Keyboard::eventList[ Keyboard::writePos ].scanCode = Xkeypressed;
		Keyboard::eventList[ Keyboard::writePos ].ascii = Keyboard::scanCodeToASCII( Xkeypressed );
	} else
	{
		Keyboard::eventList[ Keyboard::writePos ].press = false;
		Keyboard::eventList[ Keyboard::writePos ].scanCode = Xkeyreleased;
		Keyboard::eventList[ Keyboard::writePos ].ascii = Keyboard::scanCodeToASCII( Xkeyreleased );
	}
	++Keyboard::writePos &= 0xff;
#endif
}
