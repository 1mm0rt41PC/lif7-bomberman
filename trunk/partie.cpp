#include "partie.h"

/*******************************************************************************
*
*/
partie::partie()
{
	c_nb_joueurs = 0;
	c_joueurs = 0;
	c_map = 0;
	//getkey(&c_nb_joueurs);
}


/*******************************************************************************
*
*/
partie::~partie()
{
	if( c_nb_joueurs && c_joueurs )
		delete[] c_joueurs;
	if( c_map )
		delete c_map;

	c_joueurs = 0;
	c_map = 0;
}


/***************************************************************************//*!
* @fn void partie::genMap()
* @brief Charge une map
*
* S'il existe déjà une map -> On vire l'ancienne map<br />
* Après avoir chargé la map, la fonction positionne les joueurs<br />
* <b>/!\\ Initialiser les perso en 1er !!! /!\\</b>
*/
void partie::genMap()
{
	if( !c_nb_joueurs || !c_joueurs )
		stdErrorVarE("Initialiser les perso avant la map !!! c_nb_joueurs(%u) c_joueurs(%u)", (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);

	if( c_map )
		delete c_map;
	c_map = new map();
	c_map->chargerMap();

	s_Coordonnees p;
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		p = c_map->mettreJoueurA_sa_PositionInitial( i+1 );
		c_joueurs[i].defPos(p.x, p.y);
	}
}


/***************************************************************************//*!
* @fn void partie::def_nbJoueurs( unsigned char nb )
* @brief Definie le nombre de joueur.
*
* S'il existe déjà des joueurs -> On vire les anciens joueurs
*/
void partie::def_nbJoueurs( unsigned char nb )
{
	if( c_nb_joueurs && c_joueurs ){
		stdErrorVar("c_nb_joueurs(%u), c_joueurs(%X)", (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
		delete[] c_joueurs;
	}
	c_nb_joueurs = nb;
	c_joueurs = new perso[nb];
}


/*******************************************************************************
*
*/
void partie::def_nbMAX_joueurs( unsigned char nb )
{
	c_nb_MAX_joueurs = nb;
}


/*******************************************************************************
*
*/
void partie::def_modeJeu( partie::t_MODE m )
{
	c_mode = m;
}


/*******************************************************************************
*
*/
unsigned char partie::nbJoueurs() const
{
	return c_nb_joueurs;
}


/***************************************************************************//*!
* @fn unsigned char partie::nbMAX_joueurs() const
* @brief Retourne le nombre de joueur maximum
*/
unsigned char partie::nbMAX_joueurs() const
{
	return c_nb_MAX_joueurs;
}


/***************************************************************************//*!
* @fn perso* partie::joueur( unsigned int joueur_numero )
* @brief Renvoie un joueur
* @return 0 en cas de bug ! Un pointeur vers le joueur demandé sinon.
*/
perso* partie::joueur( unsigned int joueur_numero ) const
{
	if( joueur_numero >= c_nb_joueurs || !c_joueurs ){
		stdErrorVar("joueur_numero(%u) >= c_nb_joueurs(%u) || !c_joueurs(%X)", (unsigned int)joueur_numero, (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
		return 0;
	}
	return c_joueurs+joueur_numero;
}

/*******************************************************************************
*
*/
partie::t_MODE partie::modeJeu() const
{
	return c_mode;
}


/***************************************************************************//*!
* @fn map* partie::getMap() const
* @brief Renvoie la map actuel
*/
map* partie::getMap() const
{
	return c_map;
}


/***************************************************************************//*!
* @fn unsigned char partie::nbJoueurVivant() const
* @brief Renvoie le nombre de joueur en vie
*/
unsigned char partie::nbJoueurVivant() const
{
	unsigned char nbJoueurVivant = 0;
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		if( c_joueurs[i].armements() && (c_joueurs[i].armements()->est_Dans_La_Liste(bonus::vie) == -1 || (c_joueurs[i].armements()->quantiteUtilisable(bonus::vie) > 0 || c_joueurs[i].armements()->quantiteMAX_Ramassable(bonus::vie) == 0)) )
			nbJoueurVivant++;
	}
	return nbJoueurVivant;
}


/***************************************************************************//*!
* @fn void partie::main( libAff* afficherMapEtEvent )
* @brief Lance le jeu
* @param[in] afficherMapEtEvent La fonction qui va servir à afficher la map
*
* Veuillez initialiser les variables :<br />
* - c_nb_joueurs
*
* @warning Toutes les variables doivent être correctement initialisées !
*/
void partie::main( libAff* afficherMapEtEvent )
{
	if( !afficherMapEtEvent )
		stdErrorE("Fonction d'affichage incorrect ! (afficherMapEtEvent=0)");

	// On initialise les bonus utilisables pour cette partie
	bonus::bonusProp();
	// On initialise les bonus pour les joueurs
	for( unsigned char i=0; i<c_nb_joueurs; i++ )
	{
		c_joueurs[i].defArmements( new bonus() );
		c_joueurs[i].armements()->param_Par_Defaut();
	}

	// On génère la map
	genMap();

	SYS_CLAVIER key;
	int i=0;
	bool continuerScanClavier=1;
	options* opt = options::getInstance();
	s_Coordonnees pos={0};
	s_Event e;

	// Commencement de la partie
	do{
		// Affichage de la map et récéption des Event
		key = afficherMapEtEvent( this );


		/***********************************************************************
		* On parcours les joueurs
		*/
		for( i=0; i<c_nb_joueurs; i++ )
		{
			// Si le joueur est mort, on saut son scan
			if( !c_joueurs[i].armements() ) continue ;

			/*******************************************************************
			* Scan des Events bonus de chaques joueurs
			*/
			while( c_joueurs[i].armements()->isEvent(&pos) )// Tanqu'il y a des event on tourne
			{
				//c_map->setBlock(pos.x, pos.y, map::vide);
				e.type = bonus::bombe;
				e.joueur = i+1;
				e.pos = pos;
				e.repetionSuivante = 0;
				e.Nb_Repetition = 0;
				e.Nb_Repetition_MAX = c_joueurs[i].armements()->quantiteUtilisable(bonus::puissance_flamme);
				c_listEvent.push_back( e );
			}

			/*******************************************************************
			* Scan du clavier pour les joueurs
			*/
			if( continuerScanClavier == 1 )
			switch( opt->clavierJoueur(i)->obtenirTouche(key) )
			{
				case clavier::NUL: {
					continuerScanClavier = 1;
					break;
				}
				case clavier::haut: {
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()-1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::bas: {
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::droite: {
					deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::gauche: {
					deplacer_le_Perso_A( c_joueurs[i].X()-1, c_joueurs[i].Y(), i );
					continuerScanClavier = 0;
					break;
				}
				/***************************************************************
				* On veux poser une bombe
				*/
				case clavier::lancerBombe: {
					/***********************************************************
					* IL NE RESTE PAS DE BOMBE EN STOCK
					*/
					if( !c_joueurs[i].armements()->quantiteUtilisable(bonus::bombe) )
						break;
					/***********************************************************
					* Il reste des bombes à poser ->
					* 2 Posibilités:
					* 1) Il n'y a que le joueur qui veut poser la bombe
					* 2) Il y a plusieurs joueurs
					*/
					switch( c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y())->element )// Utilisation d'un switch pour la rapidité ( en asm ebx modifié une fois => plus rapide )
					{
						/*******************************************************
						* Cas 1 et 2:
						*/
						case map::UN_joueur:
						case map::plusieurs_joueurs: {
							c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_UN_joueur );
							c_map->ajouterInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );// Ajout de l'info > Mais qui a donc posé la bombe ...
							c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe, c_joueurs[i].X(), c_joueurs[i].Y() );
							break;
						}
						default: {// On ne pose pas la bombe ! On a pas les conditions réuni
							break;
						}
					}
					continuerScanClavier = 0;
					break;
				}
				case clavier::declancheur: {
					// A FAIRE
					continuerScanClavier = 0;
					break;
				}
				default: {
					stdErrorVarE("Touche envoyé par le joueur %d est inconnue : %d", (int)opt->clavierJoueur(i)->obtenirTouche(key), (int)key);
					break;
				}
			}
		}

		/***********************************************************************
		* Scan des Events interne
		*/
		checkInternalEvent();

		continuerScanClavier = 1;


	}while( key != KEY_ESCAP && (nbJoueurVivant() > 1 || c_listEvent.size()) );

	if( nbJoueurVivant() <= 1 )
		stdError("Un joueur a gagner !");

	// On désinitialise les bonus utilisables pour cette partie
	bonus::unInitBonusProp();
}


/***************************************************************************//*!
* @fn void partie::deplacer_le_Perso_A( unsigned int newX, unsigned int newY )
* @brief Test si un personnage peut se déplacer à un endroit.
* Si on peut déplacer le perso dans la zone demandé => on déplace le perso
* SINON rien.
* C'est aussi par cette fonction que l'on :
*		- ramasse les bonus
*		- meur ( flamme )
*		- ne passe pas a travers les objets
*		- ne dépasse pas la taille de la map
*
* @bug <b>NE GERE PAS TOUT LES CAS</b>
* @todo faire tout les cas possibles
*/
void partie::deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur )
{
	map::t_type elementNouvellePosition;

	if( !c_map )
		stdErrorE("Initialiser la map avant !");

	if( !c_joueurs )
		stdErrorE("Initialiser les joueurs avant !");

	if( joueur >= c_nb_joueurs )
		stdErrorVarE("Le joueur %d n'existe pas ! Impossible de déplacer le joueur !", joueur);

	// Ce qui a sur la nouvelle case
	elementNouvellePosition = c_map->getBlock(newX, newY)->element;

	/***********************************************************************
	* Toutes les condition nécéssaire pour pouvoir bouger
	* Si l'une d'elle pas bonne => return void;
	*/
	if( !(	0 <= newX && newX < c_map->X()// On vérif si on ne dépasse pas la
		&&	0 <= newY && newY < c_map->Y()// taille de la map
		&&	elementNouvellePosition != map::Mur_destructible		// On vérif s'il n'y a pas
		&&	elementNouvellePosition != map::Mur_INdestructible	// d'objet solid
		&&	elementNouvellePosition != map::bombe_poser
		&&	elementNouvellePosition != map::bombe_poser_AVEC_UN_joueur
		&&	elementNouvellePosition != map::bombe_poser_AVEC_plusieurs_joueurs))
	{
		return ;
	}

	/***********************************************************************
	* On ne tombe pas sur un objet inconnu
	* Anti bug
	*/
	if( elementNouvellePosition == map::inconnu )
		stdErrorVarE("Objet inconnu dans la map à X=%d, Y=%d !", newX, newY);

	// Tout est Ok, on déplace le perso

	/***********************************************************************
	* On enlève le perso de son ancienne position.
	* ( Et remise en état de la case )
	*/
	// On regarde le nombre d'info joueur(meta données) sur la case actuel
	if( c_map->nb_InfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y()) > 1 ){
		// Il reste plus de 1 joueurs sur la case
		switch( c_map->getBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y())->element )
		{
			case map::bombe_poser_AVEC_UN_joueur: {
				// On enlève le perso de son ancienne position
				c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::bombe_poser);
				c_map->rmInfoJoueur( c_joueurs[joueur].X(), c_joueurs[joueur].Y(), joueur+1, 0 );
				break;
			}

			case map::plusieurs_joueurs: {
				// On enlève le perso de son ancienne position et S'il n'y avait plus que 2 joueurs => il ne restera q'un joueur
				if( c_map->nb_InfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y()) == 2 )
					c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::UN_joueur);

				c_map->rmInfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), joueur+1, 1);
				break;
			}

			case map::bombe_poser_AVEC_plusieurs_joueurs: {
				break;
			}

			default: {
				stdErrorVar("cas NON traite %d", (int)c_map->getBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y())->element);
				break;
			}
		}
	}else{// Suppression total -> case toute propre
		// On enlève le perso de son ancienne position
		c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::vide);
	}

	/***********************************************************************
	* On place le perso a sa nouvelle position
	*/
	switch( elementNouvellePosition )
	{
		case map::vide: {
			c_map->setBlock(newX, newY, map::UN_joueur);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::UN_joueur:
		case map::plusieurs_joueurs: {
			c_map->setBlock(newX, newY, map::plusieurs_joueurs);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		case map::flamme_horizontal:
		case map::flamme_vertical:
		case map::flamme_pointe_haut:
		case map::flamme_pointe_bas:
		case map::flamme_pointe_droite:
		case map::flamme_pointe_gauche:
		case map::flamme_origine: {
			// Viens d'aller dans le feu -> Boulet ?
			c_joueurs[joueur].defArmements(0);// Mort
			c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::vide);
			break;
		}
		default: {
			stdErrorVarE("Cas non pris en charge c_map->getBlock(%u, %u).element=%d", newX, newY, elementNouvellePosition);
		}
	}
}


/***************************************************************************//*!
* @fn void partie::checkInternalEvent()
* @brief Analyse et traite les Event internes
*/
void partie::checkInternalEvent()
{
	/*
		typedef struct {
			bonus::t_Bonus type;//!< Type de bonus
			s_Coordonnees pos;//!< Position Originel de l'event
			unsigned char joueur;//!< Le joueur qui est la cause de l'event
			unsigned char Nb_Repetition;//!< Nombre de répétition actuel pour l'event
			clock_t repetionSuivante;//!< Time de la prochaine répétion
		} s_Event;
	*/
	unsigned char j;
	s_Event e;
	bool continuerBoucle = 1;
	for( unsigned int i=0; i<c_listEvent.size(); i++ )
	{
		// Tant que l'on a pas atteint le temps
		if( c_listEvent.at(i).repetionSuivante > clock() ) continue;

		e = c_listEvent.at(i);
		switch( c_listEvent.at(i).type )
		{
			case bonus::bombe: {
				// Encore un tour ?
				if( e.Nb_Repetition > e.Nb_Repetition_MAX ){
					c_listEvent.erase(c_listEvent.begin()+i);
					// On reset les block touché par vide
					/*******************************************************
					* On efface les flammes
					*/
					// Centre de la déflagration
					c_map->setBlock(e.pos.x, e.pos.y, map::vide);

					// Haut de la déflagration
					for( j=1; j<e.Nb_Repetition; j++ )
					{
						// Adition des flammes avec le reste de l'environement
						switch( c_map->getBlock(e.pos.x, e.pos.y-j)->element )// Monté vers le haut
						{
							case map::flamme_horizontal:
							case map::flamme_vertical:
							case map::flamme_pointe_haut:
							case map::flamme_pointe_bas:
							case map::flamme_pointe_droite:
							case map::flamme_pointe_gauche:
							case map::flamme_origine: {
								c_map->setBlock(e.pos.x, e.pos.y-j, map::vide);
								break;
							}
							default: {// On s'arrête sur les autres cas
								j = e.Nb_Repetition;// On va pas plus loin
								break;
							}
						}
					}
					// Bas de la déflagration
					for( j=1; j<e.Nb_Repetition; j++ )
					{
						// Adition des flammes avec le reste de l'environement
						switch( c_map->getBlock(e.pos.x, e.pos.y+j)->element )// Monté vers le haut
						{
							case map::flamme_horizontal:
							case map::flamme_vertical:
							case map::flamme_pointe_haut:
							case map::flamme_pointe_bas:
							case map::flamme_pointe_droite:
							case map::flamme_pointe_gauche:
							case map::flamme_origine: {
								c_map->setBlock(e.pos.x, e.pos.y+j, map::vide);
								break;
							}
							default: {// On s'arrête sur les autres cas
								j = e.Nb_Repetition;// On va pas plus loin
								break;
							}
						}
					}
					// Droite de la déflagration
					for( j=1; j<e.Nb_Repetition; j++ )
					{
						// Adition des flammes avec le reste de l'environement
						switch( c_map->getBlock(e.pos.x+j, e.pos.y)->element )// Monté vers le haut
						{
							case map::flamme_horizontal:
							case map::flamme_vertical:
							case map::flamme_pointe_haut:
							case map::flamme_pointe_bas:
							case map::flamme_pointe_droite:
							case map::flamme_pointe_gauche:
							case map::flamme_origine: {
								c_map->setBlock(e.pos.x+j, e.pos.y, map::vide);
								break;
							}
							default: {// On s'arrête sur les autres cas
								j = e.Nb_Repetition;// On va pas plus loin
								break;
							}
						}
					}
					// Gauche de la déflagration
					for( j=1; j<e.Nb_Repetition; j++ )
					{
						// Adition des flammes avec le reste de l'environement
						switch( c_map->getBlock(e.pos.x-j, e.pos.y)->element )// Monté vers le haut
						{
							case map::flamme_horizontal:
							case map::flamme_vertical:
							case map::flamme_pointe_haut:
							case map::flamme_pointe_bas:
							case map::flamme_pointe_droite:
							case map::flamme_pointe_gauche:
							case map::flamme_origine: {
								c_map->setBlock(e.pos.x-j, e.pos.y, map::vide);
								break;
							}
							default: {// On s'arrête sur les autres cas
								j = e.Nb_Repetition;// On va pas plus loin
								break;
							}
						}
					}
					break;
				}

				/***********************************************************
				************************************************************
				* Explosion en cours
				************************************************************
				***********************************************************/

				/***********************************************************
				* Centre de la déflagration
				*/
				killPlayers(e.pos.x, e.pos.y);// Adition des flammes avec le reste de l'environement
				c_map->setBlock(e.pos.x, e.pos.y, map::flamme_origine);

				/***********************************************************
				* Haut de la déflagration
				*/
				continuerBoucle = 1;
				for( j=1; j<=e.Nb_Repetition && continuerBoucle; j++ )
				{
					switch( killPlayers(e.pos.x, e.pos.y-j) )
					{
						case -1: {
							continuerBoucle = 0;
							break;
						}
						case 0: {
							continuerBoucle = 0;
							j = e.Nb_Repetition;// Pour mettre une pointe
						}// Pas de break pour lire le block du dessous
						case 1: {
							if( j == e.Nb_Repetition ){
								c_map->setBlock(e.pos.x, e.pos.y-j, map::flamme_pointe_haut);
							}else{
								c_map->setBlock(e.pos.x, e.pos.y-j, map::flamme_vertical);
							}
							break;
						}
					}
				}

				/***********************************************************
				* Bas de la déflagration
				*/
				continuerBoucle = 1;
				for( j=1; j<=e.Nb_Repetition && continuerBoucle; j++ )
				{
					switch( killPlayers(e.pos.x, e.pos.y+j) )
					{
						case -1: {
							continuerBoucle = 0;
							break;
						}
						case 0: {
							continuerBoucle = 0;
							j = e.Nb_Repetition;// Pour mettre une pointe
						}// Pas de break pour lire le block du dessous
						case 1: {
							if( j == e.Nb_Repetition ){
								c_map->setBlock(e.pos.x, e.pos.y+j, map::flamme_pointe_bas);
							}else{
								c_map->setBlock(e.pos.x, e.pos.y+j, map::flamme_vertical);
							}
							break;
						}
					}
				}

				/***********************************************************
				* Droite de la déflagration
				*/
				continuerBoucle = 1;
				for( j=1; j<=e.Nb_Repetition && continuerBoucle; j++ )
				{
					switch( killPlayers(e.pos.x+j, e.pos.y) )
					{
						case -1: {
							continuerBoucle = 0;
							break;
						}
						case 0: {
							continuerBoucle = 0;
							j = e.Nb_Repetition;// Pour mettre une pointe
						}// Pas de break pour lire le block du dessous
						case 1: {
							if( j == e.Nb_Repetition ){
								c_map->setBlock(e.pos.x+j, e.pos.y, map::flamme_pointe_droite);
							}else{
								c_map->setBlock(e.pos.x+j, e.pos.y, map::flamme_horizontal);
							}
							break;
						}
					}
				}

				/***********************************************************
				* Gauche de la déflagration
				*/
				continuerBoucle = 1;
				for( j=1; j<=e.Nb_Repetition && continuerBoucle; j++ )
				{
					switch( killPlayers(e.pos.x-j, e.pos.y) )
					{
						case -1: {
							continuerBoucle = 0;
							break;
						}
						case 0: {
							continuerBoucle = 0;
							j = e.Nb_Repetition;// Pour mettre une pointe
						}// Pas de break pour lire le block du dessous
						case 1: {
							if( j == e.Nb_Repetition ){
								c_map->setBlock(e.pos.x-j, e.pos.y, map::flamme_pointe_gauche);
							}else{
								c_map->setBlock(e.pos.x-j, e.pos.y, map::flamme_horizontal);
							}
							break;
						}
					}
				}

				c_listEvent.at(i).Nb_Repetition++;
				c_listEvent.at(i).repetionSuivante = clock() + VITESSE_DEFLAGRATION_FLAMME;// * CLOCKS_PER_SEC;// Ajustement du temps
				break;
			}
			default: {
				break;
			}
		}
	}
}


/***************************************************************************//*!
* @fn char partie::killPlayers( unsigned int x, unsigned int y )
* @brief Tue les joueurs et détruit les block destructibles
* @param[in] x Position X de la map a check
* @param[in] y Position Y de la map a check
* @return 3 Valeur de retoures possible<br />
*			- -1 On stop tout et on ne touche pas au block à cette adresse ! (Out of map ou Mur_INdestructible)
*			-  0 On stop tout mais on traite le block à l'adresse (Mur_destructible)
*			-  1 On continue
*
* @note Les valeurs de X et de Y sont comparées à la taille de la map => Pas de risque de sortir de la map
*/
char partie::killPlayers( unsigned int x, unsigned int y )
{
	static unsigned int k;// ça ne sert à rien de la recréer à chaque fois

	if( x >= c_map->X() || y >= c_map->Y() )
		return -1;

	switch( c_map->getBlock(x, y)->element )// Monté vers le haut
	{
		case map::UN_joueur: {// Le pauvre joueur qui est au millieu du chemin de la flamme -> dead
			c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].defArmements(0);
			return 1;
		}
		case map::plusieurs_joueurs: {// Les pauvres joueurs qui sont au millieu du chemin de la flamme -> dead
				for( k=0; k<c_map->getBlock(x, y)->joueur->size(); k++ )
				{
					c_joueurs[c_map->getBlock(x, y)->joueur->at(k)-1].defArmements(0);
				}
			return 1;
		}
		case map::bombe_poser_AVEC_UN_joueur: {// Le pauvre joueur qui est au millieu du chemin de la flamme -> dead
			c_joueurs[c_map->getBlock(x, y)->joueur->at(1)-1].defArmements(0);
			return 1;
		}
		case map::bombe_poser_AVEC_plusieurs_joueurs: {// Les pauvres joueurs qui sont au millieu du chemin de la flamme -> dead
				for( k=1; k<c_map->getBlock(x, y)->joueur->size(); k++ )
				{
					c_joueurs[c_map->getBlock(x, y)->joueur->at(k)-1].defArmements(0);
				}
			return 1;
		}
		case map::Mur_destructible: {
			c_map->ajouterInfoJoueur(x, y, 0, 1);
			return 0;
		}
		case map::Mur_INdestructible: {
			return -1;
		}
		case map::flamme_pointe_haut:
		case map::flamme_pointe_bas:
		case map::flamme_pointe_droite:
		case map::flamme_pointe_gauche:{
			if( c_map->getBlock(x, y)->joueur && c_map->getBlock(x, y)->joueur->size() && c_map->getBlock(x, y)->joueur->at(0) == 0 )
				return -1;

			break;
		}
		default: {// On eject les autres cas
			break;
		}
	}

	return 1;
}
