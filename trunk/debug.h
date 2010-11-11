#include <stdio.h>
#include <stdlib.h>

#ifndef stdError

// Permet de mettre entre quot (")
#define QUOT_ME( data ) #data

// NOTE: utiliser fflush(stderr) pour afficher directe les données

// Système d'erreur SANS EXIT
#define stdErrorVar( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, __VA_ARGS__); fflush(stderr);}
#define stdError( msg ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__); fflush(stderr);}

// Système d'erreur AVEC EXIT
#define stdErrorVarE( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, __VA_ARGS__); exit(__LINE__);}
#define stdErrorE( msg ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__); exit(__LINE__);}

#define redirectSdtErr( filename ) freopen(filename, "w", stderr)
#define closeSdtErr( filename ) fclose(stderr)

#endif
