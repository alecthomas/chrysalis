/*****************************************************************************
    Controls the game play of the game

  Author: Alec Thomas, Eric Sambach
  Update: Nov '94
  Comments:
*****************************************************************************/

#include <unistd.h>
#include "include.h"
#include "world.h"
#include "lib.h"
#include "image.h"
#include "monsters.h"
#include "player.h"

#ifdef __WATCOMC__
#include <conio.h>
#endif

int statusx = 8, statusy = 165;

int parse_INI_command( char *option, char *setting )
/* =====================
	Parse an INI command
*/
{
	if ( !strcmp( option, "detail" ) )
	{
		WORLD::layerswitch[ 1 ] = strncmp( setting, "off", 3 );
		if ( flags.debug ) cout << "background parallax layer " << (char*)( WORLD::layerswitch[ 1 ] ? "on" : "off" ) << endl;
		CHECK( ( setting = strchr( setting, ',' ) ) == NULL, "INI command \"detail\" incomplete" );
		setting++; while ( *setting == ' ' && *setting != 0 ) setting++;
		WORLD::layerswitch[ 3 ] = strncmp( setting, "off", 3 );
		if ( flags.debug ) cout << "foreground parallax layer " << (char*)( WORLD::layerswitch[ 3 ] ? "on" : "off" ) << endl;
		return TRUE;
	}
	if ( !strcmp( option, "retrace" ) )
	{
		flags.retrace = ( !strcmp( setting, "true" ) );
		if ( flags.debug ) cout << "vertical retrace checking " << (char*)(flags.retrace ? "on" : "off") << endl;
		return TRUE;
	}
	if ( !strcmp( option, "cheat" ) )
	{
		flags.cheat = ( !strcmp( setting, "ZaffodBeeblebrox" ) );
		if ( flags.debug ) cout << "cheat code " << (char*)(flags.cheat ? "on...greetings HitchHiker ;)" : "invalid") << endl;
		return TRUE;
	}
	return FALSE;
}

int executable()
/* ==============
	Return 2 because in game
*/
{
	return 2;
}

void title()
{
	cout << "ChRySaLiS " VERSION " ";
}

void untitle()
{
	if ( flags.debug ) cout << "exit ok" << endl;
}

void command_help()
/* =================
	Command line help
*/
{
}

void parse( int &argument )
/* =========
	Parse command-line arguments
*/
{
	CHECK( TRUE, "argument got through to PLAY parser" );
}

#ifdef __linux__
extern "C" void translucentbox( char *ptr, int w, int h, int colour );
#endif

#ifdef __WATCOMC__
void translucentbox( char *ptr, int w, int h, int colour );
#pragma aux translucentbox = \
		"cmp ecx, 0" \
		"je quit" \
		"cmp ebx, 0" \
		"je quit" \
		"shl eax, 8" \
		"add eax, TBL_translucent" \
		"mov edx, eax" \
		"push edi" \
		"push ecx" \
		"inc ah" \
		"inc ah" \
	"toprow:" \
		"mov al, [edi]" \
		"mov al, [eax]" \
		"mov [edi], al" \
		"inc edi" \
		"dec ecx" \
		"jnz toprow" \
		"pop ecx" \
		"pop edi" \
		"mov ah, dh" \
		"dec ebx" \
	"yloop:" \
		"push edi" \
		"push ecx" \
		"dec ecx" \
		"inc ah" \
		"inc ah" \
	"xloop:" \
		"mov al, [edi]" \
		"mov al, [eax]" \
		"mov ah, dh" \
		"mov [edi], al" \
		"inc edi" \
		"dec ecx" \
		"jnz xloop" \
		"dec ah" \
		"dec ah" \
		"mov al, [edi]" \
		"mov al, [eax]" \
		"mov [edi], al" \
		"mov ah, dh" \
		"pop ecx" \
		"pop edi" \
		"add edi, 320" \
		"dec ebx" \
		"jnz yloop" \
		"dec ah" \
		"dec ah" \
	"bottomrow:" \
		"mov al, [edi]" \
		"mov al, [eax]" \
		"mov [edi], al" \
		"inc edi" \
		"dec ecx" \
		"jnz bottomrow" \
	"quit:" \
	modify [edx] \
	parm [edi] [ecx] [ebx] [eax]

#endif


int go()
{
int l, pause = FALSE;
char buffer[ 32 ], cheatlist[ 9 ];
	
	strcpy( cheatlist, "        " );
	world = new WORLD();
	world->load( WORLD::firstlevel );
	G_setfont( largefont );
	world->createplayer();
	while ( K_pressed() != kbESC )
	{
		world->setlayer( 2 );
		if ( K_pressed() )
		{
			if ( K_index( kbALT ) )
			{
				if ( K_pressed() == kbP ) pause = !pause;
				if ( K_pressed() == kbK )
				{
					K_deinit();
					getc( stdin );
					K_init( NULL );
				}
				if ( flags.cheat )
				{ // cheat codes
					if ( K_pressed() == kbF1 )
						world->player->attribute.sceneryghost = !world->player->attribute.sceneryghost;
					if ( K_pressed() == kbF2 )
						world->player->attribute.creatureghost = !world->player->attribute.creatureghost;
				}
			} else
			{
			}
			K_clearpressed();
		}
		world->go();
		if ( flags.debug )
		{
			G_setfont( smallfont );
			V_blit( V_screen->ptr, V_screen->ptr, 320 * ( flags.superdebug ? 20 : 10 ), FADE( -4 ) );
			V_print( 0, 0, "creatures: ", SINGLE( 40 ) );
			sprintf( buffer, "%i", world->playlayer->creatureQ->items() );
			V_print( V_textwidth( "creatures: " ), 0, buffer, SINGLE( 255 ) );
			V_print( 80, 0, "scenery: ", SINGLE( 40 ) );
			sprintf( buffer, "%i", world->playlayer->sceneryQ->items() );
			V_print( 80 + V_textwidth( "scenery: " ), 0, buffer, SINGLE( 255 ) );
			if ( flags.superdebug )
			{
				if ( world->player->attribute.sceneryghost ) cheatlist[ 0 ] = 'S'; else cheatlist[ 0 ] = ' ';
				if ( world->player->attribute.creatureghost ) cheatlist[ 1 ] = 'C'; else cheatlist[ 1 ] = ' ';
				V_print( 0, 10, cheatlist, SINGLE( 255 ) );
			}
			G_setfont( largefont );
		}
		// display status
		for ( l = 0; l != 4; l++ )
		{
			switch ( l )
			{
				case 0 :
					translucentbox( V_screen->ytable[ statusy - 1 ] + statusx
						- 1 + l * 18, 18, 18, 109 );
				break;
				case 1 :
					translucentbox( V_screen->ytable[ statusy - 1 ] + statusx
						- 1 + l * 18, 18, 18, 61 );
				break;
				case 2 :
					translucentbox( V_screen->ytable[ statusy - 1 ] + statusx
						- 1 + l * 18, 18, 18, 29 );
				break;
				case 3 :
					translucentbox( V_screen->ytable[ statusy - 1 ] + statusx
						- 1 + l * 18, 18, 18, 253 );
				break;
			}
			world->actualplayer->display_shapeicon( l * 18 + statusx, statusy, l );
		}
		translucentbox( V_screen->ytable[ statusy + 17 ] + statusx - 1, 72, 9, 2 );
		V_fbox( statusx + 1, statusy + 19, world->player->health, 2, 255 );
		V_fbox( statusx + 1, statusy + 22, world->player->damage, 2, 29 );

		// display to screen
		world->display();
		usleep(250);
		if ( pause )
		{
			K_clearpressed();
			while ( !K_pressed() ) ;
		}
		if ( !world->player->attribute.alive || world->player->health <= 0 ||
			!world->actualplayer->attribute.alive ) return TRUE;
		if ( flags.retrace ) G_waitretrace();
	}
	delete world;
	return TRUE;
}
