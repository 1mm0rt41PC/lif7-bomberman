#ifndef OUTILS_h
#define OUTILS_h

#include "config.h"
#include <stdlib.h>// rand
#include <string.h>
#include <ctype.h>

#if defined(OS_WINDOWS)
	#include <time.h>
	#include <sys/timeb.h>
	#ifndef _TIMEVAL_DEFINED
	#define _TIMEVAL_DEFINED
		struct timeval {
			long int    tv_sec;		// Secondes
			long int    tv_usec;		// Microsecondes
		};
	#endif
	int gettimeofday( struct timeval* tp, void* tz );
#else
	#include <sys/time.h>
#endif
typedef struct timeval s_timeval;

int getSeconde();
int myRand( int a, int b );
unsigned char getTailleNombre( int nb );
unsigned char getTailleNombre( unsigned int nb );
char* trimString( char texte[] );


#include "outils.inl"
#endif
