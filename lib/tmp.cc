#include <stdio.h>
#include "graphics.h"
#include "virtual.h"

VPAGESTRUCT *page;

int main()
{
	G_init();
	V_init();
	V_setpage(page = V_createpage(320, 200));
	for (int i = 0; i < 1000; i++) 
	{
		V_fbox(rand() % 320, rand() % 200, rand() % 320, rand() % 200, rand() % 255);
		V_showpage();
	}
	return 0;
}
