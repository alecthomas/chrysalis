/*****************************************************************************
  Initialises graphics mode, LIBraries, etc.

  Author: Alec Thomas
  Update: Nov '94
  Comments:
*****************************************************************************/

#include "include.h"


#ifdef __WATCOMC__
#include <dos.h>
#endif

#include <signal.h>
#include "object.h"
#include "world.h"
#include "lib.h"
#include "data.h"

FLAGSSTRUCT flags = {
	// retrace
	FALSE,
	// debugging
	FALSE,
};

char *smallfont[ 128 ], *largefont[ 128 ];
int arguments, frame = 0;
char **argument;

void checkheap();
int go();
int parse_INI_command( char *command, char *setting );
void parse( int &argument );
void title();
void untitle();
int executable();
void command_help();
void clearline();

void clearline()
{
	if ( flags.debug ) cout << endl; else cout << "\r                                                                        \r";
}

void initindex()
{
int l, l1;
LIBLIST *ll;

	for ( l = l1 = 0; l != lib->entries; l++ )
	{
		ll = lib->examine( l );
		if ( !strncmp( ll->name, "CREATURE_", 9 ) ||
			!strncmp( ll->name, "SCENERY_", 8 ) ||
			!strncmp( ll->name, "ACTIVE_", 7 ) )
		{
			image->set( l1, ll->name );
			l1++;
		}
		cout.flush();
	}
	for ( l = lib->entrypercent( 20, l1 ); l != lib->entrypercent( 20,
		lib->entries ); l++ ) { cout << "."; cout.flush(); }
}

void kbhandler()
/*  =============
	Handle CTRL-ALT-DEL, *** NOTE : DO NOT CTRL-ALT-DEL UNDER OS/2 ***
*/
{
	//if ( K_index( kbCTRL ) && K_index( kbALT ) && K_index( kbpDEL ) ) exit( 1 );
}

void unmain()
/*  ==========
	General exit routine, delete classes, etc.
*/
{
	delete lib;
	delete image;
}

void detect_os()
/*  =============
	Detect DOS, OS/2, Windows and Desqview(/X)
*/
{
#ifdef __WATCOMC__
	RMIREGS r;
	int dver;
	
	memset( &r, 0, sizeof( RMIREGS ) );
	
	// Detect OS/2
	r.eax = 0x00003306;
	simRMint( 0x21, &r );
	dver = r.ebx;
	if ( ( r.ebx & 0xff ) >= 20 )
	{
		if ( ( r.ebx & 0xff ) == 20 && ( r.ebx >> 8 ) >= 30 )
			cout << "running under OS/2 Warp, ";
		else
			cout << "running under OS/2 v" << ( r.ebx & 0xff ) / 10 << "." << ( r.ebx >> 8 ) << endl;
		return;
	}
	
	// Detect Desqview
	r.eax = 0x00002b01;
	r.ecx = 0x00004445;
	r.edx = 0x00005351;
	r.ebx = 0x0000ffff;
	simRMint( 0x21, &r );
	if ( ( r.eax & 0xff ) != 0xff )
	{
		cout << "running under Desqview v";
		if ( r.ebx == 0x0000ffff )
			cout << "1.x, ";
		else
			cout << ( r.ebx >> 8 ) << "." << ( r.ebx & 0xff ) << endl;
		return;
	}
	
	// Detect Windows
	memset( &r, 0, sizeof( RMIREGS ) );
	r.eax = 0x0000160a;
	simRMint( 0x2f, &r );
	if ( r.eax == 0 )
	{
		if ( ( r.ebx >> 8 ) == 4 )
			cout << "running under Windows 95" << endl;
		else
			cout << "running under Windows v" << ( r.ebx >> 8 ) << "." << ( r.ebx & 0xff ) << endl;
		return;
	}
	
	// Default to running under DOS
	cout << "running under DOS v" << ( dver & 0xff ) << "." << ( dver >> 8 ) << endl;
#endif
#ifdef __linux__
	cout << "running under Linux" << endl;
#endif
}

int detect_memory()
/*  =================
	Detect the amount of free memory and exit if there isn't enough (>= 4m)
*/
{
int l, l1;
char *t;
	
	for ( l = 1; ; l++ )
	{
		if ( ( t = (char*)malloc( l * 1024 * 1024 ) ) == NULL )
			break;
		free( t );
	}
	return l;
}

int parse_INI_option( char *option, char *setting )
/* =================================================
	Evaluate an INI file option
*/
{
FILE *f;

	if ( !strcmp( option, "debug" ) )
	{
		flags.debug = ( !strcmp( setting, "true" ) );
		if ( flags.superdebug ) flags.debug = TRUE;
		if ( !strcmp( setting, "super" ) )
		{
			flags.superdebug = TRUE;
			flags.debug = TRUE;
		}
		if ( flags.debug ) cout << "debugging " << (char*)(flags.debug ? flags.superdebug ? "super" : "on" : "off") << endl;
		return TRUE;
	}
	if ( !strcmp( option, "cache" ) )
	{
		IMAGE::cachememory = strtol( setting, NULL, 10 ) * 1024;
		if ( flags.debug ) cout << "image cache set to " << setting << "K" << endl;
		return TRUE;
	}
	if ( !strcmp( option, "file" ) )
	{
		cout << "adding data library " << setting << " [                    ]\radding data library " << setting << " [";
		CHECK( !lib->mergelib( setting ), "invalid library" );
		clearline();
		if ( flags.debug ) cout << lib->entries << " entries in main library" << endl;
		return TRUE;
	}
	return FALSE;
}

void parse_INI()
{
FILE *io;
char option[ 128 ], *setting, num[ 32 ];
int section = -1;

	CHECK( ( io = fopen( "crysalis.ini", "rt" ) ) == NULL, "could not open CRYSALIS.INI" );
	do {
		fscanf( io, "%[\n\r ]", option );
		fscanf( io, "%[^\n\r]", option );
		if ( feof( io ) ) break;
		if ( option[ 0 ] == ';' )
		{ // comment
			fscanf( io, "%[^\n\r]", option );
			fscanf( io, "%[\n\r]", option );
			continue;
		}
		if ( option[ 0 ] == '[' )
		{ // section
			if ( !strcmp( option, "[general]" ) ) section = 0;
			if ( !strcmp( option, "[editor]" ) ) section = 1;
			if ( !strcmp( option, "[game]" ) ) section = 2;
			continue;
		}
		// split command into "option = setting"
		setting = strchr( option, '=' );
		*setting = 0;
		while ( *setting == ' ' ) { *setting = 0; setting--; }
		setting++;
		while ( *setting == 0 || *setting == ' ' ) setting++;
		while ( setting[ strlen( setting ) - 1 ] == ' ' )
			setting[ strlen( setting ) - 1 ] = 0;
		while ( option[ strlen( option ) - 1 ] == ' ' )
			option[ strlen( option ) - 1 ] = 0;
		switch ( section )
		{
			case 0 :	// general directives
				CHECK( !parse_INI_option( option, setting ), "invalid INI file directive" );
			break;
			case 1 :	// editor directives
				if ( executable() == 1 )
					CHECK( !parse_INI_command( option, setting ), "invalid editor directive in INI file" );
			break;
			case 2 :	// game directives
				if ( executable() == 2 )
					CHECK( !parse_INI_command( option, setting ), "invalid game directive in INI file" );
			break;
			default :	// fuck up
				ERROR( "INI directive not in valid section" );
			break;
		}
	} while( TRUE );
	fclose( io );
}

int main( int argc, char **argv )
/*  ==============================
	Initialise keyboard, signal handlers, libraries, keyboard, graphics mode,
	etc.
*/
{
int l, size, totalsize;
char sbuf[ 64 ], *tp;

	arguments = argc;
	argument = argv;

//	atexit( checkheap );

	title();

	cout << ( l = detect_memory() ) << "m free, ";
	CHECK( l < 4, "not enough available DPMI memory, 4m minimum required" );

	detect_os();
	if ( !flags.debug )
		cout << "Copyright (c) Forge Software Australia (FSA), 1994-1995" << endl;

	// Initialise data library
	lib = new LIB();

	// parse INI file...do this first
	parse_INI();

	// do command line stuff
	if ( arguments > 1 )
		for ( l = 1; l != arguments; l++ )
			if ( argument[ l ][ 0 ] == '-' || argument[ l ][ 0 ] == '/' )
				switch ( argument[ l ][ 1 ] )
				{
					case 'h' :
						cout << "  -warp [level]        warp to a level" << endl
							<<  "  -file [file]         merge a library" << endl;
						command_help();
						return 0;
					break;
					case 'd' :
						if ( !strcmp( argument[ l ] + 1, "debug" ) )
						{
							CHECK( ++l >= arguments, "argument -debug incomplete" );
							flags.debug = ( strcmp( argument[ l ], "on" ) == 0 );
							if ( !strcmp( argument[ l ], "super" ) )
								flags.debug = flags.superdebug = TRUE;
						}
					break;
					case 'f' :
						if ( !strcmp( argument[ l ] + 1, "file" ) )
						{
							CHECK( ++l >= arguments, "argument -debug incomplete" );
							cout << "adding data library " << argument[ l ] << " [שששששששששששששששששששש]\radding data library " << argument[ l ] << " [";
							CHECK( !lib->mergelib( argument[ l ] ), "invalid library" );
							clearline();
							if ( flags.debug ) cout << lib->entries << " entries in main library" << endl;
						}
					break;
					case 'w' :
						if ( !strcmp( argument[ l ] + 1, "warp" ) )
						{
							CHECK( ++l >= arguments, "argument -warp incomplete" );
							cout << "warping to level " << argument[ l ] << endl;
							WORLD::firstlevel = strtol( argument[ l ], NULL, 10 );
						} else
							parse( l );
					break;
					default :
						parse( l );
					break;
				}
			else
			{ // quit with an error
				strcpy( sbuf, "invalid command line argument: " );
				strcat( sbuf, argument[ l ] );
				CHECK( TRUE, sbuf );
			}

	cout << "initialising image index [                    ]\rinitialising image index [";
	// Initialise image swapping routines
	image = new IMAGE();
	initindex();
	clearline();
	
	atexit( unmain );

	// Load palette
	CHECK( lib->seek( "MISC_PALETTE" ) == -1, "could not retrieve PALETTE" );
	fread( palette, 1, 768, lib->io );

	// Load translucency table
	CHECK( lib->seek( "TABLES_TRANSLUC" ) == -1, "could not retrieve image TABLES_TRANSLUC" );
	fseek( lib->io, +8, SEEK_CUR );
	V_loadtranslucency( lib->io );

	// Load fonts
	CHECK( ( smallfont[ 32 ] = lib->retrieve_image( "FONT_SMALL" ) ) == NULL,
		"could not retrieve FONT_SMALL" );
	for ( l = 33; l != 127; l++ )
		smallfont[ l ] = G_loadimage( lib->io );
	CHECK( ( largefont[ 32 ] = lib->retrieve_image( "FONT_LARGE" ) ) == NULL,
		"could not retrieve FONT_LARGE" );
	for ( l = 33; l != 127; l++ )
		largefont[ l ] = G_loadimage( lib->io );
	G_setfont( smallfont );
	
	// Initialise and check VGA
	CHECK( !G_init(), "could not initialise/detect VGA" );

	// Initialise keyboard handler
	K_init( kbhandler );
	
	// Initialise virtual page driver
	CHECK( !V_init(), "could not initialise virtual page driver" );
	
	// Actually start playing
	CHECK( !go(), "go() failed" );

	G_deinit();

	untitle();
	if ( flags.debug )
	{
		cout << image->cache_used() << "% of cache used" << endl;
		if ( image->cache_overflow() ) cout << "cache overflowed " << image->cache_overflow() << " time(s)" << endl;
		cout << image->cache_active_images() << " images in cache of a possible " << image->images() << endl;
		for ( l = totalsize = size = 0; l != image->images(); l++ )
		{
			if ( image->in_cache( l ) ) size += image->get_size( l );
			totalsize += image->get_size( l );
		}
		cout << ( size / 1024 ) << "k of a possible " << ( totalsize / 1024 ) << "k images in cache" << endl;
		cout << lib->entries << " entries in library" << endl;
		cout << detect_memory() << "m free" << endl;
	}

	return 0;
}
