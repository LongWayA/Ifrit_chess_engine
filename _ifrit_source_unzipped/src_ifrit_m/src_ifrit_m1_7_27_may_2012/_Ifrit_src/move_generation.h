
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 20.11.2010 18.09.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 99%




///!
#include "a_move_list_struct.h"
#include "a_position_struct.h"

/// включаем-выключаем(1-0) контроль списка ходов 
#define KONTROL_B 0 

/// 
#define TEST_H 0 

/*
Move Generation
http://chessprogramming.wikispaces.com/Move+Generation

 Generation of moves is a basic part of a chess engine with many variations. 
 The implementation heavily depends on the board representation, and it 
 can be generalized into two types:

Pseudo-legal
 In Pseudo-legal move generation pieces obey their normal rules of movement,
 but they're not checked beforehand to see if they'll leave the king in check.
 It is left up to the move-making function to test the move, or it is even 
 possible to let the king remain in check and only test for the capture of the 
 king on the next move.


Magic Bitboards 
http://chessprogramming.wikispaces.com/Magic+Bitboards

 Magic bitboards uses a multiply-right-shift perfect hashing algorithm 
 to index a attack bitboard database - which leaves both line-attacks 
 of a bishop or rook in one run.

 Thanks to the fast 64-bit multiplication and fast and huge caches of 
 recent processors, Magic Bitboards has become a de facto standard of 
 modern bitboard engines, as used for instance in Crafty, Arasan, Stockfish 
 and Houdini. While Robert Hyatt reported no immediate speed gain over 
 Rotated Bitboards in Crafty [1], Jon Dart mentioned a 20-25% speedup [2] 
 in Arasan over rotated.

end link(конец цитаты)---------------------
*/

/*
 реализуем битбоард(bit board) генератор
 переписал на Magic Bitboards

 опирался на следующие работы:
 "Hyatt, Robert. Rotated bitmaps, a new twist on an old idea"
 "Magic Move-Bitboard Generation in Computer Chess Pradyumna Kannan April 30, 2007"

 магические константы взял у Pradyumna Kannan из magicmoves.с
 вставить адрес источника

 по позиции заданной в структуре Position генерирует список ходов (структура List)
 список ходов избыточен потому что включает ходы под шах и открывающие шах
 + реализован детектор шахов

 проверка на шах в этом модуле не сделана потому что это требует движения фигур
 поэтому на шах позицию мы проверяем в другом месте после реализации хода

 ТЕСТИРОВАНИЕ
 тестируем в полном переборе на специальных тестовых позициях
 на данный момент(7.09.11) багов(bugs) не замечено
*/

///---------------------------------------------------------------------------
/// список возможных ходов 
namespace Move_generation 
{

/// функции для магии


//////////////////////////////////////////////////////////////////////////////////////
//private:




//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	///
	void ini_attacks_magic_masks_database();

	///---------------------------------------------------------------------------
	/// 
	inline unsigned __int64 get_bishop_attacks_masks_magic
	(
		const unsigned __int8 & square,/// расположение фигуры 
		const unsigned __int64 & position_all_piece ///
	);

	///---------------------------------------------------------------------------
	/// 
	inline unsigned __int64 get_rook_attacks_masks_magic
	(
		const unsigned __int8 & square,/// расположение фигуры 
		const unsigned __int64 & position_all_piece ///
	);


//////////////////////////////////////////////////////////////////////////////////////
//private:

	///---------------------------------------------------------------------------
	///
	unsigned __int64 ini_deg_masks
	(
		const int number_on_bit[], 
		const int count_attack_squares, 
		const unsigned __int64 deg
	);

	///---------------------------------------------------------------------------
	///
	unsigned __int64 ini_rook_attacks_masks
	(
		const int square, 
		const unsigned __int64 deg_masks
	);

	///---------------------------------------------------------------------------
	///
	unsigned __int64 ini_bishop_attacks_masks
	(
		const int square, 
		const unsigned __int64 deg_masks
	);


/// генератор ходов

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// генерируем список взятий + превращения пешки (для белых)
	void generator_captures_white
	(
		struct List & list,/// представление списка ходов
		const struct Position & position /// представление доски            
	);

	///---------------------------------------------------------------------------
	/// генерируем список взятий  + превращения пешки (для черных)
	void generator_captures_black
	(
		struct List & list,/// представление списка ходов
		const struct Position & position /// представление доски    
	);

	///---------------------------------------------------------------------------
	/// генерируем список остальных ходов 
	/// (т.е. всех ходов кроме взятий + превращения пешки)(для белых)
	void generator_move_white
	(
		struct List & list,/// представление списка ходов
		const struct Position & position /// представление доски    
	);

	///---------------------------------------------------------------------------
	/// генерируем список остальных ходов 
	/// (т.е. всех ходов кроме взятий + превращения пешки)(для черных)
	void generator_move_black
	(
		struct List & list,/// представление списка ходов
		const struct Position & position /// представление доски    
	);

	///---------------------------------------------------------------------------
	/// под боем ли король белых если шаха нет то функция 
	/// возвращает 0 иначе цифру шахующей фигуры
	unsigned __int8 king_white_check
	(
		const struct Position & position /// представление доски   
	);

	///---------------------------------------------------------------------------
	/// под боем ли король черных если шаха нет то функция 
	/// возвращает 0 иначе цифру шахующей фигуры
	unsigned __int8 king_black_check
	(
		const struct Position & position /// представление доски   
	);




/////////////////////////////////////////////////////////
///       мобильность фигур


//////////////////////////////////////////////////////////////////////////////////////
//private:


	///---------------------------------------------------------------------------
	/// под боем ли белые поля если шаха нет то функция 
	/// возвращает 0 иначе цифру шахующей фигуры
	inline unsigned __int8 white_check_square
	(
		const unsigned __int8 & sq,///индекс проверяемого поля(0-63) 
		const struct Position & position /// представление доски   
	);

	///---------------------------------------------------------------------------
	/// под боем ли черные поля если шаха нет то функция 
	/// возвращает 0 иначе цифру шахующей фигуры
	inline unsigned __int8 black_check_square
	(
		const unsigned __int8 & sq,///индекс проверяемого поля(0-63) 
		const struct Position & position /// представление доски   
	);

	///---------------------------------------------------------------------------
	/// заполняем список ходов - взятия за белых
	/// две функции только из за разного контроля списков(строки взятия)
	inline void list_capture_white
	(
		struct List & list,/// представление списка ходов
		const unsigned __int8 name,/// имя ходящей фигуры
		const unsigned __int8 type_move,/// тип хода
		const unsigned __int8 & from,/// откуда ходим
		const unsigned __int8 & to,/// куда ходим
		unsigned __int8 & count,/// номер хода в списке
		const struct Position & position /// представление доски    
	);

	///---------------------------------------------------------------------------
	/// заполняем список ходов - взятия за черных
	/// две функции только из за разного контроля списков(строки взятия)
	inline void list_capture_black
	(
		struct List & list,/// представление списка ходов
		const unsigned __int8 name,/// имя ходящей фигуры
		const unsigned __int8 type_move,/// тип хода
		const unsigned __int8 & from,/// откуда ходим
		const unsigned __int8 & to,/// куда ходим
		unsigned __int8 & count,/// номер хода в списке
		const struct Position & position /// представление доски    
	);

	///---------------------------------------------------------------------------
	/// заполняем список ходов
	inline void list_record_move_white
	(
		struct List & list,/// представление списка ходов
		const unsigned __int8 name,/// имя ходящей фигуры
		const unsigned __int8 type_move,/// тип хода
		const unsigned __int8 & from,/// откуда ходим
		const unsigned __int8 & to,/// куда ходим
		unsigned __int8 & count,/// номер хода в списке
		const struct Position & position /// представление доски    
	);

	///---------------------------------------------------------------------------
	/// заполняем список ходов
	inline void list_record_move_black
	(
		struct List & list,/// представление списка ходов
		const unsigned __int8 name,/// имя ходящей фигуры
		const unsigned __int8 type_move,/// тип хода
		const unsigned __int8 & from,/// откуда ходим
		const unsigned __int8 & to,/// куда ходим
		unsigned __int8 & count,/// номер хода в списке
		const struct Position & position /// представление доски    
	);

	///---------------------------------------------------------------------------
	/// заполняем список ходов для особых случаев типа рокировки 
	/// взятия на проходе и превращений пешек
	inline void list_record_special
	(
		struct List & list,/// представление списка ходов
		const unsigned __int8 name,/// имя ходящей фигуры
		const unsigned __int8 d_m,/// тип хода
		unsigned __int8 & count,/// номер хода в списке
		const unsigned __int8 & from,/// откуда ходим
		const unsigned __int8 to,/// куда ходим
		const unsigned __int8 capture/// имя взятой фигуры
	);

	///---------------------------------------------------------------------------
	/// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
	/// все это для белых фигур генератора взятий
	inline void KONTROL_captures_W
	(
		const struct Position & position, /// представление доски    
		const unsigned __int8 & from,/// откуда ходит фигура
		const unsigned __int8 & to,/// куда ходит фигура
		const unsigned __int8 & p,/// номер хода в списке
		const unsigned __int8 name_figure/// имя ходящей фигуры
	);

	///---------------------------------------------------------------------------
	/// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
	/// все это для черных фигур генератора взятий
	inline void KONTROL_captures_B
	(
		const struct Position & position, /// представление доски    
		const unsigned __int8 & from,/// откуда ходит фигура
		const unsigned __int8 & to,/// куда ходит фигура
		const unsigned __int8 & p,/// номер хода в списке
		const unsigned __int8 name_figure/// имя ходящей фигуры
	);

	///---------------------------------------------------------------------------
	/// контролируем выход за пределы доски, за пределы списка 
	/// для генератора ходов 
	inline void KONTROL_move
	(
		const unsigned __int8 & from,// откуда ходит фигура
		const unsigned __int8 & to,// куда ходит фигура
		const unsigned __int8 & p// номер хода в списке
	);


//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// ищем первый установленный бит(т.е. бежим справа)
	__inline __int32 bit_scan_forward_win32
	(
		const unsigned __int64 analyzed_number/// число в котором ищем взведенный бит
	);

	///---------------------------------------------------------------------------
	/// ищем последний установленный бит(т.е. бежим слева)
	__inline __int32 bit_scan_reverse_win32
	(
		const unsigned __int64 analyzed_number/// число в котором ищем взведенный бит
	);

};


