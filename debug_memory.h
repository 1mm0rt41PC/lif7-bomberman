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
* @brief Syst�me de v�rification de fuite m�moire.
*
* @warning <b>Cette class n'est pas faites pour �tre utilis�e directement !</b><br />
* Cette class surcharge les operateurs new et delete.<br />
* La class ne peut �tre instanci� qu'une seule fois (class singleton) !<br />
* De plus elle s'instancie sans pointeur => Si une autre class singleton utilise la m�me technologie => BUG
*
* Pour utiliser cette class, mettre le code suivant dans les fichiers <b>.cpp</b>  <span style="text-decoration:underline;">UNIQUEMENT</span> !
* @code
* #define __DEBUG_MEMORY__// Active le debug memoire
* #inlcude "debug_memory.h"
* @endcode
*/
class CMemoryManager
{
	private:
		/*!
		* @struct s_AllocatatedBlock
		* @brief Stock les info sur les block m�moires allou�s
		*
		*/
		typedef struct {
			std::string		fileName;	//!< Fichier contenant l'allocation
			void*			ptr;		//!< Le pointeur
			size_t			size;		//!< Taille allou�e
			unsigned int	line;		//!< Ligne de l'allocation
			bool			is_array;	//!< Est-ce un objet ou un tableau ?
		} s_AllocatatedBlock;
		typedef std::vector<s_AllocatatedBlock> v_AllocatatedBlock;

		v_AllocatatedBlock c_Blocks; //!< Blocs de m�moire allou�s
		std::stack<s_AllocatatedBlock> c_DeleteStack; //!< Pile contenant les infos sur les prochaines d�sallocations
		FILE* c_fp; //!< Pointeur de fichier o� sera stock� le rapport
		static CMemoryManager c_inst;//!< L'instance de la class

	private :
		CMemoryManager();
		~CMemoryManager();

	public :
		// Ajoute une allocation m�moire
		void* allocate( size_t size, const char fileName[], unsigned int line, bool is_array );

		// Retire une allocation m�moire
		void Free( void* ptr, bool is_array );

		// Sauvegarde les infos sur la d�sallocation courante
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