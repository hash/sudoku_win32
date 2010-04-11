#ifndef UI_H
#define UI_H

#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include "konsola.h"
#include "plansza.h"

namespace interfejs_urzytkownika {

using std::cin;
using std::cout;
using std::string;
using std::vector;
using sudoku::stany_Pola;
using namespace konsola;

inline const Koordynaty& marginesy() {
	static const Koordynaty marginesy_(3,1);
	return marginesy_;
}

/// Klasa bazowa interfejsu użytkownika.
class ElementInterfejsu {
public:
	ElementInterfejsu()
		: nazwa_(), lewa_gora_(), prawy_dol_(), rozmiar_(), kolumna_elementow_(0) {}
	explicit ElementInterfejsu(const string& nazwa)
		: nazwa_(nazwa), lewa_gora_(), prawy_dol_(), rozmiar_(), kolumna_elementow_(0) {
		if (!nazwa_.empty()) {
			rozmiar_.x_ = prawy_dol_.x_ = 2 * marginesy().x_ + nazwa_.size() + (nazwa_.size() % 2 != 0 ? 1 : 0);
			rozmiar_.y_ = prawy_dol_.y_ = 2 * marginesy().y_ + 1;
		}
	}
	virtual ~ElementInterfejsu() {}
	virtual ElementInterfejsu& NaSrodek();
	virtual ElementInterfejsu& MarginesPrawy(short margines);
	virtual ElementInterfejsu& MarginesLewy(short margines);
	virtual ElementInterfejsu& MarginesGorny(short margines);
	virtual ElementInterfejsu& MarginesDolny(short margines);
	virtual void Uruchom() = 0;
	bool w_kolumnie_elementow() const {
		return kolumna_elementow_ != 0;
	}
	Koordynaty rozmiar() const {
		return rozmiar_;
	}
	void ustaw_w_kolumnie_elementow(ElementInterfejsu* kolumna_elementow) {
		if (kolumna_elementow != this) {
			kolumna_elementow_ = kolumna_elementow;
		} // else KolumnaElementow nie może być sama w sobie
	}
	friend class KolumnaElementow;
protected:
	ElementInterfejsu* kolumna_elementow_;
	string nazwa_;
	Koordynaty lewa_gora_, prawy_dol_, rozmiar_;
};

class KolumnaElementow : public ElementInterfejsu {
public:
	KolumnaElementow() : ElementInterfejsu() {}
	virtual void Uruchom();
	void DodajElement(ElementInterfejsu* element);
	virtual ElementInterfejsu& NaSrodek();
	virtual ElementInterfejsu& MarginesPrawy(short margines);
	virtual ElementInterfejsu& MarginesLewy(short margines);
	virtual ElementInterfejsu& MarginesGorny(short margines);
	virtual ElementInterfejsu& MarginesDolny(short margines);
	virtual ~KolumnaElementow();
private:
	void UstawElementyWRelacjiDoKolumnyElementow();
	vector< ElementInterfejsu* > wektor_elementow_;
};

/// Klasa wypisująca ascii art.
class Splasz : public ElementInterfejsu {
public:
	explicit Splasz(const string&);
	virtual void Uruchom();
private:
	unsigned int IloscLini;
	vector< string > Linie;
};

/// Klasa obsługująca nazwę opisu.
class Opis : public ElementInterfejsu {
public:
	explicit Opis(const string& nazwa) : ElementInterfejsu(nazwa) {}
	virtual void Uruchom();
};

/// Klasa obsługująca nazwy opcji wyboru
class Opcje : public ElementInterfejsu {
public:
	Opcje() : ElementInterfejsu(), wybor_(0), opcja_ucieczki_(0) {}
	explicit Opcje(const string& nazwa) : ElementInterfejsu(nazwa), wybor_(0), opcja_ucieczki_(0) {}
	virtual void Uruchom();
	void DodajOpcje(const string opcja);
	void DodajOpcjeUcieczki(const string opcja);
	unsigned int wybor() {
		return wybor_;
	}
private:
	unsigned int wybor_;
	unsigned int opcja_ucieczki_;
	vector< string > wektor_opcji_;
};

/// Klasa wyswietlajaca plansze do gry.
class Plansza : public ElementInterfejsu {
private:
	sudoku::Plansza plansza;
	BuforEkranu bufor_;
	Koordynaty XY;
	bool podpowiedzi_;
	int X, Y;
public:
	explicit Plansza() : ElementInterfejsu() {
		rozmiar_.x_ = prawy_dol_.x_ = 48;
		rozmiar_.y_ = prawy_dol_.y_ = 27;
	}
	Plansza(const sudoku::Plansza& PL)
		: ElementInterfejsu(), plansza(PL) {
		rozmiar_.x_ = prawy_dol_.x_ = 48;
		rozmiar_.y_ = prawy_dol_.y_ = 27;
	}
	virtual void Uruchom();
	void Wpisz(const sudoku::Plansza& PL) {
		plansza.Reset();
		plansza = const_cast< sudoku::Plansza& >(PL);
	}
	void Reset() {
		plansza.Reset();
	}
	void Generuj(unsigned int poziom) {
		plansza.Reset();
		plansza.Generuj(static_cast< int >(poziom));
	}
	void WlaczPodpowiedzi() { podpowiedzi_ = true; }
	void WylaczPodpowiedzi() { podpowiedzi_ = false; }
	void Podpowiedzi(const stany_Pola mozliwe);
	Koordynaty WypiszLiczby();
	void PustaPlansza();
	void obsluga(Koordynaty xy);
};

/// Klasa mierzy czas.
/** Mierzy czas rozwiązywania sudoku i zapisuje go do pliku. */
class Czas {
private:
	Czas() : start(0), stop(0), millitmStart(0), millitmStop(0), k(0) {}
	std::fstream plik;
	struct __timeb64 tstruct;
	__int64 start, stop;
	unsigned short millitmStart, millitmStop;
	double k;
	template< typename T >
	string toString(T& ref) {
		std::ostringstream oss;
		oss << ref;
		return oss.str();
	}
	void Oblicz();
	void Wypisz(string name);
	void DoPliku(string name);
	inline void Start() {
		_ftime64_s( &tstruct );
		start = tstruct.time;
		millitmStart = tstruct.millitm;
	}
	inline void Stop() {
		_ftime64_s( &tstruct );
		stop = tstruct.time;
		millitmStop = tstruct.millitm;
		Oblicz();
	}
	friend class Gracz;
};

/// Klasa nowego użytkownika.
class Gracz : public ElementInterfejsu {
public:
	Gracz() : ElementInterfejsu(), czas_gracza_() {
		rozmiar_.x_ = 20 + 2 * marginesy().x_;
		rozmiar_.y_ += 1 + 2 * marginesy().y_;
		prawy_dol_ = rozmiar_;
	}
	explicit Gracz(const string& nazwa) : ElementInterfejsu(nazwa), czas_gracza_() {
		rozmiar_.x_ = 20 + 2 * marginesy().x_;
		rozmiar_.y_ += 1 + 2 * marginesy().y_;
		prawy_dol_ = rozmiar_;
	}
	virtual void Uruchom();
	bool Ustawiony() const {
		return !nazwa_gracza_.empty();
	}
	void ZaczynaGre() {
		czas_gracza_.Start();
	}
	void KonczyGre() {
		czas_gracza_.Stop();
	}
	void ZapisCzasuDoPliku() {
		czas_gracza_.DoPliku(nazwa_gracza_);
	}
	void WyswietlCzas() {
		czas_gracza_.Wypisz(nazwa_gracza_);
	}
private:
	Czas czas_gracza_;
	string nazwa_gracza_;
};

} // namespace interfejs_urzytkownika

#endif // UI_H
