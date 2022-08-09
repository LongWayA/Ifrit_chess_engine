#include <iostream> // TEST
#include <fstream>  // TEST
//#include <cstring>    // используем для работы со строкой (сравнение строк)
#include <stdlib.h>   // обеспечиваем завершение потока функцией exit(1)


#include "k_Util.h"
#include "e_Move.h"
#include "h_Evaluate.h"
#include "i_Quiescence_search.h"
#include "k_Win_32.h"
#include "g_Search.h"
#include "j_Hash.h"
#include "j_Killer.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"
#include "j_ZObrist.h"
#include "g_Search_switch.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009
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


namespace 
{
	// NULL MOVE
	const unsigned __int8 NULL_YES = 1; // мы внутри нулевого хода 
	const unsigned __int8 NULL_NO = 0; // мы снаружи нулевого хода 
	const unsigned __int8 NULL_DEPTH = 2; // дистанция до дна меньше которой NULL не вызывается
	const unsigned __int8 SWITCH_NULL_REDUCTION_1_2 = 5; // глубина на которой меняется редукция нулевого хода 
										// с NULL_REDUCTION_ONE на NULL_REDUCTION_TWO
	const unsigned __int8 NULL_REDUCTION = 4; // 4 сокращение глубины перебора для NULL
	const unsigned __int8 NULL_REDUCTION_ONE = 4; // 4 сокращение глубины перебора для динамического NULL
	const unsigned __int8 NULL_REDUCTION_TWO = 3; // 3 сокращение глубины перебора для динамического NULL
	const unsigned __int8 VERIFICATOIN_SEARCH_REDUCTION = 5; // сокращение глубины перебора 
											//для поиска по обнаружению цугцванга
	// LMR
	const unsigned __int8 LMR_DEPTH = 3; // дистанция до дна меньше которой LMR не вызывается
	const unsigned __int8 LMR_REDUCTION_D = 1; // сокращение глубины перебора для LMR
	const unsigned __int8 FULL_SEARCH = 2;  // было 2
											// число веток с полным поиском без сокращений 
											// ход из хеш-таблицы, лучшее взятие,
											// два киллера, все взятия не редуцируются
											// хеш-ход два киллера это 3 и минус один потому что с нуля 
											// надо еще помнить что условие редукции i > lmr_reduction

	// константы
	const unsigned __int8 CHECK_YES = 1; // есть шах
	const unsigned __int8 CHECK_NO = 0; // нет шаха


	const unsigned __int8 MOVE_IS_CAPTURE = 2; // ход это взятие
	const unsigned __int8 CAPTURE_NO = 0; // нет взятий

	const unsigned __int8 PV_YES = 1; // основной вариант
	const unsigned __int8 PV_NO = 0; // 

	const  __int32 VALUE_MATE = 1000000; // оценка мата
	const  __int16 DELTA_MATE = 1000; // дельта для нахождения скорейшего мата

	//-------------------------------------------------------------------------

	unsigned __int16 audit;// задержка опроса команды экстренного выхода
	unsigned __int8 depth_max_rem;//
	bool flag_stop_search;// переменная для отсечки перебора при экстренном выходе
	__int64 nodes;// количество узлов
	char string_in2[65536];//
	unsigned __int8 from;// откуда ходит фигура
	unsigned __int8 to;// куда ходит фигура
	bool flag_stop_null_move;// останавливаем использование нулевого хода
	//int return_hash_score;

	double deep_time_terminate; // передаем на глубину время остановки счета 
}


//	===================================================================
// ищем максимум в узле. перебор за белых 
__int32 FULLSEARCH::public_white
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
	struct T_undo undo[1]; // использую при возврате хода
	struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	bool flag_is_legal_moves = 0;           // флаг существования легальных ходов
	bool flag_save_best = 0;           // пишем ли лучший вариант
	const unsigned __int8 new_depth = depth + 1; // следующая глубина
	bool flag_check = 0;//флаг шаха

//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//новая глубина перебора
 
	struct T_PV pv_best_point;// лучший для данного узла вариант

	  
	// все для кеширования
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
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
	if (ZOBRIST::public_analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(0 // оценка ничья
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
		hash_score = HASHM::public_look_hash_score_white(
			p_bitboard->hash_key,depth,depth_max,
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
	BITBOARDGENERATOR::public_generator_captures_white(p_bitboard,&list_surplus_moves);
	BITBOARDGENERATOR::public_generator_move_white(p_bitboard,&list_surplus_moves);
	sorting_moves(&list_surplus_moves);

 
#if (KILLER && SAVE_LINE)
	// если нет взятий или взятие плохое
	// то киллер вставляем на первую позицию иначе на вторую
	if ((list_surplus_moves.end_captures == CAPTURE_NO) 
	|| (list_surplus_moves.sorting_score[0] < 0))
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,0);
	}
	else
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// точнее мы поднимаем ход из хеш таблицы на первое место сдвигая остальные вниз
	HASHM::public_look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

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
	lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


	//бежим по списку избыточных ходов
	// старт и конец цикла должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

		// реализуем ход из списка другими словами генерируем позицию
		MOVES::public_onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);

		// если король под шахом то 
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

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
			PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);
		}

		ZOBRIST::public_modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = p_bitboard->hash_key;

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		ZOBRIST::public_start_position_random_key(p_bitboard);

		if (test_key_control != p_bitboard->hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision white depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth); 

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
		LMR_white(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check,lmr_reduction);
#endif//#if  LMR

		// восстанавливаем позицию
		MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

#if SAVE_LINE
		// восстанавливаем хеш-ключ доски
		p_bitboard->hash_key = key_undo;

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
					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
						SKILLER::public_save_killer(i,&list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// кэшируем лучший на данный момент ход
					HASHM::public_save_best_hash_score_or_move(i
						, p_bitboard->hash_key, &list_surplus_moves, 
						value, HASHM::LOWER, depth, depth_max);
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
					PVSAVE::public_update_PV_best_point(&pv_best_point);
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
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(value,depth);
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
			PVSAVE::public_update_PV_best(&pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)
	if (flag_save_best == 1)
	{
		// кэшируем ход улучшающий альфу т.е. флаг 1
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_1
			, p_bitboard->hash_key, &list_surplus_moves, 
			alpha, HASHM::EXACT, depth, depth_max);
	}
	else
	{
		// кэшируем ход дающий локальный максимум т.е. флаг 3
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_3
			, p_bitboard->hash_key, &list_surplus_moves, 
			value_max, HASHM::UPPER, depth, depth_max);
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
			std::cout << "p_bitboard->hash_key= " << p_bitboard->hash_key << "\n";
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
				<< UTIL::public_int_to_char_board(((list_surplus_moves.move[0]>>24)& 7),1)
				<< "\n";
               
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[0]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>12)& 63)/8);
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[0]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>18)& 63)/8) << "\n";

			std::cout << "ход найденный в поиске " << "\n";
			std::cout << "hash_moves_test12= " << hash_moves_test13  << "\n";
			std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				<< ((list_surplus_moves.move[hash_moves_test13]>>6)& 63) << "\n";
			std::cout << "Ходящая фигура " 
				<< UTIL::public_int_to_char_board(((
					list_surplus_moves.move[hash_moves_test13]>>24)& 7),1)<< "\n";
               
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>12)& 63)/8);
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>18)& 63)%8);
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
__int32 FULLSEARCH::public_black
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
	struct T_undo undo[1];
	struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	bool flag_is_legal_moves = 0;           // флаг существования легальных ходов
	bool flag_save_best = 0;           // пишем ли лучший вариант
	const unsigned __int8 new_depth = depth + 1; // следующая глубина
	bool flag_check = 0;//флаг шаха


#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//новая глубина перебора
 
	struct T_PV pv_best_point;// лучший для данного узла вариант
	bool taking_passage_yes_mem = 0;
	unsigned __int8 taking_passage_y_mem = 0;
	unsigned __int8 taking_passage_x_mem = 0;

	unsigned __int8 flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
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
	if (ZOBRIST::public_analysis_hash_string_moves(depth))
	{
		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(0 // оценка ничья
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
		hash_score = HASHM::public_look_hash_score_black(
			p_bitboard->hash_key,depth,depth_max,
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
	BITBOARDGENERATOR::public_generator_captures_black(p_bitboard,&list_surplus_moves);
	BITBOARDGENERATOR::public_generator_move_black(p_bitboard,&list_surplus_moves);
	sorting_moves(&list_surplus_moves);


#if (KILLER && SAVE_LINE)
	// если нет взятий или взятие проигрывающее
	// то киллер вставляем на первую позицию иначе на вторую
	if ((list_surplus_moves.end_captures == CAPTURE_NO)
	|| (list_surplus_moves.sorting_score[0] < 0))
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,0);
	}
	else
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	HASHM::public_look_hash_move(&list_surplus_moves
		,p_bitboard->hash_key,depth,flag_insert_hash_move);

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
	lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


	// бежим по списку избыточных ходов
	// старт и конец цикла; должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

		// реализуем ход из списка другими словами генерируем позицию
		MOVES::public_onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);

		// если король по шахом то
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard)!= CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

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
			PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);
		}

		ZOBRIST::public_modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = p_bitboard->hash_key;

		//тут ключ считается полностью по новой
		//тестировал хеш-таблицу
		//инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		ZOBRIST::public_start_position_random_key(p_bitboard);

		if (test_key_control != p_bitboard->hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision black depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// записываем ключ для последующего анализа на повтор позиций
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth); 

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
		LMR_black(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check,lmr_reduction);
#endif//#if  LMR

		// восстанавливаем позицию
		MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

#if SAVE_LINE
		// восстанавливаем хеш-ключ доски
		p_bitboard->hash_key = key_undo;

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
					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
						SKILLER::public_save_killer(i,&list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// кэшируем лучший на данный момент ход
					HASHM::public_save_best_hash_score_or_move(i, p_bitboard->hash_key, &list_surplus_moves, 
						value, HASHM::LOWER, depth, depth_max);	
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
					PVSAVE::public_update_PV_best_point(&pv_best_point);
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
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(value,depth);
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
			PVSAVE::public_update_PV_best(&pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	if ( flag_save_best == 1 ) 
	{
		// кэшируем ход улучшающий бету т.е. флаг 1
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_1, p_bitboard->hash_key, &list_surplus_moves, 
			beta, HASHM::EXACT, depth, depth_max);
	}
	else
	{
		// кэшируем ход дающий локальный минимум т.е. флаг 3
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_3, p_bitboard->hash_key, &list_surplus_moves, 
			value_min, HASHM::UPPER, depth, depth_max);
	}
#endif//#if HASH

	return value_min;
}

//	===================================================================

// за белых
inline void FULLSEARCH::PVS_white
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
    const struct T_list_surplus_moves list_surplus_moves,
	const bool flag_check
)
{

#if (ALPHA_BETA_ON && SAVE_LINE)
	if (i == 0)
#else
	if (1)
#endif//#if ALPHA_BETA_ON
	{
		value = public_black(p_bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),
			flag_pv_f,flag_check,flag_null_m);
	}
	else
	{
		// рекурсивный вызов функции
		value = public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,

		// TEST
		//value = Black(p_bitboard,alpha,beta,depth_max_new,

		new_depth,((list_surplus_moves.move[i]>>6) & 63),
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

			value = public_black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				flag_pv_f,flag_check,flag_null_m);

			//std::cout << "value mod = " << value << "\n";
			//std::cout << "   " <<"\n";

		}
	}
}


//	===================================================================

//  за черных
inline void FULLSEARCH::PVS_black
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
    const struct T_list_surplus_moves list_surplus_moves,
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
		value = public_white(p_bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
			flag_check,flag_null_m);

	}
	else
	{
		value = public_white(p_bitboard,(beta - 1),beta,depth_max_new,

		// TEST
		//value = White(p_bitboard,alpha,beta,depth_max_new,

			new_depth,((list_surplus_moves.move[i]>>6) & 63),
			PV_NO,flag_check,flag_null_m);

		if ((value > alpha) && (value < beta))
		{
			value = public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6)& 63),
				flag_pv_f,flag_check,flag_null_m);

		}
	}
}

//	===================================================================

// за белых
inline void FULLSEARCH::LMR_white
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
    const struct T_list_surplus_moves list_surplus_moves,
	const bool flag_check,
	const unsigned __int8 lmr_reduction
)
{
	if (flag_pv_f)
	{
		// если мы в главном варианте то считаем по методике с нулевым окном
		// т.е. от alpha до beta = alpha + 1

		if (i == 0) 
		{
			// первый ход смотрим на полной глубине и с полным окном
			// мы предполагаем что если он первый то лучший и достоин полного просмотра
			value = public_black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			// пытаемся обойтись перебором с нулевым окном
			value = public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			// суть поиска с нулевым окном в том что мы не ждем что вариант улучшит 
			// альфу поэтому считаем его с максимально узким т.е. нулевым окном
			//  если нас ждет сюрприз то пересчитываем c полным окном
			if (value > alpha)
			{
				//
				value = public_black(p_bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.move[i]>>6) & 63),
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
			value = public_black(p_bitboard,alpha,(alpha + 1),(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
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
			value = public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}

	}
}


//	===================================================================

//  за черных
inline void FULLSEARCH::LMR_black
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
    const struct T_list_surplus_moves list_surplus_moves,
	const bool flag_check,
	const unsigned __int8 lmr_reduction
)
{
	if(flag_pv_f)
	{
		// если мы в главном варианте то считаем по методике с нулевым окном

		if (i == 0)
		{
			//
			value = public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			value = public_white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			if (value < beta)
			{
				value = public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6)& 63),
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
			value = public_white(p_bitboard,(beta - 1),beta,(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}
		else
		{
			value = beta - 1;
		}

		if (value < beta)
		{
			value = public_white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6)& 63),
				PV_NO,flag_check,flag_null_m);
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
inline __int32 FULLSEARCH::null_move_white
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
		bool taking_passage_yes_mem = p_bitboard->taking_passage_yes;
		unsigned __int8 taking_passage_x_mem = p_bitboard->taking_passage_x;
		unsigned __int8 taking_passage_y_mem = p_bitboard->taking_passage_y;

		// меняем хеш-ключ под нулевой ход -------------------------
		// учитываем взятие на проходе
		if (p_bitboard->taking_passage_yes)
		{
			p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));

			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
		}

		// учитываем цвет хода
		// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
		// некоторые позиции отличаются только цветом хода
		p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);

		p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

		p_bitboard->colour_move = !p_bitboard->colour_move;

		const __int64 nodes_null_rem = nodes;

		// TEST тестировал изменение хеш-ключа
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//HASHM::public_start_position_random_key(p_bitboard);

		// не динамическая редукция
		__int32 value = public_black(p_bitboard,alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
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

		p_bitboard->taking_passage_yes = taking_passage_yes_mem;
		p_bitboard->taking_passage_x = taking_passage_x_mem ;
		p_bitboard->taking_passage_y = taking_passage_y_mem ;

		p_bitboard->colour_move = !p_bitboard->colour_move;

		// восстанавливаем хеш-ключ доски
		p_bitboard->hash_key = key_undo;

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
			value = public_white(p_bitboard,(beta - 1),beta
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
inline __int32 FULLSEARCH::null_move_black
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
		bool taking_passage_yes_mem = p_bitboard->taking_passage_yes;
		unsigned __int8 taking_passage_x_mem = p_bitboard->taking_passage_x;
		unsigned __int8 taking_passage_y_mem = p_bitboard->taking_passage_y;

		// меняем хеш-ключ под нулевой ход -------------------------
		// учитываем взятие на проходе
		if (p_bitboard->taking_passage_yes)
		{
			p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));

			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
		}

		// учитываем цвет хода
		// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
		// некоторые позиции отличаются только цветом хода
		p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);

		//---------------------------------------------
		p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

		p_bitboard->colour_move = !p_bitboard->colour_move;
	
		const __int64 nodes_null_rem = nodes;

		// TEST тестировал изменение хеш-ключа
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//HASHM::public_start_position_random_key(p_bitboard);

		__int32 value = public_white(p_bitboard,(beta - 1),beta,(depth_max - NULL_REDUCTION),
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

		p_bitboard->taking_passage_yes = taking_passage_yes_mem;
		p_bitboard->taking_passage_x = taking_passage_x_mem ;
		p_bitboard->taking_passage_y = taking_passage_y_mem ;

		p_bitboard->colour_move = !p_bitboard->colour_move;

		// восстанавливаем хеш-ключ доски
		p_bitboard->hash_key = key_undo;


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
			value = public_black(p_bitboard,alpha,(alpha + 1)
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
inline __int32 FULLSEARCH::max_depth_white
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
				HASHM::public_hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// остановка поиска
				flag_stop_search = 1;
			}
		}

		//экстренный выход с глубины. просрочили время на поиск
		TIMER::public_time_stop();
		deep_time_current = TIMER::public_time_delta();

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
	score = QUIESCENCESEARCH::public_forced_variant_white(p_bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;
#endif//#if FORCED


	// включаем оценку без форсировки(просчета только взятий)
#if !FORCED 
	score = EVALUATE::public_eval_state(p_bitboard) + (p_bitboard->value_white_material - p_bitboard->value_black_material);
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
inline __int32 FULLSEARCH::max_depth_black
(
	struct T_Bitboard * const p_bitboard,// представление доски
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
			//char string_in2[65536];
			std::cin.getline(string_in2,LENGTH_STRING, '\n');

			if (strcmp(string_in2, "quit") == 0)
			{
				HASHM::public_hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// остановка поиска
				flag_stop_search = 1;
			}
		}

		// экстренный выход с глубины. просрочили время на поиск
		TIMER::public_time_stop();
		deep_time_current = TIMER::public_time_delta();

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

	score = QUIESCENCESEARCH::public_forced_variant_black(p_bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;

#endif//#if FORCED 


	// включаем оценку без форсировки(просчета только взятий)
#if !FORCED
	score = EVALUATE::public_eval_state(p_bitboard) + (p_bitboard->value_white_material - p_bitboard->value_black_material);
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
inline void FULLSEARCH::extension_for_chek_white
(
	const struct T_Bitboard * const p_bitboard,// представление доски
	const unsigned __int8 depth_max,// максимальная глубина
	unsigned __int8 & depth_max_new,// новая максимальная глубина
	bool & flag_check// флаг шаха
)
{
	// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
	if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
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
inline void FULLSEARCH::extension_for_chek_black
(
	const struct T_Bitboard * const p_bitboard,// представление доски
	const unsigned __int8 depth_max,// максимальная глубина
	unsigned __int8 & depth_max_new,// новая максимальная глубина
	bool & flag_check// флаг шаха
)
{
	// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
	if(BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
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
// полная сортировка
inline void FULLSEARCH::sorting_moves
(
	struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
)
{
	__int32 move_s;// используем для запоминания хода
	__int32 score_s;// используем для запоминания оценки хода
	unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//количество взятий
	unsigned __int8 end_list = p_list_surplus_moves->end_list;//количество ходов

	/*
	список у нас начинается с 0 и имеет вид
	0 взятие
	1 взятие
	2 взятие
	3 ход          p_list_surplus_moves->end_captures
	т.е. количество взятий на 1 превышает реальный индекс последнего взятия
	4 ход
	5 ход
	6 ход
	7              p_list_surplus_moves->end_list
	получается что у нас настоящее количество взятий и ходов т.е. если считать с 1

	счетчик взятий и ходов в списке генератор взятий изначально ставит в ноль

	*/

	// 1 сортируем взятия----------------------------------
	/*
	если у нас есть взятия тогда мы их сортируем по оценке обсчитанной еще в генераторе взятий
	идем по списку и сравниваем(меньше ли она) текущую строку со всеми нижестоящими
	понятно что если у нас текущая строка k то следующая строка будет k + 1
	если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и текущую строку
	таким образом когда мы доходим до конца все взятия у нас отсортированы по убывающей.
	*/
	for (unsigned __int8 k = 0; k < end_captures; k++)
	{	
		for (unsigned __int8 i = k + 1; i < end_captures; i++)
		{
			if (p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i])
			{ 
				//сохраняем данные   
				move_s = p_list_surplus_moves->move[k];
				score_s = p_list_surplus_moves->sorting_score[k];

				//копируем из i в k
				p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
				p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

				//восстанавливаем запись
				p_list_surplus_moves->move[i] = move_s;
				p_list_surplus_moves->sorting_score[i] = score_s;
			}
		}
	}

}

//	===================================================================
//
void FULLSEARCH::public_ini_global_loop()
{
	audit = 0;// задержка опроса команды экстренного выхода
	flag_stop_search = 0;//
}

//	===================================================================
// возвращаем флаг остановки поиска. так если в глубине поиска получили команду
// на остановку то передаем ее наружу
bool FULLSEARCH::public_get_stop_search()
{
	return flag_stop_search;
}

//	===================================================================
// возвращаем лучший вариант
struct T_PV * FULLSEARCH::public_public_get_pv_best()
{
	return PVSAVE::public_get_pv_best();
}

//	===================================================================
// возвращаем количество рассмотренных узлов
__int64  FULLSEARCH::public_get_nodes()
{
	return nodes;// количество узлов;
}

//	===================================================================
// обнуляем количество рассмотренных узлов
void FULLSEARCH::public_set_nodes_in_0()
{
	nodes = 0;// количество узлов;
}

//	===================================================================
//
void FULLSEARCH::public_ini_deep_time_terminate
(
	double deep_time_terminate_v//
)
{
	deep_time_terminate = deep_time_terminate_v;
}

//	===================================================================
//
void FULLSEARCH::public_ini_flag_stop_null_move
(
	bool flag_stop_null_move_v//
)
{
	flag_stop_null_move = flag_stop_null_move_v;
}

//	===================================================================
//
void FULLSEARCH::public_set_depth_max_rem
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
	struct T_list_surplus_moves * p_list_surplus_moves// список ходов
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