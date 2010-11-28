#ifndef BONUS_h
#define BONUS_h

#include "debug.h"
#include "coordonnees.h"
#include "outils.h"
#include <vector>
#include <time.h>

/*!
* @class bonus
* @brief G�re les bonus.
*
* Cette classe poss�de 2 couches de fonctionnement:
* -# La couche g�n�ral, qui regroupe les bonus disponibles pour la partie.<br />
*	(En commum avec toutes les instances de cette class)<br />
*	<b>A initialiser en 1er !via bonus::bonusProp()</b><br />
*	> Cette couche contient les probabilit�s de chaque objets et la quantit� Maximum ramassables par joueur.
* -# La couche instanciable, unique a chaque instance.<br />
*	> Cette couche contient la quatit� max en stock et la quantit� utilisable.
*
* @todo Meilleur syst�me de gestion des bonus
*
* @section aide Aide sur les variables
* Mais c'est quoi la diff�rence entre: quantite_utilisable, quantite_MAX_en_stock, quantite_MAX_Ramassable, probabiliter_pop ?<br />
*
* Prenons un exemple:<br />
*	- Soit un perso bomberman.
*	- Le perso est capable de porter 10 bombes s'il y a le sac ad�quate. ( quantite_MAX_Ramassable = 10 )
*	- Notre perso a un sac pouvant contenir 5 bombes maximum. ( quantite_MAX_en_stock = 5 )
*	- Le sac contient actuellement 3 bombes.( quantite_utilisable = 3)
*	- J'ai donc 2 bombes pos�es. Ces 2 bombes sont dans la variable @link bonus::c_listEvent c_listEvent@endlink en attandant qu'elles explosent ^^
*/
class bonus
{
	public:
		/*!
		* @enum t_Bonus
		* @brief Les types de bonus possible dans le jeu
		*
		* @attention En cas de modification du nombre d'element, modifier aussi
		* le define NB_ELEMENT_t_Bonus
		*/
		#define NB_ELEMENT_t_Bonus 5
		enum t_Bonus {
			bombe=0,
			puissance_flamme,
			declancheur,
			vitesse,
			vie
		};

		/*!
		* @struct s_bonus_proprieter
		* @brief Cette structure accueil la propri�t� des bonus
		*/
		typedef struct {
			//t_Bonus type; // <- Le type est stock� dans le num�ro du tableau [0] -> [t_Bonus::bombe]
			unsigned char probabiliter_pop;//!< Pourcentage q'un bonus pop (valeur: entre 0 et 100 au max)
			unsigned char quantite_MAX_Ramassable;//!< Quantit� max qu'un joueur peut avoir d'un objet
			/**
			* @var duree
			* La dur�e peut avoir 3 status<br />
			* - -1 => Pas de callback
			* - +0 => Callback direct
			* - >0 => Callback au bout de X secs
			*/
			int duree;// Dur�e d'utilisation
		} s_bonus_proprieter;

	private:
		/*!
		* @struct s_bonus
		* @brief Cette sturcture accueil un bonus d'un joueur.
		*
		* ( un tableau de cette structure forme l'ensemble des bonus d'un joueur )
		* quantite_utilisable: Nombre d'objet poss�d� actuellement et utilisable
		* quantite_MAX_en_stock: Quantit� Maxi d'un objet ( ex: 5 bonus de type bombe => 5 Bombes max en stock ! )
		*/
		typedef struct {
			t_Bonus type;//!< Le bonus
			unsigned char quantite_utilisable;//!< Nombre d'objet poss�d� actuellement et utilisable
			unsigned char quantite_MAX_en_stock;//!< Quantit� Maxi d'un objet
		} s_bonus;

		/*!
		* @struct s_Event
		* @brief Cette sturcture accueil les Events pos�es
		*
		* Exemple: C'est par elle que l'on sait quand une bombe explose.<br />
		* Elle fourni, le O�  et Quand.<br />
		*/
		typedef struct {
			t_Bonus type;//!< Le bonus
			s_Coordonnees pos;//!< Position de la bombe pos�
			clock_t finEvent;//!< Temps avant la fin de l'event
		} s_Event;

		// Varaible G�n�rale ( Variables global � toutes les class )
		static s_bonus_proprieter *C_bonusProp;//!< Liste des bonus charg� et utilisables sur la map ( cette var n'est remplis q'une fois ! (static) )
		// Varaibles Local � la class ( � l'instance de chaque class )
		s_bonus* c_liste;//!< Liste des bonus attrap� par un player. ( bonus utilisable )
		unsigned char c_nb;//!< Nombre de bonus obtenu par un player
		std::vector<s_Event> c_listEvent;//!< Tableau contenant les bombes pos�es


	public:
		bonus();// Constructeur
		~bonus();// Destructeur
		static s_bonus_proprieter* bonusProp();// Constructeur pour C_bonusProp
		static void unInitBonusProp();// Destructeur pour inclut C_bonusProp

		// Accesseurs
		bool aDeja( t_Bonus b ) const;// Permet de savoir si un perso a un bonus ( existe et non null )
		int est_Dans_La_Liste( t_Bonus b ) const;// Permet de savoir si un bonus est d�j� dans la liste
		unsigned char quantiteUtilisable( t_Bonus b ) const;
		unsigned char quantiteMAX( t_Bonus b ) const;
		unsigned char quantiteMAX_Ramassable( t_Bonus b ) const;
		unsigned char probabiliter( t_Bonus b ) const;

		// Modificateurs
		bool incQuantiteUtilisable( t_Bonus b );// Incr�menter quantite_utilisable
		bool incQuantiteMAX_en_stock( t_Bonus b );// Incr�menter quantite_MAX_en_stock
		bool decQuantiteUtilisable( t_Bonus b );// D�cr�menter quantite_utilisable
		bool decQuantiteUtilisable( t_Bonus b, unsigned int x, unsigned int y );
		inline bool decQuantiteUtilisable( t_Bonus b, s_Coordonnees pos );
		bool decQuantiteMAX_en_stock( t_Bonus b );// D�cr�menter quantite_MAX_en_stock
		void param_Par_Defaut();
		void defQuantiteUtilisable( t_Bonus b, unsigned char quantite_utilisable );
		void defQuantiteMAX( t_Bonus b, unsigned char quantite_MAX_en_stock );
		static void defQuantiteMAX_Ramassable( t_Bonus b, unsigned char quantite_MAX_Ramassable );
		static void defProbabiliter( t_Bonus b, unsigned char probabiliter_pop );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop );

		// Autres
		bool isEvent( s_Coordonnees* pos );
		static t_Bonus getBonusAleatoire();
};

#endif
