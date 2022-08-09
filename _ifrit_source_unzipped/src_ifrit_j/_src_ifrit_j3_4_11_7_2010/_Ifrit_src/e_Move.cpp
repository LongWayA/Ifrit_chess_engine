#include <iostream> // TEST

#include "e_Move.h"
#include "h_Evaluate.h"
#include "d_Bitboard_const.h"

//---------------------------------------------------------------------------
/*++
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.02.2009 26.06.2010
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 99,99999%

 задаем функции по реализации ходов
*/

//	===================================================================
// реализуем ходы вперед для белых
void MOVES::public_onward_moves_white
(
	const unsigned __int8 & i,//номер хода по списку
	struct T_undo undo[],//информация для отката хода
	const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
	struct T_Bitboard * const p_bitboard// представление позиции
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);// исходная позиция фигуры
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);// финальная позиция фигуры
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);// взятая фигура

	// запоминаем флаг взятия на проходе
	undo->return_taking_passage = p_bitboard->taking_passage_yes;

	// всегда запоминаем(нужно для хеш-таблицы)
	undo->taking_passage_x = p_bitboard->taking_passage_x;
	undo->taking_passage_y = p_bitboard->taking_passage_y;

	// запрещаем взятие на проходе 
	p_bitboard->taking_passage_yes = 0;

	// белые сходят значит станет ход черных
	p_bitboard->colour_move = 0;

	// всегда запоминаем флаги рокировок (нужно для хеш-таблицы)
	undo->return_castling_K = p_bitboard->castling_K;
	undo->return_castling_Q = p_bitboard->castling_Q;
	undo->return_castling_k = p_bitboard->castling_k;
	undo->return_castling_q = p_bitboard->castling_q;

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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{
		//1 ход на пустое поле
		case BITBOARDGENERATOR::SIMPLE_MOVE:

			//стираем фигуру на старом месте---------------------------------------------- 
			//стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			//стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			//переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте------------------------------------------------
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			//---------------------------------------------------------------------------------
			// особо отмечаем случаи влияющие на рокировку и взятие на проходе
			// мы тут смотрим какой фигурой ходили и если это пешка то проверяем не нужно
			// ли отметить взятие на проходе если это король то отменяем его рокировки
			// а если ладья то отменяем рокировку с ней 
			//---------------------------------------------------------------------------------

			// пешка - флаг взятия на проходе
			if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					undo->taking_passage_x = p_bitboard->taking_passage_x;
					undo->taking_passage_y = p_bitboard->taking_passage_y;
					p_bitboard->taking_passage_yes = 1;
					p_bitboard->taking_passage_x =(i_p + 8) % 8;
					p_bitboard->taking_passage_y =(i_p + 8) / 8;
				}
			}// король - флаг рокировки
			else if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				// запоминаем флаги рокировок
				undo->return_castling_K = p_bitboard->castling_K;
				undo->return_castling_Q = p_bitboard->castling_Q;
				p_bitboard->castling_K = 0;
				p_bitboard->castling_Q = 0;	   	
			}// ладья - флаг рокировки  
			else if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{
				if (i_p == A1)//0
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (i_p == H1)//7
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}
			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 взятие
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			// убираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// убираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// взятие белыми
			// у черных забираем фигуру
#if MATERIAL
			p_bitboard->value_black_material = p_bitboard->value_black_material 
				- EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// обнуляем взятую фигуру
			p_bitboard->black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
			// рисуем берущую фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// король - флаг рокировки
			if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				// запоминаем флаги рокировок
				undo->return_castling_K = p_bitboard->castling_K;
				undo->return_castling_Q = p_bitboard->castling_Q;
				p_bitboard->castling_K = 0;
				p_bitboard->castling_Q = 0;		
  
			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{	
				if (i_p == A1)//0
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q =0;
				}

				if (i_p == H1)//7
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K =0;
				}
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A8)//56
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == H8)//63
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
		break;

		//5 взятие на проходе
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			// убираем взятую фигуру из маски всех черных фигур
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
			// кроме того убираем взятую фигуру из битового списка фигур
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

			// взятие белыми
			// у черных забираем пешку
#if MATERIAL
			p_bitboard->value_black_material = p_bitboard->value_black_material 
				- EVALUATE::PAWN;	
#endif//#if MATERIAL


			// обнуляем взятую фигуру
			p_bitboard->black_name_from_coord[(f_p - 8)] = 0;

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//3 рокировка 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			//стираем фигуру на старом месте
			// ход королем
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ход ладьей
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[A1];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[A1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[A1]] ^ move_masks_m[A1];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[D1] = p_bitboard->white_name_from_coord[A1];
			p_bitboard->white_name_from_coord[A1] = 0;

			//рисуем фигуру на новом месте------------------
			// ход королем
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ход ладьей
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[D1];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[D1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[D1]] ^ move_masks_m[D1];

			// король - флаг рокировки
			// запоминаем флаги рокировок
			undo->return_castling_Q = p_bitboard->castling_Q;
			undo->return_castling_K = p_bitboard->castling_K;
			p_bitboard->castling_Q = 0;
			p_bitboard->castling_K = 0;

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 рокировка 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:

			//стираем фигуру на старом месте
			// ход королем
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ход ладьей
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[H1];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[H1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[H1]] ^ move_masks_m[H1];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[F1] = p_bitboard->white_name_from_coord[H1];
			p_bitboard->white_name_from_coord[H1] = 0;

			//рисуем фигуру на новом месте
			// ход королем
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ход ладьей
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[F1];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[F1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[F1]] ^ move_masks_m[F1];

			// король - флаг рокировки
			// запоминаем флаги рокировок
			undo->return_castling_Q = p_bitboard->castling_Q;
			undo->return_castling_K = p_bitboard->castling_K;
			p_bitboard->castling_Q = 0;
			p_bitboard->castling_K = 0;

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//15 превращение в ферзя
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_QUEEN;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				+ EVALUATE::QUEEN - EVALUATE::PAWN;
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//14 превращение в ладью
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_ROOK;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				+ EVALUATE::ROOK - EVALUATE::PAWN;	
#endif//#if MATERIAL
			

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//13 превращение в слона
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:

			//стираем фигуру на старом месте 
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_BISHOP;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
				p_bitboard->value_white_material = p_bitboard->value_white_material 
					+ EVALUATE::BISHOP - EVALUATE::PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//12 превращение в коня
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			//стираем фигуру на старом месте
			// стираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// переносим имя фигуры в новую координату и обнуляем его по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_KNIGHT;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// превращение белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				+ EVALUATE::KNIGHT - EVALUATE::PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// рисуем фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
		break;

		//25 взятие с превращением в ферзя
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			// убираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// убираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			p_bitboard->value_white_material = 
				p_bitboard->value_white_material + EVALUATE::QUEEN - EVALUATE::PAWN;

			p_bitboard->value_black_material = p_bitboard->value_black_material	
				- EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// обнуляем взятую фигуру
			p_bitboard->black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и зануляем по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_QUEEN;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем берущую фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A8)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == H8)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//24 взятие с превращением в ладью
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			// убираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// убираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			p_bitboard->value_white_material = 
				p_bitboard->value_white_material + EVALUATE::ROOK - EVALUATE::PAWN;

			p_bitboard->value_black_material = p_bitboard->value_black_material 
				- EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// зануляем взятую фигуру
			p_bitboard->black_name_from_coord[f_p] = 0;
			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_ROOK;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
			// рисуем берущую фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A8)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == H8)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//23 взятие с превращением слона
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			// убираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// убираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];


			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			p_bitboard->value_white_material = 
				p_bitboard->value_white_material + EVALUATE::BISHOP - EVALUATE::PAWN;

			p_bitboard->value_black_material = p_bitboard->value_black_material 
				- EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// зануляем взятую фигуру
			p_bitboard->black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_BISHOP;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем берущую фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A8)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == H8)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//22 взятие с превращением  в коня
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// убираем фигуру из маски всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// убираем взятую фигуру из маски всех черных фигур
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// убираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// кроме того убираем взятую фигуру из битового списка фигур
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];


			// взятие с превращением белыми
			// к оценке материала былых
			// мы прибавляем значение фигуры превращения и отнимаем пешку
			// от оценки матерала черных отнимаем взятую фигуру
#if MATERIAL
			p_bitboard->value_white_material = 
				p_bitboard->value_white_material + EVALUATE::KNIGHT - EVALUATE::PAWN;

			p_bitboard->value_black_material = p_bitboard->value_black_material 
				- EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// обнуляем взятую фигуру
			p_bitboard->black_name_from_coord[f_p] = 0;

			// переносим имя фигуры и зануляем по старой координате
			p_bitboard->white_name_from_coord[f_p] = BITBOARDGENERATOR::N_KNIGHT;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			// рисуем берущую фигуру в маске всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// рисуем берущую фигуру в битовом списке фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];  

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f  == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A8)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == H8)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//ход не распознан
   
	}

}

//	===================================================================
// реализуем ходы вперед для черных
void MOVES::public_onward_moves_black
(
	const unsigned __int8 & i,//номер хода по списку
	struct T_undo undo[],//информация для отката хода
	const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
	struct T_Bitboard * const p_bitboard// представление позиции
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


	// запоминаем флаг взятия на проходе
	undo->return_taking_passage= p_bitboard->taking_passage_yes;

	// всегда запоминаем(нужно для хеш-таблицы)
	undo->taking_passage_x = p_bitboard->taking_passage_x;
	undo->taking_passage_y = p_bitboard->taking_passage_y;


	// запрещаем взятие на проходе 
	p_bitboard->taking_passage_yes = 0;

	// черные сходят значит станет ход белых
	p_bitboard->colour_move = 1;

	// всегда запоминаем флаги рокировок (нужно для хеш-таблицы)
	undo->return_castling_K = p_bitboard->castling_K;
	undo->return_castling_Q = p_bitboard->castling_Q;
	undo->return_castling_k = p_bitboard->castling_k;
	undo->return_castling_q = p_bitboard->castling_q;


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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{

		//1 ход на пустое поле
		case BITBOARDGENERATOR::SIMPLE_MOVE:

			//стираем фигуру на старом месте 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// пешка - флаг взятия на проходе 
			if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					//cout << "i_p= "<< i_p<<"\n";
					//cout << "f_p= "<< f_p<<"\n";
					//cout << "p_bitboard->black_name_from_coord[f_p]= "<< p_bitboard->black_name_from_coord[f_p]<<"\n";
					undo->taking_passage_x = p_bitboard->taking_passage_x;
					undo->taking_passage_y = p_bitboard->taking_passage_y;
					p_bitboard->taking_passage_yes =1;
					p_bitboard->taking_passage_x =(i_p - 8) % 8;
					p_bitboard->taking_passage_y =(i_p - 8) / 8;
				}
			
			}// король - флаг рокировки
			else if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				undo->return_castling_k = p_bitboard->castling_k;
				undo->return_castling_q = p_bitboard->castling_q;
				p_bitboard->castling_k = 0;
				p_bitboard->castling_q = 0;

    
			}// ладья - флаг рокировки   
			else if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 взятие
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			//стираем фигуру на старом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// взятие черными
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL


			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// король - флаг рокировки
			if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				undo->return_castling_k = p_bitboard->castling_k;
				undo->return_castling_q = p_bitboard->castling_q;
				p_bitboard->castling_k = 0;
				p_bitboard->castling_q = 0;

			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
     
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		// 5 взятие на проходе
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			//стираем фигуру на старом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];
  
			// взятие черными
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::PAWN;	
#endif//#if MATERIAL
			
			p_bitboard->white_name_from_coord[(f_p + 8)] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//3 рокировка 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			//стираем фигуру на старом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[A8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[A8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[A8]] ^ move_masks_m[A8];
			p_bitboard->black_name_from_coord[D8] = p_bitboard->black_name_from_coord[A8];
			p_bitboard->black_name_from_coord[A8] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[D8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[D8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[D8]] ^ move_masks_m[D8];


			// король - флаг рокировки 
			undo->return_castling_q = p_bitboard->castling_q;
			undo->return_castling_k = p_bitboard->castling_k;
			p_bitboard->castling_q = 0;
			p_bitboard->castling_k = 0; 

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 рокировка 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:

			//стираем фигуру на старом месте 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[H8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[H8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[H8]] ^ move_masks_m[H8];
			p_bitboard->black_name_from_coord[F8] = p_bitboard->black_name_from_coord[H8];
			p_bitboard->black_name_from_coord[H8] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[F8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[F8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[F8]] ^ move_masks_m[F8];


			// король - флаг рокировки
			undo->return_castling_q = p_bitboard->castling_q;
			undo->return_castling_k = p_bitboard->castling_k;
			p_bitboard->castling_q = 0;
			p_bitboard->castling_k = 0;

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//15 превращение в ферзя
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:

			//стираем фигуру на старом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_QUEEN;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// превращение черными
#if MATERIAL
			p_bitboard->value_black_material = 
				p_bitboard->value_black_material + EVALUATE::QUEEN - EVALUATE::PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//14 превращение в ладью
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:

			//стираем фигуру на старом месте 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_ROOK;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// превращение черными
			#if MATERIAL
				p_bitboard->value_black_material = 
					p_bitboard->value_black_material + EVALUATE::ROOK - EVALUATE::PAWN;	
			#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//13 превращение в слона
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:

			//стираем фигуру на старом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_BISHOP;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// превращение черными
#if MATERIAL
			p_bitboard->value_black_material = 
				p_bitboard->value_black_material + EVALUATE::BISHOP - EVALUATE::PAWN;	
#endif//#if MATERIAL


			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//12 превращение в конь
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			//стираем фигуру на старом месте 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_KNIGHT;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// превращение черными
#if MATERIAL
			p_bitboard->value_black_material = 
				p_bitboard->value_black_material + EVALUATE::KNIGHT - EVALUATE::PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// корректируем маску всех фигур
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//25 взятие с превращением в ферзя
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// превращение с взятием черными
#if MATERIAL
			p_bitboard->value_black_material = 
				p_bitboard->value_black_material + EVALUATE::QUEEN - EVALUATE::PAWN;

			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL


			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_QUEEN;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}	
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//24 взятие с превращением в ладью
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// превращение с взятием черными
#if MATERIAL
			p_bitboard->value_black_material = 
				p_bitboard->value_black_material + EVALUATE::ROOK - EVALUATE::PAWN;

			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL

			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_ROOK;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// если сьедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//23 взятие с превращением слона
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// превращение с взятием черными
#if MATERIAL
			p_bitboard->value_black_material = 
				p_bitboard->value_black_material + EVALUATE::BISHOP - EVALUATE::PAWN;

			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL

			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_BISHOP;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// если сьедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//22 взятие с превращением  в коня
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// превращение с взятием черными
#if MATERIAL
			p_bitboard->value_black_material 
				= p_bitboard->value_black_material + EVALUATE::KNIGHT - EVALUATE::PAWN;

			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL

			p_bitboard->white_name_from_coord[f_p] = 0;
			p_bitboard->black_name_from_coord[f_p] = BITBOARDGENERATOR::N_KNIGHT;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//ход не распознан
   
	}

}

//	===================================================================
// ход назад для белых
void MOVES::public_backs_moves_white
(
	const unsigned __int8 & i,//номер хода по списку
	const struct T_undo undo[],//информация для отката хода
	const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
	struct T_Bitboard * const p_bitboard// представление позиции
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


	// восстанавливаем флаг взятия на проходе
	p_bitboard->taking_passage_yes=undo->return_taking_passage;

	// белые забрали свой ход значит опять их ход стал
	p_bitboard->colour_move = 1;

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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{
		//1 ход на пустое поле
		case BITBOARDGENERATOR::SIMPLE_MOVE:
 
			// пешка - флаг взятия на проходе 
			if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					p_bitboard->taking_passage_yes =undo->return_taking_passage;
					p_bitboard->taking_passage_x = undo->taking_passage_x;
					p_bitboard->taking_passage_y = undo->taking_passage_y;
				}

				
			}// король - флаг рокировки 
			else if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				// вспоминаем флаги рокировок
				p_bitboard->castling_K = undo->return_castling_K;
				p_bitboard->castling_Q = undo->return_castling_Q;   

	    
			}// ладья - флаг рокировки   
			else if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{ 
				if (i_p == A1)//0
				{
					p_bitboard->castling_Q = undo->return_castling_Q;
				}

				if (i_p == H1)//7
				{
					p_bitboard->castling_K = undo->return_castling_K;
				}
			}

			//стираем фигуру на старом месте
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 взятие
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			// король - флаг рокировки
			if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				// вспоминаем флаги рокировок
				p_bitboard->castling_K = undo->return_castling_K;
				p_bitboard->castling_Q = undo->return_castling_Q;   

			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if (p_bitboard->white_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{	
				if (i_p == A1)//0
				{
					p_bitboard->castling_Q = undo->return_castling_Q;
				}

				if (i_p == H1)//7
				{
					p_bitboard->castling_K = undo->return_castling_K;
				}
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A8)//56
				{
					p_bitboard->castling_q = undo->return_castling_q;
				}

				if (f_p == H8)//63
				{
					p_bitboard->castling_k = undo->return_castling_k;
				}
			}

			//стираем фигуру на старом месте 
			// стираем фигуру из списка всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// востанавливаем имя взятой фигуры
			p_bitboard->black_name_from_coord[f_p] = t_f;

			// взятие белыми
			// к оценке материала черных мы прибавляем значение взятой фигуры 
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_black_material = p_bitboard->value_black_material 
				+ EVALUATE::kod[t_f];
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// в маске все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// в маске все черные фигуры
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// битовый список взятой фигуры
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//5 взятие на проходе
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			//стираем фигуру на старом месте
			// стираем фигуру из списка всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// востанавливаем имя взятой фигуры
			p_bitboard->black_name_from_coord[(f_p - 8)] = t_f;

			// взятие белыми
			// к оценке материала черных мы прибавляем значение взятой фигуры 
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_black_material = p_bitboard->value_black_material 
				+ EVALUATE::PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			// в маске все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// в маске все черные фигуры
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// битовый список взятой фигуры
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
		break;

		//3 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			// король - флаг рокировки
			p_bitboard->castling_Q = undo->return_castling_Q;
			p_bitboard->castling_K = undo->return_castling_K;   

			//стираем фигуру на старом месте
			// король
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// ладья
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[D1];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[D1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[D1]] ^ move_masks_m[D1];

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[A1] = p_bitboard->white_name_from_coord[D1];
			p_bitboard->white_name_from_coord[D1] = 0;

			//рисуем фигуру на новом месте
			// король
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ладья
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[A1];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[A1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[A1]] ^ move_masks_m[A1];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:

			// король - флаг рокировки
			p_bitboard->castling_Q = undo->return_castling_Q;
			p_bitboard->castling_K = undo->return_castling_K;   

			//стираем фигуру на старом месте
			// король
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// переносим имя фигуры и зануляем по старой координате
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// ладья
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[F1];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[F1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[F1]] ^ move_masks_m[F1];

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[H1] = p_bitboard->white_name_from_coord[F1];
			p_bitboard->white_name_from_coord[F1] = 0;

			//рисуем фигуру на новом месте
			// король
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ладья
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[H1];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[H1]] 
			= p_bitboard->white_piece[p_bitboard->white_name_from_coord[H1]] ^ move_masks_m[H1];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//вернем 15-ферзь 14-ладью 13-слона 12-коня
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			//стираем фигуру на старом месте
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// превращение белыми
			// от оценки материала былых мы отнимаем значение фигуры 
			// превращения и прибавляем пешку
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]] + EVALUATE::PAWN;
#endif//#if MATERIAL

			// переносим имя фигуры и зануляем по старой координате
			p_bitboard->white_name_from_coord[i_p] = BITBOARDGENERATOR::N_PAWN;
			p_bitboard->white_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			// все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		// вернем взятие с превращением 25-ферзя 24-ладьи 23-слона 22-коня
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			//  вспоминаем если съедена ладья 
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A8)//56
				{
					p_bitboard->castling_q = undo->return_castling_q;
				}

				if (f_p == H8)//63
				{
					p_bitboard->castling_k = undo->return_castling_k;
				}
			}

			//стираем фигуру на старом месте
			// стираем фигуру из списка всех белых фигур
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// стираем фигуру из битового списка фигур
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];


			// превращение белыми
			// от оценки материала былых мы отнимаем значение фигуры 
			// превращения и прибавляем пешку
			// к оценке материала черных мы прибавляем значение взятой фигуры
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				- EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]] + EVALUATE::PAWN;

			p_bitboard->value_black_material = p_bitboard->value_black_material 
				+ EVALUATE::kod[t_f];
#endif//#if MATERIAL
			//------------------------------------------------------------------------------

			// переносим имя фигуры и обнуляем по старой координате
			p_bitboard->white_name_from_coord[i_p] = BITBOARDGENERATOR::N_PAWN;
			p_bitboard->white_name_from_coord[f_p] = 0;

			// восстанавливаем имя взятой фигуры
			p_bitboard->black_name_from_coord[f_p] = t_f;

			//рисуем фигуру на новом месте
			// в маске все белые фигуры
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// в маске все черные фигуры
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// битовый список фигуры
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// битовый список взятой фигуры
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//ход не распознан
	}
}


//	===================================================================
// ход назад для черных
void MOVES::public_backs_moves_black
(
	const unsigned __int8 & i,//номер хода по списку
	const struct T_undo undo[],//информация для отката хода
	const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
	struct T_Bitboard * const p_bitboard// представление позиции
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


	// востанавливаем флаг взятия на проходе
	p_bitboard->taking_passage_yes=undo->return_taking_passage;

	// черные забрали свой ход значит опять их ход стал
	p_bitboard->colour_move = 0;

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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{
		//1 ход на пустое поле
		case BITBOARDGENERATOR::SIMPLE_MOVE:

			// пешка - флаг взятия на проходе 
			if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					p_bitboard->taking_passage_yes =undo->return_taking_passage;
					p_bitboard->taking_passage_x = undo->taking_passage_x;
					p_bitboard->taking_passage_y = undo->taking_passage_y;
				}

			}// король - флаг рокировки 
			else if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				p_bitboard->castling_k = undo->return_castling_k;
				p_bitboard->castling_q = undo->return_castling_q;

			}// ладья - флаг рокировки  
			else if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{	
				if (i_p == A8)//56
				{
					p_bitboard->castling_q = undo->return_castling_q;
				}

				if (i_p == H8)//63
				{
					p_bitboard->castling_k =  undo->return_castling_k;
				}

			}

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 взятие
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			// король - флаг рокировки
			if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_KING)
			{
				p_bitboard->castling_k = undo->return_castling_k;
				p_bitboard->castling_q = undo->return_castling_q;

			}// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем 
			else if (p_bitboard->black_name_from_coord[f_p] == BITBOARDGENERATOR::N_ROOK)
			{	
				if (i_p == A8)//56
				{
					p_bitboard->castling_q = undo->return_castling_q;
				}

				if (i_p == H8)//63
				{
					p_bitboard->castling_k = undo->return_castling_k;
				}
			}

			// если съедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A1)//0
				{
					p_bitboard->castling_Q = undo->return_castling_Q;
				}

				if (f_p == H1)//7
				{
					p_bitboard->castling_K = undo->return_castling_K;
				}
			}

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->white_name_from_coord[f_p] = t_f;

			// взятие белыми
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				+ EVALUATE::kod[t_f];
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//5 взятие на проходе
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->white_name_from_coord[(f_p + 8)] = t_f;

			// взятие белыми
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_white_material = p_bitboard->value_white_material 
				+ EVALUATE::PAWN;	
#endif//#if MATERIAL

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];

			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
		break;

		//3 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			// король - флаг рокировки
			p_bitboard->castling_q = undo->return_castling_q;
			p_bitboard->castling_k = undo->return_castling_k;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[D8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[D8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[D8]] ^ move_masks_m[D8];
			p_bitboard->black_name_from_coord[A8] = p_bitboard->black_name_from_coord[D8];
			p_bitboard->black_name_from_coord[D8] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[A8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[A8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[A8]] ^ move_masks_m[A8];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:
 
			// король - флаг рокировки
			p_bitboard->castling_q = undo->return_castling_q;
			p_bitboard->castling_k = undo->return_castling_k;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[F8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[F8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[F8]] ^ move_masks_m[F8];
			p_bitboard->black_name_from_coord[H8] = p_bitboard->black_name_from_coord[F8];
			p_bitboard->black_name_from_coord[F8] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[H8];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[H8]] 
			= p_bitboard->black_piece[p_bitboard->black_name_from_coord[H8]] ^ move_masks_m[H8];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		// вернем взятие 15-ферзья 14-ладьи 13-слона 12-коня
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
		  
			// превращение белыми
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_black_material = p_bitboard->value_black_material
				- EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]] + EVALUATE::PAWN; 

#endif//#if MATERIAL
			
			p_bitboard->black_name_from_coord[i_p] = BITBOARDGENERATOR::N_PAWN;
			p_bitboard->black_name_from_coord[f_p] = 0;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//вернем взятие с превращением 25-ферзья 24-ладьи 23-слона 22-коня
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// если сьедена ладья то запрещаем рокировки с ее участием
			if (t_f == BITBOARDGENERATOR::N_ROOK)
			{
				if (f_p == A1)//0
				{
					p_bitboard->castling_Q = undo->return_castling_Q;
				}

				if (f_p == H1)//7
				{
					p_bitboard->castling_K = undo->return_castling_K;
				}
			}
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// превращение белыми
			// (обратный ход)
#if MATERIAL
			p_bitboard->value_black_material = p_bitboard->value_black_material 
				- EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]] + EVALUATE::PAWN; 

			p_bitboard->value_white_material = 
				p_bitboard->value_white_material + EVALUATE::kod[t_f];
#endif//#if MATERIAL

			p_bitboard->black_name_from_coord[i_p] = BITBOARDGENERATOR::N_PAWN;
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->white_name_from_coord[f_p] = t_f;

			//рисуем фигуру на новом месте
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//ход не распознан
	}
}

/*
 для справки 

BITBOARDGENERATOR::N_PAWN      1
BITBOARDGENERATOR::N_KNIGHT    2
BITBOARDGENERATOR::N_BISHOP    3
BITBOARDGENERATOR::N_ROOK      4
BITBOARDGENERATOR::N_QUEEN     5
BITBOARDGENERATOR::N_KING      6
*/
