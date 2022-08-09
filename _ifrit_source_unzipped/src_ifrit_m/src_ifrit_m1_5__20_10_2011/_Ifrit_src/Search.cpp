
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009 25.07.2010 17.10.2010

#include <fstream>
#include <iostream>

#include "Search_switch.h"
#include "List.h"
#include "Search_constants.h"
#include "Search.h"
#include "Search_quiescence.h"
#include "Evaluate.h"
#include "Generator_moves.h"
#include "Make_move.h"
#include "Sorting_moves.h"
#include "Killer.h"
#include "History.h"
#include "ZObrist.h"
#include "Hash.h"
#include "Hash_constants.h"
#include "Timer.h"
#include "Win_32.h"
#include "PV_save.h"
#include "Generator_moves_const.h"
#include "List_const.h"

	unsigned __int16 audit;/// задержка опроса команды экстренного выхода
	unsigned __int8 depth_max_rem;///
	bool flag_stop_search;/// переменная для отсечки перебора при экстренном выходе
	__int64 nodes;/// количество узлов
	__int8 string_in2[65536];///
	unsigned __int8 from;/// откуда ходит фигура
	unsigned __int8 to;/// куда ходит фигура
	bool flag_stop_null_move;/// останавливаем использование нулевого хода
	//int return_hash_score;

	double deep_time_terminate; /// передаем на глубину время остановки счета 


//	===================================================================
// ищем максимум в узле. перебор за белых 
__int32 Search::white
(
	struct Position & position,// представление доски
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move_up,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m// флаг нулевого хода
)
{
	__int32 value;              // текущая оценка позиции
	__int32 value_max = -9999999;   // оценка позиции лучшая в узле 


	struct List list_surplus_moves; // список возможных ходов (всевозможные ходы из данного узла)

	bool flag_is_legal_moves = false;           // флаг существования легальных ходов
	bool flag_save_best = false;           // пишем ли лучший вариант
	const unsigned __int8 new_depth = depth + 1; // следующая глубина
	bool flag_check = false;//флаг шаха

	unsigned __int8 depth_max_new;//новая глубина перебора

	// переменные параметры в поиске NegaScout и LMR
		__int32 beta_scout;//
		unsigned __int8 depth_max_new_scout;// максимальная глубина
		bool flag_pv_f_scout;// основной ли это вариант (pv)
		bool search_scout;


//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
 
	struct PV_line pv_best_point;// лучший для данного узла вариант

	  
	// все для кеширования
	const unsigned __int64 key_undo = position.hash_key;   // запоминаем хеш-ключ даннного узла
	unsigned __int8  flag_insert_hash_move = 0; // поместили ли ход из хеш-таблицы на первую позицию
	__int32 hash_score = -1000000000;// хеш-оценка позиции
	unsigned __int8 i_hash_moves_1;// ход который улучшает альфу (1- улучшили альфу) нужно для кеширования 
	unsigned __int8 i_hash_moves_3;// ход дающий локальный максимум (3-просто локальный максимум )нужно для кеширования

//#if (LMR && SAVE_LINE)
//	unsigned __int8 lmr_reduction;//
//#endif//#if (LMR

	//unsigned __int64 test_key_control;
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 
//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

#if HASH_TEST_SCORE
	unsigned __int8 hash_moves_test13;// для теста
#endif// #if HASH_TEST_SCORE

#if (HASH_SCORE && SAVE_LINE)
	// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 
	// 2-отсечка по бете(альфе))
	unsigned __int8 test_flag_hash_moves;
	unsigned __int8 hash_depth;// максимальная глубина
	unsigned __int8 hash_depth_max;// максимальная глубина
#endif//#if (HASH_SCORE)

//----------------------------------------------------------
// 1 отсечка по команде на остановку счета

#if SAVE_LINE

	// это отсечка при экстренном выходе
	if (flag_stop_search == true) return 0;
	// 
	quit_or_stop();

#endif//#if SAVE_LINE 

//----------------------------------------------------------
// 2 отсечка по максимальной глубине (оценка позиции)

	// на предельной глубине производим статическую оценку позиции
	if (depth >= depth_max)
	{
#if SAVE_LINE

	// форсировка
	// подключаем форсированный вариант
#if FORCED
		// включаем режим просчета только взятий
		value = Search_quiescence::forced_variant_white(position,alpha,beta
													,depth,flag_pv_f);
#endif//#if FORCED

	// включаем оценку без форсировки
#if !FORCED 
		value = max_depth_white(position,alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

//----------------------------------------------------------
// 3 отсечка по повторению позиции

#if (REPETITION && ALPHA_BETA_ON && SAVE_LINE)
	// обнаружили повторение позиции
	if (ZObrist::analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка 
		// этой позиции
		if (flag_pv_f)
		{
			PV_save::update_PV_best_max_depth(0 // оценка ничья
													,depth);
		}

		return 0;
	}
#endif//#if REPETITION

//----------------------------------------------------------
// 4 отсечка по хеш-оценке

#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// отсечку по хеш оценке не используем для главного варианта
	// главным образом для достоверности оценки и линии варианта и для полноты картины
	// считаю обрывочный главный вариант неприемлемым.
	if (!flag_pv_f)
	{ 
		// смотрим не встречалась ли нам такая позиция если встречалась то 
		// пытаемся применить сохраненную оценку
		hash_score = Hash::look_hash_score_white(
			position.hash_key,depth,depth_max,
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

//----------------------------------------------------------
// 5 отсечка по razoring и futility

#if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING ))

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

*/

	if (!flag_pv_f)
	{ 
		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
		//&& (description_move_up != MOVE_IS_CAPTURE) // если не взятие
		&& (old_flag_check == CHECK_NO)) // если шаха нет

		{
			
			value = position.value_white_material 
				- position.value_black_material;

#if RAZORING
			// razoring
			// это потрясающая по силе отсечка :)
			// пока эта эвристика работает хуже чем должна. 
			// потому что все висит на статическом поиске. 
			// а у меня в нем пока нет обработки шахов.

			//--
			__int32 beta_q_margin =  beta - RAZORING_MARGIN[(depth_max - depth)];

			if (value < beta_q_margin)
			{
				value = Search_quiescence::forced_variant_white(position,
							alpha,beta,depth,flag_pv_f);

				//if ( value < beta_q_margin )
				if ( value < beta )
				{
					return value; 
				}
			}
			//--
#endif// #if RAZORING


#if FUTILITY
			//futility
			//--
			__int32 beta_margin =  beta + FUTILITY_MARGIN[(depth_max - depth)];

			if (value >= beta_margin)
			{
				value = position.value_white_material 
					- position.value_black_material
					+ Evaluate::eval_state(position);

				if (value >= beta )  
				{
					return value;  
				}
			}
			//--
#endif// #if FUTILITY

		}
	}

#endif// #if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING ))


//----------------------------------------------------------
// 6 отсечка по нулевому ходу

#if (NULL_MOVE && SAVE_LINE)
	// очень эффективная эвристика известная под именем нулевого хода
	// я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
	// суть его в том что
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	value = null_move_white(position,alpha,beta,depth_max,depth,description_move_up,
							flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE


//----------------------------------------------------------
// 7 генерируем и сортируем список избыточных ходов

	// насчитываем список избыточных ходов
	// множество этих списков и создают дерево перебора
	Generator_moves::generator_captures_white(list_surplus_moves,position);
	Generator_moves::generator_move_white(list_surplus_moves,position);
	Sorting_moves::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Sorting_moves::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY
 
#if (KILLER && SAVE_LINE)
	// если нет взятий
	// то киллер вставляем на первую позицию иначе на вторую
	//if ( list_surplus_moves.end_captures == CAPTURE_NO )
	//{
	//	Killer::insert_killer(list_surplus_moves,depth,0);
	//}
	//else
	//{
	//	Killer::insert_killer(list_surplus_moves,depth,1);
	//}


//Killer::insert_killer(list_surplus_moves,depth,0);
Killer::insert_killer(list_surplus_moves,depth,list_surplus_moves.end_captures);

#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход 
	// смотрим первым
	// точнее мы поднимаем ход из хеш таблицы на первое место сдвигая остальные вниз
	Hash::look_hash_move(list_surplus_moves,position.hash_key,depth
						,flag_insert_hash_move);

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


//#if (LMR && SAVE_LINE)
//	// ход из хеш-таблицы, лучшее взятие, два киллера, все взятия не редуцируются
//	lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures; // 
//	//lmr_reduction = FULL_SEARCH;
//#endif//#if LMR

/*
 нету    Internal iterative deepening
*/

//----------------------------------------------------------
// 8 бежим по списку избыточных ходов

	// старт и конец цикла должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)

			if (( !flag_pv_f )
			&& (description_move_up  == MOVE_IS_SIMPLE)//
			&& (i > 3)//
			&& (old_flag_check == CHECK_NO))
			{
				// отсечка базируется на счетчике ходов
				if((depth_max - depth) < 11)
				{
					if ( i >= futility_move_count[(depth_max - depth)]) continue;
				}
			}

#endif//#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)

		// реализуем ход из списка другими словами генерируем позицию
		Make_move::do_moves_white(position,i,depth,list_surplus_moves);

		// если король под шахом то 
		if (Generator_moves::king_white_check(position) != CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			Make_move::undo_moves_white(position,i,depth,list_surplus_moves);

            // идем дальше по списку 
			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = true;
		nodes = nodes + 1;

		// для каждого нового хода восстанавливаем глубину
		// что-бы продления работали только для выбранного хода
		depth_max_new = depth_max;

#if (EXTENSION_CHEK && SAVE_LINE)

		// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
		if (Generator_moves::king_black_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES; 
			depth_max_new = depth_max + 1;
		}
		else
		{
			flag_check = CHECK_NO;
		}

#endif//#if EXTENSION_CHEK

		// тип хода: 0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
		// 4 – короткая рокировка 5 – взятие на проходе и т.д.
		const __int32 description_move = ( list_surplus_moves.move[i] >> 6 ) & 63;

		// ходящая фигура 0 - нет фигуры 1 - пешка 2 - конь  3 - слон
		// 4 - ладья 5 - ферзь 6 - король
		//const __int32 piece_move = ( list_surplus_moves.move[i] >> 24 ) & 7;

#if SAVE_LINE


		// из списка возможных ходов копируем текущий ход в 
		// текущий вариант на текущей глубине.
		if (flag_pv_f)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		ZObrist::modification_random_key(position,1,i,list_surplus_moves
										,Make_move::get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = position.hash_key;

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		ZObrist::start_position_random_key(position);

		if (test_key_control != position.hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision white depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		ZObrist::save_hash_three(position.hash_key,new_depth); 


#endif//#if SAVE_LINE 

/*
За белых.

Если мы в главном варианте и это первый ход то его смотрим с полным окном и на полной глубине.

Все другие ходы смотрим с нулевым окном.
А все тихие ходы если они не шах и не киллер еще и редуцируем по глубине на один.
Делаем это если до максимальной глубины есть еще три полухода.

Дальше если ход больше альфы то его пересматриваем без редукции просто с нулевым окном.

Дальше если мы в главном варианте и если ход больше альфы и меньше беты 
значит мы вернулись на главный вариант и нужно пересчитывать с полной глубиной.


*/

		depth_max_new_scout = depth_max_new;

//_scout_scout white
#if (ALPHA_BETA_ON && SAVE_LINE)
		if ( (flag_pv_f) && (i == 0) ) 
#else
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			// если мы в главном варианте то
			// первый ход смотрим на полной глубине и с полным окном
			// мы предполагаем что если он первый то лучший и достоин полного 
			// просмотра
			// кроме того первый ход это ход из хеш-таблицы
			beta_scout = beta;
			flag_pv_f_scout = PV_YES;

			value = black(position,alpha,beta_scout,depth_max_new_scout,new_depth
							,description_move,
							flag_pv_f_scout,flag_check,flag_null_m);
		}
		else
		{
			//
			search_scout = true;

#if (LMR && SAVE_LINE)

			// считаем по методике LMR

			//(i > 3)// 4 это 0кеш, 1хорошее взятие, 2киллер, 3киллер, взятия, история 
			//if ((i > lmr_reduction) 
			//&& (flag_check == CHECK_NO) 
			//&& (old_flag_check == CHECK_NO)
			//&& ((depth_max_new - depth) >= LMR_DEPTH ))
	
			//if (
   //         (i > 5)
			//&& (flag_check == CHECK_NO)  // ход не шах. критически важный флаг!
			//&& (list_surplus_moves.move[i] != Killer::get_killer_moves_1(depth))// не киллер
			//&& (list_surplus_moves.move[i] != Killer::get_killer_moves_2(depth))
			//&& ((depth_max_new - depth) >= LMR_DEPTH ))// достаточная глубина
			//{


			if (
			(i != 0)
			&& (description_move == MOVE_IS_SIMPLE)// простой ход
			//&& (piece_move != N_KING)  // ход не королем
			&& (flag_check == CHECK_NO)  // ход не шах. критически важный флаг!
			&& (list_surplus_moves.move[i] != Killer::get_killer_moves_1(depth))// не киллер
			&& (list_surplus_moves.move[i] != Killer::get_killer_moves_2(depth))
			&& ((depth_max_new - depth) >= LMR_DEPTH ))// достаточная глубина
			{
				// ищем с редукцией глубины и с нулевым окном
				beta_scout = alpha + 1;
				flag_pv_f_scout = PV_NO;
				depth_max_new_scout = depth_max_new - LMR_REDUCTION_D;

				//
				value = black(position,alpha,beta_scout,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
				//
				search_scout = (value > alpha);

				depth_max_new_scout = depth_max_new;
			}
#endif//#if  LMR

			if (search_scout)
			{
				beta_scout = alpha + 1;
				flag_pv_f_scout = PV_NO;
				//
				value = black(position,alpha,beta_scout,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);

				// для главного варианта пересчитываем с полным окном и глубиной
				// суть поиска с нулевым окном в том что мы не ждем что вариант улучшит 
				// альфу поэтому считаем его с максимально узким т.е. нулевым окном
				// если нас ждет сюрприз то пересчитываем c полным окном
				// здесь мы ветке возвращем статус главного варианта
				if ( (flag_pv_f) && (value > alpha) && (value < beta) )
				{
					beta_scout = beta;
					flag_pv_f_scout = PV_YES;

					//
					value = black(position,alpha,beta_scout,depth_max_new_scout,new_depth
									,description_move,
									flag_pv_f_scout,flag_check,flag_null_m);
				}
			}
		}

		// восстанавливаем позицию
		Make_move::undo_moves_white(position,i,depth,list_surplus_moves);


#if SAVE_LINE
		// восстанавливаем хеш-ключ доски
		position.hash_key = key_undo;

		//--------------------------------------------
		if (value > value_max)
		{
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if ALPHA_BETA_ON
			if (value > alpha)
			{					
				if (value >= beta)
				{        
					// пишем киллера если ход не взятие
					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
#if KILLER
						Killer::save_killer(i,list_surplus_moves,depth);
#endif//#if KILLER
#if SW_HISTORY
						History::white_history_good_save(i,list_surplus_moves,depth,depth_max);
#endif//#if SW_HISTORY
					}


#if HASH
					// кэшируем лучший на данный момент ход
					Hash::save_best_hash_score_or_move(i
						, position.hash_key, list_surplus_moves, 
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
				flag_save_best = true;
				i_hash_moves_1 = i; // ход который улучшает альфу (1- улучшили альфу) 
									// нужно для кеширования 

				// здесь мы перезаписываем лучший вариант соответствующий 
				// лучшей оценке
				if (flag_pv_f)
				{
					PV_save::update_PV_best_point(pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			if (flag_pv_f)
			{
				PV_save::update_PV_best_point(pv_best_point);
			}

			flag_save_best = true;
			i_hash_moves_1 = i;// при тестировании полного перебора
#endif// #if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_max = value;
			i_hash_moves_3 = i; // ход дающий локальный максимум 
								//(3-просто локальный максимум)нужно для кеширования

		}
#endif//#if SAVE_LINE 

	}


//----------------------------------------------------------
// 9 обработка случая когда не было легальных ходов
//   если не одного хода сделано не было то производим статическую оценку позиции
//   это случай пата или мата

	if (flag_is_legal_moves == false)
	{
#if SAVE_LINE
		value = 0;
		if (Generator_moves::king_white_check(position) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка 
		// этой позиции
		if (flag_pv_f)
		{
			PV_save::update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == true)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать 
		// на более высокий уровень
		if (flag_pv_f)
		{
			PV_save::update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)

	//History::white_history_bad_save(list_surplus_moves,depth,depth_max);

	if (flag_save_best == true)
	{
		// кэшируем ход улучшающий альфу т.е. флаг 1
		Hash::save_best_hash_score_or_move(i_hash_moves_1
			, position.hash_key, list_surplus_moves, 
			alpha, EXACT, depth, depth_max);
	}
	else
	{
		// кэшируем ход дающий локальный максимум т.е. флаг 3
		Hash::save_best_hash_score_or_move(i_hash_moves_3
			, position.hash_key, list_surplus_moves, 
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
			std::cout << "bitboard.get_hash_key()= " << position.hash_key << "\n";
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
				<< ((list_surplus_moves.move[0]>>6)& 63) << "\n";

			std::cout << "Ходящая фигура " 
				<< PV_save::int_to_char_board(((list_surplus_moves.move[0]>>24)& 7),1)
				<< "\n";
               
			std::cout <<PV_save::x_to_notation(((list_surplus_moves.move[0]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>12)& 63)/8);
			std::cout <<PV_save::x_to_notation(((list_surplus_moves.move[0]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>18)& 63)/8) << "\n";

			std::cout << "ход найденный в поиске " << "\n";
			std::cout << "hash_moves_test12= " << hash_moves_test13  << "\n";
			std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				<< ((list_surplus_moves.move[hash_moves_test13]>>6)& 63) << "\n";
			std::cout << "Ходящая фигура " 
				<< PV_save::int_to_char_board(((
					list_surplus_moves.move[hash_moves_test13]>>24)& 7),1)<< "\n";
               
			std::cout <<PV_save::x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>12)& 63)/8);
			std::cout <<PV_save::x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>18)& 63)/8) << "\n";
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
	struct Position & position,// представление доски
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 description_move_up,// тип хода который привел к данной позиции
	const bool flag_pv_f,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m// флаг нулевого хода
)
{
	__int32 value;              // текущая оценка позиции
	__int32 value_min = 9999999;   // оценка позиции лучшая в узле 

	struct List list_surplus_moves; // список возможных ходов (всевозможные ходы из данного узла)

	bool flag_is_legal_moves = false;           // флаг существования легальных ходов
	bool flag_save_best = false;           // пишем ли лучший вариант
	const unsigned __int8 new_depth = depth + 1; // следующая глубина
	bool flag_check = false;//флаг шаха

	unsigned __int8 depth_max_new;//новая глубина перебора

	// переменные параметры в поиске NegaScout и LMR
		__int32 alpha_scout;//
		unsigned __int8 depth_max_new_scout;// максимальная глубина
		bool flag_pv_f_scout;// основной ли это вариант (pv)
		bool search_scout;

#if SAVE_LINE
 
	struct PV_line pv_best_point;// лучший для данного узла вариант
	bool taking_passage_yes_mem = false;
	unsigned __int8 taking_passage_y_mem = 0;
	unsigned __int8 taking_passage_x_mem = 0;

	unsigned __int8 flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
	const unsigned __int64 key_undo = position.hash_key;   // запоминаем хеш-ключ даннного узла
	__int32 hash_score = -1000000000;// хеш-оценка позиции
	unsigned __int8 i_hash_moves_1;// ход который улучшает бету (1- улучшили бету) нужно для кеширования 
	unsigned __int8 i_hash_moves_3;// ход дающий локальный минимум (3-просто локальный минимум)нужно для кеширования

//#if (LMR && SAVE_LINE)
//	unsigned __int8 lmr_reduction;//
//#endif//#if (LMR

	//unsigned __int64 test_key_control;// запоминаем хеш-ключ даннного узла
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 

#if (HASH_SCORE && SAVE_LINE)
	// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 
	// 2-отсечка по бете(альфе))
	unsigned __int8 test_flag_hash_moves;
	unsigned __int8 hash_depth;// максимальная глубина
	unsigned __int8 hash_depth_max;// максимальная глубина
#endif//#if (HASH_SCORE)


	// на предельной глубине производим статическую оценку позиции
	if (depth >= depth_max)
	{
#if SAVE_LINE

	//  форсировка
	//  подключаем форсированный вариант
#if FORCED 
		// включаем режим форсировки(просчета только взятий)
		value = Search_quiescence::forced_variant_black(position,alpha,beta
													,depth,flag_pv_f);
#endif//#if FORCED 


	// включаем оценку без форсировки
#if !FORCED
		value = max_depth_black(position,alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

#if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
	// обнаружили повторение позиции
	if (ZObrist::analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			PV_save::update_PV_best_max_depth(0 // оценка ничья
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
		hash_score = Hash::look_hash_score_black(
			position.hash_key,depth,depth_max,
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

#if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING ))

	/*
	Вот что пишет по этому поводу первоисточник:
	Ernst A. Heinz
	Given a suitable value for max posn score, 
	it is therefore safe to simply cut off 
	all non-checking moves at frontier nodes 
	for which the futility condition 
	mat balance(node) + mat gain(move) + max posn score  <= alpha(node) holds. 

	*/

	if (!flag_pv_f)
	{ 

		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
		//&& (description_move_up != MOVE_IS_CAPTURE) // если не взятие
		&& (old_flag_check == CHECK_NO)) // если шаха нет
		{
			
			value = position.value_white_material 
				- position.value_black_material;

#if RAZORING
			//razoring 

			//--
			__int32 alpha_q_margin =  alpha + RAZORING_MARGIN[(depth_max - depth)];

			if (value > alpha_q_margin)
			{
				value = Search_quiescence::forced_variant_black(position,
							alpha,beta,depth,flag_pv_f);

				//if (value > alpha_q_margin)
				if (value > alpha)
				{
					return value; 
				}
			}
			//--
#endif// #if RAZORING


#if FUTILITY
			//futility

			//--
			__int32 alpha_margin =  alpha - FUTILITY_MARGIN[(depth_max - depth)];

			if (value <= alpha_margin)
			{
				value = position.value_white_material
					- position.value_black_material
					+ Evaluate::eval_state(position);

				if (value <= alpha )  
				{
					return value;
				}
			}
			//--
#endif// #if FUTILITY
		}
	}

#endif// #if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING )) 


#if (NULL_MOVE && SAVE_LINE)
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	value = null_move_black(position,alpha,beta,depth_max,depth,description_move_up,
							flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE

	//Generator_moves::mobility_evaluation(position);

	// насчитываем список избыточных ходов
	// множество этих списков и создают дерево перебора
	Generator_moves::generator_captures_black(list_surplus_moves,position);
	Generator_moves::generator_move_black(list_surplus_moves,position);
	Sorting_moves::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Sorting_moves::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY


#if (KILLER && SAVE_LINE)
	// если нет взятий
	// то киллер вставляем на первую позицию иначе на вторую
	//if (list_surplus_moves.end_captures == CAPTURE_NO)
	//{
	//	Killer::insert_killer(list_surplus_moves,depth,0);
	//}
	//else
	//{
	//	Killer::insert_killer(list_surplus_moves,depth,1);
	//}

//Killer::insert_killer(list_surplus_moves,depth,0);
Killer::insert_killer(list_surplus_moves,depth,list_surplus_moves.end_captures);

#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась 
	// то ее ход смотрим первым
	Hash::look_hash_move(list_surplus_moves
						,position.hash_key,depth,flag_insert_hash_move);

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


//#if (LMR && SAVE_LINE)
//	// ход из хеш-таблицы, лучшее взятие, два киллера, все взятия не редуцируются
//	//lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures; // 
//	//lmr_reduction = FULL_SEARCH; // 
//#endif//#if LMR


	// бежим по списку избыточных ходов
	// старт и конец цикла; должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)


			if (( !flag_pv_f )
			&& (description_move_up  == MOVE_IS_SIMPLE)//
			&& (i > 3)//
			&& (old_flag_check == CHECK_NO))
			{
				//Move count based pruning
				if((depth_max - depth) < 11)
				{
					if ( i >= futility_move_count[(depth_max - depth)]) continue;
				}
			}

#endif//#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)

		// реализуем ход из списка другими словами генерируем позицию
		Make_move::do_moves_black(position,i,depth,list_surplus_moves);

		// если король по шахом то
		if (Generator_moves::king_black_check(position)!= CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			Make_move::undo_moves_black(position,i,depth,list_surplus_moves);

			// идем дальше по списку 
			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = true;
		nodes = nodes + 1;

		depth_max_new = depth_max;

#if (EXTENSION_CHEK && SAVE_LINE)
		// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
		if(Generator_moves::king_white_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES;
			depth_max_new = depth_max + 1;
		}
		else
		{
			flag_check = CHECK_NO;
		}
#endif//#if EXTENSION_CHEK

		const __int32 description_move = ( list_surplus_moves.move[i] >> 6 ) & 63;
		//const __int32 piece_move = ( list_surplus_moves.move[i] >> 24 ) & 7;

#if SAVE_LINE
		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине.
		if (flag_pv_f)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		ZObrist::modification_random_key(position,0,i,list_surplus_moves,Make_move::get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = position.hash_key;

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		ZObrist::start_position_random_key(position);

		if (test_key_control != position.hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision black depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		ZObrist::save_hash_three(position.hash_key,new_depth); 

#endif//#if SAVE_LINE 


		depth_max_new_scout = depth_max_new;

//_scout_scout black
#if (ALPHA_BETA_ON && SAVE_LINE)
		if ( (flag_pv_f) && (i == 0) ) 
#else
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			alpha_scout = alpha;//
			flag_pv_f_scout = PV_YES;// основной ли это вариант (pv)
	
			value = white(position,alpha_scout,beta,depth_max_new_scout,new_depth,description_move,
							flag_pv_f_scout,flag_check,flag_null_m);
		}
		else
		{
			//
			search_scout = true;

#if (LMR && SAVE_LINE)
			// считаем по методике LMR

			//if ((i > lmr_reduction)
			//&& (flag_check == CHECK_NO) 
			//&& (old_flag_check == CHECK_NO)
			//&& ((depth_max_new - depth) >= LMR_DEPTH )) 

			//if (
   //         (i > 5)
			//&& (flag_check == CHECK_NO)  // ход не шах. критически важный флаг!
			//&& (list_surplus_moves.move[i] != Killer::get_killer_moves_1(depth))// не киллер
			//&& (list_surplus_moves.move[i] != Killer::get_killer_moves_2(depth))
			//&& ((depth_max_new - depth) >= LMR_DEPTH ))// достаточная глубина
			//{

			if (
			(i != 0)
			&& (description_move == MOVE_IS_SIMPLE)// простой ход
			//&& (piece_move != N_KING)  // ход не королем
			&& (flag_check == CHECK_NO)
			&& (list_surplus_moves.move[i] != Killer::get_killer_moves_1(depth))
			&& (list_surplus_moves.move[i] != Killer::get_killer_moves_2(depth))
			&& ((depth_max_new - depth) >= LMR_DEPTH ))
			{
				alpha_scout = beta - 1;//
				flag_pv_f_scout = PV_NO;//
				depth_max_new_scout = depth_max_new - LMR_REDUCTION_D;// максимальная глубина

				value = white(position,alpha_scout,beta,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
				//
				search_scout = (value < beta);

				depth_max_new_scout = depth_max_new;

			}
#endif//#if  LMR

			if (search_scout)
			{
				alpha_scout = beta - 1;//
				flag_pv_f_scout = PV_NO;//

				value = white(position,alpha_scout,beta,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);

				if ( (flag_pv_f) && (value > alpha) && (value < beta) )
				{
					alpha_scout = alpha;//
					flag_pv_f_scout = PV_YES;//

					value = white(position,alpha_scout,beta,depth_max_new_scout,new_depth
									,description_move,
									flag_pv_f_scout,flag_check,flag_null_m);
				}
			}
		}

		// восстанавливаем позицию
		Make_move::undo_moves_black(position,i,depth,list_surplus_moves);


#if SAVE_LINE
		// восстанавливаем хеш-ключ доски
		position.hash_key = key_undo;

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

					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
#if KILLER
						Killer::save_killer(i,list_surplus_moves,depth);
#endif//#if KILLER
#if SW_HISTORY
						History::black_history_good_save(i,list_surplus_moves,depth,depth_max);
#endif//#if SW_HISTORY
					}


#if HASH
					// кэшируем лучший на данный момент ход
					Hash::save_best_hash_score_or_move(i, position.hash_key
													  ,list_surplus_moves, 
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
				flag_save_best = true;
				i_hash_moves_1 = i; //ход который улучшает бету (1- улучшили бету) 
									//нужно для кеширования 

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				if (flag_pv_f)
				{
					PV_save::update_PV_best_point(pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			if (flag_pv_f)
			{
				PV_save::update_PV_best_point(pv_best_point);
			}

			flag_save_best = true;
			i_hash_moves_1 = i;// при тестировании полного перебора
#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_min = value;
			i_hash_moves_3 = i; // ход дающий локальный минимум 
								//(3-просто локальный минимум)нужно для кеширования

		}
#endif//#if SAVE_LINE 

	}

	//-----------------------------------------------
	// обработка случая когда не было легальных ходов
	// если не одного хода сделано не было то производим статическую оценку позиции
	// это случай пата или мата
	if (flag_is_legal_moves == false)
	{
#if SAVE_LINE
		value = 0;
		if (Generator_moves::king_black_check(position) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			PV_save::update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == true)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на 
		// более высокий уровень
		if (flag_pv_f)
		{
			PV_save::update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)

	//History::black_history_bad_save(list_surplus_moves,depth,depth_max);

	if ( flag_save_best == true ) 
	{
		// кэшируем ход улучшающий бету т.е. флаг 1
		Hash::save_best_hash_score_or_move(i_hash_moves_1,position.hash_key,list_surplus_moves, 
			beta, EXACT, depth, depth_max);
	}
	else
	{
		// кэшируем ход дающий локальный минимум т.е. флаг 3
		Hash::save_best_hash_score_or_move(i_hash_moves_3,position.hash_key,list_surplus_moves, 
			value_min, UPPER, depth, depth_max);
	}
#endif//#if HASH

	return value_min;
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
	struct Position & position,// представление доски
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

	if ((flag_stop_null_move == NULL_YES) //
	&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	&& (depth_max > NULL_REDUCTION ) // иначе уходим в минус а у нас глубина всегда положительна
	&& ((depth_max - depth) >= NULL_DEPTH ) // если до максимальной глубины расстояние достаточное
	&& (flag_pv_f == PV_NO) // если мы не в главном варианте
	&& (description_move != MOVE_IS_CAPTURE) // если ход не взятие
	&& (old_flag_check == CHECK_NO) // если шаха нет
	//&& (bitboard- >= beta) // дикая потеря эффективности. почему не понимаю
									// наверное дело в том что я не использую статической
									// составляющей оценки а только материал
	)
	{// тогда вызываем нулевой ход


		// запоминаем флаги взятия на проходе и обнуляем разрешение.
		bool taking_passage_yes_mem = position.taking_passage_yes;
		unsigned __int8 taking_passage_x_mem = position.taking_passage_x;
		unsigned __int8 taking_passage_y_mem = position.taking_passage_y;

        unsigned __int64 hash_key_temp = position.hash_key;

		// меняем хеш-ключ под нулевой ход -------------------------
		// учитываем взятие на проходе
		if (taking_passage_yes_mem)
		{
			hash_key_temp = hash_key_temp ^ 11782889425078278382;
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ 6567344619664815343);

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ 10614454250492056336);
		}

		// учитываем цвет хода
		// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
		// некоторые позиции отличаются только цветом хода
		hash_key_temp = hash_key_temp ^ 8935749541958697872;
		position.hash_key = hash_key_temp;


		position.taking_passage_yes = 0;
		position.taking_passage_x = 0;
		position.taking_passage_y = 0;

		position.colour_move = !position.colour_move;

		const __int64 nodes_null_rem = nodes;

		// TEST тестировал изменение хеш-ключа
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//ZObrist::start_position_random_key(bitboard);

		// не динамическая редукция
		__int32 value = black(position,alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES); 

		//--------------------------------------------------------
		// динамическая редукция нулевого хода при дистанции до максимальной глубины
		// больше равной 5 редуцируем 4 иначе 3
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
		//
		//value = Black(bitboard,alpha,(alpha + 1),
		//	(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES); 
		//--------------------------------------------------------

		// узлы насчитанные в нулевом поиске не учитываем
		nodes = nodes_null_rem;

		// TEST
		//std::cout << "bitboard- "<< bitboard- << "\n";
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

		position.taking_passage_yes = taking_passage_yes_mem;
		position.taking_passage_x = taking_passage_x_mem;
		position.taking_passage_y = taking_passage_y_mem;

		position.colour_move = !position.colour_move;

		// восстанавливаем хеш-ключ доски
		position.hash_key = key_undo;


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
			value = white(position,(beta - 1),beta
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
	struct Position & position,// представление доски
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

	if ((flag_stop_null_move == NULL_YES) //
	&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	&& (depth_max > NULL_REDUCTION ) // иначе уходим в минус а у нас глубина всегда положительна
	&& ((depth_max - depth) >= NULL_DEPTH ) // если до максимальной глубины расстояние достаточное
	&& (flag_pv_f == PV_NO) // если мы не в главном варианте
	&& (description_move != MOVE_IS_CAPTURE) // если ход не взятие
	&& (old_flag_check == CHECK_NO) // если шаха нет
	//&& (bitboard- <= alpha) //
	)
	{

		// тогда вызываем нулевой ход
		const bool taking_passage_yes_mem = position.taking_passage_yes;
		const unsigned __int8 taking_passage_x_mem = position.taking_passage_x;
		const unsigned __int8 taking_passage_y_mem = position.taking_passage_y;


       unsigned __int64 hash_key_temp = position.hash_key;

		// меняем хеш-ключ под нулевой ход -------------------------
		// учитываем взятие на проходе
		if ( taking_passage_yes_mem)
		{
			hash_key_temp = hash_key_temp ^ 11782889425078278382;
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ 6567344619664815343);

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ 10614454250492056336);
		}

		// учитываем цвет хода
		// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
		// некоторые позиции отличаются только цветом хода
		hash_key_temp = hash_key_temp ^ 8935749541958697872;

		position.hash_key = hash_key_temp;

//--------

		position.taking_passage_yes = 0;
		position.taking_passage_x = 0;
		position.taking_passage_y = 0;

		position.colour_move = !position.colour_move;
	
		const __int64 nodes_null_rem = nodes;

		// TEST тестировал изменение хеш-ключа
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//ZObrist::start_position_random_key(bitboard);

		__int32 value = white(position,(beta - 1),beta,(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES);

		//--------------------------------------------------------
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 

		//value = White(bitboard,(beta - 1),beta
		//	,(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES);
		//--------------------------------------------------------

		nodes = nodes_null_rem;

		position.taking_passage_yes = taking_passage_yes_mem;
		position.taking_passage_x = taking_passage_x_mem;
		position.taking_passage_y = taking_passage_y_mem;

		position.colour_move = !position.colour_move;

		// восстанавливаем хеш-ключ доски
		position.hash_key = key_undo;


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
			value = black(position,alpha,(alpha + 1)
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
	struct Position & position,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// основной ли это вариант (pv)
	const unsigned __int8 depth// глубина
)
{
	__int32 score = 0;
	__int32 score_f = 0;

	score = Evaluate::eval_state(position) 
		+ (position.value_white_material 
		- position.value_black_material)
	;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(bitboard) != bitboard-)
	//{
	//	std::cout << "WHITE  Eval_forced(bitboard) != bitboard-" <<"\n";
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<"\n";
	//	std::cout << "bitboard-= "<< bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
	//надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
	if (flag_pv_f)
	{
		PV_save::update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//именно здесь можно проверить правильно ли рисуется вариант на предельной глубине
	//кроме  случая шаха и пата
	//Print_std::cout(list_surplus_moves,pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score ;
}

//	===================================================================
// производим расчеты на максимальной глубине. за черных
inline __int32 Search::max_depth_black
(
	struct Position & position,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// основной ли это вариант (pv)
	const unsigned __int8 depth// глубина
)
{
	__int32 score = 0;
	__int32 score_f = 0;

	score = Evaluate::eval_state(position) 
		+ (position.value_white_material
		- position.value_black_material)
	;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(bitboard) != bitboard-)
	//{
	//	std::cout << "BLACK  Eval_forced(bitboard) != bitboard-" <<"\n";
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<"\n";
	//	std::cout << "bitboard-= "<< bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
	//надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
	if (flag_pv_f)
	{
		PV_save::update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//именно здесь можно проверить правильно ли рисуется вариант на предельной глубине
	//кроме  случая шаха и пата
	//Print_std::cout(list_surplus_moves,pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score;
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
struct PV_line & Search::get_pv_best()
{
	return PV_save::get_pv_best();
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
	double _deep_time_terminate//
)
{
	deep_time_terminate = _deep_time_terminate;
}

//	===================================================================
//
void Search::ini_flag_stop_null_move
(
	bool _flag_stop_null_move//
)
{
	flag_stop_null_move = _flag_stop_null_move;
}

//	===================================================================
//
void Search::set_depth_max_rem
(
	unsigned __int8 _depth_max_rem//
)
{
	depth_max_rem = _depth_max_rem;
}

//	===================================================================
// времени больше нет. экстренный выход
inline void Search::quit_or_stop()
{
	double delta_time_current;

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
				std::cout << "Ifrit: quit command. I am terminated." << "\n";
				Hash::hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// остановка поиска
				flag_stop_search = 1;
			}
		}

		//экстренный выход с глубины. просрочили время на поиск
		Timer::time_stop();
		delta_time_current = Timer::time_delta();

		//выходим по времени
		if ((deep_time_terminate > 0) && (deep_time_terminate < delta_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;

}