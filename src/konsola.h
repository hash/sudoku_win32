#ifndef KONSOLA_H
#define KONSOLA_H

#define _WIN32_WINNT 0x0500		// Windows crap
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <tchar.h>
#pragma comment(lib, "Kernel32.lib")

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/// Funkcje konsoli.
/** Wykorzystywane w dużej części programu funkcje do obsługi konsoli w windows. */
namespace konsola {
struct Koordynaty {
	Koordynaty() : x_(0), y_(0) {}
	Koordynaty(const COORD& coord) : x_(coord.X), y_(coord.Y) {}
	Koordynaty(const short& x, const short& y) : x_(x), y_(y) {}
	Koordynaty(const Koordynaty& koordynaty) : x_(koordynaty.x_), y_(koordynaty.y_) {}
	operator COORD() {
		COORD coord = {x_, y_};
		return coord;
	}
	operator COORD() const {
		COORD coord = {x_, y_};
		return coord;
	}
	const Koordynaty operator+(const Koordynaty& koordynaty_prawe) const {
		return Koordynaty(x_ + koordynaty_prawe.x_, y_ + koordynaty_prawe.y_);
	}
	const Koordynaty operator-(const Koordynaty& koordynaty_prawe) const {
		return Koordynaty(x_ - koordynaty_prawe.x_, y_ - koordynaty_prawe.y_);
	}
	const Koordynaty operator/(const short& przez) const {
		short przez_poprawne(przez == 0 ? 1 : przez);// kiedyś może exception gdy przez=0
		return Koordynaty(x_ / przez_poprawne, y_ / przez_poprawne);
	}
	const Koordynaty operator*(const short& przez) const {
		return Koordynaty(x_ * przez, y_ * przez);
	}
	Koordynaty& operator+=(const Koordynaty& koordynaty_prawe) {
		x_ += koordynaty_prawe.x_;
		y_ += koordynaty_prawe.y_;
		return *this;
	}
	Koordynaty& operator=(const Koordynaty& koordynaty_prawe) {
		if (this == &koordynaty_prawe) return *this;
		x_ = koordynaty_prawe.x_;
		y_ = koordynaty_prawe.y_;
		return *this;
	}
	short x_, y_;
};

class BuforEkranu {
public:
	BuforEkranu() : szerokosc_wysokosc_(), zero_() {}
	CHAR_INFO& operator()(unsigned int wiersz, unsigned int kolumna) {
		return bufor_[szerokosc_wysokosc_.x_ * wiersz + kolumna];
	}
	const CHAR_INFO& operator()(unsigned int wiersz, unsigned int kolumna) const {
		return bufor_[szerokosc_wysokosc_.x_ * wiersz + kolumna];
	}
	void WczytajBufor(const Koordynaty& lewa_gora_, const Koordynaty& szerokosc_wysokosc);
	void WypiszBufor();
private:
	typedef std::vector< CHAR_INFO > CHAR_INFO_vector;
	CHAR_INFO_vector bufor_;
	const Koordynaty zero_;
	Koordynaty szerokosc_wysokosc_;
	SMALL_RECT region_;
};

/// Kolory.
/** Wygodniejszy sposób korzystania z kolorów w windows. */
enum Kolor
{
	Black		= 0,
	Grey		= FOREGROUND_INTENSITY,
	LightGrey	= FOREGROUND_RED	| FOREGROUND_GREEN	| FOREGROUND_BLUE,
	White		= FOREGROUND_RED	| FOREGROUND_GREEN	| FOREGROUND_BLUE |	FOREGROUND_INTENSITY,
	Blue		= FOREGROUND_BLUE,
	Green		= FOREGROUND_GREEN,
	Cyan		= FOREGROUND_GREEN	| FOREGROUND_BLUE,
	Red			= FOREGROUND_RED,
	Purple		= FOREGROUND_RED	| FOREGROUND_BLUE,
	LightBlue	= FOREGROUND_BLUE	| FOREGROUND_INTENSITY,
	LightGreen	= FOREGROUND_GREEN	| FOREGROUND_INTENSITY,
	LightCyan	= FOREGROUND_GREEN	| FOREGROUND_BLUE	| FOREGROUND_INTENSITY,
	LightRed	= FOREGROUND_RED	| FOREGROUND_INTENSITY,
	LightPurple	= FOREGROUND_RED	| FOREGROUND_BLUE	| FOREGROUND_INTENSITY,
	Orange		= FOREGROUND_RED	| FOREGROUND_GREEN,
	Yellow		= FOREGROUND_RED	| FOREGROUND_GREEN	| FOREGROUND_INTENSITY
};
/// Klawisze.
/** Skróty nazw wirtualnych klawiszy w windows. */
enum Klawisz
{
	ESC		= VK_ESCAPE,
	ENTER	= VK_RETURN,
	SPACE	= VK_SPACE,
	LEFT	= VK_LEFT,
	UP		= VK_UP,
	RIGHT	= VK_RIGHT,
	DOWN	= VK_DOWN,
	INNY	= 0x0,
	ZERO	= 0x40,
	JED		= 0x80,
	DWA		= 0x100,
	TRZ		= 0x200,
	CZT		= 0x400,
	PIE		= 0x800,
	SZE		= 0x1000,
	SIE		= 0x2000,
	OSI		= 0x4000,
	DZI		= 0x8000
};

void InicjalizujKonsole();
void WysrodkujOkno();
void ZamknijKonsole();
void OdswierzInformacjeOKonsoli();
Koordynaty RozmiarBuforaKonsoli();
/// Funkcja ustawiająca kursor na danej pozycji (x, y) w konsoli.
void UstawKursor(const Koordynaty& xy);
/// Funkcja ustawiająca rozmiar kursora oraz zmieniająca informacje o widzialnosci kursora.
void UstawRozmiarKursora(DWORD rozmiar_0_100, bool widoczny);
void WyczyscBuforKonsoli(const Kolor& kolor_czcionki = LightGrey, const Kolor& kolor_tla = Black);
/// Funkcja ustawiająca kolor wpisywanego tekstu w konsoli.
void UstawKolorTekstuITla(const Kolor& kolor_czcionki = LightGrey, const Kolor& kolor_tla = Black);
void ZmienKoloryObszaru(const Koordynaty& lewa_gora, const Koordynaty& prawy_dol,
						const Kolor& kolor_czcionki, const Kolor& kolor_tla);
Klawisz ZlapKlawisz();

} // namespace konsola

#endif // KONSOLA_H
