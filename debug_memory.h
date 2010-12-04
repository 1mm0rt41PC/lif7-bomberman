#if defined(__DEBUG_MEMORY__) && !defined(DEBUG_MEMORY_h)
#define DEBUG_MEMORY_h

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <string>
#include <vector>
#include <ctime>
#include "debug.h"


/*!
* @class CMemoryManager
* @brief Système de vérification de fuite mémoire.
*
* @warning Cette class surcharge les operateurs new et delete
*
* Pour utiliser cette class, mettre le code suivant dans les fichiers <b>.cpp</b> <em>UNIQUEMENT</em> !
* @code
* #define __DEBUG_MEMORY__// Active le debug memoire
* #inlcude "debug_memory.h"
* @endcode
*/
class CMemoryManager
{
	private:
		typedef struct {
			std::string		fileName;	// Fichier contenant l'allocation
			void*			ptr;		// Le pointeur
			size_t			size;		// Taille allouée
			unsigned int	line;		// Ligne de l'allocation
			bool			is_array;	// Est-ce un objet ou un tableau ?
		} s_AllocatatedBlock;
		typedef std::vector<s_AllocatatedBlock> v_AllocatatedBlock;

		v_AllocatatedBlock c_Blocks; // Blocs de mémoire alloués
		std::stack<s_AllocatatedBlock> c_DeleteStack; // Pile contenant les infos sur les prochaines désallocations
		FILE* c_fp;
		static CMemoryManager c_inst;

	private :
		CMemoryManager();
		~CMemoryManager();

	public :
		// Ajoute une allocation mémoire
		void* allocate( size_t size, const char fileName[], unsigned int line, bool is_array );

		// Retire une allocation mémoire
		void Free( void* ptr, bool is_array );

		// Sauvegarde les infos sur la désallocation courante
		void nextDelete( const char fileName[], unsigned int line );

		// Renvoie l'instance de la classe
		static CMemoryManager& getInstance();
};

inline void* operator new ( size_t taille, const char fileName[], unsigned int line )
{
	return CMemoryManager::getInstance().allocate( taille , fileName, line, false );
}
inline void* operator new[] ( size_t taille, const char fileName[], unsigned int line )
{
	return CMemoryManager::getInstance().allocate( taille , fileName, line, true );
}
inline void operator delete( void* ptr )
{
	CMemoryManager::getInstance().Free(ptr, false);
}
inline void operator delete[]( void* ptr )
{
	CMemoryManager::getInstance().Free(ptr, true);
}
inline void operator delete( void* ptr, const char fileName[], unsigned int line )
{
	CMemoryManager::getInstance().nextDelete(fileName, line);
	CMemoryManager::getInstance().Free(ptr, false);
}
inline void operator delete[]( void* ptr, const char fileName[], unsigned int line )
{
	CMemoryManager::getInstance().nextDelete(fileName, line);
	CMemoryManager::getInstance().Free(ptr, true);
}
#define new new(__FILE__, __LINE__)
#define delete CMemoryManager::getInstance().nextDelete(__FILE__, __LINE__), delete

#endif
