#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;
// compiler avec -lsfml-system -lsfml-window
// Si non utilisation d'opengl -> sfml-graphics


#define DrawImage( Images, X, Y, Sprite )

int main(int argc, char** argv)
{
	//Window App(VideoMode(800, 600, 32), "SFML Window");
	RenderWindow App(VideoMode(800, 600, 32), "SFML Window");
	Event Event;
	Image test;
	if( !test.LoadFromFile("bomberman1_bas.png") )
		return 1;
	
	Sprite sp( test );
		
	sp.SetX(100);
	sp.SetY(100);

	while( App.IsOpened() )
	{
		while (App.GetEvent(Event))
		{
            // Close window : exit
            if (Event.Type == sf::Event::Closed)
                App.Close();

            // Escape key : exit
            if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
                App.Close();
		}
		
		App.Draw( sp );
		DrawImage( test, 100, 100 )
		
		
		
		// Display window on screen
		App.Display();
    }
   
    return 0;
}