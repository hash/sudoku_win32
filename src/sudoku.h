#ifndef SUDOKU_H
#define SUDOKU_H

namespace sudoku
{
const unsigned int SMALL_SIZE = 3;					///< rozmiar malej planszy
const unsigned int SIZE = SMALL_SIZE * SMALL_SIZE;	///< rozmiar planszy
const unsigned int ref = 0x1FF;						///< 111111111 do sprawdzania czy są możliwości dla pola
}

#endif // SUDOKU_H
