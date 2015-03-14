#include "bonus.h"
#include "debug.h"

// NOTE A MOI M�ME : NE PAS OUBLIER D'INITIALISER CETTE SALTE DE MEMBRE STATIC AINSI !!! ( principe singleton )
bonus::s_bonus_proprieter *bonus::C_bonusProp = NULL;

/***************************************************************************//*!
* @fn bonus::bonus()
* @brief Initialise la class bonus ( mais ne cr�er et ne charge aucun bonus en m�moire )
*
* Si C_bonusProp non initialiser auparavant, alors C_bonusProp sera initialiser
*/
bonus::bonus()
{
	c_listEvent.clear();

	if( !C_bonusProp )
		stdErrorE("Veuillez initialiser la partie g�n�ral des bonus avant de quoi que se soit ! -> bonus::bonusProp()");
}


/***************************************************************************//*!
* @fn bonus::~bonus()
* @brief D�sinitialise la class bonus et supprime tous les bonus qui ont pu �tre charg�s
*/
bonus::~bonus()
{
	c_listEvent.clear();
}


/***************************************************************************//*!
* @fn bonus::s_bonus_proprieter* bonus::bonusProp()
* @brief  Renvoie le pointeur des bonus g�n�raux
* @return un pointeur vers les bonus g�n�raux
*
* Si non initialis� alors on initalise les bonus utilisables pour la partie en cours<br />
* Les valeurs probabiliter_pop et quantite_MAX_Ramassable peuvent �tre modifi�es<br />
* Via les fonction defQuantiteMAX_Ramassable() et defProbabiliter()<br />
* <br />
* A utiliser au minimum debut de chaque partie !<br />
*/
bonus::s_bonus_proprieter* bonus::bonusProp()
{
	if( C_bonusProp )// Si d�j� initialis� => on retourne le tableau
		return C_bonusProp;

	C_bonusProp = new s_bonus_proprieter[NB_ELEMENT_t_Bonus];

	C_bonusProp[bombe].probabiliter_pop = 60;// 60% de pop
	C_bonusProp[bombe].quantite_MAX_Ramassable = 20;
	// 1.5secs avant explosion
	C_bonusProp[bombe].duree.tv_sec = 1;// 1.5secs avant explosion
	C_bonusProp[bombe].duree.tv_usec = 500000;

	C_bonusProp[super_bombe].probabiliter_pop = 5;// 5% de pop
	C_bonusProp[super_bombe].duree.tv_sec = 1;// 1.5secs avant explosion
	C_bonusProp[super_bombe].duree.tv_usec = 500000;

	C_bonusProp[puissance_flamme].probabiliter_pop = 80;// 80% de pop
	C_bonusProp[puissance_flamme].quantite_MAX_Ramassable = 15;
	C_bonusProp[puissance_flamme].duree.tv_sec = -1;
	C_bonusProp[puissance_flamme].duree.tv_usec = -1;

	C_bonusProp[super_puissance_flamme].probabiliter_pop = 10;// 10% de pop
	C_bonusProp[super_puissance_flamme].quantite_MAX_Ramassable = 1;
	C_bonusProp[super_puissance_flamme].duree.tv_sec = -1;
	C_bonusProp[super_puissance_flamme].duree.tv_usec = -1;

	C_bonusProp[declancheur].probabiliter_pop = 30;// 30% de pop
	C_bonusProp[declancheur].quantite_MAX_Ramassable = 1;
	C_bonusProp[declancheur].duree.tv_sec = -1;
	C_bonusProp[declancheur].duree.tv_usec = -1;

	C_bonusProp[pousse_bombe].probabiliter_pop = 30;// 30% de pop
	C_bonusProp[pousse_bombe].quantite_MAX_Ramassable = 1;
	C_bonusProp[pousse_bombe].duree.tv_sec = -1;
	C_bonusProp[pousse_bombe].duree.tv_usec = -1;

	C_bonusProp[vie].probabiliter_pop = 5;// 5% de pop
	C_bonusProp[vie].quantite_MAX_Ramassable = 10;
	C_bonusProp[vie].duree.tv_sec = -1;
	C_bonusProp[vie].duree.tv_usec = -1;

	C_bonusProp[teleporteur].probabiliter_pop = 30;// 30% de pop
	C_bonusProp[teleporteur].quantite_MAX_Ramassable = 1;// Au moins 1, sinan pas pop
	C_bonusProp[teleporteur].duree.tv_sec = -1;
	C_bonusProp[teleporteur].duree.tv_usec = -1;

	C_bonusProp[inversseur_touche].probabiliter_pop = 25;// 25% de pop
	C_bonusProp[inversseur_touche].quantite_MAX_Ramassable = 1;
	// 10 sec d'inverstion de touche
	C_bonusProp[force_explosion].duree.tv_sec = 10;
	C_bonusProp[force_explosion].duree.tv_usec = 0;

	C_bonusProp[force_explosion].probabiliter_pop = 20;// 30% de pop
	C_bonusProp[force_explosion].quantite_MAX_Ramassable = 1;// Au moins 1, sinan pas pop
	C_bonusProp[force_explosion].duree.tv_sec = -1;
	C_bonusProp[force_explosion].duree.tv_usec = -1;

	return C_bonusProp;
}


/***************************************************************************//*!
* @fn static void bonus::unInitBonusProp()
* @brief D�truit les bonus qui �taient utilisables durant la partie.
*
* <b>A utiliser � la fin de chaque partie !</b>
*/
void bonus::unInitBonusProp()
{
	if( !C_bonusProp )// Si PAS encore initialis� => out
		stdErrorE("C_bonusProp PAS encore initialis�.");

	delete[] C_bonusProp;
	C_bonusProp = 0;// ATTENTION delete[] ne met rien a 0
}


/***************************************************************************//*!
* @fn void bonus::param_Par_Defaut()
* @brief Charge en m�moire les param�tres par defaut pour une partie F4A
*
* Charge les param�tres par defaut pour une partie F4A et supprime les anciens<br />
* param�tres si besoin.
*/
void bonus::param_Par_Defaut()// Pour une partie classique ( F4A )
{
	c_liste[bombe].quantite_utilisable = 1;// 1 Bombe pour commencer
	c_liste[bombe].quantite_MAX_en_stock = 1;// 1 Bombe pour commencer

	c_liste[super_bombe].quantite_utilisable = 0;
	c_liste[super_bombe].quantite_MAX_en_stock = 0;

	c_liste[declancheur].quantite_utilisable = 0;// 0 D�clancheur pour commencer
	c_liste[declancheur].quantite_MAX_en_stock = 0;// 0 D�clancheur pour commencer

	c_liste[pousse_bombe].quantite_utilisable = 0;// 0 Pousse bombe pour commencer
	c_liste[pousse_bombe].quantite_MAX_en_stock = 0;// 0 Pousse bombe pour commencer

	c_liste[puissance_flamme].quantite_utilisable = 1;
	c_liste[puissance_flamme].quantite_MAX_en_stock = 1;

	c_liste[super_puissance_flamme].quantite_utilisable = 0;
	c_liste[super_puissance_flamme].quantite_MAX_en_stock = 0;

	c_liste[vie].quantite_utilisable = 0;
	c_liste[vie].quantite_MAX_en_stock = 0;

	c_liste[teleporteur].quantite_utilisable = 0;
	c_liste[teleporteur].quantite_MAX_en_stock = 0;

	c_liste[inversseur_touche].quantite_utilisable = 0;
	c_liste[inversseur_touche].quantite_MAX_en_stock = 0;

	c_liste[force_explosion].quantite_utilisable = 0;
	c_liste[force_explosion].quantite_MAX_en_stock = 0;
}


/***************************************************************************//*!
* @fn unsigned char bonus::quantiteUtilisable( t_Bonus b ) const
* @brief Renvoie la quantit� utilisatble d'un bonus
*/
unsigned char bonus::quantiteUtilisable( t_Bonus b ) const
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return 0;
	}

	return c_liste[b].quantite_utilisable;
}


/***************************************************************************//*!
* @fn unsigned char bonus::quantiteMAX( t_Bonus b ) const
* @brief Renvoie la quantit� MAX que le joueur a rammass� d'un bonus
*
* ie: 5 bombes sur 8 sur 20 -> On a 5 bombes que l'on peut encore pos�<br />
* et on en a pos� 8-5=3, de plus on peut ramasser un maximum de 20-8=12 bombes<br />
* supl�mentaire.
*/
unsigned char bonus::quantiteMAX( t_Bonus b ) const
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return 0;
	}

	return c_liste[b].quantite_MAX_en_stock;
}


/***************************************************************************//*!
* @fn unsigned char bonus::quantiteMAX_Ramassable( t_Bonus b ) const
* @brief Renvoie la quantit� Max que l'on peut ramasser d'un objet
*/
unsigned char bonus::quantiteMAX_Ramassable( t_Bonus b ) const
{
	return C_bonusProp[b].quantite_MAX_Ramassable;
}


/***************************************************************************//*!
* @fn unsigned char bonus::probabiliter( t_Bonus b ) const
* @brief Renvoie la probabilit� qu'un bonus pop
*/
unsigned char bonus::probabiliter( t_Bonus b ) const
{
	if( C_bonusProp[b].quantite_MAX_Ramassable == 0 )
		return 0;

	return C_bonusProp[b].probabiliter_pop;
}


/***************************************************************************//*!
* @fn bool bonus::incQuantiteUtilisable( t_Bonus b )
* @brief Incr�mente quantite_utilisable
* @return Renvoie vrais si on a pu incr�menter la valeur
*/
bool bonus::incQuantiteUtilisable( t_Bonus b )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return false;
	}

	if( c_liste[b].quantite_utilisable+1 <= c_liste[b].quantite_MAX_en_stock ){
		c_liste[b].quantite_utilisable++;
		return true;
	}
	return false;
}


/***************************************************************************//*!
* @fn bool bonus::incQuantiteUtilisable_Event( t_Bonus b )
* @brief Incr�mente quantite_utilisable et active l'event
* @param[in] b Le bonus que l'on veut modifier
* @return Renvoie vrais si on a pu incr�menter la valeur
*
* Cette fonction permet d'activer le d�clancheur d'event du bonus
* @warning Le d�clancheur d'event va ajouter +1 a ce bonus au bout de C_bonusProp[b].duree.<br />C'est a vous de traiter l'objet
*/
bool bonus::incQuantiteUtilisable_Event( t_Bonus b )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return false;
	}

	if( c_liste[b].quantite_utilisable+1 <= c_liste[b].quantite_MAX_en_stock ){
		c_liste[b].quantite_utilisable++;
		// Cr�ation de l'event
		s_Event e;
		e.type = b;

		s_timeval now;
		gettimeofday(&now, 0);
		e.finEvent.tv_sec = now.tv_sec + C_bonusProp[b].duree.tv_sec;// Ajustement du temps
		e.finEvent.tv_usec = now.tv_usec + C_bonusProp[b].duree.tv_usec;// Ajustement du temps
		if( e.finEvent.tv_usec > 999999 ){
			e.finEvent.tv_sec += e.finEvent.tv_usec/1000000;
			e.finEvent.tv_usec %= 1000000;
		}

		c_listEvent.push_back( e );// On ajout l'event � la liste des event
		return true;
	}
	return false;
}


/***************************************************************************//*!
* @fn bool bonus::incQuantiteMAX_en_stock( t_Bonus b )
* @brief Incr�mente quantite_MAX_en_stock et quantite_utilisable
* @return Renvoie vrais si on a pu incr�menter la valeur
*/
bool bonus::incQuantiteMAX_en_stock( t_Bonus b )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return false;
	}

	if( c_liste[b].quantite_MAX_en_stock+1 <= C_bonusProp[b].quantite_MAX_Ramassable ){
		c_liste[b].quantite_utilisable++;
		c_liste[b].quantite_MAX_en_stock++;
		return true;
	}
	return false;
}


/***************************************************************************//*!
* @fn bool bonus::incQuantiteMAX_en_stock_Event( t_Bonus b )
* @brief Incr�mente quantite_MAX_en_stock et quantite_utilisable puis active l'event
* @param[in] b Le bonus que l'on veut modifier
* @return Renvoie vrais si on a pu incr�menter la valeur
*
* Cette fonction permet d'activer le d�clancheur d'event du bonus
* @warning Le d�clancheur d'event va ajouter +1 a ce bonus au bout de C_bonusProp[b].duree.<br />C'est a vous de traiter l'objet
*/
bool bonus::incQuantiteMAX_en_stock_Event( t_Bonus b )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return false;
	}

	if( c_liste[b].quantite_MAX_en_stock+1 <= C_bonusProp[b].quantite_MAX_Ramassable ){
		c_liste[b].quantite_utilisable++;
		c_liste[b].quantite_MAX_en_stock++;

		// Cr�ation de l'event
		s_Event e;
		e.type = b;

		s_timeval now;
		gettimeofday(&now, 0);
		e.finEvent.tv_sec = now.tv_sec + C_bonusProp[b].duree.tv_sec;// Ajustement du temps
		e.finEvent.tv_usec = now.tv_usec + C_bonusProp[b].duree.tv_usec;// Ajustement du temps
		if( e.finEvent.tv_usec > 999999 ){
			e.finEvent.tv_sec += e.finEvent.tv_usec/1000000;
			e.finEvent.tv_usec %= 1000000;
		}

		c_listEvent.push_back( e );// On ajout l'event � la liste des event
		return true;
	}
	return false;
}


/***************************************************************************//*!
* @fn bool bonus::decQuantiteUtilisable( t_Bonus b )
* @brief D�cr�mente quantite_utilisable
* @return Renvoie vrais si on a pu d�cr�ment� la valeur
*/
bool bonus::decQuantiteUtilisable( t_Bonus b )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return false;
	}

	if( c_liste[b].quantite_utilisable-1 >= 0 && (unsigned char)(c_liste[b].quantite_utilisable-1) <= c_liste[b].quantite_MAX_en_stock ){
		c_liste[b].quantite_utilisable--;
		return true;
	}
	return false;
}


/***************************************************************************//*!
* @fn bool bonus::decQuantiteUtilisable( t_Bonus b, unsigned int x, unsigned int y )
* @brief D�cr�mente quantite_utilisable
* @param[in] b Le bonus que l'on veut modifier
* @param[in] x Position X du bonus
* @param[in] y Position Y du bonus
* @return Renvoie vrais si on a pu d�cr�ment� la valeur
*
* Cette fonction permet d'activer le d�clancheur d'event du bonus
*/
bool bonus::decQuantiteUtilisable( t_Bonus b, unsigned int x, unsigned int y )
{
	if( C_bonusProp[b].duree.tv_sec < 0 || C_bonusProp[b].duree.tv_usec < 0 ){
		stdError("Le temps du Bonus ne lui permet pas d'avoir un Event. bonus=%d, C_bonusProp[b].duree={%ld,%ld}", (int)b, C_bonusProp[b].duree.tv_sec, C_bonusProp[b].duree.tv_usec);
		return decQuantiteUtilisable( b );
	}

	if( !decQuantiteUtilisable( b ) )
		return false;

	// Cr�ation de l'event
	s_Event e;
	e.type = b;
	e.pos.x = x;
	e.pos.y = y;

	s_timeval now;
	gettimeofday(&now, 0);
	e.finEvent.tv_sec = now.tv_sec + C_bonusProp[b].duree.tv_sec;// Ajustement du temps
	e.finEvent.tv_usec = now.tv_usec + C_bonusProp[b].duree.tv_usec;// Ajustement du temps
	if( e.finEvent.tv_usec > 999999 ){
		e.finEvent.tv_sec += e.finEvent.tv_usec/1000000;
		e.finEvent.tv_usec %= 1000000;
	}

	c_listEvent.push_back( e );// On ajout l'event � la liste des event

	return true;
}


/***************************************************************************//*!
* @fn bool bonus::decQuantiteMAX_en_stock( t_Bonus b )
* @brief D�cr�mente quantite_MAX_en_stock
* @return Renvoie vrais si on a pu d�cr�ment� la valeur
*/
bool bonus::decQuantiteMAX_en_stock( t_Bonus b )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return false;
	}

	if( c_liste[b].quantite_MAX_en_stock-1 >=0 && (unsigned char)(c_liste[b].quantite_MAX_en_stock-1) <= C_bonusProp[b].quantite_MAX_Ramassable ){
		c_liste[b].quantite_MAX_en_stock--;
		if( c_liste[b].quantite_utilisable > c_liste[b].quantite_MAX_en_stock ){
			c_liste[b].quantite_utilisable--;
		}
		return true;
	}
	return false;
}


/***************************************************************************//*!
* @fn void bonus::defQuantiteUtilisable( t_Bonus b, unsigned char quantite_utilisable )
* @brief Modifie la quantit� utilisable. (quantite_utilisable)
*
* Si quantite_utilisable > quantite_MAX_en_stock => quantite_utilisable = quantite_MAX_en_stock<br />
* <b>/!\\quantite_MAX_Ramassable >= quantite_utilisable/!\\</b>
*/
void bonus::defQuantiteUtilisable( t_Bonus b, unsigned char quantite_utilisable )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus || C_bonusProp[b].quantite_MAX_Ramassable < quantite_utilisable ){
		stdError("Bonus non trouv� ou erreur > bonus=%d, C_bonusProp[%d].quantite_MAX_Ramassable=%d, quantite_utilisable=%d", b, b, (int)C_bonusProp[b].quantite_MAX_Ramassable, (int)quantite_utilisable);
		return ;
	}

	c_liste[b].quantite_utilisable = quantite_utilisable;

	if( quantite_utilisable > c_liste[b].quantite_MAX_en_stock ){
		c_liste[b].quantite_MAX_en_stock = quantite_utilisable;
	}
}


/***************************************************************************//*!
* @fn void bonus::defQuantiteMAX( t_Bonus b, unsigned char quantite_MAX_en_stock )
* @brief Modifie la quantit� MAX en stock. (quantite_MAX_en_stock)
*
* <b>/!\\quantite_MAX_Ramassable >= quantite_MAX_en_stock/!\\</b>
* @see @ref aide
*/
void bonus::defQuantiteMAX( t_Bonus b, unsigned char quantite_MAX_en_stock )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus || C_bonusProp[b].quantite_MAX_Ramassable < quantite_MAX_en_stock ){
		stdError("Bonus non trouv� ou erreur > bonus=%d, C_bonusProp[%d].quantite_MAX_Ramassable=%d, quantite_MAX_en_stock=%d", b, b, (int)C_bonusProp[b].quantite_MAX_Ramassable, (int)quantite_MAX_en_stock);
		return ;
	}

	c_liste[b].quantite_MAX_en_stock = quantite_MAX_en_stock;
}


/***************************************************************************//*!
* @fn void bonus::defQuantiteMAX_Ramassable( t_Bonus b, unsigned char quantite_MAX_Ramassable )
* @brief Modifie la quantit� MAX ramassable. (quantite_MAX_en_stock)
* @see @link bonus::defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop ) bonus::defBonus()@endlink
*/
void bonus::defQuantiteMAX_Ramassable( t_Bonus b, unsigned char quantite_MAX_Ramassable )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus )
		stdErrorE("Bonus non trouv� ou erreur > bonus=%d", b);

	C_bonusProp[b].quantite_MAX_Ramassable = quantite_MAX_Ramassable;
}


/***************************************************************************//*!
* @fn void bonus::defProbabiliter( t_Bonus b, unsigned char probabiliter_pop )
* @brief Modifie la probabilit� d'un bonus de pop
* @param[in] b					Le bonus dont on veut definir la probabilit�
* @param[in] probabiliter_pop	La probabilit� de pop apr�s une destruction de case (valeur: entre [0 et 100 au max])
* @see @link bonus::defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop ) bonus::defBonus()@endlink
*/
void bonus::defProbabiliter( t_Bonus b, unsigned char probabiliter_pop )
{
	if( bombe > b || b >= NB_ELEMENT_t_Bonus )
		stdErrorE("Bonus non trouv� ou erreur > bonus=%d", b);

	if( probabiliter_pop > 100 )
		stdErrorE("probabiliter_pop doit avoir une valeur comprise entre [0 et 100] ! bonus(b)=%d, probabiliter_pop=%d", (int)b, (int)probabiliter_pop);

	C_bonusProp[b].probabiliter_pop = probabiliter_pop;
}


/***************************************************************************//*!
* @fn void bonus::defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock )
* @brief Modifie un bonus
* @see @link bonus::defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop ) bonus::defBonus()@endlink
*/
void bonus::defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock )
{
	// On fait les v�rifications ad�quate
	if( quantite_utilisable > quantite_MAX_en_stock || C_bonusProp[b].quantite_MAX_Ramassable < quantite_MAX_en_stock )
		stdErrorE("Les donn�es ont des valeurs incorrectes : bonus::defBonus( %d, %u, %u ) et C_bonusProp[b].quantite_MAX_Ramassable = %u", (int)b, (unsigned int)quantite_utilisable, (unsigned int)quantite_MAX_en_stock, (unsigned int)C_bonusProp[b].quantite_MAX_Ramassable);

	if( bombe > b || b >= NB_ELEMENT_t_Bonus ){// Ajout d'un nouveau bonus
		stdError("Bonus non trouv� ou erreur > bonus=%d", b);
		return ;
	}

	// Modification du bonus ( Relatif a l'instance en cours )
	c_liste[b].quantite_utilisable = quantite_utilisable;
	c_liste[b].quantite_MAX_en_stock = quantite_MAX_en_stock;
}


/***************************************************************************//*!
* @fn void bonus::defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop )
* @brief Modifie (ou cr�er un bonus si celui-ci n'est pas encore en m�moire)
* @param[in] b							Le bonus dont on veut definir la probabilit�
* @param[in] quantite_utilisable		La quantit� que l'on a actuellement et qui est utilisable. (J'ai 3 bombes au maximum mais 1 est pos�e donc il m'en reste 2 utilisables (quantite_utilisable))
* @param[in] quantite_MAX_en_stock		La quantit� que le joueur peux utiliser. (J'ai 3 bombes au maximum (quantite_MAX_en_stock) mais 1 est pos�e donc il m'en reste 2 utilisables)
* @param[in] quantite_MAX_Ramassable	(G�n�ral) La quantit� que le joueur peux au maximum ramasser. (Je peux mettre 5 bombes dans mon sac � bombes)
* @param[in] probabiliter_pop			(G�n�ral) La probabilit� de pop apr�s une destruction de case (valeur: entre [0 et 100 au max])
* (G�n�ral): Propri�t�s communes � toutes les instances.
*
* Exemple:<br />
*	- Soit un perso bomberman.
*	- Le perso est capable de porter 10 bombes s'il y a le sac ad�quate. ( quantite_MAX_Ramassable = 10 )
*	- Notre perso a un sac pouvant contenir 5 bombes maximum. ( quantite_MAX_en_stock = 5 )
*	- Le sac contient actuellement 3 bombes.( quantite_utilisable = 3)
*	- J'ai donc 2 bombes pos�es.
*/
void bonus::defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop )
{
	if( probabiliter_pop > 100 )
		stdErrorE("probabiliter_pop doit avoir une valeur comprise entre [0 et 100] ! bonus(b)=%d, probabiliter_pop=%d", (int)b, (int)probabiliter_pop);

	// Modification du bonus ( Relatif a TOUTES les instances )
	C_bonusProp[b].probabiliter_pop = probabiliter_pop;
	C_bonusProp[b].quantite_MAX_Ramassable = quantite_MAX_Ramassable;

	// PUIS modification du bonus
	defBonus( b, quantite_utilisable, quantite_MAX_en_stock );
}


/***************************************************************************//*!
* @fn bool bonus::isEvent( s_Event* pos )
* @brief V�rifit que le si le temps est �coul� pour une bombe pos�
* @param[in,out] pos	Cette variables contiendra la position X,Y de la bombe qui a explos�.
* @return Renvoie le pointeur pos si une bombe a explos�, NULL sinon.
*
* Si le temps est �coul�, alors on renvoie sa position et on dit la vire de la<br />
* file d'attente. De plus on rajoute cette bombe au nombre de bombe quantite_utilisable<br />
*
* @warning Cette fonction doit �tre utilis� en boucle !<br />
* Elle renvoie les bombes qui ont explos�es une � une.<br />
* Cette fonction <b>doit �tre � coupl�e avec un tanque</b> jusqu'a NULL
*
* Utilisation:
* @code
* // Plus haut on a bonus* joueur; qui a �t� instanci� et correctement defini
* s_Coordonnees pos = {0};
* // La on check toutes les bombes
* while( joueur->isEvent(&pos) == true )
* {
*	// Cette boucle est ex�cut�e autant de fois qu'il y a de bombes qui ont explos�es.
*	// Exemple, en cas d'utilisation d'un d�clancheur => toutes les bombes pos�es par le joueur vont exploser
*	// Traitement pour la bombe
*	printf("La bombe a la position X=%u, Y=%u � explos� !\n", pos.x, pos.y);
*	printf("De plus elle est de nouveau disponnible dans les bombes pouvant �tre lanc�es ! (quantite_utilisable)")
* }
* @endcode
*/
bool bonus::isEvent( s_Event* e )
{
	s_timeval now;
	gettimeofday(&now, 0);

	if( C_bonusProp[declancheur].quantite_MAX_Ramassable > 0 ){// Partie avec d�clancheur
		for( unsigned int i=0; i<c_listEvent.size(); i++ )
		{
			if( (quantiteUtilisable(declancheur) == 0 && c_listEvent.at(i).finEvent <= now)// On a pas de d�clancheur + timeout
				|| (quantiteUtilisable(declancheur) > 0 && (c_listEvent.at(i).finEvent.tv_sec == 0 || c_listEvent.at(i).finEvent.tv_usec == 0) )// On a un d�clancheur + timeout==0 => timeout
				){// Si timeout
				*e = c_listEvent.at(i);
				c_listEvent.erase(c_listEvent.begin()+i);// On supprime la bombe qui �tait pos�e
				incQuantiteUtilisable( bombe );// On redonne la bombe au joueur
				return true;
			}
		}

	}else{// Partie SANS d�clancheur
		for( unsigned int i=0; i<c_listEvent.size(); i++ )
		{
			if( c_listEvent.at(i).finEvent <= now ){// Si timeout
				*e = c_listEvent.at(i);
				c_listEvent.erase(c_listEvent.begin()+i);// On supprime la bombe qui �tait pos�e
				incQuantiteUtilisable( bombe );// On redonne la bombe au joueur
				return true;
			}
		}
	}
	return false;
}


/***************************************************************************//*!
* @fn bonus::t_Bonus bonus::getBonusAleatoire()
* @brief Renvoie un bonus al�atoire
* @return Renvoie un bonus al�atoire
*/
bonus::t_Bonus bonus::getBonusAleatoire()
{
	int r;
	//return  super_bombe;

	do{
		r = myRand(0, NB_ELEMENT_t_Bonus-1);
	}while( !C_bonusProp[r].quantite_MAX_Ramassable );// On ne prend que les bonus dont la quantit� MAX ramassable est non null !

	if( myRand(0, 100) <= C_bonusProp[r].probabiliter_pop )
		return (bonus::t_Bonus)r;

	do{
		r = myRand(0, NB_ELEMENT_t_Bonus-1);
	}while( !C_bonusProp[r].quantite_MAX_Ramassable );// On ne prend que les bonus dont la quantit� MAX ramassable est non null !

	if( myRand(0, 100) <= C_bonusProp[r].probabiliter_pop  )
		return (bonus::t_Bonus)r;

	return __RIEN__;
}


/***************************************************************************//*!
* @fn bonus::s_Event* bonus::getEvent( unsigned int x, unsigned int y )
* @brief Renvoie l'envent qui se trouve a la position X, Y
*/
bonus::s_Event* bonus::getEvent( unsigned int x, unsigned int y )
{
	for( unsigned int i=0; i<c_listEvent.size(); i++ )
	{
		if( c_listEvent.at(i).pos.x == x && c_listEvent.at(i).pos.y == y && c_listEvent.at(i).type == bombe )
			return &c_listEvent.at(i);
	}

	stdError("Attention pas d'event a X=%u, Y=%u", x, y);
	return 0;
}


/***************************************************************************//*!
* @fn void bonus::forceTimeOut()
* @brief Met fin au temps d'attente de tout les event
*/
void bonus::forceTimeOut()
{
	for( unsigned int i=0; i<c_listEvent.size(); i++ ){
		c_listEvent.at(i).finEvent.tv_sec = 0;
		c_listEvent.at(i).finEvent.tv_usec = 0;
	}
}


/***************************************************************************//*!
* @fn void bonus::forceTimeOut( unsigned int x, unsigned int y )
* @brief Met fin au temps d'attente de tout les event qui sont en X,Y
*/
void bonus::forceTimeOut( unsigned int x, unsigned int y )
{
	for( unsigned int i=0; i<c_listEvent.size(); i++ )
	{
		if( c_listEvent.at(i).pos.x == x && c_listEvent.at(i).pos.y == y ){
			c_listEvent.at(i).finEvent.tv_sec = 0;
			c_listEvent.at(i).finEvent.tv_usec = 0;
			return ;
		}
	}
}


/***************************************************************************//*!
* @fn void bonus::forceTimeOut( t_Bonus b )
* @brief Met fin au temps d'attente de tout les event qui sont == � b
*/
void bonus::forceTimeOut( t_Bonus b )
{
	for( unsigned int i=0; i<c_listEvent.size(); i++ )
	{
		if( c_listEvent.at(i).type == b ){
			c_listEvent.at(i).finEvent.tv_sec = 0;
			c_listEvent.at(i).finEvent.tv_usec = 0;
		}
	}
}


/***************************************************************************//*!
* @fn void bonus::kill()
* @brief Met fin au temps d'attente de tous les event et ajoute le signal kill,
* qui signifit, qu'il faut supprimer l'armement
*/
void bonus::kill()
{
	for( unsigned int i=0; i<c_listEvent.size(); i++ ){
		c_listEvent.at(i).finEvent.tv_sec = 0;
		c_listEvent.at(i).finEvent.tv_usec = 0;
	}

	// Cr�ation de l'event
	s_Event e;
	e.type = SIGNAL_kill;
	e.pos.x = 0;
	e.pos.y = 0;
	e.finEvent.tv_sec = 0;
	e.finEvent.tv_usec = 0;
	c_listEvent.push_back( e );// On ajout l'event � la liste des event
}
