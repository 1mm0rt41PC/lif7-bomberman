#ifndef MAP_h
#define MAP_h

#include <stdlib.h>// Pour fopen, fread
#include <vector>
#include <string>
#include <dirent.h>
#include "coordonnees.h"
#include "bonus.h"
#include "outils.h"

/*!
* @class map
* @brief G�re une map.
* @todo Coder les fonctions enleverAuBlock(), ajouterAuBlock()<br />
* - PASSER std::vector<unsigned char>* joueur; -> std::vector<unsigned char> joueur; ???<br />
* - PROBLEME : c'est la map o� la partie qui defini quand on meure ?<br />
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
			Mur_destructible,
			Mur_INdestructible,
			// Flammes
			flamme_horizontal,
			flamme_vertical,
			flamme_origine,
			flamme_pointe_haut,
			flamme_pointe_bas,
			flamme_pointe_droite,
			flamme_pointe_gauche,
			// Armes
			bombe_poser,
			// Bonus
			bonus,
			//gain_bombe,
			//gain_puissance_flamme,
			//gain_declancheur_manuel,
			// Player
			UN_joueur,
			plusieurs_joueurs,
			// Combinaison
			bombe_poser_AVEC_UN_joueur,
			bombe_poser_AVEC_plusieurs_joueurs
			// Points de d�part
			//point_de_depart_j1,
			//point_de_depart_j2,
			//point_de_depart_j3,
			//point_de_depart_j4
		};


		/*!
		* @enum t_direction
		* @brief Les directions posssibles pour un personnages ou une flamme
		*/
		enum t_direction {
			DIRECT_haut,
			DIRECT_bas,
			DIRECT_droite,
			DIRECT_gauche,
			DIRECT_vertical,
			DIRECT_sans_direction
		};

		/*!
		* @struct s_Case
		* Exemple:<br />
		* - flamme � QUI ? => La liste contient : le num du joueur qui a provoqu� ces flammes<br />
		* - bombe_poser � QUI ? => La liste contient : le num du joueur qui a pos� la bombe<br />
		* - UN_joueur QUI ? => La liste contient : le num du joueur qui est � cette endroit<br />
		* - plusieurs_joueurs QUI ... ? => La liste contient : les num des joueurs qui sont � cette endroit<br />
		* - bombe_poser_AVEC_UN_joueur QUI et QUI ? => La liste contient : (dans l'ordre !)<br />
		*		1) Le num du joueur a qui appartient la bombe.<br />
		*		2) Le num du joueur qui est � cette position<br />
		* - bombe_poser_AVEC_plusieurs_joueurs QUI et QUI ... ? => La liste contient : (dans l'ordre !)<br />
		*		1) Le num du joueur a qui appartient la bombe.<br />
		*		2 � ...) Les num des joueurs qui sont � cette position.<br />
		*/
		typedef struct {
			std::vector<unsigned char>* joueur;//!< La liste est ici pour obtenir des info sur le perso � l'origine du type t.
			t_type element;//!< type d'element.
		} s_Case;


	// Variables
	private:
		// struct map {
		std::vector<s_Coordonnees> c_listDesChangement;//!< Les changement qui ont eu lieu sur la map
		s_Coordonnees c_taille;//!< Taille de la map
		s_Case *c_block;//!< Tableau de case => la map en ent�re
		s_Coordonnees *c_PointDeDepartJoueur;//!< Liste des points de d�part des joueurs ({point_de_depart_jXXX} numJ)
		unsigned char c_nb_PointDeDepartJoueur;//!< Le nombre de points de d�part => le nombre maximum de joueur sur une map
		//}


	// Fonctions
	public:
		// Modificateurs
		map();
		map( unsigned int tailleX, unsigned int tailleY );
		~map();
		void setBlock( unsigned int X, unsigned int Y, map::t_type what );
		void setBlock( s_Coordonnees& pos, map::t_type what );
		void ajouterInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierePosition=0 );
		void rmInfoJoueur( unsigned int X, unsigned int Y, unsigned char id_Joueur, bool premierEltInclu );
		void rmInfoJoueur( unsigned int X, unsigned int Y );
		void setTaille( unsigned int tailleX, unsigned int tailleY );
		int chargerMap( const char fichier[]=0 );
		s_Coordonnees mettreJoueurA_sa_PositionInitial( unsigned char joueur );

		// Accesseurs
		const s_Case* getBlock( unsigned int X, unsigned int Y ) const;
		const s_Case* getBlock( s_Coordonnees& pos ) const;
		unsigned int X() const;
		unsigned int Y() const;
		s_Coordonnees positionInitialJoueur( unsigned char joueur ) const;
		unsigned char nb_PointDeDepartJoueur() const;
		unsigned char nb_MAX_Joueur() const;
		unsigned int nb_InfoJoueur( unsigned int X, unsigned int Y ) const;
		bool getModification( s_Coordonnees& pos);

		// Autres
		static void readDir( std::vector<std::string>* files );
};

// Ajout des fonctions inline
#include "map.inl"

#endif
