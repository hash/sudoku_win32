#ifndef PLANSZA_H
#define PLANSZA_H

#include <fstream>
#include <ctime>
#include <vector>
#include <set>
#include <utility>
#include <iostream>
#include <cmath>
#include "pole.h"
#include "sudoku.h"

namespace sudoku {

using std::vector;

class Plansza {
private:
	std::fstream plik;
	unsigned int stan;		///< stan planszy
	typedef vector< Pole > wektor_pol;
	typedef vector< Pole* > wektor_wskaznikow_pol;
	wektor_pol pola, polas;
	wektor_wskaznikow_pol::iterator it2;
	vector< wektor_wskaznikow_pol >::iterator it;
	vector< wektor_wskaznikow_pol > vpola;

	// solution to sudoku
	int s[SIZE][SIZE];

	// playing of sukoku
	int p[SIZE][SIZE];

	bool pOrig[SIZE][SIZE]; // squares that were originally given
	void Inicjalizacja();
	Pole* PierwszeNierozwiazane();
public:
	Plansza() : stan(0) {
		Inicjalizacja();
	}
	explicit Plansza(std::string plansza_w_lini) : stan(0) {
		Inicjalizacja();
		plansza_w_lini.resize(SIZE * SIZE, 0x30);
		char bufor[2] = "0";
		for (unsigned int i(0), j(0); i < (SIZE * SIZE) ; ++i) {
			bufor[0] = plansza_w_lini.at(i);
			j = atoi(bufor);
			if (!( j < 1 || j > SIZE )) {
				pola[i].Ustaw(j);
				pola[i].blokada_ = true;
			}
		}
	}
	Plansza(const Plansza& p) : stan(p.stan) {
		Inicjalizacja();
		for (unsigned int i = 0 ; i < SIZE * SIZE ; ++i) {
			pola[i] = const_cast< Pole& >(p.pola[i]);
		}
	}

	Pole& operator()(const unsigned int x, const unsigned int y) {
		return pola[SIZE * x + y];
	}
	Plansza& operator=(Plansza& p) {
		if (this == &p) return *this;
		stan = p.stan;
		for (unsigned int i = 0 ; i < SIZE * SIZE ; ++i) {
			pola[i] = const_cast< Pole& >(p.pola[i]);
		}
		return *this;
	}
	void Reset() {
		stan = 0;
		Pole puste_pole;
		for (unsigned int i = 0 ; i < SIZE * SIZE ; ++i) {
			pola[i] = polas[i] = puste_pole;
		}
	}
	/// funkcje zawierajaca algorytm rozwiązujacy sudou
	void OgraniczenieMozliwosci();
	bool Eliminacja();
	bool Samotnik();
	/*
	bool SamotnikSektory();
	bool SamotnikWiersze();
	bool SamotnikKolumny();
	*/
	bool ZnajdzBlizniaki();
	bool Trojki();
	/// funkcje zawierające algorytm generowania sudoku
	void Generuj(int poziom);
	bool Generuj(int i, int j);
	bool Unikalna(int i, int j, int& difficulty);
	void WpiszRozwiazanie();
	bool Rozwiaz();
	bool CzyRozwiazana();
	bool CzyDaSieRozwiazac();
};

} // namespace sudoku

#endif // PLANSZA_H
