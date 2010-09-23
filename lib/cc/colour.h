#ifndef __COLOUR_H__
#define __COLOUR_H__

/*
	RGB Colour class. Use it for all colours, because that's the way it should be.

	Author: Alec Thomas
	Date: Tue Nov 21 17:33:26 EST 1995
*/

#include <fixed.h>

class Video;
class Font;
class ostream;

class Colour {
		friend Video;
		static char toActual[ 32 ][ 32 ][ 32 ];
		inline void setDirect( fixed const &r, fixed const &g, fixed const &b );
		friend int operator == ( Colour const &c, Colour const &c2 );
		friend int operator < ( Colour const &c, Colour const &c1 );
		friend int operator > ( Colour const &c, Colour const &c1 );
		friend int operator <= ( Colour const &c, Colour const &c1 );
		friend int operator >= ( Colour const &c, Colour const &c1 );
		friend Font &operator << ( Font &outfile, Colour &colour );
		friend ostream &operator << ( ostream &outfile, Colour &colour );
	public :
		fixed r, g, b;
		int actual;

		inline Colour();
		inline Colour( int value );
		inline Colour( fixed const &r, fixed const &g, fixed const &b );
		inline Colour( float r, float g, float b );
		inline int set( fixed const &r, fixed const &g, fixed const &b );
		static void initTables( char *fileName, char *palette );
		inline Colour &operator += ( Colour const &colour );
		inline Colour &operator += ( fixed const &number );
		inline Colour &operator += ( int number );
		inline Colour &operator -= ( Colour const &colour );
		inline Colour &operator -= ( fixed const &number );
		inline Colour &operator -= ( int number );
		inline Colour &operator /= ( Colour const &colour );
		inline Colour &operator /= ( fixed const &number );
		inline Colour &operator /= ( int number );
		inline Colour &operator *= ( Colour const &colour );
		inline Colour &operator *= ( fixed const &number );
		inline Colour &operator *= ( int number );
		inline Colour &operator = ( Colour const &colour );
		inline Colour operator + ( Colour const &colour );
		inline Colour operator + ( int number );
		inline Colour operator + ( fixed const &number );
		inline Colour operator - ( Colour const &colour );
		inline Colour operator - ( fixed const &number );
		inline Colour operator - ( int number );
		inline Colour operator / ( Colour const &colour );
		inline Colour operator / ( fixed const &number );
		inline Colour operator / ( int number );
		inline Colour operator * ( Colour const &colour );
		inline Colour operator * ( fixed const &number );
		inline Colour operator * ( int number );
};


inline Colour::Colour( int value )
/* ================================
	Create a new colour....direct index
*/
{
	actual = value;
}

inline Colour::Colour()
/* =====================
	Default constructor...does squat
*/
{
}

inline Colour::Colour( fixed const &r, fixed const &g, fixed const &b )
/* =========================================================
	Initialise a colour
*/
{
	setDirect( r, g, b );
}

inline Colour::Colour( float R, float G, float B )
/* =========================================================
	Initialise a colour
*/
{
	setDirect( R, G, B );
}

inline void Colour::setDirect( fixed const &R, fixed const &G, fixed const &B )
/* =====================================================
	Sets the RGB values directly, does not modify any other values
*/
{
fixed r = R, g = G, b = B;

	Colour::r = r; Colour::g = g; Colour::b = b;
	if ( r < 0 ) r = 0;
	if ( g < 0 ) g = 0;
	if ( b < 0 ) b = 0;
	if ( Colour::r > 255 ) r = 255;
	if ( Colour::g > 255 ) g = 255;
	if ( Colour::b > 255 ) b = 255;
	actual = toActual[ r.integer() >> 3 ][ g.integer() >> 3 ][ b.integer() >> 3 ];
}

inline int Colour::set( fixed const &r, fixed const &g, fixed const &b )
/* ========================================================
	Set colour to a specific rgb value
*/
{
	setDirect( r, g, b );
	return actual;
}

inline Colour &Colour::operator += ( Colour const &colour )
{
	setDirect( r + colour.r, g + colour.g, b + colour.b );
	return *this;
}

inline Colour &Colour::operator += ( fixed const &number )
{
	setDirect( r + number, g + number, b + number );
	return *this;
}

inline Colour &Colour::operator += ( int number )
{
	setDirect( r + number, g + number, b + number );
	return *this;
}

inline Colour &Colour::operator -= ( fixed const &number )
{
	setDirect( r - number, g - number, b - number );
	return *this;
}

inline Colour &Colour::operator -= ( int number )
{
	setDirect( r - number, g - number, b - number );
	return *this;
}

inline Colour &Colour::operator -= ( Colour const &colour )
{
	setDirect( r - colour.r, g - colour.g, b - colour.b );
	return *this;
}

inline Colour &Colour::operator /= ( Colour const &colour )
{
	setDirect( r / colour.r, g / colour.g, b / colour.b );
	return *this;
}

inline Colour &Colour::operator /= ( fixed const &number )
{
	setDirect( r / number, g / number, b / number );
	return *this;
}

inline Colour &Colour::operator /= ( int number )
{
	setDirect( r / number, g / number, b / number );
	return *this;
}

inline Colour &Colour::operator *= ( Colour const &colour )
{
	setDirect( r * colour.r, g * colour.g, b * colour.b );
	return *this;
}

inline Colour &Colour::operator *= ( fixed const &number )
{
	setDirect( r * number, g * number, b * number );
	return *this;
}

inline Colour &Colour::operator *= ( int number )
{
	setDirect( r * number, g * number, b * number );
	return *this;
}

inline Colour &Colour::operator = ( Colour const &colour )
{
	setDirect( colour.r, colour.g, colour.b );
	return *this;
}

inline Colour Colour::operator + ( Colour const &colour )
{
	return Colour( r + colour.r, g + colour.g, b + colour.b );
}

inline Colour Colour::operator + ( fixed const &number )
{
	return Colour( r + number, g + number, b + number );
}

inline Colour Colour::operator + ( int number )
{
	return Colour( r + number, g + number, b + number );
}

inline Colour Colour::operator - ( Colour const &colour )
{
	return Colour( r - colour.r, g - colour.g, b - colour.b );
}

inline Colour Colour::operator - ( fixed const &number )
{
	return Colour( r - number, g - number, b - number );
}

inline Colour Colour::operator - ( int number )
{
	return Colour( r - number, g - number, b - number );
}

inline Colour Colour::operator / ( Colour const &colour )
{
	return Colour( r / colour.r, g / colour.g, b / colour.b );
}

inline Colour Colour::operator / ( fixed const &number )
{
	return Colour( r / number, g / number, b / number );
}

inline Colour Colour::operator / ( int number )
{
	return Colour( r / number, g / number, b / number );
}

inline Colour Colour::operator * ( Colour const &colour )
{
	return Colour( r * colour.r, g * colour.g, b * colour.b );
}

inline Colour Colour::operator * ( fixed const &number )
{
	return Colour( r * number, g * number, b * number );
}

inline Colour Colour::operator * ( int number )
{
	return Colour( r * number, g * number, b * number );
}

inline int operator == ( Colour const &c, Colour const &c2 )
{
	return ( c.r == c2.r ) && ( c.g == c2.g ) && ( c.b == c2.b );
}

inline int operator < ( Colour const &c, Colour const &c1 )
{
	return ( c.r * c.r + c.g * c.g + c.b * c.b ) < ( c1.r * c1.r + c1.g * c1.g + c1.b * c1.b );
}

inline int operator > ( Colour const &c, Colour const &c1 )
{
	return ( c.r * c.r + c.g * c.g + c.b * c.b ) > ( c1.r * c1.r + c1.g * c1.g + c1.b * c1.b );
}

inline int operator <= ( Colour const &c, Colour const &c1 )
{
	return ( c.r * c.r + c.g * c.g + c.b * c.b ) <= ( c1.r * c1.r + c1.g * c1.g + c1.b * c1.b );
}

inline int operator >= ( Colour const &c, Colour const &c1 )
{
	return ( c.r * c.r + c.g * c.g + c.b * c.b ) >= ( c1.r * c1.r + c1.g * c1.g + c1.b * c1.b );
}
#endif
