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

			// Inscription dans le fichier des informations sur le bloc courant
			fprintf(c_fp, "-> 0x%X | %15d octets | %15s | %u\n", (unsigned int)c_Blocks[i].ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);

			// Lib�ration de la m�moire
			//free(c_Blocks[i].ptr);
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
	newBlock.is_array	= is_array;
	c_Blocks.push_back(newBlock);

	fprintf(c_fp, "++ Allocation		| 0x%X | %15d octets | <%15s>:%u\n", (unsigned int)ptr, static_cast<int>(size), fileName, line);
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
		// est pris en compte m�me l� o� on n'inclue pas DebugNew.h,
		// mais pas la macro pour le new
		// Dans ce cas on d�truit le bloc et on quitte imm�diatement
		free(ptr);
		return ;
	}

	// Si le type d'allocation (tableau / objet) ne correspond pas, on g�n�re une erreur
	if( c_Blocks[i].is_array != is_array )
	{
		if( c_DeleteStack.size() )
			fprintf(c_fp, ">> Probl�me Tableau -- D�sallocation	| 0x%X | %15d octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_DeleteStack.top().fileName.c_str(), c_DeleteStack.top().line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
		else
			fprintf(c_fp, ">> Probl�me Tableau -- D�sallocation	| 0x%X | %15d octets | <%15s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
		return ;
	}

	// Finalement, si tout va bien, on supprime le bloc et on loggiz tout �a
	fprintf(c_fp, "-- D�sallocation	| 0x%X | %15d octets | <%15s>:%u | ORG:<%s>:%u\n", (unsigned int)ptr, static_cast<int>(c_Blocks[i].size), c_DeleteStack.top().fileName.c_str(), c_DeleteStack.top().line, c_Blocks[i].fileName.c_str(), c_Blocks[i].line);
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
