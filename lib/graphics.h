#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdio.h>

extern int
  G_screenwidth,
  G_screenheight;

extern char
  *G_oldscreen,
  G_oldmode,
  *G_screen,
  **G_ytable,
  **font,
  fadepalette[ 768 ],
  palette[ 768 ];

void G_setpalette( int start, int finish, char *pal );
void G_getpalette( int start, int finish, char *pal );
int G_init( void );
int G_setmode( int mode );
void G_deinit( void );
void G_waitretrace();
int G_loadfont( char **font, FILE *handle );
void G_setfont( char **font );
void G_saveimage( char *image, FILE *handle );
char *G_loadimage( FILE *handle );
void G_skipimage( FILE *handle );
char *G_loadimage16( FILE *handle );

#ifdef __WATCOMC__
#pragma aux G_setpalette = \
    "sub ecx, ebx" \
    "inc ecx" \
    "imul eax, ecx, 3" \
    "mov ecx, eax" \
    "mov eax, ebx" \
    "mov dx, 3c8h" \
    "out dx, al" \
    "inc dx" \
    "cld" \
    "rep outsb" \
    parm [ebx] [ecx] [esi] \
    modify [edx]

#pragma aux G_getpalette = \
    "sub ecx, ebx" \
    "inc ecx" \
    "imul eax, ecx, 3" \
    "mov ecx, eax" \
    "mov eax, ebx" \
    "mov dx, 3c7h" \
    "out dx, al" \
    "inc dx" \
    "inc dx" \
    "cld" \
    "rep insb" \
    parm [ebx] [ecx] [edi] \
    modify [edx]

#pragma aux G_waitretrace = \
    "mov dx, 03dah" \
  "waitr1:" \
    "in al,dx" \
    "test al,08h" \
    "jnz waitr1" \
  "waitr2:" \
    "in al,dx" \
    "test al,08h" \
    "jz waitr2" \
    modify [edx eax]
#endif
#endif
