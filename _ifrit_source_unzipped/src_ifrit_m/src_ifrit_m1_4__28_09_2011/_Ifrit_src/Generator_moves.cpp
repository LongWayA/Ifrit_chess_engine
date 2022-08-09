
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 21.02.2009  21.11.2010 18.09.2011 27.09.2011

#include <fstream>
#include <iostream>

#include "Search_switch.h"
#include "List_const.h"
#include "Position_const.h"
#include "Generator_moves.h"
#include "Generator_moves_const.h"
#include "History.h"

#include "Test_chess_bitboard.h"


unsigned __int64 bishop_attacks_masks_database[64][512]; /// 1<<9
unsigned __int64 rook_attacks_masks_database[64][4096]; /// 1<<12


//	===================================================================
//
void Generator_moves::ini_attacks_magic_masks_database()
{
	int number_on_bit[64];
	int count_bishop_attack_squares;// считаем атакованные слоном поля в маске bishop_masks
	unsigned __int64 bishop_masks;// маска атак слоном на пустой доске
	unsigned __int64 deg_masks;
	int index;
	int count_rook_attack_squares;// считаем атакованные ладьей поля в маске rook_masks
	unsigned __int64 rook_masks;// маска атак ладьей на пустой доске

//----------------------------------------------------
// для слона
	for( int square = 0; square < 64; square++ )
	{
		count_bishop_attack_squares = 0;
		bishop_masks = bishop_full_masks[square];

		// посчитали атакованные слоном биты (count_bishop_attack_squares)
		// и записали позицию каждого взведенного бита (number_on_bit[])
		while ( bishop_masks )
		{
			number_on_bit[count_bishop_attack_squares++] = bit_scan_forward_win32(bishop_masks);
			bishop_masks = bishop_masks & (bishop_masks - 1);
		}

		// здесь мы находим 2 в степени количества атакованных слоном полей
		// т.е. через move_masks считаем степень :)
		// таким образом мы определяем количество возможных комбинаций битов
		unsigned __int64  deg = move_masks[count_bishop_attack_squares];

		for(unsigned __int64  i = 0; i < deg; i++ )
		{
			// выдаем всевозможные маски атакованных битов (в данном случае слоном)
			// каждый раз одну маску но в итоге перебираем все возможные комбинации битов
			deg_masks = ini_deg_masks(number_on_bit,count_bishop_attack_squares,i);

			index = ( deg_masks * bishop_magics[square] ) >> BISHOP_BITS_SHIFT;
			bishop_attacks_masks_database[square][index] = ini_bishop_attacks_masks(square,deg_masks);
		}
	}

//----------------------------------------------------
// для ладьи
	for( int square = 0; square < 64; square++ )
	{
		count_rook_attack_squares = 0;
		rook_masks = rook_full_masks[square];

		while ( rook_masks )
		{
			number_on_bit[count_rook_attack_squares++] = bit_scan_forward_win32(rook_masks);
			rook_masks = rook_masks & (rook_masks - 1);
		}

		unsigned __int64  deg = move_masks[count_rook_attack_squares];

		//deg = 20;

		for( unsigned __int64  i = 0; i < deg; i++ )
		{
			deg_masks = ini_deg_masks(number_on_bit,count_rook_attack_squares,i);
			index = ( deg_masks * rook_magics[square] ) >> ROOK_BITS_SHIFT;
			rook_attacks_masks_database[square][index] = ini_rook_attacks_masks(square,deg_masks);

		}
	}
}

//	===================================================================
//
unsigned __int64 Generator_moves::ini_deg_masks
(
	const int number_on_bit[], 
	const int count_attack_squares, 
	const unsigned __int64 deg
)
{
	unsigned __int64 deg_masks = 0;

/*
основа заключается в том что битовая маска чисел следующая
1,           // 0 bit т.е. ....0001
2,           // 1 bit т.е. ....0010
2,           //       т.е. ....0011
4,           // 2 bit т.е. ....0100 
и т.д.
т.е. мы имеем всевозможные комбинации взведенных битов
в нашем случае роль такого числа играет deg
осталось только отобразить это многообразие на маску атаки фигур
в нашем случае слонов и ладьей
*/

	for( int i = 0; i < count_attack_squares; i++ )
	{
		if( deg & move_masks[i] )
		{
			deg_masks = deg_masks | move_masks[ number_on_bit[i] ];
		}
	}

	return deg_masks;
}

//	===================================================================
// генерируем маску атак ладьи
unsigned __int64 Generator_moves::ini_rook_attacks_masks
(
	const int square, 
	const unsigned __int64 deg_masks
)
{
	unsigned __int64 rook_attacks_masks = 0;
	unsigned __int64 blocking_square_mask;
	int blocking_coordinate;

		// луч вверх------------------------------------------------------------------
		blocking_square_mask = deg_masks & rook_up_masks[square];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			rook_attacks_masks = rook_attacks_masks | (rook_up_masks[square] ^ rook_up_masks[blocking_coordinate]);
		}
		else
		{
			rook_attacks_masks = rook_attacks_masks | rook_up_masks[square];
		}

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = deg_masks & rook_left_masks[square];
		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			rook_attacks_masks = rook_attacks_masks | (rook_left_masks[square] ^ rook_left_masks[blocking_coordinate]);
		}
		else
		{
			rook_attacks_masks = rook_attacks_masks | rook_left_masks[square];
		}

		// луч вниз------------------------------------------------------------------
		blocking_square_mask = deg_masks & rook_down_masks[square];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			rook_attacks_masks = rook_attacks_masks | (rook_down_masks[square] ^ rook_down_masks[blocking_coordinate]);
		}
		else
		{
			rook_attacks_masks = rook_attacks_masks |  rook_down_masks[square];
		}

		// луч вправо-----------------------------------------------------------------
		blocking_square_mask = deg_masks & rook_right_masks[square];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			rook_attacks_masks = rook_attacks_masks | (rook_right_masks[square] ^ rook_right_masks[blocking_coordinate]);
		}
		else
		{
			rook_attacks_masks = rook_attacks_masks |  rook_right_masks[square];
		}

	return rook_attacks_masks;
}

//	===================================================================
//  генерируем маску атак слона
unsigned __int64 Generator_moves::ini_bishop_attacks_masks
(
	const int square, 
	const unsigned __int64 deg_masks
)
{

	unsigned __int64 bishop_attacks_masks = 0;
	unsigned __int64 blocking_square_mask; // маска блокированных полей 
	__int32 blocking_coordinate; // координата блокированного поля 

	// смотрим в четырех различных направлениях (по типу слона)

	// луч вверх - вправо--------------------------------------------------------
	blocking_square_mask = deg_masks & bishop_up_right_masks[square];

	if(blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		bishop_attacks_masks = bishop_attacks_masks | ( bishop_up_right_masks[square] ^ bishop_up_right_masks[blocking_coordinate] );
	}
	else
	{
		bishop_attacks_masks = bishop_attacks_masks | bishop_up_right_masks[square];
	}

	// луч вверх - влево---------------------------------------------------------
	blocking_square_mask = deg_masks & bishop_up_left_masks[square];

	if(blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		bishop_attacks_masks = bishop_attacks_masks | ( bishop_up_left_masks[square] ^ bishop_up_left_masks[blocking_coordinate] );
	}
	else
	{
		bishop_attacks_masks = bishop_attacks_masks | bishop_up_left_masks[square];
	}

	// луч вниз - влево----------------------------------------------------------
	blocking_square_mask = deg_masks & bishop_down_left_masks[square];

	if(blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		bishop_attacks_masks = bishop_attacks_masks | ( bishop_down_left_masks[square] ^ bishop_down_left_masks[blocking_coordinate] );
	}
	else
	{
		bishop_attacks_masks = bishop_attacks_masks | bishop_down_left_masks[square];
	}

	// луч вниз - вправо---------------------------------------------------------
	blocking_square_mask = deg_masks & bishop_down_right_masks[square];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		bishop_attacks_masks = bishop_attacks_masks | ( bishop_down_right_masks[square] ^ bishop_down_right_masks[blocking_coordinate] );
	}
	else
	{
		bishop_attacks_masks = bishop_attacks_masks | bishop_down_right_masks[square];
	}

	return bishop_attacks_masks;

}

//	===================================================================
//
inline unsigned __int64 Generator_moves::get_bishop_attacks_masks_magic
(
	const unsigned __int8 & square,/// расположение фигуры 
	const unsigned __int64 & position_all_piece ///
)
{
	int index = 
	( ( position_all_piece & bishop_full_masks[square] ) * bishop_magics[square] )
	>> BISHOP_BITS_SHIFT ;
	
	return bishop_attacks_masks_database[square][index];
}

//	===================================================================
//
inline unsigned __int64 Generator_moves::get_rook_attacks_masks_magic
(
	const unsigned __int8 & square,/// расположение фигуры 
	const unsigned __int64 & position_all_piece ///
)
{

	int index = 
	( ( position_all_piece & rook_full_masks[square] ) * rook_magics[square] ) 
	>> ROOK_BITS_SHIFT ;

	return rook_attacks_masks_database[square][index];
}



//	===================================================================
// генерируем список взятий + превращения пешки (для белых)
void Generator_moves::generator_captures_white
(
	struct List & list,// представление списка ходов
	const struct Position & position // представление доски    
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_conversion_mask; // маска ходов пешки с превращеинем
	unsigned __int64 captures_conversion_mask; // маска взятий пешки с превращениями
	unsigned __int64 captures_mask; // маска взятий
	unsigned __int64 attack_mask;

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит 

	// генерируем ходы белых фигур
	p = 0;

	// ходы пешки----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//выделяем одну пешку 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		/* 
		отображение доски напрямую задаем формулой:
		line_index = 8 * rank_index + file_index
		to = 8 * position.taking_passage_y + position.taking_passage_x;

		обратное отображение прямой на доску:
		file_index   = line_index modulo 8 = line_index % 8  = line_index & 7
		rank_index   = line_index div 8    = line_index / 8  = line_index >> 3

		здесь у нас:
		line_index – координата одномерной доски (от 0 до 63)
		rank_index – координата доски по вертикали(от 0 до 7)
		file_index – координата доски по горизонтали (от 0 до 7)
		*/

		// считаем ходы с превращением и взятия с превращением
		if ( from / 8 == 6 )// если мы на предпоследней горизонтали т.е. следующий ход
							// пешки с превращением
		{
			// 1 находим простые ходы белых пешек на одно поле вперед
			move_conversion_mask = (one_pawn_mask << 8) & (~(position.all_piece));

			// взятия с превращением
			captures_conversion_mask = pawn_capture_white[from] &  position.all_black_piece;

			// находим координаты ходов с превращением пешки
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// отмечаем превращение пешки
				// превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,превращение в коня 12
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0);
 
#if KONTROL_B
				KONTROL_move(from,to,p);
#endif//KONTROL_B
			}

			// находим координаты взятий с превращением пешки  
			while (captures_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(captures_conversion_mask);
				captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

				// отмечаем превращение пешки с взятием
				// превращение в ферзь 25,превращение в ладью 24,превращение в слона 23,превращение в коня 22
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,position.black_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,position.black_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,position.black_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,position.black_name_from_coord[to]);

#if KONTROL_B
				KONTROL_captures_W(position,from,to,p,120);
#endif//KONTROL_B
			}

		}
		else
		{
			// считаем взятия пешкой
			captures_mask = pawn_capture_white[from] &  position.all_black_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_white(list,N_PAWN,CAPTURE_MOVE,from,to,p,position);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 находим взятия на проходе
		if (position.taking_passage_yes == 1)
		{
			// черная пешка может сходить на два поля только с начальной позиции
			// значит только если белая пешка стоит на 4 считая с нуля горизонтали
			// черная пешка может схотить через битое поле т.е. 
			// нужно проверять взятие на проходе
			if ( from / 8 == 4 )
			{
				to = 8 * position.taking_passage_y + position.taking_passage_x;

				// смотрим взятие на право. пешка не должна быть на самой правой вертикали
				if (((from - to) == -9) && (from % 8 != 7))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.black_name_from_coord[to - 8]);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(position,from,to,p,130);
#endif//KONTROL_B
				}

				// смотрим взятие на налево. пешка не должна быть на самой левой вертикали
				if (((from - to) == -7) && (from % 8 != 0))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.black_name_from_coord[to - 8]);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(position,from,to,p,130);
#endif//KONTROL_B
				}
			}
		}
	}
	// пешки----------------------------------------------------------------------------------


	// ходы коня ----------------------------------------------------------------------------------
	// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
	// расположения коней обнуляя тот бит который мы уже посмотрели
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (position.all_black_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(list,N_KNIGHT,CAPTURE_MOVE,from,to,p,position);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// коня ----------------------------------------------------------------------------------


	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);


		attack_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		captures_mask = attack_mask & position.all_black_piece;

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			captures_mask = captures_mask & (captures_mask - 1);
			list_capture_white(list,N_BISHOP,CAPTURE_MOVE,from,to,p,position);
		}

	}
	// слона---------------------------------------------------------------------------------- 


	// ходы ладьи---------------------------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		attack_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		captures_mask = attack_mask & position.all_black_piece;

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			captures_mask = captures_mask & (captures_mask - 1);
			list_capture_white(list,N_ROOK,CAPTURE_MOVE,from,to,p,position);
		}

	}
	// ладьи----------------------------------------------------------------------------------


	// ходы ферзя---------------------------------------------------------------------------------
	// маска расположения белого ферзя
	from_mask = position.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);


		captures_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		attack_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		captures_mask = (captures_mask | attack_mask) & position.all_black_piece;

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			captures_mask = captures_mask & (captures_mask - 1);
			list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,to,p,position);
		}
	}
	// ферзя--------------------------------------------------------------------------------------


	// ходы короля--------------------------------------------------------------------------------
	// маска расположения белого короля
	from_mask = position.white_piece[N_KING];

	if (from_mask != 0)
	{
		// находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
		from = bit_scan_forward_win32(from_mask);

		// находим маску взятий короля
		captures_mask = (position.all_black_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			// Заполняем список ходов
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(list,N_KING,CAPTURE_MOVE,from,to,p,position);
			captures_mask = captures_mask & (captures_mask - 1);

		}

	}
	// ходы короля--------------------------------------------------------------------------------

	list.end_list = p;
	list.end_captures = p;

}

//	===================================================================
// генерируем список взятий  + превращения пешки (для черных)
void Generator_moves::generator_captures_black
(
	struct List & list,// представление списка ходов
	const struct Position & position // представление доски   
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_conversion_mask; // маска ходов пешки с превращеинем
	unsigned __int64 captures_conversion_mask; // маска взятий пешки с превращениями
	unsigned __int64 captures_mask; // маска взятий
	unsigned __int64 attack_mask;

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит

	// генерируем ходы черных фигур
	p = 0;

	// ходы пешки----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// выделяем одну пешку 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		if ( from / 8 == 1 )
		{
			// считаем ходы с превращением и взятия с превращением
			// 1 находим простые ходы черных пешек на одно поле вперед
			move_conversion_mask = (one_pawn_mask >> 8) & (~(position.all_piece));

			// взятия с превращением
			captures_conversion_mask = pawn_capture_black[from] &  position.all_white_piece;

			// находим координаты ходов с превращением пешки
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// отмечаем превращение пешки
				// превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,
				// превращение в коня 12
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0);
 
#if KONTROL_B
				KONTROL_move(from,to,p);
#endif//KONTROL_B
			}

			// находим координаты взятий с превращением пешки  
			// находим координаты взятий с превращением пешки  
			while (captures_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(captures_conversion_mask);
				captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

				// отмечаем превращение пешки с взятием
				// превращение в ферзь 25,превращение в ладью 24,превращение в слона 23,
				// превращение в коня 22
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,position.white_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,position.white_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,position.white_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,position.white_name_from_coord[to]);

#if KONTROL_B
				KONTROL_captures_B(position,from,to,p,120);
#endif//KONTROL_B
			}
		}
		else
		{
			// считаем взятия пешкой
			captures_mask =  pawn_capture_black[from] &  position.all_white_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_black(list,N_PAWN,CAPTURE_MOVE,from,to,p,position);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 находим взятия на проходе
		if (position.taking_passage_yes == 1)
		{
			if ( from / 8 == 3 )
			{
				to = 8 * position.taking_passage_y + position.taking_passage_x;

				if (((from - to) == 9) && (from % 8 != 0))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.white_name_from_coord[to + 8]);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(position,from,to,p,130);
#endif//KONTROL_B
				}

				if (((from - to) == 7) && (from % 8 != 7))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.white_name_from_coord[to + 8]);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(position,from,to,p,130);
#endif//KONTROL_B
				}
			}
		}
	}
	// пешки----------------------------------------------------------------------------------


	// ходы коня ----------------------------------------------------------------------------------
	// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
	// расположения коней обнуляя тот бит который мы уже посмотрели
	// Генерируем ходы
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (position.all_white_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
		   to = bit_scan_forward_win32(captures_mask);
		   list_capture_black(list,N_KNIGHT,CAPTURE_MOVE,from,to,p,position);
		   captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// коня ----------------------------------------------------------------------------------

	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);


		attack_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		captures_mask = attack_mask & position.all_white_piece;

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			captures_mask = captures_mask & (captures_mask - 1);
			list_capture_black(list,N_BISHOP,CAPTURE_MOVE,from,to,p,position);
		}

	}
	// слона---------------------------------------------------------------------------------- 


	// ходы ладьи---------------------------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		attack_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		captures_mask = attack_mask & position.all_white_piece;

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			captures_mask = captures_mask & (captures_mask - 1);
			list_capture_black(list,N_ROOK,CAPTURE_MOVE,from,to,p,position);
		}

	}
	// ладьи----------------------------------------------------------------------------------


	// ходы ферзя---------------------------------------------------------------------------------
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);


		captures_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		attack_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		captures_mask = (captures_mask | attack_mask) & position.all_white_piece;

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			captures_mask = captures_mask & (captures_mask - 1);
			list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,to,p,position);
		}
	}
	// ферзя--------------------------------------------------------------------------------------

	// ходы короля---------------------------------------------------------------------------------
	from_mask = position.black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		captures_mask = (position.all_white_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_black(list,N_KING,CAPTURE_MOVE,from,to,p,position);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// короля---------------------------------------------------------------------------------


	list.end_list = p;
	list.end_captures = p;

}

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)
//(для белых)
void Generator_moves::generator_move_white
(
	struct List & list,// представление списка ходов
	const struct Position & position // представление доски   
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 move_mask; // 
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_mask_d; // маска хода пешки на два поля

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит

	p = list.end_list;

	// генерируем ходы белых фигур
	// ходы короля--------------------------------------------------------------------------------
	// маска расположения белого короля
	from_mask = position.white_piece[N_KING];

	if (from_mask != 0)
	{
		// находим координату фигуры(самый первый(для нашего отображения правый ) 
		// установленный бит)
		from = bit_scan_forward_win32(from_mask);

		// находим маску ходов короля(без взятий)
		move_mask = king_moves_masks[from] & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_KING,SIMPLE_MOVE,from,to,p,position);
		}

		// рокировки белого короля

		// если длинная рокировка разрешена
		if (position.castling_Q)
		{
			// 
			to = C1;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// поля  1 2 3 пустые  то добавляем ход
			if (((~position.all_piece) & CASTLING_Q ) == CASTLING_Q)
			{
				// длинная рокировка белых  0-0-0 проверяем под боем ли поля 2 3 4 
				if ((white_check_square(C1,position) == 0) 
				&& (white_check_square(D1,position) == 0) 
				&& (white_check_square(E1,position) == 0))
				{
					list_record_special(list,N_KING,LONG_CASTLING,p,from,to,0);
				}
			}
		}

		// если короткая рокировка разрешена то добавляем ход 
		if (position.castling_K)
		{
			to = G1;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			//поля  5 6 пустые  то добавляем ход
			if (((~position.all_piece) & CASTLING_K ) == CASTLING_K)
			{
				// короткая рокировка белых 0-0 проверяем под боем ли поля  4 5 6
				if ((white_check_square(E1,position) == 0)
				&& (white_check_square(F1,position) == 0)
				&& (white_check_square(G1,position) == 0))
				{
					list_record_special(list,N_KING,SHORT_CASTLING,p,from,to,0);
				}
			}
		}
	}

	// ходы ферзя----------------------------------------------------------------------------------
	// маска расположения белого ферзя
	from_mask = position.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// надо пояснить что конструкция типа b = b & (b-1)  
		// сбрасывает самый правый единичный бит 
		from_mask = from_mask & (from_mask - 1);

		unsigned __int64 mobility_mask = 0; // маска мобильности

		move_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		//move_mask = move_mask ^ (position.all_piece & move_mask);
		move_mask = move_mask & (~position.all_piece);

		mobility_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		//mobility_mask = mobility_mask ^ (position.all_piece & mobility_mask);
		mobility_mask = mobility_mask & (~position.all_piece);

		move_mask = move_mask |	mobility_mask;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
	}

	// ходы ладьи----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		move_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		//move_mask = move_mask ^ (position.all_piece & move_mask);
		move_mask = move_mask & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
	}

	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		move_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		//move_mask = move_mask ^ (position.all_piece & move_mask);
		move_mask = move_mask & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}

	}

	// ходы коня ----------------------------------------------------------------------------------
	// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
	// расположения коней обнуляя тот бит который мы уже посмотрели
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		move_mask = knigt_moves_masks[from] & (~position.all_piece); // маска мобильности

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_KNIGHT,SIMPLE_MOVE,from,to,p,position);
		}
	}

	// коды пешки----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//выделяем одну пешку 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 находим простые ходы белых пешек на одно поле вперед
		move_mask = (one_pawn_mask << 8) & (~(position.all_piece));

		// 2 находим ходы пешек на 2 поля вперед 
		// в начале оставляем только пешки стоящие на 2 горизонтали
		move_mask_d = one_pawn_mask & LINE_1;

		// отметаем пешки если они чем то заблокированны
		move_mask_d = (move_mask_d << 8) & (~(position.all_piece));

		// оставшимися пешками делаем ход
		move_mask_d = (move_mask_d << 8) & (~(position.all_piece));

		// здесь все ходы пешки как на одно так на два поля вперед
		move_mask = (move_mask | move_mask_d) ;

		// ходы без превращений
		move_mask = move_mask & ~LINE_7;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_PAWN,SIMPLE_MOVE,from,to,p,position);
		}
	}

	list.end_list = p;

}

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)(для черных)
void Generator_moves::generator_move_black
(
	struct List & list,// представление списка ходов
	const struct Position & position // представление доски   
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 move_mask; // маска ходов здесь только для ходов пешки с превращением
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_mask_d; // маска хода пешки на два поля

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит

	p = list.end_list;

	// генерируем ходы черных фигур
	// ходы короля---------------------------------------------------------------------------------
	from_mask = position.black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// находим маску ходов короля(без взятий)
		move_mask = king_moves_masks[from] & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_KING,SIMPLE_MOVE,from,to,p,position);
		}

		// рокировки черного короля

		// если длинная рокировка разрешена
		if (position.castling_q)
		{
			to = C8;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// и поля  1 2 3 пустые  то добавляем ход
			if ((~(position.all_piece) & CASTLING_q )== CASTLING_q)
			{
				// длинная рокировка черных проверяем 0-0-0 58 59 60
				if ((black_check_square(C8,position) == 0)
				&& (black_check_square(D8,position) == 0)
				&& (black_check_square(E8,position) == 0))
				{
					list_record_special(list,N_KING,LONG_CASTLING,p,from,to,0);			 
				}
			}
		}

		// если короткая рокировка разрешена то добавляем ход 
		if (position.castling_k)
		{
			to = G8;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// и поля  5 6 пустые  то добавляем ход
			if ((~(position.all_piece) & CASTLING_k )== CASTLING_k)
			{
				// короткая рокировка черных проверяем 0-0 60 61 62
				if ((black_check_square(E8,position) == 0)
				&& (black_check_square(F8,position) == 0)
				&&(black_check_square(G8,position) == 0))
				{
					list_record_special(list,N_KING,SHORT_CASTLING,p,from,to,0);
				}
			}
		}
	}

	// ходы ферзя----------------------------------------------------------------------------------
	// маска расположения ферзя
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает 
		//самый правый единичный бит 
		from_mask = from_mask & (from_mask - 1);

		unsigned __int64 mobility_mask = 0; // маска мобильности

		move_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		//move_mask = move_mask ^ (position.all_piece & move_mask);
		move_mask = move_mask & (~position.all_piece);

		mobility_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		//mobility_mask = mobility_mask ^ (position.all_piece & mobility_mask);
		mobility_mask = mobility_mask & (~position.all_piece);

		move_mask = move_mask |	mobility_mask;


		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
	}

	// ходы ладьи----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		move_mask = get_rook_attacks_masks_magic(from,position.all_piece);
		//move_mask = move_mask ^ (position.all_piece & move_mask);
		move_mask = move_mask & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
	}

	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		move_mask = get_bishop_attacks_masks_magic(from,position.all_piece);
		//move_mask = move_mask ^ (position.all_piece & move_mask);
		move_mask = move_mask & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
	}

	// ходы коня ----------------------------------------------------------------------------------
	// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
	// расположения коней обнуляя тот бит который мы уже посмотрели
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		move_mask = knigt_moves_masks[from] & (~position.all_piece); // маска мобильности

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_KNIGHT,SIMPLE_MOVE,from,to,p,position);
		}
	}

	// коды пешки----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 находим простые ходы черных пешек на одно поле вперед
		move_mask = (one_pawn_mask >> 8) & (~(position.all_piece));

		// 2 находим ходы пешек на 2 поля вперед 
		// в начале оставляем только пешки стоящие на 7 горизонтали
		move_mask_d = one_pawn_mask & LINE_6;

		// отметаем пешки если они чем то заблокированны
		move_mask_d = (move_mask_d >> 8) & (~(position.all_piece));

		// оставшимися пешками делаем ход
		move_mask_d = (move_mask_d >> 8) & (~(position.all_piece));

		// здесь все ходы пешки как на одно так на два поля вперед
		move_mask = (move_mask | move_mask_d) ;

		// ходы без превращений
		move_mask = move_mask & ~LINE_0;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_PAWN,SIMPLE_MOVE,from,to,p,position);
		}
	}

	list.end_list = p;

}

//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 Generator_moves::king_white_check
(
	const struct Position & position // представление доски   
)
{

	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 attack_mask; // маска блокированных полей
	unsigned __int64 capture_mask; // маска взятий


	unsigned __int8 from; // координата поля откуда фигура ходит

	// маска расположения белого короля
	from_mask = position.white_piece[N_KING];

	// находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
	from = bit_scan_forward_win32(from_mask);

	// все атаки ладьи
    attack_mask = get_rook_attacks_masks_magic(from,position.all_piece);

	// теперь смотрим есть ли в их числе черные ладьи
	capture_mask = attack_mask & position.black_piece[N_ROOK];
	if (capture_mask != 0) return N_ROOK;

	// теперь смотрим есть ли в их числе черные ферзи
	capture_mask = attack_mask & position.black_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;

	// все атаки слона
    attack_mask = get_bishop_attacks_masks_magic(from,position.all_piece);

	// теперь смотрим есть ли в их числе черные слоны 
	capture_mask = attack_mask & position.black_piece[N_BISHOP];
	if (capture_mask != 0) return N_BISHOP;

	// теперь смотрим есть ли в их числе черные ферзи
	capture_mask = attack_mask & position.black_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;


	// все атаки коня
	capture_mask = (knigt_moves_masks[from] & position.black_piece[N_KNIGHT]);
	if (capture_mask != 0) return N_KNIGHT;


	// по типу короля
	capture_mask = (king_moves_masks[from] & position.black_piece[N_KING]);
	if (capture_mask != 0) return N_KING;

// пешки считаем напрямую
	if ((from / 7) < 8)
	{
		if (from % 8 != 0)
		{
			if (position.black_name_from_coord[from + 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}

		if (from % 8 != 7)
		{
			if (position.black_name_from_coord[from + 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}
	}

	return 0; 
}


//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 Generator_moves::king_black_check
(
	const struct Position & position // представление доски   
)
{

	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 attack_mask; // маска блокированных полей
	unsigned __int64 capture_mask; // маска взятий


	unsigned __int8 from; // координата поля откуда фигура ходит

	// маска расположения черного короля
	from_mask = position.black_piece[N_KING];

	// находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
	from = bit_scan_forward_win32(from_mask);

	// все атаки ладьи
    attack_mask = get_rook_attacks_masks_magic(from,position.all_piece);

	// теперь смотрим есть ли в их числе белые ладьи
	capture_mask = attack_mask & position.white_piece[N_ROOK];
	if (capture_mask != 0) return N_ROOK;

	// теперь смотрим есть ли в их числе белые ферзи
	capture_mask = attack_mask & position.white_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;

	// все атаки слона
    attack_mask = get_bishop_attacks_masks_magic(from,position.all_piece);

	// теперь смотрим есть ли в их числе белые слоны 
	capture_mask = attack_mask & position.white_piece[N_BISHOP];
	if (capture_mask != 0) return N_BISHOP;

	// теперь смотрим есть ли в их числе белые ферзи
	capture_mask = attack_mask & position.white_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;


	// все атаки коня
	capture_mask = (knigt_moves_masks[from] & position.white_piece[N_KNIGHT]);
	if (capture_mask != 0) return N_KNIGHT;


	// по типу короля
	capture_mask = (king_moves_masks[from] & position.white_piece[N_KING]);
	if (capture_mask != 0) return N_KING;

	// пешки считаем напрямую
	if ((from / 7) > 1 )
	{
		if (from % 8 != 7)
		{
			if (position.white_name_from_coord[from - 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}
 
		if (from % 8 != 0)
		{
			if (position.white_name_from_coord[from - 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}
	}

	return 0;
 
}


//private

//	===================================================================
// под боем ли поля белых если шаха нет то функция возвращает 0
// иначе цифру шахующей фигуры
inline unsigned __int8 Generator_moves::white_check_square
(
	const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
	const struct Position & position // представление доски   
)
{
	unsigned __int64 attack_mask; // маска блокированных полей
	unsigned __int64 capture_mask; // маска взятий

	// все атаки ладьи
    attack_mask = get_rook_attacks_masks_magic(sq,position.all_piece);

	// теперь смотрим есть ли в их числе черные ладьи
	capture_mask = attack_mask & position.black_piece[N_ROOK];
	if (capture_mask != 0) return N_ROOK;

	// теперь смотрим есть ли в их числе черные ферзи
	capture_mask = attack_mask & position.black_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;

	// все атаки слона
    attack_mask = get_bishop_attacks_masks_magic(sq,position.all_piece);

	// теперь смотрим есть ли в их числе черные слоны 
	capture_mask = attack_mask & position.black_piece[N_BISHOP];
	if (capture_mask != 0) return N_BISHOP;

	// теперь смотрим есть ли в их числе черные ферзи
	capture_mask = attack_mask & position.black_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;


	// все атаки коня
	capture_mask = (knigt_moves_masks[sq] & position.black_piece[N_KNIGHT]);
	if (capture_mask != 0) return N_KNIGHT;


	// по типу короля
	capture_mask = (king_moves_masks[sq] & position.black_piece[N_KING]);
	if (capture_mask != 0) return N_KING;

// пешки считаем напрямую
	if ((sq / 7) < 8)
	{
		if (sq % 8 != 0)
		{
			if (position.black_name_from_coord[sq + 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}

		if (sq % 8 != 7)
		{
			if (position.black_name_from_coord[sq + 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}
	}

	return 0; 

}

//	===================================================================
// под боем ли  поля черных если шаха нет 
// то функция возвращает 0 иначе цифру шахующей фигуры
inline unsigned __int8 Generator_moves::black_check_square
(
	const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
	const struct Position & position // представление доски   
)
{
	unsigned __int64 attack_mask; // маска блокированных полей
	unsigned __int64 capture_mask; // маска взятий

	// все атаки ладьи
    attack_mask = get_rook_attacks_masks_magic(sq,position.all_piece);

	// теперь смотрим есть ли в их числе белые ладьи
	capture_mask = attack_mask & position.white_piece[N_ROOK];
	if (capture_mask != 0) return N_ROOK;

	// теперь смотрим есть ли в их числе белые ферзи
	capture_mask = attack_mask & position.white_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;

	// все атаки слона
    attack_mask = get_bishop_attacks_masks_magic(sq,position.all_piece);

	// теперь смотрим есть ли в их числе белые слоны 
	capture_mask = attack_mask & position.white_piece[N_BISHOP];
	if (capture_mask != 0) return N_BISHOP;

	// теперь смотрим есть ли в их числе белые ферзи
	capture_mask = attack_mask & position.white_piece[N_QUEEN];
	if (capture_mask != 0) return N_QUEEN;


	// все атаки коня
	capture_mask = (knigt_moves_masks[sq] & position.white_piece[N_KNIGHT]);
	if (capture_mask != 0) return N_KNIGHT;


	// по типу короля
	capture_mask = (king_moves_masks[sq] & position.white_piece[N_KING]);
	if (capture_mask != 0) return N_KING;

	// пешки считаем напрямую
	if ((sq / 7) > 1 )
	{
		if (sq % 8 != 7)
		{
			if (position.white_name_from_coord[sq - 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}
 
		if (sq % 8 != 0)
		{
			if (position.white_name_from_coord[sq - 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}
	}

	return 0; 
}

//	===================================================================
// заполняем список ходов - взятия за белых
// две функции только из за разного контроля списков(строки взятия)
inline void Generator_moves::list_capture_white
(
	struct List & list,// представление списка ходов
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 type_move,// тип хода
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 & to,// куда ходим
	unsigned __int8 & count,// номер хода в списке
	const struct Position & position // представление доски   
)
{
	unsigned __int8 capture;// взятая фигура

#if KONTROL_B
	KONTROL_captures_W(position,from,to,count,name);
#endif//KONTROL_B

	// взятая фигура
	capture = position.black_name_from_coord[to];

	// записываем оценку взятия(mvv/lva в моем понимании) для последующей сортировки
	list.sorting_score[count] = 10 * capture - name; 

	// 6 бит 
	// записали имя взятой фигуры (1 - пешка 2 - конь 3 - слон 4 - ладья 5 - ферзь 
	// 6 - король)
	list.move[count] = capture

	// 6 бит 
	// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
	//4 – короткая рокировка 5 – взятие на проходе
	//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 
	//15 - превращение в ферзь 
	//22 - взятие с превращением в конь 23 - взятие с превращением в слон 
	//24 - взятие с превращением в ладья 
	//25 - взятие с превращением в ферзь )
	| (type_move << 6)

	// 6 бит 
	// записали откуда ходим (0 -63)
	| (from << 12)

	// 6 бит 
	// записали куда ходим (0 -63)
	| (to << 18)

	// 3 бит 
	// ходящая фигура (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "move[count]= "<< move[count] << "\n";
	//
	//std::cout << "position.black_name_from_coord[to]= "<< position.black_name_from_coord[to] << "\n";
	//std::cout << "position.black_name_from_coord[to]= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "type_move= "<< type_move << "\n";
	//std::cout << "type_move= "<< ((move[count]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[count]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[count]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[count]>>24)& 7)  << "\n";
	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// заполняем список ходов - взятия за черных
// две функции только из за разного контроля списков(строки взятия)
inline void Generator_moves::list_capture_black
(
	struct List & list,// представление списка ходов
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 type_move,// тип хода
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 & to,// куда ходим
	unsigned __int8 & count,// номер хода в списке
	const struct Position & position // представление доски   
)
{
	unsigned __int8 capture;// взятая фигура

#if KONTROL_B
	KONTROL_captures_B(position,from,to,count,name);
#endif//KONTROL_B

	// взятая фигура
	capture = position.white_name_from_coord[to];

	// записываем оценку взятия(mvv/lva в моем понимании) для последующей сортировки
	list.sorting_score[count] = 10 * capture - name; 

	// 6 бит 
	// записали имя взятой фигуры (1 - пешка 2 - конь 3 - слон 4 - ладья 5 - ферзь 6 - король)
	list.move[count] = capture

	// 6 бит 
	// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
	//4 – короткая рокировка 5 – взятие на проходе
	//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
	//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
	//25 - взятие с превращением в ферзь )
	| (type_move << 6)

	// 6 бит 
	// записали откуда ходим (0 -63)
	| (from << 12)

	// 6 бит 
	// записали куда ходим (0 -63)
	| (to << 18)

	// 3 бит 
	// ходящая фигура (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "move[count]= "<< move[count] << "\n";
	//
	//std::cout << "position.white_name_from_coord[to]= "<< position.white_name_from_coord[to] << "\n";
	//std::cout << "position.white_name_from_coord[to]= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "type_move= "<< type_move << "\n";
	//std::cout << "type_move= "<< ((move[count]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[count]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[count]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[count]>>24)& 7)  << "\n";
	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// заполняем список ходов
inline void Generator_moves::list_record_move_white
(
	struct List & list,// представление списка ходов
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 type_move,// тип хода
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 & to,// куда ходим
	unsigned __int8 & count,// номер хода в списке
	const struct Position & position // представление доски   
)
{

#if KONTROL_B
	KONTROL_move(from,to,count);
#endif//KONTROL_B

#if SW_HISTORY
	//  записываем историю
	list.sorting_score[count] = History::get_white_history(name,to); 
#endif//#if SW_HISTORY

	// 6 бит 
	// взятая фигура 0
	list.move[count] = 0

	// 6 бит 
	// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
	//4 – короткая рокировка 5 – взятие на проходе
	//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
	//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
	//25 - взятие с превращением в ферзь )
	| (type_move << 6)

	// 6 бит 
	// записали откуда ходим (0 -63)
	| (from << 12)

	// 6 бит 
	// записали куда ходим (0 -63)
	| (to << 18)

	// 3 бит 
	// ходящая фигура (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "move[p]= "<< move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// заполняем список ходов
inline void Generator_moves::list_record_move_black
(
	struct List & list,// представление списка ходов
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 type_move,// тип хода
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 & to,// куда ходим
	unsigned __int8 & count,// номер хода в списке
	const struct Position & position // представление доски   
)
{

#if KONTROL_B
	KONTROL_move(from,to,count);
#endif//KONTROL_B

#if SW_HISTORY
	// записываем историю
	list.sorting_score[count] =  History::get_black_history(name,to); 
#endif//#if SW_HISTORY

	// 6 бит 
	// взятая фигура 0
	list.move[count] = 0

	// 6 бит 
	// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
	//4 – короткая рокировка 5 – взятие на проходе
	//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
	//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
	//25 - взятие с превращением в ферзь )
	| (type_move << 6)

	// 6 бит 
	// записали откуда ходим (0 -63)
	| (from << 12)

	// 6 бит 
	// записали куда ходим (0 -63)
	| (to << 18)

	// 3 бит 
	// ходящая фигура (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "move[p]= "<< move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	count = count + 1;
}


//	===================================================================
// заполняем список ходов для особых случаев типа рокировки взятия на проходе и превращений пешек
inline void Generator_moves::list_record_special
(
	struct List & list,// представление списка ходов
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 d_m,// тип хода
	unsigned __int8 & count,// номер хода в списке
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 to,// куда ходим
	const unsigned __int8 capture// имя взятой фигуры
)
{
	// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// тут рокировка превращение пешек взятие на проходе 
	// таких ходов мало и лучше их посмотреть первыми среди простых
	list.sorting_score[count] = 110000;   //d_m;

	// 6 бит 
	// взятая фигура 0
	list.move[count] = capture

	// 6 бит 
	// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
	//4 – короткая рокировка 5 – взятие на проходе
	//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
	//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
	//25 - взятие с превращением в ферзь )
	| (d_m << 6)

	// 6 бит 
	// записали откуда ходим (0 -63)
	| (from << 12)

	// 6 бит 
	// записали куда ходим (0 -63)
	| (to << 18)

	// 3 бит 
	// ходящая фигура (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "move[p]= "<< move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для белых фигур генератора взятий
inline void Generator_moves::KONTROL_captures_W
(
	const struct Position & position, // представление доски   
	const unsigned __int8 & from,// откуда ходит фигура
	const unsigned __int8 & to,// куда ходит фигура
	const unsigned __int8 & p,// номер хода в списке
	const unsigned __int8 name_figure// имя ходящей фигуры
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((p < 0)||(p > 256))
	{
		std::cout << "W ERROR Generator_captures p= "   << p    << "\n";
	}

	if ((from < 0)||(from > 63))
	{
		std::cout << "W ERROR Generator_captures from= " << from << "\n";
	}

	if ((to < 0)||(to > 63))
	{
		std::cout << "W ERROR Generator_captures to= "  << to   << "\n";
	}

	if ((position.white_name_from_coord[from] < 1)
	||(position.white_name_from_coord[from] > 6))
	{
		std::cout << "W ERROR Generator_captures position.white_name_from_coord[*from] = "  
			<< position.white_name_from_coord[from]<< "\n";
	}

	if ((position.black_name_from_coord[to] < 1)
	||(position.black_name_from_coord[to] > 6))
	{
		std::cout << "W ERROR Generator_captures position.black_name_from_coord[*to]= " 
			<< position.black_name_from_coord[to]   << "\n";
		std::cout << "W ERROR Generator_captures from= "<< from << "\n";
		std::cout << "W ERROR Generator_captures to= "  << to   << "\n";
		std::cout << "W ERROR Generator_captures name_figure= "  << name_figure << "\n";
		position_print(position);
	}

	//if (position.black_name_from_coord[to] ==6)
	//{
		//std::cout << "ERROR Generator_captures name_figure KING "  << "\n";
	//}
	///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для черных фигур генератора взятий
inline void Generator_moves::KONTROL_captures_B
(
	const struct Position & position, // представление доски   
	const unsigned __int8 & from,// откуда ходит фигура
	const unsigned __int8 & to,// куда ходит фигура
	const unsigned __int8 & p,// номер хода в списке
	const unsigned __int8 name_figure// имя ходящей фигуры
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((p < 0)||(p > 256))
	{
		std::cout << "B ERROR Generator_captures p= "   << p    << "\n";
	}

	if ((from < 0)||(from > 63))
	{
		std::cout << "B ERROR Generator_captures from= " << from << "\n";
	}

	if ((to < 0)||(to > 63))
	{
		std::cout << "B ERROR Generator_captures to= "  << to   << "\n";
	}

	if ((position.black_name_from_coord[from] < 1)
	||(position.black_name_from_coord[from] > 6))
	{
		std::cout << "B ERROR Generator_captures position.black_name_from_coord[from] = "
			<< position.black_name_from_coord[from]   << "\n";
	}

	if ((position.white_name_from_coord[to] < 1)
	||(position.white_name_from_coord[to] > 6))
	{
		std::cout << "B ERROR Generator_captures position.white_name_from_coord[to]= "
			<< position.white_name_from_coord[to]   << "\n";
		std::cout << "B ERROR Generator_captures from= "<< from << "\n";
		std::cout << "B ERROR Generator_captures to= "  << to   << "\n";
		std::cout << "B ERROR Generator_captures name_figure= "  << name_figure << "\n";
		position_print(position);
	}

	//if (position.white_name_from_coord[to] ==6)
	//{
	//std::cout << "B ERROR Generator_captures name_figure KING "  << "\n";
	//}
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// контролируем выход за пределы доски, за пределы списка 
// для генератора ходов 
inline void Generator_moves::KONTROL_move
(
	const unsigned __int8 & from,// откуда ходит фигура
	const unsigned __int8 & to,// куда ходит фигура
	const unsigned __int8 & p// номер хода в списке
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((p < 0)||(p > 256))
	{
		std::cout << "ERROR Generator_move p= "   << p    << "\n";
	}

	if ((from < 0)||(from > 63))
	{
		std::cout << "ERROR Generator_move from= " << from << "\n";
	}

	if ((to < 0)||(to > 63))
	{
		std::cout << "ERROR Generator_move to= "  << to   << "\n";
	}
	///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

/*
Для тех кто интересуется ассемблером могу порекомендовать 
великолепную книгу
Юрий Магда. - Ассемблер для процессоров Intel Pentium.
в ней почти нету досовской(dos) херни которой полны другие учебники
и которая совершенно не актуальна сегодня
(все эти сегменты и прочая дребедень :))
*/

//	===================================================================
// ищем первый установленный бит(т.е. бежим справа)
__inline __int32 Generator_moves::bit_scan_forward_win32
(
	const unsigned __int64 analyzed_number// число в котором ищем взведенный бит
)
{ 
	_asm
	{ 
		/*
		из учебника по ассемблеру(This document saved from http://mind.telenet.ru/samouch/7/31/index.html):
		Команда mov замещает первый операнд (приемник) вторым (источником). 
		При этом исходное значение первого операнда теряется. 
		Второй операнд не изменяется. В зависимости от описания операндов, 
		пересылается слово или байт. Если операнды описаны по-разному или 
		режим адресации не позволяет однозначно определить размер операнда, 
		для уточнения размера передаваемых данных в команду следует включить один 
		из атрибутных операторов byte ptr или word ptr. 
		Команда не воздействует на флаги процессора. 
		Часто возникает необходимость выполнить обратную операцию - 
		к паре байтов обратиться, как к слову. Для этого надо использовать оператор word ptr:
		*/
		mov  eax, dword ptr analyzed_number[0]

		/*
		Команда test выполняет операцию логического умножения 
		И над двумя операндами и, в зависимости от результата, 
		устанавливает флаги SF, ZF и PF. Флаги OF и CF сбрасываются, 
		a AF имеет неопределенное значение. Состояние флагов можно затем 
		проанализировать командами условных переходов. Команда test не изменяет 
		ни один из операндов. В качестве первого операнда команды test можно 
		указывать регистр (кроме сегментного) или ячейку памяти, в качестве 
		второго - регистр (кроме сегментного), ячейку памяти или непосредственное 
		значение, однако не допускается определять оба операнда одновременно как 
		ячейки памяти. Операнды могут быть байтами или словами и представлять числа 
		со знаком или без знака. 

		Правила побитового умножения:

		Первый операнд-бит 0101
		Второй операнд-бит 0011
		Бит результата 0001

		Флаг SF устанавливается в 1, если в результате выполнения команды 
		образовалось число с установленным знаковым битом.
		Флаг ZF устанавливается в 1, если в результате выполнения команды 
		образовалось число, состоящее из одних двоичных нулей.
		Флаг PF устанавливается в 1, если в результате выполнения команды 
		образовалось число с четным количеством двоичных единиц в его битах.
		*/
		test eax, eax

		/*
		Команда Перейти, если Условие перехода
		jz нуль ZF= 1
		*/
		jz   grown_up

		/*
		Команда bsf сканирует слово или двойное слово в поисках бита, равного 1. 
		Сканирование выполняется от младшего бита (0) к старшему. Если в слове 
		не найдено установленных битов, то устанавливается флаг ZF. 
		Если установленные биты есть, то номер первого установленного бита 
		заносится в указанный в команде регистр. Номером бита считается его 
		позиция в слове, отсчитываемая от бита 0. В качестве первого операнда команды bsf 
		следует указывать регистр, куда будет помещен результат сканирования, в 
		качестве второго - регистр или ячейку памяти со сканируемым словом. 
		В команде bsf допустимо использование как 16-битовых, так и 32-битовых операндов 
		(но и первый, и второй операнды должны быть одного типа).
		*/
		bsf  eax, eax

		/*
		Команда jmp передает управление в указанную точку того же или другого программного 
		сегмента. Адрес возврата не сохраняется. Команда не воздействует на флаги процессора.
		*/
		jmp jump_end

		grown_up:    bsf  eax, dword ptr analyzed_number[4]

		/*
		сложение
		*/
		add  eax, 20h
		jump_end:
	}
}

//	===================================================================
// ищем последний установленный бит(т.е. бежим слева)
__inline __int32 Generator_moves::bit_scan_reverse_win32
(
	const unsigned __int64 analyzed_number// число в котором ищем взведенный бит
)
{ 

	_asm
	{ 
		mov  eax, dword ptr analyzed_number[4]
		test eax, eax
		jz   younger

		/*
		Команда bsr сканирует слово или двойное слово в поисках бита, равного 1. 
		Сканирование выполняется от старшего бита (15 или 31) к младшему. 
		Если в слове не найдено установленных битов, то устанавливается флаг ZF. 
		Если установленные биты есть, то номер первого установленного бита заносится 
		в указанный в команде регистр. Номером бита считается его позиция в слове, 
		отсчитываемая от бита 0. В качестве первого операнда команды bsr следует 
		указывать регистр, куда будет помещен результат сканирования, в качестве 
		второго - регистр или ячейку памяти со сканируемым словом. 
		В команде bsr допустимо использование как 16-битовых, так и 32-битовых операндов, 
		но и первый, и второй операнды должны быть одного типа (за исключением случая, 
		когда второй операнд - константа).
		*/
		bsr  eax, eax

		add  eax, 20h
		jmp  jump_end
		younger:    bsr  eax, dword ptr analyzed_number[0]
		jump_end:
	}
}

//	===================================================================
// 


/*
логика
             и &  0 0 1 первый операнд
                  0 1 1 второй операнд
                  -----
                  0 0 1 результат

           или |  0 0 1
                  0 1 1
                  -----
                  0 1 1

   исключающее ^  0 0 1
   или            0 1 1
                  -----
                  0 1 0

*/
