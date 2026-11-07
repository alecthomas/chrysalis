/*
	Portable C port of vblit.S.

	The assembler version relied on TBL_translucent and TBL_shade being
	64K aligned so that a table lookup could be built by loading bytes into
	AL/AH. The explicit indices below reproduce the same lookups, including
	the differing byte order of each mode.
*/

#include <string.h>
#include "virtual.h"

void V_blit( const char *source, char *target, unsigned len, unsigned mode )
{
const unsigned char *s = (const unsigned char *)source;
unsigned char *t = (unsigned char *)target;
const unsigned char *trans = (const unsigned char *)TBL_translucent,
	*shade = (const unsigned char *)TBL_shade;
unsigned l, level;
unsigned char c;

	switch ( mode >> 24 )
	{
		case MOV >> 24 :
			memmove( t, s, len );
		break;
		case MASK >> 24 :
			for ( l = 0; l != len; l++ )
				if ( s[ l ] ) t[ l ] = s[ l ];
		break;
		case XOR >> 24 :
			for ( l = 0; l != len; l++ )
				if ( s[ l ] ) t[ l ] ^= s[ l ];
		break;
		case OR >> 24 :
			for ( l = 0; l != len; l++ )
				if ( s[ l ] ) t[ l ] |= s[ l ];
		break;
		case AND >> 24 :
			for ( l = 0; l != len; l++ )
				if ( s[ l ] ) t[ l ] &= s[ l ];
		break;
		case TRANSLUCENT >> 24 :
			for ( l = 0; l != len; l++ )
				if ( s[ l ] ) t[ l ] = trans[ ( t[ l ] << 8 ) | s[ l ] ];
		break;
		case ALIAS >> 24 :
			// Anti-alias the left and right edges of the image against the
			// background. Like the original, this reads one byte either side
			// of the span.
			for ( l = 0; l != len; l++ )
			{
				if ( !( c = s[ l ] ) ) continue;
				if ( !s[ l + 1 ] ) c = trans[ ( t[ l + 1 ] << 8 ) | c ];
				if ( !s[ (int)l - 1 ] ) c = trans[ ( t[ (int)l - 1 ] << 8 ) | c ];
				t[ l ] = c;
			}
		break;
		case 7 : // TINT, unimplemented in the original
		case 8 : // TINTBACK, unimplemented in the original
		break;
		case 9 : // SINGLE
			for ( l = 0; l != len; l++ )
				if ( s[ l ] ) t[ l ] = (unsigned char)mode;
		break;
		case 10 : // SOLID
			memset( t, (unsigned char)mode, len );
		break;
		case 11 : // FADE
			level = mode & 0x1f;
			if ( level == 15 )
				V_blit( source, target, len, MASK );
			else
				for ( l = 0; l != len; l++ )
					if ( s[ l ] ) t[ l ] = shade[ ( level << 8 ) | s[ l ] ];
		break;
		case 13 : // SHADE
			level = mode & 0x1f;
			for ( l = 0; l != len; l++ )
				if ( s[ l ] ) t[ l ] = shade[ ( level << 8 ) | t[ l ] ];
		break;
		case 14 : // TRANSLUCENTFADE
			level = mode & 0x1f;
			if ( level == 15 )
				V_blit( source, target, len, TRANSLUCENT );
			else
				for ( l = 0; l != len; l++ )
					if ( s[ l ] )
						t[ l ] = trans[ ( s[ l ] << 8 ) | shade[ ( level << 8 ) | t[ l ] ] ];
		break;
	}
}
