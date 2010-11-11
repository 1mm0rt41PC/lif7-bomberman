#ifndef BONUS_h
#define BONUS_h

#include "debug.h"
#include <time.h>

/*!
* @class bonus
* @brief Gère les bonus.
*
* Cette classe possède 2 couches de fonctionnement:
* -# La couche général, qui regroupe les bonus disponibles pour la partie.<br />
* (En commum avec toutes les instances de cette class)
* <b>A initialiser en 1er !via bonus::bonusProp()</b>
* -# La couche instanciable, unique a chaque instance.
*
* @todo Meilleur système de gestion des bonus
*/
class bonus
{
	public:
		/*!
		* @enum t_Bonus
		* @brief Les types de bonus possible dans le jeu
		*/
		enum t_Bonus{
			bombe=0,
			puissance_flamme,
			declancheur,
			vitesse,
			vie
		};

		/*!
		* @struct s_bonus_proprieter
		* @brief Cette structure accueil la propriété des bonus
		* @var probabiliter_pop	Pourcentage q'un bonus pop (valeur: entre 0 et 100 au max)
		* @var probabiliter_pop	Pourcentage q'un bonus pop (valeur: entre 0 et 100 au max)
		*/
		typedef struct {
			//t_Bonus type; // <- Le type est stocké dans le numéro du tableau [0] -> [t_Bonus::bombe]
			unsigned char probabiliter_pop;
			unsigned char quantite_MAX_Ramassable;
			/*
			* La durée peut avoir 3 status
			* -1 => L'objet est utilisé sans limite de temps (Durée Illimité)
			*  0 => L'objet est utilisé directement (Durée instantané)
			* >0 => L'objet est utilisé au bout de "XXX" (valeur dans la variable)
			*/
			int duree;
		} s_bonus_proprieter;

	private:
		// Cette sturcture accueil un bonus d'un joueur ( un tableau de cette structure forme l'ensemble des bonus d'un joueur )
		/*!
		* @struct s_bonus
		* @brief Cette sturcture accueil un bonus d'un joueur.
		*
		* ( un tableau de cette structure forme l'ensemble des bonus d'un joueur )
		* quantite_utilisable: Nombre d'objet possédé actuellement et utilisable
		* quantite_MAX_en_stock: Quantité Maxi d'un objet ( ex: 5 bonus de type bombe => 5 Bombes max en stock ! )
		*/
		typedef struct {
			t_Bonus type;
			unsigned char quantite_utilisable;// Nombre d'objet possédé actuellement et utilisable
			unsigned char quantite_MAX_en_stock;// Quantité Maxi d'un objet
		} s_bonus;

		static s_bonus_proprieter *C_bonusProp;// Liste des bonus chargé et utilisables sur la map ( cette var n'est remplis q'une fois ! (static) )
		s_bonus* c_liste;// Liste des bonus attrapé par un player. ( bonus utilisable )
		unsigned char c_nb;// Nombre de bonus obtenu par un player

		static clock_t calcFinEvent();


	public:
		bonus();// Constructeur
		~bonus();// Destructeur
		static s_bonus_proprieter* bonusProp();// Constructeur pour C_bonusProp
		static void unInitBonusProp();// Destructeur pour inclut C_bonusProp

		// Accesseurs
		bool aDeja( t_Bonus b ) const;// Permet de savoir si un perso a un bonus ( existe et non null )
		int est_Dans_La_Liste( t_Bonus b ) const;// Permet de savoir si un bonus est déjà dans la liste
		unsigned char quantiteUtilisable( t_Bonus b ) const;
		unsigned char quantiteMAX( t_Bonus b ) const;
		unsigned char quantiteMAX_Ramassable( t_Bonus b ) const;
		unsigned char probabiliter( t_Bonus b ) const;

		// Modificateurs
		bool incQuantiteUtilisable( t_Bonus b );// Incrémenter quantite_utilisable
		bool incQuantiteMAX_en_stock( t_Bonus b );// Incrémenter quantite_MAX_en_stock
		bool decQuantiteUtilisable( t_Bonus b );// Décrémenter quantite_utilisable
		bool decQuantiteMAX_en_stock( t_Bonus b );// Décrémenter quantite_MAX_en_stock
		void param_Par_Defaut();
		void defQuantiteUtilisable( t_Bonus b, unsigned char quantite_utilisable );
		void defQuantiteMAX( t_Bonus b, unsigned char quantite_MAX_en_stock );
		void defQuantiteMAX_Ramassable( t_Bonus b, unsigned char quantite_MAX_Ramassable );
		void defProbabiliter( t_Bonus b, unsigned char probabiliter_pop );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock );
		void defBonus( t_Bonus b, unsigned char quantite_utilisable, unsigned char quantite_MAX_en_stock, unsigned char quantite_MAX_Ramassable, unsigned char probabiliter_pop );
};

#endif
