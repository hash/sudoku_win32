#include "ui.h"

namespace interfejs_urzytkownika {

ElementInterfejsu& ElementInterfejsu::NaSrodek() {
	if(w_kolumnie_elementow()) {
	} else {
		lewa_gora_ = RozmiarBuforaKonsoli();
		lewa_gora_.x_ /= 2;
		lewa_gora_.y_ /= 2;
		lewa_gora_.x_ -= rozmiar_.x_ / 2;
		lewa_gora_.y_ -= (rozmiar_.y_ / 2 + (rozmiar_.y_ % 2 != 0 ? 1 : 0)); // 1 linię w górę jeśli wysokość elementu nieparzysta
		prawy_dol_ = lewa_gora_ + rozmiar_;
	}
	return *this;
}

ElementInterfejsu& ElementInterfejsu::MarginesLewy(short margines) {
	if(w_kolumnie_elementow()) {
	} else {
		lewa_gora_.x_ = (margines < 0 ? marginesy().x_ : margines);
		prawy_dol_.x_ = lewa_gora_.x_ + rozmiar_.x_;
	}
	return *this;
}

ElementInterfejsu& ElementInterfejsu::MarginesGorny(short margines) {
	if(w_kolumnie_elementow()) {
	} else {
		lewa_gora_.y_ = (margines < 0 ? marginesy().y_ : margines);
		prawy_dol_.y_ = lewa_gora_.y_ + rozmiar_.y_;
	}
	return *this;
}

ElementInterfejsu& ElementInterfejsu::MarginesPrawy(short margines) {
	if(w_kolumnie_elementow()) {
	} else {
		prawy_dol_.x_ = RozmiarBuforaKonsoli().x_ - (margines < 0 ? marginesy().x_ : margines);
		lewa_gora_.x_ = prawy_dol_.x_ - rozmiar_.x_;
	}
	return *this;
}

ElementInterfejsu& ElementInterfejsu::MarginesDolny(short margines) {
	if(w_kolumnie_elementow()) {
	} else {
		prawy_dol_.y_ = RozmiarBuforaKonsoli().y_ - (margines < 0 ? marginesy().y_ : margines);
		lewa_gora_.y_ = prawy_dol_.y_ - rozmiar_.y_;
	}
	return *this;
}

void KolumnaElementow::DodajElement(ElementInterfejsu* element) {
	element->ustaw_w_kolumnie_elementow(this);
	rozmiar_.y_ += element->rozmiar().y_;
	rozmiar_.x_ = element->rozmiar().x_ > rozmiar_.x_ ? element->rozmiar().x_ : rozmiar_.x_ ;
	wektor_elementow_.push_back(element);
}

void KolumnaElementow::Uruchom() {
	if (!wektor_elementow_.empty()) {
		vector< ElementInterfejsu* >::iterator it = wektor_elementow_.begin();
		for ( ; it != wektor_elementow_.end() ; ++it) (*it)->Uruchom();
	}
}

void KolumnaElementow::UstawElementyWRelacjiDoKolumnyElementow() {
	if (!w_kolumnie_elementow()) {
		if (!wektor_elementow_.empty()) {
			Koordynaty temp(lewa_gora_);
			vector< ElementInterfejsu* >::iterator it = wektor_elementow_.begin();
			for ( ; it != wektor_elementow_.end() ; ++it) {
				temp.x_ = lewa_gora_.x_ + (rozmiar_.x_ - (*it)->rozmiar_.x_) / 2;
				(*it)->lewa_gora_ = temp;
				(*it)->prawy_dol_ = (*it)->lewa_gora_ + (*it)->rozmiar_;
				temp.y_ += (*it)->rozmiar_.y_;
			}
		}
	}
}

ElementInterfejsu& KolumnaElementow::NaSrodek() {
	ElementInterfejsu::NaSrodek();
	UstawElementyWRelacjiDoKolumnyElementow();
	return *this;
}

ElementInterfejsu& KolumnaElementow::MarginesPrawy(short margines) {
	ElementInterfejsu::MarginesPrawy(margines);
	UstawElementyWRelacjiDoKolumnyElementow();
	return *this;
}

ElementInterfejsu& KolumnaElementow::MarginesLewy(short margines) {
	ElementInterfejsu::MarginesLewy(margines);
	UstawElementyWRelacjiDoKolumnyElementow();
	return *this;
}

ElementInterfejsu& KolumnaElementow::MarginesGorny(short margines) {
	ElementInterfejsu::MarginesGorny(margines);
	UstawElementyWRelacjiDoKolumnyElementow();
	return *this;
}

ElementInterfejsu& KolumnaElementow::MarginesDolny(short margines) {
	ElementInterfejsu::MarginesDolny(margines);
	UstawElementyWRelacjiDoKolumnyElementow();
	return *this;
}

KolumnaElementow::~KolumnaElementow() {
	if (!wektor_elementow_.empty()) {
		vector< ElementInterfejsu* >::iterator it = wektor_elementow_.begin();
		for ( ; it != wektor_elementow_.end() ; ++it) {
			(*it)->kolumna_elementow_ = 0;
		}
	}
}

Splasz::Splasz(const string& nazwa) : ElementInterfejsu(nazwa) {
	IloscLini = 0;
	if (!nazwa.empty()) {
		vector< unsigned int > DlugosciLini;
		vector< string > VektorLini;
		string Linia;
		unsigned int DlLini, i, j;
		for (i = 0 ; i < nazwa_.size() ; ++i) {
			if (nazwa_[i] == 0x0A) {
				Linia = nazwa_;
				if (DlugosciLini.empty()) {
					DlugosciLini.push_back(i);
					Linia.erase(i);
				} else {
					DlLini = 0;
					for (j = 0 ; j < DlugosciLini.size() ; ++j) DlLini += DlugosciLini[j];
					DlugosciLini.push_back(i - DlLini);
					Linia.erase(i);
					Linia = Linia.substr(DlLini + 1);
				}
				VektorLini.push_back(Linia);
			}
		}
		DlLini = 0;
		for (i = 0 ; i < DlugosciLini.size() ; ++i)
			if (DlugosciLini[i] > DlLini) DlLini = DlugosciLini[i];
		if (DlLini % 2 != 0) ++DlLini;
		IloscLini = VektorLini.size();
		Linie.swap(VektorLini);
		prawy_dol_.x_ = rozmiar_.x_ = 2 * marginesy().x_ + DlLini;
		prawy_dol_.y_ = rozmiar_.y_ = 2 * marginesy().y_ + IloscLini;
	}
}

void Splasz::Uruchom() {
	Koordynaty kursor(lewa_gora_ + marginesy());
	for (unsigned int i = 0 ; i < IloscLini ; ++i, ++kursor.y_) {
		UstawKursor(kursor);
		cout << Linie[i];
	}
	ZmienKoloryObszaru(lewa_gora_, prawy_dol_, Grey, Black);
}

void Opis::Uruchom() {
	Koordynaty kursor(lewa_gora_ + marginesy());
	UstawKursor(kursor);
	cout << nazwa_;
	ZmienKoloryObszaru(lewa_gora_, prawy_dol_, Black, LightGrey);
}

void Gracz::Uruchom() {
	string buff;
	Koordynaty kursor(lewa_gora_ + marginesy());
	UstawKursor(kursor);
	cout << nazwa_;
	kursor.y_ += 1 + 2 * marginesy().y_;
	UstawKursor(kursor);
	ZmienKoloryObszaru(lewa_gora_, prawy_dol_, Black, White);
	ZmienKoloryObszaru(lewa_gora_, lewa_gora_ + Koordynaty(rozmiar_.x_, 1 + 2 * marginesy().y_), Black, LightGrey);
	UstawKolorTekstuITla(Black, White);
	UstawRozmiarKursora(10, true);
	std::getline(cin,buff);
	if (buff.empty()) {
		nazwa_gracza_ = "Anonymous";
	} else {
		nazwa_gracza_.swap(buff);
	}
	UstawRozmiarKursora(1, false);
}

void Opcje::DodajOpcje(const string opcja) {
	short rozmiar_opcjii(opcja.size() + 4 * marginesy().x_);
	rozmiar_.x_ = rozmiar_.x_ > rozmiar_opcjii ? rozmiar_.x_ : rozmiar_opcjii;
	rozmiar_.x_ += rozmiar_.x_ % 2 != 0 ? 1 : 0;
	wektor_opcji_.push_back(opcja);
	rozmiar_.y_ = (1 + marginesy().y_ * 2) * (wektor_opcji_.size() + (nazwa_.empty() ? 0 : 1)) + marginesy().y_ * 2;
	prawy_dol_ = lewa_gora_ + rozmiar_;
}

void Opcje::DodajOpcjeUcieczki(const string opcja) {
	opcja_ucieczki_ = wektor_opcji_.size();
	DodajOpcje(opcja);
}

void Opcje::Uruchom() {
	bool nazwa_jest(!nazwa_.empty());
	Koordynaty szerokosc_wysokosc_opcji(rozmiar_.x_ - 2 * marginesy().x_, 1 + 2 * marginesy().y_);
	if (nazwa_jest) {
		Koordynaty ofset_nazwy(lewa_gora_.x_ + (rozmiar_.x_ - nazwa_.size()) / 2, lewa_gora_.y_ + marginesy().y_);
		UstawKursor(ofset_nazwy);
		cout << nazwa_;
		ofset_nazwy.x_ = lewa_gora_.x_ + rozmiar_.x_;
		ofset_nazwy.y_ = lewa_gora_.y_ + szerokosc_wysokosc_opcji.y_;
		ZmienKoloryObszaru(lewa_gora_, ofset_nazwy, Black, LightGrey);
	}
	unsigned int ilosc_opcji(wektor_opcji_.size());
	Klawisz key;
	wybor_ = 0;
	vector< string >::iterator i = wektor_opcji_.begin(), k = wektor_opcji_.end();
	Koordynaty kursor(lewa_gora_ + marginesy() * 2);
	kursor.y_ += (nazwa_jest ? szerokosc_wysokosc_opcji.y_ : 0);
	for ( ; i != k ; ++i, kursor.y_ += szerokosc_wysokosc_opcji.y_) {
		kursor.x_ += (szerokosc_wysokosc_opcji.x_ - (2 * marginesy().x_ + i->size())) / 2;
		UstawKursor(kursor);
		cout << *i;
		kursor.x_ = lewa_gora_.x_ + 2 * marginesy().x_;
	}

	kursor = lewa_gora_;
	kursor.y_ += (nazwa_jest ? szerokosc_wysokosc_opcji.y_ : 0);
	ZmienKoloryObszaru(kursor, prawy_dol_, Black, White);
	kursor += marginesy();
	Koordynaty prawy_dol_opcji(szerokosc_wysokosc_opcji + kursor);
	ZmienKoloryObszaru(kursor, prawy_dol_opcji, White, Black);

	do {
		key = ZlapKlawisz();
		switch (key) {
			case UP: {
				if (wybor_ > 0) {
					ZmienKoloryObszaru(kursor, prawy_dol_opcji, Black, White);
					prawy_dol_opcji.y_ = kursor.y_;
					kursor.y_ -= szerokosc_wysokosc_opcji.y_;
					--wybor_;
					ZmienKoloryObszaru(kursor, prawy_dol_opcji, White, Black);
				}
				break;
			}
			case DOWN: {
				if (wybor_ < ilosc_opcji - 1) {
					ZmienKoloryObszaru(kursor, prawy_dol_opcji, Black, White);
					kursor.y_ = prawy_dol_opcji.y_;
					prawy_dol_opcji.y_ += szerokosc_wysokosc_opcji.y_;
					++wybor_;
					ZmienKoloryObszaru(kursor, prawy_dol_opcji, White, Black);
				}
				break;
			}
			case ESC: {
				wybor_ = opcja_ucieczki_;
				key = ENTER;
				break;
			}
			default : {
				break;
			}
		}
	}while (key != ENTER);
}

void Plansza::Podpowiedzi(const stany_Pola mozliwe) {
	Koordynaty rozmiar_podpowiedzi(15, 9);
	Koordynaty podpowiedzi(lewa_gora_.x_ + (rozmiar_.x_ + 8), lewa_gora_.y_);
	Koordynaty kursor(3, 2);
	ZmienKoloryObszaru(podpowiedzi, podpowiedzi + rozmiar_podpowiedzi, Blue, White);
	bufor_.WczytajBufor(podpowiedzi, rozmiar_podpowiedzi);
	for (unsigned int i(0) ; i < sudoku::SMALL_SIZE ; ++i) {
		for (unsigned int j(0) ; j < sudoku::SMALL_SIZE ; ++j) {
			if (mozliwe.test(i * sudoku::SMALL_SIZE + j)) {
				bufor_(kursor.y_, kursor.x_).Char.AsciiChar = 0x31 + (i * sudoku::SMALL_SIZE + j);
			} else {
				bufor_(kursor.y_, kursor.x_).Char.AsciiChar = '_';
			}
			kursor.x_ += 4;
		}
		kursor.y_ += 2;
		kursor.x_ = 3;
	}
	bufor_.WypiszBufor();
}

void Plansza::Uruchom() {
	NaSrodek();
	MarginesGorny(8);
	if (podpowiedzi_) {
		MarginesLewy(16);
	}
	PustaPlansza();
	obsluga(WypiszLiczby());
}

void Plansza::PustaPlansza() {
	unsigned int kolorLini(Grey | White << 4);
	int i(0), j(0);
	ZmienKoloryObszaru(lewa_gora_, lewa_gora_ + rozmiar_, Blue, White);
	bufor_.WczytajBufor(lewa_gora_, rozmiar_);

	bufor_(1, 7).Char.UnicodeChar = 'A';
	bufor_(1, 11).Char.UnicodeChar = 'B';
	bufor_(1, 15).Char.UnicodeChar = 'C';
	bufor_(1, 21).Char.UnicodeChar = 'D';
	bufor_(1, 25).Char.UnicodeChar = 'E';
	bufor_(1, 29).Char.UnicodeChar = 'F';
	bufor_(1, 35).Char.UnicodeChar = 'G';
	bufor_(1, 39).Char.UnicodeChar = 'H';
	bufor_(1, 43).Char.UnicodeChar = 'I';

	bufor_(4, 2).Char.UnicodeChar = '1';
	bufor_(6, 2).Char.UnicodeChar = '2';
	bufor_(8, 2).Char.UnicodeChar = '3';
	bufor_(12, 2).Char.UnicodeChar = '4';
	bufor_(14, 2).Char.UnicodeChar = '5';
	bufor_(16, 2).Char.UnicodeChar = '6';
	bufor_(20, 2).Char.UnicodeChar = '7';
	bufor_(22, 2).Char.UnicodeChar = '8';
	bufor_(24, 2).Char.UnicodeChar = '9';

	// pionowe linie
	for (i = 0 ; i < rozmiar_.y_ ; ++i) {
		bufor_(i, 0).Char.UnicodeChar = 0x2588;
		bufor_(i, 0).Attributes = ( kolorLini );
		bufor_(i, 47).Char.UnicodeChar = 0x2588;
		bufor_(i, 47).Attributes = ( kolorLini );
		bufor_(i, 4).Char.UnicodeChar = 0x2502;
		bufor_(i, 4).Attributes = ( kolorLini );
		bufor_(i, 18).Char.UnicodeChar = 0x2502;
		bufor_(i, 18).Attributes = ( kolorLini );
		bufor_(i, 32).Char.UnicodeChar = 0x2502;
		bufor_(i, 32).Attributes = ( kolorLini );
	}
	// poziome linie
	for (j = 1 ; j < rozmiar_.x_ - 1 ; ++j) {
		bufor_(26, j).Char.UnicodeChar = 0x2584;
		bufor_(26, j).Attributes = ( kolorLini );
		bufor_(0, j).Char.UnicodeChar = 0x2580;
		bufor_(0, j).Attributes = ( kolorLini );
		bufor_(2, j).Char.UnicodeChar = 0x2500;
		bufor_(2, j).Attributes = ( kolorLini );
		bufor_(10, j).Char.UnicodeChar = 0x2500;
		bufor_(10, j).Attributes = ( kolorLini );
		bufor_(18, j).Char.UnicodeChar = 0x2500;
		bufor_(18, j).Attributes = ( kolorLini );
	}
	// biale po bokach
	for (i = 1 ; i < rozmiar_.x_ - 1 ; ) {
		for (j = 2 ; j < rozmiar_.y_ - 7 ; ) {
			bufor_(j, i).Char.UnicodeChar = ' ';
			bufor_(j, i).Attributes = ( kolorLini );
			j += 8;
		}
		i += 45;
	}
	// krzyze
	for (i = 2 ; i < rozmiar_.y_ - 6 ; ) {
		for (j = 4 ; j < rozmiar_.x_ - 15 ; ) {
			bufor_(i, j).Char.UnicodeChar = 0x253C;
			bufor_(i, j).Attributes = ( kolorLini );
			j += 14;
		}
		i += 8;
	}
	bufor_.WypiszBufor();
}

Koordynaty Plansza::WypiszLiczby() {
	bool first(true);
	int i(0), j(0), x(0), y(0);
	Koordynaty lewa_gora(lewa_gora_.x_ + 7, lewa_gora_.y_ + 4);
	Koordynaty szerokosc_wysokosc(rozmiar_.x_ - 11, rozmiar_.y_ - 6);
	bufor_.WczytajBufor(lewa_gora, szerokosc_wysokosc);

	for (i = 0, x = 0 ; i < szerokosc_wysokosc.y_ + 1 ; ++x) {
		char buff[2];
		if (i == 6) i += 2;
		if (i == 14) i += 2;
		for (j = 0, y = 0 ; j < szerokosc_wysokosc.x_ + 2 ; ++y) {
			if (j == 12) j += 2;
			if (j == 26) j += 2;
			if (plansza(x, y).stan()) {
				_itoa_s(plansza(x, y).stan(), buff, 10);
			} else {
				strcpy_s(buff, "_");
			}
			bufor_(i, j).Char.UnicodeChar = buff[0];
			if (plansza(x, y).blokada()) {
				bufor_(i, j).Attributes = ( Black | White << 4 );
			} else {
				bufor_(i, j).Attributes = ( LightGrey | White << 4 );
				if (first) {
					lewa_gora.x_ = j + lewa_gora.x_;
					lewa_gora.y_ = i + lewa_gora.y_;
					X = y;
					Y = x;
					first = false;
				}
			}
			j += 4;
		}
		i += 2;
	}

	bufor_.WypiszBufor();

	return lewa_gora;
}

void Plansza::obsluga(Koordynaty xy) {
	UstawKolorTekstuITla(Green, White);
	UstawRozmiarKursora(99, true);
	UstawKursor(xy);
	Klawisz key;
	plansza.OgraniczenieMozliwosci();
	do {
		if (podpowiedzi_) Podpowiedzi(plansza(Y, X).mozliwe());
		key = ZlapKlawisz();
		switch (key) {
		case ENTER: {
				if (!plansza.Rozwiaz()) cout << "Nie da sie rozwiazac" << std::endl;
				xy = WypiszLiczby();
				break;
			}
		case SPACE: {
				plansza.WpiszRozwiazanie();
				xy = WypiszLiczby();
				break;
			}
		case UP: {
				if (Y - 1 >= 0) {
					--Y;
					xy.y_ -= 2;
					if (Y == 2 || Y == 5) xy.y_ -= 2;
				}
				break;
			}
		case DOWN: {
				if (Y + 1 < sudoku::SIZE) {
					++Y;
					xy.y_ += 2;
					if (Y == 3 || Y == 6) xy.y_ += 2;
				}
				break;
			}
		case LEFT: {
				if (X - 1 >= 0) {
					--X;
					xy.x_ -= 4;
					if (X == 2 || X == 5) xy.x_ -= 2;
				}
				break;
			}
		case RIGHT: {
				if (X + 1 < sudoku::SIZE) {
					++X;
					xy.x_ += 4;
					if (X == 3 || X == 6) xy.x_ += 2;
				}
				break;
			}
		case ZERO: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(0)) {
						UstawKolorTekstuITla(LightGrey, White);
						cout << "_";
						UstawKolorTekstuITla(Green, White);
					}
				break;
			}
		case JED: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(1))
						cout << plansza(Y, X).stan();
				break;
			}
		case DWA: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(2))
						cout << plansza(Y, X).stan();
				break;
			}
		case TRZ: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(3))
						cout << plansza(Y, X).stan();
				break;
			}
		case CZT: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(4))
						cout << plansza(Y, X).stan();
				break;
			}
		case PIE: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(5))
						cout << plansza(Y, X).stan();
				break;
			}
		case SZE: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(6))
						cout << plansza(Y, X).stan();
				break;
			}
		case SIE: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(7))
						cout << plansza(Y, X).stan();
				break;
			}
		case OSI: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(8))
						cout << plansza(Y, X).stan();
				break;
			}
		case DZI: {
				if (!plansza(Y, X).blokada())
					if (plansza(Y, X).Ustaw(9))
						cout << plansza(Y, X).stan();
				break;
			}
		default: {
				break;
			}
		}
		UstawKursor(xy);
		UstawKolorTekstuITla(Green, White);
		if (!plansza.CzyDaSieRozwiazac()) break;
	}while (key != ESC);
	UstawRozmiarKursora(1, false);
}

void Czas::Oblicz() {
	k = static_cast< double >(stop - start) +
		(static_cast< double >(millitmStart) - static_cast< double >(millitmStop)) / 1000;
	k = (k < 0) ? -k : k;
}

void Czas::Wypisz(string name) {
	name.append(" - ");
	name.append(toString(k));
	name.append(" s");
	Opis cz(name);
	cz.MarginesDolny(0).MarginesLewy(0);
	cz.Uruchom();
}

void Czas::DoPliku(string name) {
	name.append(".txt");
	plik.open(name.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	plik << k << std::endl;
	plik.close();
}

} // namespace interfejs_urzytkownika
