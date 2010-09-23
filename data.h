/*****************************************************************************
  Holds definitions of global data stuff

    Author: Alec Thomas
    Date: Nov '94
    Comments:
*****************************************************************************/

#ifndef __DATA_H__
#define __DATA_H__

typedef struct {
	int
		retrace : 1,
		superdebug : 1,
		debug : 1,
		cheat : 1;
} FLAGSSTRUCT;

class WORLD;
class LIB;
class IMAGE;

extern FLAGSSTRUCT flags;
extern char *smallfont[ 128 ], *largefont[ 128 ];
extern int frame;
extern WORLD *world;
extern IMAGE *image;
extern LIB *lib;
#endif
