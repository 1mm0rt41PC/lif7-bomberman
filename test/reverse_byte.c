#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

template<class Type> Type reverseBit( Type var )// Inverse les bits
{
	const unsigned int nbByte = sizeof(Type)*CHAR_BIT;

	Type out = 0;
	unsigned int i=0;

	do{
		out|= var&0x01;		//copie du bit de poids faible
		i++;				//compter les bits copiés
		if( i == nbByte)	//si tous les bits sont traités...
			break;			//...arrêter
		var >>= 1;			//vider vers la droite (préparer le suivant)...
		out<<= 1;			//...pour remplir vers la gauche (préparer la place)
	}while(1);

	return out;
}

// Conversion Little Endian <-> Big Endian
template<class Type> Type reverseByte( Type var )// Inverse les octets
{
	const unsigned int nbByte = sizeof(Type)*CHAR_BIT;

	Type out = 0;
	const	char* pIn = (const		char*) &var;
			char* pOut= (			char*)(&out+1)-1;

	for( unsigned int i=sizeof(Type); i>0 ; i-- )
	{
		*pOut-- = *pIn++;
	}
	return out;
}


bool isLittleEndian(void)// isIntel
{
	long l = 1;
	return (bool) *((char *)(&l));
}


int main()
{

	long t1 = 0x10;
	long t2 = 0;
	printf("%#X %#X\n", t1, t2);
	
	t2 = reverseByte( t1 );
	printf("%#X %#X\n", t1, t2);

	return 0;
}