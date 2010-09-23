/*
	Fixed point
	===========
	A class to implement fixed point. Handles all possible combinations of
	integer/fixed +,-,*,/,etc.

	Author:		Alec Thomas
	Date:		27th April '95
	Comments:
		Still have to implement logical operators such as && and ||.
*/
#ifndef __FIXED_H__
#define __FIXED_H__

class ostream;
class istream;

#pragma inline_recursion on

#define FIXED_SHIFT_SIZE	16
#define FIXED_MASK_VALUE	0xffff

#ifdef __WATCOMC__
// shrd value shouuld be FIXED_SHIFT_SIZE
int __fixed_mul(int a, int b);
#pragma aux __fixed_mul = \
	"imul edx" \
	"shrd eax, edx, 16" \
	parm [eax] [edx] \
	value [eax]

// shrd/sar values should be (32-FIXED_SHIFT_SIZE)
int __fixed_div(int a, int b);
#pragma aux __fixed_div = \
	"mov eax, 0" \
	"shrd eax, edx, 16" \
	"sar edx, 16" \
	"idiv ebx" \
	parm [edx] [ebx] \
	value [eax]
#endif

#ifdef __linux__

inline int __fixed_mul(int left, int right)
{
	asm volatile (	
		"imull %%edx\n"
		"shrdl $16, %%edx, %%eax\n" 
		: "=a" (left)
		: "a" (left), "d" (right) 
		);
	return left;
}

inline int __fixed_div(int left, int right)
{
	asm volatile (	
		"movl $0, %%eax\n"
		"shrdl $16, %%edx, %%eax\n"
		"sarl $16, %%edx\n"
		"idivl %%ebx\n" 
		: "=a" (left)
		: "d" (left), "b" (right) 
	);
	return left;
}

#endif

class fixed {
	public :
		int number;
		inline fixed() {}
		inline fixed(int i, int f)
		{
			number = (i << FIXED_SHIFT_SIZE) | (f & FIXED_MASK_VALUE);
		}
		inline fixed(int v)
		{
			number = v << FIXED_SHIFT_SIZE;
		}
		inline fixed(double num) 
		{
		int n = (int)num;

			number = (n << (32 - FIXED_SHIFT_SIZE)) + 
				((int)((num - n) * 65536.0));
		}
		inline fixed(float num) 
		{
		int n = (int)num;

			number = (n << (32 - FIXED_SHIFT_SIZE)) + 
				((int)((num - n) * 65536.0));
		}
		inline fixed(fixed const &v) { number = v.number; }
		inline fixed(int i, const char *dummy) { (void)dummy; number = i; }
		inline ~fixed() {}
		inline int integer() const { 
			return number < 0 ? -((-number) >> FIXED_SHIFT_SIZE) : 
				number >> FIXED_SHIFT_SIZE;
		}
		inline int fraction() const {
			return number < 0 ? (-number) & FIXED_MASK_VALUE : 
				number & FIXED_MASK_VALUE;
		}
		inline fixed &operator = (fixed const &num) {
			number = num.number;
			return *this;
		}
		inline fixed &operator = (double num) 
		{
			number = ((int)num << (32 - FIXED_SHIFT_SIZE)) + 
				((int)((num - (int)num) * 65536));
			return *this;
		}
		inline fixed &operator = (float num) 
		{
			number = ((int)num << (32 - FIXED_SHIFT_SIZE)) + 
				((int)((num - (int)num) * 65536));
			return *this;
		}
		inline fixed &operator = (int num) {
			number = num << FIXED_SHIFT_SIZE;
			return *this;
		}
		inline fixed &operator ++ (int) {
			number += 1 << FIXED_SHIFT_SIZE;
			return *this;
		}
		inline fixed &operator -- (int) {
			number -= 1 << FIXED_SHIFT_SIZE;
			return *this;
		}
		inline fixed &operator += (int num) {
			number += num << FIXED_SHIFT_SIZE;
			return *this;
		}
		inline fixed &operator += (fixed const &num) {
			number += num.number;
			return *this;
		}
		inline fixed &operator -= (int num) {
			number -= num << FIXED_SHIFT_SIZE;
			return *this;
		}
		inline fixed &operator -= (fixed const &num) {
			number -= num.number;
			return *this;
		}
		inline fixed &operator *= (int num) {
			number *= num;
			return *this;
		}
		inline fixed &operator *= (fixed const &num) {
			number = __fixed_mul(number, num.number);
			return *this;
		}
		inline fixed &operator /= (int num) {
			number /= num;
			return *this;
		}
		inline fixed &operator /= (fixed const &num) {
			number = __fixed_div(number, num.number);
			return *this;
		}
		inline fixed &operator <<= (int num) {
			number <<= num;
			return *this;
		}
		inline fixed &operator <<= (fixed const &num) {
			number <<= num.number;
			return *this;
		}
		inline fixed &operator >>= (int num) {
			number >>= num;
			return *this;
		}
		inline fixed &operator >>= (fixed const &num) {
			number >>= num.number;
			return *this;
		}
		inline fixed operator - () const { return fixed(-number, ""); }
		inline fixed operator + () const { return fixed(number, ""); }
		inline operator int () const { return integer(); }
		inline operator double () const { 
			return (double)integer() + 
				(double)(fraction() / 65536.0); 
			}
		inline operator float () const { 
			return (float)(number >> FIXED_SHIFT_SIZE) + 
				((float)(number & FIXED_SHIFT_SIZE) / 65536.0); 
		}
		friend inline fixed sin(fixed);
		friend inline fixed cos(fixed);
		friend inline fixed abs(fixed);

	private :
		static int sintable[ 512 ], costable[ 512 ];
};

ostream &operator << (ostream &outfile, fixed const &f);
istream &operator >> (istream &outfile, fixed const &f);

// Misc routines
fixed sqrt(fixed number);
int sqrt(int number);

inline fixed abs(fixed num)
{
	if (num.number < 0) return fixed(-num.number);
	return fixed(num.number);
}

inline fixed sin(fixed angle) {
	if (angle.number < 0)
		return fixed(-fixed::sintable[ ((-angle.number) >> FIXED_SHIFT_SIZE) & 0x1ff ], "");
	else
		return fixed(fixed::sintable[ (angle.number >> FIXED_SHIFT_SIZE) & 0x1ff ], "");
}

inline fixed cos(fixed angle) {
	if (angle.number < 0)
		return fixed(fixed::costable[ ((-angle.number) >> FIXED_SHIFT_SIZE) & 0x1ff ], "");
	else
		return fixed(fixed::costable[ (angle.number >> FIXED_SHIFT_SIZE) & 0x1ff ], "");
}

// Fixed point addition
inline fixed operator + (fixed const &num1, fixed const &num2) {
	return fixed(num1.number + num2.number, "");
}

inline fixed operator + (fixed const &num1, int num2) {
	return fixed(num1.number + (num2 << FIXED_SHIFT_SIZE), "");
}

inline fixed operator + (int num1, fixed const &num2) {
	return fixed((num1 << FIXED_SHIFT_SIZE) + num2.number, "");
}

// Fixed point subtraction
inline fixed operator - (fixed const &num1, fixed const &num2) {
	return fixed(num1.number - num2.number, "");
}

inline fixed operator - (fixed const &num1, int num2) {
	return fixed(num1.number - (num2 << FIXED_SHIFT_SIZE), "");
}

inline fixed operator - (int num1, fixed const &num2) {
	return fixed((num1 << FIXED_SHIFT_SIZE) - num2.number, "");
}

// Fixed point multiplication
inline fixed operator * (fixed const &num1, fixed const &num2) {
	return fixed(__fixed_mul(num1.number, num2.number), "");
}

inline fixed operator * (fixed const &num1, int num2) {
	return fixed(num1.number * num2, "");
}

inline fixed operator * (int num1, fixed const &num2) {
	return fixed(num1 * num2.number, "");
}

// Fixed point division
inline fixed operator / (fixed const &num1, fixed const &num2) {
	return fixed(__fixed_div(num1.number, num2.number), "");
}

inline fixed operator / (fixed const &num1, int num2) {
	return fixed(num1.number / num2, "");
}

inline fixed operator / (int num1, fixed const &num2) {
	return fixed(__fixed_div(num1 << FIXED_SHIFT_SIZE, num2.number), "");
}

// Fixed point !=
inline int operator != (fixed const &num1, fixed const &num2) {
	return num1.number != num2.number;
}

inline int operator != (fixed const &num1, int num2) {
	return num1.number != (num2 << FIXED_SHIFT_SIZE);
}

inline int operator != (int num1, fixed const &num2) {
	return (num1 << FIXED_SHIFT_SIZE) != num2.number;
}

// Fixed point <=
inline int operator <= (fixed const &num1, fixed const &num2) {
	return num1.number <= num2.number;
}

inline int operator <= (fixed const &num1, int num2) {
	return num1.number <= (num2 << FIXED_SHIFT_SIZE);
}

inline int operator <= (int num1, fixed const &num2) {
	return (num1 << FIXED_SHIFT_SIZE) <= num2.number;
}

// Fixed point >=
inline int operator >= (fixed const &num1, fixed const &num2) {
	return num1.number >= num2.number;
}

inline int operator >= (fixed const &num1, int num2) {
	return num1.number >= (num2 << FIXED_SHIFT_SIZE);
}

inline int operator >= (int num1, fixed const &num2) {
	return (num1 << FIXED_SHIFT_SIZE) >= num2.number;
}

// Fixed point ==
inline int operator == (fixed const &num1, fixed const &num2) {
	return num1.number == num2.number;
}

inline int operator == (fixed const &num1, int num2) {
	return num1.number == (num2 << FIXED_SHIFT_SIZE);
}

inline int operator == (int num1, fixed const &num2) {
	return (num1 << FIXED_SHIFT_SIZE) == num2.number;
}

// Fixed point <
inline int operator < (fixed const &num1, fixed const &num2) {
	return num1.number < num2.number;
}

inline int operator < (fixed const &num1, int num2) {
	return num1.number < (num2 << FIXED_SHIFT_SIZE);
}

inline int operator < (int num1, fixed const &num2) {
	return (num1 << FIXED_SHIFT_SIZE) < num2.number;
}

// Fixed point >
inline int operator > (fixed const &num1, fixed const &num2) {
	return num1.number > num2.number;
}

inline int operator > (fixed const &num1, int num2) {
	return num1.number > (num2 << FIXED_SHIFT_SIZE);
}

inline int operator > (int num1, fixed const &num2) {
	return (num1 << FIXED_SHIFT_SIZE) >= num2.number;
}

// Fixed point >>
inline fixed operator >> (fixed const &num1, fixed const &num2) {
	return fixed(num1.number >> (int)num2, "");
}

inline fixed operator >> (fixed const &num1, int num2) {
	return fixed(num1.number >> num2, "");
}

inline fixed operator >> (int num1, fixed const &num2) {
	return fixed(num1 >> (int)num2);
}

// Fixed point <<
inline fixed operator << (fixed const &num1, fixed const &num2) {
	return fixed(num1.number << (int)num2, "");
}

inline fixed operator << (fixed const &num1, int num2) {
	return fixed(num1.number << num2, "");
}

inline fixed operator << (int num1, fixed const &num2) {
	return fixed(num1 << (int)num2);
}

// fixed point &&
inline int operator && (fixed const &num1, fixed const &num2) {
	return num1.number && num2.number;
}

inline int operator && (fixed const &num1, int const num2) {
	return num1.number && num2;
}

inline int operator && (int const num1, fixed const &num2) {
	return num1 && num2.number;
}

// fixed point ||
inline int operator || (fixed const &num1, fixed const &num2) {
	return num1.number || num2.number;
}

inline int operator || (fixed const &num1, int const num2) {
	return num1.number || num2;
}

inline int operator || (int const num1, fixed const &num2) {
	return num1 || num2.number;
}

#endif
