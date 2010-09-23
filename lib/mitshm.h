/* Copyright (c) 1996, Brian Spilsbury. All right reservered */
/* May be used with permission of author. */
/* This includes Forge Software */

#ifndef _MITSHM_H_
#define _MITSHM_H_

#ifdef __cplusplus
extern "C" {
#endif

void setTitleX(char *s);
void Xinitialize (int width, int height);
void Xuninitialize ();
void setColor(int index, int r, int g, int b);

//void queryMouse(int *x, int *y, int unsigned *state);

void updateScreenClipped( int x, int y, int w, int h );
void setMouse(int x, int y);
void updateScreen();

void updateKeyboard();
void initKeyboard();
int queryKeyboard(int ind);
char *queryKeylinear();
void initKeylinear();

#define xPixel(x,y) *(shared_mem+x+y*xWidth)

extern int Xmousebuttons;
extern int Xmousex, Xmousey;
extern char Xkeypressed;
extern char keymap[128];
extern char Xkeyreleased;
extern char *shared_mem;
extern int xWidth;
extern int xHeight;

#ifdef __cplusplus
}
#endif

#endif
