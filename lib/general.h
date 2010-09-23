#ifndef __GENERAL_H
#define __GENERAL_H

#define TRUE       1
#define FALSE      0

#ifndef NULL
  #define NULL     0
#endif

#define FIXEDSHIFT 8

#ifdef __DEBUG__
#ifndef REPORT
#define REPORT( s ) printf( "%s: %s\n\r", __PRETTY_FUNCTION__, s )
#endif
#ifndef ERROR
#define ERROR( s ) { printf( "%s (fatal error): %s\n\r", __PRETTY_FUNCTION__, s ); exit( 1 ); }
#endif
#else
#ifndef REPORT
#define REPORT( s )
#endif
#ifndef ERROR
#define ERROR( s ) { printf( "error: %s\n\r", s ); exit( 1 ); }
#endif
#endif

#define __unfixedround( n ) ((n) >> FIXEDSHIFT)
#define __fixed( n ) ((n) << FIXEDSHIFT)
#define __unfixed( n ) ((n) >> FIXEDSHIFT)

typedef struct {
  long
    edi,
    esi,
    ebp,
    reserved,
    ebx,
    edx,
    ecx,
    eax;
  short
    flags,
    es, ds, fs, gs, ip, cs, sp, ss;
} RMIREGS;

extern int
  costable[ 256 ], sintable[ 256 ];
extern char sqrt_table[];

char *tokenise( char *&source, char *target, char *whitespace,
  char *operators );

#ifdef __WATCOMC__
int simRMint( int i, RMIREGS *r );
char far *lowmalloc( int l );
void lowfree( char far *p );
#endif

inline void xchg( int &a, int &b )
{
int t;

  t = a;
  a = b;
  b = t;
}

inline void xchgb( char &a, char &b )
{
char t;

  t = a;
  a = b;
  b = t;
}

// Clips an image to an area, returns TRUE if image is to be displayed
inline int clip( char *&image, int &x, int &y, int &w, int &h, int tw, int th )
{
  image += 8;
  if ( w < 0 )
  {
    w = -w + 1;
    x -= w - 1;
  }
  if ( h < 0 )
  {
    h = -h + 1;
    y -= h - 1;
  }
  if ( y < 0 )
    { h += y; image -= y * w; y = 0; }
  if ( x < 0 )
    { w += x; image -= x; x = 0; }
  if ( x + w - 1 >= tw )
    w = tw - x;
  if ( y + h - 1 >= th )
    h = th - y;
  if ( w <= 0 || h <= 0 ) return FALSE;
  return TRUE;
}

inline int touching( int x, int y, int w, int h, int x1, int y1, int w1, int h1 )
{
  return !( x > x1 + w1 - 1 || y > y1 + h1 - 1 || x + w - 1 < x1 || y + h - 1 < y1 );
}

inline int touching( int x, int y, int x1, int y1, int w1, int h1 )
{
  return !( x > x1 + w1 - 1 || y > y1 + h1 - 1 || x < x1 || y < y1 );
}

#ifdef __GNUC__
/* Fast SQRT algorithm, courtesy of Jare - uses table, slightly inaccurate */
static inline float fsqrt( float a )
{
	asm volatile (
		"movl %%eax, %%edx;"
		"andl $0x007fffff, %%eax;"
		"subl $0x3f800000, %%edx;"
		"movl %%edx, %%ebx;"
		"andl $0xff000000, %%edx;"
		"sarl $1, %%edx;"
		"andl $0x00800000, %%ebx;"
		"addl $0x3f800000, %%edx;"
		"orl %%ebx, %%eax;"
		"shrl $16, %%eax;"
		"movl sqrt_table(,%%eax,4), %%eax;"
		"addl %%edx, %%eax"
	: "=a" (a) : "a" (a) : "bx", "dx" );
	return a;
}

#endif
#endif
