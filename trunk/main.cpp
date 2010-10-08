#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "partie.h"
using namespace sf;


/*******************************************************************************
* CONVENTION DE PROGRAMATION !
* Toutes les variables d'une class doivent commencer par "c_"
*
*
* A FAIRE
* Partie.h.cpp
*	MODE c_mode
*
*/



int main()
{
	/*
    // Create the main window
    RenderWindow App(VideoMode(800, 600), "SFML window");

	// Start the game loop
    while (App.IsOpened())
    {
        // Process events
        Event Event;
        while (App.GetEvent(Event))
        {
            // Close window : exit
            if (Event.Type == Event::Closed)
                App.Close();
        }

        // Clear screen
        App.Clear(Color(255, 255, 255));

		// ICI on déssine

        // Update the window
        App.Display();
    }
*/

	partie* nouvelle_partie = new partie();


    return EXIT_SUCCESS;
}
