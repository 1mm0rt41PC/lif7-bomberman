// Factorielle
template <unsigned int N> struct Fact
{
	enum { value = N * Fact<N-1>::value };
};
template <> struct Fact<0>
{
	enum { value = 1 };
};


// Pow
template < unsigned int N, unsigned int puiss > struct Pow
{
	enum { value = N * Pow<N, puiss-1>::value };
};
template < unsigned int N > struct Pow<N, 0>
{
	enum { value = 1 };
};


// Détermine le nombre de chiffre dans un nombre.
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


// Liste de tests
template < int N > struct test
{
	enum {
		isPair = ( N%2 ? false : true ),
		isNegative = ( N<0 ? true : false )	
	};
};


// Détermine le nombre de chiffre dans un nombre.
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


enum {
	Is32Bits = (sizeof(void*) == 4 ? 1 : 0),
	Is64Bits = (sizeof(void*) == 8 ? 1 : 0)
};