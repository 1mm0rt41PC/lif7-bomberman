#include <stdio.h>
/*
#include "../bonus.h"
#include "../clavier.h"
#include "../map.h"
#include "../moteur_ncurses.h"
#include "../partie.h"
#include "../perso.h"
#include "../options.h"
#define __DEBUG_MEMORY__
#include "../debug.h"
*/
#define PRINT_SIZEOF( data ) printf(#data": %d\n", sizeof(data));



int main()
{
	PRINT_SIZEOF(float);
	PRINT_SIZEOF(double);
	//PRINT_SIZEOF(clock_t);
	//PRINT_SIZEOF(s_Coordonnees);
	//PRINT_SIZEOF(bonus);
	//PRINT_SIZEOF(bonus::t_Bonus);
//	PRINT_SIZEOF(std::string);
//	PRINT_SIZEOF(CMemoryManager);
//	PRINT_SIZEOF(bonus::s_bonus_proprieter);// Private
//	PRINT_SIZEOF(bonus::s_Event);// Private
//	PRINT_SIZEOF(std::vector<bonus::s_Event>);// Private
	return 0;
}
