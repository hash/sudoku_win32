#include "plansza.h"

namespace sudoku {
void Plansza::Inicjalizacja() {
	unsigned int i(0), j(0), x(0), y(0);
	Pole tmp;
	wektor_wskaznikow_pol t;
	t.reserve(SIZE);
	vpola.reserve(3 * SIZE);
	pola.reserve(SIZE * SIZE);
	pola.resize(SIZE * SIZE, tmp);
	polas.reserve(SIZE * SIZE);
	polas.resize(SIZE * SIZE, tmp);
	// po wierszach 0-8
	for (i = 0 ; i < SIZE ; ++i) {
		t.clear();
		for (j = 0 ; j < SIZE ; ++j) {
			t.push_back(&pola[i * SIZE + j]);
		}
		vpola.push_back(t);
	}
	// po kolumnach 9-17
	for (i = 0 ; i < SIZE ; ++i) {
		t.clear();
		for (j = 0 ; j < SIZE ; ++j) {
			t.push_back(&pola[j * SIZE + i]);
		}
		vpola.push_back(t);
	}
	// po sektorach 18-26
	for (i = 0 ; i < SMALL_SIZE ; ++i) {
		for (j = 0 ; j < SMALL_SIZE ; ++j) {
			t.clear();
			for (x = 0 ; x < SMALL_SIZE ; ++x) {
				for (y = 0 ; y < SMALL_SIZE ; ++y) {
					t.push_back(&pola[(i * SMALL_SIZE + x) * SIZE + j * SMALL_SIZE + y]);
				}
			}
			vpola.push_back(t);
		}
	}
}

Pole* Plansza::PierwszeNierozwiazane() {
	std::set< Pole*, DereferencjaPorownanieMniejsze > set_pol;
	for (wektor_pol::iterator iterator_pola(pola.begin()), koniec(pola.end()) ;
		iterator_pola != koniec ; ++iterator_pola) {
		if (iterator_pola->stan_ == 0) {
			set_pol.insert(&(*iterator_pola));
		}
	}
	if(!set_pol.empty()) {
		return *set_pol.begin();
	} else {
		return 0;
	}
}

void Plansza::OgraniczenieMozliwosci() {
	stany_Pola bits;
	bits.reset().flip();
	for (wektor_pol::iterator iterator_pola(pola.begin()), koniec(pola.end()) ;
		iterator_pola != koniec ; ++iterator_pola) {
		iterator_pola->mozliwe_ = bits;
	}
	for (it = vpola.begin() ; it != vpola.end() ; ++it) {
		bits.reset();
		for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
			if ((*it2)->stan_ != 0) {
				bits.set((*it2)->stan_ - 1);
			}
		}
		for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
			if ((*it2)->stan_ == 0) {
				(*it2)->Ogranicz(bits);
			} else {
				(*it2)->mozliwe_.reset();
			}
		}
	}
}

bool Plansza::Eliminacja() {
	unsigned int n(0);
	bool zmiana(false);
	for (wektor_pol::iterator iterator_pola(pola.begin()), koniec(pola.end()) ;
		iterator_pola != koniec ; ++iterator_pola) {
			if (iterator_pola->Ustal()) {
				zmiana = true;
			}
	}
	return zmiana;
}

bool Plansza::Samotnik() {
	unsigned int n(0), i(0), ik(0), moz(0),c(0);
	bool zmiana(false);
	Pole* t;
	OgraniczenieMozliwosci();
	for (ik = vpola.size(), i = 0 ; i < ik ; ++i) {
		for (n = 0 ; n < SIZE ; ++n) {
			moz = 0;
			t = NULL;
			for (it2 = vpola[i].begin() ; it2 != vpola[i].end() ; ++it2) {
				if ((*it2)->mozliwe_.test(n)) {
					++moz;
					if (moz > 1) continue;
					t = *it2;
				}
			}
			if (moz == 1) {
				if (t->stan_ == 0) {
					t->stan_ = n + 1;
					t->mozliwe_.reset();
					zmiana = true;
					++c;
				}
			}
		}
		if (((i + 1) % SIZE) == 0) {
			if (zmiana) {
				do {
					OgraniczenieMozliwosci();
				} while(Eliminacja());
				zmiana = false;
			}
		}
	}
	if (c != 0) zmiana = true;
	return zmiana;
}

bool Plansza::ZnajdzBlizniaki() {
	bool zmiana(false);
	wektor_wskaznikow_pol JarekILech;
	for (it = vpola.begin() ; it != vpola.end() ; ++it) {
		JarekILech.clear();
		for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
			if ((*it2)->mozliwe_.count() == 2) {
				JarekILech.push_back(*it2);
			}
		}
		if (JarekILech.size() == 2) {
			if (JarekILech[0]->mozliwe_ == JarekILech[1]->mozliwe_) {
				for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
					if ((*it2)->mozliwe_ != JarekILech[0]->mozliwe_ && (*it2)->mozliwe_.any()) {
						if ((*it2)->mozliwe_.to_ulong() & JarekILech[0]->mozliwe_.to_ulong()) {
							(*it2)->mozliwe_ &= ~JarekILech[0]->mozliwe_;
							zmiana = true;
						}
					}
				}
			}
		}
	}
	return zmiana;
}

bool Plansza::Trojki() {
	bool zmiana(false);
	wektor_wskaznikow_pol trojki,dwojki;
	for (it = vpola.begin() ; it != vpola.end() ; ++it) {
		trojki.clear();
		dwojki.clear();
		for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
			if ((*it2)->mozliwe_.count() == 3)
				trojki.push_back(*it2);
		}
		if (trojki.size() == 3) {
			if (trojki[0]->mozliwe_ == trojki[1]->mozliwe_ && trojki[1]->mozliwe_ == trojki[2]->mozliwe_) {
				for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
					if ((*it2)->mozliwe_ != trojki[0]->mozliwe_ && (*it2)->mozliwe_.any()) {
						if ((*it2)->mozliwe_.to_ulong() & trojki[0]->mozliwe_.to_ulong()) {
							(*it2)->mozliwe_ &= ~trojki[0]->mozliwe_;
							zmiana = true;
						}
					}
				}
			}
		}
		if (trojki.size() == 2) {
			if (trojki[0]->mozliwe_ == trojki[1]->mozliwe_) {
				for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
					if ((*it2)->mozliwe_.count() == 2) {
						dwojki.push_back(*it2);
					}
				}
				if (dwojki.size() == 1) {
					if (dwojki[0]->mozliwe_.to_ulong() ==
						(trojki[0]->mozliwe_.to_ulong() & dwojki[0]->mozliwe_.to_ulong())) {
						for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
							if (((*it2)->mozliwe_ != trojki[0]->mozliwe_ &&
								(*it2)->mozliwe_ != dwojki[0]->mozliwe_) && (*it2)->mozliwe_.any()) {
								if ((*it2)->mozliwe_.to_ulong() & trojki[0]->mozliwe_.to_ulong()) {
									(*it2)->mozliwe_ &= ~trojki[0]->mozliwe_;
									zmiana = true;
								}
							}
						}
					}
				}
			}
		}
		if (trojki.size() == 1) {
			for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
				if ((*it2)->mozliwe_.count() == 2) {
					dwojki.push_back(*it2);
				}
			}
			if (dwojki.size() == 2) {
				if ((dwojki[0]->mozliwe_.to_ulong() ==
					(trojki[0]->mozliwe_.to_ulong() & dwojki[0]->mozliwe_.to_ulong())) &&
					(dwojki[1]->mozliwe_.to_ulong() ==
					(trojki[0]->mozliwe_.to_ulong() & dwojki[1]->mozliwe_.to_ulong())) &&
					(dwojki[0]->mozliwe_ != dwojki[1]->mozliwe_)) {
					for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
						if (((*it2)->mozliwe_ != trojki[0]->mozliwe_ &&
							(*it2)->mozliwe_ != dwojki[0]->mozliwe_ &&
							(*it2)->mozliwe_ != dwojki[1]->mozliwe_) && (*it2)->mozliwe_.any()) {
							if ((*it2)->mozliwe_.to_ulong() & trojki[0]->mozliwe_.to_ulong()) {
								(*it2)->mozliwe_ &= ~trojki[0]->mozliwe_;
								zmiana = true;
							}
						}
					}
				}
			}
		}
	}
	return zmiana;
}

void Plansza::Generuj(int poziom) {
	for (int i = 0 ; i < SIZE ; ++i)
		for (int j = 0 ; j < SIZE ; ++j)
			p[i][j] = s[i][j] = 0;

	// generate a solution -- store in s
	Generuj(0, 0);

	// difficulty is measured by the number of required backtracks
	int difficulty;
	do {
		difficulty = static_cast< int >(100 * pow(10.0, poziom));
		// give "answers" to 25 % of i, j in p
		for (int i = 0 ; i < SIZE ; ++i)
			for (int j = 0 ; j < SIZE ; ++j) {
				p[i][j] = (rand() % 4) ? 0 : s[i][j];
				pOrig[i][j] = (p[i][j] != 0);
			}
			// force p leads to a unique solution s
	}while (Unikalna(0, 0, difficulty));

	for (unsigned int i = 0 ; i < SIZE ; ++i)
		for (unsigned int j = 0 ; j < SIZE ; ++j) {
			polas[SIZE * i + j].blokada_ = false;
			polas[SIZE * i + j].Ustaw(s[i][j]);
			pola[SIZE * i + j].blokada_ = false;
			pola[SIZE * i + j].Ustaw(p[i][j]);
			if (p[i][j]) pola[SIZE * i + j].blokada_ = true;
		}
}

bool Plansza::Generuj(int i, int j) {
	static bool first(true);
	static unsigned int count = 0;

	// each bit (1 - 9), b, of each int represents whether the number,
	// b has been used
	static unsigned int cols[9];	// bits for cols 0 - 9
	static unsigned int rows[9];	// bits for rows 0 - 9
	static unsigned int sqs[9];		// bits for squares 0 - 9

	if (first) {
		srand(static_cast< long >(time(0)));
		first = false;
	}

	// new generation of square, reset test vectors
	if (i == 0 && j == 0) {
		for (int i = 0 ; i < 9 ; ++i)
			sqs[i] = rows[i] = cols[i] = 0;
	}

	if (i == 9) {
		// all rows complete!
		return true;
	}

	int sqnum = (j / 3) + (i / 3) * 3;
	// possible values are those that have not been assigned
	// for the position in question, look at the corresponding
	// row, col and square. The complement of the union of these
	// three sets are the possible numbers for this position.
	// use '|' (or) to get the union and '^' (xor) to get the complement
	unsigned int possible = (rows[i] | cols[j] | sqs[sqnum]) ^ 0x3FE;

	if (!possible)
		// no number can satisfy i, j
		return false;

	// put the possible numbers that might work in an array
	// nums, where numpos "points" to one beyond the last possibility
	int numpos = 0;
	int nums[9];
	for (int k = 1 ; k <= 9 ; ++k)
		if (possible & (1 << k)) {
			nums[numpos++] = k;
		}

		// order possiblilites randomly
		for (int k = 0 ; k < numpos ; ++k)
			std::swap(nums[k], nums[rand() % numpos]);

		// try each of the possible numbers in i, j
		for (int k = 0 ; k < numpos ; ++k) {
			// keep current row, col and sq data in case
			// this possibility does not work out
			int save_rows = rows[i];
			int save_cols = cols[j];
			int save_sqs = sqs[sqnum];

			// save the actual number in num (for speed)
			int num = nums[k];

			// mark row, col and square with the number
			rows[i] |= (1 << num);
			cols[j] |= (1 << num);
			sqs[sqnum] |= (1 << num);

			// mark the solution matrix with the number
			s[i][j] = num;

			// perform the solution on the next position (first by column, then by row)
			if (Generuj(i + static_cast< int >(j % 9 == 8), (j + 1) % 9))
				return true; // got a good square

			// num did not work
			// need to try again, restore old state
			rows[i] = save_rows;
			cols[j] = save_cols;
			sqs[sqnum] = save_sqs;

			s[i][j] = 0;
		}

		// we ran out of possibilities - so tell the guy that
		// sent us the partial solution to try again
		return false;
}

bool Plansza::Unikalna(int i, int j, int& difficulty) {
	static unsigned int cols[9];
	static unsigned int rows[9];
	static unsigned int sqs[9];

	if (i == 0 && j== 0) {
		// reset cols, rows, sqs on first call
		for (int i = 0 ; i < 9 ; ++i)
			cols[i] = rows[i] = sqs[i] = 0;

		// set the initial information of rows, cols, sqs
		for (int i = 0 ; i < 9 ; ++i)
			for (int j = 0 ; j < 9 ; ++j) {
				int num = p[i][j];
				if (num) {
					int sqnum = (j / 3) + (i / 3) * 3;
					rows[i] |= (1 << num);
					cols[j] |= (1 << num);
					sqs[sqnum] |= (1 << num);
				}
			}
	}

	if (i == 9) {
		// all rows complete!
		// verify if the solution is the same as we have
		for (int i = 0 ; i < 9 ; ++i)
			if (p[i][j] != s[i][j])
				return true; // oops non - unique
		return false; // looks unique so far
	}

	if (pOrig[i][j])
		// original value, no change may be made to this location
		return Unikalna(i + static_cast< int >(j % 9 == 8), (j + 1) % 9, difficulty);

	int sqnum = (j / 3) + (i / 3) * 3;
	unsigned int possible = (rows[i] | cols[j] | sqs[sqnum]) ^ 0x3FE;

	if (!possible)
		// no number can satisfy i, j
		return false;

	if (--difficulty == 0) {
		// looks too hard, tell them not unique
		return true;
	}

	int numpos = 0;
	int nums[9];
	for (int k = 1 ; k <= 9 ; ++k)
		if (possible & (1 << k))
			nums[numpos++] = k;

	// try this number in i, j
	for (int k = 0 ; k < numpos ; ++k) {
		int save_rows = rows[i];
		int save_cols = cols[j];
		int save_sqs = sqs[sqnum];
		int num = nums[k];
		bool nonUnique; // does this generate a nonUnique?

		rows[i] |= (1 << num);
		cols[j] |= (1 << num);
		sqs[sqnum] |= (1 << num);
		p[i][j] = num;

		nonUnique = Unikalna(i + static_cast< int >(j % 9 == 8), (j + 1) % 9, difficulty);

		// restore to old state (always)
		rows[i] = save_rows;
		cols[j] = save_cols;
		sqs[sqnum] = save_sqs;
		p[i][j] = 0;

		if (nonUnique) // tell above, non - unique solution exists
			return true;
	}
	return false; // looks ok at this level
}

void Plansza::WpiszRozwiazanie() {
	pola = polas;
}

bool Plansza::Rozwiaz() {
	do {
		do {
			OgraniczenieMozliwosci();
		} while (Eliminacja());
		if (CzyRozwiazana())
			return true;
		while (Samotnik()) {}
		if (CzyRozwiazana()) {
			return true;
		} else {
			do {
				while (ZnajdzBlizniaki()) {}
			} while (Trojki());
		}
	} while (Eliminacja());
	if (CzyRozwiazana()) {
		return true;
	} else if (CzyDaSieRozwiazac()) {
		vector< unsigned int > stany;
		for (wektor_pol::iterator iterator_pola(pola.begin()), koniec(pola.end()) ;
			iterator_pola != koniec ; ++iterator_pola) {
			stany.push_back(iterator_pola->stan_);
		}

		Pole* t = PierwszeNierozwiazane();
		if (t == 0) return false;

		unsigned int i(0);
		std::set< unsigned int > stany_pola;
		do {
			if (t->mozliwe_.test(i)) {
				stany_pola.insert(i + 1);
			}
			++i;
		} while (stany_pola.size() < t->mozliwe_.count());

		for (std::set< unsigned int >::iterator iterator_stany_pola(stany_pola.begin()), koniec(stany_pola.end()) ;
			iterator_stany_pola != koniec ; ++iterator_stany_pola) {
			t->stan_ = *iterator_stany_pola;
			if (Rozwiaz()) return true;

			const unsigned int n(pola.size());
			for (i = 0 ; i < n ; ++i) {
				pola[i].stan_ = stany[i];
			}
		}
		return false;
	} else {
		return false;
	}
}

bool Plansza::CzyRozwiazana() {
	stany_Pola bits;
	stan = 0;
	for (it = vpola.begin() ; it != vpola.end() ; ++it) {
		bits.reset();
		for (it2 = it->begin() ; it2 != it->end() ; ++it2) {
			if ((*it2)->stan_ != 0) {
				bits.flip((*it2)->stan_ - 1);
			} else {
				break;
			}
		}
		if (bits.to_ulong() == ref) {
			++stan;
		}
	}
	return stan == (3 * SIZE);
}

bool Plansza::CzyDaSieRozwiazac() {
	OgraniczenieMozliwosci();
	for (wektor_pol::iterator iterator_pola(pola.begin()), koniec(pola.end()) ;
		iterator_pola != koniec ; ++iterator_pola) {
			if (iterator_pola->stan_ == 0) {
				if (iterator_pola->mozliwe_.none()) return false;
			}
	}
	return true;
}

} // namespace sudoku
