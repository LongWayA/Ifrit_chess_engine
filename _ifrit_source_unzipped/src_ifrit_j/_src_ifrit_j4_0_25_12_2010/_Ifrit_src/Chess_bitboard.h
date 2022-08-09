#ifndef A_CHESS_BITBOARD_H
#define A_CHESS_BITBOARD_H

#include <cstring>

#include "Structure.h"
#include "List_surplus_moves.h"

// a_Chess_bitboard_make_move.cpp
#define MATERIAL 1 // включаем-выключаем(1-0) режим расчета материала

//---------------------------------------------------------------------------
/*++
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 20.11.2010
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 99%

Класс Chess_bitboard включает в себя внутреннее представление 
шахматной доски и методы для работы с ней.
такие как:
реализация ходов
инициализация доски по фен позиции
обнуление всех переменных представления доски

*/


// битовая доска
class Chess_bitboard
{
	// классы из которых возможен прямой доступ к закрытым полям
	friend class List_surplus_moves;
	friend class Evaluate;

// поля класса
//////////////////////////////////////////////////////////////////////////////////////////
private:
//public:
	//-------------------------------------------------
	// доска представлена следующим образом
	//A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
	//A7(48) B7(49) C7(50) D7(51) E7(52) F7(53) G7(54) H7(55)
	//A6(40) B6(41) C6(42) D6(43) E6(44) F6(45) G6(46) H6(47)
	//A5(32) B5(33) C5(34) D5(35) E5(36) F5(37) G5(38) H5(39)
	//A4(24) B4(25) C4(26) D4(27) E4(28) F4(29) G4(30) H4(31)
	//A3(16) B3(17) C3(18) D3(19) E3(20) F3(21) G3(22) H3(23)
	//A2(8)  B2(9)  C2(10) D2(11) E2(12) F2(13) G2(14) H2(15)
	//A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)
	//

	//A1(LSB)- это нулевой бит H8(MSB)-это 63 бит
	// т.е. битовая развертка выглядит следующим образом
	// H8(63) G8(62) F8(61) . . . . . .C1(2) B1(1) A1(0)
	//-------------------------------------------------
	// ДОСКА
	// в этих 64 битных переменных
	// пишется только факт наличия или отсутствия фигуры в каком то поле
	// причем неизвестно  что это за фигура известен только цвет по принадлежности
	// к той или иной переменной
	unsigned __int64  all_piece;// все фигуры
	unsigned __int64  all_white_piece;// все белые фигуры
	unsigned __int64  all_black_piece;// все черные фигуры

	//-------------------------------------------------
	// СПИСКИ ФИГУР
	/*
	имя фигуры
	0 - нет фигуры
	1 - пешка 
	2 - конь 
	3 - слон
	4 - ладья 
	5 - ферзь 
	6 - король
	*/
	// битовые списки белых и черных фигур
	unsigned __int64 white_piece[MAX_NAME];
	unsigned __int64 black_piece[MAX_NAME];

	// имя фигуры от координаты 
	unsigned __int8 white_name_from_coord[MAX_COORDINATE];
	unsigned __int8 black_name_from_coord[MAX_COORDINATE];

	//-------------------------------------------------
	// ВСПОМОГАТЕЛЬНАЯ ИНФОРМАЦИЯ
	// цвет хода 0 - черные 1 - белые
	bool colour_move;

	// разрешение взятия на проходе 1/0
	bool taking_passage_yes;

	// х координата битого поля(конвертируем из буквы)
	// так а - 0 , h - 7 таким образом по х отображение прямое
	unsigned __int8 taking_passage_x;

	// у координата битого поля
	// так '1' - 0 , '8' - 7 таким образом отображение тоже прямое только сдвинуто на 1
	unsigned __int8 taking_passage_y;

	// рокировка белых в длинную сторону   1/0
	bool castling_Q;

	// рокировка белых в короткую сторону  1/0
	bool castling_K;

	// рокировка черных в длинную сторону  1/0
	bool castling_q;

	// рокировка черных в короткую сторону 1/0
	bool castling_k;

	// 64 битный хеш-ключ позиции
	unsigned __int64 hash_key ;

	// оценка позиции материал за белых	
	unsigned __int16 value_white_material ;

	// оценка позиции материал за черных
	unsigned __int16 value_black_material ;

	// используем для delta pruning
	// запоминаем фигуры до резки
	unsigned __int64 all_black_or_white_piece;


// доступ к полям класса
//////////////////////////////////////////////////////////////////////////////////////////
public:

	// все фигуры
	inline unsigned __int64 get_all_piece()
	{
		return all_piece;
	};

	// все белые фигуры
	inline unsigned __int64 get_all_white_piece()
	{
		return all_white_piece;
	};

	// все черные фигуры
	inline unsigned __int64 get_all_black_piece()
	{
		return all_black_piece;
	};

	// битовые списки белых фигур
	inline unsigned __int64 get_white_piece(unsigned __int8 name_piece)
	{
		return white_piece[name_piece];
	};

	// битовые списки черных фигур
	inline unsigned __int64 get_black_piece(unsigned __int8 name_piece)
	{
		return  black_piece[name_piece];
	};


	//  имя фигуры от координаты 
	inline unsigned __int8 get_white_name_from_coord(unsigned __int8 coordinate_piece)
	{
		return white_name_from_coord[coordinate_piece];
	};

	// имя фигуры от координаты 
	inline unsigned __int8 get_black_name_from_coord(unsigned __int8 coordinate_piece)
	{
		return  black_name_from_coord[coordinate_piece];
	};

	// цвет хода 0 - черные 1 - белые
	inline bool get_colour_move()
	{
		return colour_move;
	};

	// разрешение взятия на проходе 1/0
	inline bool get_taking_passage_yes()
	{
		return taking_passage_yes;
	};

	// х координата битого поля(конвертируем из буквы)
	// так а - 0 , h - 7 таким образом по х отображение прямое
	inline unsigned __int8 get_taking_passage_x()
	{
		return taking_passage_x;
	};

	// у координата битого поля
	// так '1' - 0 , '8' - 7 таким образом отображение тоже прямое только сдвинуто на 1
	inline unsigned __int8 get_taking_passage_y()
	{
		return taking_passage_y;
	};

	// рокировка белых в длинную сторону   1/0
	// все черные фигуры
	inline bool get_castling_Q()
	{
		return castling_Q;
	};

	// рокировка белых в короткую сторону  1/0
	inline bool get_castling_K()
	{
		return castling_K;
	};

	// рокировка черных в длинную сторону  1/0
	inline bool get_castling_q()
	{
		return castling_q;
	};

	// рокировка черных в короткую сторону 1/0
	inline bool get_castling_k()
	{
		return castling_k;
	};

	// 64 битный хеш-ключ позиции
	inline unsigned __int64 get_hash_key()
	{
		return hash_key;
	};

	// оценка позиции материал за белых
	inline unsigned __int16 get_value_white_material()
	{
		return value_white_material;
	};

	// оценка позиции материал за черных
	inline unsigned __int16 get_value_black_material()
	{
		return value_black_material;
	};

	// используем для delta pruning
	inline unsigned __int64 get_all_black_or_white_piece()
	{
		return all_black_or_white_piece;
	};


// изменяем поля класса 
//////////////////////////////////////////////////////////////////////////////////////////
public:

	// 64 битный хеш-ключ позиции
	inline void set_hash_key(unsigned __int64 _hash_key)
	{
		hash_key = _hash_key;
	};

	// цвет хода 0 - черные 1 - белые
	inline void set_colour_move(bool _colour_move)
	{
		colour_move = _colour_move;
	};

	// разрешение взятия на проходе 1/0
	inline void set_taking_passage_yes(bool _taking_passage_yes)
	{
		taking_passage_yes = _taking_passage_yes;
	};

	// х координата битого поля(конвертируем из буквы)
	// так а - 0 , h - 7 таким образом по х отображение прямое
	inline void set_taking_passage_x(unsigned __int8 _taking_passage_x)
	{
		taking_passage_x = _taking_passage_x;
	};

	// у координата битого поля
	// так '1' - 0 , '8' - 7 таким образом отображение тоже прямое только сдвинуто на 1
	inline void set_taking_passage_y(unsigned __int8 _taking_passage_y)
	{
		taking_passage_y = _taking_passage_y;
	};

	// оценка позиции материал за белых
	inline void set_value_white_material(unsigned __int16 _value_white_material)
	{
		value_white_material = _value_white_material;
	};

	// оценка позиции материал за черных
	inline void set_value_black_material(unsigned __int16 _value_black_material)
	{
		value_black_material = _value_black_material;
	};


// реализация в файле
// a_Chess_bitboard_make_move.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// реализуем ходы вперед для белых
	void onward_moves_white
	(
		const unsigned __int8 & i,//номер хода по списку
		struct Undo undo[],//информация для отката хода
		const class List_surplus_moves & list_surplus_moves//список возможных ходов
	);

	//---------------------------------------------------------------------------
	// реализуем ходы вперед для черных
	void onward_moves_black
	(
		const unsigned __int8 & i,//номер хода по списку
		struct Undo undo[],//информация для отката хода
		const class List_surplus_moves & list_surplus_moves//список возможных ходов
	);

	//---------------------------------------------------------------------------
	// реализуем ходы назад для белых
	void backs_moves_white
	(
		const unsigned __int8 & i,//номер хода по списку
		const struct Undo undo[],//информация для отката хода
		const class List_surplus_moves & list_surplus_moves//список возможных ходов
	);

	//---------------------------------------------------------------------------
	// реализуем ходы назад для черных
	void backs_moves_black
	(
		const unsigned __int8 & i,//номер хода по списку
		const struct Undo undo[],//информация для отката хода
		const class List_surplus_moves & list_surplus_moves//список возможных ходов
	);


// реализация в файле
// a_Chess_bitboard_FEN.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// разбираем входящую фен - позицию
	void parse_position
	(
		std::string string_in//входящая строка
	);


	//---------------------------------------------------------------------------
	// начальная инициализация доски
	void bitboard_ini_t();


// реализация в файле
// a_Chess_bitboard_FEN.cpp
/////////////////////////////////////////////////////////////////////////////////////
private:

	//---------------------------------------------------------------------------
	// по букве выдает индекс клетки	
	// a-0 b-1 c-2 d-3 e-4 f-5 g-6 h-7 иначе 777 :)
	int koordinata_to_indeks
	(
		__int8 ch
	);

	//---------------------------------------------------------------------------
	//	по символу цифры выдает цифру
	// откровенно говоря сегодня я знаю как можно обойтись без этой функции :)
	// '1'-0 '2'-1 '3'-2 '4'-3 '5'-4 '6'-5 '7'-6 '8'-7 иначе 777 :)
	int ch_koordinata_to_indeks
	(
		int ch
	);

	//---------------------------------------------------------------------------
	// разбираем строку фен и инициализируем доску
	void fen_to_board
	(
		std::string position_fen//строка содержащая фен позицию
	);

	//---------------------------------------------------------------------------
	// загоняем в движок мувики т.е. ходы приводящие к текущей позиции из начальной
	void move_to_board
	(
		__int8 move_string[]// строка содержащая ходы(7)
	);


// реализация в файле
// a_Chess_bitboard_delta_pruning.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	//
	void delta_pruning_remember_piese_white();

	//---------------------------------------------------------------------------
	//
	void delta_pruning_white
	(
		const __int32 alpha,//
		__int32 & value_stat // статическая оценка
	);

	//---------------------------------------------------------------------------
	//
	void delta_pruning_return_piese_white();


	//---------------------------------------------------------------------------
	//
	void delta_pruning_remember_piese_black();


	//---------------------------------------------------------------------------
	//
	void delta_pruning_black
	(
		const __int32 beta,//
		__int32 & value_stat // статическая оценка
	);

	//---------------------------------------------------------------------------
	//
	void delta_pruning_return_piese_black();


// реализация в файле
// a_Chess_bitboard_test.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// использую для теста
	void print_name_from_coord();

	//---------------------------------------------------------------------------
	// использую для теста. записываем доску
	void save_test_bitboard
	(
		const class Chess_bitboard & bitboard// представление доски
	);

	//---------------------------------------------------------------------------
	// использую для теста. сравниваем доски
	void compare_test_bitboard
	(
		const class Chess_bitboard & bitboard// представление доски
	);

};

#endif // !defined A_CHESS_BITBOARD_H