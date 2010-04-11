#include "konsola.h"

namespace konsola {
namespace {
CONSOLE_SCREEN_BUFFER_INFO informacje_o_konsoli;
/// Uchwyt do standardowego wyjścia ekranu konsoli.
const HANDLE out(GetStdHandle(STD_OUTPUT_HANDLE));
/// Uchwyt do standardowego wejścia ekranu konsoli.
const HANDLE in(GetStdHandle(STD_INPUT_HANDLE));
const DWORD tryb_konsoli(ENABLE_WINDOW_INPUT | ENABLE_ECHO_INPUT);
DWORD stary_tryb_konsoli;
} // namespace

void InicjalizujKonsole(){
	SetConsoleTitle(TEXT("Sudoku"));
	SetConsoleOutputCP(852);
	GetConsoleMode(in, &stary_tryb_konsoli);
	SetConsoleMode(in, tryb_konsoli);
	WysrodkujOkno();
	Sleep(100);
	FlushConsoleInputBuffer(in);
}

void WysrodkujOkno() {
	HWND uchwyt_okna_konsoli = GetConsoleWindow();
	SMALL_RECT nowy_rozmiar_okna_konsoli = {0, 0, 0, 0};
	RECT rozmiar_ekranu = {0, 0, 0, 0};
	Koordynaty rozmiar_bufora_konsoli(104, 58);
	nowy_rozmiar_okna_konsoli.Right = rozmiar_bufora_konsoli.x_ - 1;
	nowy_rozmiar_okna_konsoli.Bottom = rozmiar_bufora_konsoli.y_ - 1;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rozmiar_ekranu, 0);
	SetConsoleScreenBufferSize(out, rozmiar_bufora_konsoli);
	SetConsoleWindowInfo(out, TRUE, &nowy_rozmiar_okna_konsoli);
	SetWindowPos(uchwyt_okna_konsoli, HWND_TOP,
		rozmiar_ekranu.right / 16, rozmiar_ekranu.bottom / 16, 0, 0, SWP_NOSIZE);
}

void ZamknijKonsole(){
	SetConsoleMode(in, stary_tryb_konsoli);
	CloseHandle(out);
	CloseHandle(in);
}

void BuforEkranu::WczytajBufor(const Koordynaty& lewa_gora, const Koordynaty& szerokosc_wysokosc) {
	szerokosc_wysokosc_ = szerokosc_wysokosc;
	region_.Left = lewa_gora.x_;
	region_.Top = lewa_gora.y_;
	region_.Right = lewa_gora.x_ + szerokosc_wysokosc_.x_ - 1;
	region_.Bottom = lewa_gora.y_ + szerokosc_wysokosc_.y_ - 1;
	CHAR_INFO temp_char;
	temp_char.Char.UnicodeChar = 0;
	temp_char.Attributes = 0;
	CHAR_INFO_vector temp_bufor;
	temp_bufor.reserve(szerokosc_wysokosc_.x_ * szerokosc_wysokosc_.y_);
	temp_bufor.resize(szerokosc_wysokosc_.x_ * szerokosc_wysokosc_.y_, temp_char);
	ReadConsoleOutput(out, &temp_bufor[0], szerokosc_wysokosc_, zero_, &region_);
	temp_bufor.swap(bufor_);
}
void BuforEkranu::WypiszBufor() {
	WriteConsoleOutput(out, &bufor_[0], szerokosc_wysokosc_, zero_, &region_);
}

void OdswierzInformacjeOKonsoli() {
	GetConsoleScreenBufferInfo(out, &informacje_o_konsoli);
}

Koordynaty RozmiarBuforaKonsoli() {
	OdswierzInformacjeOKonsoli();
	Koordynaty rozmiar_bufora_konsoli(informacje_o_konsoli.srWindow.Right + 1,
									informacje_o_konsoli.srWindow.Bottom + 1);
	return rozmiar_bufora_konsoli;
}

void UstawKursor(const Koordynaty& xy) {
	SetConsoleCursorPosition (out, xy);
}

void UstawRozmiarKursora(DWORD rozmiar_0_100, bool widoczny) {
	CONSOLE_CURSOR_INFO CursorInfo = { rozmiar_0_100, widoczny };
	SetConsoleCursorInfo(out, &CursorInfo);
}

void WyczyscBuforKonsoli(const Kolor& kolor_czcionki, const Kolor& kolor_tla) {
	OdswierzInformacjeOKonsoli();
	Koordynaty gorny_lewy_rog_konsoli(0, 0);
	DWORD znaki_wpisane;
	DWORD ilosc_znakow_w_konsoli = informacje_o_konsoli.dwSize.X * informacje_o_konsoli.dwSize.Y;

	FillConsoleOutputCharacter(out, ' ', ilosc_znakow_w_konsoli, gorny_lewy_rog_konsoli, &znaki_wpisane);
	FillConsoleOutputAttribute(out, kolor_czcionki | kolor_tla << 4,
		ilosc_znakow_w_konsoli, gorny_lewy_rog_konsoli, &znaki_wpisane);
	UstawKursor(gorny_lewy_rog_konsoli);
	UstawKolorTekstuITla(kolor_czcionki, kolor_tla);
}

void UstawKolorTekstuITla(const Kolor& kolor_czcionki, const Kolor& kolor_tla) {
	SetConsoleTextAttribute (out, kolor_czcionki | kolor_tla << 4);
}

void ZmienKoloryObszaru(const Koordynaty& lewa_gora, const Koordynaty& prawy_dol,
						const Kolor& kolor_czcionki, const Kolor& kolor_tla) {
	DWORD cCharsWritten(0), szerokosc(prawy_dol.x_ - lewa_gora.x_);
	WORD nowy_kolor(kolor_czcionki | kolor_tla << 4);
	for (Koordynaty kursor(lewa_gora) ; kursor.y_ < prawy_dol.y_ ; ++kursor.y_) {
		FillConsoleOutputAttribute(out, nowy_kolor, szerokosc, kursor, &cCharsWritten);
	}
}

Klawisz ZlapKlawisz() {
	const unsigned int buffSize = 0x10;
	Klawisz k(INNY);
	DWORD cNumRead, i;
	INPUT_RECORD irInBuf[buffSize];
	while (k == INNY) {
		ReadConsoleInput(
				in,		// input buffer handle
				irInBuf,	// buffer to read into
				buffSize,	// size of read buffer
				&cNumRead);	// number of records read
		for (i = 0 ; i < cNumRead ; ++i) {
			if (irInBuf[i].EventType == KEY_EVENT) {
				if (!irInBuf[i].Event.KeyEvent.bKeyDown) {
					switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode) {
					case ESC: {
							k = ESC;
							break;
						}
					case ENTER: {
							k = ENTER;
							break;
						}
					case SPACE: {
							k = SPACE;
							break;
						}
					case UP: {
							k = UP;
							break;
						}
					case DOWN: {
							k = DOWN;
							break;
						}
					case LEFT: {
							k = LEFT;
							break;
						}
					case RIGHT: {
							k = RIGHT;
							break;
						}
					case 0x30: {
							k = ZERO;
							break;
						}
					case 0x31: {
							k = JED;
							break;
						}
					case 0x32: {
							k = DWA;
							break;
						}
					case 0x33: {
							k = TRZ;
							break;
						}
					case 0x34: {
							k = CZT;
							break;
						}
					case 0x35: {
							k = PIE;
							break;
						}
					case 0x36: {
							k = SZE;
							break;
						}
					case 0x37: {
							k = SIE;
							break;
						}
					case 0x38: {
							k = OSI;
							break;
						}
					case 0x39: {
							k = DZI;
							break;
						}
					case 0x60: {
							k = ZERO;
							break;
						}
					case 0x61: {
							k = JED;
							break;
						}
					case 0x62: {
							k = DWA;
							break;
						}
					case 0x63: {
							k = TRZ;
							break;
						}
					case 0x64: {
							k = CZT;
							break;
						}
					case 0x65: {
							k = PIE;
							break;
						}
					case 0x66: {
							k = SZE;
							break;
						}
					case 0x67: {
							k = SIE;
							break;
						}
					case 0x68: {
							k = OSI;
							break;
						}
					case 0x69: {
							k = DZI;
							break;
						}
					default: {
							k = INNY;
							break;
						}
					}
				}
			} else {
				k = INNY;
			}
		}
	}
	return k;
}

} // namespace konsola
