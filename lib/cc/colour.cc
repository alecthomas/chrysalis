#include <fixed.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include "colour.h"
#include "video.h"
#include "font.h"

void Colour::initTables( char *fileName, char *palette )
/* ==============================================
	Initialise the RGB->indexed conversion table
*/
{
unsigned r, g, b, lr, lg, lb, lookingFor, found, closestSquare, closestIndex, l;
unsigned longRGB[ 768 ];
char *spinChar = "|/-\\", spinIndex = 0;
FILE *file;

	clog << "initialising colour conversion tables ";
	if ( ( file = fopen( fileName, "rb" ) ) != NULL )
	{
		fread( &toActual, sizeof( toActual ), 1, file );
		fclose( file );
		clog << "loaded" << endl;
		return;
	}
	clog << "|";
	for ( l = 0; l != 768; l++ ) longRGB[ l ] = Video::palette[ l ];
	for ( r = 0; r != 32; r++ )
	{
		for ( g = 0; g != 32; g++ )
		{
			for ( b = 0; b != 32; b++ )
			{
				closestSquare = 0xffffffff;
				for ( l = 0; l < 768; l += 3 )
				{
					lr = longRGB[ l ] - ( r << 1 );
					lg = longRGB[ l + 1 ] - ( g << 1 );
					lb = longRGB[ l + 2 ] - ( b << 1 );
					found = lr * lr + lg * lg + lb * lb;
					if ( found < closestSquare )
					{
						closestSquare = found;
						closestIndex = l;
						if ( found == 0 ) break;
					}
				}
				toActual[ r ][ g ][ b ] = closestIndex / 3;
			}
		}
		clog << "" << spinChar[ ( spinIndex++ & 3 ) ];
	}
	clog << "writing ";
	if ( ( file = fopen( fileName, "w+b" ) ) == NULL ) 
	{
		cerr << "could not create colour conversion table" << endl;
		exit( 1 );
	}
	fwrite( &toActual, sizeof( toActual ), 1, file );
	fclose( file );
	clog << endl;
}

Font &operator << ( Font &outfile, Colour &colour )
{
	outfile << "(" << colour.r << "," << colour.g << "," << colour.b << ")";
	return outfile;
}

ostream &operator << ( ostream &outfile, Colour &colour )
{
	outfile << "(" << colour.r << "," << colour.g << "," << colour.b << ")";
	return outfile;
}
