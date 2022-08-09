
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.02.2009 26.06.2010  21.11.2010

#include <iostream>


#include "b_constants.h"
#include "evaluate.h"
#include "b_evaluate_const.h"
#include "b_move_generation_const.h"
#include "make_move.h"


//информация для отката хода
struct Undo d_undo[MAX_DEPTH];

//	===================================================================
struct Undo Make_move::get_d_undo(const unsigned __int8 depth)
{
	return  d_undo[depth];
};

//	===================================================================
// реализуем ходы вперед для белых
void Make_move::do_moves_white
(
	struct Position & position, //
	const unsigned __int8 & i,//номер хода по списку
	const unsigned __int8 & depth,//глубина поиска
	const struct List & list_surplus_moves//список возможных ходов
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12) & 63);// исходная позиция фигуры
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18) & 63);// финальная позиция фигуры
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);// взятая фигура

	// запоминаем флаг взятия на проходе
	undo.return_taking_passage = position.taking_passage_yes;

	// всегда запоминаем(нужно для хеш-таблицы)
	undo.taking_passage_x = position.taking_passage_x;
	undo.taking_passage_y = position.taking_passage_y;

	// запрещаем взятие на проходе 
	position.taking_passage_yes = 0;

	// белые сходят значит станет ход черных
	position.colour_move = 0;

	// всегда запоминаем флаги рокировок (нужно для хеш-таблицы)
	undo.return_castling_K = position.castling_K;
	undo.return_castling_Q = position.castling_Q;
	undo.return_castling_k = position.castling_k;
	undo.return_castling_q = position.castling_q;

	/*
	тип хода
	0 - нет хода
	1 – простой ход
	2 – взятие
	3 – длинная рокировка
	4 – короткая рокировка
	5 – взятие на проходе

	12 - превращение в конь 
	13 - превращение в слон
	14 - превращение в ладья 
	15 - превращение в ферзь 

	22 - взятие с превращением в конь 
	23 - взятие с превращением в слон
	24 - взятие с превращением в ладья 
	25 - взятие с превращением в ферзь 
	*/

	switch (((list_surplus_moves.move[i]>>6) & 63))
	{
		//1 ход на пустое поле
		case SIMPLE_MOVE:

			//стираем фигуру на старом месте---------------------------------------------- 
			//стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			//стираем фигуру из битового списка фигур
			position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			//переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте------------------------------------------------
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];
			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			//---------------------------------------------------------------------------------
			// особо отмечаем случаи влияющие на рокировку и взятие на проходе
			// мы тут смотрим какой фигурой ходили и если это пешка то проверяем не нужно
			// ли отметить взятие на проходе если это король то отменяем его рокировки
			// а если ладья то отменяем рокировку с ней 
			//---------------------------------------------------------------------------------

			// пешка - флаг взятия на проходе
			if ( position.white_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					undo.taking_passage_x = position.taking_passage_x;
					undo.taking_passage_y = position.taking_passage_y;
					position.taking_passage_yes = 1;
					position.taking_passage_x =(i_p + 8) % 8;
					position.taking_passage_y =(i_p + 8) / 8;
				}
			}// король - флаг рокировки
			else if ( position.white_name_from_coord[f_p] == N_KING)
			{
				// запоминаем флаги рокировок
				undo.return_castling_K = position.castling_K;
				undo.return_castling_Q = position.castling_Q;
				position.castling_K = 0;
				position.castling_Q = 0;	   	
			}// ладья - флаг рокировки  
			else if ( position.white_name_from_coord[f_p] == N_ROOK)
			{
				if (i_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (i_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}
			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//2 взятие
		case CAPTURE_MOVE:

			// убираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// убираем фигуру из битового списка фигур
			 position.white_piece[position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// взятие белыми
			// у черных забираем фигуру
#if MATERIAL
			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// обнуляем взятую фигуру
			position.black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];
			// рисуем берущую фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// король - флаг рокировки
			if ( position.white_name_from_coord[f_p] == N_KING)
			{
				// запоминаем флаги рокировок
				undo.return_castling_K = position.castling_K;
				undo.return_castling_Q = position.castling_Q;
				position.castling_K = 0;
				position.castling_Q = 0;		
  
			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if ( position.white_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q =0;
				}

				if (i_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K =0;
				}
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)//63
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;
		break;

		//5 взятие на проходе
		case TAKING_PASSAGE:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			// убираем взятую фигуру из маски всех черных фигур
			 position.all_black_piece =  position.all_black_piece ^ move_masks[(f_p - 8)];
			// кроме того убираем взятую фигуру из битового списка фигур
			position.black_piece[position.black_name_from_coord[(f_p - 8)]] = position.black_piece[position.black_name_from_coord[(f_p - 8)]] ^ move_masks[(f_p - 8)];

			// взятие белыми
			// у черных забираем пешку
#if MATERIAL
			position.value_black_material = position.value_black_material 
				- PAWN;	
#endif//#if MATERIAL


			// обнуляем взятую фигуру
			position.black_name_from_coord[(f_p - 8)] = 0;

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//3 рокировка 0-0-0
		case LONG_CASTLING:

			//стираем фигуру на старом месте
			// ход королем
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			// ход ладьей
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[A1];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[A1]] 
			= position.white_piece[ position.white_name_from_coord[A1]] ^ move_masks[A1];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[D1] =  position.white_name_from_coord[A1];
			 position.white_name_from_coord[A1] = 0;

			//рисуем фигуру на новом месте------------------
			// ход королем
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ход ладьей
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[D1];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[position.white_name_from_coord[D1]] 
			= position.white_piece[position.white_name_from_coord[D1]] ^ move_masks[D1];

			// король - флаг рокировки
			// запоминаем флаги рокировок
			undo.return_castling_Q = position.castling_Q;
			undo.return_castling_K = position.castling_K;
			position.castling_Q = 0;
			position.castling_K = 0;

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//4 рокировка 0-0
		case SHORT_CASTLING:

			//стираем фигуру на старом месте
			// ход королем
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			// ход ладьей
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[H1];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[H1]] 
			= position.white_piece[ position.white_name_from_coord[H1]] ^ move_masks[H1];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[F1] =  position.white_name_from_coord[H1];
			 position.white_name_from_coord[H1] = 0;

			//рисуем фигуру на новом месте
			// ход королем
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ход ладьей
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[F1];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[F1]] 
			= position.white_piece[ position.white_name_from_coord[F1]] ^ move_masks[F1];

			// король - флаг рокировки
			// запоминаем флаги рокировок
			undo.return_castling_Q = position.castling_Q;
			undo.return_castling_K = position.castling_K;
			position.castling_Q = 0;
			position.castling_K = 0;

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//15 превращение в ферзя
		case TRANSFORMATION_IN_QUEEN:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] = N_QUEEN;
			 position.white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ QUEEN - PAWN;
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//14 превращение в ладью
		case TRANSFORMATION_IN_ROOK:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] = N_ROOK;
			 position.white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ ROOK - PAWN;	
#endif//#if MATERIAL
			

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//13 превращение в слона
		case TRANSFORMATION_IN_BISHOP:

			//стираем фигуру на старом месте 
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] = N_BISHOP;
			 position.white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
				position.value_white_material = position.value_white_material 
					+ BISHOP - PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//12 превращение в коня
		case TRANSFORMATION_IN_KNIGHT:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			 position.white_name_from_coord[f_p] = N_KNIGHT;
			 position.white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ KNIGHT - PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;
		break;

		//25 взятие с превращением в ферзя
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			// убираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// убираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + QUEEN - PAWN;

			position.value_black_material = position.value_black_material	
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// обнуляем взятую фигуру
			position.black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и зануляем по старой координате
			 position.white_name_from_coord[f_p] = N_QUEEN;
			 position.white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем берущую фигуру в битовом списке фигур
			 position.white_piece[position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//24 взятие с превращением в ладью
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			// убираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// убираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + ROOK - PAWN;

			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// зануляем взятую фигуру
			position.black_name_from_coord[f_p] = 0;
			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[f_p] = N_ROOK;
			 position.white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];
			// рисуем берущую фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//23 взятие с превращением слона
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			// убираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// убираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];


			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + BISHOP - PAWN;

			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// зануляем взятую фигуру
			position.black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[f_p] = N_BISHOP;
			 position.white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем берущую фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//22 взятие с превращением  в коня
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// убираем фигуру из маски всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// убираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];


			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + KNIGHT - PAWN;

			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// обнуляем взятую фигуру
			position.black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и зануляем по старой координате
			 position.white_name_from_coord[f_p] = N_KNIGHT;
			 position.white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// рисуем берущую фигуру в битовом списке фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];  

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		default:;//ход не распознан
   
	}

}

//	===================================================================
// реализуем ходы вперед для черных
void Make_move::do_moves_black
(
	struct Position & position, //
	const unsigned __int8 & i,//номер хода по списку
	const unsigned __int8 & depth,//глубина поиска
	const struct List & list_surplus_moves//список возможных ходов
)
{
	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// запоминаем флаг взятия на проходе
	undo.return_taking_passage= position.taking_passage_yes;

	// всегда запоминаем(нужно для хеш-таблицы)
	undo.taking_passage_x = position.taking_passage_x;
	undo.taking_passage_y = position.taking_passage_y;


	// запрещаем взятие на проходе 
	position.taking_passage_yes = 0;

	// черные сходят значит станет ход белых
	position.colour_move = 1;

	// всегда запоминаем флаги рокировок (нужно для хеш-таблицы)
	undo.return_castling_K = position.castling_K;
	undo.return_castling_Q = position.castling_Q;
	undo.return_castling_k = position.castling_k;
	undo.return_castling_q = position.castling_q;


	/*
	тип хода
	0 - нет хода
	1 – простой ход
	2 – взятие
	3 – длинная рокировка
	4 – короткая рокировка
	5 – взятие на проходе

	12 - превращение в конь 
	13 - превращение в слон
	14 - превращение в ладья 
	15 - превращение в ферзь 

	22 - взятие с превращением в конь 
	23 - взятие с превращением в слон
	24 - взятие с превращением в ладья 
	25 - взятие с превращением в ферзь 
	*/

	switch (((list_surplus_moves.move[i]>>6)& 63))
	{

		//1 ход на пустое поле
		case SIMPLE_MOVE:

			//стираем фигуру на старом месте 
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// пешка - флаг взятия на проходе 
			if (position.black_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					//cout << "i_p= "<< i_p<<std::endl;
					//cout << "f_p= "<< f_p<<std::endl;
					//cout << "position.black_name_from_coord[f_p]= "<< position.black_name_from_coord[f_p]<<std::endl;
					undo.taking_passage_x = position.taking_passage_x;
					undo.taking_passage_y = position.taking_passage_y;
					position.taking_passage_yes =1;
					position.taking_passage_x =(i_p - 8) % 8;
					position.taking_passage_y =(i_p - 8) / 8;
				}
			
			}// король - флаг рокировки
			else if (position.black_name_from_coord[f_p] == N_KING)
			{
				undo.return_castling_k = position.castling_k;
				undo.return_castling_q = position.castling_q;
				position.castling_k = 0;
				position.castling_q = 0;

    
			}// ладья - флаг рокировки   
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//2 взятие
		case CAPTURE_MOVE:

			//стираем фигуру на старом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// взятие черными
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL


			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// король - флаг рокировки
			if (position.black_name_from_coord[f_p] == N_KING)
			{
				undo.return_castling_k = position.castling_k;
				undo.return_castling_q = position.castling_q;
				position.castling_k = 0;
				position.castling_q = 0;

			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
     
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		// 5 взятие на проходе
		case TAKING_PASSAGE:

			//стираем фигуру на старом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			position.all_white_piece = position.all_white_piece ^ move_masks[(f_p + 8)];
			 position.white_piece[ position.white_name_from_coord[(f_p + 8)]] = position.white_piece[ position.white_name_from_coord[(f_p + 8)]] ^ move_masks[(f_p + 8)];
  
			// взятие черными
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- PAWN;	
#endif//#if MATERIAL
			
			 position.white_name_from_coord[(f_p + 8)] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//3 рокировка 0-0-0
		case LONG_CASTLING:

			//стираем фигуру на старом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			 position.all_black_piece =  position.all_black_piece ^ move_masks[A8];
			position.black_piece[position.black_name_from_coord[A8]] 
			= position.black_piece[position.black_name_from_coord[A8]] ^ move_masks[A8];
			position.black_name_from_coord[D8] = position.black_name_from_coord[A8];
			position.black_name_from_coord[A8] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			 position.all_black_piece =  position.all_black_piece ^ move_masks[D8];
			position.black_piece[position.black_name_from_coord[D8]] 
			= position.black_piece[position.black_name_from_coord[D8]] ^ move_masks[D8];


			// король - флаг рокировки 
			undo.return_castling_q = position.castling_q;
			undo.return_castling_k = position.castling_k;
			position.castling_q = 0;
			position.castling_k = 0; 

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//4 рокировка 0-0
		case SHORT_CASTLING:

			//стираем фигуру на старом месте 
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			 position.all_black_piece =  position.all_black_piece ^ move_masks[H8];
			position.black_piece[position.black_name_from_coord[H8]] 
			= position.black_piece[position.black_name_from_coord[H8]] ^ move_masks[H8];
			position.black_name_from_coord[F8] = position.black_name_from_coord[H8];
			position.black_name_from_coord[H8] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			 position.all_black_piece =  position.all_black_piece ^ move_masks[F8];
			position.black_piece[position.black_name_from_coord[F8]] 
			= position.black_piece[position.black_name_from_coord[F8]] ^ move_masks[F8];


			// король - флаг рокировки
			undo.return_castling_q = position.castling_q;
			undo.return_castling_k = position.castling_k;
			position.castling_q = 0;
			position.castling_k = 0;

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//15 превращение в ферзя
		case TRANSFORMATION_IN_QUEEN:

			//стираем фигуру на старом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_QUEEN;
			position.black_name_from_coord[i_p] = 0;

			// превращение черными
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + QUEEN - PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//14 превращение в ладью
		case TRANSFORMATION_IN_ROOK:

			//стираем фигуру на старом месте 
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_ROOK;
			position.black_name_from_coord[i_p] = 0;

			// превращение черными
			#if MATERIAL
				position.value_black_material = 
					position.value_black_material + ROOK - PAWN;	
			#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//13 превращение в слона
		case TRANSFORMATION_IN_BISHOP:

			//стираем фигуру на старом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_BISHOP;
			position.black_name_from_coord[i_p] = 0;

			// превращение черными
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + BISHOP - PAWN;	
#endif//#if MATERIAL


			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//12 превращение в конь
		case TRANSFORMATION_IN_KNIGHT:

			//стираем фигуру на старом месте 
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_KNIGHT;
			position.black_name_from_coord[i_p] = 0;

			// превращение черными
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + KNIGHT - PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// корректируем маску всех фигур
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//25 взятие с превращением в ферзя
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// превращение с взятием черными
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + QUEEN - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL


			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = N_QUEEN;
			position.black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}	
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//24 взятие с превращением в ладью
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// превращение с взятием черными
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + ROOK - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL

			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = N_ROOK;
			position.black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// если сьедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//23 взятие с превращением слона
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// превращение с взятием черными
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + BISHOP - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL

			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = N_BISHOP;
			position.black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// если сьедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//22 взятие с превращением  в коня
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// превращение с взятием черными
#if MATERIAL
			position.value_black_material 
				= position.value_black_material + KNIGHT - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL

			 position.white_name_from_coord[f_p] = 0;
			position.black_name_from_coord[f_p] = N_KNIGHT;
			position.black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			 position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		default:;//ход не распознан
   
	}

}

//	===================================================================

// ход назад для белых
void Make_move::undo_moves_white
(
	struct Position & position, //
	const unsigned __int8 & i,//номер хода по списку
	const unsigned __int8 & depth,//глубина поиска
	const struct List & list_surplus_moves//список возможных ходов
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// восстанавливаем флаг взятия на проходе
	position.taking_passage_yes=undo.return_taking_passage;

	// белые забрали свой ход значит опять их ход стал
	position.colour_move = 1;

	/*
	тип хода
	0 - нет хода
	1 – простой ход
	2 – взятие
	3 – длинная рокировка
	4 – короткая рокировка
	5 – взятие на проходе

	12 - превращение в конь 
	13 - превращение в слон
	14 - превращение в ладья 
	15 - превращение в ферзь 

	22 - взятие с превращением в конь 
	23 - взятие с превращением в слон
	24 - взятие с превращением в ладья 
	25 - взятие с превращением в ферзь 
	*/

	switch (((list_surplus_moves.move[i]>>6)& 63))
	{
		//1 ход на пустое поле
		case SIMPLE_MOVE:
 
			// пешка - флаг взятия на проходе 
			if ( position.white_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					position.taking_passage_yes =undo.return_taking_passage;
					position.taking_passage_x = undo.taking_passage_x;
					position.taking_passage_y = undo.taking_passage_y;
				}

				
			}// король - флаг рокировки 
			else if ( position.white_name_from_coord[f_p] == N_KING)
			{
				// вспоминаем флаги рокировок
				position.castling_K = undo.return_castling_K;
				position.castling_Q = undo.return_castling_Q;   

	    
			}// ладья - флаг рокировки   
			else if (position.white_name_from_coord[f_p] == N_ROOK)
			{ 
				if (i_p == A1)//0
				{
					position.castling_Q = undo.return_castling_Q;
				}

				if (i_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}

			//стираем фигуру на старом месте
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//2 взятие
		case CAPTURE_MOVE:

			// король - флаг рокировки
			if (position.white_name_from_coord[f_p] == N_KING)
			{
				// вспоминаем флаги рокировок
				position.castling_K = undo.return_castling_K;
				position.castling_Q = undo.return_castling_Q;   

			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if ( position.white_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A1)//0
				{
					position.castling_Q = undo.return_castling_Q;
				}

				if (i_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					position.castling_q = undo.return_castling_q;
				}

				if (f_p == H8)//63
				{
					position.castling_k = undo.return_castling_k;
				}
			}

			//стираем фигуру на старом месте 
			// стираем фигуру из списка всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// востанавливаем имя взятой фигуры
			position.black_name_from_coord[f_p] = t_f;

			// взятие белыми
			// к оценке материала черных мы прибавляем значение взятой фигуры 
			// (обратный ход)
#if MATERIAL
			position.value_black_material = position.value_black_material 
				+ KOD[t_f];
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// в маске все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// в маске все черные фигуры
			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// битовый список взятой фигуры
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//5 взятие на проходе
		case TAKING_PASSAGE:

			//стираем фигуру на старом месте
			// стираем фигуру из списка всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// востанавливаем имя взятой фигуры
			position.black_name_from_coord[(f_p - 8)] = t_f;

			// взятие белыми
			// к оценке материала черных мы прибавляем значение взятой фигуры 
			// (обратный ход)
#if MATERIAL
			position.value_black_material = position.value_black_material 
				+ PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// в маске все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// в маске все черные фигуры
			position.all_black_piece = position.all_black_piece ^ move_masks[(f_p - 8)];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// битовый список взятой фигуры
			position.black_piece[position.black_name_from_coord[(f_p - 8)]] = position.black_piece[position.black_name_from_coord[(f_p - 8)]] ^ move_masks[(f_p - 8)];

			position.all_piece = position.all_white_piece | position.all_black_piece;
   
		break;

		//3 0-0-0
		case LONG_CASTLING:

			// король - флаг рокировки
			position.castling_Q = undo.return_castling_Q;
			position.castling_K = undo.return_castling_K;   

			//стираем фигуру на старом месте
			// король
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// ладья
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[D1];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[D1]] 
			= position.white_piece[ position.white_name_from_coord[D1]] ^ move_masks[D1];

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[A1] =  position.white_name_from_coord[D1];
			 position.white_name_from_coord[D1] = 0;

			//рисуем фигуру на новом месте
			// король
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ладья
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[A1];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[A1]] 
			= position.white_piece[ position.white_name_from_coord[A1]] ^ move_masks[A1];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//4 0-0
		case SHORT_CASTLING:

			// король - флаг рокировки
			position.castling_Q = undo.return_castling_Q;
			position.castling_K = undo.return_castling_K;   

			//стираем фигуру на старом месте
			// король
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// переносим имя фигуры и зануляем по старой координате
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// ладья
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[F1];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[F1]] 
			= position.white_piece[ position.white_name_from_coord[F1]] ^ move_masks[F1];

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[H1] =  position.white_name_from_coord[F1];
			 position.white_name_from_coord[F1] = 0;

			//рисуем фигуру на новом месте
			// король
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ладья
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[H1];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[H1]] 
			= position.white_piece[ position.white_name_from_coord[H1]] ^ move_masks[H1];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//вернем 15-ферзь 14-ладью 13-слона 12-коня
		case TRANSFORMATION_IN_QUEEN:
		case TRANSFORMATION_IN_ROOK:
		case TRANSFORMATION_IN_BISHOP:
		case TRANSFORMATION_IN_KNIGHT:

			//стираем фигуру на старом месте
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// превращение белыми
			// от оценки материала былых мы отнимаем значение фигуры 
			// превращения и прибавляем пешку
			// (обратный ход)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]] + PAWN;
#endif//#if MATERIAL

			// переносим имя фигуры и зануляем по старой координате
			 position.white_name_from_coord[i_p] = N_PAWN;
			 position.white_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			// все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		// вернем взятие с превращением 25-ферзя 24-ладьи 23-слона 22-коня
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			//  вспоминаем если съедена ладья 
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					position.castling_q = undo.return_castling_q;
				}

				if (f_p == H8)//63
				{
					position.castling_k = undo.return_castling_k;
				}
			}

			//стираем фигуру на старом месте
			// стираем фигуру из списка всех белых фигур
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// стираем фигуру из битового списка фигур
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];


			// превращение белыми
			// от оценки материала былых мы отнимаем значение фигуры 
			// превращения и прибавляем пешку
			// к оценке материала черных мы прибавляем значение взятой фигуры
			// (обратный ход)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]] + PAWN;

			position.value_black_material = position.value_black_material 
				+ KOD[t_f];
#endif//#if MATERIAL
			//------------------------------------------------------------------------------

			// переносим имя фигуры и обнуляем по старой координате
			 position.white_name_from_coord[i_p] = N_PAWN;
			 position.white_name_from_coord[f_p] = 0;

			// восстанавливаем имя взятой фигуры
			position.black_name_from_coord[f_p] = t_f;

			//рисуем фигуру на новом месте
			// в маске все белые фигуры
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// в маске все черные фигуры
			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];

			// битовый список фигуры
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// битовый список взятой фигуры
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		default:;//ход не распознан
	}
}


//	===================================================================
// ход назад для черных
void Make_move::undo_moves_black
(
	struct Position & position, //
	const unsigned __int8 & i,//номер хода по списку
	const unsigned __int8 & depth,//глубина поиска
	const struct List & list_surplus_moves//список возможных ходов
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// востанавливаем флаг взятия на проходе
	position.taking_passage_yes=undo.return_taking_passage;

	// черные забрали свой ход значит опять их ход стал
	position.colour_move = 0;

	/*
	тип хода
	0 - нет хода
	1 – простой ход
	2 – взятие
	3 – длинная рокировка
	4 – короткая рокировка
	5 – взятие на проходе

	12 - превращение в конь 
	13 - превращение в слон
	14 - превращение в ладья 
	15 - превращение в ферзь 

	22 - взятие с превращением в конь 
	23 - взятие с превращением в слон
	24 - взятие с превращением в ладья 
	25 - взятие с превращением в ферзь 
	*/

	switch (((list_surplus_moves.move[i]>>6)& 63))
	{
		//1 ход на пустое поле
		case SIMPLE_MOVE:

			// пешка - флаг взятия на проходе 
			if (position.black_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					position.taking_passage_yes =undo.return_taking_passage;
					position.taking_passage_x = undo.taking_passage_x;
					position.taking_passage_y = undo.taking_passage_y;
				}

			}// король - флаг рокировки 
			else if (position.black_name_from_coord[f_p] == N_KING)
			{
				position.castling_k = undo.return_castling_k;
				position.castling_q = undo.return_castling_q;

			}// ладья - флаг рокировки  
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					position.castling_q = undo.return_castling_q;
				}

				if (i_p == H8)//63
				{
					position.castling_k =  undo.return_castling_k;
				}

			}

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];

			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//2 взятие
		case CAPTURE_MOVE:

			// король - флаг рокировки
			if (position.black_name_from_coord[f_p] == N_KING)
			{
				position.castling_k = undo.return_castling_k;
				position.castling_q = undo.return_castling_q;

			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					position.castling_q = undo.return_castling_q;
				}

				if (i_p == H8)//63
				{
					position.castling_k = undo.return_castling_k;
				}
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					position.castling_Q = undo.return_castling_Q;
				}

				if (f_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			position.white_name_from_coord[f_p] = t_f;

			// взятие белыми
			// (обратный ход)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ KOD[t_f];
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//5 взятие на проходе
		case TAKING_PASSAGE:

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			 position.white_name_from_coord[(f_p + 8)] = t_f;

			// взятие белыми
			// (обратный ход)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[(f_p + 8)];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[(f_p + 8)]] = position.white_piece[ position.white_name_from_coord[(f_p + 8)]] ^ move_masks[(f_p + 8)];

			
			position.all_piece = position.all_white_piece | position.all_black_piece;
   
		break;

		//3 0-0-0
		case LONG_CASTLING:

			// король - флаг рокировки
			position.castling_q = undo.return_castling_q;
			position.castling_k = undo.return_castling_k;

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			position.all_black_piece = position.all_black_piece ^ move_masks[D8];
			position.black_piece[position.black_name_from_coord[D8]] 
			= position.black_piece[position.black_name_from_coord[D8]] ^ move_masks[D8];
			position.black_name_from_coord[A8] = position.black_name_from_coord[D8];
			position.black_name_from_coord[D8] = 0;

			//рисуем фигуру на новом месте
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];

			position.all_black_piece = position.all_black_piece ^ move_masks[A8];
			position.black_piece[position.black_name_from_coord[A8]] 
			= position.black_piece[position.black_name_from_coord[A8]] ^ move_masks[A8];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//4 0-0
		case SHORT_CASTLING:
 
			// король - флаг рокировки
			position.castling_q = undo.return_castling_q;
			position.castling_k = undo.return_castling_k;

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			position.all_black_piece = position.all_black_piece ^ move_masks[F8];
			position.black_piece[position.black_name_from_coord[F8]] 
			= position.black_piece[position.black_name_from_coord[F8]] ^ move_masks[F8];
			position.black_name_from_coord[H8] = position.black_name_from_coord[F8];
			position.black_name_from_coord[F8] = 0;

			//рисуем фигуру на новом месте
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];

			position.all_black_piece = position.all_black_piece ^ move_masks[H8];
			position.black_piece[position.black_name_from_coord[H8]] 
			= position.black_piece[position.black_name_from_coord[H8]] ^ move_masks[H8];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		// вернем взятие 15-ферзья 14-ладьи 13-слона 12-коня
		case TRANSFORMATION_IN_QUEEN:
		case TRANSFORMATION_IN_ROOK:
		case TRANSFORMATION_IN_BISHOP:
		case TRANSFORMATION_IN_KNIGHT:

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
		  
			// превращение белыми
			// (обратный ход)
#if MATERIAL
			position.value_black_material = position.value_black_material
				- KOD[position.black_name_from_coord[f_p]] + PAWN; 

#endif//#if MATERIAL
			
			position.black_name_from_coord[i_p] = N_PAWN;
			position.black_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//вернем взятие с превращением 25-ферзья 24-ладьи 23-слона 22-коня
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// если сьедена ладья то запрещаем рокировки с ее участием
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					position.castling_Q = undo.return_castling_Q;
				}

				if (f_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}
			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// превращение белыми
			// (обратный ход)
#if MATERIAL
			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]] + PAWN; 

			position.value_white_material = 
				position.value_white_material + KOD[t_f];
#endif//#if MATERIAL

			position.black_name_from_coord[i_p] = N_PAWN;
			position.black_name_from_coord[f_p] = 0;

			 position.white_name_from_coord[f_p] = t_f;

			//рисуем фигуру на новом месте
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		default:;//ход не распознан
	}
}

/*
 для справки 

N_PAWN      1
N_KNIGHT    2
N_BISHOP    3
N_ROOK      4
N_QUEEN     5
N_KING      6
*/
