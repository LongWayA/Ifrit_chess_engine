
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009 25.07.2010 17.10.2010

#include <fstream>
#include <iostream>

#include "_search_switch.h"
#include "a_move_list_struct.h"
#include "b_search_const.h"
#include "search.h"
#include "search_quiescence.h"
#include "evaluate.h"
#include "move_generation.h"
#include "make_move.h"
#include "move_ordering.h"
#include "killer_heuristic.h"
#include "history_heuristic.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "b_constants.h"
#include "time_management.h"
#include "win_32.h"
#include "pv_save.h"
#include "b_move_generation_const.h"
#include "uci_engine_to_gui.h"

	unsigned __int16 audit;/// задержка опроса команды экстренного выхода
	__int8 string_in2[65536];///

	unsigned __int8 depth_max_rem;///
	bool flag_stop_search;/// переменная для отсечки перебора при экстренном выходе
	__int64 nodes;/// количество узлов

	//int return_hash_score;

	double deep_time_terminate; /// передаем на глубину время остановки счета 


/*
 Ищем максимум в узле. перебор за белых 

1.Отсечка по командам команды stop или quit. Смотрим, не 
   было ли команды stop или quit. Если команда была, то выходим 
   из поиска с нулевой оценкой. Далее результат поиска для этой 
   итерации мы не учитываем.

2.Отсечка по повторению позиции. Если позиция повторилась, 
   то выходим из поиска с оценкой ничья. Повтор позиции 
   детектируем через анализ хеш ключей позиций на линии варианта. 

3.Оценка из таблицы перестановок (хеш-таблицы). Пытаемся 
   воспользоваться результатом предыдущего поиска. Смотрим 
   в хеш-таблице. Если такая позиция уже встречалась, и 
   она была рассмотрена на такую же или большую глубину 
   тогда анализируем тип оценки. Если тип оценки совпадает, 
   и она проходит по альфа бета пределам, то используем ее. 
   Оценка может быть трех видов (описание со стороны белых). 
   Отсечка по бете, не смогли  улучшить альфу, точная оценка, 
   т.е. улучшили альфу, но не отсекались по бете (она лежит в альфа бета пределах). 
   Выходим из поиска возвращая эту оценку. Не используем таблицу в главном варианте.

4.Статический поиск. Если достигли максимальной глубины, то вызываем 
   статический поиск. В статическом поиске смотрим только выгодные 
   размены. Т.е. посмотрели цепочку взятий и вызвали оценку позиции.

5.Отсечка по razoring. Описание со стороны белых. Если статическая 
   оценка позиции плюс бонус все равно меньше беты то смотрим 
   статическим поиском. Бонус это пара пешек или фигура. Когда 
   имеем такой недостаток материала, то все что нужно это посмотреть можем 
   мы его вернуть или нет. Если можем, то продолжаем поиск, а если нет, 
   то возвращаем статическую оценку. Конечно же черные за нее уцепятся. 
   Поэтому эта эвристика допустима только в неглавном варианте. В данной 
   реализации ее устойчивость оставляет желать лучшего, потому что мы не 
   смотрим шахи в статическом поиске.

6.Отсечка по futility. Описание со стороны белых. Если статическая оценка 
   позиции минус бонус все равно больше или равно бете то дальше не 
   считаем. Мы предполагаем, что, так как у нас перевес в пару 
   пешек или фигуру и сейчас наш ход то мы сможем найти ходы, 
   которые как минимум удержат наш перевес. Черные пересчитывать не 
   будут так, что эта оценка может быть опровергнута только с 
   ростом глубины поиска.

7.Отсечка по нулевому ходу. Мы пропускаем свой ход и отдаем на 
   расчет черным. Таким образом, черные делают два хода подряд. 
   Если и при таком бонусе оценка остается больше беты, то можно 
   не считать. Ситуация аналогична отсечке по  futility.

8.Список избыточных ходов. Отсечки кончились, дальше мы насчитываем 
   список избыточных ходов. Список избыточен, потому что в нем 
   есть ходы под шах и под шахом.
  Сортировка ходов. Взятия мы сортируем по эвристике 
  MVV-LVA (Most Valuable Victim - Least Valuable Aggressor). 
  Тихие ходы сортируем по эвристике history heuristic. 
  Киллеров (killer moves) помещаем после всех взятий. Ход из таблицы 
  перестановок (transposition table) помещаем в самое начало.

9.Перебор списка избыточных ходов. 
 9.1 Делаем ход. 
 9.2 Прежде всего, проверяем под шахом ли наш король после хода. 
      Если да то отменяем ход как некорректный и смотрим следующий.
 9.3 Если есть шах, то для этой ветки увеличиваем максимальную 
      глубину перебора на единицу.
 9.4 Считаем позицию используя метод Late Move Reductions (LMR). 
      Описание реализации дам отдельно.
 9.5 Возвращаем ход.
 9.6 Описание со стороны белых. Полученную оценку сравниваем с 
      локальным максимумом. Если она больше то максимум приравниваем 
	  к ней. Далее оценку сравниваем с альфой. Если она больше то 
	  альфу приравниваем к ней. Далее оценку сравниваем с бетой. 
	  Если она больше или равна, то отсекаем поиск, возвращая оценку 
	  предварительно записав данные в таблицу перестановок и обновив киллеры.

10.Все ходы рассмотрены. Если легальных ходов не было значит это случай 
    мата или пата. Если ходов нет, а наш король под шахом значит нам мат. 
	Если ходов нет и шаха нам нет, значит нам пат. Если легальные ходы были 
	то обновляем таблицу перестановок и выходим из поиска 
	возвращая локальный максимум.


Описание поиска Late Move Reductions (LMR).(Со стороны белых)

Полный поиск проводим только в первой ветке главного узла. И для тех 
веток, которые претендуют на эту роль. Тогда проводим поиск на полной 
глубине и с полным окном.

Если мы не в первой ветке главного узла тогда смотрим, можем 
ли мы применить лмр поиск.
Если нет шаха, если ход не из таблицы перестановок, если ход не киллер, 
если ход простой, т.е. не взятие, рокировка и т.д. Если глубины хватает  
для редукции, тогда проводим поиск с сокращением глубины на один полуход 
и с нулевым окном. 
Если полученная оценка больше альфы тогда проводим повторный поиск на 
полной глубине, но по-прежнему с нулевым окном.
Если полученная оценка больше альфы и мы в главном узле то проводим поиск 
на полной глубине и с полным окном. Мы нашли новую главную ветку.

Если по каким то причинам сокращенный поиск проводить нельзя, то проводим поиск 
на полной глубине с нулевым окном. Если полученная оценка больше альфы и мы в 
главном узле, то проводим поиск на полной глубине и с полным окном. Мы нашли новую 
главную ветку.

*/
__int32 Search::white_searching_max_value
(
	struct Position & position,// представление доски
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth,// глубина
	const unsigned __int8 depth_max,// максимальная глубина
	const bool flag_pv_node,// основной ли это вариант (pv)

	const __int32 old_description_move,// тип хода который привел к данной позиции
	const bool flag_old_check,//флаг шаха
	const bool flag_null_m// флаг нулевого хода
)
{
	struct List list_surplus_moves;// список возможных ходов 
								   //(всевозможные ходы из данного узла)

	__int32 value;// текущая оценка позиции
	__int32 value_max = -INFINITE_SCORE;// оценка позиции лучшая в узле 
	const unsigned __int8 new_depth = depth + 1;// следующая глубина
	unsigned __int8 new_depth_max;//новая глубина перебора

	// переменные параметры в поиске LMR и PVS
	__int32 beta_null_window;//
	unsigned __int8 new_lmr_depth_max;// максимальная глубина
	bool flag_pv_move;// основной ли это вариант (pv)

	bool flag_do_pv_search;
	bool flag_do_null_window_search;
	bool flag_do_lmr_search;

	bool flag_is_legal_moves = false;// флаг существования легальных ходов
	bool flag_alpha_updated = false;// пишем ли лучший вариант
	bool flag_check = false;//флаг шаха


//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
 
	struct PV_line pv_best_point;// лучший для данного узла вариант

	  
	// все для кеширования
	const unsigned __int64 key_undo = position.hash_key;// запоминаем хеш-ключ даннного узла
	unsigned __int8  flag_insert_hash_move = 0;// поместили ли ход из хеш-таблицы на первую позицию
	__int32 hash_score = -1000000000;// хеш-оценка позиции
	unsigned __int8 i_hash_moves_1;// ход который улучшает альфу (1- улучшили альфу) нужно для кеширования 
	unsigned __int8 i_hash_moves_3;// ход дающий локальный максимум (3-просто локальный максимум )нужно для кеширования


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
	if (flag_stop_search == true) return DRAWN_SCORE;
	// 
	quit_or_stop();

#endif//#if SAVE_LINE 

//----------------------------------------------------------
// 2 отсечка по повторению позиции

#if (REPETITION && ALPHA_BETA_ON && SAVE_LINE)
	// обнаружили повторение позиции
	if (Zobrist_hashing::analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка 
		// этой позиции
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(DRAWN_SCORE // оценка ничья
													,depth);
		}

		return 0;
	}
#endif//#if REPETITION

//----------------------------------------------------------
// 3 отсечка по хеш-оценке

#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// отсечку по хеш оценке не используем для главного варианта
	// главным образом для достоверности оценки и линии варианта и для полноты картины
	// считаю обрывочный главный вариант неприемлемым.
	if (flag_pv_node == PV_NO)
	{ 
		// смотрим не встречалась ли нам такая позиция если встречалась то 
		// пытаемся применить сохраненную оценку
		hash_score = Transposition_table::look_hash_score_white(
			position.hash_key,depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (Transposition_table::get_flag_found_score())
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
// 4 отсечка по максимальной глубине (оценка позиции)

	// на предельной глубине производим статическую оценку позиции
	if (depth >= depth_max)
	{
#if SAVE_LINE

	// форсировка
	// подключаем форсированный вариант
#if FORCED
		// включаем режим просчета только взятий
		value = Search_quiescence::forced_variant_white(position,alpha,beta
													,depth,flag_pv_node);
#endif//#if FORCED

	// включаем оценку без форсировки
#if !FORCED 
		value = max_depth_white(position,alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

//----------------------------------------------------------
// 5 6 отсечка по razoring и futility

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

	if (flag_pv_node == PV_NO)
	{ 
		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
		//&& (description_move_up != MOVE_IS_CAPTURE) // если не взятие
		&& (flag_old_check == CHECK_NO)) // если шаха нет

		{
			
				value = position.value_white_material 
					- position.value_black_material
					+ Evaluate::eval_state(position);

#if RAZORING
			// razoring
			// это потрясающая по силе отсечка.
			// пока эта эвристика работает хуже чем должна. 
			// потому что все висит на статическом поиске. 
			// а у меня в нем пока нет обработки шахов.

			//--
			__int32 beta_q_margin =  beta - RAZORING_MARGIN[(depth_max - depth)];

			if (value < beta_q_margin)
			{

				value = Search_quiescence::forced_variant_white(position,
							(beta_q_margin - 1),beta_q_margin,depth,flag_pv_node);

				if ( value < beta_q_margin )
				{
					return value; 
				}
			}
			//--
#endif// #if RAZORING


#if FUTILITY
			//futility
			//--
			if (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
			{
				__int32 beta_margin =  beta + FUTILITY_MARGIN[(depth_max - depth)];

				if (value >= beta_margin)
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
// 7 отсечка по нулевому ходу
/*

*/
#if (NULL_MOVE && SAVE_LINE)
	// очень эффективная эвристика известная под именем нулевого хода
	// я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
	// суть его в том что
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	value = white_null_move(position,alpha,beta,depth_max,depth,old_description_move,
							flag_pv_node,flag_old_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE


//----------------------------------------------------------
// 8 генерируем и сортируем список избыточных ходов

	// насчитываем список избыточных ходов
	// множество этих списков и создают дерево перебора
	Move_generation::generator_captures_white(list_surplus_moves,position);
	Move_generation::generator_move_white(list_surplus_moves,position);
	Move_ordering::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Move_ordering::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY
 
#if (KILLER && SAVE_LINE)

	Killer_heuristic::insert_killer
		(list_surplus_moves,depth,list_surplus_moves.end_captures);

#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход 
	// смотрим первым
	// точнее мы поднимаем ход из хеш таблицы на первое место сдвигая остальные вниз
	Transposition_table::look_hash_move(list_surplus_moves,position.hash_key,depth
						,flag_insert_hash_move);

// для полного теста нужно отключать отсечку по оценке
#if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

	if (flag_insert_hash_move == 1)
	{
		std::cout << "коллизия хеша белые" << std::endl;
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH

/*
 нету    Internal iterative deepening
*/

//----------------------------------------------------------
// 9 бежим по списку избыточных ходов

	// бежим по списку избыточных ходов
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
		if (Move_generation::king_white_check(position) != CHECK_NO)
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
		new_depth_max = depth_max;

#if (EXTENSION_CHEK && SAVE_LINE)

		// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
		if (Move_generation::king_black_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES; 
			new_depth_max = depth_max + 1;
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
		if (flag_pv_node)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		Zobrist_hashing::modification_random_key(position,1,i,list_surplus_moves
										,Make_move::get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = position.hash_key;

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		Zobrist_hashing::start_position_random_key(position);

		if (test_key_control != position.hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << std::endl;
			std::cout << "collision white depth = " << depth << std::endl;
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		Zobrist_hashing::save_hash_three(position.hash_key,new_depth); 


#endif//#if SAVE_LINE 

/*
За белых.
*/
//-------------------------------------------------------------

		if (flag_pv_node && (i == 0))
		{
			flag_do_pv_search = true;
			flag_do_lmr_search = false;
			flag_do_null_window_search = false;
		}
		else
		{
			flag_do_pv_search = false;
			flag_do_null_window_search = true;
		}

// если полный поиск не заказан тогда смотрим можем ли мы использовать lmr-технику
		if (!flag_do_pv_search)
		{
			// смотрим можем ли использовать lmr
			flag_do_lmr_search =
				(i != 0)
				&&(description_move == MOVE_IS_SIMPLE)// простой ход
				&& (flag_check == CHECK_NO)
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_1(depth))
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_2(depth))
				&& ((new_depth_max - new_depth) > LMR_REDUCTION_D )
				;
		}

#if (LMR && SAVE_LINE)
		// первым реализуем  lmr-технику
		if (flag_do_lmr_search)
		{
			// ищем с редукцией глубины и с нулевым окном
			beta_null_window = alpha + 1;
			flag_pv_move = PV_NO;
			new_lmr_depth_max = new_depth_max - LMR_REDUCTION_D;

			//
			value = black_searching_min_value(position,alpha,beta_null_window,
								new_depth,new_lmr_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);
				
			// если оценка неожиданно хорошая то даем добро на персчет с нулевым окном
			// иначе не даем добро
			flag_do_null_window_search = (value > alpha);
		}
#else
		do_null_window_search = true;
#endif//#if  LMR

#if (ALPHA_BETA_ON && SAVE_LINE)
		// вторым реализуем технику поиска с нулевым окном
		if (flag_do_null_window_search)
		{
			beta_null_window = alpha + 1;
			flag_pv_move = PV_NO;

			//
			value = black_searching_min_value(position,alpha,beta_null_window,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);

			// для главного варианта пересчитываем с полным окном и глубиной
			// суть поиска с нулевым окном в том что мы не ждем что вариант улучшит 
			// альфу поэтому считаем его с максимально узким т.е. нулевым окном
			// если нас ждет сюрприз то пересчитываем c полным окном
			// здесь мы ветке возвращем статус главного варианта
			flag_do_pv_search = flag_pv_node && (value > alpha); 
		}
#else
		do_pv_search = true;
#endif//#if ALPHA_BETA_ON

		// третьим реализуем технику поиска с полным окном и глубиной
		if (flag_do_pv_search)
		{
			// если мы в главном варианте то
			// первый ход смотрим на полной глубине и с полным окном
			// мы предполагаем что если он первый то лучший и достоин полного 
			// просмотра
			// кроме того первый ход это ход из хеш-таблицы
			flag_pv_move = PV_YES;

			value = black_searching_min_value(position,alpha,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);
		}
//-------------------------------------------------------------

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
						Killer_heuristic::save_killer(i,list_surplus_moves,depth);
#endif//#if KILLER
#if SW_HISTORY
						History_heuristic::white_history_good_save(i,
							list_surplus_moves,depth,depth_max);
#endif//#if SW_HISTORY
					}


#if HASH
					// (value >= beta)  (value > alpha) 
					Transposition_table::save_best_hash_score_or_move(i
						, position.hash_key, list_surplus_moves, 
						value, BETA_CUTOFF, depth, depth_max);
#endif//#if HASH

					// TEST
					//if (( Transposition_table::get_flag_found_score() ) && ( test_flag_hash_moves == 3 ))
					//{
						//std::cout << "отсечение по хеш таблице белые " << std::endl;
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
				if (( Transposition_table::get_flag_found_score()) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "осечка отсечения по хеш таблице белые " << std::endl;
					std::cout << "depth= " << depth << std::endl;
					std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
					std::cout << "hash_depth= " << (__int32)hash_depth << std::endl;
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max << std::endl;
					std::cout << "hash_score= " << hash_score  << std::endl;
					std::cout << "value= " << value << std::endl;
					std::cout << "alpha= " << alpha << std::endl;
					std::cout << "beta= " << beta << std::endl;
					std::cout << "  " << std::endl;

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-----------------


				alpha = value;
				flag_alpha_updated = true;
				i_hash_moves_1 = i; // ход который улучшает альфу (1- улучшили альфу) 
									// нужно для кеширования 

				// здесь мы перезаписываем лучший вариант соответствующий 
				// лучшей оценке
				if (flag_pv_node)
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
// 10 обработка случая когда не было легальных ходов
//   если не одного хода сделано не было то производим статическую оценку позиции
//   это случай пата или мата

	if (flag_is_legal_moves == false)
	{
#if SAVE_LINE
		value = 0;
		if (Move_generation::king_white_check(position) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка 
		// этой позиции
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_alpha_updated == true)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать 
		// на более высокий уровень
		if (flag_pv_node)
		{
			PV_save::update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)

	//History_heuristic::white_history_bad_save(list_surplus_moves,depth,depth_max);

	if (flag_alpha_updated == true)
	{   // (value > alpha) (value < beta)
		Transposition_table::save_best_hash_score_or_move(i_hash_moves_1
			, position.hash_key, list_surplus_moves, 
			alpha, ALPHA_UPDATED, depth, depth_max);
	}
	else
	{  // (value <= alpha) (value < beta)
		Transposition_table::save_best_hash_score_or_move(i_hash_moves_3
			, position.hash_key, list_surplus_moves, 
			value_max, LOCAL_MAX, depth, depth_max);
	}

	//if (flag_alpha_updated == true)
	//{   // (value > alpha) (value < beta)
	//	Transposition_table::save_best_hash_move(i_hash_moves_1
	//		, position.hash_key, list_surplus_moves, 
	//		alpha, ALPHA_UPDATED, depth, depth_max);
	//}
	//else
	//{  // (value <= alpha) (value < beta)
	//	Transposition_table::save_best_hash_move(i_hash_moves_3
	//		, position.hash_key, list_surplus_moves, 
	//		value_max, LOCAL_MAX, depth, depth_max);
	//}

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
	if (Transposition_table::get_flag_found_score())
	//if ( 0 )
	{
		hash_moves_test13 = i_hash_moves_3;
		if (flag_save_best == 1) hash_moves_test13 = i_hash_moves_1;

		//if ( hash_moves_test13 != 0 )
		//if ( hash_score != value_max )
		if (value_max > alpha) 
		{
			std::cout << "хешированный и найденный ходы дающие максимум разные белые " << std::endl;
			std::cout << "bitboard.get_hash_key()= " << position.hash_key << std::endl;
			std::cout << "depth= " << depth << std::endl;
			std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
			std::cout << "hash_depth= " << (__int32)hash_depth << std::endl;
			std::cout << "hash_depth_max= " << (__int32)hash_depth_max << std::endl;
			std::cout << "test_flag_hash_moves= " << test_flag_hash_moves  << std::endl;
			std::cout << "hash_score= " << hash_score  << std::endl;
			std::cout << "value_max= " << value_max << std::endl;
			std::cout << "alpha= " << alpha << std::endl;
			std::cout << "beta= " << beta << std::endl;

			std::cout << "ход из хеш таблицы " << std::endl;
			std::cout << "((list_surplus_moves.move[0]>>6)& 63)= " 
				<< ((list_surplus_moves.move[0]>>6)& 63) << std::endl;

			std::cout << "Ходящая фигура " 
				<< Uci_engine_to_gui::int_to_char_board(((list_surplus_moves.move[0]>>24)& 7),1)
				<< std::endl;
               
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[0]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>12)& 63)/8);
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[0]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>18)& 63)/8) << std::endl;

			std::cout << "ход найденный в поиске " << std::endl;
			std::cout << "hash_moves_test12= " << hash_moves_test13  << std::endl;
			std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				<< ((list_surplus_moves.move[hash_moves_test13]>>6)& 63) << std::endl;
			std::cout << "Ходящая фигура " 
				<< Uci_engine_to_gui::int_to_char_board(((
					list_surplus_moves.move[hash_moves_test13]>>24)& 7),1)<< std::endl;
               
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>12)& 63)/8);
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>18)& 63)/8) << std::endl;
			std::cout << "  " << std::endl; 

			exit(1);
		}
	}
#endif //#if HASH_TEST_SCORE
//-----------------

	return value_max;
}



/*
 ищем минимум в узле. перебор за черных
*/
__int32 Search::black_searching_min_value
(
	struct Position & position,// представление доски
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth,// глубина
	const unsigned __int8 depth_max,// максимальная глубина
	const bool flag_pv_node,// основной ли это вариант (pv)

	const __int32 old_description_move,// тип хода который привел к данной позиции
	const bool flag_old_check,//флаг шаха
	const bool flag_null_m// флаг нулевого хода
)
{
	struct List list_surplus_moves;// список возможных ходов 
								   //(всевозможные ходы из данного узла)
	__int32 value;              // текущая оценка позиции
	__int32 value_min = INFINITE_SCORE;   // оценка позиции лучшая в узле 
	const unsigned __int8 new_depth = depth + 1; // следующая глубина
	unsigned __int8 new_depth_max;//новая глубина перебора

	// переменные параметры в поиске LMR и PVS
	__int32 alpha_null_window;//
	unsigned __int8 new_lmr_depth_max;// максимальная глубина
	bool flag_pv_move;// основной ли это вариант (pv)

	bool flag_do_pv_search;
	bool flag_do_null_window_search;
	bool flag_do_lmr_search;

	bool flag_is_legal_moves = false;           // флаг существования легальных ходов
	bool flag_beta_updated = false;           // пишем ли лучший вариант
	bool flag_check = false;//флаг шаха


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

	//unsigned __int64 test_key_control;// запоминаем хеш-ключ даннного узла

#endif//#if SAVE_LINE 

#if (HASH_SCORE && SAVE_LINE)
	// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 
	// 2-отсечка по бете(альфе))
	unsigned __int8 test_flag_hash_moves;
	unsigned __int8 hash_depth;// максимальная глубина
	unsigned __int8 hash_depth_max;// максимальная глубина
#endif//#if (HASH_SCORE)


//----------------------------------------------------------
// 1 отсечка по команде на остановку счета
// только у белых

//----------------------------------------------------------
// 2 отсечка по повторению позиции
#if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
	// обнаружили повторение позиции
	if (Zobrist_hashing::analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(DRAWN_SCORE // оценка ничья
														,depth);
		}

		return 0;
	}
#endif//#if ALPHA_BETA_ON

//----------------------------------------------------------
// 3 отсечка по хеш-оценке
#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// отсечку по хеш оценке не используем для главного варианта
	// главным образом для достоверности оценки и линии варианта и для полноты картины
	// считаю обрывочный главный вариант неприемлемым.
	if (flag_pv_node == PV_NO)
	{ 
		// смотрим не встречалась ли нам такая позиция если встречалась то 
		// пытаемся применить сохраненную оценку
		hash_score = Transposition_table::look_hash_score_black(
			position.hash_key,depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (Transposition_table::get_flag_found_score())
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
// 4 отсечка по максимальной глубине (оценка позиции)
// на предельной глубине производим статическую оценку позиции
	if (depth >= depth_max)
	{
#if SAVE_LINE

	//  форсировка
	//  подключаем форсированный вариант
#if FORCED 
		// включаем режим форсировки(просчета только взятий)
		value = Search_quiescence::forced_variant_black(position,alpha,beta
													,depth,flag_pv_node);
#endif//#if FORCED 


	// включаем оценку без форсировки
#if !FORCED
		value = max_depth_black(position,alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

//----------------------------------------------------------
// 5 6 отсечка по razoring и futility
#if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING ))

	if (flag_pv_node == PV_NO)
	{ 

		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
		//&& (description_move_up != MOVE_IS_CAPTURE) // если не взятие
		&& (flag_old_check == CHECK_NO)) // если шаха нет
		{
			
				value = position.value_white_material 
					- position.value_black_material
					+ Evaluate::eval_state(position);
#if RAZORING
			//razoring 

			//--
			__int32 alpha_q_margin =  alpha + RAZORING_MARGIN[(depth_max - depth)];

				if(value > alpha_q_margin)
				{

					value = Search_quiescence::forced_variant_black(position,
								alpha_q_margin,(alpha_q_margin + 1),depth,flag_pv_node);

					if (value > alpha_q_margin)
					{
						return value; 
					}
				}
			//--
#endif// #if RAZORING


#if FUTILITY

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
			if (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
			{
				//--
				__int32 alpha_margin =  alpha - FUTILITY_MARGIN[(depth_max - depth)];

				if (value <= alpha_margin)
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
// 7 отсечка по нулевому ходу
#if (NULL_MOVE && SAVE_LINE)
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	value = black_null_move(position,alpha,beta,depth_max,depth,old_description_move,
							flag_pv_node,flag_old_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE

//----------------------------------------------------------
// 8 генерируем и сортируем список избыточных ходов

	//Move_generation::mobility_evaluation(position);

	// насчитываем список избыточных ходов
	// множество этих списков и создают дерево перебора
	Move_generation::generator_captures_black(list_surplus_moves,position);
	Move_generation::generator_move_black(list_surplus_moves,position);
	Move_ordering::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Move_ordering::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY


#if (KILLER && SAVE_LINE)
	Killer_heuristic::insert_killer(list_surplus_moves,depth,list_surplus_moves.end_captures);
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась 
	// то ее ход смотрим первым
	Transposition_table::look_hash_move(list_surplus_moves
						,position.hash_key,depth,flag_insert_hash_move);

// для полного теста нужно отключать отсечку по оценке
#if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

	if (flag_insert_hash_move == 1)
	{
		std::cout << "коллизия хеша черные" << std::endl;
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH

//----------------------------------------------------------
// 9 бежим по списку избыточных ходов
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
		if (Move_generation::king_black_check(position)!= CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			Make_move::undo_moves_black(position,i,depth,list_surplus_moves);

			// идем дальше по списку 
			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = true;
		nodes = nodes + 1;

		new_depth_max = depth_max;

#if (EXTENSION_CHEK && SAVE_LINE)
		// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
		if(Move_generation::king_white_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES;
			new_depth_max = depth_max + 1;
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
		if (flag_pv_node)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		Zobrist_hashing::modification_random_key(
			position,0,i,list_surplus_moves,Make_move::get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = position.hash_key;

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		Zobrist_hashing::start_position_random_key(position);

		if (test_key_control != position.hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << std::endl;
			std::cout << "collision black depth = " << depth << std::endl;
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		Zobrist_hashing::save_hash_three(position.hash_key,new_depth); 

#endif//#if SAVE_LINE 


//-------------------------------------------------------------

// если мы в главном узле(pv) то первый ход ищем полным поиском
		if (flag_pv_node && (i == 0))
		{
			flag_do_pv_search = true;
			flag_do_lmr_search = false;
			flag_do_null_window_search = false;
		}
		else
		{
			flag_do_pv_search = false;
			flag_do_null_window_search = true;
		}

// если полный поиск не заказан тогда смотрим можем ли мы использовать lmr-технику
		if (!flag_do_pv_search)
		{
			// смотрим можем ли использовать lmr
			flag_do_lmr_search =
				(i != 0)
				&&(description_move == MOVE_IS_SIMPLE)// простой ход
				&& (flag_check == CHECK_NO)
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_1(depth))
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_2(depth))
				&& ((new_depth_max - new_depth) > LMR_REDUCTION_D )
				;
		}

#if (LMR && SAVE_LINE)
		// первым реализуем  lmr-технику
		if (flag_do_lmr_search)
		{
			alpha_null_window = beta - 1;//
			flag_pv_move = PV_NO;//
			new_lmr_depth_max = new_depth_max - LMR_REDUCTION_D;// максимальная глубина

			value = white_searching_max_value(position,alpha_null_window,beta,
								new_depth,new_lmr_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);

			// если оценка неожиданно хорошая то даем добро на персчет с нулевым окном
			// иначе не даем добро
			flag_do_null_window_search = (value < beta);
		}
#else
		do_null_window_search = true;
#endif//#if  LMR

#if (ALPHA_BETA_ON && SAVE_LINE)
		// вторым реализуем технику поиска с нулевым окном
		if (flag_do_null_window_search)
		{
			alpha_null_window = beta - 1;//
			flag_pv_move = PV_NO;//

			value = white_searching_max_value(position,alpha_null_window,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);

			// если оценка неожиданно хорошая и мы в главном узле
			// то даем добро на персчет c полным окном и глубиной
			// иначе не даем добро
			flag_do_pv_search = flag_pv_node && (value < beta);
		}
#else
		do_pv_search = true;
#endif//#if ALPHA_BETA_ON

		// третьим реализуем технику поиска с полным окном и глубиной
		if (flag_do_pv_search)
		{
			flag_pv_move = PV_YES;// основной ли это вариант (pv)

			value = white_searching_max_value(position,alpha,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);
		}
//-------------------------------------------------------------

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
						Killer_heuristic::save_killer(i,list_surplus_moves,depth);
#endif//#if KILLER
#if SW_HISTORY
						History_heuristic::black_history_good_save(
							i,list_surplus_moves,depth,depth_max);
#endif//#if SW_HISTORY
					}


#if HASH
					//(value <= alpha)
					Transposition_table::save_best_hash_score_or_move(i, position.hash_key
													  ,list_surplus_moves, 
													  value, ALPHA_CUTOFF, depth, depth_max);	
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
				if (( Transposition_table::get_flag_found_score() ) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "осечка отсечения по хеш таблице черные" << std::endl;
					std::cout << "depth= " << depth << std::endl;
					std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
					std::cout << "hash_depth= " << (__int32)hash_depth << std::endl;
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max 
						<< std::endl;

					std::cout << "hash_score= " << hash_score  << std::endl;
					std::cout << "value= " << value << std::endl;
					std::cout << "alpha= " << alpha << std::endl;
					std::cout << "beta= " << beta << std::endl;
					std::cout << "  " << std::endl;

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-------------------

				beta = value;
				flag_beta_updated = true;
				i_hash_moves_1 = i; //ход который улучшает бету (1- улучшили бету) 
									//нужно для кеширования 

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				if (flag_pv_node)
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
		if (Move_generation::king_black_check(position) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_beta_updated == true)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на 
		// более высокий уровень
		if (flag_pv_node)
		{
			PV_save::update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)

	//History_heuristic::black_history_bad_save(list_surplus_moves,depth,depth_max);

	if ( flag_beta_updated == true ) 
	{   // (value < beta) и (value > alpha)
		Transposition_table::save_best_hash_score_or_move(
			i_hash_moves_1,position.hash_key,list_surplus_moves, 
			beta, BETA_UPDATED, depth, depth_max);
	}
	else
	{	// (value >= beta) и (value > alpha)
		Transposition_table::save_best_hash_score_or_move(
			i_hash_moves_3,position.hash_key,list_surplus_moves, 
			value_min, LOCAL_MIN, depth, depth_max);
	}

	//if ( flag_beta_updated == true ) 
	//{   // (value < beta) и (value > alpha)
	//	Transposition_table::save_best_hash_move(
	//		i_hash_moves_1,position.hash_key,list_surplus_moves, 
	//		beta, BETA_UPDATED, depth, depth_max);
	//}
	//else
	//{	// (value >= beta) и (value > alpha)
	//	Transposition_table::save_best_hash_move(
	//		i_hash_moves_3,position.hash_key,list_surplus_moves, 
	//		value_min, LOCAL_MIN, depth, depth_max);
	//}

#endif//#if HASH

	return value_min;
}

/*
 очень эффективная эвристика известная под именем нулевого хода
 я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
 суть его в том что
 мы пропускаем свой ход и если даже при такой форе противник не находит
 лучшего хода эту ветку можно подрезать
 за белых
*/
inline __int32 Search::white_null_move
(
	struct Position & position,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 old_description_move,// тип хода который привел к данной позиции
	const bool flag_pv_node,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m,// флаг нулевого хода
    const unsigned __int8 new_depth,// следующая глубина
	const unsigned __int64 key_undo
)
{

	if ((flag_pv_node == PV_NO) // если мы не в главном варианте
	  &&(flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	  &&((depth_max - new_depth) > NULL_REDUCTION ) // если до максимальной глубины 
											   // расстояние достаточное
	&& (old_description_move != MOVE_IS_CAPTURE) // если ход не взятие
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
		//Zobrist_hashing::start_position_random_key(bitboard);

		// не динамическая редукция
		__int32 value = black_searching_min_value(position,alpha,(alpha + 1),
								new_depth,(depth_max - NULL_REDUCTION),PV_NO,
								old_description_move,old_flag_check,NULL_YES);

		//--------------------------------------------------------
		// динамическая редукция нулевого хода при дистанции до максимальной глубины
		// больше равной 5 редуцируем 4 иначе 3
		//unsigned __int8 dynamic_null_move_reduction = 3 +
		//	( depth >= SWITCH_NULL_REDUCTION_1_2 
		//	? depth / 4 : 0); // 

		//dynamic_null_move_reduction = (dynamic_null_move_reduction >= (depth_max - depth))
		//	? (depth_max - depth - 1) : dynamic_null_move_reduction;
		////
		//__int32 value = black_searching_min_value(position,alpha,(alpha + 1),
		//						new_depth,(depth_max - dynamic_null_move_reduction),PV_NO,
		//						old_description_move,old_flag_check,NULL_YES);
		//--------------------------------------------------------

		// узлы насчитанные в нулевом поиске не учитываем
		nodes = nodes_null_rem;

		// TEST
		//std::cout << "bitboard- "<< bitboard- << std::endl;
		//std::cout << "i_move= "<< i_move << std::endl;
		//std::cout << "description_move= "<< description_move << std::endl;
		//std::cout << "check= "<< check << std::endl;
		//std::cout << "       "<< std::endl;
		//
		//if (value < beta)
		//{
		//	std::cout << "осталось "<< std::endl;
		//	std::cout << "depth_max= "<< depth_max <<std::endl;
		//	std::cout << "depth= "<< depth <<std::endl;
		//	std::cout << "value= "<< value <<std::endl;
		//	std::cout << "beta= "<< beta <<std::endl;
		//	std::cout << "       "<< std::endl;
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
			//std::cout << "отсекли "<< std::endl;
			//std::cout << "depth_max= "<< depth_max <<std::endl;
			//std::cout << "depth= "<< depth <<std::endl;
			//std::cout << "value= "<< value <<std::endl;
			//std::cout << "beta= "<< beta <<std::endl;
			//std::cout << "       "<< std::endl;
			//return beta;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return beta;
			}

			// пробуем обойти цугцванг
			value = white_searching_max_value(position,(beta - 1),beta,
								depth,(depth_max - VERIFICATOIN_SEARCH_REDUCTION)
								,PV_NO,old_description_move,old_flag_check,NULL_YES);

			if (value >= beta)
			{
				return beta;
			}
		}
	}

	return -1000000000;
}

/*
 мы пропускаем свой ход и если даже при такой форе противник не находит
 лучшего хода эту ветку можно подрезать
 за черных
*/
inline __int32 Search::black_null_move
(
	struct Position & position,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// максимальная глубина
	const unsigned __int8 depth,// глубина
	const __int32 old_description_move,// тип хода который привел к данной позиции
	const bool flag_pv_node,// основной ли это вариант (pv)
	const bool old_flag_check,//флаг шаха
	const bool flag_null_m,// флаг нулевого хода
    const unsigned __int8 new_depth,// следующая глубина
	const unsigned __int64 key_undo
)
{

	if ((flag_pv_node == PV_NO) // если мы не в главном варианте
	  &&(flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	  &&((depth_max - new_depth) > NULL_REDUCTION ) // если до максимальной глубины 
											   // расстояние достаточное
	&& (old_description_move != MOVE_IS_CAPTURE) // если ход не взятие
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
		//Zobrist_hashing::start_position_random_key(bitboard);

		__int32 value = white_searching_max_value(position,(beta - 1),beta,
								new_depth,(depth_max - NULL_REDUCTION)
								,PV_NO,old_description_move,old_flag_check,NULL_YES);
			
 
		//--------------------------------------------------------
		//unsigned __int8 dynamic_null_move_reduction = 3 +
		//	( depth >= SWITCH_NULL_REDUCTION_1_2 ? depth / 4 : 0); // 

		//dynamic_null_move_reduction = (dynamic_null_move_reduction >= (depth_max - depth))
		//	? (depth_max - depth - 1) : dynamic_null_move_reduction;

		//__int32 value = white_searching_max_value(position,(beta - 1),beta,
		//						new_depth,(depth_max - dynamic_null_move_reduction)
		//						,PV_NO,old_description_move,old_flag_check,NULL_YES);

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
			//std::cout << "отсекли "<< std::endl;
			//std::cout << "depth_max= "<< depth_max <<std::endl;
			//std::cout << "depth= "<< depth <<std::endl;
			//std::cout << "value= "<< value <<std::endl;
			//std::cout << "alpha= "<< alpha <<std::endl;
			//std::cout << "       "<< std::endl;

			//return alpha;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return alpha;
			}

			// пробуем обойти цугцванг
			value = black_searching_min_value(position,alpha,(alpha + 1),
								depth,(depth_max - VERIFICATOIN_SEARCH_REDUCTION)
								,PV_NO,old_description_move,old_flag_check,NULL_YES);

			if (value <= alpha)
			{
				return alpha;
			}
		}
	}

	return -1000000000;
}


/*
 производим расчеты на максимальной глубине. за белых
*/
inline __int32 Search::max_depth_white
(
	struct Position & position,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_node,// основной ли это вариант (pv)
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
	//	std::cout << "WHITE  Eval_forced(bitboard) != bitboard-" <<std::endl;
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<std::endl;
	//	std::cout << "bitboard-= "<< bitboard-<<std::endl;
	//}
	//--------------------------------------------------------------------------

	//у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
	//надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
	if (flag_pv_node)
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

/*
 производим расчеты на максимальной глубине. за черных
*/
inline __int32 Search::max_depth_black
(
	struct Position & position,// представление доски
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_node,// основной ли это вариант (pv)
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
	//	std::cout << "BLACK  Eval_forced(bitboard) != bitboard-" <<std::endl;
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<std::endl;
	//	std::cout << "bitboard-= "<< bitboard-<<std::endl;
	//}
	//--------------------------------------------------------------------------

	//у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
	//надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
	if (flag_pv_node)
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


/*
 времени больше нет. экстренный выход
*/
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
				std::cout << "Ifrit: quit command. I am terminated." << std::endl;
				Transposition_table::hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// остановка поиска
				flag_stop_search = 1;
			}
		}

		//экстренный выход с глубины. просрочили время на поиск
		Time_management::time_stop();
		delta_time_current = Time_management::time_delta();

		//выходим по времени
		if ((deep_time_terminate > 0) && (deep_time_terminate < delta_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;

}

/*

*/
void Search::set_depth_max_rem
(
	unsigned __int8 _depth_max_rem//
)
{
	depth_max_rem = _depth_max_rem;
}

/*

*/
void Search::ini_deep_time_terminate
(
	double _deep_time_terminate//
)
{
	deep_time_terminate = _deep_time_terminate;
}

/*

*/
void Search::ini_global_loop()
{
	audit = 0;// задержка опроса команды экстренного выхода
	flag_stop_search = 0;//
}

/*
 возвращаем флаг остановки поиска. так если в глубине поиска получили команду
 на остановку то передаем ее наружу
*/
bool Search::get_stop_search()
{
	return flag_stop_search;
}

/*
 возвращаем количество рассмотренных узлов
*/
__int64  Search::get_nodes()
{
	return nodes;// количество узлов;
}

/*
 обнуляем количество рассмотренных узлов
*/
void Search::set_nodes_in_0()
{
	nodes = 0;// количество узлов;
}


