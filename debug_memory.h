#if defined(__DEBUG_MEMORY__) && !defined(DEBUG_MEMORY_h)
#define DEBUG_MEMORY_h

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <string>
#include <vector>
#include <ctime>
#include "debug.h"
#include "config.h"

// A VOIR: http://stackoverflow.com/questions/7194127/how-should-i-write-iso-c-standard-conformant-custom-new-and-delete-operators#7194137
// pour appeler le new classic (std) -> ::new
// pour appeler le delete classic (std) -> ::delete


// Rapport complet ? ( defined or not )
//#define MEMORY_FULL_REPORT


/*!
* @class CMemoryManager
* @brief Système de vérification de fuite mémoire.
*
* @warning <b>Cette class n'est pas faites pour être utilisée directement !</b><br />
* Cette class surcharge les operateurs new et delete.<br />
* La class ne peut être instancié qu'une seule fois (class singleton) !<br />
* De plus elle s'instancie sans pointeur => Si une autre class singleton utilise la même technologie => BUG
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
		* @enum t_obj
		* @brief Type d'objet alloué: Classic ( new / delete ), SDL_... ( Surface, TTF )
		*/
		typedef enum {
			__classic__,
			__SDL_Surface__,
			__SDL_Font__
		} t_obj;

		/*!
		* @struct s_AllocatatedBlock
		* @brief Stock les info sur les block mémoires alloués
		*/
		typedef struct {
			std::string		fileName;	//!< Fichier contenant l'allocation
			void*			ptr;		//!< Le pointeur
			size_t			size;		//!< Taille allouée
			unsigned int	line;		//!< Ligne de l'allocation
			t_obj			type;		//!< Type d'objet (SDL ou classic ?)
			bool			is_array;	//!< Est-ce un objet ou un tableau ?
		} s_AllocatatedBlock;
		typedef std::vector<s_AllocatatedBlock> v_AllocatatedBlock;

		v_AllocatatedBlock c_Blocks; //!< Blocs de mémoire alloués
		std::stack<s_AllocatatedBlock> c_DeleteStack; //!< Pile contenant les infos sur les prochaines désallocations
		FILE* c_fp; //!< Pointeur de fichier où sera stocké le rapport
		static CMemoryManager c_inst;//!< L'instance de la class

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

		#ifdef __LIB_SDL__
		SDL_Surface* IMG_Load_Debug( const char fileImg[], const char fileName[], unsigned int line );
		SDL_Surface* SDL_CreateRGBSurface_Debug( Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask, const char fileName[], unsigned int line );
		#ifdef _SDL_rotozoom_h
			SDL_Surface* rotozoomSurface_Debug( SDL_Surface * src, double angle, double zoom, int smooth, const char fileName[], unsigned int line );
		#endif
		void SDL_FreeSurface_Debug( SDL_Surface* ptr, const char fileName[], unsigned int line );
		SDL_Surface* TTF_RenderText_Blended_Debug( TTF_Font *font, const char text[], SDL_Color color, const char fileName[], unsigned int line );
		TTF_Font* TTF_OpenFont_Debug( const char file[], int size, const char fileName[], unsigned int line );
		void TTF_CloseFont_Debug( TTF_Font* ptr, const char fileName[], unsigned int line );
		#endif
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

/*******************************************************************************
* Partie SDL
*/
#ifdef __LIB_SDL__
	#define IMG_Load( img ) CMemoryManager::getInstance().IMG_Load_Debug(img, __FILE__, __LINE__)
	#define rotozoomSurface( src, angle, zoom, smooth ) CMemoryManager::getInstance().rotozoomSurface_Debug(src, angle, zoom, smooth, __FILE__, __LINE__)
	#define SDL_CreateRGBSurface( flags, width, height, depth, Rmask, Gmask, Bmask, Amask ) CMemoryManager::getInstance().SDL_CreateRGBSurface_Debug(flags, width, height, depth, Rmask, Gmask, Bmask, Amask, __FILE__, __LINE__)
	#define SDL_FreeSurface( surface ) CMemoryManager::getInstance().SDL_FreeSurface_Debug(surface, __FILE__, __LINE__)
	#define TTF_RenderText_Blended( police, texte, couleur ) CMemoryManager::getInstance().TTF_RenderText_Blended_Debug(police, texte, couleur, __FILE__, __LINE__)
	#define TTF_OpenFont( strPolice, taille ) CMemoryManager::getInstance().TTF_OpenFont_Debug(strPolice, taille, __FILE__, __LINE__)
	#define TTF_CloseFont( ptrPolice ) CMemoryManager::getInstance().TTF_CloseFont_Debug(ptrPolice, __FILE__, __LINE__)
#endif

#endif
