/*****************************************************************************
  Control the movement of the base creature

  Author: Alec Thomas
  Update: 10th Dec '94
  Comments:
    Could be tricky trying to get the player to use the creature!!!
*****************************************************************************/

#include "include.h"
#include "creature.h"
#include "player.h"
#include "keyboard.h"

void CREATURE::reset_data()
{
	OBJECT::reset_data();
	memset(&activity, 0, sizeof(activity));
	memset(&ability, 0, sizeof(ability));
}

void CREATURE::load(FILE *f)
{
	OBJECT::load(f);
	fread(&ability, sizeof(ABILITY), 1, f);
}

void CREATURE::save(FILE *f)
{
	OBJECT::save(f);
	fwrite(&ability, sizeof(ABILITY), 1, f);
}

void CREATURE::performaction(ACTION *a)
{
}

void CREATURE::reset()
{
}
