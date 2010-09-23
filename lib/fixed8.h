/*
	Fixed point
	===========
	A class to implement fixed8 point. Handles all possible combinations of
	integer/fixed8 +,-,*,/,etc.

	Author:		Alec Thomas
	Date:		27th April '95
	Comments:
		Still have to implement logical operators such as && and ||.
*/
#ifndef __FIXED8_H__
#define __FIXED8_H__

class fixed;
class ostream;
class istream;

#pragma inline_recursion on

#define FIXED8_SHIFT_SIZE	8
#define FIXED8_MASK_VALUE	0xff

#ifdef __WATCOMC__
// shrd value shouuld be FIXED8_SHIFT_SIZE
int __fixed8_mul( int a, int b );
#pragma aux __fixed8_mul = \
	"imul edx" \
	"shrd eax, edx, 8" \
	parm [eax] [edx] \
	value [eax]

// shrd/sar values should be (32-FIXED8_SHIFT_SIZE)
int __fixed8_div( int a, int b );
#pragma aux __fixed8_div = \
	"mov eax, 0" \
	"shrd eax, edx, 24" \
	"sar edx, 24" \
	"idiv ebx" \
	parm [edx] [ebx] \
	value [eax]
#endif

#ifdef __linux__

inline int __fixed8_mul( int left, int right )
{
	asm volatile ( 	
		"imull %%edx\n"
		"shrdl $8, %%edx, %%eax\n" 
		: "=a" (left)
		: "a" (left), "d" (right) 
		);
	return left;
}

inline int __fixed8_div( int left, int right )
{
	asm volatile (	
		"movl $0, %%eax\n"
		"shrdl $24, %%edx, %%eax\n"
		"sarl $24, %%edx\n"
		"idivl %%ebx\n" 
		: "=a" (left)
		: "d" (left), "b" (right) 
	);
	return left;
}

#endif

class fixed8 {
	public :
		int number;
		inline fixed8() {}
		inline fixed8( int i, int f )
		{
			number = ( i << FIXED8_SHIFT_SIZE ) | ( f & FIXED8_MASK_VALUE );
		}
		inline fixed8( int v )
		{
			number = v << FIXED8_SHIFT_SIZE;
		}
		inline fixed8( double num ) 
		{
			number = ( (int)num << ( FIXED8_SHIFT_SIZE ) ) + 
				( (int)( ( num - (int)num ) * 256 ) );
		}
		inline fixed8( float num ) 
		{
			number = ( (int)num << ( FIXED8_SHIFT_SIZE ) ) + 
				( (int)( ( num - (int)num ) * 256 ) );
		}
		inline fixed8( fixed8 const &v ) { number = v.number; }
		inline fixed8( int i, const char *dummy ) { (void)dummy; number = i; }
		inline ~fixed8() {}
		inline int integer() const {
			return number < 0 ? -( (-number) >> FIXED8_SHIFT_SIZE ) : 
				number >> FIXED8_SHIFT_SIZE;
		}
		inline int fraction() const {
			return number < 0 ? (-number) & FIXED8_MASK_VALUE : 
				number & FIXED8_MASK_VALUE;
		}
		inline fixed8 &operator = ( fixed8 const &num ) {
			number = num.number;
			return *this;
		}
		inline fixed8 &operator = ( double num ) 
		{
			number = ( (int)num << ( FIXED8_SHIFT_SIZE ) ) + 
				( (int)( ( num - (int)num ) * 256 ) );
			return *this;
		}
		inline fixed8 &operator = ( float num ) 
		{
			number = ( (int)num << ( FIXED8_SHIFT_SIZE ) ) + 
				( (int)( ( num - (int)num ) * 256 ) );
			return *this;
		}
		inline fixed8 &operator = ( int num ) {
			number = num << FIXED8_SHIFT_SIZE;
			return *this;
		}
		inline fixed8 &operator ++ ( int ) {
			number += 1 << FIXED8_SHIFT_SIZE;
			return *this;
		}
		inline fixed8 &operator -- ( int ) {
			number -= 1 << FIXED8_SHIFT_SIZE;
			return *this;
		}
		inline fixed8 &operator += ( int num ) {
			number += num << FIXED8_SHIFT_SIZE;
			return *this;
		}
		inline fixed8 &operator += ( fixed8 const &num ) {
			number += num.number;
			return *this;
		}
		inline fixed8 &operator -= ( int num ) {
			number -= num << FIXED8_SHIFT_SIZE;
			return *this;
		}
		inline fixed8 &operator -= ( fixed8 const &num ) {
			number -= num.number;
			return *this;
		}
		inline fixed8 &operator *= ( int num ) {
			number *= num;
			return *this;
		}
		inline fixed8 &operator *= ( fixed8 const &num ) {
			number = __fixed8_mul( number, num.number );
			return *this;
		}
		inline fixed8 &operator /= ( int num ) {
			number /= num;
			return *this;
		}
		inline fixed8 &operator /= ( fixed8 const &num ) {
			number = __fixed8_div( number, num.number );
			return *this;
		}
		inline fixed8 &operator <<= ( int num ) {
			number <<= num;
			return *this;
		}
		inline fixed8 &operator <<= ( fixed8 const &num ) {
			number <<= num.number;
			return *this;
		}
		inline fixed8 &operator >>= ( int num ) {
			number >>= num;
			return *this;
		}
		inline fixed8 &operator >>= ( fixed8 const &num ) {
			number >>= num.number;
			return *this;
		}
		inline int operator ! () const { return number == 0; }
		inline fixed8 operator - () const { return fixed8( -number, "" ); }
		inline fixed8 operator + () const { return fixed8( number, "" ); }
		inline operator int () const { return number >> FIXED8_SHIFT_SIZE ; }
		inline operator double () const 
			{ return (double)( number >> FIXED8_SHIFT_SIZE ) + ( (double)( number & FIXED8_MASK_VALUE ) / 256.0 ); }
		inline operator float () const 
			{ return (float)( number >> FIXED8_SHIFT_SIZE ) + ( (float)( number & FIXED8_MASK_VALUE ) / 256.0 ); }
		friend inline fixed8 sin( fixed8 );
		friend inline fixed8 cos( fixed8 );
		friend inline fixed8 abs( fixed8 );

	private :
		static int sintable[ 512 ], costable[ 512 ];
};

ostream &operator << ( ostream &outfile, fixed8 const &f );
istream &operator >> ( istream &outfile, fixed8 const &f );

// Misc routines
fixed8 sqrt( fixed8 number );
int sqrt( int number );

inline fixed8 abs( fixed8 num )
{
	if ( num.number < 0 ) return fixed8( -num.number );
	return fixed8( num.number );
}

// Trig routines for fixed8 point class
inline fixed8 sin( fixed8 angle ) {
	if ( angle.number < 0 )
		return fixed8( -fixed8::sintable[ ( ( -angle.number ) >> FIXED8_SHIFT_SIZE ) & 0x1ff ], "" );
	else
		return fixed8( fixed8::sintable[ ( angle.number >> FIXED8_SHIFT_SIZE ) & 0x1ff ], "" );
}

inline fixed8 cos( fixed8 angle ) {
	if ( angle.number < 0 )
		return fixed8( fixed8::costable[ ( ( -angle.number ) >> FIXED8_SHIFT_SIZE ) & 0x1ff ], "" );
	else
		return fixed8( fixed8::costable[ ( angle.number >> FIXED8_SHIFT_SIZE ) & 0x1ff ], "" );
}

// Fixed point addition
inline fixed8 operator + ( fixed8 const &num1, fixed8 const &num2 ) {
	return fixed8( num1.number + num2.number, "" );
}

inline fixed8 operator + ( fixed8 const &num1, int num2 ) {
	return fixed8( num1.number + ( num2 << FIXED8_SHIFT_SIZE ), "" );
}

inline fixed8 operator + ( int num1, fixed8 const &num2 ) {
	return fixed8( ( num1 << FIXED8_SHIFT_SIZE ) + num2.number, "" );
}

// Fixed point subtraction
inline fixed8 operator - ( fixed8 const &num1, fixed8 const &num2 ) {
	return fixed8( num1.number - num2.number, "" );
}

inline fixed8 operator - ( fixed8 const &num1, int num2 ) {
	return fixed8( num1.number - ( num2 << FIXED8_SHIFT_SIZE ), "" );
}

inline fixed8 operator - ( int num1, fixed8 const &num2 ) {
	return fixed8( ( num1 << FIXED8_SHIFT_SIZE ) - num2.number, "" );
}

// Fixed point multiplication
inline fixed8 operator * ( fixed8 const &num1, fixed8 const &num2 ) {
	return fixed8( __fixed8_mul( num1.number, num2.number ), "" );
}

inline fixed8 operator * ( fixed8 const &num1, int num2 ) {
	return fixed8( num1.number * num2, "" );
}

inline fixed8 operator * ( int num1, fixed8 const &num2 ) {
	return fixed8( num1 * num2.number, "" );
}

// Fixed point division
inline fixed8 operator / ( fixed8 const &num1, fixed8 const &num2 ) {
	return fixed8( __fixed8_div( num1.number, num2.number ), "" );
}

inline fixed8 operator / ( fixed8 const &num1, int num2 ) {
	return fixed8( num1.number / num2, "" );
}

inline fixed8 operator / ( int num1, fixed8 const &num2 ) {
	return fixed8( __fixed8_div( num1 << FIXED8_SHIFT_SIZE, num2.number ), "" );
}

// Fixed point !=
inline int operator != ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number != num2.number;
}

inline int operator != ( fixed8 const &num1, int num2 ) {
	return num1.number != ( num2 << FIXED8_SHIFT_SIZE );
}

inline int operator != ( int num1, fixed8 const &num2 ) {
	return ( num1 << FIXED8_SHIFT_SIZE ) != num2.number;
}

// Fixed point <=
inline int operator <= ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number <= num2.number;
}

inline int operator <= ( fixed8 const &num1, int num2 ) {
	return num1.number <= ( num2 << FIXED8_SHIFT_SIZE );
}

inline int operator <= ( int num1, fixed8 const &num2 ) {
	return ( num1 << FIXED8_SHIFT_SIZE ) <= num2.number;
}

// Fixed point >=
inline int operator >= ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number >= num2.number;
}

inline int operator >= ( fixed8 const &num1, int num2 ) {
	return num1.number >= ( num2 << FIXED8_SHIFT_SIZE );
}

inline int operator >= ( int num1, fixed8 const &num2 ) {
	return ( num1 << FIXED8_SHIFT_SIZE ) >= num2.number;
}

// Fixed point ==
inline int operator == ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number == num2.number;
}

inline int operator == ( fixed8 const &num1, int num2 ) {
	return num1.number == ( num2 << FIXED8_SHIFT_SIZE );
}

inline int operator == ( int num1, fixed8 const &num2 ) {
	return ( num1 << FIXED8_SHIFT_SIZE ) == num2.number;
}

// Fixed point <
inline int operator < ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number < num2.number;
}

inline int operator < ( fixed8 const &num1, int num2 ) {
	return num1.number < ( num2 << FIXED8_SHIFT_SIZE );
}

inline int operator < ( int num1, fixed8 const &num2 ) {
	return ( num1 << FIXED8_SHIFT_SIZE ) < num2.number;
}

// Fixed point >
inline int operator > ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number > num2.number;
}

inline int operator > ( fixed8 const &num1, int num2 ) {
	return num1.number > ( num2 << FIXED8_SHIFT_SIZE );
}

inline int operator > ( int num1, fixed8 const &num2 ) {
	return ( num1 << FIXED8_SHIFT_SIZE ) >= num2.number;
}

// Fixed point >>
inline fixed8 operator >> ( fixed8 const &num1, fixed8 const &num2 ) {
	return fixed8( num1.number >> (int)num2, "" );
}

inline fixed8 operator >> ( fixed8 const &num1, int num2 ) {
	return fixed8( num1.number >> num2, "" );
}

inline fixed8 operator >> ( int num1, fixed8 const &num2 ) {
	return fixed8( num1 >> (int)num2 );
}

// Fixed point <<
inline fixed8 operator << ( fixed8 const &num1, fixed8 const &num2 ) {
	return fixed8( num1.number << (int)num2, "" );
}

inline fixed8 operator << ( fixed8 const &num1, int num2 ) {
	return fixed8( num1.number << num2, "" );
}

inline fixed8 operator << ( int num1, fixed8 const &num2 ) {
	return fixed8( num1 << (int)num2 );
}

// fixed8 point &&
inline int operator && ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number && num2.number;
}

inline int operator && ( fixed8 const &num1, int const num2 ) {
	return num1.number && num2;
}

inline int operator && ( int const num1, fixed8 const &num2 ) {
	return num1 && num2.number;
}

// fixed8 point ||
inline int operator || ( fixed8 const &num1, fixed8 const &num2 ) {
	return num1.number || num2.number;
}

inline int operator || ( fixed8 const &num1, int const num2 ) {
	return num1.number || num2;
}

inline int operator || ( int const num1, fixed8 const &num2 ) {
	return num1 || num2.number;
}

#endif
