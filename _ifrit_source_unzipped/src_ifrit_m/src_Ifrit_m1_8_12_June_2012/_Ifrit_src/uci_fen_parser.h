
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 20.11.2010
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%



///!
#include "a_position_struct.h"

/*
Forsyth-Edwards Notation
http://chessprogramming.wikispaces.com/Forsyth-Edwards+Notation

Forsyth-Edwards Notation (FEN) describes a Chess Position. 
It is an one-line ASCII-string. FEN is based on a system 
created by Scotsman David Forsyth in the 19th century. 
Steven Edwards specified the FEN standard for computer 
chess applications as part of the Portable Game Notation

end link(конец цитаты)---------------------
*/

namespace Uci_fen_parser
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

/*
	начальная инициализация доски (все параметры позиции загоняем в ноль)
*/
	void bitboard_ini
	(
		struct Position & position// представление доски
	);

/*
	по букве выдает индекс клетки	
	'a'-0 'b'-1 'c'-2 'd'-3 'e'-4 'f'-5 'g'-6 'h'-7 иначе 777 :)
*/
	int koordinata_to_indeks
	(
		const __int8 ch
	);

/*
	по символу цифры выдает цифру
	откровенно говоря сегодня я знаю как можно обойтись без этой функции :)
	'1'-0 '2'-1 '3'-2 '4'-3 '5'-4 '6'-5 '7'-6 '8'-7 иначе 777 :)
*/
	int ch_koordinata_to_indeks
	(
		const int ch
	);

/*
	работаем со строкой вида "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	символом / отделяются горизонтали шахматной доски
	цифры означают количество пустых клеток на горизонтали
	буквы это фигуры причем большие буквы значат белый цвет а маленькие черный
	теперь нам осталось пояснить подстроку вида
	w - - 0 1  ход белых рокировок нет 
	b KQkq e3 0 1 ход черных есть все рокировки и взятие на проходе
	напомним как кодируется битое поле:
	например после хода 1. e4:
	rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
	другими словами нам просто передают координаты 
	битого поля (при ходе е4 битое поле е3)

	разбираем фен - позицию и инициализируем внутреннюю позицию 
*/
	void fen_to_board
	(
		struct Position & position,// представление доски
		const std::string position_fen//строка содержащая фен позицию
	);
};