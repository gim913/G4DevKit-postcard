#ifndef std_defines_h
#define std_defines_h

#define mmax(a,b) ((a) > (b) ? (a) : (b))
#define mmin(a,b) ((a) < (b) ? (a) : (b))

#ifndef __syntax_parser__
	#define offsetof(type,member) __offsetof(type,member)
#else
	// This is so the IDE syntax parser parses this correctly,
	// since clang's built in offsetof is __builtin_offsetof
	#define offsetof(type,member) __builtin_offsetof(type,member)
#endif

#define NULL (void*)0L

// This is so we define inline VBCC style assembly functions,
// without the IDE's syntax parser throwing errors.
// Otherwise we would need seperate blocks with #ifdef __syntax_parser__
#ifdef __syntax_parser__
	#define __reg(x)
	#define INLINEASM(str)
#else
	#define INLINEASM(str) =str
#endif

#endif

