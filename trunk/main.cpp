#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;


/*******************************************************************************
* CONVENTION DE PROGRAMATION !
* Toutes les variables d'une class doivent commencer par "c_"
*
*
*
*
*/



int main()
{
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

    return EXIT_SUCCESS;
}
