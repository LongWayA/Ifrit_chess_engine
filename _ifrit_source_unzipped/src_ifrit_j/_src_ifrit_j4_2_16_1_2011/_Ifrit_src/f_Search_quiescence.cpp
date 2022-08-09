
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.12.2008 19.11.2009 22.06.10 22.11.10

#include <fstream>
#include <iostream>

#include "f_Search_quiescence.h"
#include "g_Evaluate.h"
#include "k_PV_save.h"
#include "l_Chess_bitboard.h"
#include "m_List_surplus_moves.h"

class List_surplus_moves quiescence_list_surplus_moves[MAX_DEPTH];// список возможных ходов (всевозможные ходы из данного узла)

extern class Evaluate evaluate;
extern class PV_save pv_save;
extern class Chess_bitboard bitboard;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
	std::ofstream Test_Loop_f;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// просчитываем только взятия для белых
__int32 Search_quiescence::forced_variant_white
(
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

	class List_surplus_moves & list_surplus_moves 
		= quiescence_list_surplus_moves[depth];// список возможных ходов (всевозможные ходы из данного узла)

	struct PV_line pv_best_point;// лучший для данного узла вариант


	// находим статическую оценку позиции
	value_stat = bitboard.get_value_white_material() 
		- bitboard.get_value_black_material()
		+ evaluate.eval_state();

	// только материал
	//value_stat = (bitboard.value_white_material - bitboard.value_black_material);


	//------------------------------------------------
	// TEST
	// проверяем на правильность пошаговую оценку материала

	//if (EVALUATE::eval_white_material(bitboard) != bitboard.value_white_material)
	//{
	//	std::cout << "WHITE eval_white_material != bitboard.value_white_material" <<"\n";
	//	std::cout << "eval_white_material(bitboard)= "<< EVALUATE::eval_white_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_white_material= "<< bitboard.value_white_material<<"\n";
	//}

	//if (EVALUATE::eval_black_material(bitboard) != bitboard.value_black_material)
	//{
	//	std::cout << "WHITE eval_black_material != bitboard.value_black_material" <<"\n";
	//	std::cout << "eval_black_material(bitboard)= "<< EVALUATE::eval_black_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_black_material= "<< bitboard.value_black_material<<"\n";
	//}

	//if (EVALUATE::eval_difference_material(bitboard) 
	//	!= (bitboard.value_white_material - bitboard.value_black_material) )
	//{
	//	std::cout << "WHITE eval_difference_material != (bitboard.value_white_material - bitboard.value_black_material)" <<"\n";
	//	std::cout << "eval_difference_material(bitboard)= "<< EVALUATE::eval_difference_material(bitboard)<<"\n";
	//	std::cout << "(bitboard.value_white_material - bitboard.value_black_material)= " << (bitboard.value_white_material - bitboard.value_black_material)<<"\n";
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
	bitboard.delta_pruning_remember_piese_white();

	if (value_stat < alpha - PAWN_MARGIN )
	{
		bitboard.delta_pruning_white(alpha,value_stat);
	}
#endif //#if DELTA_PRUNING


	//насчитываем список возможных взятий и сортируем его
	list_surplus_moves.generator_captures_white(bitboard);
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);
	list_surplus_moves.sorting_moves();
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);

#if DELTA_PRUNING
	// возвращаем на доску фигуры
    bitboard.delta_pruning_return_piese_white();
#endif //#if DELTA_PRUNING

    //
	value_max_din = value_stat;

	// бежим по списку возможных ходов
	// режим с альфа-бета отсечением
	for (unsigned __int8 i = 0; i < list_surplus_moves.get_end_captures(); i++)
	{
		//TEST
		//cout << "white depth= " << depth<<"\n";
		//cout << "i = " << i <<"\n";
		//cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

		// реализуем ход
		bitboard.do_moves_white(i,depth,list_surplus_moves);

		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		// если короля не съели
		if ((list_surplus_moves.get_move(i) & 63) != 6)
		{
			// рекурсивный вызов функции
			value_din = forced_variant_black(alpha,beta,new_depth
				,flag_pv_f);

			// восстанавливаем позицию
			bitboard.undo_moves_white(i,depth,list_surplus_moves);

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
						pv_save.update_PV_best_point(pv_best_point);
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

			bitboard.undo_moves_white(i,depth,list_surplus_moves);

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				pv_save.update_PV_best_max_depth(value_stat,depth);
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
				pv_save.update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// теперь мы местный лучший вариант скопируем в структуру 
			// PV_best что бы передать на более высокий уровень
			if (flag_pv_f)
			{
				pv_save.update_PV_best(pv_best_point);
			}
		}
	}
	return value_max_din;
}

//	===================================================================
// просчитываем только взятия для черных
__int32 Search_quiescence::forced_variant_black
(
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

	class List_surplus_moves & list_surplus_moves 
		= quiescence_list_surplus_moves[depth];// список возможных ходов (всевозможные ходы из данного узла)

	struct PV_line pv_best_point;// лучший для данного узла вариант

	value_stat = bitboard.get_value_white_material() 
		- bitboard.get_value_black_material()
		+ evaluate.eval_state();
 
	// только материал
	//value_stat = bitboard.value_white_material - bitboard.value_black_material;

	//------------------------------------------------
	// TEST
	// проверяем на правильность пошаговую оценку материала

	//if (EVALUATE::eval_white_material(bitboard) != bitboard.value_white_material)
	//{
	//	std::cout << "BLACK eval_white_material != bitboard.value_white_material" <<"\n";
	//	std::cout << "eval_white_material(bitboard)= "<< EVALUATE::eval_white_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_white_material= "<< bitboard.value_white_material<<"\n";
	//}

	//if (EVALUATE::eval_black_material(bitboard) != bitboard.value_black_material)
	//{
	//	std::cout << "BLACK value_stat != bitboard.value_black_material" <<"\n";
	//	std::cout << "eval_black_material(bitboard)= "<< EVALUATE::eval_black_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_black_material= "<< bitboard.value_black_material<<"\n";
	//}

	//if (EVALUATE::eval_difference_material(bitboard) 
	//	!= (bitboard.value_white_material - bitboard.value_black_material) )
	//{
	//	std::cout << "BLACK eval_difference_material != (bitboard.value_white_material - bitboard.value_black_material)" <<"\n";
	//	std::cout << "eval_difference_material(bitboard)= "<< EVALUATE::eval_difference_material(bitboard)<<"\n";
	//	std::cout << "(bitboard.value_white_material - bitboard.value_black_material)= " << (bitboard.value_white_material - bitboard.value_black_material)<<"\n";
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
	bitboard.delta_pruning_remember_piese_black();


	if (value_stat > beta + PAWN_MARGIN)
	{
		bitboard.delta_pruning_black(beta,value_stat);
	}
#endif //#if DELTA_PRUNING


	//насчитываем список возможных взятий
	list_surplus_moves.generator_captures_black(bitboard);
	list_surplus_moves.sorting_moves();

#if DELTA_PRUNING
	// возвращаем на доску фигуры
	bitboard.delta_pruning_return_piese_black();
#endif //#if DELTA_PRUNING


	//
	value_min_din = value_stat;

	//если до сих пор отсечек не было значит статическая оценка не катит

	//бежим по списку возможных ходов
	// режим с альфа бета отсечением
	for (unsigned __int8 i = 0; i < list_surplus_moves.get_end_captures(); i++)
	{
		//TEST
		//cout << "black depth= " << depth<<"\n";
		//cout << "i = " << i <<"\n";
		//cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

		// реализуем ход
		bitboard.do_moves_black(i,depth,list_surplus_moves);

		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		// если короля не съели
		if ((list_surplus_moves.get_move(i) & 63) != 6)
		{
			// рекурсивный вызов функции
			value_din = forced_variant_white(alpha,beta,new_depth
				,flag_pv_f);

			// восстанавливаем позицию
			bitboard.undo_moves_black(i,depth,list_surplus_moves);

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
						pv_save.update_PV_best_point(pv_best_point);
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
			bitboard.undo_moves_black(i,depth,list_surplus_moves);

			// здесь мы обновили лучший выходной вариант особенностью является то что мы 
			// в лучший вариант сразу записали оценку
			if (flag_pv_f)
			{
				pv_save.update_PV_best_max_depth(value_stat,depth);
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
				pv_save.update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// теперь мы местный лучший вариант скопируем в структуру PV_best 
			// что бы передать на более высокий уровень
			if (flag_pv_f)
			{
				pv_save.update_PV_best(pv_best_point);
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
	const class List_surplus_moves & list_surplus_moves//список ходов
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
