#include "partie.h"
#include "debug.h"

/***************************************************************************//*!
* @fn partie::partie()
* @brief Initialise la class partie
*
* Pas de new ici
*/
partie::partie()
{
	c_connection = CNX_None;
	c_nb_joueurs = 0;
	c_joueurs = 0;
	c_map = 0;

	c_server = 0;
	//c_client = 0;// <- Pas besoin ( union )

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
}


/***************************************************************************//*!
* @fn partie::~partie()
* @brief Désinitialise la class partie et supprime tous les joueurs et la map
*/
partie::~partie()
{
	if( c_nb_joueurs && c_joueurs )
		delete[] c_joueurs;
	if( c_map )
		delete c_map;

	c_joueurs = 0;
	c_map = 0;

	if( c_connection == CNX_Host && c_server ){
		delete c_server;
		c_server = 0;
	}

	if( c_connection == CNX_Client && c_client ){
		delete c_client;
		c_client = 0;
	}
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
		stdErrorE("Initialiser les perso avant la map !!! c_nb_joueurs(%u) c_joueurs(%u)", (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);

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
		stdError("c_nb_joueurs(%u), c_joueurs(%X)", (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
		delete[] c_joueurs;
	}
	if(	!nb	)
		stdErrorE("def_nbJoueurs(0) Interdit !");

	c_nb_joueurs = nb;
	c_joueurs = new perso[nb];
}


/***************************************************************************//*!
* @fn void partie::def_connection( partie::t_Connection cnx )
* @brief Permet de modifier la connection: Host, Local, None ( pas de connection )
*/
void partie::def_connection( partie::t_Connection cnx )
{
	c_connection = cnx;
	if( cnx == CNX_Host )
		c_server = new server;

	if( cnx == CNX_Client )
		c_client = new client;
}


/***************************************************************************//*!
* @fn perso* partie::joueur( unsigned int joueur_numero ) const
* @brief Renvoie un joueur
* @return 0 en cas de bug ! Un pointeur vers le joueur demandé sinon.
*/
perso* partie::joueur( unsigned int joueur_numero ) const
{
	if( joueur_numero >= c_nb_joueurs || !c_joueurs ){
		stdError("joueur_numero(%u) >= c_nb_joueurs(%u) || !c_joueurs(%X)", (unsigned int)joueur_numero, (unsigned int)c_nb_joueurs, (unsigned int)c_joueurs);
		return 0;
	}
	return c_joueurs+joueur_numero;
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
* @fn server* partie::getServeur() const
* @brief Renvoie le serveur utilisé pour communiquer.
* @return Le pointeur vers le serveur si tout est OK. NULL sinon
*/
server* partie::getServeur() const
{
	if( c_connection == CNX_Host )
		return c_server;

	stdError("Attention ! Cette partie n'est pas configurée en tant que Host !");
	return NULL;
}


/***************************************************************************//*!
* @fn client* partie::getClient() const
* @brief Renvoie le client utilisé pour communiquer.
* @return Le pointeur vers le client si tout est OK. NULL sinon
*/
client* partie::getClient() const
{
	if( c_connection == CNX_Client )
		return c_client;

	stdError("Attention ! Cette partie n'est pas configurée en tant que Client !");
	return NULL;
}


/***************************************************************************//*!
* @fn void partie::main( libAff * afficherMapEtEvent )
* @brief Lance le jeu
* @param[in] afficherMapEtEvent La fonction qui va servir à afficher la map
*
* Veuillez initialiser les variables :<br />
* - c_nb_joueurs
*
* @warning Toutes les variables doivent être correctement initialisées !
*/
//void partie::start( CLASS_TO_USE& moteur, fctAff afficherMapEtEvent )
void partie::main( libAff * afficherMapEtEvent )
{
	if( !afficherMapEtEvent )
		stdErrorE("Fonction d'affichage incorrect ! (afficherMapEtEvent=0)");

	SYS_CLAVIER key;
	int i=0;
	bool continuerScanClavier=1;
	options* opt = options::getInstance();
	bonus::s_Event bonusEvent;
	s_Event e;
	SOCKET s;

	/*##########################################################################
	* Partie en mode client
	*/
	if( c_connection == CNX_Client ){
		// Teste sur les types
		if( sizeof(uint8_t) != sizeof(unsigned char) )
			stdErrorE("sizeof(uint8_t){%u} != sizeof(unsigned char){%u}", sizeof(uint8_t), sizeof(unsigned char));

		// Tentative de connection
		if( !c_client->connection() ){
			stdError("Impossible de se connecter au serveur demandé !");
			return ;
		}

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Envoie de la config 32Bits?, IsIntel?
		sprintf(c_buffer, "%d %d", Is32Bits, (int)baseClientServer::isLittleEndian());
		c_client->send_message(c_buffer, PACK_bufferSize);

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Attente de la réponce
		if( !c_client->readServer(c_buffer, PACK_bufferSize) ){
			stdError("Configuration 32Bits, IsIntel incorrect pour le serveur ! Connection refusé !");
			return ;// On a été déconnecté !
		}

		// Envoie du nom
		c_client->send_message(c_joueurs[0].nom()->c_str(), c_joueurs[0].nom()->length());

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Attente de la réponce
		if( !c_client->readServer(c_buffer, PACK_bufferSize) ){
			stdError("Nom déjà utilisé ! Connection refusé !");
			return ;// On a été déconnecté !
		}

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		// Attente de la dimention de la map
		if( !c_client->readServer(c_buffer, PACK_bufferSize) ){
			stdError("Erreur de connection !");
			return ;// On a été déconnecté !
		}
		// Définition des variables qui seront utilisées
		uint32_t X=0, Y=0, nb_MetaDonnee=0;
		map::t_type type;

		unPackIt( &X, &Y, &type, &nb_MetaDonnee );// type et nb_MetaDonnee inutile ici

		if( c_map )
			delete c_map;
		c_map = new map(X,Y);// Création de la map

		memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		while( !c_client->readServer(c_buffer, PACK_bufferSize) && std::string("end") != c_buffer ){
			unPackIt( &X, &Y, &type, &nb_MetaDonnee );// type et nb_MetaDonnee inutile ici
			c_map->setBlock(X, Y, type);
			c_map->ajouterInfoJoueur(X, Y, nb_MetaDonnee);
			memset(c_buffer, 0, PACK_bufferSize);// On vide le buffer
		}

		// Commencement de la partie
		do{
			// Affichage de la map et récéption des Event claviers
			//key = moteur.afficherMapEtEvent( this );
			key = afficherMapEtEvent( this );

		}while( key != RETOUR_MENU_PRECEDENT && c_client->is_connected() );
		return ;
	}

	/*##########################################################################
	* Partie en mode serveur ou offline
	*/
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

	if( c_connection == CNX_Host ){// Si on est en mode serveur alors on connecte le serveur
		// On détermine le nombre maximum de player online
		unsigned int nbLocal=0;
		for( i=0; i<c_nb_joueurs; i++ )
			nbLocal += c_joueurs[i].isLocal();
		stdError("Nombre de player online max: %u",c_nb_joueurs-nbLocal);
		// Ajustement des paramètres
		c_server->setNbClientMax(c_nb_joueurs-nbLocal);
		c_server->Listening();

		// Teste sur les types
		if( sizeof(uint8_t) != sizeof(unsigned char) )
			stdErrorE("sizeof(uint8_t){%u} != sizeof(unsigned char){%u}", sizeof(uint8_t), sizeof(unsigned char));

		// S'il n'y a qu'un perso en local, alors on wait un perso exterieur
		if( nbLocal == 1 ){
			c_server->setWait(0,100000);// 0.1 secondes
			// Ajout des player
			while( (s = c_server->lookupNewConnection()) == INVALID_SOCKET && key != RETOUR_MENU_PRECEDENT )
			{
				key = afficherMapEtEvent( this );// Pour récup les touches
			}
			// On a enfin un player, on lui envoie la map
			ajouterNouvelleConnection( s );
		}
		c_server->setWait(0,1000);// 0.001 secondes
	}

	// Commencement de la partie
	do{
		/***********************************************************************
		* Traitement des connections ( HOST )
		*/
		if( c_connection == CNX_Host ){
			// On traite les joueurs déjà connecté
			while( (s = c_server->lookupConnectionClient()) != INVALID_SOCKET )
			{
				if( c_server->readClient( s, c_buffer, PACK_bufferSize ) ){

				}else{
					stdError("Joueur déco");
				}
			}
			// Ajout des player
			while( (s = c_server->lookupNewConnection()) != INVALID_SOCKET )
			{
				// Nouveau joueur !
				ajouterNouvelleConnection( s );
			}
		}

		// Affichage de la map et récéption des Event
		//key = moteur.afficherMapEtEvent( this );
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
			while( c_joueurs[i].armements()->isEvent(&bonusEvent) )// Tanqu'il y a des event on tourne
			{
				//c_map->setBlock(pos.x, pos.y, map::vide);
				e.type = bonusEvent.type;
				e.joueur = i+1;
				e.pos = bonusEvent.pos;
				e.repetionSuivante = 0;
				e.Nb_Repetition = 0;
				e.Nb_Repetition_MAX = c_joueurs[i].armements()->quantiteUtilisable(bonus::puissance_flamme);
				e.continue_X = true;
				e.continue_negativeX = true;
				e.continue_Y = true;
				e.continue_negativeY = true;
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
					c_joueurs[i].defOrientation(perso::ORI_haut);
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()-1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::bas: {
					c_joueurs[i].defOrientation(perso::ORI_bas);
					deplacer_le_Perso_A( c_joueurs[i].X(), c_joueurs[i].Y()+1, i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::droite: {
					c_joueurs[i].defOrientation(perso::ORI_droite);
					deplacer_le_Perso_A( c_joueurs[i].X()+1, c_joueurs[i].Y(), i );
					continuerScanClavier = 0;
					break;
				}
				case clavier::gauche: {
					c_joueurs[i].defOrientation(perso::ORI_gauche);
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
						case map::UN_joueur:{
							c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_UN_joueur );
							c_map->ajouterInfoJoueur( c_joueurs[i].X(), c_joueurs[i].Y(), i+1, 1 );// Ajout de l'info > Mais qui a donc posé la bombe ...
							c_joueurs[i].armements()->decQuantiteUtilisable( bonus::bombe, c_joueurs[i].X(), c_joueurs[i].Y() );
							break;
						}
						case map::plusieurs_joueurs: {
							c_map->setBlock( c_joueurs[i].X(), c_joueurs[i].Y(), map::bombe_poser_AVEC_plusieurs_joueurs );
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
					if( c_joueurs[i].armements()->quantiteUtilisable(bonus::declancheur) )
						c_joueurs[i].armements()->forceTimeOut();
					continuerScanClavier = 0;
					break;
				}
				default: {
					stdErrorE("Touche envoyé par le joueur %d est inconnue : %d", (int)opt->clavierJoueur(i)->obtenirTouche(key), (int)key);
					break;
				}
			}
		}

		/***********************************************************************
		* Scan des Events interne
		*/
		checkInternalEvent();

		continuerScanClavier = 1;

	}while( key != RETOUR_MENU_PRECEDENT && (nbJoueurVivant() > 1 || c_listEvent.size()) );

	if( nbJoueurVivant() <= 1 )
		stdError("Un joueur a gagner !");

	// On désinitialise les bonus utilisables pour cette partie
	bonus::unInitBonusProp();

	if( c_connection == CNX_Host )// Si on est en mode serveur alors on connecte le serveur
		c_server->endListen();
}


/***************************************************************************//*!
* @fn void partie::deplacer_le_Perso_A( unsigned int newX, unsigned int newY, unsigned char joueur )
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
		stdErrorE("Le joueur %d n'existe pas ! Impossible de déplacer le joueur !", joueur);

	// Ce qui a sur la nouvelle case
	elementNouvellePosition = c_map->getBlock(newX, newY)->element;

	/***********************************************************************
	* Toutes les condition nécéssaire pour pouvoir bouger
	* Si l'une d'elle pas bonne => return void;
	* NOTE: newX, newY unsigned ! => pas besoin de check => 0
	*/
	if( !(	/*0 <= newX &&*/ newX < c_map->X()// On vérif si on ne dépasse pas la
		&&	/*0 <= newY &&*/ newY < c_map->Y()// taille de la map
		&&	elementNouvellePosition != map::Mur_destructible		// On vérif s'il n'y a pas
		&&	elementNouvellePosition != map::Mur_INdestructible		// d'objet solid
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
		stdErrorE("Objet inconnu dans la map à X=%d, Y=%d !", newX, newY);

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
				stdError("cas NON traite %d", (int)c_map->getBlock(c_joueurs[joueur].X(), c_joueurs[joueur].Y())->element);
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
		case map::bonus: {
			if( !c_map->getBlock(newX, newY)->joueur )
				stdErrorE("BONUS: Problème de pointeur ! c_map->getBlock(%u, %u)->joueur = 0 !", newX, newY);
			if( !c_map->getBlock(newX, newY)->joueur->size() )
				stdErrorE("BONUS: Problème de contenu ! c_map->getBlock(%u, %u)->joueur->size() = 0 !", newX, newY);
			if( c_map->getBlock(newX, newY)->joueur->at(0) >= NB_ELEMENT_t_Bonus )
				stdErrorE("BONUS: Problème de contenu ! c_map->getBlock(%u, %u)->joueur->at(0){%u} >= {%d}NB_ELEMENT_t_Bonus !", newX, newY, (unsigned int)c_map->getBlock(newX, newY)->joueur->at(0), NB_ELEMENT_t_Bonus);
			// Ajout de l'armement
			c_joueurs[joueur].armements()->incQuantiteMAX_en_stock((bonus::t_Bonus)c_map->getBlock(newX, newY)->joueur->at(0));
			c_map->setBlock(newX, newY, map::vide);// On surrpime tout ce qui avait avant
			// Placement du perso
			c_map->setBlock(newX, newY, map::UN_joueur);
			c_map->ajouterInfoJoueur(newX, newY, joueur+1);
			c_joueurs[joueur].defPos(newX, newY);
			break;
		}
		default: {
			stdErrorE("Cas non pris en charge c_map->getBlock(%u, %u).element=%d", newX, newY, elementNouvellePosition);
		}
	}
}


/***************************************************************************//*!
* @fn void partie::checkInternalEvent()
* @brief Analyse et traite les Event internes pour une partie F4A
* @todo la selection d'Event
*/
void partie::checkInternalEvent()
{
	//static s_Event& e;
	static unsigned int i;

	for( i=0; i<c_listEvent.size(); i++ )
	{
		// Tant que l'on a pas atteint le temps
		if( c_listEvent.at(i).repetionSuivante > clock() ) continue;


		s_Event& e = c_listEvent.at(i);
		// A FAIRE
		// On select le type d'event
		//

		// Encore un tour ?
		if( e.Nb_Repetition > e.Nb_Repetition_MAX ){// On a fini ! Plus de tour
			// Suppression des blocks en-flammé
			for( unsigned int j=0; j<e.deflagration.size(); j++ )
			{
				c_map->rmInfoJoueur(e.deflagration.at(j).x, e.deflagration.at(j).y, e.joueur, true);
				if( !c_map->getBlock(e.deflagration.at(j))->joueur->size() )
					c_map->setBlock(e.deflagration.at(j), map::vide);
			}

			// Pop des bonus
			for( unsigned int j=0; j<e.listBlockDetruit.size(); j++ )
			{
				if( !c_map->getBlock(e.listBlockDetruit.at(j))->joueur->size() ){// One ne fait rien pop si il y a déjà quelqu'un à cette emplacement ou si il y a des flammes enemis
					bonus::t_Bonus tmp = bonus::getBonusAleatoire();
					if( tmp == bonus::__RIEN__ ){
						c_map->setBlock(e.listBlockDetruit.at(j), map::vide);
					}else{
						c_map->rmInfoJoueur(e.listBlockDetruit.at(j).x, e.listBlockDetruit.at(j).y);// Pour supprimer tout les meta-info
						c_map->setBlock(e.listBlockDetruit.at(j), map::bonus);
						c_map->ajouterInfoJoueur(e.listBlockDetruit.at(j).x,e.listBlockDetruit.at(j).y, (unsigned char)tmp, 1);
					}
				}
			}

			// Fin de l'event => out de la liste
			c_listEvent.erase(c_listEvent.begin()+i);

		}else{// ENCORE un tour

			// Création de la déflagration
			if( e.Nb_Repetition == 0 ){// On ne traite que le centre le 1er tour
				killPlayers(&e, e.pos.x, e.pos.y);// Adition des flammes avec le reste de l'environement
				c_map->setBlock(e.pos.x, e.pos.y, map::flamme_origine);
				e.deflagration.push_back( coordonneeConvert(e.pos.x, e.pos.y) );

				c_listEvent.at(i).Nb_Repetition++;
				c_listEvent.at(i).repetionSuivante = clock() + VITESSE_DEFLAGRATION_FLAMME;// * CLOCKS_PER_SEC;// Ajustement du temps

				continue;
			}

			// Flamme gauche
			if( e.continue_negativeX ){
				char r = actionSurLesElements( &e, e.pos.x-e.Nb_Repetition, e.pos.y, -1 );
				if( r == -1 || r == 0 )
					e.continue_negativeX = false;
				if( r == 0 || r == 1 )
					e.deflagration.push_back( coordonneeConvert(e.pos.x-e.Nb_Repetition, e.pos.y) );
			}

			// Flamme droite
			if( e.continue_X ){
				char r = actionSurLesElements( &e, e.pos.x+e.Nb_Repetition, e.pos.y, +1 );
				if( r == -1 || r == 0 )
					e.continue_X = false;
				if( r == 0 || r == 1 )
				e.deflagration.push_back( coordonneeConvert(e.pos.x+e.Nb_Repetition, e.pos.y) );
			}

			// Flamme haut
			if( e.continue_negativeY ){
				char r = actionSurLesElements( &e, e.pos.x, e.pos.y-e.Nb_Repetition, -2 );
				if( r == -1 || r == 0 )
					e.continue_negativeY = false;
				if( r == 0 || r == 1 )
				e.deflagration.push_back( coordonneeConvert(e.pos.x, e.pos.y-e.Nb_Repetition) );
			}

			// Flamme bas
			if( e.continue_Y ){
				char r = actionSurLesElements( &e, e.pos.x, e.pos.y+e.Nb_Repetition, +2 );
				if( r == -1 || r == 0 )
					e.continue_Y = false;
				if( r == 0 || r == 1 )
				e.deflagration.push_back( coordonneeConvert(e.pos.x, e.pos.y+e.Nb_Repetition) );
			}

			c_listEvent.at(i).Nb_Repetition++;
			c_listEvent.at(i).repetionSuivante = clock() + VITESSE_DEFLAGRATION_FLAMME;// * CLOCKS_PER_SEC;// Ajustement du temps
		}
	}
}



char partie::actionSurLesElements( s_Event* e, unsigned int x, unsigned int y, char direction )
{
	char continuer = 1;
	switch( killPlayers(e, x, y) )
	{
		case -1: {
			return -1;
		}
		case 0: {
			continuer = 0;
		}// Pas de break pour lire le block du dessous
		case 1: {
			switch( direction )
			{
				case -2:{// -Y
					if( e->pos.y+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_haut);
					}else{
						c_map->setBlock(x, y, map::flamme_vertical);
					}
					break;
				}
				case 2:{// +Y
					if( e->pos.y+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_bas);
					}else{
						c_map->setBlock(x, y, map::flamme_vertical);
					}
					break;
				}
				case -1:{// -X
					if( e->pos.x+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_gauche);
					}else{
						c_map->setBlock(x, y, map::flamme_horizontal);
					}
					break;
				}
				case 1:{// +X
					if( e->pos.x+(unsigned int)e->Nb_Repetition == e->Nb_Repetition_MAX || continuer == false ){// Si on a atteint la taille max
						c_map->setBlock(x, y, map::flamme_pointe_droite);
					}else{
						c_map->setBlock(x, y, map::flamme_horizontal);
					}
					break;
				}
			}
			c_map->ajouterInfoJoueur(x, y, e->joueur, 1);// On dit a qui appartient la flamme
			break;
		}
	}
	return continuer;
}


/***************************************************************************//*!
* @fn char partie::killPlayers( s_Event* e, unsigned int x, unsigned int y )
* @brief Tue les joueurs et détruit les block destructibles (Mode: F4A)
* @param[in,out] e L'event en cours
* @param[in] x Position X de la map a check
* @param[in] y Position Y de la map a check
* @return 3 Valeur de retoures possible<br />
*			- -1 On stop tout et on ne touche pas au block à cette adresse ! (Out of map ou Mur_INdestructible)
*			-  0 On stop tout mais on traite le block à l'adresse (Mur_destructible)
*			-  1 On continue
*
* @note Les valeurs de X et de Y sont comparées à la taille de la map => Pas de risque de sortir de la map
*/
char partie::killPlayers( s_Event* e, unsigned int x, unsigned int y )
{
	// k sert dans les boucle for comme compteur.
	// Peut être utilisée sans problème
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
			// On fait explosé la bombe
			c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements()->forceTimeOut(x,y);
			c_joueurs[c_map->getBlock(x, y)->joueur->at(1)-1].defArmements(0);
			return 0;
		}
		case map::bombe_poser_AVEC_plusieurs_joueurs: {// Les pauvres joueurs qui sont au millieu du chemin de la flamme -> dead
				for( k=1; k<c_map->getBlock(x, y)->joueur->size(); k++ )
				{
					c_joueurs[c_map->getBlock(x, y)->joueur->at(k)-1].defArmements(0);
				}
				// On fait explosé la bombe
				c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements()->forceTimeOut(x,y);
			return 0;
		}
		case map::bombe_poser: {
			if( !c_map->getBlock(x, y)->joueur )
				stdErrorE("Problème de pointeur vide ! c_map->getBlock(%u, %u)->joueur = 0", x, y);
			if( !c_map->getBlock(x, y)->joueur->size() )
				stdErrorE("Vector vide à x=%u, y=%u", x, y);

			// On fait explosé la bombe
			c_joueurs[c_map->getBlock(x, y)->joueur->at(0)-1].armements()->forceTimeOut(x,y);
			break;
			return 0;
		}
		case map::Mur_destructible: {
			//c_map->ajouterInfoJoueur(x, y, 0, 1);// On dit au block -> Now tu as été détruit et tu est dans une liste
			// Ajout du block à la liste des blocks qui ont été détruit
			e->listBlockDetruit.push_back( coordonneeConvert(x, y) );
			return 0;
		}
		case map::Mur_INdestructible: {
			return -1;
		}
		case map::flamme_pointe_haut:
		case map::flamme_pointe_bas:
		case map::flamme_pointe_droite:
		case map::flamme_pointe_gauche:{
			// On regarde si le block sur lequel on est n'est pas un block que l'on a détruit
			// Si c'est le cas, on regarde si c'est nous qui l'avons détruit
			//s_Coordonnees pos=coordonneeConvert( x, y );
			// On parcours la liste des blocks qui ont été détruit
			/*
			for( k=0; k<c_listBlockDetruit.size(); k++ )
				if(	coordonneeEgal(c_listBlockDetruit.at(k).pos, pos) ) // Si on trouve notre block
					if( c_listBlockDetruit.at(k).joueur == joueur )// Si on est à l'origine de la destruction du block => STOP
						return -1;
					//else // On est pas à l'origine de la destruction du block => on lui passe dessus ^^ et on continue la progression mortelle
			*/
			break;
		}
		case map::bonus: {
			if( c_map->getBlock(x, y)->joueur )
				c_map->getBlock(x, y)->joueur->clear();
			break;
		}
		default: {// On eject les autres cas
			break;
		}
	}

	return 1;
}


/***************************************************************************//*!
* @fn const char* partie::packIt( uint32_t X, uint32_t Y, map::t_type type, uint32_t nb_MetaDonnee )
* @brief Empaquette les variable X, Y, type, nb_MetaDonnee pour les envoyer sur le réseau
* @return Les variables formaté dans une chaine de caractère
*/
const char* partie::packIt( uint32_t X, uint32_t Y, map::t_type type, uint32_t nb_MetaDonnee )
{
	unsigned int PACK_size = snprintf(c_buffer, PACK_bufferSize, "%u,%u,%u,%u", X, Y, type, nb_MetaDonnee);
	if( PACK_size >= PACK_bufferSize )
		stdError("ATTENTION ! Perte de donnees ! %d donnees lues et %d donnees ecrites Maximum ( Ne pas oublier dans les calcules le \\0 ! )", PACK_size, PACK_bufferSize);

	return c_buffer;
}


/***************************************************************************//*!
* @fn void partie::unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type, uint32_t* nb_MetaDonnee )
* @brief Désempaquette le buffer et met les données dans les variables X, Y, type et nb_MetaDonnee
*/
void partie::unPackIt( uint32_t* X, uint32_t* Y, map::t_type* type, uint32_t* nb_MetaDonnee )
{
	unsigned int i=0;
	bool firstLap = 1;

	*X = 0;
	firstLap = 1;
	for( ; c_buffer[i] != ',' ; i++ )
	{
		if( firstLap ){
			firstLap = 0;
		}else{
			*X *= 10;
		}
		*X += c_buffer[i]-'0';
	}
	i++;// Pour virer la ,

	*Y = 0;
	firstLap = 1;
	for( ; c_buffer[i] != ','; i++ )
	{
		if( firstLap ){
			firstLap = 0;
		}else{
			*Y *= 10;
		}
		*Y += c_buffer[i]-'0';
	}
	i++;// Pour virer la ,

	*type = (map::t_type)0;
	firstLap = 1;
	for( ; c_buffer[i] != ','; i++ )
	{
		if( firstLap ){
			firstLap = 0;
		}else{
			*type = (map::t_type)(*type*10);
		}
		*type = (map::t_type)(*type+(c_buffer[i]-'0'));
	}
	i++;// Pour virer la ,

	*nb_MetaDonnee = 0;
	firstLap = 1;
	for( ; c_buffer[i] != 0; i++ )
	{
		if( firstLap ){
			firstLap = 0;
		}else{
			*nb_MetaDonnee *= 10;
		}
		*nb_MetaDonnee += c_buffer[i]-'0';
	}

	//sscanf(c_buffer, "%u,%u,%u,%u", X, Y, type, nb_MetaDonnee );
}


/***************************************************************************//*!
* @fn void partie::ajouterNouvelleConnection( SOCKET s )
* @brief Permet d'ajouter un nouveau joueur à la partie
*
* <b>Protocole</b> (Envoie des données)<br />
* Client: 0-1 0-1 (32bits ? Intel ?)<br />
* Server: 1-0 (Connection accordé ?)<br />
* Client: Nom Joueur<br />
* Server: 1-0 (Accordé ou non)<br />
* Server: {Envoie de la taille de la map}
* Server: {Envoie de la map} : "X, Y, type, nb_MetaDonnee"<br />
* Server: Si nb_MetaDonnee > 0 => Envoie des Meta-Donnees
* Server: "end" (Fin de transmission)
*<br />
* Si le perso du client bouge:<br />
* Client: 1 {action: haut, bas, droite, gauche, poser bombe, exploser} (note, 1=action)<br />
* Server: 0-1 (Accordé ou non)(Si accordé, alors le server fait l'action)<br />
* PAS d'envoie de nouvelle position, envoie effectué lors du rafraichissement.
*/
void partie::ajouterNouvelleConnection( SOCKET s )
{
	unsigned int	l_isIntel=1,
					l_is32Bits=1;
	unsigned char	i,
					idJoueur = 0;

	stdError("Nouvelle connection !");

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
	c_server->readClient( s, c_buffer, PACK_bufferSize );// On attend les données du client
	sscanf(c_buffer, "%u %u", &l_is32Bits, &l_isIntel );// Utilisation d'un buffer pour eviter tout problème de lecture

	stdError("Nouvelle connection p2 !");

	if( Is32Bits != l_is32Bits || l_isIntel != (int)baseClientServer::isLittleEndian() ){
		stdError("Attention ! Un client a essayé de se connecté avec les paramètres suivant: is32Bits=%u, isIntel=%u (Paramètre accèpté: is32Bits=%u, isIntel=%u)", l_is32Bits, l_isIntel, Is32Bits, (unsigned int)baseClientServer::isLittleEndian());
		c_server->rmClient( s );// Connection refusé
		return ;
	}

	stdError("Nouvelle connection p1 !");

	c_server->send_message( s, "1", 2);// Connection accèpté

	// On vide le buffer
	memset(c_buffer, 0, PACK_bufferSize);
	c_server->readClient( s, c_buffer, PACK_bufferSize );// On attend le nom du client

// COMMENTER CETTE LIGNE SI PAS UTILISATION telnet
//	c_buffer[strlen(c_buffer)] = 0;// Permet de virer le '\n'


	// On cherche un SLOT pour notre joueur
	// Et on vérifit si le nom n'est pas dans la liste
	for( i=0; i<c_nb_joueurs; i++ ){
		if( c_joueurs[i].isLocal() ){
			stdError("%s == %s", (const char*)c_joueurs[i].nom()->c_str(), c_buffer)
			if( *c_joueurs[i].nom() == c_buffer ){// Nom déjà utilisé
				c_server->rmClient( s );// Nom refusé => Déco
				return ;
			}
		}else{
			if( c_joueurs[i].socket() == INVALID_SOCKET ){// Note, on est obligé de trouver un slot libre, sinon le server n'aurait pas accèpté la connection
				idJoueur = i;
			}
		}
	}
	c_server->send_message( s, "1", 2);// Nom accèpté

	// Envoie des dim de la map
	c_server->send_message(s, packIt(c_map->X(), c_map->Y(), (map::t_type)0, 0), PACK_bufferSize);

	// Envoie de la map
	for( unsigned int x=0, y=0; y<c_map->Y(); y++ )
	{
		for( x=0; x< c_map->X(); x++ )
		{
			if( c_map->getBlock(x,y)->joueur ){
				c_server->send_message(s, packIt(x, y, c_map->getBlock(x,y)->element, c_map->getBlock(x,y)->joueur->size()), PACK_bufferSize);
				for( unsigned int k=0; k<c_map->getBlock(x,y)->joueur->size(); k++ )// Envoie du tableau de meta-données
					c_server->send_message(s, (const char*)(&c_map->getBlock(x,y)->joueur->at(k)), sizeof(uint8_t));
			}else{
				c_server->send_message(s, packIt(x, y, c_map->getBlock(x,y)->element, 0), PACK_bufferSize);
			}
		}
	}

	c_server->send_message( s, "end", 4);// Fin de transmition
}
