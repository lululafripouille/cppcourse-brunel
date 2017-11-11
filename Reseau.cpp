#include <cassert>
#include "Reseau.hpp"
#include <cmath>
#include <random>
#include <array>
#include <fstream>
#include <string>

//CONSTRUCTEUR, DESTRUCTEUR_____________________________________________

Reseau::Reseau() 
	: moyennePicsParNeuroneParSeconde (0) {
	for (size_t i(0); i < reseau.size(); ++i) {
		if (i < nombreExcitateurs) {
			reseau[i] = new Neurone(Excitateur);						//crée les neurones, les premiers sont excitateurs, les seconds inhibiteurs
		} else {
			reseau[i] = new Neurone(Inhibiteur);
		}
	}
	std::random_device rd;										//le static n'est pas indispensable ici
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> disExc (0, nombreExcitateurs-1);
	std::uniform_int_distribution<> disInh (nombreExcitateurs, nombreExcitateurs+nombreInhibiteurs-1);
	for (int i (0); i < tailleReseau; ++i) {
		std::array<unsigned short int, nombreConnexions> ligne;
		for (int j(0); j < ConnexionExcitatrices ; ++j) {				//assigne une connexion à une relation selon le nombre généré par Poisson 
			int a (disExc(gen));
			assert(a >= 0 and a < nombreExcitateurs);
			ligne[j] = a; 
		}
		for (int k(0); k < ConnexionInhibitrices; ++k) {				//assigne une connexion à une relation selon le nombre généré par Poisson
			int b(disInh(gen));
			assert(b < tailleReseau and b > nombreExcitateurs-1);
			ligne[k+ConnexionExcitatrices] = b;
		}
		relations.push_back(ligne);
	}
	for (auto& carre : tableauAImprimer) {
		carre = 0;														//initialise à zéro
	}
}

Reseau::~Reseau() {
	
	for (size_t i(0); i < reseau.size(); ++i) {							
			delete reseau[i];											//libère la mémoire des pointeurs alloués dynamiquement
	}
}

//ACCESSEURS____________________________________________________________

int Reseau::accesMoyennePicsParNeuroneParSec() {
	return moyennePicsParNeuroneParSeconde;
}

int Reseau::accesCaseRelations (int ligne, int colonne) {
	assert(ligne < relations.size());
	assert(colonne < relations[ligne].size());
	return relations[ligne][colonne];
}

//AUTRE METHODE PUBLIQUE________________________________________________

void Reseau::evolue(int pasCourant, int pasFinal, double Iext, bool poisson) {	
	assert(pasCourant < pasFinal);
	std::string nomDossier ("donnee.txt");
	std::ofstream texte (nomDossier);
	while (pasCourant < pasFinal) {										//boucle jusqu'au temps final
		tableauAImprimer.push_back(0);									
		for (size_t i (0); i < tailleReseau ; ++i) {
			assert (reseau[i] != nullptr);
			if (reseau[i]->evolue(pasCourant, Iext, poisson)) {			//s'il y a un pic, réaliser ce qui suit
				tableauAImprimer[pasCourant] += 1;
				moyennePicsParNeuroneParSeconde += 1;
				//n'écrit dans le fichier que pour les pas de temps correspondant au Brunel, il suffit de commenter le if pour éviter cela
				if ((pasCourant > 5000 and pasCourant < 6000 and eta == 3 and g == 2) or  //conditions pour écrire les bons numéros dans le fichier -- A
					(pasCourant > 10000 and pasCourant < 12000)) {						  //pour B, C, D
					texte << pasCourant/1000.0 << '\t' << i << '\n';
				}	
				for (size_t j (0) ; j < nombreConnexions ; ++j) {
					if (i != j) {										//évite de s'autoconnecter, et se charger soi-même
						if (reseau[i]->accesNature() == Excitateur) {
							reseau[relations[i][j]]->recoit(TensionJe);	//transmet le pic selon sa nature
						} else {
							reseau[relations[i][j]]->recoit(TensionJi);
						}
					}
				}
			}
		}
		pasCourant += h;												//incrémente le temps
	}
	std::cerr << "   " << moyennePicsParNeuroneParSeconde << std::endl;	//affiche la moyenne des pics par neurone par seconde
	texte.close();
	moyennePicsParNeuroneParSeconde = moyennePicsParNeuroneParSeconde * 10000 / (pasFinal * tailleReseau);
	//sert à afficher dans le terminal pour les pas de temps correpondant au Brunel
	//et permet de faire un document excel/numbers avec ça 
	if (eta == 3 and g == 2 and pasFinal > 6000) {
		for (int i (5000); i < 6000; ++i) {
			std::cout << tableauAImprimer[i] << std::endl;
		}
	} else if (pasFinal > 12000-1) {
		for (int i (10000); i < 12000; ++i) {
			std::cout << tableauAImprimer[i] << std::endl;
		}
	}
}
