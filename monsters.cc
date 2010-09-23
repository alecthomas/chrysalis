#include "include.h"
#include "creature.h"
#include "monsters.h"
#include "world.h"
#include "player.h"

/*  Triffid code
   ============== */
TRIFFID::TRIFFID(fixed cx, fixed cy, int o)
{
	// set up attributes
	reset_data();
	x = cx; y = cy;
	id = o;
	reset();
}

void TRIFFID::reset()
{
	attribute.alive = TRUE;
	attribute.dynamic = TRUE;
	attribute.sceneryghost = TRUE;
	action = frame = 0;
	subaction = 16;
	action = rand() % 100;
	if (attribute.possessed)
		sporex = 17;
	else
		sporex = 23;
}

void TRIFFID::go()
{
	if (!subaction) // bring spore up
		if (action != 0)
			action--;
		else
			activity.shot = TRUE;
	performaction(&activity);
}

void TRIFFID::display(int xadj)
{
	image->put(x - xadj + sporex, y + subaction, id, 1, fade);
	image->put(x - xadj, y, id, frame, fade);
}

void TRIFFID::performaction(ACTION *p)
{
CREATURE *o;

	if (p->shot && !subaction)
	{ // spawn a spore.....heh heh that's funny :)
		world->inserttemporarycreature(priority + 1, (o = new TRIFFIDSPORE(x + sporex, y - 2, this)));
		o->attribute.possessed = attribute.possessed;
		o->setfade((fade & 0xff) - 15);
		o->reset();
		p->shot = FALSE;
		action = rand() % 100;
		subaction = 16;
	}
	if (subaction) subaction--;
	// check if the sucker's off the screen
	if (x + getw() < world->getposition()) attribute.alive = FALSE;
	if (health <= 0) attribute.alive = FALSE;
}

TRIFFIDSPORE::TRIFFIDSPORE(fixed cx, fixed cy, TRIFFID *p)
{
	reset_data();
	x = cx; y = cy;
	id = p->id;
	reset();
}

void TRIFFIDSPORE::reset()
{
	attribute.alive = TRUE;
	attribute.dynamic = TRUE;
	attribute.sceneryghost = TRUE;
	attribute.temporary = TRUE;
	frame = 1;
	ydir = -16;
	action = 0;
	if (attribute.possessed) xdir = -1; else xdir = +1;
}

void TRIFFIDSPORE::go()
{
	action++;
	if (action > 8 && action < 27) // fire spore up and down
		frame = (action - 8) / 2 + 1;
	if (y > 200) // end of the line for you punk
		attribute.alive = FALSE;
	if (ydir++ > 16) ydir = 16;
	if (action == 4 || action == 8) xdir += xdir < 0 ? -1 : +1;
	if (health <= 0) attribute.alive = FALSE;
	x += xdir; y += ydir;
}

#define SLIMER_STOPPED		0
#define SLIMER_WALKING		1
#define SLIMER_FALLING		2
#define SLIMER_SPLATTING	3

SLIMER::SLIMER(fixed cx, fixed cy, int o)
{
	reset_data();
	x = cx; y = cy; id = o;
	reset();
}

void SLIMER::reset()
{
	attribute.alive = TRUE;
	attribute.dynamic = TRUE;
	attribute.possessable = TRUE;
	action = SLIMER_STOPPED;
	damage = 5;
	frame = 7;
	ydir = 2;
}

void SLIMER::go()
{
	activity.left = TRUE;
	performaction(&activity);
}

void SLIMER::performaction(ACTION *a)
{
OBJECT *o;

	xdir = 0;
	switch (action)
	{
		case SLIMER_STOPPED :
			ydir = 2;
			frame = 7;
			if (a->left || a->right) action = SLIMER_WALKING;
		break;
		case SLIMER_WALKING :
			ydir = 2;
			if (!a->left && !a->right) action = SLIMER_STOPPED;
			if (a->left)
			{
				xdir += -4;
				frame++;
				if (frame == 10) frame = 0;
			}
			if (a->right)
			{
				xdir += +4;
				frame++;
				if (frame == 10) frame = 0;
			}
		break;
		case SLIMER_FALLING :
			frame++;
			if (frame > 26) frame = 26;
		break;
		case SLIMER_SPLATTING :
			frame++;
			if (frame > 22)
			{
				frame = 7;
				action = SLIMER_STOPPED;
			}
		break;
	}
	// ran into an object on the Y axis
	if (action != SLIMER_SPLATTING)
		if (world->layer->updateycollision(this))
		{ // splatting?
			if (action == SLIMER_FALLING)
			{
				action = SLIMER_SPLATTING;
				frame = 16;
			}
		} else
	{ // didn't run into an object on the Y axis :)
		if (action != SLIMER_FALLING)
		{
			action = SLIMER_FALLING;
			frame = 22;
		} else
			ydir++;
	}
	// ran into an object on the X axis
	if (world->layer->updatexcollision(this)) action = SLIMER_STOPPED;
	if (x + getw() < world->getposition()) attribute.alive = FALSE;
	if (y > 200) attribute.alive = FALSE;
}

WINGEDMONKEY::WINGEDMONKEY(fixed cx, fixed cy, int o)
{
	reset_data();
	x = cx; y = cy; id = o;
	reset();
}

void WINGEDMONKEY::reset()
{
	attribute.alive = TRUE;
	attribute.dynamic = TRUE;
	attribute.possessable = TRUE;
	action = 0;
	frame = 0;
}

void WINGEDMONKEY::go()
{
	activity.left = TRUE;
	performaction(&activity);
}

void WINGEDMONKEY::performaction(ACTION *a)
{
	if (!a->left && !a->right)
	{
		if (xdir < 2) xdir++;
		if (xdir > 2) xdir--;
	}
	if (!a->up && !a->down)
	{
		if (ydir < 0) ydir++;
		if (ydir > 0) ydir--;
	}
	if (a->left) xdir--;
	if (a->right) xdir++;
	if (a->up) ydir--;
	if (a->down) ydir++;
	if (xdir > 8) xdir = 8;
	if (xdir < -6) xdir = -6;
	if (ydir > 8) ydir = 8;
	if (ydir < -8) ydir = -8;
	if (x + getw() < world->getposition()) attribute.alive = FALSE;
	if (world->layer->updateycollision(this)) ydir = 0;
	if (world->layer->updatexcollision(this)) xdir = 0;
	if (!action) frame++; else frame--;
	if (frame == 3 || frame == 0) action = !action;
}
