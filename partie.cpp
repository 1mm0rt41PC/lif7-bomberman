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

	map::s_Pos p;
	for( unsigned char i=0; i<c_nb_joueurs; i++ ){
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
void partie::def_modeJeu( partie::MODE m )
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
*
* @brief Retourne le nombre de joueur maximum
*
*/
unsigned char partie::nbMAX_joueurs() const
{
	return c_nb_MAX_joueurs;
}


/***************************************************************************//*!
* @fn perso* partie::joueur( unsigned int joueur_numero )
*
* @brief Renvoie un joueur
* @return 0 en cas de bug ! Un pointeur vers le joueur demandé sinon.
*/
perso* partie::joueur( unsigned int joueur_numero ) const
{
	if(joueur_numero >= c_nb_joueurs || !c_joueurs){
		stdErrorVar("joueur_numero(%u) >= c_nb_joueurs(%u) || !c_joueurs(%X)", (unsigned int)joueur_numero, (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
		return 0;
	}
	return c_joueurs+joueur_numero;
}

/*******************************************************************************
*
*/
partie::MODE partie::modeJeu() const
{
	return c_mode;
}


/***************************************************************************//*!
* @fn map* partie::getMap() const
* @brief Renvoie la map actuel
*
*/
map* partie::getMap() const
{
	return c_map;
}


/***************************************************************************//*!
* @fn unsigned char partie::nbJoueurVivant() const
* @brief Renvoie le nombre de joueur en vie
*
*/
unsigned char partie::nbJoueurVivant() const
{
	unsigned char nbJoueurVivant = 0;
	for(unsigned char i=0; i<c_nb_joueurs; i++){
		if(c_joueurs[i].armements() && (c_joueurs[i].armements()->quantiteUtilisable(bonus::vie) > 0 || c_joueurs[i].armements()->quantiteMAX_Ramassable(bonus::vie) == 0))
			nbJoueurVivant++;
	}
	return nbJoueurVivant;
}


/***************************************************************************//*!
* @fn void partie::main()
* @brief Lance le jeu
*/
void partie::main( libAff* afficherMapEtEvent )
{
	if(!afficherMapEtEvent)
		stdErrorE("Fonction d'affichage incorrect ! (afficherMapEtEvent=0)");

	// On initialise les bonus utilisables pour cette partie
	bonus::bonusProp();
	// On initialise les bonus pour les joueurs
	for(unsigned char i=0; i<c_nb_joueurs; i++){
		c_joueurs[i].defArmements( new bonus() );
		c_joueurs[i].armements()->param_Par_Defaut();
	}

	// On génère la map
	genMap();

	SYS_CLAVIER key;
	int i=0;
	bool continuerScanClavier=1;

	// Commencement de la partie
	do{
		// Affichage de la map et récéption des Event
		key = afficherMapEtEvent( this );

		/***********************************************************************
		* Scan du clavier pour les joueurs
		*/
		for( i=0; i<c_nb_joueurs && continuerScanClavier == 1; i++ ){
			switch( G_OPTIONS.clavierJoueur(i)->obtenirTouche(key) ){
				case clavier::NUL:{
					continuerScanClavier = 1;
					break;
				}
				case clavier::haut:{
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()-1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::bas:{
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::droite:{
					deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::gauche:{
					deplacer_le_Perso_A( c_joueurs[i].X()-1, c_joueurs[i].Y(), i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::lancerBombe:{
					if(!c_joueurs[i].armements()->quantiteUtilisable( bonus::bombe ))
						break;/*IL NE RESTE PAS DE BOMBE EN STOCK*/

					switch(c_map->getBlock( c_joueurs[i].X(), c_joueurs[i].Y()).element ){
						case map::UN_joueur:{
							c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_UN_joueur, i+1 );// id_joueur = 0 => on ne met pas le joueur dans les meta données
							c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe );
							break;
						}
						case map::plusieurs_joueurs:{
							c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_UN_joueur, i+1 );// id_joueur = 0 => on ne met pas le joueur dans les meta données
							c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe );
							break;
						}
						default:{
							break;
						}
					}
					continuerScanClavier = 0;
					break;
				}
				case clavier::declancheur:{
					// A FAIRE
					continuerScanClavier = 0;
					break;
				}
				default:{
					stdErrorVarE("Touche envoyé par le joueur %d est inconnue : %d", (int)G_OPTIONS.clavierJoueur(i)->obtenirTouche(key), (int)key);
					break;
				}
			}
		}
		continuerScanClavier = 1;


		/***********************************************************************
		* Scan des compteurs
		*/

	}while(key != KEY_ESCAP);


	// On désinitialise les bonus utilisables pour cette partie
	bonus::unInitBonusProp();
}


/***************************************************************************//*!
* @fn void partie::deplacer_le_Perso_A( unsigned int newX, unsigned int newY )
*
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
	if(!c_map)
		stdErrorE("Initialiser la map avant !");

	if(!c_joueurs)
		stdErrorE("Initialiser les joueurs avant !");

	if(joueur >= c_nb_joueurs)
		stdErrorVarE("Le joueur %d n'existe pas ! Impossible de déplacer le joueur !", joueur);

	if(		0 <= newX && newX < c_map->X()// On vérif si on ne dépasse pas la
		&&	0 <= newY && newY < c_map->Y()// taille de la map
		&&	c_map->getBlock(newX, newY).element != map::Mur_destrucible		// On vérif s'il n'y a pas
		&&	c_map->getBlock(newX, newY).element != map::Mur_INdestrucible	// d'objet solid
		&&	c_map->getBlock(newX, newY).element != map::bombe_poser
		&&	c_map->getBlock(newX, newY).element != map::bombe_poser_AVEC_UN_joueur
		&&	c_map->getBlock(newX, newY).element != map::bombe_poser_AVEC_plusieurs_joueurs)
	{
		// On ne tombe pas sur un objet inconnu
		if(c_map->getBlock(newX, newY).element == map::inconnu)
			stdErrorVarE("Objet inconnu dans la map à X=%d, Y=%d !", newX, newY);

		// Tout est Ok, on déplace le perso

		/***********************************************************************
		* On enlève le perso de son ancienne position.
		* ( Et remise en état de la case )
		*/
		// On regarde le nombre de joueur sur la case actuel
		if(c_map->getNbJoueur_SurBlock( c_joueurs[joueur].X(), c_joueurs[joueur].Y() ) > 1){stdError("zarb");
			// Il reste plus de 1 joueurs sur la case
			switch(c_map->getBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y()).element){
				case map::bombe_poser_AVEC_UN_joueur:{
					// On enlève le perso de son ancienne position
					c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::bombe_poser, 0);// id_joueur = 0 => on ne met pas le joueur dans les meta données
					c_map->rmInfoJoueur( c_joueurs[joueur].X(), c_joueurs[joueur].Y(), joueur+1, 0 );// NE PAS OUBLIER QUE {joueur} va ICI de [0 à 255] OR LA FONCTION VEUT [1 à 255]
					break;
				}

				case map::plusieurs_joueurs:{
					break;
				}

				case map::bombe_poser_AVEC_plusieurs_joueurs:{
					break;
				}

				default:{
					break;
				}
			}
		}else{// Suppression total -> case toute propre
			// On enlève le perso de son ancienne position
			c_map->setBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), map::vide, joueur+1);// NE PAS OUBLIER QUE {joueur} va ICI de [0 à 255] OR LA FONCTION VEUT [1 à 255]
			c_map->rmInfoJoueur(c_joueurs[joueur].X(), c_joueurs[joueur].Y(), joueur+1, 1);// NE PAS OUBLIER QUE {joueur} va ICI de [0 à 255] OR LA FONCTION VEUT [1 à 255]
		}

		/***********************************************************************
		* On place le perso a sa nouvelle position
		*/
		c_map->setBlock(newX, newY, map::UN_joueur, joueur+1);
		c_joueurs[joueur].defPos(newX, newY);
	}
}
