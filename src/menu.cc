#include "menu.h"

namespace menu {

using namespace konsola;
using namespace interfejs_urzytkownika;
using std::vector;

namespace {
Splasz sudoku("               _       _\n              | |     | |\n ___ _   _  __| | ___ | | ___   _\n/ __| | | |/ _` |/ _ \\| |/ / | | |\n\\__ \\ |_| | (_| | (_) |   <| |_| |\n|___/\\__,_|\\__,_|\\___/|_|\\_\\\\__,_|\n");
Opcje opcje_startowe, wybor_podpowiedzi("Podpowiadac?"), wybor_rodzaju_planszy("Jaki rodzaj planszy?"), wybor_poziomu_gry("Poziom gry.");
KolumnaElementow ekran_startowy;
Gracz edward("Wpisz swoje imie:");
Plansza PlanszaSudoku;

void Start() {
	InicjalizujKonsole();
	UstawRozmiarKursora(1, false);

	opcje_startowe.DodajOpcje("Nowa gra");
	opcje_startowe.DodajOpcjeUcieczki("Zakoncz");
	ekran_startowy.DodajElement(&sudoku);
	ekran_startowy.DodajElement(&opcje_startowe);
	ekran_startowy.NaSrodek().MarginesGorny(8);

	edward.NaSrodek().MarginesGorny(8);

	wybor_rodzaju_planszy.DodajOpcje("Generowana");
	wybor_rodzaju_planszy.DodajOpcje("Wlasna");
	wybor_rodzaju_planszy.NaSrodek().MarginesGorny(8);

	wybor_poziomu_gry.DodajOpcje("Bardzo latwy");
	wybor_poziomu_gry.DodajOpcje("Latwy");
	wybor_poziomu_gry.DodajOpcje("Trudny");
	wybor_poziomu_gry.DodajOpcje("Bardzo trudny");
	wybor_poziomu_gry.NaSrodek().MarginesGorny(8);

	wybor_podpowiedzi.DodajOpcje("Nie.");
	wybor_podpowiedzi.DodajOpcje("Tak.");
	wybor_podpowiedzi.NaSrodek().MarginesGorny(8);
}

void Koniec() {
	WyczyscBuforKonsoli();
	ZamknijKonsole();
}

/// Powitalne okno w menu wyboru (nowa gra lub zakoncz)
bool NowaGra() {
	WyczyscBuforKonsoli();
	ekran_startowy.Uruchom();
	if (opcje_startowe.wybor() == 0) {
		return true;
	} else {
		return false;
	}
}

/// Utworzenie planszy sudoku
void Gra() {
	WyczyscBuforKonsoli();
	edward.ZaczynaGre();
	PlanszaSudoku.Uruchom();
	edward.KonczyGre();
	if (wybor_rodzaju_planszy.wybor() == 0) edward.ZapisCzasuDoPliku();
	edward.WyswietlCzas();
	while (ZlapKlawisz() != ENTER) {}
}
} // namespace

void Uruchom() {
	Start();
	while (NowaGra()) {
		if (!edward.Ustawiony()) {
		/// Wpisywanie nowej nazwy użytkownika.
			WyczyscBuforKonsoli();
			edward.Uruchom();
		}
		/// Wybór rodzaju planszy (generowana lub własna)
		WyczyscBuforKonsoli();
		wybor_rodzaju_planszy.Uruchom();
		if (wybor_rodzaju_planszy.wybor() == 0) {
			/// Jesli generowane to jaki poziom
			WyczyscBuforKonsoli();
			wybor_poziomu_gry.Uruchom();
			/// Wybór trybu gry z podpowiadaniem (tak lub nie)
			WyczyscBuforKonsoli();
			wybor_podpowiedzi.Uruchom();
			if (wybor_podpowiedzi.wybor() == 0) {
				PlanszaSudoku.WylaczPodpowiedzi();
			} else {
				PlanszaSudoku.WlaczPodpowiedzi();
			}
			PlanszaSudoku.Generuj(wybor_poziomu_gry.wybor());
		} else {
			PlanszaSudoku.Reset();
		}
		Gra();
	}
	Koniec();
}

} // namespace menu
