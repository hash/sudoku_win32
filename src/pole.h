#ifndef POLE_H
#define POLE_H

#include "sudoku.h"
#include <bitset>

namespace sudoku {

typedef std::bitset<SIZE> stany_Pola;

class Pole {
public:
	/// Funkcja do ustawiania stanu pola.
	bool Ustaw(unsigned int stan);
	bool blokada() const {
		return blokada_;
	}
	unsigned int stan() const {
		return stan_;
	}
	stany_Pola mozliwe() const {
		return mozliwe_;
	}
	bool operator<(const Pole& prawe) const {
		return mozliwe_.count() < prawe.mozliwe_.count();
	}
	friend class Plansza;
	Pole(const Pole& P) : blokada_(P.blokada_), stan_(P.stan_), mozliwe_(P.mozliwe_) {}
	Pole& operator=(const Pole& p) {
		if (this == &p) return *this;
		blokada_ = p.blokada_;
		stan_ = p.stan_;
		mozliwe_ = p.mozliwe_;
		return *this;
	}
private:
	Pole() : blokada_(false), stan_(0), mozliwe_(ref) {}
	explicit Pole(const unsigned int stan) : blokada_(true), stan_(stan), mozliwe_(0) {}
	/// Funkcja ograniczająca możliwości stanów możliwych do dopisania polu.
	void Ogranicz(stany_Pola ograniczenie);
	bool Ustal();

	bool blokada_;				///< stan blokady pola
	unsigned int stan_;		///< stan pola
	stany_Pola mozliwe_;	///< możliwe stany pola
};

struct DereferencjaPorownanieMniejsze {
	template< typename wskaznik >
	bool operator()(wskaznik lewy, wskaznik prawy) const {
		return *lewy < *prawy;
	}
};

} // namespace sudoku

#endif // POLE_H
