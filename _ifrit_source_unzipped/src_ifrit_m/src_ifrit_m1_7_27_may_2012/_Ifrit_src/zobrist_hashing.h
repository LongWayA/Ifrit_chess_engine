
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.05.10
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%


//!
#include "a_undo_struct.h"
#include "a_move_list_struct.h"
#include "a_position_struct.h"

#define TEST_H 0 // включаем-выключаем(1-0) режим вывода в файл тестовой информации

/*

Zobrist Hashing
http://chessprogramming.wikispaces.com/Zobrist+Hashing

 Zobrist Hashing,
 a technique to transform a board position of arbitrary size into 
 a number of a set length, with an equal distribution over all 
 possible numbers, invented by Albert Zobrist [1]. In an early 
 Usenet post in 1982, Tom Truscott mentioned Jim Gillogly's 
 n-bit hashing technique [2], who apparently read Zobrist's paper 
 early, and credits Zobrist in a 1997 rgcc post [3].

 The main purpose of Zobrist hash codes in chess programming 
 is to get an almost unique index number for any chess position, 
 with a very important requirement that two similar positions 
 generate entirely different indices. These index numbers are 
 used for faster and more space efficient Hash tables or databases, 
 e.g. transposition tables and opening books.

end link(конец цитаты)---------------------
*/

/*
 реализуем ZObrist-ключи
*/

namespace Zobrist_hashing
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

	/// РАБОТА С ХЕШ - КЛЮЧОМ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	///---------------------------------------------------------------------------
	/// каждой возможной позиции каждой возможной фигуры 
	/// присваиваем случайное число
	void ini_random_piese_key();

	///---------------------------------------------------------------------------
	/// инициализируем ключ начальной позиции
	void start_position_random_key
	(
		struct Position & position/// представление доски
	);

	///---------------------------------------------------------------------------
	/// меняем ключ во время хода
	void modification_random_key
	(
		struct Position & position,/// представление доски
		bool  colour_figure,/// цвет ходящей стороны
		unsigned __int8 i,/// номер хода по списку
		struct List & list_surplus_moves,/// представление списка ходов
		struct Undo & undo///информация для отката хода
	);


//////////////////////////////////////////////////////////////////////////////////////
//private:

	///---------------------------------------------------------------------------
	/// генерируем случайный 64 битный ключ
	unsigned __int64 rand_64();


//////////////////////////////////////////////////////////////////////////////////////
//public:

	/// ПОВТОРЫ ПОЗИЦИЙ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	///---------------------------------------------------------------------------
	/// возвращаем количество ходов в строке игры переданной из оболочки
	__int32 get_save_cnt_move();

	///---------------------------------------------------------------------------
	/// инициализируем хеш-массив повторов возрастающими числами что бы исключить ложное срабатывание
	void ini_hash_three();

	///---------------------------------------------------------------------------
	/// инициализируем хеш-массив повторов возрастающими числами что бы исключить ложное срабатывание
	void ini_hash_string_moves();

	///---------------------------------------------------------------------------
	/// запоминаем хеш-ключ на текущей глубине
	/// нужно для детектора повторов в линии варианта
	void save_hash_three
	(
		unsigned __int64 key,/// хеш-ключ
		unsigned __int8 depth /// глубина
	);

	///---------------------------------------------------------------------------
	/// запоминаем хеш-ключ для текущего хода
	/// нужно для детектора повторов в линии партии переданной оболочкой
	void save_hash_string_moves
	(
		unsigned __int64 key,/// хеш-ключ
		unsigned __int16 cnt_move /// счетчик ходов
	);

	///---------------------------------------------------------------------------
	/// анализируем линию ключей полученную из строки партии. если видим совпадение то возвращаем 1 иначе 0
	/// конечно совпадение не должно быть трехкратным ведь тогда это уже ничья
	bool analysis_hash_string_moves
	(
		unsigned __int8 depth /// глубина
	);

};