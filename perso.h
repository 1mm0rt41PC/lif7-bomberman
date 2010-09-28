#ifndef PERSO_h
#define PERSO_h
#include <string>

class perso
{
	private:
		// struct {
		std::string c_nom;
		unsigned int c_X_pos;
		unsigned int c_Y_pos;
		bool c_est_vivant;
		// }

	public:
		perso();
		perso(std::string nom, unsigned int Xpos, unsigned int Ypos);
		~perso();
		void setPos(unsigned int Xpos, unsigned int Ypos);

};

#endif
