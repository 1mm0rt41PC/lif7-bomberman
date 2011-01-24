#ifndef QLIST_h
#define QLIST_h

#ifndef stdError
	#include <stdio.h>
	#include <stdlib.h>

	// Permet de mettre entre quot (")
	#define QUOT_ME( data ) #data

	// NOTE: utiliser fflush(stderr) pour afficher directe les données

	// Système d'erreur SANS EXIT
	#define stdError( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); fflush(stderr);}

	// Système d'erreur AVEC EXIT
	#define stdErrorE( msg, ... ) {fprintf(stderr, "[file " __FILE__ ", line %d]: " msg "\n", __LINE__, ##__VA_ARGS__); exit(__LINE__);}

	#define redirectSdtErr( filename ) freopen(filename, "w", stderr)
	#define closeSdtErr( filename ) fclose(stderr)
#endif

template<class Type>
class QList
{
	private:
		Type* c_list;
		unsigned int c_nb_ojb;

	public:
		QList();
		QList( unsigned int nb, Type& fill );
		~QList();
		inline unsigned int size();
		void resize( unsigned int size, Type& fill );
		inline Type& operator[](unsigned int n);
		Type& at(unsigned int n);
		void operator<<( const Type& newObj );
		void insert( unsigned int at, const Type& newObj );
		void remove( unsigned int at );
		void clear();
};

#include "QList.inl"

#endif
