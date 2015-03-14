/***************************************************************************//*!
* @brief Fonction de calcule Factorielle.
* @param[in] N Le nombre a calculer
*
* Utilisation:
* @code
* printf("Factoriel de 5 %d", Fact<5>::value);
* @endcode
* @note Calculs effectués à la compilation
* @warning NE PAS METTRE DE VALEUR INCONNUE !
*/
template <unsigned int N> struct Fact
{
	enum { value = N * Fact<N-1>::value };
};
template <> struct Fact<0>
{
	enum { value = 1 };
};


/***************************************************************************//*!
* @brief Fonction de calcule un nombre (N) à Puissance (puiss).
* @param[in] N Le nombre a calculer
* @param[in] puiss La puissance
*
* Utilisation:
* @code
* printf("5 au carré = %d", Pow<5,2>::value);
* @endcode
* @note Calculs effectués à la compilation
* @warning NE PAS METTRE DE VALEUR INCONNUE !
*/
template < unsigned int N, unsigned int puiss > struct Pow
{
	enum { value = N * Pow<N, puiss-1>::value };
};
template < unsigned int N > struct Pow<N, 0>
{
	enum { value = 1 };
};


/***************************************************************************//*!
* @brief Détermine le nombre de chiffre dans un nombre. (NOMBRE POSITIF !)
* @param[in] N Le nombre a calculer
*
* Utilisation:
* @code
* printf("Il y a %d chiffre dans le nombre 5380", getSizeOfNumber<5380>::value);
* @endcode
* @note Calculs effectués à la compilation
* @warning NE PAS METTRE DE VALEUR INCONNUE !
* @see struct getSizeOfNumberS
*/
template < unsigned int N > struct getSizeOfNumber
{
	enum {
		value = 1+ getSizeOfNumber< N/10 >::value
	};
};
template <> struct getSizeOfNumber<0>
{
	enum {
		value = 0
	};
};


/***************************************************************************//*!
* @brief Ensemble de test
* @param[in] N Le nombre a tester
*
* Utilisation:
* @code
* printf("5380 est paire? : %d\n 5380 est négatif? : %d", test<5380>::isPair, test<5380>::isNegative);
* @endcode
* @note Calculs effectués à la compilation
* @warning NE PAS METTRE DE VALEUR INCONNUE !
*/
template < int N > struct test
{
	enum {
		isPair = ( N%2 ? false : true ),
		isNegative = ( N<0 ? true : false )
	};
};


/***************************************************************************//*!
* @brief Détermine le nombre de chiffre dans un nombre. (Positif ou négatif). Prise en compte du signe - dans les calculs.
* @param[in] N Le nombre a calculer
*
* Utilisation:
* @code
* printf("Il y a %d chiffre dans le nombre -12", getSizeOfNumberS<-12>::value);
* @endcode
* @note Calculs effectués à la compilation
* @warning NE PAS METTRE DE VALEUR INCONNUE !
*/
template < int N > struct _getSizeOfNumberS// Sous fonction. NE pas utiliser directement
{
	enum {
		value = 1 + _getSizeOfNumberS< N/10 >::value
	};
};
template <> struct _getSizeOfNumberS<0>// Sous fonction. NE pas utiliser directement
{
	enum {
		value = 0
	};
};
template < int N > struct getSizeOfNumberS
{
	enum {
		value = _getSizeOfNumberS<N>::value + (test<N>::isNegative ? 1 : 0 )
	};
};


/***************************************************************************//*!
* @brief Liste de valeurs intéressantes
*
* @note Calculs effectués à la compilation
*/
enum {
	Is32Bits = (sizeof(void*) == 4 ? 1 : 0),
	Is64Bits = (sizeof(void*) == 8 ? 1 : 0)
};
