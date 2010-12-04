#include <stdio.h>
#include <stdlib.h>

#ifndef stdError

	// Permet de mettre entre quot (")
	#define QUOT_ME( data ) #data

	// NOTE: utiliser fflush(stderr) pour afficher directe les donn�es

	// Syst�me d'erreur SANS EXIT
	#define stdError( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); fflush(stderr);}

	// Syst�me d'erreur AVEC EXIT
	#define stdErrorE( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); exit(__LINE__);}

	#define redirectSdtErr( filename ) freopen(filename, "w", stderr)
	#define closeSdtErr( filename ) fclose(stderr)

	// Debug M�moire
	#include "debug_memory.h"
#endif
