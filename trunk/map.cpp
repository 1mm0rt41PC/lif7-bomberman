#include "map.h"

using namespace std;

/*******************************************************************************
* Constructeur
*/
map::map()
{
	c_block = 0;
	c_taille.x = 0;
	c_taille.y = 0;
	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/***************************************************************************//*!
* @fn map::map( unsigned int tailleX, unsigned int tailleY )
* @brief Constructeur :: Cr�er une map vide !
*/
map::map( unsigned int tailleX, unsigned int tailleY )
{
	c_taille.x = tailleX;
	c_taille.y = tailleY;
	c_block = new s_Case[ tailleX * tailleY ];

	for( unsigned int i=0; i<tailleX*tailleY; i++ )
	{
		c_block[i].element = map::vide;
		c_block[i].joueur = 0;
	}

	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/*******************************************************************************
* Destructeur
*/
map::~map()
{
	// Suppression de la map si elle existe
	if( c_block ){
		// Suppression des meta donn�es
		for( unsigned int i=0; i<c_taille.x*c_taille.y; i++ )
			delete c_block[i].joueur;
		delete[] c_block;
	}

	// Suppression des points de d�part s'ils existent
	if( c_PointDeDepartJoueur )
		delete[] c_PointDeDepartJoueur;

	c_block = 0;
	c_taille.x = 0;
	c_taille.y = 0;
	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/***************************************************************************//*!
* @fn void map::setBlock( unsigned int X, unsigned int Y, map::t_type what, unsigned char id_du_JoueurQuiAAjouterWhat )
* @brief Modifie le block qui est � la position X, Y
*
* Si l'id du joueur==0 => on n'enregistre pas les meta-donn�es<br />
* <b>/!\\ATTENTION: si what = vide => rmInfoJoueur( X, Y, id_du_JoueurQuiAAjouterWhat, 0 ) /!\\</b>
*/
void map::setBlock( unsigned int X, unsigned int Y, map::t_type what, unsigned char id_du_JoueurQuiAAjouterWhat )
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorVarE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, what=%d, id_du_JoueurQuiAAjouterWhat=%u", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, what, id_du_JoueurQuiAAjouterWhat);

	c_block[X * c_taille.x + Y].element = what;

	switch( what )
	{
		case vide:{
			rmInfoJoueur(X, Y, id_du_JoueurQuiAAjouterWhat, 1);
			return ;
		}
		case UN_joueur:{
			if( id_du_JoueurQuiAAjouterWhat == 0 )
				return ;

			if( !c_block[X * c_taille.x + Y].joueur )
				c_block[X * c_taille.x + Y].joueur = new vector<unsigned char>;

			c_block[X * c_taille.x + Y].joueur->push_back( id_du_JoueurQuiAAjouterWhat );
			return ;
		}
		default:{
			stdErrorVarE("Cas non g�r� par setBlock() what=%d", (int)what);
		}
	}
}


/***************************************************************************//*!
* @fn void map::rmInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierEltInclu )
* @brief Supprime les info du joueur du block (X, Y)
* @param[in] premierEltInclu Si le premier elemnt dela liste est l'id du joueur => SUPPRIMER ?
*
* NOTE: Pour certain map::type comme bombe_poser, le 1er element repr�sente l'id du joueur qui a pos� l'objet
*/
void map::rmInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierEltInclu )
{
	vector<unsigned char>* vct = NULL;

	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorVarE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, id_Joueur=%u, premierEltInclu=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, (unsigned int)id_Joueur, (int)premierEltInclu);

	if( id_Joueur == 0 )
		stdErrorVarE("ID de joueur incorrect ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, id_Joueur=%u, premierEltInclu=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, (unsigned int)id_Joueur, (int)premierEltInclu);

	if( !(vct = c_block[X * c_taille.x + Y].joueur) )
		stdErrorVarE("Pas de meta donn�e pour le block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, id_Joueur=%u, premierEltInclu=%d", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y, (unsigned int)id_Joueur, (int)premierEltInclu);

	for( unsigned int i=!premierEltInclu; i<vct->size(); i++ )
	{
		if(vct->at(i) == id_Joueur)
			vct->erase(vct->begin()+i);
	}
}


/***************************************************************************//*!
* @fn void map::setTaille( unsigned int tailleX, unsigned int tailleY )
* @brief Modifie la taille de la map.
*
* Si une map existe d�j� => delete de l'ancienne et delete des points de d�part
*/
void map::setTaille( unsigned int tailleX, unsigned int tailleY )
{
	// Suppression de la map si elle existe
	if( c_block ){
		// Suppression des meta donn�es
		for( unsigned int i=0; i<c_taille.x*c_taille.y; i++ )
			delete c_block[i].joueur;
		delete[] c_block;
	}

	// Suppression des points de d�part s'ils existent
	if( c_PointDeDepartJoueur )
		delete[] c_PointDeDepartJoueur;


	c_taille.x = tailleX;
	c_taille.y = tailleY;
	c_block = new s_Case[ tailleX * tailleY ];

	for( unsigned int i=0; i<tailleX*tailleY; i++ )
	{
		c_block[i].element = map::vide;
		c_block[i].joueur = 0;
	}

	c_PointDeDepartJoueur = 0;
	c_nb_PointDeDepartJoueur = 0;
}


/***************************************************************************//*!
* @fn int map::chargerMap( const char fichier[] )
* @brief Charge une map en m�moire
* @param[in] fichier	Si = 0 => map al�atoire
* @return Renvoie le nombre de joueur maximum pour cette carte
*
* Si la map existe d�j� => ancienne map out<br />
* Un fichier de map a une forme :<br />
* X Y NB_Joueur\n<br />
* #...#\n<br />
*/
int map::chargerMap( const char fichier[] )
{
	char *fichierUtiliser = (char*)fichier;
	FILE* fp=0;
	char c=0;

	// Si pas de nom de fichier donn�e => map al�atoire
	if( !fichier ){
		char fichier_bis[20]={0};
		sprintf(fichier_bis, "lvl%d.map", myRand(1, NB_MAP));
		fichierUtiliser = fichier_bis;
	}

	if( !(fp=fopen(fichierUtiliser, "r")) )
		stdErrorVarE("Erreur lors de l'ouverture de la map <%s>", fichierUtiliser);

	if( fscanf(fp, "%2u %2u %2u\n", &c_taille.x, &c_taille.y, (unsigned int*)&c_nb_PointDeDepartJoueur) != 3 ){
		fclose(fp);
		stdErrorVarE("Erreur lors de la lecture du fichier <%s>", fichierUtiliser);
	}

	// Bonne dimension ?
	if( !c_taille.x || !c_taille.y )
		stdErrorVarE("Les dimensions de la map sont incorrect ! c_taille=(%u,%u)", c_taille.x, c_taille.y);

	// Nombre de joueur correct
	if( !c_nb_PointDeDepartJoueur )
		stdErrorVarE("Le nombre de joueur pour la map ( ou nombre de point de d�part ) est incorrect ! c_nb_PointDeDepartJoueur=%u", c_nb_PointDeDepartJoueur);

	// Allocation de la map
	if( !(c_block = new s_Case[ c_taille.x * c_taille.y ]) )
		stdErrorVarE("Erreur lors de l'allocation de la m�moire ! c_taille=(%u,%u)", c_taille.x, c_taille.y);

	// Allocation des points de d�part
	if( !(c_PointDeDepartJoueur = new s_Pos[ c_nb_PointDeDepartJoueur ]) )
		stdErrorVarE("Erreur lors de l'allocation de la m�moire ! c_taille=(%u,%u)", c_taille.x, c_taille.y);


	for( unsigned int y=0,x; y<c_taille.y; y++ )
	{
		for( x=0; x<c_taille.x; x++ )
		{
			switch( c=fgetc(fp) )
			{
				case EOF:{
					stdErrorVarE("Fin de fichier pr�matur� ! x=%u, y=%u, c_taille=(%u,%u), <%s>", x, y, c_taille.x, c_taille.y, fichierUtiliser);
					break;
				}
				case '1':
				case '2':
				case '3':
				case '4':{
					c=c-'0';// Convertion en nombre manipulable

					// On v�rif que l'on a pas un joueur qui soit en dehors de ce que l'on a r�serv�
					if( c > c_nb_PointDeDepartJoueur )
						stdErrorVarE("Joueur inconnu dans la map <%s> ! x=%u, y=%u, joueur=%d", fichierUtiliser, x, y, (int)c);

					c_PointDeDepartJoueur[c-1].x = x;
					c_PointDeDepartJoueur[c-1].y = y;
					c_block[x * c_taille.x + y].element = vide;
					break;
				}
				case ' ':{
					c_block[x * c_taille.x + y].element = vide;
					break;
				}
				case '#':{
					c_block[x * c_taille.x + y].element = Mur_INdestrucible;
					break;
				}
				case '%':{
					c_block[x * c_taille.x + y].element = Mur_destrucible;
					break;
				}
				default:{
					stdErrorVarE("Caract�re inatendu dans le fichier <%s> : (code)%d, (aff)%c, x=%u, y=%u", fichierUtiliser, (int)c, c, x, y);
					break;
				}
			}
			c_block[x * c_taille.x + y].joueur = 0;
		}
		fgetc(fp);// Pour virer le \n de la fin
	}

	fclose(fp);

	return c_nb_PointDeDepartJoueur;
}


/***************************************************************************//*!
* @fn map::s_Pos map::mettreJoueurA_sa_PositionInitial( unsigned char joueur )
* @brief Met le perso {joueur} a sa place initial
* @param[in] joueur	Joueur que l'on veut d�placer � sa position initial ( un nombre entre 1 et 255 )
* @return Renvoie la position initial du joueur.
*
* <b>Vous devez vous m�me supprimer le perso de son ancienne position, si il y en avait une</b><br />
* Si le joueur n'est pas trouv� => Erreur FATAL !
* @todo
*	- Permettre le positionnement � la place init au millieu d'une partie<br />
*	- V�rification de ce qui a � la position initial avant de positionner le perso
*/
map::s_Pos map::mettreJoueurA_sa_PositionInitial( unsigned char joueur )
{
	if( joueur == 0 || joueur > c_nb_PointDeDepartJoueur )
		stdErrorVarE("Le num�ro de joueur Doit �tre entre 1 et %d ! mettreJoueurA_sa_PositionInitial( %d )", (int)joueur, (int)c_nb_PointDeDepartJoueur);

	if( !c_block || c_PointDeDepartJoueur[joueur-1].x >= c_taille.x || c_PointDeDepartJoueur[joueur-1].y >= c_taille.y )
		stdErrorVarE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u, Joueur=%u", (unsigned int)c_block, c_taille.x, c_taille.y, c_PointDeDepartJoueur[joueur-1].x, c_PointDeDepartJoueur[joueur-1].y, (unsigned int)joueur);

	s_Case* c = &c_block[ c_PointDeDepartJoueur[joueur-1].x * c_taille.x + c_PointDeDepartJoueur[joueur-1].y ];
	c->element = UN_joueur;
	if( !c->joueur )
		c->joueur = new vector<unsigned char>;
	c->joueur->push_back( joueur );

	return c_PointDeDepartJoueur[joueur-1];
}


/***************************************************************************//*!
* @fn map::s_Case map::getBlock( unsigned int X, unsigned int Y ) const
* @brief Retourn le block qui est � la position X, Y
*/
map::s_Case map::getBlock( unsigned int X, unsigned int Y ) const
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorVarE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y);

	return c_block[X * c_taille.x + Y];
}


/***************************************************************************//*!
* @fn unsigned int map::X() const
* @brief Renvoie la taille X de la map
*/
unsigned int map::X() const
{
	return c_taille.x;
}


/***************************************************************************//*!
* @fn unsigned int map::Y() const
* @brief Renvoie la taille Y de la map
*/
unsigned int map::Y() const
{
	return c_taille.y;
}


/***************************************************************************//*!
* @fn map::s_Pos map::positionInitialJoueur( unsigned char joueur ) const
* @brief Renvoie la position initial du joueur.
* @param[in] joueur	Joueur dont on veut avoir sa position initial ( un nombre entre 1 et 255 )
*
* Si une position initial n'est pas trouv� pour le joueur => Erreur FATAL !
*/
map::s_Pos map::positionInitialJoueur( unsigned char joueur ) const
{
	if( joueur == 0 )
		stdErrorE("positionInitialJoueur( 0 ) INTERDIT !");

	if( c_PointDeDepartJoueur )
		stdErrorE("Charger une map correct avant d'appeler positionInitialJoueur() ! Il n'existe aucun point de d�part !");

	if( joueur > c_nb_PointDeDepartJoueur )
		stdErrorVarE("Le joueur demand� ne peut �tre positionn� sur cette map ! c_nb_PointDeDepartJoueur=%u, joueur=%u", (unsigned int)c_nb_PointDeDepartJoueur, (unsigned int)joueur);

	return c_PointDeDepartJoueur[joueur-1];
}


/***************************************************************************//*!
* @fn unsigned char map::nb_PointDeDepartJoueur() const
* @brief Renvoie le nombre de joueur maximum sur la carte en cours.
*/
unsigned char map::nb_PointDeDepartJoueur() const
{
	return c_nb_PointDeDepartJoueur;
}


/***************************************************************************//*!
* @fn unsigned int map::getNbJoueur_SurBlock( unsigned int X, unsigned int Y ) const
* @brief Renvoie le nombre de joueurs dans le block (X, Y)
*/
unsigned int map::getNbJoueur_SurBlock( unsigned int X, unsigned int Y ) const
{
	if( !c_block || X >= c_taille.x || Y >= c_taille.y )
		stdErrorVarE("Impossible d'acc�der au block demand� ! c_block=%X, c_taille=(%u,%u), X=%u, Y=%u", (unsigned int)c_block, c_taille.x, c_taille.y, X, Y);

	s_Case* c = &c_block[X * c_taille.x + Y];

	if( !c->joueur || !c->joueur->size() )
		return 0;

	switch( c->element )
	{
		case UN_joueur:
		case plusieurs_joueurs:{
			return c->joueur->size();
		}
		case vide:{
			stdErrorVarE("Syst�me erron� ! Sur une case vide, il y a %u joueur resens�", c->joueur->size());
		}
		case bombe_poser_AVEC_UN_joueur:
		case bombe_poser_AVEC_plusieurs_joueurs:{
			return c->joueur->size()-1;
		}
		default:{
			stdErrorVarE("getNbJoueur_SurBlock() :: Cas non trait� ! c->element=%d", (int)c->element);
		}
	}
}


/***************************************************************************//*!
* @fn int map::myRand( int a, int b )
* @brief Cr�er un nombre al�atoire compris entre a et b
*/
int map::myRand( int a, int b )
{
	double r;

	r = (double) rand() / RAND_MAX;
	r *= (double) b+1.-a;
	r += (double) a;
	return (int) r;
}
