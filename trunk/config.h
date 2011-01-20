#ifndef CONFIG_h
#define CONFIG_h


// Define LIB : __LIB_ncurses__, __LIB_SDL__, __LIB_SFML__

// Pour les IDE et ne pas interférer avec les commandes make
#if !defined(__LIB_ncurses__) && !defined(__LIB_SDL__) && !defined(__LIB_SFML__)
	#define __LIB_SDL__ 1
	#warning "Veuillez definir la lib d'affichage ! ( __LIB_ncurses__, __LIB_SDL__, __LIB_SFML__ ) !"
#endif

// NOTE: si DEFAULT_PORT < 1024 alors les droits root sont nécéssaire
#define DEFAULT_PORT 947 // 947 = 'b'+'o'+'m'+'b'+'e'+'r'+'m'+'a'+'n' °(^_^)°

enum {
	__BOMBERMAN_VERSION__ = 1
};

#if defined(_WIN32) || defined(WIN32) || defined(WIN64) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define OS_WINDOWS 1
#else
#endif

/*******************************************************************************
* Lib SFML
*/
#ifdef __LIB_SFML__
	#include <SFML/Window.hpp>
	#include <SFML/Graphics.hpp>
	#define SYS_CLAVIER sf::Key::Code
	#define RETOUR_MENU_PRECEDENT sf::Key::Escape// Permet de faire la liaison pour quitter le menu principal


/*******************************************************************************
* Lib NCruses
*/
#elif __LIB_ncurses__
	#if defined(OS_WINDOWS)
		#include <Ncurses/curses.h>
	#else
		#include <ncurses.h>
	#endif
	#define SYS_CLAVIER int

	// Touches
	#define KEY_ESCAP 27
	#define KEY_ENTER_bis 10
	#define KEY_BACKSPACE_bis 8
	#define RETOUR_MENU_PRECEDENT KEY_ESCAP// Permet de faire la liaison pour quitter le menu principal

	#ifndef PADPLUS
		#define PADPLUS '+'
	#endif

	#ifndef PADMINUS
		#define PADMINUS '-'
	#endif

	#ifndef PADENTER
		#define PADENTER 0x1CB
	#endif


/*******************************************************************************
* Lib SDL
*/
#elif __LIB_SDL__
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
	#include <SDL/SDL_image.h>
	#define SYS_CLAVIER SDLKey //Uint8 || SDLKey
	#define RETOUR_MENU_PRECEDENT SDLK_ESCAPE// Permet de faire la liaison pour quitter le menu principal


/*******************************************************************************
* BUG
*/
#else
	#error "Veuillez definir la lib d'affichage ! ( __LIB_ncurses__, __LIB_SDL__, __LIB_SFML__ ) !"
#endif

#endif
