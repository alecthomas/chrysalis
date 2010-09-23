#ifndef __ROOT_H__
#define __ROOT_H__

#include <objc/Object.h>
#include <stdarg.h>

typedef enum {
	false,
	true
} bool;

#ifdef __DEBUG__
	#define PRINT( format, args... ) printf( "(%s) " format "\n", __PRETTY_FUNCTION__ , ## args )
	#define DEBUG( format, args... ) printf( "(function %s, file %s, line %i) "format"\n", __FUNCTION__, __FILE__, __LINE__ , ## args )
#else
	#define PRINT( format, args... )
	#define DEBUG( format, args... )
#endif

typedef enum {
	None,
	Minor, 
	Mild, 
	Bad, 
	Nasty, 
	Fatal 
} ProblemSeverity;

@protocol ProblemProtocol
+autoContinue:(char)answer;
+setProblemHandler:anObject :(SEL)aSelector;
+setIgnoreLevel:(ProblemSeverity)severity;
+problem:(ProblemSeverity)severity :(char*)aMessage, ...;
+notice:(char*)aMessage, ...;
+(bool)problems;
+(char*)composeProblemMessage:(ProblemSeverity)severity :(char*)aMessage :(va_list)argList;
-problem:(ProblemSeverity)severity :(char*)aMessage, ...;
-notice:(char*)aMessage, ...;
-(bool)problems;
-setProblemHandler:anObject :(SEL)aSelector;
-setIgnoreLevel:(ProblemSeverity)severity;
@end

@interface Root : Object <ProblemProtocol>
{
	id problemObject;
	SEL problemSelector;
	ProblemSeverity ignoreLevel;
	bool problems;
}

-init;

+autoContinue:(char)answer;
		// Set auto-continuation on non-critical problems

+setProblemHandler:anObject :(SEL)aSelector;
		// Set the default problem handler/selector. Might be better if 
		// -problem was overloaded 

+setIgnoreLevel:(ProblemSeverity)severity;
		// Set the default ignore level

+problem:(ProblemSeverity)severity :(char*)aMessage, ...;
		// See -problem for description

+(bool)problems;
		// See -problems for description

-problem:(ProblemSeverity)severity :(char*)aMessage, ...;
		//	Report a problem. Will perform certain actions, depending on the
		//	severity.
		//	Default actions are:
		//		Severity   Action
		//		---------------------------------------------------
		//		None-Bad	Display and continue
		//		Nasty		Display and confirm before continuation
		//		Fatal		Display and terminate
		//

-(bool)problems;
		// Return true if any problems occured recently. Also resets problem
		// flag.
+(char*)composeProblemMessage:(ProblemSeverity)severity :(char*)aMessage :(va_list)argList;
		// Display the problem. Called internally by -problem

-setProblemHandler:anObject :(SEL)aSelector;
		// Set a problem handler for this object alone
-setIgnoreLevel:(ProblemSeverity)severity;
@end
#endif
