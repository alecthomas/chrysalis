#include <ctype.h>
#include <stdarg.h>
#include "root.h"

static char autoContinue = 0;
static bool classProblems = false;
static id classProblemObject = nil;
static SEL classProblemSelector = 0;
static ProblemSeverity classIgnoreLevel = None;;
static char *severityList[] = { "No", "Minor", "Mild", "Bad", "Nasty", "Fatal" };

@implementation Root
-init
/* ===
	Default initialiser.
*/
{
	[super init];
	problemObject = nil;
	problemSelector = 0;
	ignoreLevel = classIgnoreLevel;
	return self;
}

+autoContinue:(char)answer
/* ========================
	Should most probably be passed either 'y' or 'n' :)
	Any non-fatal errors that ask for continuation will automatically
	assume this answer.
*/
{
	autoContinue = tolower( answer );
	return self;
}

+setProblemHandler:anObject :(SEL)aSelector
/* ===============================================
	Sets a default problem handler.
	Probably a bad idea. A better idea would be to modify the [Root problem]
	message.
*/
{
	classProblemObject = anObject;
	classProblemSelector = aSelector;
	return self;
}

+setIgnoreLevel:(ProblemSeverity)severity
/* =======================================
	Sets the global ignore level. 
	This means that problems of a certain severity are ignored totally.
	Probably a bad idea setting it to ignore Fatal errors, but hey,
	whatever lights your candle :).
*/
{
	classIgnoreLevel = severity + 1;
	return self;
}

+problem:(ProblemSeverity)severity :(char*)aMessage, ...
{
char q;
va_list argList;

	if ( severity < classIgnoreLevel ) return self;
	classProblems = true;
	va_start( argList, aMessage );
	if ( classProblemObject != nil && classProblemSelector != 0 ) 
	{
		[classProblemObject perform:classProblemSelector];
		return self;
	}
	[self notice:"%s", [Root composeProblemMessage:severity :aMessage :argList] ];
	if ( severity > None )
	{
		if ( severity >= Nasty )
		{
			if ( severity == Fatal ) exit( 1 );
			if ( autoContinue == 0 )
			{
				[self notice:"continue [y/n]? "];
				fflush( stdout );
				for (;;)
				{
					q = tolower( getchar() );
					if ( q == 'y' || q == 'n' ) break;
				}
			} else
				q = autoContinue;
			if ( q == 'n' ) 
			{
				if ( autoContinue == 0 )
					[self notice:"halting\n" ];
				exit( 1 );
			}
		}
	}
	return self;
}

+notice:(char*)aMessage, ...
{
va_list argList;

	va_start( argList, aMessage );
	printf( "[%s] ", [self name] );
	vprintf( aMessage, argList );
	return self;
}

+(bool)problems
{
bool t = classProblems;

	classProblems = false;
	return t;
}

+(char*)composeProblemMessage:(ProblemSeverity)severity :(char*)aMessage :(va_list)argList
{
static char problemStr[ 512 ];

	vsprintf( problemStr, aMessage, argList );
	sprintf( problemStr + strlen( problemStr ), " (%s problem)\n", severityList[ severity ] );
	return problemStr;
}

-problem:(ProblemSeverity)severity :(char*)aMessage, ...;
/* ==================================================
	Handle the problem. Pass it along to a handler if there is one. 
	Otherwise, print it and perform a default action.
*/
{
char q = 0;
va_list argList;

	if ( severity < ignoreLevel || severity < classIgnoreLevel ) return self;
	va_start( argList, aMessage );
	problems = true;
	if ( classProblemObject != nil && classProblemSelector != 0 ) 
	{
		[classProblemObject perform:classProblemSelector];
		return self;
	}
	if ( problemObject != nil && problemSelector != 0 ) 
	{
		[problemObject perform:problemSelector];
		return self;
	}

	[self notice:"%s", [Root composeProblemMessage:severity :aMessage :argList] ];

	if ( severity > None )
	{
		if ( severity >= Nasty )
		{
			if ( severity == Fatal ) exit( 1 );
			if ( autoContinue == 0 )
			{
				[self notice:"continue [y/n]? " ];
				fflush( stdout );
				for (;;)
				{
					q = tolower( getchar() );
					if ( q == 'y' || q == 'n' ) break;
				}
			} else
				q = autoContinue;
			if ( q == 'n' ) 
			{
				if ( autoContinue == 0 )
					[self notice:"halting\n" ];
				exit( 1 );
			}
		}
	}
	return self;
}

-notice:(char*)aMessage, ...
{
va_list argList;

	va_start( argList, aMessage );
	printf( "[%s] ", [self name] );
	vprintf( aMessage, argList );
	return self;
}

-(bool)problems
{
bool t = problems;

	problems = false;
	return t;
}

-setProblemHandler:anObject :(SEL)aSelector
{
	problemObject = anObject;
	problemSelector = aSelector;
	return self;
}

-setIgnoreLevel:(ProblemSeverity)severity
{
	ignoreLevel = severity + 1;
	return self;
}
@end
