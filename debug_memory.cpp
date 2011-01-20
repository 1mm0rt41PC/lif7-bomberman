#include "debug_memory.h"

// NOTE A MOI M�ME : NE PAS OUBLIER D'INITIALISER CETTE SALTE DE MEMBRE STATIC AINSI !!! ( principe singleton )
CMemoryManager CMemoryManager::c_inst;


/***************************************************************************//*!
* @fn CMemoryManager::CMemoryManager()
* @brief Le constructeur par d�faut.
* @note Initialise uniquement l'ouverture du fichier de rapport
*/
CMemoryManager::CMemoryManager()
{
	if( !(c_fp = fopen("allocations.txt","w")) )
		stdErrorE("Impossible d'ouvrir le fichier <allocations.txt>");
}


/***************************************************************************//*!
* @fn CMemoryManager::~CMemoryManager()
* @brief Le desconstructeur par d�faut
*
* C'est ici que l'on :
*	- d�termine s'il y a une fuite m�moire.
*	- construit la fin de rapport.
*/
CMemoryManager::~CMemoryManager()
{
	if( c_Blocks.empty() ){// Aucune fuite, bravo !
		fprintf(c_fp, "No leak detected, congratulations !\n");
	}else{// Fuites m�moires =(
		fprintf(c_fp, "Oops... Some leaks have been detected\n");
		// Cr�ation du rapport
		// D�tail des fuites
		std::size_t totalSize = 0;
		for( unsigned int i = 0; i != c_Blocks.size(); i++ )
		{
			// Ajout de la taille du bloc au cumul
			totalSize += c_Blocks[i].size;

			switch( c_Blocks[i].type )
			{
				case __classic__: {
					// Inscription dans le fichier des informations sur le bloc courant
					fprintf(c_fp, "Pointeur	-> 0x%08X | %15d octets | %15s | %u\n", (unsigned int)c_Blocks[i].ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
					// Lib�ration de la m�moire
					//free(c_Blocks[i].ptr);
					break;
				}
				case __SDL_Surface__: {
					// Inscription dans le fichier des informations sur le bloc courant
					fprintf(c_fp, "Surface	-> 0x%08X | %15d octets | %15s | %u\n", (unsigned int)c_Blocks[i].ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
					// Lib�ration de la m�moire
					//SDL_FreeSurface(c_Blocks[i].ptr);
					break;
				}
				case __SDL_Font__: {
					// Inscription dans le fichier des informations sur le bloc courant
					fprintf(c_fp, "TTF_Font	-> 0x%08X | %15d octets | %15s | %u\n", (unsigned int)c_Blocks[i].ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
					// Lib�ration de la m�moire
					//TTF_CloseFont(c_Blocks[i].ptr);
					break;
				}
			}
		}

		// Affichage du cumul des fuites
		fprintf(c_fp, "-- %d blocs non-lib�r�(s), %d octets --\n", static_cast<int>(c_Blocks.size()), static_cast<int>(totalSize));
	}
	time_t hold_time = time(NULL);
	fprintf( c_fp, "Report dated: %s\n", ctime(&hold_time));
	if( c_fp )
		fclose(c_fp);
}


/***************************************************************************//*!
* @fn void* CMemoryManager::allocate( size_t size, const char fileName[], unsigned int line, bool is_array )
* @brief R�colte les informations sur les allocations
*
* Permet grace aux surchages de new de compter et d'identifier les allocations
*/
// Ajoute une allocation m�moire
void* CMemoryManager::allocate( size_t size, const char fileName[], unsigned int line, bool is_array )
{
	// Allocation de la m�moire
	void* ptr = malloc(size);

	// Ajout du bloc � la liste des blocs allou�s
	s_AllocatatedBlock newBlock;
	newBlock.ptr		= ptr;
	newBlock.size		= size;
	newBlock.fileName	= fileName;
	newBlock.line		= line;
	newBlock.type		= __classic__;
	newBlock.is_array	= is_array;
	c_Blocks.push_back(newBlock);

	#ifdef MEMORY_FULL_REPORT
		fprintf(c_fp, "++ Allocation				| 0x%08X | %15d octets | <%15s>:%u\n", (unsigned int)ptr, static_cast<int>(size), fileName, line);
	#endif
	return ptr;
}


/***************************************************************************//*!
* @fn void CMemoryManager::Free( void* ptr, bool is_array )
* @brief R�colte les informations sur les d�sallocations
*
* Permet grace aux surchages de delete de compter et d'identifier les d�sallocations.<br />
* ( Retire une allocation m�moire )
*/
void CMemoryManager::Free( void* ptr, bool is_array )
{
	// Recherche de l'adresse dans les blocs allou�s
	unsigned int i=0;
	for( i=0; i<c_Blocks.size(); i++ )
	{
		if( c_Blocks[i].ptr == ptr ) break;
	}

	// Si le bloc n'a pas �t� allou�, on g�n�re une erreur
	if( c_Blocks.size() <= i )
	{
		// En fait �a arrive souvent, du fait que le delete surcharg�
		// est pris en compte m�me l� o� on n'inclue pas debug_memory.h,
		// mais pas la macro pour le new
		// Dans ce cas on d�truit le bloc et on quitte imm�diatement
		free(ptr);
		return ;
	}

	// Si le type d'allocation (tableau / objet) ne correspond pas, on g�n�re une erreur
	if( c_Blocks[i].is_array != is_array )
	{
		if( c_DeleteStack.size() )
			fprintf(c_fp, ">> Probl�me Tableau -- D�sallocation	| 0x%08X | %15d octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_DeleteStack.top().fileName.c_str(), c_DeleteStack.top().line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
		else
			fprintf(c_fp, ">> Probl�me Tableau -- D�sallocation	| 0x%08X | %15d octets | <%15s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
		return ;
	}

	#ifdef MEMORY_FULL_REPORT
		// Finalement, si tout va bien, on supprime le bloc et on loggiz tout �a
		fprintf(c_fp, "-- D�sallocation			| 0x%08X | %15d octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_DeleteStack.top().fileName.c_str(), c_DeleteStack.top().line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
	#endif
	c_Blocks.erase(i+c_Blocks.begin());
	c_DeleteStack.pop();

	// Lib�ration de la m�moire
	free(ptr);
}


/***************************************************************************//*!
* @fn void CMemoryManager::nextDelete( const char fileName[], unsigned int line )
* @brief Sauvegarde les infos sur la d�sallocation courante
*/
void CMemoryManager::nextDelete( const char fileName[], unsigned int line )
{
	s_AllocatatedBlock Delete;
	Delete.fileName = fileName;
	Delete.line = line;

	c_DeleteStack.push(Delete);
}


/***************************************************************************//*!
* @fn CMemoryManager& CMemoryManager::getInstance()
* @brief Renvoie l'intance de la class
*/
// Renvoie l'instance de la classe
CMemoryManager& CMemoryManager::getInstance()
{
	return c_inst;
}


/*******************************************************************************
* Partie SDL
*/
#ifdef __LIB_SDL__
	// On vire les define
	#undef IMG_Load
	#undef SDL_FreeSurface
	#undef TTF_RenderText_Blended
	#undef TTF_OpenFont
	#undef TTF_CloseFont

/***************************************************************************//*!
* @fn SDL_Surface* CMemoryManager::IMG_Load_Debug( const char img, const char fileName[], unsigned int line )
* @brief R�colte les informations sur les allocations des Surfaces SDL
*
* Permet grace aux surchages de compter et d'identifier les allocations
*/
SDL_Surface* CMemoryManager::IMG_Load_Debug( const char fileImg[], const char fileName[], unsigned int line )
{
	// Ajout du bloc � la liste des blocs allou�s
	s_AllocatatedBlock newBlock;
	newBlock.ptr		= (void*)IMG_Load(fileImg);
	newBlock.size		= 0;
	newBlock.fileName	= fileName;
	newBlock.line		= line;
	newBlock.type		= __SDL_Surface__;
	newBlock.is_array	= false;
	c_Blocks.push_back(newBlock);

	#ifdef MEMORY_FULL_REPORT
		fprintf(c_fp, "++ Allocation Surface		| 0x%08X | %15c octets | <%15s>:%u\n", (unsigned int)newBlock.ptr, '?', fileName, line);
	#endif

	return (SDL_Surface*)newBlock.ptr;
}


/***************************************************************************//*!
* @fn void CMemoryManager::SDL_FreeSurface_Debug( SDL_Surface* ptr, const char fileName[], unsigned int line )
* @brief R�colte les informations sur les d�sallocations des Surfaces SDL
*
* Permet grace aux surchages de compter et d'identifier les d�sallocations.<br />
* ( Retire une allocation m�moire )
*/
void CMemoryManager::SDL_FreeSurface_Debug( SDL_Surface* ptr, const char fileName[], unsigned int line )
{
	// Recherche de l'adresse dans les blocs allou�s
	unsigned int i=0;
	for( i=0; i<c_Blocks.size(); i++ )
	{
		if( c_Blocks[i].ptr == (void*)ptr ) break;
	}

	// Si le bloc n'a pas �t� allou�, on g�n�re une erreur
	if( c_Blocks.size() <= i )
	{
		// SDL_FreeSurface a �t� appel� mais la surface qui a �t� g�n�r�e n'est pas dans notre liste !
		// Ce probl�me arrive souvent si la surface g�n�r�e n'est pas dans la liste des fonctions surcharg�es.
		// Dans ce cas on d�truit le bloc et on quitte imm�diatement
		static bool signal = 1;// Permet de signaler le probl�me une seul fois
		if( signal ){
			fprintf(c_fp, "Probl�me de d�sallocation d'une surface (SDL) ! Pointeur(%u) non r�f�renc� <%s>:%u\n", (unsigned int)ptr, fileName, line);
			signal = 0;
		}
		SDL_FreeSurface(ptr);
		return ;
	}

	#ifdef MEMORY_FULL_REPORT
		// Finalement, si tout va bien, on supprime le bloc et on loggiz tout �a
		fprintf(c_fp, "-- D�sallocation			| 0x%08X | %15c octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, '?', fileName, line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
	#endif
	c_Blocks.erase(i+c_Blocks.begin());

	// Lib�ration de la m�moire
	SDL_FreeSurface(ptr);
}


/***************************************************************************//*!
* @fn SDL_Surface* CMemoryManager::TTF_RenderText_Blended_Debug( TTF_Font *font, const char text[], SDL_Color color, const char fileName[], unsigned int line )
* @brief R�colte les informations sur les allocations des Surfaces SDL:TTF
*
* Permet grace aux surchages de compter et d'identifier les allocations
*/
SDL_Surface* CMemoryManager::TTF_RenderText_Blended_Debug( TTF_Font *font, const char text[], SDL_Color color, const char fileName[], unsigned int line )
{
	// Ajout du bloc � la liste des blocs allou�s
	s_AllocatatedBlock newBlock;
	newBlock.ptr		= (void*)TTF_RenderText_Blended(font, text, color);
	newBlock.size		= 0;
	newBlock.fileName	= fileName;
	newBlock.line		= line;
	newBlock.type		= __SDL_Surface__;
	newBlock.is_array	= false;
	c_Blocks.push_back(newBlock);

	#ifdef MEMORY_FULL_REPORT
		fprintf(c_fp, "++ Allocation Surface text	| 0x%08X | %15c octets | <%15s>:%u\n", (unsigned int)newBlock.ptr, '?', fileName, line);
	#endif

	return (SDL_Surface*)newBlock.ptr;
}


/***************************************************************************//*!
* @fn TTF_Font* CMemoryManager::TTF_OpenFont_Debug( const char file[], int size, const char fileName[], unsigned int line )
* @brief R�colte les informations sur les allocations des TTF_Font
*
* Permet grace aux surchages de compter et d'identifier les allocations
*/
TTF_Font* CMemoryManager::TTF_OpenFont_Debug( const char file[], int size, const char fileName[], unsigned int line )
{
	// Ajout du bloc � la liste des blocs allou�s
	s_AllocatatedBlock newBlock;
	newBlock.ptr		= (void*)TTF_OpenFont(file, size);
	newBlock.size		= 0;
	newBlock.fileName	= fileName;
	newBlock.line		= line;
	newBlock.type		= __SDL_Font__;
	newBlock.is_array	= false;
	c_Blocks.push_back(newBlock);

	#ifdef MEMORY_FULL_REPORT
		fprintf(c_fp, "++ Allocation TTF			| 0x%08X | %15c octets | <%15s>:%u\n", (unsigned int)newBlock.ptr, '?', fileName, line);
	#endif

	return (TTF_Font*)newBlock.ptr;
}


/***************************************************************************//*!
* @fn void CMemoryManager::TTF_CloseFont_Debug( TTF_Font* ptr, const char fileName[], unsigned int line )
* @brief R�colte les informations sur les d�sallocations des TTF_Font
*
* Permet grace aux surchages de compter et d'identifier les d�sallocations.<br />
* ( Retire une allocation m�moire )
*/
void CMemoryManager::TTF_CloseFont_Debug( TTF_Font* ptr, const char fileName[], unsigned int line )
{
	// Recherche de l'adresse dans les blocs allou�s
	unsigned int i=0;
	for( i=0; i<c_Blocks.size(); i++ )
	{
		if( c_Blocks[i].ptr == (void*)ptr ) break;
	}

	// Si le bloc n'a pas �t� allou�, on g�n�re une erreur
	if( c_Blocks.size() <= i )
	{
		// TTF_CloseFont a �t� appel� mais la font qui a �t� g�n�r�e n'est pas dans notre liste !
		// Ce probl�me arrive souvent si la font g�n�r�e n'est pas dans la liste des fonctions surcharg�es.
		fprintf(c_fp, "Probl�me de d�sallocation d'un TTF (SDL) ! Pointeur(%u) non r�f�renc� <%s>:%u\n", (unsigned int)ptr, fileName, line);
		TTF_CloseFont(ptr);
		return ;
	}

	#ifdef MEMORY_FULL_REPORT
		// Finalement, si tout va bien, on supprime le bloc et on loggiz tout �a
		fprintf(c_fp, "-- D�sallocation d'un TTF	| 0x%08X | %15c octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, '?', fileName, line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
	#endif
	c_Blocks.erase(i+c_Blocks.begin());

	// Lib�ration de la m�moire
	TTF_CloseFont(ptr);
}
#endif
