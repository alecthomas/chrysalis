#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#define VERSION   "0.1.1"

#define  GETCH()  { while ( !keypressed ); keypressed = 0; }

#include <iostream>
#include <string.h>
#include <stdlib.h>

using std::cout;
using std::endl;

#ifdef __WATCOMC__
#include <dos.h>
#include <io.h>
#include <i86.h>
#endif

#include "fixed.h"
#include "keyboard.h"
#include "graphics.h"
#include "virtual.h"
#include "chkerror.h"

//inline void ERROR( char *m ) { G_deinit(); cout << "error: " << m << "\n\r"; exit( 1 ); }
//inline void ERROR( char *m, char *m1 ) { G_deinit(); cout << "error: " << m << m1 << "\n\r"; exit( 1 ); }

extern int arguments;
extern char **argument;

#endif
