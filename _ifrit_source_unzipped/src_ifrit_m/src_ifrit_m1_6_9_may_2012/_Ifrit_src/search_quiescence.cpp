
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.12.2008 19.11.2009 22.06.10 22.11.10 28.09.2011

#include <fstream>
#include <iostream>

#include "move_list_struct.h"
#include "constants.h"
#include "search_const.h"
#include "search_quiescence.h"
#include "evaluate.h"
#include "move_generation.h"
#include "make_move.h"
#include "move_ordering.h"
#include "pv_save.h"
#include "search_switch.h"


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
	std::ofstream Test_Loop_f;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*

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

*/

//	===================================================================
// просчитываем только взятия для белых
__int32 Search_quiescence::forced_variant_white
(
	struct Position & position,// представление доски
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
	const unsigned __int8 new_depth = depth + 1; // следующая глубина

	struct List list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)

	struct PV_line pv_best_point;// лучший для данного узла вариант

#if DELTA_PRUNING
	/// запоминаем фигуры до резки
	const unsigned __int64 save_all_black_piece = position.all_black_piece;
#endif //#if DELTA_PRUNING

	// находим статическую оценку позиции
	value_stat = position.value_white_material
		- position.value_black_material
		+ Evaluate::eval_state(position)
		;

	// только материал
	//value_stat = (bitboard.value_white_material - bitboard.value_black_material);


	//------------------------------------------------
	// TEST
	// проверяем на правильность пошаговую оценку материала

	//if (Evaluate::eval_white_material(position) != position.value_white_material)
	//{
	//	std::cout << "WHITE eval_white_material != bitboard.value_white_material" <<"\n";
	//	std::cout << "eval_white_material(position)= "<< Evaluate::eval_white_material(position)<<"\n";
	//	std::cout << "position.value_white_material= "<< position.value_white_material<<"\n";
	//	exit(1);
	//}

	//if (Evaluate::eval_black_material(position) != position.value_black_material)
	//{
	//	std::cout << "WHITE eval_black_material != position.value_black_material" <<"\n";
	//	std::cout << "eval_black_material(position)= "<< Evaluate::eval_black_material(position)<<"\n";
	//	std::cout << "position.value_black_material= "<< position.value_black_material<<"\n";
	//	exit(1);
	//}

	//if (Evaluate::eval_difference_material(position) 
	//	!= (position.value_white_material - position.value_black_material) )
	//{
	//	std::cout << "WHITE eval_difference_material != (position.value_white_material - position.value_black_material)" <<"\n";
	//	std::cout << "eval_difference_material(position)= "<< Evaluate::eval_difference_material(position)<<"\n";
	//	std::cout << "(position.value_white_material - position.value_black_material)= " << (position.value_white_material - position.value_black_material)<<"\n";
	//	exit(1);
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
 
    //
	value_max_din = value_stat;

#if DELTA_PRUNING
	/*
	напомню что при взятиях черных фигур пишем что то типа
	captures_mask = (all_black_piece & knigt_moves_masks[from]);
	таким образом если в all_black_piece фигуры нет 
	то и взятия не будет
	*/

	if (value_stat < (alpha - PAWN_MARGIN) )// взятие пешек исключаем
	{
		// 
		value_max_din = value_stat + PAWN_MARGIN;

		position.all_black_piece = position.all_black_piece 
			^ position.black_piece[N_PAWN];

		if (value_stat < (alpha - MAX_BISHOP_KNIGHT_MARGIN) )// взятие слонов и коней исключаем
		{
			// 
			value_max_din = value_stat + MAX_BISHOP_KNIGHT_MARGIN;

			position.all_black_piece = position.all_black_piece 
				^ position.black_piece[N_KNIGHT];

			position.all_black_piece = position.all_black_piece 
				^ position.black_piece[N_BISHOP];

			if (value_stat < (alpha - ROOK_MARGIN) )// взятие ладьи исключаем
			{
			// 
			value_max_din = value_stat + ROOK_MARGIN;

				position.all_black_piece = position.all_black_piece 
					^ position.black_piece[N_ROOK];

				if (value_stat < (alpha - QUEEN_MARGIN) )// взятие ферзя исключаем
				{
				// 
				value_max_din = value_stat + QUEEN_MARGIN;

					position.all_black_piece = position.all_black_piece 
						^ position.black_piece[N_QUEEN];
				}
			}
		}
	}
#endif //#if DELTA_PRUNING

	//насчитываем список возможных взятий и сортируем его
	Move_generation::generator_captures_white(list_surplus_moves,position);
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);
	Move_ordering::sorting_moves_captures(list_surplus_moves);
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);

#if DELTA_PRUNING
	// возвращаем на доску фигуры
	position.all_black_piece = save_all_black_piece;
#endif //#if DELTA_PRUNING


	// бежим по списку возможных ходов
	// режим с альфа-бета отсечением
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++)
	{
		//TEST
		//cout << "white depth= " << depth<<"\n";
		//cout << "i = " << i <<"\n";
		//cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

		// реализуем ход
		Make_move::do_moves_white(position,i,depth,list_surplus_moves);

		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине.
		if (flag_pv_f)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		// если короля не съели
		if ((list_surplus_moves.move[i] & 63) != 6)
		{
			// рекурсивный вызов функции
			value_din = forced_variant_black(position,alpha,beta,new_depth
				,flag_pv_f);

			// восстанавливаем позицию
			Make_move::undo_moves_white(position,i,depth,list_surplus_moves);


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
						PV_save::update_PV_best_point(pv_best_point);
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

			Make_move::undo_moves_white(position,i,depth,list_surplus_moves);

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				PV_save::update_PV_best_max_depth(value_stat,depth);
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
				PV_save::update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// теперь мы местный лучший вариант скопируем в структуру 
			// PV_best что бы передать на более высокий уровень
			if (flag_pv_f)
			{
				PV_save::update_PV_best(pv_best_point);
			}
		}
	}
	return value_max_din;
}

//	===================================================================
// просчитываем только взятия для черных
__int32 Search_quiescence::forced_variant_black
(
	struct Position & position,// представление доски
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
	const unsigned __int8 new_depth = depth + 1; // следующая глубина

	struct List list_surplus_moves; // список возможных ходов (всевозможные ходы из данного узла)

	struct PV_line pv_best_point;// лучший для данного узла вариант

#if DELTA_PRUNING
	/// запоминаем фигуры до резки
	const unsigned __int64 save_all_white_piece = position.all_white_piece;
#endif //#if DELTA_PRUNING

	value_stat = position.value_white_material
		- position.value_black_material
		+ Evaluate::eval_state(position)
		;
 
	// только материал
	//value_stat = bitboard.value_white_material - bitboard.value_black_material;

	//------------------------------------------------
	// TEST
	// проверяем на правильность пошаговую оценку материала

	//if (Evaluate::eval_white_material(position) != position.value_white_material)
	//{
	//	std::cout << "WHITE eval_white_material != bitboard.value_white_material" <<"\n";
	//	std::cout << "eval_white_material(position)= "<< Evaluate::eval_white_material(position)<<"\n";
	//	std::cout << "position.value_white_material= "<< position.value_white_material<<"\n";
	//	exit(1);
	//}

	//if (Evaluate::eval_black_material(position) != position.value_black_material)
	//{
	//	std::cout << "WHITE eval_black_material != position.value_black_material" <<"\n";
	//	std::cout << "eval_black_material(position)= "<< Evaluate::eval_black_material(position)<<"\n";
	//	std::cout << "position.value_black_material= "<< position.value_black_material<<"\n";
	//	exit(1);
	//}

	//if (Evaluate::eval_difference_material(position) 
	//	!= (position.value_white_material - position.value_black_material) )
	//{
	//	std::cout << "WHITE eval_difference_material != (position.value_white_material - position.value_black_material)" <<"\n";
	//	std::cout << "eval_difference_material(position)= "<< Evaluate::eval_difference_material(position)<<"\n";
	//	std::cout << "(position.value_white_material - position.value_black_material)= " << (position.value_white_material - position.value_black_material)<<"\n";
	//	exit(1);
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


	//
	value_min_din = value_stat;

#if DELTA_PRUNING

	if (value_stat > (beta + PAWN_MARGIN) )
	{
		value_min_din = value_stat - PAWN_MARGIN;
		position.all_white_piece = position.all_white_piece 
			^ position.white_piece[N_PAWN];

		if (value_stat > (beta + MAX_BISHOP_KNIGHT_MARGIN) )
		{
			value_min_din = value_stat - MAX_BISHOP_KNIGHT_MARGIN;
			position.all_white_piece = position.all_white_piece 
				^ position.white_piece[N_KNIGHT];
			position.all_white_piece = position.all_white_piece 
				^ position.white_piece[N_BISHOP];      

			if (value_stat > (beta + ROOK_MARGIN) )
			{
				value_min_din = value_stat - ROOK_MARGIN;
				position.all_white_piece = position.all_white_piece 
					^ position.white_piece[N_ROOK];

				if (value_stat > (beta + QUEEN_MARGIN) )
				{
					value_min_din = value_stat - QUEEN_MARGIN;
					position.all_white_piece = position.all_white_piece 
						^ position.white_piece[N_QUEEN];
				}
			}
		}
	}
#endif //#if DELTA_PRUNING

	//насчитываем список возможных взятий
	Move_generation::generator_captures_black(list_surplus_moves,position);
	Move_ordering::sorting_moves_captures(list_surplus_moves);

#if DELTA_PRUNING
	// возвращаем на доску фигуры
	position.all_white_piece = save_all_white_piece;
#endif //#if DELTA_PRUNING

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
		Make_move::do_moves_black(position,i,depth,list_surplus_moves);

		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине.
		if (flag_pv_f)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		// если короля не съели
		if ((list_surplus_moves.move[i] & 63) != 6)
		{
			// рекурсивный вызов функции
			value_din = forced_variant_white(position,alpha,beta,new_depth
				,flag_pv_f);

			// восстанавливаем позицию
			Make_move::undo_moves_black(position,i,depth,list_surplus_moves);


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
						PV_save::update_PV_best_point(pv_best_point);
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
			Make_move::undo_moves_black(position,i,depth,list_surplus_moves);

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				PV_save::update_PV_best_max_depth(value_stat,depth);
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
				PV_save::update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// теперь мы местный лучший вариант скопируем в структуру PV_best 
			// что бы передать на более высокий уровень
			if (flag_pv_f)
			{
				PV_save::update_PV_best(pv_best_point);
			}
		}
	}
	return value_min_din;
}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
#if TEST_F
void Search_quiescence::list_Print_F
(
	int depth,// текущая глубина
	const namespace List_surplus_moves & list_surplus_moves//список ходов
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
