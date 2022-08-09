
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 21.02.2009  21.11.2010

#include <fstream>
#include <iostream>

//!
#include "l_Chess_bitboard.h"

#include "m_List_surplus_moves.h"
#include "m_List_surplus_moves_const.h"


/*
в этом модуле ссылку на const class Chess_bitboard & bitboard
оставил из за того что использую поля напрямую и 
const здесь необходим для гарантии неизменности полей.
*/

//	===================================================================
// генерируем список взятий + превращения пешки (для белых)
void List_surplus_moves::generator_captures_white
(
	const class Chess_bitboard & bitboard// представление доски
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_conversion_mask; // маска ходов пешки с превращеинем
	unsigned __int64 captures_conversion_mask; // маска взятий пешки с превращениями
	unsigned __int64 captures_mask; // маска взятий
	unsigned __int64 blocking_square_mask; // маска блокированных полей

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит
	__int32 blocking_coordinate; // координата блокированного поля 

	// генерируем ходы белых фигур
	p = 0;

	// ходы пешки----------------------------------------------------------------------------------
	from_mask = bitboard.white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//выделяем одну пешку 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		/* 
		отображение доски напрямую задаем формулой:
		line_index = 8 * rank_index + file_index
		to = 8 * bitboard.taking_passage_y + bitboard.taking_passage_x;

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
			move_conversion_mask = (one_pawn_mask << 8) & (~(bitboard.all_piece));

			// взятия с превращением
			captures_conversion_mask = pawn_capture_white[from] &  bitboard.all_black_piece;

			// находим координаты ходов с превращением пешки
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// отмечаем превращение пешки
				// превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,превращение в коня 12
				list_record_special(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0);
				list_record_special(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0);
				list_record_special(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0);
				list_record_special(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0);
 
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
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,bitboard.black_name_from_coord[to]);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,bitboard.black_name_from_coord[to]);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,bitboard.black_name_from_coord[to]);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,bitboard.black_name_from_coord[to]);

#if KONTROL_B
				KONTROL_captures_W(bitboard,from,to,p,120);
#endif//KONTROL_B
			}

		}
		else
		{
			// считаем взятия пешкой
			captures_mask = pawn_capture_white[from] &  bitboard.all_black_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_white(N_PAWN,CAPTURE_MOVE,from,to,p,bitboard);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 находим взятия на проходе
		if (bitboard.taking_passage_yes == 1)
		{
			// черная пешка может сходить на два поля только с начальной позиции
			// значит только если белая пешка стоит на 4 считая с нуля горизонтали
			// черная пешка может схотить через битое поле т.е. 
			// нужно проверять взятие на проходе
			if ( from / 8 == 4 )
			{
				to = 8 * bitboard.taking_passage_y + bitboard.taking_passage_x;

				// смотрим взятие на право. пешка не должна быть на самой правой вертикали
				if (((from - to) == -9) && (from % 8 != 7))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,bitboard.black_name_from_coord[to - 8]);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(bitboard,from,to,p,130);
#endif//KONTROL_B
				}

				// смотрим взятие на налево. пешка не должна быть на самой левой вертикали
				if (((from - to) == -7) && (from % 8 != 0))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,bitboard.black_name_from_coord[to - 8]);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(bitboard,from,to,p,130);
#endif//KONTROL_B
				}
			}
		}
	}
	// пешки----------------------------------------------------------------------------------


	// ходы коня ----------------------------------------------------------------------------------
	// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
	// расположения коней обнуляя тот бит который мы уже посмотрели
	from_mask = bitboard.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (bitboard.all_black_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(N_KNIGHT,CAPTURE_MOVE,from,to,p,bitboard);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// коня ----------------------------------------------------------------------------------


	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = bitboard.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях

		// луч вверх - вправо--------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------
	}
	// слона---------------------------------------------------------------------------------- 


	// ходы ладьи---------------------------------------------------------------------------------
	from_mask = bitboard.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях

		// луч вверх------------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_left_masks[from];
		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вправо-----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------
	}
	// ладьи----------------------------------------------------------------------------------


	// ходы ферзя---------------------------------------------------------------------------------
	// маска расположения белого ферзя
	from_mask = bitboard.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
		from_mask = from_mask & (from_mask - 1);

		//смотрим в четырех различных направлениях (по типу ладьи)

		//луч вверх------------------------------------------------------------------
		// 1) находим блокированные биты.
		blocking_square_mask = (bitboard.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) находим координату первого блокированного бита
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);

			// 3) находим маску взятий
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			// 4) заполняем список ходов
			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = bitboard.all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------  
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вправо----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// смотрим в четырех различных направлениях (по типу слона)

		// луч вверх - вправо--------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

	}
	// ферзя--------------------------------------------------------------------------------------


	// ходы короля--------------------------------------------------------------------------------
	// маска расположения белого короля
	from_mask = bitboard.white_piece[N_KING];

	if (from_mask != 0)
	{
		// находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
		from = bit_scan_forward_win32(from_mask);

		// находим маску взятий короля
		captures_mask = (bitboard.all_black_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			// Заполняем список ходов
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(N_KING,CAPTURE_MOVE,from,to,p,bitboard);
			captures_mask = captures_mask & (captures_mask - 1);

		}

	}
	// ходы короля--------------------------------------------------------------------------------

	end_list = p;
	end_captures = p;

}

//	===================================================================
// генерируем список взятий  + превращения пешки (для черных)
void List_surplus_moves::generator_captures_black
(
	const class Chess_bitboard & bitboard// представление доски
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_conversion_mask; // маска ходов пешки с превращеинем
	unsigned __int64 captures_conversion_mask; // маска взятий пешки с превращениями
	unsigned __int64 captures_mask; // маска взятий
	unsigned __int64 blocking_square_mask; // маска блокированных полей

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит
	__int32 blocking_coordinate; // координата блокированного поля 

	// генерируем ходы черных фигур
	p = 0;

	// ходы пешки----------------------------------------------------------------------------------
	from_mask = bitboard.black_piece[N_PAWN];

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
			move_conversion_mask = (one_pawn_mask >> 8) & (~(bitboard.all_piece));

			// взятия с превращением
			captures_conversion_mask = pawn_capture_black[from] &  bitboard.all_white_piece;

			// находим координаты ходов с превращением пешки
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// отмечаем превращение пешки
				// превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,
				// превращение в коня 12
				list_record_special(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0);
				list_record_special(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0);
				list_record_special(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0);
				list_record_special(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0);
 
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
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,bitboard.white_name_from_coord[to]);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,bitboard.white_name_from_coord[to]);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,bitboard.white_name_from_coord[to]);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,bitboard.white_name_from_coord[to]);

#if KONTROL_B
				KONTROL_captures_B(bitboard,from,to,p,120);
#endif//KONTROL_B
			}
		}
		else
		{
			// считаем взятия пешкой
			captures_mask =  pawn_capture_black[from] &  bitboard.all_white_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_black(N_PAWN,CAPTURE_MOVE,from,to,p,bitboard);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 находим взятия на проходе
		if (bitboard.taking_passage_yes == 1)
		{
			if ( from / 8 == 3 )
			{
				to = 8 * bitboard.taking_passage_y + bitboard.taking_passage_x;

				if (((from - to) == 9) && (from % 8 != 0))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,bitboard.white_name_from_coord[to + 8]);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(bitboard,from,to,p,130);
#endif//KONTROL_B
				}

				if (((from - to) == 7) && (from % 8 != 7))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,bitboard.white_name_from_coord[to + 8]);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(bitboard,from,to,p,130);
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
	from_mask = bitboard.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (bitboard.all_white_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
		   to = bit_scan_forward_win32(captures_mask);
		   list_capture_black(N_KNIGHT,CAPTURE_MOVE,from,to,p,bitboard);
		   captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// коня ----------------------------------------------------------------------------------


	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = bitboard.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях
		// луч вверх - вправо--------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

	}
	// слона---------------------------------------------------------------------------------- 


	// ходы ладьи-----------------------------------------------------------------------------
	from_mask = bitboard.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях

		// луч вверх------------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вправо----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

	}
	// ладьи----------------------------------------------------------------------------------


	// ходы ферзя----------------------------------------------------------------------------------
	from_mask = bitboard.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях

		// луч вверх-----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вправо----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// смотрим в четырех различных направлениях
		// луч вверх - вправо---------------------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,bitboard);
			}
		}
		//---------------------------------------------------------------------------

	}
	// ферзя----------------------------------------------------------------------------------


	// ходы короля---------------------------------------------------------------------------------
	from_mask = bitboard.black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		captures_mask = (bitboard.all_white_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_black(N_KING,CAPTURE_MOVE,from,to,p,bitboard);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// короля---------------------------------------------------------------------------------


	end_list = p;
	end_captures = p;

}

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)
//(для белых)
void List_surplus_moves::generator_move_white
(
	const class Chess_bitboard & bitboard// представление доски
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 move_mask; // маска ходов здесь только для ходов пешки с превращением
	unsigned __int64 blocking_square_mask; // маска блокированных полей
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_mask_d; // маска хода пешки на два поля

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит
	__int32 blocking_coordinate; // координата блокированного поля 

	p = end_list;

	// генерируем ходы белых фигур
	// ходы короля--------------------------------------------------------------------------------
	// маска расположения белого короля
	from_mask = bitboard.white_piece[N_KING];

	if (from_mask != 0)
	{
		// находим координату фигуры(самый первый(для нашего отображения правый ) 
		// установленный бит)
		from = bit_scan_forward_win32(from_mask);

		// находим маску ходов короля(без взятий)
		move_mask = king_moves_masks[from] & (~bitboard.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KING,SIMPLE_MOVE,from,to,p,bitboard);
		}

		// рокировки белого короля

		// если длинная рокировка разрешена
		if (bitboard.castling_Q)
		{
			// 
			to = C1;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// поля  1 2 3 пустые  то добавляем ход
			if (((~bitboard.all_piece) & CASTLING_Q ) == CASTLING_Q)
			{
				// длинная рокировка белых  0-0-0 проверяем под боем ли поля 2 3 4 
				if ((white_check_square(C1,bitboard) == 0) 
				&& (white_check_square(D1,bitboard) == 0) 
				&& (white_check_square(E1,bitboard) == 0))
				{
					list_record_special(N_KING,LONG_CASTLING,p,from,to,0);
				}
			}
		}

		// если короткая рокировка разрешена то добавляем ход 
		if (bitboard.castling_K)
		{
			to = G1;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			//поля  5 6 пустые  то добавляем ход
			if (((~bitboard.all_piece) & CASTLING_K ) == CASTLING_K)
			{
				// короткая рокировка белых 0-0 проверяем под боем ли поля  4 5 6
				if ((white_check_square(E1,bitboard) == 0)
				&& (white_check_square(F1,bitboard) == 0)
				&& (white_check_square(G1,bitboard) == 0))
				{
					list_record_special(N_KING,SHORT_CASTLING,p,from,to,0);
				}
			}
		}
	}
	// ходы короля--------------------------------------------------------------------------------


	// ходы ферзя----------------------------------------------------------------------------------
	// маска расположения белого ферзя
	from_mask = bitboard.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// надо пояснить что конструкция типа b = b & (b-1)  
		// сбрасывает самый правый единичный бит 
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях (по типу ладьи)

		// луч вверх-----------------------------------------------------------------
		// 1) находим блокированные биты.
		blocking_square_mask = bitboard.all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) находим координату первого блокированного бита
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = bitboard.all_piece & rook_left_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------  
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вправо----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------


		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// смотрим в четырех различных направлениях (по типу слона)

		// луч вверх - вправо--------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

	}
	// ферзя----------------------------------------------------------------------------------


	// ходы ладьи----------------------------------------------------------------------------------
	from_mask = bitboard.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях (по типу ладьи)

		// луч вверх-----------------------------------------------------------------
		// 1) находим блокированные биты.
		blocking_square_mask = bitboard.all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) находим координату первого блокированного бита
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = bitboard.all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------  
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вправо----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------
	}
	// ладьи----------------------------------------------------------------------------------



	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = bitboard.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях (по типу слона)

		// луч вверх - вправо--------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------
	}
	// слона---------------------------------------------------------------------------------- 


	// ходы коня ----------------------------------------------------------------------------------
	// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
	// расположения коней обнуляя тот бит который мы уже посмотрели
	from_mask = bitboard.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		move_mask = knigt_moves_masks[from] & (~bitboard.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,bitboard);
		}
	}
	// коня ----------------------------------------------------------------------------------

	// коды пешки----------------------------------------------------------------------------------
	from_mask = bitboard.white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//выделяем одну пешку 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 находим простые ходы белых пешек на одно поле вперед
		move_mask = (one_pawn_mask << 8) & (~(bitboard.all_piece));

		// 2 находим ходы пешек на 2 поля вперед 
		// в начале оставляем только пешки стоящие на 2 горизонтали
		move_mask_d = one_pawn_mask & LINE_1;

		// отметаем пешки если они чем то заблокированны
		move_mask_d = (move_mask_d << 8) & (~(bitboard.all_piece));

		// оставшимися пешками делаем ход
		move_mask_d = (move_mask_d << 8) & (~(bitboard.all_piece));

		// здесь все ходы пешки как на одно так на два поля вперед
		move_mask = (move_mask | move_mask_d) ;

		// ходы без превращений
		move_mask = move_mask & ~LINE_7;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,bitboard);
		}
	}
	// пешки----------------------------------------------------------------------------------

	end_list = p;

}

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)(для черных)
void List_surplus_moves::generator_move_black
(
	const class Chess_bitboard & bitboard// представление доски
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 move_mask; // маска ходов здесь только для ходов пешки с превращением
	unsigned __int64 blocking_square_mask; // маска блокированных полей
	unsigned __int64 one_pawn_mask; // маска расположения одной пешки
	unsigned __int64 move_mask_d; // маска хода пешки на два поля

	unsigned __int8 p; // счетчик ходов в списке
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int8 to; // координата поля куда фигура ходит
	__int32 blocking_coordinate; // координата блокированного поля 

	p = end_list;

	// генерируем ходы черных фигур
	// ходы короля---------------------------------------------------------------------------------
	from_mask = bitboard.black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		move_mask = king_moves_masks[from] & (~(bitboard.all_piece));

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KING,SIMPLE_MOVE,from,to,p,bitboard);
		}

		// рокировки черного короля

		// если длинная рокировка разрешена
		if (bitboard.castling_q)
		{
			to = C8;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// и поля  1 2 3 пустые  то добавляем ход
			if ((~(bitboard.all_piece) & CASTLING_q )== CASTLING_q)
			{
				// длинная рокировка черных проверяем 0-0-0 58 59 60
				if ((black_check_square(C8,bitboard) == 0)
				&& (black_check_square(D8,bitboard) == 0)
				&& (black_check_square(E8,bitboard) == 0))
				{
					list_record_special(N_KING,LONG_CASTLING,p,from,to,0);			 
				}
			}
		}

		// если короткая рокировка разрешена то добавляем ход 
		if (bitboard.castling_k)
		{
			to = G8;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// и поля  5 6 пустые  то добавляем ход
			if ((~(bitboard.all_piece) & CASTLING_k )== CASTLING_k)
			{
				// короткая рокировка черных проверяем 0-0 60 61 62
				if ((black_check_square(E8,bitboard) == 0)
				&& (black_check_square(F8,bitboard) == 0)
				&&(black_check_square(G8,bitboard) == 0))
				{
					list_record_special(N_KING,SHORT_CASTLING,p,from,to,0);
				}
			}
		}
	}
	// короля---------------------------------------------------------------------------------


	// ходы ферзя----------------------------------------------------------------------------------
	// маска расположения ферзя
	from_mask = bitboard.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает 
		//самый правый единичный бит 
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях (по типу ладьи)

		// луч вверх-----------------------------------------------------------------
		// 1) находим блокированные биты.
		blocking_square_mask = bitboard.all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) находим координату первого блокированного бита
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = bitboard.all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------  
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вправо----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------


		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// смотрим в четырех различных направлениях (по типу слона)
		// луч вверх - вправо--------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

	}
	// ферзя----------------------------------------------------------------------------------


	// ходы ладьи----------------------------------------------------------------------------------
	from_mask = bitboard.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях (по типу ладьи)

		// луч вверх-----------------------------------------------------------------
		// 1) находим блокированные биты.
		blocking_square_mask = bitboard.all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			//   2) находим координату первого блокированного бита
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч влево-----------------------------------------------------------------
		blocking_square_mask = bitboard.all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз------------------------------------------------------------------  
		blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вправо----------------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------
	}
	// ладьи----------------------------------------------------------------------------------


	// ходы слона---------------------------------------------------------------------------------- 
	from_mask = bitboard.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// смотрим в четырех различных направлениях (по типу слона)

		// луч вверх - вправо--------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вверх - влево---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - влево----------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------

		// луч вниз - вправо---------------------------------------------------------
		blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,bitboard);
		}
		//---------------------------------------------------------------------------
	}
	// слона---------------------------------------------------------------------------------- 


	// ходы коня ----------------------------------------------------------------------------------
	// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
	// расположения коней обнуляя тот бит который мы уже посмотрели
	from_mask = bitboard.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		move_mask = knigt_moves_masks[from] & (~bitboard.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,bitboard);
		}
	}
	// коня ----------------------------------------------------------------------------------

	// коды пешки----------------------------------------------------------------------------------
	from_mask = bitboard.black_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 находим простые ходы черных пешек на одно поле вперед
		move_mask = (one_pawn_mask >> 8) & (~(bitboard.all_piece));

		// 2 находим ходы пешек на 2 поля вперед 
		// в начале оставляем только пешки стоящие на 7 горизонтали
		move_mask_d = one_pawn_mask & LINE_6;

		// отметаем пешки если они чем то заблокированны
		move_mask_d = (move_mask_d >> 8) & (~(bitboard.all_piece));

		// оставшимися пешками делаем ход
		move_mask_d = (move_mask_d >> 8) & (~(bitboard.all_piece));

		// здесь все ходы пешки как на одно так на два поля вперед
		move_mask = (move_mask | move_mask_d) ;

		// ходы без превращений
		move_mask = move_mask & ~LINE_0;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,bitboard);
		}
	}
	// пешки----------------------------------------------------------------------------------

	end_list = p;

}

//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 List_surplus_moves::king_white_check
(
	const class Chess_bitboard & bitboard// представление доски
)
{

	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 blocking_square_mask; // маска блокированных полей
	unsigned __int64 captures_mask; // маска взятий


	unsigned __int8 from; // координата поля откуда фигура ходит
	__int32 blocking_coordinate; // координата блокированного поля 
	unsigned __int8 i; // координата 

	// маска расположения белого короля
	from_mask = bitboard.white_piece[N_KING];

	// находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
	from = bit_scan_forward_win32(from_mask);

	// луч вверх------------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч влево-----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз------------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вправо-----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// смотрим в четырех различных направлениях

	// луч вверх - вправо--------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вверх - влево---------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз - влево----------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз - вправо---------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// по типу коня--------------------------------------------------------------
	captures_mask = (bitboard.all_black_piece & knigt_moves_masks[from]);

	while (captures_mask != 0 )
	{
		if (bitboard.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------


	// по типу короля--------------------------------------------------------------------
	captures_mask = (bitboard.all_black_piece & king_moves_masks[from]);
	while (captures_mask != 0)
	{
		if (bitboard.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

// Пешки считаем напрямую----------------------------------------------------
	if ((from / 7) < 8)
	{
		if (from % 8 != 0)
		{
			if (bitboard.black_name_from_coord[from + 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}

		if (from % 8 != 7)
		{
			if (bitboard.black_name_from_coord[from + 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}

	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 List_surplus_moves::king_black_check
(
	const class Chess_bitboard & bitboard// представление доски
)
{

	unsigned __int64 from_mask; // маска расположения фигур одного типа
	unsigned __int64 blocking_square_mask; // маска блокированных полей
	unsigned __int64 captures_mask; // маска взятий


	unsigned __int8 from; // координата поля откуда фигура ходит
	__int32 blocking_coordinate; // координата блокированного поля 
	unsigned __int8 i; // координата 

	// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	from_mask = bitboard.black_piece[N_KING];
	from = bit_scan_forward_win32(from_mask);
 
	// смотрим в четырех различных направлениях

	// луч вверх------------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч влево-----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз------------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вправо----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// смотрим в четырех различных направлениях
	// луч вверх - вправо--------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вверх - влево---------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз - влево----------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз - вправо---------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// По типу коня--------------------------------------------------------------
	captures_mask = (bitboard.all_white_piece & knigt_moves_masks[from]);

	while (captures_mask != 0)
	{
		if (bitboard.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		==  N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// по типу короля------------------------------------------------------------
	captures_mask = (bitboard.all_white_piece & king_moves_masks[from]);

	while (captures_mask != 0)
	{
		if (bitboard.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	//--------------------------------------------------------------------------- 
	if ((from / 7) > 1 )
	{
		// Пешки считаем напрямую
		if (from % 8 != 7)
		{
			if (bitboard.white_name_from_coord[from - 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}
 
		if (from % 8 != 0)
		{
			if (bitboard.white_name_from_coord[from - 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}
 
	}
	//---------------------------------------------------------------------------

	return 0;
 
}


//private

//	===================================================================
// под боем ли белые поля если шаха нет то функция возвращает 0
// иначе цифру шахующей фигуры
inline unsigned __int8 List_surplus_moves::white_check_square
(
	const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
	const class Chess_bitboard & bitboard// представление доски
)
{

	unsigned __int64 blocking_square_mask; // маска блокированных полей
	unsigned __int64 captures_mask; // маска взятий

	__int32 blocking_coordinate; // координата блокированного поля 
	unsigned __int8 i; // координата 

	// луч вверх------------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_up_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч влево-----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вправо-----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// луч вверх - вправо--------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_up_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вверх - влево---------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_up_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// по типу коня
	//---------------------------------------------------------------------------
	captures_mask = (bitboard.all_black_piece & knigt_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (bitboard.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		==  N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// по типу короля
	//---------------------------------------------------------------------------
	captures_mask = (bitboard.all_black_piece & king_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (bitboard.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------


	// пешки считаем напрямую
	//---------------------------------------------------------------------------
	if (bitboard.black_name_from_coord[sq + 7] == N_PAWN)
	{
		return N_PAWN;
	}

	if (bitboard.black_name_from_coord[sq + 9] == N_PAWN)
	{
		return N_PAWN;
	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// под боем ли черные поля если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
inline unsigned __int8 List_surplus_moves::black_check_square
(
	const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
	const class Chess_bitboard & bitboard// представление доски
)
{
	unsigned __int64 blocking_square_mask; // маска блокированных полей
	unsigned __int64 captures_mask; // маска взятий

	__int32 blocking_coordinate; // координата блокированного поля 
	unsigned __int8 i; // координата 

	// луч влево-----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз------------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_down_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вправо----------------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & rook_right_masks[sq];

	if(blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// луч вниз - влево----------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_down_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// луч вниз - вправо---------------------------------------------------------
	blocking_square_mask = (bitboard.all_piece) & bishop_down_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = bitboard.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (bitboard.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (bitboard.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// по типу коня
	//---------------------------------------------------------------------------
	captures_mask = (bitboard.all_white_piece & knigt_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (bitboard.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// по типу короля
	//---------------------------------------------------------------------------
	captures_mask = (bitboard.all_white_piece & king_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (bitboard.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// пешки считаем напрямую
	//---------------------------------------------------------------------------
	if (bitboard.white_name_from_coord[sq - 7] == N_PAWN)
	{
		return N_PAWN;
	}

	if (bitboard.white_name_from_coord[sq - 9] == N_PAWN)
	{
		return N_PAWN;
	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// заполняем список ходов - взятия за белых
// две функции только из за разного контроля списков(строки взятия)
inline void List_surplus_moves::list_capture_white
(
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 type_move,// тип хода
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 & to,// куда ходим
	unsigned __int8 & count,// номер хода в списке
	const class Chess_bitboard & bitboard// представление доски
)
{
	unsigned __int8 capture;// взятая фигура

#if KONTROL_B
	KONTROL_captures_W(bitboard,from,to,count,name);
#endif//KONTROL_B

	// взятая фигура
	capture = bitboard.black_name_from_coord[to];

	// записываем оценку взятия(mvv/lva в моем понимании) для последующей сортировки
	sorting_score[count] = 10 * capture - name; 

	// 6 бит 
	// записали имя взятой фигуры (1 - пешка 2 - конь 3 - слон 4 - ладья 5 - ферзь 
	// 6 - король)
	move[count] = capture

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
	//std::cout << "bitboard.black_name_from_coord[to]= "<< bitboard.black_name_from_coord[to] << "\n";
	//std::cout << "bitboard.black_name_from_coord[to]= "<< ( move[p] & 63) << "\n";
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
inline void List_surplus_moves::list_capture_black
(
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 type_move,// тип хода
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 & to,// куда ходим
	unsigned __int8 & count,// номер хода в списке
	const class Chess_bitboard & bitboard// представление доски
)
{
	unsigned __int8 capture;// взятая фигура

#if KONTROL_B
	KONTROL_captures_B(bitboard,from,to,count,name);
#endif//KONTROL_B

	// взятая фигура
	capture = bitboard.white_name_from_coord[to];

	// записываем оценку взятия(mvv/lva в моем понимании) для последующей сортировки
	sorting_score[count] = 10 * capture - name; 

	// 6 бит 
	// записали имя взятой фигуры (1 - пешка 2 - конь 3 - слон 4 - ладья 5 - ферзь 6 - король)
	move[count] = capture

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
	//std::cout << "bitboard.white_name_from_coord[to]= "<< bitboard.white_name_from_coord[to] << "\n";
	//std::cout << "bitboard.white_name_from_coord[to]= "<< ( move[p] & 63) << "\n";
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
inline void List_surplus_moves::list_record_move
(
	const unsigned __int8 name,// имя ходящей фигуры
	const unsigned __int8 type_move,// тип хода
	const unsigned __int8 & from,// откуда ходим
	const unsigned __int8 & to,// куда ходим
	unsigned __int8 & count,// номер хода в списке
	const class Chess_bitboard & bitboard// представление доски
)
{

#if KONTROL_B
	KONTROL_move(from,to,count);
#endif//KONTROL_B

	// 6 бит 
	// взятая фигура 0
	move[count] = 0

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
inline void List_surplus_moves::list_record_special
(
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
	sorting_score[count] = d_m;

	// 6 бит 
	// взятая фигура 0
	move[count] = capture

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
inline void List_surplus_moves::KONTROL_captures_W
(
	const class Chess_bitboard & bitboard,// представление доски
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

	if ((bitboard.white_name_from_coord[from] < 1)
	||(bitboard.white_name_from_coord[from] > 6))
	{
		std::cout << "W ERROR Generator_captures bitboard.white_name_from_coord[*from] = "  
			<< bitboard.white_name_from_coord[from]<< "\n";
	}

	if ((bitboard.black_name_from_coord[to] < 1)
	||(bitboard.black_name_from_coord[to] > 6))
	{
		std::cout << "W ERROR Generator_captures bitboard.black_name_from_coord[*to]= " 
			<< bitboard.black_name_from_coord[to]   << "\n";
		std::cout << "W ERROR Generator_captures from= "<< from << "\n";
		std::cout << "W ERROR Generator_captures to= "  << to   << "\n";
		std::cout << "W ERROR Generator_captures name_figure= "  << name_figure << "\n";
		Bitboard_print(bitboard);
	}

	//if (bitboard.black_name_from_coord[to] ==6)
	//{
		//std::cout << "ERROR Generator_captures name_figure KING "  << "\n";
	//}
	///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для черных фигур генератора взятий
inline void List_surplus_moves::KONTROL_captures_B
(
	const class Chess_bitboard & bitboard,// представление доски
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

	if ((bitboard.black_name_from_coord[from] < 1)
	||(bitboard.black_name_from_coord[from] > 6))
	{
		std::cout << "B ERROR Generator_captures bitboard.black_name_from_coord[from] = "
			<< bitboard.black_name_from_coord[from]   << "\n";
	}

	if ((bitboard.white_name_from_coord[to] < 1)
	||(bitboard.white_name_from_coord[to] > 6))
	{
		std::cout << "B ERROR Generator_captures bitboard.white_name_from_coord[to]= "
			<< bitboard.white_name_from_coord[to]   << "\n";
		std::cout << "B ERROR Generator_captures from= "<< from << "\n";
		std::cout << "B ERROR Generator_captures to= "  << to   << "\n";
		std::cout << "B ERROR Generator_captures name_figure= "  << name_figure << "\n";
		Bitboard_print(bitboard);
	}

	//if (bitboard.white_name_from_coord[to] ==6)
	//{
	//std::cout << "B ERROR Generator_captures name_figure KING "  << "\n";
	//}
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// контролируем выход за пределы доски, за пределы списка 
// для генератора ходов 
inline void List_surplus_moves::KONTROL_move
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
__inline __int32 List_surplus_moves::bit_scan_forward_win32
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
__inline __int32 List_surplus_moves::bit_scan_reverse_win32
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