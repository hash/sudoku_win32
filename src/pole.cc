#include "pole.h"

namespace sudoku {
bool Pole::Ustaw(unsigned int stan) {
	if (blokada_ || stan > SIZE) {
		return false;
	} else {
		if (stan == 0) {
			stan_ = 0;
			mozliwe_.reset().flip();
			return true;
		}
		if (mozliwe_.test(stan - 1)) {
			stan_ = stan;
			mozliwe_.reset();
			return true;
		} else {
			return false;
		}
	}
}

void Pole::Ogranicz(stany_Pola ograniczenie) {
	ograniczenie &= mozliwe_;
	mozliwe_ ^= ograniczenie;
}

bool Pole::Ustal() {
	if (mozliwe_.count() == 1) {
		for (unsigned int n = 0 ; n < SIZE ; ++n) {
			if (mozliwe_.test(n)) {
				stan_ = n + 1;
				mozliwe_.reset();
			}
		}
		return true;
	} else {
		return false;
	}
}

} // namespace sudoku
