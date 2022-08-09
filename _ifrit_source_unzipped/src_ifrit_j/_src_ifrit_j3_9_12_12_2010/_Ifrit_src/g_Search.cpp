#include <iostream> // TEST
#include <fstream>  // TEST
//#include <cstring>    // используем для работы со строкой (сравнение строк)
#include <stdlib.h>   // обеспечиваем завершение потока функцией exit(1)


#include "g_Search_quiescence.h"
#include "k_Util.h"
#include "h_Evaluate.h"
#include "k_Win_32.h"
#include "g_Search.h"
#include "j_Hash.h"
#include "j_Killer.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "j_ZObrist.h"
#include "g_Search_switch.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009 25.07.2010 17.10.2010
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%

 перебор позиций

 так работает генератор позиций:
 1 в начале генерируются список избыточных ходов. избыточных потому что нет проверки на шах
   и в них есть такие ходы как ход короля под шах
 2 дальше мы сортируем этот список выводя взятия на верх и ход из хеш-таблицы если он есть и киллер
 3 дальше мы реализуем ход и проверяем позицию на шах если 
   под боем наш король то ход откатываем иначе идем дальше
 4 итерационное углубление
 5 когда вернулись с глубины откатываем ход
 6 если ходов не было то это случай мата или пата

*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
   ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

extern class Evaluate evaluate;
class Search_quiescence search_quiescence;
extern class Hash hash;
extern class ZObrist zobrist;
class Timer timer;
extern class PV_save pv_save;
extern class Killer killer;

//	===================================================================
// ищем максимум в узле. перебор за белых 
__int32 Search::white
(
	class Chess_bitboard * const p_bitboard,// представление доски
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m// флаг нулевого хода
)
{
	__int32 value;              // текущая оценка позиции
	__int32 value_max = -9999999;   // оценка позиции лучшая в узле 
	struct Undo undo[1]; // использую при возврате хода
	class List_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	bool flag_is_legal_moves = 0;           // флаг существования легальных ходов
	bool flag_save_best = 0;           // пишем ли лучший вариант
	const unsigned __int8 new_depth = depth + 1; // следующая глубина
	bool flag_check = 0;//флаг шаха

//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//новая глубина перебора
 
	struct PV_line pv_best_point;// лучший для данного узла вариант

	  
	// все для кеширования
	const unsigned __int64 key_undo = p_bitboard->get_hash_key();   // запоминаем хеш-ключ даннного узла
	unsigned __int8  flag_insert_hash_move = 0; // поместили ли ход из хеш-таблицы на первую позицию
	__int32 hash_score = -1000000000;// хеш-оценка позиции
	unsigned __int8 i_hash_moves_1;// ход который улучшает альфу (1- улучшили альфу) нужно для кеширования 
	unsigned __int8 i_hash_moves_3;// ход дающий локальный максимум (3-просто локальный максимум )нужно для кеширования

#if (LMR && SAVE_LINE)
	unsigned __int8 lmr_reduction;//
#endif//#if (LMR

	//unsigned __int64 test_key_control;
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 
//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

#if HASH_TEST_SCORE
	unsigned __int8 hash_moves_test13;// для теста
#endif// #if HASH_TEST_SCORE

#if (HASH_SCORE && SAVE_LINE)
	unsigned __int8 test_flag_hash_moves;// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 hash_depth;// максимальная глубина
	unsigned __int8 hash_depth_max;// максимальная глубина
#endif//#if (HASH_SCORE)


	// 1
	// отсечка по команде на остановку счета

#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (flag_stop_search == 1)
	{
		return 0;
	}
#endif//#if SAVE_LINE 

	// 2
	// отсечка по максимальной глубине (оценка позиции)

	// на предельной глубине производим статическую оценку позиции
	if (depth >= depth_max)
	{

#if SAVE_LINE
		value = max_depth_white(p_bitboard,alpha,beta,flag_pv_f,depth);
#endif//#if SAVE_LINE 

		return value ;
	}

	// 3
	// отсечка по повторению позиции

#if (REPETITION && ALPHA_BETA_ON && SAVE_LINE)
	// обнаружили повторение позиции
	if (zobrist.analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(0 // оценка ничья
													,depth);
		}

		return 0;
	}
#endif//#if REPETITION

	// 4
	// отсечка по хеш-оценке

#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// отсечку по хеш оценке не используем для главного варианта
	// главным образом для достоверности оценки и линии варианта и для полноты картины
	// считаю обрывочный главный вариант неприемлемым.
	if (!flag_pv_f)
	{ 
		// смотрим не встречалась ли нам такая позиция если встречалась то 
		// пытаемся применить сохраненную оценку
		hash_score = hash.look_hash_score_white(
			p_bitboard->get_hash_key(),depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (hash_score != -1000000000 )
		{
			//return_hash_score = return_hash_score + 1 ;
#if (!HASH_TEST_SCORE)
			return hash_score;
#endif//#if (!HASH_TEST_SCORE)

		}
	}

#endif//#if HASH
#endif//#if (HASH_SCORE)

	// 5
	// отсечка
#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY)

/*
со стороны белых мы отсекаемся только по beta
принципиально может быть две ситуации
1. 
value = value_m - FUTILITY_MARGIN 
if (value > beta) return value;

здесь оценка для белых так хороша что можно дальше не считать
сейчас наш ход и при таком перевесе мы должны удержать.

2.
value = value_m + FUTILITY_MARGIN 
if (value < beta) return value;

статическая оценка позиции для белых настолько плоха что даже бонус не спасает
такая картина возникает если черные отыграли материал. тогда все что нам нужно
это посмотреть можем ли мы вернуть материал или нет.
в любом случае в такого рода позициях видимо можно ограничиться статическим поиском.
однако что-то тут не работает.


*/

	//futility
	if (!flag_pv_f)
	{ 
		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (description_move != MOVE_IS_CAPTURE) // если ход не взятие
		//&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
		&& (old_flag_check == CHECK_NO)) // если шаха нет
		{
			
			value = p_bitboard->get_value_white_material() 
				- p_bitboard->get_value_black_material()
				- futility_margin[(depth_max - depth)];

			if (value >= beta)
			{
				value = p_bitboard->get_value_white_material() - p_bitboard->get_value_black_material()
					+ evaluate.eval_state(p_bitboard);

				if (value >= ( beta + futility_margin[(depth_max - depth)]) )  
				{
					return value;  
				}
			}


			//-------------------
			//razoring
			// в таком виде не годится
			// можно будет вернуться когда сделаю шахи в форсированном поиске

			//value = p_bitboard->get_value_white_material() 
			//	- p_bitboard->get_value_black_material()
			//	+ futility_margin[(depth_max - depth)];

			//if (value < beta)
			//{
			//	value = search_quiescence.forced_variant_white(p_bitboard
			//				,alpha,beta,depth,flag_pv_f);

			//	if ( value < ( beta - futility_margin[(depth_max - depth)]) )
			//	{
			//		return value; 
			//	}
			//}
			//------------------------
		}
	}

#endif//#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY)

	// 6
	// отсечка по нулевому ходу

#if (NULL_MOVE && SAVE_LINE)
	// очень эффективная эвристика известная под именем нулевого хода
	// я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
	// суть его в том что
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	value = null_move_white(p_bitboard,alpha,beta,depth_max,depth,description_move,
		flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE




	// насчитываем список избыточных ходов
	// множество этих списков и создают дерево перебора
	list_surplus_moves.generator_captures_white(p_bitboard);
	list_surplus_moves.generator_move_white(p_bitboard);
	list_surplus_moves.sorting_moves();

 
#if (KILLER && SAVE_LINE)
	// если нет взятий или взятие плохое
	// то киллер вставляем на первую позицию иначе на вторую
	if ((list_surplus_moves.get_end_captures() == CAPTURE_NO) 
	|| (list_surplus_moves.get_sorting_score(0) < 0))
	{
		killer.insert_killer(&list_surplus_moves,depth,0);
	}
	else
	{
		killer.insert_killer(&list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// точнее мы поднимаем ход из хеш таблицы на первое место сдвигая остальные вниз
	hash.look_hash_move(&list_surplus_moves,p_bitboard->get_hash_key(),depth,flag_insert_hash_move);

// для полного теста нужно отключать отсечку по оценке
#if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

	if (flag_insert_hash_move == 1)
	{
		std::cout << "коллизия хеша белые" << "\n";
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH


#if (LMR && SAVE_LINE)
	// ход из хеш-таблицы, лучшее взятие, два киллера, все взятия не редуцируются
	lmr_reduction = FULL_SEARCH + list_surplus_moves.get_end_captures() ; // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


	//бежим по списку избыточных ходов
	// старт и конец цикла должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.get_end_list(); i++)
	{
		// реализуем ход из списка другими словами генерируем позицию
		p_bitboard->onward_moves_white(i,undo,&list_surplus_moves);

		// если король под шахом то 
		if (list_surplus_moves.king_white_check(p_bitboard) != CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			p_bitboard->backs_moves_white(i,undo,&list_surplus_moves);

            // идем дальше по списку 
			continue;
		}


		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = 1;
		nodes = nodes + 1;

#if SAVE_LINE
		// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,&list_surplus_moves);
		}

		zobrist.modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = p_bitboard->get_hash_key();

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		zobrist.start_position_random_key(p_bitboard);

		if (test_key_control != p_bitboard->get_hash_key())
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision white depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		zobrist.save_hash_three(p_bitboard->get_hash_key(),new_depth); 

		// каждый раз сбрасываем новую максимальную глубину что бы продлевать ветку 
		// а не весь куст
		depth_max_new = depth_max;
#endif//#if SAVE_LINE 

#if (EXTENSION_CHEK && SAVE_LINE)
		// если король под шахом то увеличиваем на 1 максимальную глубину перебора
		extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK


#if !SAVE_LINE 
		// рекурсивный вызов функции
		value = public_black(p_bitboard,alpha,beta,depth_max,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
			flag_check,flag_null_m);
#endif//#if !SAVE_LINE


#if (!LMR && SAVE_LINE)
		PVS_white(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check);
#endif//#if !LMR


#if (LMR && SAVE_LINE)
	if (flag_pv_f)
	{
		// если мы в главном варианте то считаем по методике с нулевым окном
		// т.е. от alpha до beta = alpha + 1

		if (i == 0) 
		{
			// первый ход смотрим на полной глубине и с полным окном
			// мы предполагаем что если он первый то лучший и достоин полного просмотра
			value = black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			// пытаемся обойтись перебором с нулевым окном
			value = black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			// суть поиска с нулевым окном в том что мы не ждем что вариант улучшит 
			// альфу поэтому считаем его с максимально узким т.е. нулевым окном
			//  если нас ждет сюрприз то пересчитываем c полным окном
			if (value > alpha)
			{
				//
				value = black(p_bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
					PV_YES,flag_check,flag_null_m);
			}
		}

	}
	else//if(flag_pv_f)
	{
		// если мы не в главном варианте то считаем по методике LMR

		// после сортировки первый ход из хеш таблицы второй это 
		// лучшее взятие третий четвертый это киллеры дальше идут 
		// просто взятия и дальше все остальные ходы
		// все это учитывается при задании lmr_reduction
		// мы редуцируем только остальные(тихие) ходы если они не шах 
		// на этой и предыдущей глубине
		if ((i > lmr_reduction) 
		&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
		&& ((depth_max_new - depth) >= LMR_DEPTH ))
		{
			// ищем с редукцией глубины и с нулевым окном
			value = black(p_bitboard,alpha,(alpha + 1),(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}
		else
		{
			// это просто формальный прием что бы выполнилось условие value > alpha
			// для случая когда ход не редуцируем по лмр(lmr)
			value = alpha + 1; 
		}

		// условия персчета многократно менялись
		// в начале было if((value > alpha) && (value < beta)) это грубейшая ошибка потому что
		// вариант не пересчитывался в случае value >= beta и получалось недостоверное отсечение
		// потом было value >= beta но это только вносит неясность хотя работает правильно
		// сейчас у нас value > alpha что очень наглядно.

		if (value > alpha)
		{
			//
			value = black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}

	}
#endif//#if  LMR

		// восстанавливаем позицию
		p_bitboard->backs_moves_white(i,undo,&list_surplus_moves);

#if SAVE_LINE
		// восстанавливаем хеш-ключ доски
		p_bitboard->set_hash_key(key_undo);

		//--------------------------------------------
		if (value > value_max)
		{
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if ALPHA_BETA_ON
			if (value > alpha)
			{
					
				if (value >= beta)
				{        
#if KILLER
					// пишем киллера если ход не взятие
					if (((list_surplus_moves.get_move(i)>>6)& 63) != MOVE_IS_CAPTURE)
					{
						killer.save_killer(i,&list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// кэшируем лучший на данный момент ход
					hash.save_best_hash_score_or_move(i
						, p_bitboard->get_hash_key(), &list_surplus_moves, 
						value, LOWER, depth, depth_max);
#endif//#if HASH

					// TEST
					//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 ))
					//{
						//std::cout << "отсечение по хеш таблице белые " << "\n";
					//}

					return value; 
				}

// TEST------------
#if HASH_TEST_SCORE
				// конечно что бы этот тест работал нельзя выше отсекаться 
				//по хеш оценке :)
				// если мы должны отсекаться но тем не менее ход не вызвал 
				//отсечку то пишем об этом
				// подробнее за белых
				// суть в том что мы взяли оценку из хеш-таблицы и увидели что 
				//она больше беты т.е. вызовет
				// отсечение. теперь мы просчитываем ее и если оценка 
				//подтверждается то до этого места
				// счет не дойдет так как отсечется раньше
				if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "осечка отсечения по хеш таблице белые " << "\n";
					std::cout << "depth= " << depth << "\n";
					std::cout << "depth_max_rem= " << depth_max_rem << "\n";
					std::cout << "hash_depth= " << (__int32)hash_depth << "\n";
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
					std::cout << "hash_score= " << hash_score  << "\n";
					std::cout << "value= " << value << "\n";
					std::cout << "alpha= " << alpha << "\n";
					std::cout << "beta= " << beta << "\n";
					std::cout << "  " << "\n";

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-----------------


				alpha = value;
				flag_save_best = 1;
				i_hash_moves_1 = i;// ход который улучшает альфу (1- улучшили альфу) нужно для кеширования 

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				if (flag_pv_f)
				{
					pv_save.update_PV_best_point(&pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_point(&pv_best_point);
			}

			flag_save_best = 1;
			i_hash_moves_1 = i;// при тестировании полного перебора
#endif// #if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_max = value;
			i_hash_moves_3 = i;// ход дающий локальный максимум (3-просто локальный максимум)нужно для кеширования

		}
#endif//#if SAVE_LINE 

	}


	// обработка случая когда не было легальных ходов
	// если не одного хода сделано не было то производим статическую оценку позиции
	// это случай пата или мата
	if (flag_is_legal_moves == 0)
	{
#if SAVE_LINE
		value = 0;
		if (list_surplus_moves.king_white_check(p_bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == 1)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
		if (flag_pv_f)
		{
			pv_save.update_PV_best(&pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)
	if (flag_save_best == 1)
	{
		// кэшируем ход улучшающий альфу т.е. флаг 1
		hash.save_best_hash_score_or_move(i_hash_moves_1
			, p_bitboard->get_hash_key(), &list_surplus_moves, 
			alpha, EXACT, depth, depth_max);
	}
	else
	{
		// кэшируем ход дающий локальный максимум т.е. флаг 3
		hash.save_best_hash_score_or_move(i_hash_moves_3
			, p_bitboard->get_hash_key(), &list_surplus_moves, 
			value_max, UPPER, depth, depth_max);
	}
#endif//#if HASH

	// это тест показал что есть позиции где расходятся ходы и оценки
	// и даже если а б одинаковы и длина поиска та же и без нулевого окна
	// все равно ходы как и оценки могут быть разными
	// все это для случая оценки меньше альфы
// TEST------------
#if HASH_TEST_SCORE
	// конечно что бы этот тест работал нельзя выше отсекаться по хеш оценке :)
	/*
	взятая из хеш-таблицы оценка меньше альфы
	если в процессе поиска она вдруг стала больше альфы то мы об этом пишем
	*/
	if (hash_score != -1000000000)
	//if ( 0 )
	{
		hash_moves_test13 = i_hash_moves_3;
		if (flag_save_best == 1) hash_moves_test13 = i_hash_moves_1;

		//if ( hash_moves_test13 != 0 )
		//if ( hash_score != value_max )
		if (value_max > alpha) 
		{
			std::cout << "хешированный и найденный ходы дающие максимум разные белые " << "\n";
			std::cout << "p_bitboard->get_hash_key()= " << p_bitboard->get_hash_key() << "\n";
			std::cout << "depth= " << depth << "\n";
			std::cout << "depth_max_rem= " << depth_max_rem << "\n";
			std::cout << "hash_depth= " << (__int32)hash_depth << "\n";
			std::cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
			std::cout << "test_flag_hash_moves= " << test_flag_hash_moves  << "\n";
			std::cout << "hash_score= " << hash_score  << "\n";
			std::cout << "value_max= " << value_max << "\n";
			std::cout << "alpha= " << alpha << "\n";
			std::cout << "beta= " << beta << "\n";

			std::cout << "ход из хеш таблицы " << "\n";
			std::cout << "((list_surplus_moves.move[0]>>6)& 63)= " 
				<< ((list_surplus_moves.get_move(0)>>6)& 63) << "\n";

			std::cout << "Ходящая фигура " 
				<< UTIL::public_int_to_char_board(((list_surplus_moves.get_move(0)>>24)& 7),1)
				<< "\n";
               
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.get_move(0)>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(0)>>12)& 63)/8);
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.get_move(0)>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(0)>>18)& 63)/8) << "\n";

			std::cout << "ход найденный в поиске " << "\n";
			std::cout << "hash_moves_test12= " << hash_moves_test13  << "\n";
			std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				<< ((list_surplus_moves.get_move(hash_moves_test13)>>6)& 63) << "\n";
			std::cout << "Ходящая фигура " 
				<< UTIL::public_int_to_char_board(((
					list_surplus_moves.get_move(hash_moves_test13)>>24)& 7),1)<< "\n";
               
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.get_move(hash_moves_test13)>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(hash_moves_test13)>>12)& 63)/8);
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.get_move(hash_moves_test13)>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(hash_moves_test13)>>18)& 63)/8) << "\n";
			std::cout << "  " << "\n"; 

			exit(1);
		}
	}
#endif //#if HASH_TEST_SCORE
//-----------------

	return value_max;
}

//	===================================================================
// ищем минимум в узле. перебор за черных
__int32 Search::black
(
	class Chess_bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m// флаг нулевого хода
)
{
	__int32 value;              // текущая оценка позиции
	__int32 value_min = 9999999;   // оценка позиции лучшая в узле 
	struct Undo undo[1];
	class List_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	bool flag_is_legal_moves = 0;           // флаг существования легальных ходов
	bool flag_save_best = 0;           // пишем ли лучший вариант
	const unsigned __int8 new_depth = depth + 1; // следующая глубина
	bool flag_check = 0;//флаг шаха


#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//новая глубина перебора
 
	struct PV_line pv_best_point;// лучший для данного узла вариант
	bool taking_passage_yes_mem = 0;
	unsigned __int8 taking_passage_y_mem = 0;
	unsigned __int8 taking_passage_x_mem = 0;

	unsigned __int8 flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
	const unsigned __int64 key_undo = p_bitboard->get_hash_key();   // запоминаем хеш-ключ даннного узла
	__int32 hash_score = -1000000000;// хеш-оценка позиции
	unsigned __int8 i_hash_moves_1;// ход который улучшает бету (1- улучшили бету) нужно для кеширования 
	unsigned __int8 i_hash_moves_3;// ход дающий локальный минимум (3-просто локальный минимум)нужно для кеширования

#if (LMR && SAVE_LINE)
	unsigned __int8 lmr_reduction;//
#endif//#if (LMR

	//unsigned __int64 test_key_control;// запоминаем хеш-ключ даннного узла
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 

#if (HASH_SCORE && SAVE_LINE)
	unsigned __int8 test_flag_hash_moves;// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 hash_depth;// максимальная глубина
	unsigned __int8 hash_depth_max;// максимальная глубина
#endif//#if (HASH_SCORE)


#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (flag_stop_search == 1)
	{
		return 0;
	}
#endif//#if SAVE_LINE 

	// на предельной глубине производим статическую оценку позиции
	if (depth >= depth_max)
	{
#if SAVE_LINE
		value = max_depth_black(p_bitboard,alpha,beta,flag_pv_f,depth);
#endif//#if SAVE_LINE 

		return value ;
	}

#if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
	// обнаружили повторение позиции
	if (zobrist.analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(0 // оценка ничья
														,depth);
		}

		return 0;
	}
#endif//#if ALPHA_BETA_ON

#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// отсечку по хеш оценке не используем для главного варианта
	// главным образом для достоверности оценки и линии варианта и для полноты картины
	// считаю обрывочный главный вариант неприемлемым.
	if (!flag_pv_f)
	{ 
		// смотрим не встречалась ли нам такая позиция если встречалась то 
		// пытаемся применить сохраненную оценку
		hash_score = hash.look_hash_score_black(
			p_bitboard->get_hash_key(),depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (hash_score != -1000000000 ) 
		{
			//return_hash_score = return_hash_score + 1 ;
#if (!HASH_TEST_SCORE)
			return hash_score;
#endif//#if (!HASH_TEST_SCORE)
		}
	}
#endif//#if HASH
#endif//#if (HASH_SCORE)

#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY)  

	/*
	Вот что пишет по этому поводу первоисточник:
	Ernst A. Heinz
	Given a suitable value for max posn score, 
	it is therefore safe to simply cut off 
	all non-checking moves at frontier nodes 
	for which the futility condition 
	mat balance(node) + mat gain(move) + max posn score  <= alpha(node) holds. 

	*/

	//futility
	if (!flag_pv_f)
	{ 
		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (description_move != MOVE_IS_CAPTURE) // если ход не взятие
		//&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
		&& (old_flag_check == CHECK_NO)) // если шаха нет
		{
			
			value = p_bitboard->get_value_white_material() 
				- p_bitboard->get_value_black_material()
				+ futility_margin[(depth_max - depth)];

			if (value <= alpha)
			{
				value = p_bitboard->get_value_white_material() - p_bitboard->get_value_black_material()
					+ evaluate.eval_state(p_bitboard);

				if (value <= ( alpha - futility_margin[(depth_max - depth)]) )  
				{
					return value;
				}
			}

			//------------------------
			//razoring 
			// в таком виде не годится
			// можно будет вернуться когда сделаю шахи в форсированном поиске

			//value = p_bitboard->get_value_white_material() 
			//	- p_bitboard->get_value_black_material()
			//	- futility_margin[(depth_max - depth)];

			//if (value > alpha)
			//{
			//	value = search_quiescence.forced_variant_black(p_bitboard
			//				,alpha,beta,depth,flag_pv_f);

			//	if (value > ( alpha + futility_margin[(depth_max - depth)]) )
			//	{
			//		return value; 
			//	}
			//}
			//------------------------
		}
	}

#endif// (SAVE_LINE && ALPHA_BETA_ON && FUTILITY) 


#if (NULL_MOVE && SAVE_LINE)
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	value = null_move_black(p_bitboard,alpha,beta,depth_max,depth,description_move,
		flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE


	// насчитываем список избыточных ходов
	// множество этих списков и создают дерево перебора
	list_surplus_moves.generator_captures_black(p_bitboard);
	list_surplus_moves.generator_move_black(p_bitboard);
	list_surplus_moves.sorting_moves();


#if (KILLER && SAVE_LINE)
	// если нет взятий или взятие проигрывающее
	// то киллер вставляем на первую позицию иначе на вторую
	if ((list_surplus_moves.get_end_captures() == CAPTURE_NO)
	|| (list_surplus_moves.get_sorting_score(0) < 0))
	{
		killer.insert_killer(&list_surplus_moves,depth,0);
	}
	else
	{
		killer.insert_killer(&list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	hash.look_hash_move(&list_surplus_moves
		,p_bitboard->get_hash_key(),depth,flag_insert_hash_move);

// для полного теста нужно отключать отсечку по оценке
#if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

	if (flag_insert_hash_move == 1)
	{
		std::cout << "коллизия хеша черные" << "\n";
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH


#if (LMR && SAVE_LINE)
	// ход из хеш-таблицы, лучшее взятие, два киллера, все взятия не редуцируются
	lmr_reduction = FULL_SEARCH + list_surplus_moves.get_end_captures(); // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


	// бежим по списку избыточных ходов
	// старт и конец цикла; должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.get_end_list(); i++)
	{
		// реализуем ход из списка другими словами генерируем позицию
		p_bitboard->onward_moves_black(i,undo,&list_surplus_moves);

		// если король по шахом то
		if (list_surplus_moves.king_black_check(p_bitboard)!= CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			p_bitboard->backs_moves_black(i,undo,&list_surplus_moves);

			// идем дальше по списку 
			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = 1;
		nodes = nodes + 1;

#if SAVE_LINE
		// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,&list_surplus_moves);
		}

		zobrist.modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = p_bitboard->get_hash_key();

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		zobrist.start_position_random_key(p_bitboard);

		if (test_key_control != p_bitboard->get_hash_key())
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision black depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		zobrist.save_hash_three(p_bitboard->get_hash_key(),new_depth); 

		depth_max_new = depth_max;
#endif//#if SAVE_LINE 


#if (EXTENSION_CHEK && SAVE_LINE)
		// если король под шахом то увеличиваем максимальную глубину перебора 
		extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK


#if !SAVE_LINE
		// рекурсивный вызов функции
		value = public_white(p_bitboard,alpha,beta,depth_max,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),PV_YES,flag_check,
			flag_null_m);
#endif//#if !SAVE_LINE


#if (!LMR && SAVE_LINE)
		PVS_black(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check);
#endif//#if (!LMR && SAVE_LINE)


#if (LMR && SAVE_LINE)
	if(flag_pv_f)
	{
		// если мы в главном варианте то считаем по методике с нулевым окном
		if (i == 0)
		{
			//
			value = white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			value = white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			if (value < beta)
			{
				value = white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6)& 63),
				PV_YES,flag_check,flag_null_m);
			}
		}
	}
	else
	{
		// если мы не в главном варианте то считаем по методике LMR
		if ((i > lmr_reduction)
		&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
		&& ((depth_max_new - depth) >= LMR_DEPTH )) 
		{
			value = white(p_bitboard,(beta - 1),beta,(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}
		else
		{
			value = beta - 1;
		}

		if (value < beta)
		{
			value = white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6)& 63),
				PV_NO,flag_check,flag_null_m);
		}
	}
#endif//#if  LMR

		// восстанавливаем позицию
		p_bitboard->backs_moves_black(i,undo,&list_surplus_moves);

#if SAVE_LINE
		// восстанавливаем хеш-ключ доски
		p_bitboard->set_hash_key(key_undo);

		//-----------------------------------------------
		if (value < value_min)
		{

//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if ALPHA_BETA_ON
			if (value < beta)
			{
				//-----------------------------------------------
				if (value <= alpha)
				{
#if KILLER
					if (((list_surplus_moves.get_move(i)>>6)& 63) != MOVE_IS_CAPTURE)
					{
						killer.save_killer(i,&list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// кэшируем лучший на данный момент ход
					hash.save_best_hash_score_or_move(i, p_bitboard->get_hash_key(), &list_surplus_moves, 
						value, LOWER, depth, depth_max);	
#endif//#if HASH

					return value; 
				}
				//-----------------------------------------------

// TEST------------
#if HASH_TEST_SCORE
				// конечно что бы этот тест работал нельзя выше отсекаться по хеш оценке :)
				// если мы должны отсекаться но тем не менее ход не вызвал отсечку то пишем об этом
				// подробнее за черных
				// суть в том что мы взяли оценку из хеш-таблицы и увидели что она меньше альфы т.е. вызовет
				// отсечение. теперь мы просчитываем ее и если оценка подтверждается то до этого места
				// счет не дойдет так как отсечется раньше
				if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "осечка отсечения по хеш таблице черные" << "\n";
					std::cout << "depth= " << depth << "\n";
					std::cout << "depth_max_rem= " << depth_max_rem << "\n";
					std::cout << "hash_depth= " << (__int32)hash_depth << "\n";
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max 
						<< "\n";

					std::cout << "hash_score= " << hash_score  << "\n";
					std::cout << "value= " << value << "\n";
					std::cout << "alpha= " << alpha << "\n";
					std::cout << "beta= " << beta << "\n";
					std::cout << "  " << "\n";

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-------------------

				beta = value;
				flag_save_best = 1;
				i_hash_moves_1 = i;// ход который улучшает бету (1- улучшили бету) нужно для кеширования 

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				if (flag_pv_f)
				{
					pv_save.update_PV_best_point(&pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_point(&pv_best_point);
			}

			flag_save_best = 1;
			i_hash_moves_1 = i;// при тестировании полного перебора
#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_min = value;
			i_hash_moves_3 = i;// ход дающий локальный минимум (3-просто локальный минимум)нужно для кеширования

		}
#endif//#if SAVE_LINE 

	}

	//-----------------------------------------------
	// обработка случая когда не было легальных ходов
	// если не одного хода сделано не было то производим статическую оценку позиции
	// это случай пата или мата
	if (flag_is_legal_moves == 0)
	{
#if SAVE_LINE
		value = 0;
		if (list_surplus_moves.king_black_check(p_bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == 1)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
		if (flag_pv_f)
		{
			pv_save.update_PV_best(&pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	if ( flag_save_best == 1 ) 
	{
		// кэшируем ход улучшающий бету т.е. флаг 1
		hash.save_best_hash_score_or_move(i_hash_moves_1, p_bitboard->get_hash_key(), &list_surplus_moves, 
			beta, EXACT, depth, depth_max);
	}
	else
	{
		// кэшируем ход дающий локальный минимум т.е. флаг 3
		hash.save_best_hash_score_or_move(i_hash_moves_3, p_bitboard->get_hash_key(), &list_surplus_moves, 
			value_min, UPPER, depth, depth_max);
	}
#endif//#if HASH

	return value_min;
}

//	===================================================================

// за белых
inline void Search::PVS_white
(
	class Chess_bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m,// флаг нулевого хода
    const unsigned __int8 new_depth,// следующая глубина
	const unsigned __int64 key_undo,
	const unsigned __int8 i,
	__int32 & value,// текущая оценка позиции
	const unsigned __int8 depth_max_new,
    class List_surplus_moves list_surplus_moves,
	const bool flag_check
)
{

#if (ALPHA_BETA_ON && SAVE_LINE)
	if (i == 0)
#else
	if (1)
#endif//#if ALPHA_BETA_ON
	{
		value = black(p_bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.get_move(i)>>6) & 63),
			flag_pv_f,flag_check,flag_null_m);
	}
	else
	{
		// рекурсивный вызов функции
		value = black(p_bitboard,alpha,(alpha + 1),depth_max_new,

		// TEST
		//value = Black(p_bitboard,alpha,beta,depth_max_new,

		new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
			PV_NO,flag_check,flag_null_m);

		// суть поиска с нулевым ходом в том что мы не ждем что вариант улучшит 
		// альфу поэтому считаем его
		// с максимально узким окном если нас ждет сюрприз 
		// то пересчитываем по полному
		if ((value > alpha) && (value < beta))
		{ 
			//std::cout << "white depth = " << depth << "\n";
			//std::cout << "value = " << value << "\n";
			//std::cout << "alpha = " << alpha << "\n";
			//std::cout << "beta = " << beta << "\n";

			value = black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				flag_pv_f,flag_check,flag_null_m);

			//std::cout << "value mod = " << value << "\n";
			//std::cout << "   " <<"\n";

		}
	}
}


//	===================================================================

//  за черных
inline void Search::PVS_black
(
	class Chess_bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m,// флаг нулевого хода
    const unsigned __int8 new_depth,// следующая глубина
	const unsigned __int64 key_undo,
	const unsigned __int8 i,
	__int32 & value,// текущая оценка позиции
	const unsigned __int8 depth_max_new,
    class List_surplus_moves list_surplus_moves,
	const bool flag_check
)
{

	// рекурсивный вызов функции	
	//depth_max_z
#if ALPHA_BETA_ON
	if (i == 0)
#else
	if (1)
#endif//#if ALPHA_BETA_ON
	{
		value = white(p_bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.get_move(i)>>6) & 63),flag_pv_f,
			flag_check,flag_null_m);

	}
	else
	{
		value = white(p_bitboard,(beta - 1),beta,depth_max_new,

		// TEST
		//value = White(p_bitboard,alpha,beta,depth_max_new,

			new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
			PV_NO,flag_check,flag_null_m);

		if ((value > alpha) && (value < beta))
		{
			value = white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6)& 63),
				flag_pv_f,flag_check,flag_null_m);

		}
	}
}

//	===================================================================
// очень эффективная эвристика известная под именем нулевого хода
// я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
// суть его в том что
// мы пропускаем свой ход и если даже при такой форе противник не находит
// лучшего хода эту ветку можно подрезать
// за белых
inline __int32 Search::null_move_white
(
	class Chess_bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m,// флаг нулевого хода
    const unsigned __int8 new_depth,// следующая глубина
	const unsigned __int64 key_undo
)
{

	if ((flag_stop_null_move == NULL_YES) // если цугцванг нам не грозит
	&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	&& (depth_max > NULL_REDUCTION ) // иначе уходим в минус а у нас глубина всегда положительна
	&& ((depth_max - depth) >= NULL_DEPTH ) // если до максимальной глубины расстояние достаточное
	&& (flag_pv_f == PV_NO) // если мы не в главном варианте
	&& (description_move != MOVE_IS_CAPTURE) // если ход не взятие
	&& (old_flag_check == CHECK_NO) // если шаха нет
	//&& (p_bitboard- >= beta) // дикая потеря эффективности. почему не понимаю
									// наверное дело в том что я не использую статической
									// составляющей оценки а только материал
	)
	{// тогда вызываем нулевой ход


		// запоминаем флаги взятия на проходе и обнуляем разрешение.
		bool taking_passage_yes_mem = p_bitboard->get_taking_passage_yes();
		unsigned __int8 taking_passage_x_mem = p_bitboard->get_taking_passage_x();
		unsigned __int8 taking_passage_y_mem = p_bitboard->get_taking_passage_y();

        unsigned __int64 hash_key_temp = p_bitboard->get_hash_key();

		// меняем хеш-ключ под нулевой ход -------------------------
		// учитываем взятие на проходе
		if (taking_passage_yes_mem)
		{
			hash_key_temp = hash_key_temp ^ U64(0xA3853B80F17F58EE);
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ U64(0x5B23E522E4B1B4EF));

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ U64(0x934E1DE72D36D310));
		}

		// учитываем цвет хода
		// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
		// некоторые позиции отличаются только цветом хода
		hash_key_temp = hash_key_temp ^ U64(0x7C0228DD624EC390);
		p_bitboard->set_hash_key(hash_key_temp);


		p_bitboard->set_taking_passage_yes(0);
		p_bitboard->set_taking_passage_x(0);
		p_bitboard->set_taking_passage_y(0);

		p_bitboard->set_colour_move(!p_bitboard->get_colour_move());

		const __int64 nodes_null_rem = nodes;

		// TEST тестировал изменение хеш-ключа
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//zobrist.start_position_random_key(p_bitboard);

		// не динамическая редукция
		__int32 value = black(p_bitboard,alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES); 

		//--------------------------------------------------------
		// динамическая редукция нулевого хода при дистанции до максимальной глубины
		// больше равной 5 редуцируем 4 иначе 3
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
		//
		//value = Black(p_bitboard,alpha,(alpha + 1),
		//	(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES); 
		//--------------------------------------------------------

		// узлы насчитанные в нулевом поиске не учитываем
		nodes = nodes_null_rem;

		// TEST
		//std::cout << "p_bitboard- "<< p_bitboard- << "\n";
		//std::cout << "i_move= "<< i_move << "\n";
		//std::cout << "description_move= "<< description_move << "\n";
		//std::cout << "check= "<< check << "\n";
		//std::cout << "       "<< "\n";
		//
		//if (value < beta)
		//{
		//	std::cout << "осталось "<< "\n";
		//	std::cout << "depth_max= "<< depth_max <<"\n";
		//	std::cout << "depth= "<< depth <<"\n";
		//	std::cout << "value= "<< value <<"\n";
		//	std::cout << "beta= "<< beta <<"\n";
		//	std::cout << "       "<< "\n";
		//}
		//--------------------------------------------------------

		p_bitboard->set_taking_passage_yes(taking_passage_yes_mem);
		p_bitboard->set_taking_passage_x(taking_passage_x_mem);
		p_bitboard->set_taking_passage_y(taking_passage_y_mem);

		p_bitboard->set_colour_move(!p_bitboard->get_colour_move());

		// восстанавливаем хеш-ключ доски
		p_bitboard->set_hash_key(key_undo);


		// был ход черных при котором они пытались найти минимальную оценку т.е. улучшали бету
		// белые пропустили свой ход дав таким образом черным фору
		// теперь опять ход черных и если мы не смогли улучшить бету т.е. наша оценка больше
		// или равна бете значит можно отсечь т.к. ветка нам не интересна
		// потому что если даже с такой форой не смогли улучшить то без форы и подавно не сможем 
		if (value >= beta)
		{

			//TEST
			//std::cout << "отсекли "<< "\n";
			//std::cout << "depth_max= "<< depth_max <<"\n";
			//std::cout << "depth= "<< depth <<"\n";
			//std::cout << "value= "<< value <<"\n";
			//std::cout << "beta= "<< beta <<"\n";
			//std::cout << "       "<< "\n";
			//return beta;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return beta;
			}

			// пробуем обойти цугцванг
			value = white(p_bitboard,(beta - 1),beta
				,(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
				depth,description_move, 
				PV_NO,old_flag_check,NULL_YES);

			if (value >= beta)
			{
				return beta;
			}
		}
	}

	return -1000000000;
}

//	===================================================================
// мы пропускаем свой ход и если даже при такой форе противник не находит
// лучшего хода эту ветку можно подрезать
// за черных
inline __int32 Search::null_move_black
(
	class Chess_bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m,// флаг нулевого хода
    const unsigned __int8 new_depth,// следующая глубина
	const unsigned __int64 key_undo
)
{

	if ((flag_stop_null_move == NULL_YES) // если цугцванг нам не грозит
	&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	&& (depth_max > NULL_REDUCTION ) // иначе уходим в минус а у нас глубина всегда положительна
	&& ((depth_max - depth) >= NULL_DEPTH ) // если до максимальной глубины расстояние достаточное
	&& (flag_pv_f == PV_NO) // если мы не в главном варианте
	&& (description_move != MOVE_IS_CAPTURE) // если ход не взятие
	&& (old_flag_check == CHECK_NO) // если шаха нет
	//&& (p_bitboard- <= alpha) //
	)
	{

		// тогда вызываем нулевой ход
		const bool taking_passage_yes_mem = p_bitboard->get_taking_passage_yes();
		const unsigned __int8 taking_passage_x_mem = p_bitboard->get_taking_passage_x();
		const unsigned __int8 taking_passage_y_mem = p_bitboard->get_taking_passage_y();


       unsigned __int64 hash_key_temp = p_bitboard->get_hash_key();

		// меняем хеш-ключ под нулевой ход -------------------------
		// учитываем взятие на проходе
		if ( taking_passage_yes_mem)
		{
			hash_key_temp = hash_key_temp ^ U64(0xA3853B80F17F58EE);
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ U64(0x5B23E522E4B1B4EF));

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ U64(0x934E1DE72D36D310));
		}

		// учитываем цвет хода
		// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
		// некоторые позиции отличаются только цветом хода
		hash_key_temp = hash_key_temp ^ U64(0x7C0228DD624EC390);

		p_bitboard->set_hash_key(hash_key_temp);

//--------

		p_bitboard->set_taking_passage_yes(0);
		p_bitboard->set_taking_passage_x(0);
		p_bitboard->set_taking_passage_y(0);

		p_bitboard->set_colour_move(!p_bitboard->get_colour_move());
	
		const __int64 nodes_null_rem = nodes;

		// TEST тестировал изменение хеш-ключа
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//zobrist.start_position_random_key(p_bitboard);

		__int32 value = white(p_bitboard,(beta - 1),beta,(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES);

		//--------------------------------------------------------
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 

		//value = White(p_bitboard,(beta - 1),beta
		//	,(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES);
		//--------------------------------------------------------

		nodes = nodes_null_rem;

		p_bitboard->set_taking_passage_yes(taking_passage_yes_mem);
		p_bitboard->set_taking_passage_x(taking_passage_x_mem);
		p_bitboard->set_taking_passage_y(taking_passage_y_mem);

		p_bitboard->set_colour_move(!p_bitboard->get_colour_move());

		// восстанавливаем хеш-ключ доски
		p_bitboard->set_hash_key(key_undo);


		//был ход белых при котором они пытались найти максимальную оценку т.е. улучшали альфу
		//черные пропустили ход
		//теперь опять ход белых и если мы не смогли улучшить альфу т.е. наша оценка меньше
		//или равна альфе значит можно отсечь т.к. ветка нам не интересна
		if (value <= alpha)
		{ 
			//std::cout << "отсекли "<< "\n";
			//std::cout << "depth_max= "<< depth_max <<"\n";
			//std::cout << "depth= "<< depth <<"\n";
			//std::cout << "value= "<< value <<"\n";
			//std::cout << "alpha= "<< alpha <<"\n";
			//std::cout << "       "<< "\n";

			//return alpha;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return alpha;
			}

			// пробуем обойти цугцванг
			value = black(p_bitboard,alpha,(alpha + 1)
				,(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
				depth,description_move, 
				PV_NO,old_flag_check,NULL_YES);

			if (value <= alpha)
			{
				return alpha;
			}
		}
	}

	return -1000000000;
}


//	===================================================================
// производим расчеты на максимальной глубине. за белых
inline __int32 Search::max_depth_white
(
	class Chess_bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// основной ли это вариант (pv)
	const unsigned __int8 depth// глубина
)
{
	__int32 score = 0;
	__int32 score_f = 0;
	double deep_time_current;

	//-----------------------------------------------
	// обработка команд экстренного завершения счета 
	if (audit > 50000)
	//if (0)
	{
		//если поступила команда то это или quit или stop
		if (public_bioskey())
		//if (0)
		{
			std::cin.getline(string_in2,LENGTH_STRING, '\n');

			if (strcmp(string_in2, "quit") == 0)
			{
				hash.hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// остановка поиска
				flag_stop_search = 1;
			}
		}

		//экстренный выход с глубины. просрочили время на поиск
		timer.time_stop();
		deep_time_current = timer.time_delta();

		//выходим по времени
		if ((deep_time_terminate > 0) && (deep_time_terminate < deep_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;
	//-----------------------------------------------

	//------------------------------------------------------
	// форсировка
	//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	// подключаем форсированный вариант
	// мы уже погрузились на полу ход так что глубина остаются те же
#if FORCED // включаем режим просчета только взятий
	score = search_quiescence.forced_variant_white(p_bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;
#endif//#if FORCED


	// включаем оценку без форсировки(просчета только взятий)
#if !FORCED 
	score = evaluate.eval_state(p_bitboard) + (p_bitboard->get_value_white_material() 
		- p_bitboard->get_value_black_material());
	//score = p_bitboard-;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(p_bitboard) != p_bitboard-)
	//{
	//	std::cout << "WHITE  Eval_forced(p_bitboard) != p_bitboard-" <<"\n";
	//	std::cout << "Eval_forced(p_bitboard)= "<< EVALUATE::Eval_forced(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard-= "<< p_bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
	//надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
	if (flag_pv_f)
	{
		PVSAVE::public_update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//именно здесь можно проверить правильно ли рисуется вариант на предельной глубине
	//кроме  случая шаха и пата
	//Print_std::cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score ;

#endif//#if !FORCED 

}

//	===================================================================
// производим расчеты на максимальной глубине. за черных
inline __int32 Search::max_depth_black
(
	class Chess_bitboard * const p_bitboard,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// основной ли это вариант (pv)
	const unsigned __int8 depth// глубина
)
{
	__int32 score = 0;
	__int32 score_f = 0;
	double deep_time_current;

	//-----------------------------------------------
	// обработка команд экстренного завершения счета 
	if (audit > 50000)
	//if (0)
	{
		//если поступила команда то это или quit или stop
		if (public_bioskey())
		//if (0)
		{
			//__int8 string_in2[65536];
			std::cin.getline(string_in2,LENGTH_STRING, '\n');

			if (strcmp(string_in2, "quit") == 0)
			{
				hash.hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// остановка поиска
				flag_stop_search = 1;
			}
		}

		// экстренный выход с глубины. просрочили время на поиск
		timer.time_stop();
		deep_time_current = timer.time_delta();

		// выходим по времени
		if ((deep_time_terminate  > 0) && (deep_time_terminate < deep_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;
	//-----------------------------------------------

	//------------------------------------------------------
	//  форсировка
	//  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	//  подключаем форсированный вариант
	//  мы уже погрузились на полу ход так что глубина остаются те же
#if FORCED // включаем режим форсировки(просчета только взятий)

	score = search_quiescence.forced_variant_black(p_bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;

#endif//#if FORCED 


	// включаем оценку без форсировки(просчета только взятий)
#if !FORCED
	score = evaluate.eval_state(p_bitboard) + (p_bitboard->get_value_white_material() 
		- p_bitboard->get_value_black_material());
	//score = p_bitboard-;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(p_bitboard) != p_bitboard-)
	//{
	//	std::cout << "BLACK  Eval_forced(p_bitboard) != p_bitboard-" <<"\n";
	//	std::cout << "Eval_forced(p_bitboard)= "<< EVALUATE::Eval_forced(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard-= "<< p_bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
	//надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
	if (flag_pv_f)
	{
		PVSAVE::public_update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//именно здесь можно проверить правильно ли рисуется вариант на предельной глубине
	//кроме  случая шаха и пата
	//Print_std::cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score;

#endif//#if !FORCED 

}

//	===================================================================
// реализуем продление на шахах -
inline void Search::extension_for_chek_white
(
	const class Chess_bitboard * const p_bitboard,// представление доски
	const unsigned __int8 depth_max,// максимальная глубина
	unsigned __int8 & depth_max_new,// новая максимальная глубина
	bool & flag_check// флаг шаха
)
{
	class List_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные

	// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
	if (list_surplus_moves.king_black_check(p_bitboard) != CHECK_NO)
	{
		flag_check = CHECK_YES;
		depth_max_new = depth_max + 1;
	}
	else
	{
		flag_check = CHECK_NO;
	}

}

//	===================================================================
// реализуем продление на шахах -
inline void Search::extension_for_chek_black
(
	const class Chess_bitboard * const p_bitboard,// представление доски
	const unsigned __int8 depth_max,// максимальная глубина
	unsigned __int8 & depth_max_new,// новая максимальная глубина
	bool & flag_check// флаг шаха
)
{

	class List_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные

	// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
	if(list_surplus_moves.king_white_check(p_bitboard) != CHECK_NO)
	{
		flag_check = CHECK_YES;
		depth_max_new = depth_max + 1;
	}
	else
	{
		flag_check = CHECK_NO;
	}

}


//	===================================================================
//
void Search::ini_global_loop()
{
	audit = 0;// задержка опроса команды экстренного выхода
	flag_stop_search = 0;//
}

//	===================================================================
// возвращаем флаг остановки поиска. так если в глубине поиска получили команду
// на остановку то передаем ее наружу
bool Search::get_stop_search()
{
	return flag_stop_search;
}

//	===================================================================
// возвращаем лучший вариант
struct PV_line * Search::get_pv_best()
{
	return pv_save.get_pv_best();
}

//	===================================================================
// возвращаем количество рассмотренных узлов
__int64  Search::get_nodes()
{
	return nodes;// количество узлов;
}

//	===================================================================
// обнуляем количество рассмотренных узлов
void Search::set_nodes_in_0()
{
	nodes = 0;// количество узлов;
}

//	===================================================================
//
void Search::ini_deep_time_terminate
(
	double deep_time_terminate_v//
)
{
	deep_time_terminate = deep_time_terminate_v;
}

//	===================================================================
//
void Search::ini_flag_stop_null_move
(
	bool flag_stop_null_move_v//
)
{
	flag_stop_null_move = flag_stop_null_move_v;
}

//	===================================================================
//
void Search::set_depth_max_rem
(
	unsigned __int8 depth_max_rem_v//
)
{
	depth_max_rem = depth_max_rem_v;
}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// максимальная глубина, глубина перебора, доска,
#if TEST_L

void FULLSEARCH::List_Print
(
	int depth,// текущая глубина
	class List_surplus_moves * p_list_surplus_moves// список ходов
)
{
	Test_Loop.open ("_TEST_LOOP.txt",ios::app);	
	//Test_Loop.open ("_TEST_LOOP.txt");	
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "------------------------------------------------- "<<"\n";
	Test_Loop <<"depth= "<< depth << "\n";
	Test_Loop << "\n";
	Test_Loop << "Список возможных ходов фигуры "<<"\n";

	for (int i = 0 ; i < p_list_surplus_moves->end_list ; i++)
	{	
		Test_Loop << "Номер хода по списку i= " << i << "\n";  
		Test_Loop << "Ходящая фигура " << util_o.public_int_to_char_board(((p_list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
		Test_Loop << "Оценка хода " << p_list_surplus_moves->sorting_score[i] << "\n";

		// вывод хода вида e2-e4
		//------------------------------
		Test_Loop << " " << (util_o.public_x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8));
		Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
		//Test_Loop<< "\n";
		Test_Loop << " " << (util_o.public_x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8));
		Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
		//------------------------------
		//тип хода 
		//0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка 4 – короткая рокировка 5 – взятие на проходе
		//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
		//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
		//25 - взятие с превращением в ферзь 
		Test_Loop << "вид хода " << ((p_list_surplus_moves->move[i]>>6)& 63)  << "\n";
		Test_Loop << "Взятая фигура " << util_o.public_int_to_char_board(( p_list_surplus_moves->move[i] & 63),1)<< "\n";
		Test_Loop << "начальная положение " << ((p_list_surplus_moves->move[i]>>12)& 63)  << "\n";
		Test_Loop << "конечное положение " << ((p_list_surplus_moves->move[i]>>18)& 63)  << "\n";
     
		Test_Loop << "\n";
	}

	Test_Loop << "p_list_surplus_moves->end_captures= " <<p_list_surplus_moves->end_captures << "\n";
	Test_Loop << "p_list_surplus_moves->end_list= " <<p_list_surplus_moves->end_list << "\n";
	Test_Loop << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop.close();
}
#endif