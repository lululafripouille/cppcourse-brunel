#include <iostream>
#include "Reseau.hpp"

int main () {
	double Iext (0);
	int pasCourant (0);
	int tempsFinal(0);
	bool poisson (true);
	Reseau res;
	std::cout << std::endl << "Bonjour, bienvenue dans la simulation !" << std::endl<< std::endl << "Veuillez entrer le courant externe (0 pour les grpahes de Brunel) !" << std::endl;
	std::cin >> Iext;
	std::cout << "Veuillez entrer la durée de la simulation [en ms]!" << std::endl << "(1200 pour les graphes b, c, d ou 600 pour le a)" << std::endl;
	std::cin >> tempsFinal;
	res.evolue(pasCourant, tempsFinal*10, Iext, poisson);
	return 0;
}
