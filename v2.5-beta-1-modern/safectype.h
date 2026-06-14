// safectype.h - make the <ctype.h> classification/conversion functions safe to
// call with raw (signed) char arguments.
//
// Legacy Comic Chat code calls isspace()/isdigit()/isalpha()/tolower()/... with
// plain `char` values all over the place. On a signed-char compiler any byte
// >= 0x80 (extended ASCII, UTF-8/DBCS lead bytes, etc.) becomes a NEGATIVE int,
// which is undefined behaviour for these functions and trips the modern debug
// CRT assertion "c >= -1 && c <= 255" (ucrt isctype.cpp). Wrapping each call so
// the argument is passed as unsigned char fixes the entire class of crash at
// once, without editing thousands of call sites.
//
// The wrappers are function-like macros; the C preprocessor does not re-expand a
// macro inside its own replacement list, so each one resolves to the real CRT
// function applied to (unsigned char)(c).

#ifndef _SAFECTYPE_H_
#define _SAFECTYPE_H_

#include <ctype.h>		// pull in the real declarations BEFORE we define macros

#define isalnum(c)   (isalnum)((unsigned char)(c))
#define isalpha(c)   (isalpha)((unsigned char)(c))
#define iscntrl(c)   (iscntrl)((unsigned char)(c))
#define isdigit(c)   (isdigit)((unsigned char)(c))
#define isgraph(c)   (isgraph)((unsigned char)(c))
#define islower(c)   (islower)((unsigned char)(c))
#define isprint(c)   (isprint)((unsigned char)(c))
#define ispunct(c)   (ispunct)((unsigned char)(c))
#define isspace(c)   (isspace)((unsigned char)(c))
#define isupper(c)   (isupper)((unsigned char)(c))
#define isxdigit(c)  (isxdigit)((unsigned char)(c))
#define tolower(c)   (tolower)((unsigned char)(c))
#define toupper(c)   (toupper)((unsigned char)(c))

#endif // _SAFECTYPE_H_
