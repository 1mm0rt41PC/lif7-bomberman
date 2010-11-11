#ifndef MAP_h
#define MAP_h

#include "debug.h"
#include <stdlib.h>// Pour fopen, fread
#include <vector>


/*!
* @def NB_MAP
* Le nombre de map dans le dossier<br />
* Les noms de map sont du format : lvl<b/>XXX</b>.map avec XXX = [1 à +oo]
*/
#define NB_MAP 1


/*!
* @class map
* @brief Gère une map.
* @todo Coder les fonctions enleverAuBlock(), ajouterAuBlock()<br />
* PASSER std::vector<unsigned char>* joueur; -> std::vector<unsigned char> joueur; ???<br />
* PROBLEME : c'est la map où la partie qui defini quand on meure ?<br />
* Une partie = map + play dessus + style de jeu<br />
* MAIS<br />
* une map c'est un ensemble de block avec des infos dont celle des joueurs<br />
* Surcharger la map ? map(virtual)+map_F4A || map(virtual)+map_CTF
*/
class map
{
	public:
		/*!
		* @enum t_type
		* @brief Les types de d'objet sur la map
		*
		* Aller voir la def dans le .h
		*/
		// A quoi correspond un block
		enum t_type {
			inconnu,// BUG
			vide,
			// MUR
			Mur_destrucible,
			Mur_INdestrucible,
			// Armes
			flamme,
			flamme_origine,
			bombe_poser,
			// Bonus
			gain_bombe,
			gain_puissance_flamme,
			gain_declancheur_manuel,
			// Player
			UN_joueur,
			plusieurs_joueurs,
			// Combinaison
			bombe_poser_AVEC_UN_joueur,
			bombe_poser_AVEC_plusieurs_joueurs,
			// Points de départ
			point_de_depart_j1,
			point_de_depart_j2,
			point_de_depart_j3,
			point_de_depart_j4
		};

		/*!
		* @enum s_Pos
		* @brief struct de coordonnées ( x,y )
		*/
		typedef struct {
			unsigned int	x,
							y;
		} s_Pos;

		/*!
		* @struct s_Case
		* @var t					type d'element.
		* @var list<int>* joueur	La liste est ici pour obtenir des info sur le perso à l'origine du type t.
		* Exemple:
		* flamme à QUI ? => La liste contient : le num du joueur qui a provoqué ces flammes
		* bombe_poser à QUI ? => La liste contient : le num du joueur qui a posé la bombe
		* UN_joueur QUI ? => La liste contient : le num du joueur qui est à cette endroit
		* plusieurs_joueurs QUI ... ? => La liste contient : les num des joueurs qui sont à cette endroit
		* bombe_poser_AVEC_UN_joueur QUI et QUI ? => La liste contient : (dans l'ordre !)
		*		1) Le num du joueur a qui appartient la bombe.
		*		2) Le num du joueur qui est à cette position
		* bombe_poser_AVEC_plusieurs_joueurs QUI et QUI ... ? => La liste contient : (dans l'ordre !)
		*		1) Le num du joueur a qui appartient la bombe.
		*		2 à ...) Les num des joueurs qui sont à cette position.
		*/
		typedef struct {
			t_type element;
			std::vector<unsigned char>* joueur;
		} s_Case;

	// Variables
	private:
		// struct map {
		s_Case *c_block;
		s_Pos c_taille;
		s_Pos *c_PointDeDepartJoueur;// {point_de_depart_jXXX} numJ
		unsigned char c_nb_PointDeDepartJoueur;
		//}


	// Fonctions
	public:
		// Modificateurs
		map();
		map( unsigned int tailleX, unsigned int tailleY );
		~map();
		void setBlock( unsigned int X, unsigned int Y, map::t_type what, unsigned char id_du_JoueurQuiAAjouterWhat );
		void rmInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierEltInclu );
		void setTaille( unsigned int tailleX, unsigned int tailleY );
		int chargerMap( const char fichier[]=0 );
		s_Pos mettreJoueurA_sa_PositionInitial( unsigned char joueur );

		// Accesseurs
		s_Case getBlock( unsigned int X, unsigned int Y ) const;
		unsigned int X() const;
		unsigned int Y() const;
		s_Pos positionInitialJoueur( unsigned char joueur ) const;
		unsigned char nb_PointDeDepartJoueur() const;
		inline unsigned char nb_MAX_Joueur() { return nb_PointDeDepartJoueur(); }
		unsigned int getNbJoueur_SurBlock( unsigned int X, unsigned int Y ) const;
		void enleverAuBlock( unsigned int X, unsigned int Y, map::t_type what, unsigned char id_du_JoueurQuiAAjouterWhat );
		void ajouterAuBlock( unsigned int X, unsigned int Y, map::t_type what, unsigned char id_du_JoueurQuiAAjouterWhat );

		// Autres
		static int myRand(int a, int b);
};

#endif

