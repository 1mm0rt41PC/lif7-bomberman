#include "debug_memory.h"

// NOTE A MOI MÊME : NE PAS OUBLIER D'INITIALISER CETTE SALTE DE MEMBRE STATIC AINSI !!! ( principe singleton )
CMemoryManager CMemoryManager::c_inst;


/***************************************************************************//*!
* @fn CMemoryManager::CMemoryManager()
* @brief Le constructeur par défaut.
* @note Initialise uniquement l'ouverture du fichier de rapport
*/
CMemoryManager::CMemoryManager()
{
	if( !(c_fp = fopen("allocations.txt","w")) )
		stdErrorE("Impossible d'ouvrir le fichier <allocations.txt>");
}


/***************************************************************************//*!
* @fn CMemoryManager::~CMemoryManager()
* @brief Le desconstructeur par défaut
*
* C'est ici que l'on :
*	- détermine s'il y a une fuite mémoire.
*	- construit la fin de rapport.
*/
CMemoryManager::~CMemoryManager()
{
	if( c_Blocks.empty() ){// Aucune fuite, bravo !
		fprintf(c_fp, "No leak detected, congratulations !\n");
	}else{// Fuites mémoires =(
		fprintf(c_fp, "Oops... Some leaks have been detected\n");
		// Création du rapport
		// Détail des fuites
		std::size_t totalSize = 0;
		for( unsigned int i = 0; i != c_Blocks.size(); i++ )
		{
			// Ajout de la taille du bloc au cumul
			totalSize += c_Blocks[i].size;

			// Inscription dans le fichier des informations sur le bloc courant
			fprintf(c_fp, "-> 0x%X | %15d octets | %15s | %u\n", (unsigned int)c_Blocks[i].ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);

			// Libération de la mémoire
			//free(c_Blocks[i].ptr);
		}

		// Affichage du cumul des fuites
		fprintf(c_fp, "-- %d blocs non-libéré(s), %d octets --\n", static_cast<int>(c_Blocks.size()), static_cast<int>(totalSize));
	}
	time_t hold_time = time(NULL);
	fprintf( c_fp, "Report dated: %s\n", ctime(&hold_time));
	if( c_fp )
		fclose(c_fp);
}


/***************************************************************************//*!
* @fn void* CMemoryManager::allocate( size_t size, const char fileName[], unsigned int line, bool is_array )
* @brief Récolte les informations sur les allocations
*
* Permet grace aux surchages de new de compter et d'identifier les allocations
*/
// Ajoute une allocation mémoire
void* CMemoryManager::allocate( size_t size, const char fileName[], unsigned int line, bool is_array )
{
	// Allocation de la mémoire
	void* ptr = malloc(size);

	// Ajout du bloc à la liste des blocs alloués
	s_AllocatatedBlock newBlock;
	newBlock.ptr		= ptr;
	newBlock.size		= size;
	newBlock.fileName	= fileName;
	newBlock.line		= line;
	newBlock.is_array	= is_array;
	c_Blocks.push_back(newBlock);

	fprintf(c_fp, "++ Allocation		| 0x%X | %15d octets | <%15s>:%u\n", (unsigned int)ptr, static_cast<int>(size), fileName, line);
	return ptr;
}


/***************************************************************************//*!
* @fn void CMemoryManager::Free( void* ptr, bool is_array )
* @brief Récolte les informations sur les désallocations
*
* Permet grace aux surchages de delete de compter et d'identifier les désallocations.<br />
* ( Retire une allocation mémoire )
*/
void CMemoryManager::Free( void* ptr, bool is_array )
{
	// Recherche de l'adresse dans les blocs alloués
	unsigned int i=0;
	for( i=0; i<c_Blocks.size(); i++ )
	{
		if( c_Blocks[i].ptr == ptr ) break;
	}

	// Si le bloc n'a pas été alloué, on génère une erreur
	if( c_Blocks.size() <= i )
	{
		// En fait ça arrive souvent, du fait que le delete surchargé
		// est pris en compte même là où on n'inclue pas DebugNew.h,
		// mais pas la macro pour le new
		// Dans ce cas on détruit le bloc et on quitte immédiatement
		free(ptr);
		return ;
	}

	// Si le type d'allocation (tableau / objet) ne correspond pas, on génère une erreur
	if( c_Blocks[i].is_array != is_array )
	{
		if( c_DeleteStack.size() )
			fprintf(c_fp, ">> Problème Tableau -- Désallocation	| 0x%X | %15d octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_DeleteStack.top().fileName.c_str(), c_DeleteStack.top().line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
		else
			fprintf(c_fp, ">> Problème Tableau -- Désallocation	| 0x%X | %15d octets | <%15s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
		return ;
	}

	// Finalement, si tout va bien, on supprime le bloc et on loggiz tout ça
	fprintf(c_fp, "-- Désallocation	| 0x%X | %15d octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_DeleteStack.top().fileName.c_str(), c_DeleteStack.top().line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
	c_Blocks.erase(i+c_Blocks.begin());
	c_DeleteStack.pop();

	// Libération de la mémoire
	free(ptr);
}


/***************************************************************************//*!
* @fn void CMemoryManager::nextDelete( const char fileName[], unsigned int line )
* @brief Sauvegarde les infos sur la désallocation courante
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
