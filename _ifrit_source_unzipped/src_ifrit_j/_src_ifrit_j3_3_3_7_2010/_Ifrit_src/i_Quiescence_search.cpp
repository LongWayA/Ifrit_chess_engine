#include <iostream> // TEST
#include <fstream>  // TEST

#include "i_Quiescence_search.h"
#include "e_Move.h"
#include "h_Evaluate.h"
#include "k_Util.h"
#include "m_Line.h"
#include "d_Bitboard_gen.h"

//---------------------------------------------------------------------------
/*+
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.12.2008 19.11.2009 22.06.10
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 80%

 форсированный перебор позиций 

 Опишу алгоритм расчета взятий для белых. Ясно что для черных аналогично.

 Главное отличие данного алгоритма от обычной альфа-бета оптимизации это использование
 статической оценки для завышения альфы и отсечения по бете. Надо еще заметить что 
 генерируем мы только взятия.

 Что бы понять данный алгоритм нужно вспомнить что такое альфа-бета оптимизация
 и какие особенности добавляет работа со статической оценкой.

 Белые ищут максимум среди минимумов которые нашли черные. Самая большая оценка которую
 белые уже нашли в предыдущем поиске и есть альфа. Таким образом белые стремятся увеличить 
 альфу. Черные ищут минимум. И самая маленькая оценка которую они уже нашли и есть бета.
 Белых интересуют оценки которые больше альфы а все остальные они отбрасывают. Допустим 
 белые сходили и теперь ход черных и найденная оценка меньше альфы. Ясно что поскольку 
 черные ищут минимум в дальнейшем оценка будет только уменьшаться. Но искать дальше 
 бесполезно так как уже ясно что белые эту оценку отбросят.
   
 В форсированном поиске мы считаем только взятия так что программа не видит что брать
 не обязательно а можно просто уклониться. Для того что бы программа не делала 
 проигрывающих взятий мы увеличиваем альфу статической оценкой. В итоге если программа
 найдет лучший вариант то она выберет его а если нет то откажется от дальнейших взятий.

 Когда мы отсекаемся статической оценкой по бете мы исходим из предположения что если 
 позиция уже в нашу пользу а тут еще есть наше взятие то мы либо улучшим оценку либо 
 удержим ее. Так как мы смотрим только взятия и не видим шахов такое предположение не
 уменьшает точность форсированного варианта.

1) Находим статическую оценку позиции.

  1.1 Если статическая оценка больше альфы то присваиваем ее альфе. Проще говоря 
   завышаем альфу статической оценкой.

  1.2 Если альфу завысили то пытаемся сразу отсечь по бете. 

  1.3 Фрагмент кода по пункту 1:

	if (value_stat > alpha)
	{ 
		if (value_stat >= beta)
		{
			return value_stat;
		}

		alpha = value_stat;
		save_best = 1;
	}

---------------------------------------
2) Насчитываем и сортируем по mvv/lva список возможных взятий. Сортировка имеет
   принципиальное значение для ускорения поиска.

  2.1  Дальше обычная альфа-бета. 

  2.2 Фрагмент кода по пункту 2:
	value_max_din = value_stat;

	if (value_din > value_max_din)
	{
		value_max_din = value_din;

		if (value_din > alpha)
		{
			if (value_din >= beta)
			{
				return value_din;
			}

			alpha = value_din;

			//здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			PVSAVE::public_update_PV_best_point(&pv_best_point);

			save_best = 1;
		}
	}

  Принципиальный момент заключается в том что 
  мы пишем только те цепочки взятий или статическую оценку которые улучшают альфу
	if (save_best == 1)
	{
		if (value_max_din == value_stat)
		{
			// статическую оценку улучшить не удалось

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			PVSAVE::public_update_PV_best_max_depth(value_stat,depth); 
		}
		else
		{
			// теперь мы местный лучший вариант скопируем в структуру PV_best что бы передать на более высокий уровень
			PVSAVE::public_update_PV_best(&pv_best_point);
		}
	}
---------------------------------------

*/

namespace 
{
	// 0 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка
	const unsigned __int16 ROOK_MARGIN = 700; // отсчка взятий ладьей
	const unsigned __int16 BISHOP_KNIGHT_MARGIN = 500; // отсчка взятий  слонов и коней
	const unsigned __int8 PAWN_MARGIN = 160; // отсчка взятий  пешек

	const  __int32 VALUE_MATE = 1000000; // оценка мата
	const  __int16 DELTA_MATE = 1000; // дельта для нахождения скорейшего мата
}

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F

	std::ofstream Test_Loop_f;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// просчитываем только взятия для белых
__int32 QUIESCENCESEARCH::public_forced_variant_white
(
	struct T_Bitboard * const p_bitboard,// представление доски
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth,// глубина
	const bool flag_pv_f// основной ли это вариант (pv)
)
{
	bool save_best = 0;// пишем ли лучший вариант
	__int32 value_din; // динамическая оценка
	__int32 value_stat; // статическая оценка
	__int32 value_max_din; // максимум в узле


	struct T_undo undo[1];// информация для возврата хода
	struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	struct T_PV pv_best_point;// лучший для данного узла вариант

#if DELTA_PRUNING
	unsigned __int64 all_black_piece;
#endif

	// находим статическую оценку позиции
	value_stat = p_bitboard->value_white_material - p_bitboard->value_black_material
		+ EVALUATE::public_eval_state(p_bitboard);

	// только материал
	//value_stat = (p_bitboard->value_white_material - p_bitboard->value_black_material);


	//------------------------------------------------
	// TEST
	// проверяем на правильность пошаговую оценку материала

	//if (EVALUATE::public_eval_white_material(p_bitboard) != p_bitboard->value_white_material)
	//{
	//	std::cout << "WHITE public_eval_white_material != p_bitboard->value_white_material" <<"\n";
	//	std::cout << "public_eval_white_material(p_bitboard)= "<< EVALUATE::public_eval_white_material(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard->value_white_material= "<< p_bitboard->value_white_material<<"\n";
	//}

	//if (EVALUATE::public_eval_black_material(p_bitboard) != p_bitboard->value_black_material)
	//{
	//	std::cout << "WHITE public_eval_black_material != p_bitboard->value_black_material" <<"\n";
	//	std::cout << "public_eval_black_material(p_bitboard)= "<< EVALUATE::public_eval_black_material(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard->value_black_material= "<< p_bitboard->value_black_material<<"\n";
	//}

	//if (EVALUATE::public_eval_difference_material(p_bitboard) 
	//	!= (p_bitboard->value_white_material - p_bitboard->value_black_material) )
	//{
	//	std::cout << "WHITE public_eval_difference_material != (p_bitboard->value_white_material - p_bitboard->value_black_material)" <<"\n";
	//	std::cout << "public_eval_difference_material(p_bitboard)= "<< EVALUATE::public_eval_difference_material(p_bitboard)<<"\n";
	//	std::cout << "(p_bitboard->value_white_material - p_bitboard->value_black_material)= " << (p_bitboard->value_white_material - p_bitboard->value_black_material)<<"\n";
	//}
	//------------------------------------------------


	// завышаем альфу что бы не делать проигрывающих взятий.
	if (value_stat > alpha)
	{ 
		// пытаемся сразу отсечь по бете 
		// почему именно по бете?
		// дело в том что в форсированном поиске на предыдущей глубине поиск ведут 
		// черные и они ищут шанса улучшить
		// именно бету
		// мы исходим из того что если у нас позиция уже не нравиться черным
		// то сделав ход мы сможем ее усилить
		if (value_stat >= beta)
		{
			return value_stat;
		}

		alpha = value_stat;
		save_best = 1;
	}

	/*
	Не знаю насколько правильно я реализовал delta pruning.
	Идея такая, что если оценка меньше альфы на полторы пешки, то взятием пешки 
	ситуацию не исправить и такие взятия можно исключить.  То же и для других фигур.
	*/
#if DELTA_PRUNING

	// запоминаем фигуры до резки
	all_black_piece = p_bitboard->all_black_piece;

	if (value_stat < alpha - PAWN_MARGIN )
	{
		/*
		напомню что при взятиях черных фигур пишем что то типа
		captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[from]);
		таким образом если в p_bitboard->all_black_piece фигуры нет 
		то и взятия не будет
		*/

		// взятие пешек исключаем
		p_bitboard->all_black_piece = p_bitboard->all_black_piece 
			^ p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN];

		if (value_stat < alpha - BISHOP_KNIGHT_MARGIN )
		{
			// взятие слонов и коней исключаем
			p_bitboard->all_black_piece = p_bitboard->all_black_piece 
				^ p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece 
				^ p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP];

			if (value_stat < alpha - ROOK_MARGIN )// взятие ладьи исключаем
			{
				p_bitboard->all_black_piece = p_bitboard->all_black_piece 
					^ p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK];
			}
		}
		value_stat = value_stat + PAWN_MARGIN;
	}
#endif //#if DELTA_PRUNING


	//насчитываем список возможных взятий и сортируем его
	BITBOARDGENERATOR::public_generator_captures_white(p_bitboard,&list_surplus_moves);
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);
	sorting_captures_moves(&list_surplus_moves);
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);

#if DELTA_PRUNING
	// возвращаем на доску фигуры
	p_bitboard->all_black_piece = all_black_piece;
#endif //#if DELTA_PRUNING

    //
	value_max_din = value_stat;

	// бежим по списку возможных ходов
	// режим с альфа-бета отсечением
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++)
	{
		//TEST
		//cout << "white depth= " << depth<<"\n";
		//cout << "i = " << i <<"\n";
		//cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

		// реализуем ход
		MOVES::public_onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);

		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине.
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);
		}

		// если короля не съели
		if ((list_surplus_moves.move[i] & 63) != 6)
		{
			// рекурсивный вызов функции
			value_din = public_forced_variant_black(p_bitboard,alpha,beta,(depth+1)
				,flag_pv_f);

			// восстанавливаем позицию
			MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

			if (value_din > value_max_din)
			{

				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
				if (value_din > alpha) 
				{
					if (value_din >= beta)
					{
						return value_din;
					}

					alpha = value_din; 

					// здесь мы перезаписываем лучший вариант соответствующий 
					// лучшей оценке
					if (flag_pv_f)
					{
						PVSAVE::public_update_PV_best_point(&pv_best_point);
					}

					save_best = 1;
				}
				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

				value_max_din = value_din;
			}

		}// обрабатываем случай взятого короля
		else
		{
			// короля сожрали !
			value_stat = (VALUE_MATE - depth * DELTA_MATE);

			MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_max_depth(value_stat,depth);
			}

			return value_stat;

		}
	}

	/*
	принципиальный момент заключается в том что мы пишем только те цепочки взятий
	или статическую оценку которые улучшают альфу
	*/
	if (save_best == 1)
	{
		if (value_max_din == value_stat)
		{// статическую оценку улучшить не удалось

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// теперь мы местный лучший вариант скопируем в структуру 
			// PV_best что бы передать на более высокий уровень
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best(&pv_best_point);
			}
		}
	}
	return value_max_din;
}

//	===================================================================
// просчитываем только взятия для черных
__int32 QUIESCENCESEARCH::public_forced_variant_black
(
	struct T_Bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth,// глубина
	const bool flag_pv_f// основной ли это вариант (pv)
)
{
	bool save_best = 0;// пишем ли лучший вариант
	__int32 value_din; // динамическая оценка
	__int32 value_stat; // статическая оценка
	__int32 value_min_din; //минимум в узле

	struct T_undo undo[1];
	struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	struct T_PV pv_best_point;// лучший для данного узла вариант

#if DELTA_PRUNING
	unsigned __int64 all_white_piece;
#endif //#if DELTA_PRUNING

	value_stat = p_bitboard->value_white_material - p_bitboard->value_black_material
		+ EVALUATE::public_eval_state(p_bitboard);
 
	// только материал
	//value_stat = p_bitboard->value_white_material - p_bitboard->value_black_material;

	//------------------------------------------------
	// TEST
	// проверяем на правильность пошаговую оценку материала

	//if (EVALUATE::public_eval_white_material(p_bitboard) != p_bitboard->value_white_material)
	//{
	//	std::cout << "BLACK public_eval_white_material != p_bitboard->value_white_material" <<"\n";
	//	std::cout << "public_eval_white_material(p_bitboard)= "<< EVALUATE::public_eval_white_material(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard->value_white_material= "<< p_bitboard->value_white_material<<"\n";
	//}

	//if (EVALUATE::public_eval_black_material(p_bitboard) != p_bitboard->value_black_material)
	//{
	//	std::cout << "BLACK value_stat != p_bitboard->value_black_material" <<"\n";
	//	std::cout << "public_eval_black_material(p_bitboard)= "<< EVALUATE::public_eval_black_material(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard->value_black_material= "<< p_bitboard->value_black_material<<"\n";
	//}

	//if (EVALUATE::public_eval_difference_material(p_bitboard) 
	//	!= (p_bitboard->value_white_material - p_bitboard->value_black_material) )
	//{
	//	std::cout << "BLACK public_eval_difference_material != (p_bitboard->value_white_material - p_bitboard->value_black_material)" <<"\n";
	//	std::cout << "public_eval_difference_material(p_bitboard)= "<< EVALUATE::public_eval_difference_material(p_bitboard)<<"\n";
	//	std::cout << "(p_bitboard->value_white_material - p_bitboard->value_black_material)= " << (p_bitboard->value_white_material - p_bitboard->value_black_material)<<"\n";
	//}
	//------------------------------------------------


	if (value_stat < beta)
	{
		if (value_stat <= alpha)
		{
			return value_stat;
		}

		beta = value_stat;
		save_best = 1;
	}


#if DELTA_PRUNING

	// запоминаем фигуры до резки
	all_white_piece = p_bitboard->all_white_piece;

	if (value_stat > beta + PAWN_MARGIN)
	{
		// взятие пешек исключаем
		p_bitboard->all_white_piece = p_bitboard->all_white_piece 
			^ p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN];

		if (value_stat > beta + BISHOP_KNIGHT_MARGIN)
		{
			// взятие слонов и коней исключаем
			p_bitboard->all_white_piece = p_bitboard->all_white_piece 
				^ p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT];

			p_bitboard->all_white_piece = p_bitboard->all_white_piece 
				^ p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP];      

			if (value_stat > beta + ROOK_MARGIN)
			{
				// взятие ладьи исключаем
				p_bitboard->all_white_piece = p_bitboard->all_white_piece 
					^ p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK];   
			}

		}
		value_stat = value_stat - PAWN_MARGIN;
	}
#endif //#if DELTA_PRUNING


	//насчитываем список возможных взятий
	BITBOARDGENERATOR::public_generator_captures_black(p_bitboard,&list_surplus_moves);
	sorting_captures_moves(&list_surplus_moves);

#if DELTA_PRUNING
	// возвращаем на доску фигуры
	p_bitboard->all_white_piece = all_white_piece;
#endif //#if DELTA_PRUNING


	//
	value_min_din = value_stat;

	//если до сих пор отсечек не было значит статическая оценка не катит

	//бежим по списку возможных ходов
	// режим с альфа бета отсечением
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++)
	{
		//TEST
		//cout << "black depth= " << depth<<"\n";
		//cout << "i = " << i <<"\n";
		//cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

		// реализуем ход
		MOVES::public_onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);

		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине.
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);
		}

		// если короля не съели
		if ((list_surplus_moves.move[i] & 63) != 6)
		{
			// рекурсивный вызов функции
			value_din = public_forced_variant_white(p_bitboard,alpha,beta,(depth+1)
				,flag_pv_f);

			// восстанавливаем позицию
			MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

			if (value_din < value_min_din)
			{
				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
				if (value_din < beta)
				{
					if (value_din <= alpha)
					{
						return value_din;
					}

					beta = value_din; 

					// здесь мы перезаписываем лучший вариант соответствующий 
					// лучшей оценке
					if (flag_pv_f)
					{
						PVSAVE::public_update_PV_best_point(&pv_best_point);
					}

					save_best = 1;
				}
				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

				value_min_din = value_din;
			}

		
		}// обрабатываем случай взятого короля
		else
		{
			// короля сожрали!
			value_stat = -(VALUE_MATE - depth * DELTA_MATE);
			MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_max_depth(value_stat,depth);
			}

			return value_stat;

		}
	}

	if (save_best == 1)
	{
		if (value_min_din == value_stat)
		{
			// статическую оценку улучшить не удалось

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// теперь мы местный лучший вариант скопируем в структуру PV_best 
			// что бы передать на более высокий уровень
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best(&pv_best_point);
			}
		}
	}
	return value_min_din;
}

//	===================================================================
// сортируем взятия возвращаем количество взятий
inline unsigned __int8 QUIESCENCESEARCH::sorting_captures_moves
(
	struct T_list_surplus_moves * const list_surplus_moves// список ходов
)
{
	// 
	__int32 move_s;// используем для запоминания хода
	__int32 score_s;// используем для запоминания оценки хода
	unsigned __int8 end_captures = list_surplus_moves->end_captures;//количество взятий
	unsigned __int8 n;

	/*
	если у нас есть взятия тогда мы их сортируем по оценке обсчитанной еще в генераторе 
	взятий идем по списку и сравниваем(меньше ли она) текущую строку со всеми 
	нижестоящими понятно что если у нас текущая строка k то следующая строка будет k + 1
	если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и 
	текущую строку таким образом когда мы доходим до конца все взятия у нас 
	отсортированы по убывающей.
	*/
	for (n = 0; n < end_captures; n++)
	{	
		for (unsigned __int8 i = n + 1; i < end_captures; i++)
		{
			if (list_surplus_moves->sorting_score[n] < list_surplus_moves->sorting_score[i])
			{ 
				// сохраняем данные из n     
				move_s = list_surplus_moves->move[n];
				score_s = list_surplus_moves->sorting_score[n];

				// копируем из i в n
				list_surplus_moves->move[n] = list_surplus_moves->move[i];
				list_surplus_moves->sorting_score[n] = list_surplus_moves->sorting_score[i];

				// восстанавливаем запись
				list_surplus_moves->move[i] = move_s;
				list_surplus_moves->sorting_score[i] = score_s;
			}
		}
	}		
	return n;
}


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
#if TEST_F
void QUIESCENCESEARCH::list_Print_F
(
	int depth,// текущая глубина
	const struct T_list_surplus_moves * list_surplus_moves//список ходов
)
{

	Test_Loop_f.open ("_TEST_LOOP_FORCED.txt",std::ios::app);	
	//Test_Loop_f.open ("_TEST_LOOP_FORCED.txt");	
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop_f << "\n";
	Test_Loop_f << "------------------------------------------------- "<<"\n";
	Test_Loop_f <<"depth= "<< depth << "\n";
	Test_Loop_f << "\n";
	Test_Loop_f << "Список возможных ходов фигуры "<<"\n";

	for (int i = 0 ; i < list_surplus_moves->end_list ; i++)
	{	
		Test_Loop_f << "Номер хода по списку i= " << i << "\n";  
		Test_Loop_f << "Ходящая фигура " << UTIL::public_int_to_char_board(((list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
		Test_Loop_f << "Оценка хода " << list_surplus_moves->sorting_score[i] << "\n";

		// вывод хода вида e2-e4
		//------------------------------
		Test_Loop_f << " " << (UTIL::public_x_to_notation(((list_surplus_moves->move[i]>>12)& 63)%8));
		Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
		//Test_Loop_f << "\n";
		Test_Loop_f << " " << (UTIL::public_x_to_notation(((list_surplus_moves->move[i]>>18)& 63)%8));
		Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
		//------------------------------

		Test_Loop_f << "вид хода " << ((list_surplus_moves->move[i]>>6)& 63)  << "\n";
		Test_Loop_f << "Взятая фигура " << UTIL::public_int_to_char_board(( list_surplus_moves->move[i] & 63),1)<< "\n";
		Test_Loop_f << "начальная положение " << ((list_surplus_moves->move[i]>>12)& 63)  << "\n";
		Test_Loop_f << "конечное положение " << ((list_surplus_moves->move[i]>>18)& 63)  << "\n";
      
		Test_Loop_f << "\n";
	}

	Test_Loop_f << "list_surplus_moves->end_captures= " <<list_surplus_moves->end_captures << "\n";
	Test_Loop_f << "list_surplus_moves->end_list= " <<list_surplus_moves->end_list << "\n";
	Test_Loop_f << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop_f.close();
}
#endif
