#include <iostream> // TEST
#include <fstream>  // TEST
//#include <cstring>    // используем для работы со строкой (сравнение строк)
//#include <stdlib.h>   // обеспечиваем завершение потока функцией exit(1)


#include "k_Util.h"
#include "e_Move.h"
#include "h_Evaluate.h"
#include "i_Quiescence_search.h"
#include "k_Win_32.h"
#include "j_Hash.h"
#include "j_Killer.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"

#include "j_ZObrist.h"
#include "g_Search_switch.h"
#include "g_Search_root.h"
#include "g_Search.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%

 перебор позиций в корне т.е. на нулевой глубине

*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
   ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


namespace 
{
	// NULL MOVE
	const unsigned __int8 NULL_NO = 0; // мы снаружи нулевого хода 

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
	__int64 nodes_root;// количество узлов

	// использую только для проверки совпадения хода с тем что прописан в линии игры
	unsigned __int8 from;// откуда ходит фигура
	unsigned __int8 to;// куда ходит фигура

}

//	===================================================================
// поиск за белых. белые стремятся найти максимум
// перебор на нулевом уровне
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
__int32 ROOTSEARCH::public_white_root
(
	struct T_Bitboard * const p_bitboard,// представление доски
	struct T_list_surplus_moves & list_surplus_moves,// список возможных ходов (всевозможные ходы из данного узла)
	__int32 alpha,//
	const __int32 beta,// 
	const unsigned __int8 depth_max// максимальная глубина
)
{

	__int32 value;// текущая оценка позиции
	struct T_undo undo[1];//информация используемая при возврате хода
	__int32 value_max = -9999999;//лучшая в узле  оценка позиции  
	unsigned __int8 depth_max_new = 0;//новая глубина перебора
	bool flag_save_best = 0;// пишем ли лучший вариант
	bool flag_is_legal_moves = 0;// флаг существования легальных ходов (если 0 то легальных ходов не было)
	bool flag_check = 0;//флаг шаха
	const unsigned __int8 depth = 0;// глубина
	const unsigned __int8 new_depth = 1; // следующая глубина


#if SAVE_LINE 
	unsigned __int8 nodes_0 = 1;            // количество ходов на нулевом уровне
	struct T_PV pv_best_point;// лучший для данного узла вариант
	struct T_Bitboard test_bitboard;// представление доски. использую для теста
	unsigned __int8 flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
	unsigned __int8 i_hash_move = 0; // номер кешируемого хода
#endif//#if SAVE_LINE 


	FULLSEARCH::public_set_depth_max_rem(depth_max);

#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (FULLSEARCH::public_get_stop_search() == 1)
	{
		return 0;
	}

	// инициализируем массив киллеров
	SKILLER::public_ini_killer(); 

#endif//#if SAVE_LINE 

	FULLSEARCH::sorting_moves(&list_surplus_moves);
	//Sorting_white_moves_root(&list_surplus_moves);// сортируем ходы

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// тестовая печать списка и доски на нулевом уровне
#if (TEST_L && SAVE_LINE)
	List_Print(depth_max,&list_surplus_moves);
	//if(depth_max == 4) List_Print(depth,&list_surplus_moves);
	//Bitboard_print(p_bitboard);
#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------
#if (HASH && SAVE_LINE)
	// работаем с хеш-таблицей
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// т.е. мы в списке ход из хеш-таблицы ставим первым а остальные ходы сдвигаем
	HASHM::public_look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

// для полного теста нужно отключать отсечку по оценке
#if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

	//std::cout << "хеша корень белые flag_insert_hash_move " << flag_insert_hash_move << "\n";

	if (flag_insert_hash_move == 0)
	{
		std::cout << "хеша не нашли корень белые++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
	}
	
	if (flag_insert_hash_move == 1)
	{
		std::cout << "коллизия хеша корень белые++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
		exit(1); 
	}
#endif//#if HASH_TEST

#endif//#if HASH
//-----------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// тестовая печать списка и доски на нулевом уровне
//#if (TEST_L && SAVE_LINE)
	//List_Print(depth,&list_surplus_moves);
	////if(depth_max == 4) List_Print(depth,&list_surplus_moves);
	////Bitboard_print(p_bitboard);
//#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


#if SAVE_LINE

	//запоминаем доску для дальнейшей проверки ее идентичности
	//после возврата с глубины 
	save_test_bitboard(p_bitboard,&test_bitboard);

	/*
	имя фигуры
	0 - нет фигуры
	1 - пешка 
	2 - конь 
	3 - слон
	4 - ладья 
	5 - ферзь 
	6 - король
	*/

	// оцениваем количество и тип фигур если меньше чем нужно то отключаем нулевой ход
	//if( (p_bitboard->white_piece[5] == 0) && // если у белых нет ферзей
	//(p_bitboard->white_piece[4] == 0) && // нет ладей
	//(p_bitboard->white_piece[3] == 0))  // нет слонов
	//)
	//{ 
		////std::cout <<"нулевой ход белые выключили ";
		////std::cout <<"\n";
		//flag_stop_null_move = 0; // то выключаем нулевой ход
	//}
	//else 
	//{
		//flag_stop_null_move = 1; // иначе он включен
	//}

	FULLSEARCH::public_ini_flag_stop_null_move(1);

#endif//#if SAVE_LINE 


	//бежим по списку избыточных ходов
	// старт и конец цикла; должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// реализуем ход из списка или другими словами генерируем позицию
		MOVES::public_onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);
		//Bitboard_print(p_bitboard);

		// если король под шахом то (детектор шахов возвращает имя шахующей фигуры)
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

			// записываем оценку в корневой список ходов
			list_surplus_moves.sorting_score[i] = -1000000;

			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = 1;
		nodes_root = nodes_root + 1;
		depth_max_new = depth_max;


#if SAVE_LINE
		// печатаем информацию о рассматриваемом ходе и глубине перебора
		PVSAVE::public_print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

		// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине
		PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);

		// обновляем хеш ключ
		ZOBRIST::public_modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

		// TEST-----------
		// тут ключ считается полностью по новой
		// тестировал хеш-таблицу
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//HASHM::public_start_position_random_key(p_bitboard);
		//----------------

		// записываем ключ для последующего анализа на повтор позиций
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth);

#endif//#if SAVE_LINE 

    
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// если король под шахом то увеличиваем максимальную глубину перебора 
		FULLSEARCH::extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------


#if (ALPHA_BETA_ON && SAVE_LINE)
		// здесь у нас классический негаскаут(NegaScout)
		// первую линию считаем главной и смотрим с полным окном остальные пытаемся
		// отсечь поиском с нулевым окном и если вдруг оценка улучшилась то пересчитываем
		// с полным окном
		if (i == 0)
#else
		// в режиме полного перебора смотрим все и все линии считаем главными
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			//рекурсивный вызов функции
			value = FULLSEARCH::public_black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,NULL_NO);
		}
		else
		{
			// вызываем функцию  с нулевым окном. это уже не главный вариант
			value = FULLSEARCH::public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,NULL_NO);

			if(value > alpha)
			{
				// если вдруг alpha улчшили то смотрим по новой как главный вариант
				value = FULLSEARCH::public_black(p_bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.move[i]>>6) & 63),
					PV_YES,flag_check,NULL_NO);
			}
		}

		// восстанавливаем позицию
		MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

		// записываем оценку в корневой список ходов
		list_surplus_moves.sorting_score[i] = value;


#if SAVE_LINE

		// восстанавливаем хеш-ключ доски
		p_bitboard->hash_key = key_undo;

		// ищем лучшую позицию. за белых ищем максимум
		if (value > value_max)
		{
			value_max = value;
			i_hash_move = i;
			from = ((list_surplus_moves.move[i]>>12)& 63);
			to = ((list_surplus_moves.move[i]>>18)& 63);

			// TEST		 
			//-----------------------------------------------
			//std::cout << "улучшили максимум. ход записанный в хеш " << "\n";
			//std::cout << "depth= " << depth << "\n";
			//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
			//std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
			//	<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
			//std::cout << "Ходящая фигура " << util_o.public_int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
           
			//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
			//std::cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
			//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
			//std::cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
			//std::cout << "  " << "\n"; 
			//-----------------------------------------------



//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// белые работают с альфой и пытаются ее улучшить
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			if (value > alpha)
			{ 
				alpha = value;       
				flag_save_best = 1;

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				PVSAVE::public_update_PV_best_point(&pv_best_point);

				// проверяем совпадение варианта и оценки
				if (value != PVSAVE::public_get_pv_best()->score_move)
				{
					std::cout << "ERROR white value " ;
					std::cout << "\n";
				}

				// TEST
				//-----------------------------------------------
				//std::cout << "улучшили альфу. ход записанный в хеш " << "\n";
				//std::cout << "depth= " << depth << "\n";
				//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
				//std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				//	<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
				//std::cout << "Ходящая фигура " << util_o.public_int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
               
				//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
				//std::cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
				//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
				//std::cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
				//std::cout << "  " << "\n";     
				//-----------------------------------------------

			}
#else///#if ALPHA_BETA_ON

			flag_save_best = 1;
			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			PVSAVE::public_update_PV_best_point(&pv_best_point);

			// проверяем совпадение варианта и оценки
			if (value != PVSAVE::public_get_pv_best()->score_move)
			{
				std::cout << "ERROR white value " ;
				std::cout << "\n";
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// выводим ход который обдумываем делаем это только на нулевой глубине т.е. у самой поверхности
		if (FULLSEARCH::public_get_stop_search() != 1)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// для теста вариантов
			PVSAVE::public_print_pv_line(1,&list_surplus_moves,PVSAVE::public_get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// для рабочего режима
			PVSAVE::public_print_pv_line(1,&list_surplus_moves,&pv_best_point,nodes_root,i,depth_max,value_max,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 


	}


#if SAVE_LINE
	// проверяем восстановилась ли доска после прохождения глубины
	compare_test_bitboard(p_bitboard,&test_bitboard);
#endif//#if SAVE_LINE 

	// если не одного хода сделано не было то производим статическую оценку позиции
	// это случай пата или мата
	if (flag_is_legal_moves == 0)
	{
		value = 0;
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		PVSAVE::public_update_PV_best_max_depth(value,depth);

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == 1)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
		PVSAVE::public_update_PV_best(&pv_best_point);
	}

#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	// кэшируем лучший ход
	HASHM::public_save_best_hash_score_or_move(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
		value_max, HASHM::UPPER, depth, depth_max);

#endif//#if HASH

	return value_max;

}

//	===================================================================
// перебор на нулевом уровне за черных ищем минимум
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
__int32 ROOTSEARCH::public_black_root
(
	struct T_Bitboard * const p_bitboard,// представление доски
	struct T_list_surplus_moves & list_surplus_moves,// список возможных ходов (всевозможные ходы из данного узла)
	const __int32 alpha,//
	__int32 beta,// 
	const unsigned __int8 depth_max// максимальная глубина
)
{
	__int32 value;              // текущая оценка позиции
	struct T_undo undo[1];//информация используемая при возврате хода

	__int32 value_min = 9999999;// лучшая в узле  оценка позиции  
	unsigned __int8 depth_max_new = 0;//новая глубина перебора
	bool flag_save_best = 0;// пишем ли лучший вариант
	bool flag_is_legal_moves = 0;// флаг существования легальных ходов (если 0 то легальных ходов не было)
	bool flag_check = 0;//флаг шаха

	const unsigned __int8 depth = 0;// глубина
	const unsigned __int8 new_depth = 1; // следующая глубина

#if SAVE_LINE
	unsigned __int8 nodes_0 = 1;            // количество ходов на нулевом уровне
	struct T_PV pv_best_point;// лучший для данного узла вариант
	struct T_Bitboard test_bitboard;// представление доски. использую для теста
	unsigned __int8  flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
	unsigned __int8 i_hash_move = 0; // номер кешируемого хода
#endif//#if SAVE_LINE 

	FULLSEARCH::public_set_depth_max_rem(depth_max);


#if SAVE_LINE

	// это отсечка при экстренном выходе
	if (FULLSEARCH::public_get_stop_search() == 1)
	{
		return 0;
	}

	// инициализируем массив киллеров
	SKILLER::public_ini_killer(); 

#endif//#if SAVE_LINE 


	FULLSEARCH::sorting_moves(&list_surplus_moves);
	//Sorting_black_moves_root(&list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// тестовая печать списка и доски на нулевом уровне
#if (TEST_L && SAVE_LINE)
	List_Print(depth_max,&list_surplus_moves);
	//if (depth_max == 3) 
		//List_Print(depth,&list_surplus_moves);

	//	Bitboard_print(p_bitboard);
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if (HASH && SAVE_LINE)
	// работаем с хеш-таблицей
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// т.е. мы в списке ход из хеш-таблицы ставим первым а остальные ходы сдвигаем
	HASHM::public_look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

#if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

	if (flag_insert_hash_move == 0)
	{
		std::cout << "хеша не нашли корень черные++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
	}

	if (flag_insert_hash_move == 1) 
	{
		std::cout << "коллизия хеша корень черные++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
		exit(1); 
	}

#endif//#if HASH_TEST
#endif//#if HASH


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//тестовая печать списка и доски на нулевом уровне
//#if (TEST_L && SAVE_LINE)
	//if (depth_max == 3)
	//{
		//List_Print(depth,&list_surplus_moves);
	//}

	////Bitboard_print(p_bitboard);
//#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



#if SAVE_LINE

	//запоминаем доску для дальнейшей проверки ее идентичности
	save_test_bitboard(p_bitboard,&test_bitboard);

#endif//#if SAVE_LINE 


	// оцениваем количество и тип фигур если меньше чем нужно то отключаем нулевой ход
	//if ((p_bitboard->black_piece[5] == 0) && // если у белых нет ферзей
		//(p_bitboard->black_piece[4] == 0) && // нет ладей
		//(p_bitboard->black_piece[3] == 0))  // нет слонов
	//{ 
		//std::cout <<"нулевой ход черные выключили ";
		////std::cout <<"\n";
		//flag_stop_null_move = 0; // то выключаем нулевой ход
	//}
	//else
	//{
		//flag_stop_null_move = 1; // иначе он включен
	//}

	FULLSEARCH::public_ini_flag_stop_null_move(1); // иначе он включен


	//бежим по списку избыточных ходов
	// старт и конец цикла; должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// реализуем ход из списка или другими словами генерируем позицию
		MOVES::public_onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);
		//Bitboard_print(p_bitboard);

		// если король под шахом то
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

			// записываем оценку в корневой список ходов
			list_surplus_moves.sorting_score[i] = 1000000;

			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = 1;
		nodes_root = nodes_root + 1;
		depth_max_new = depth_max;


#if SAVE_LINE
		// печатаем информацию о рассматриваемом ходе и глубине перебора
		PVSAVE::public_print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

		// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
		PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);

		// обновляем хеш ключ
		ZOBRIST::public_modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);

		// TEST-----------
		// тут ключ считается полностью по новой
		// тестировал хеш-таблицу
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//HASHM::public_start_position_random_key(p_bitboard);
		//----------------

		// записываем ключ для последующего анализа на повтор позиций
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth);

#endif//#if SAVE_LINE 


//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// если король под шахом то увеличиваем максимальную глубину перебора 
		FULLSEARCH::extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

#if (ALPHA_BETA_ON && SAVE_LINE)
		if (i == 0)
#else
		if (1)
#endif//#if ALPHA_BETA_ON

		{
			value = FULLSEARCH::public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,NULL_NO);

		}
		else
		{
			//рекурсивный вызов функции
			value = FULLSEARCH::public_white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,NULL_NO);

			if (value < beta)
			{
				//рекурсивный вызов функции
				value = FULLSEARCH::public_white(p_bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.move[i]>>6)& 63),
					PV_YES,flag_check,NULL_NO);
			}
		}

		//восстанавливаем позицию
		MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

		// записываем оценку в корневой список ходов
		list_surplus_moves.sorting_score[i] = value;


#if SAVE_LINE

		// восстанавливаем хеш-ключ доски
		p_bitboard->hash_key = key_undo;

		//ищем лучшую позицию за черных ищем минимум
		if (value < value_min)
		{
			value_min = value;
			i_hash_move = i; // номер кешируемого хода
			from = ((list_surplus_moves.move[i] >> 12) & 63);
			to = ((list_surplus_moves.move[i] >> 18) & 63);
  
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			// черные пытаются улучшить бету
			if (value < beta)
			{
				beta = value;
				flag_save_best = 1;

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				PVSAVE::public_update_PV_best_point(&pv_best_point);

				// проверяем совпадение варианта и оценки
				if (value != PVSAVE::public_get_pv_best()->score_move)
				{
					std::cout << "ERROR black value " ;
					std::cout << "\n";
				}
			}
#else//#if ALPHA_BETA_ON

			flag_save_best = 1;
			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			PVSAVE::public_update_PV_best_point(&pv_best_point);

			// проверяем совпадение варианта и оценки
			if (value != PVSAVE::public_get_pv_best()->score_move)
			{
				std::cout << "ERROR black value " ;
				std::cout << "\n";
			}
#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// выводим ход который обдумываем делаем это только на нулевой глубине т.е. у самой поверхности
		if (FULLSEARCH::public_get_stop_search() != 1)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// для теста вариантов
			PVSAVE::public_print_pv_line(0,&list_surplus_moves,PVSAVE::public_get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// для рабочего режима
			PVSAVE::public_print_pv_line(0,&list_surplus_moves,&pv_best_point,nodes_root,i,depth_max,value_min,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 


	}


#if SAVE_LINE
	// проверяем восстановилась ли доска после прохождения глубины
	compare_test_bitboard(p_bitboard,&test_bitboard);
#endif//#if SAVE_LINE 

	// если не одного хода сделано не было то производим статическую оценку позиции
	// это случай пата или мата
	if (flag_is_legal_moves == 0)
	{
		value = 0;
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);// может это слишком большое число?
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		PVSAVE::public_update_PV_best_max_depth(value,depth);

		return value;
	}


#if SAVE_LINE
	if (flag_save_best == 1)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
		PVSAVE::public_update_PV_best(&pv_best_point);
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	//кэшируем лучший ход
	HASHM::public_save_best_hash_score_or_move(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
		value_min, HASHM::UPPER, depth, depth_max);
#endif//#if HASH


	return value_min;
}

//	===================================================================
//
void ROOTSEARCH::public_ini_global_loop
(
	unsigned __int8 depth_max//
)
{
	// инициализируем глобальные переменные модуля loop
	FULLSEARCH::public_ini_global_loop();
}

//	===================================================================
//
void ROOTSEARCH::public_ini_deep_time_terminate
(
	double deep_time_terminate_v//
)
{
	FULLSEARCH::public_ini_deep_time_terminate(deep_time_terminate_v);
}

//	===================================================================
// возвращаем флаг остановки поиска. так если в глубине поиска получили команду
// на остановку то передаем ее наружу
bool ROOTSEARCH::public_get_stop_search()
{
	return FULLSEARCH::public_get_stop_search();
}

//	===================================================================
// возвращаем лучший вариант
struct T_PV * ROOTSEARCH::public_get_pv_best()
{
	return PVSAVE::public_get_pv_best();
}

//	===================================================================
// возвращаем количество рассмотренных узлов
__int64  ROOTSEARCH::public_get_nodes()
{
	return (nodes_root + FULLSEARCH::public_get_nodes());// количество узлов;
}

//	===================================================================
// обнуляем количество рассмотренных узлов
void ROOTSEARCH::public_set_nodes_in_0()
{
	nodes_root = 0;// количество узлов;
	FULLSEARCH::public_set_nodes_in_0();
}

//	===================================================================
// откуда делается ход
unsigned __int8 ROOTSEARCH::public_get_from()
{
	return from;
}

//	===================================================================
// куда делается ход
unsigned __int8 ROOTSEARCH::public_get_to()
{
	return to;
}





//	===================================================================
// полная сортировка корневого списка
inline void ROOTSEARCH::sorting_white_moves_root
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

	// 1 сортируем корневые ходы----------------------------------
	/*
	мы их сортируем по оценке обсчитанной на предыдущей итерации
	идем по списку и сравниваем(меньше ли она) текущую строку со всеми нижестоящими
	понятно что если у нас текущая строка k то следующая строка будет k + 1
	если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и текущую строку
	таким образом когда мы доходим до конца все ходы у нас отсортированы по убывающей.
	*/
	for (unsigned __int8 k = 0; k < end_list; k++)
	{	
		for (unsigned __int8 i = k + 1; i < end_list; i++)
		{
			if (p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i])
			{ 
				// сохраняем данные   
				move_s = p_list_surplus_moves->move[k];
				score_s = p_list_surplus_moves->sorting_score[k];

				// копируем из i в k
				p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
				p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

				// восстанавливаем запись
				p_list_surplus_moves->move[i] = move_s;
				p_list_surplus_moves->sorting_score[i] = score_s;
			}
		}
	}

}

//	===================================================================
// полная сортировка корневого списка
inline void ROOTSEARCH::sorting_black_moves_root
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

	// 1 сортируем корневые ходы----------------------------------
	/*
	мы их сортируем по оценке обсчитанной на предыдущей итерации
	идем по списку и сравниваем(меньше ли она) текущую строку со всеми нижестоящими
	понятно что если у нас текущая строка k то следующая строка будет k + 1
	если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и текущую строку
	таким образом когда мы доходим до конца все ходы у нас отсортированы по убывающей.
	*/
	for (unsigned __int8 k = 0; k < end_list; k++)
	{	
		for (unsigned __int8 i = k + 1; i < end_list; i++)
		{
			if (p_list_surplus_moves->sorting_score[k] > p_list_surplus_moves->sorting_score[i])
			{ 
				// сохраняем данные   
				move_s = p_list_surplus_moves->move[k];
				score_s = p_list_surplus_moves->sorting_score[k];

				// копируем из i в k
				p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
				p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

				// восстанавливаем запись
				p_list_surplus_moves->move[i] = move_s;
				p_list_surplus_moves->sorting_score[i] = score_s;
			}
		}
	}
}

//	===================================================================
// использую для теста. сейчас уже не помню для чего он мне был нужен
void ROOTSEARCH::print_name_from_coord
(
	const struct T_Bitboard * const p_bitboard// представление доски
)
{

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << p_bitboard->white_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	std::cout << "\n";

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << p_bitboard->black_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	//for (int i= 0; i < 64; i++)
	//{
	//	std::cout << bitboard->white_name_from_coord[i];
	//	if( i%8 == 7)
	//{
	//		std::cout << "\n";
	//}
	//}
}

//	===================================================================
// использую для теста. запоминаем доску
inline void ROOTSEARCH::save_test_bitboard
(
	const struct T_Bitboard * const p_bitboard,// представление доски
	struct T_Bitboard * p_test_bitboard// представление доски
)
{

	p_test_bitboard->all_piece = p_bitboard->all_piece;// все фигуры
	p_test_bitboard->all_white_piece = p_bitboard->all_white_piece;// все белые фигуры
	p_test_bitboard->all_black_piece = p_bitboard->all_black_piece;// все черные фигуры
	p_test_bitboard->colour_move = p_bitboard->colour_move;
	p_test_bitboard->taking_passage_yes = p_bitboard->taking_passage_yes;
	p_test_bitboard->taking_passage_x = p_bitboard->taking_passage_x;
	p_test_bitboard->taking_passage_y = p_bitboard->taking_passage_y;
	p_test_bitboard->castling_Q = p_bitboard->castling_Q;
	p_test_bitboard->castling_K = p_bitboard->castling_K;
	p_test_bitboard->castling_q = p_bitboard->castling_q;
	p_test_bitboard->castling_k = p_bitboard->castling_k;
	p_test_bitboard->hash_key =  p_bitboard->hash_key;
	p_test_bitboard->value_white_material = p_bitboard->value_white_material;
	p_test_bitboard->value_black_material = p_bitboard->value_black_material;

	for (unsigned __int8 n =0; n < 7; n++ )
	{
		p_test_bitboard->white_piece[n] = p_bitboard->white_piece[n];
		p_test_bitboard->black_piece[n] = p_bitboard->black_piece[n];
	}

	for (unsigned __int8 k =0; k < 64; k++ )
	{
		p_test_bitboard->white_name_from_coord[k] = p_bitboard->white_name_from_coord[k];
		p_test_bitboard->black_name_from_coord[k] = p_bitboard->black_name_from_coord[k];
	}
}

//	===================================================================
// использую для теста. сравниваем доски
inline void ROOTSEARCH::compare_test_bitboard
(
	const struct T_Bitboard * const p_bitboard,// представление доски
	struct T_Bitboard * p_test_bitboard// представление доски
)
{
	// проверяем востановилась ли доска после прохождения глубины
	if (p_test_bitboard->all_piece != p_bitboard->all_piece)
	{
		std::cout << "white test_all_piece != bitboard->all_piece" << "\n";// все фигуры
	}

	if (p_test_bitboard->all_white_piece != p_bitboard->all_white_piece)
	{
		std::cout << "white test_all_white_piece != bitboard->all_white_piece" << "\n";// все белые фигуры
	}

	if (p_test_bitboard->all_black_piece != p_bitboard->all_black_piece) 
	{
		std::cout << "white test_all_black_piece != bitboard->all_black_piece" << "\n";// все черные фигуры
	}

	if (p_test_bitboard->colour_move != p_bitboard->colour_move)
	{
		std::cout << "white test_colour_move != bitboard->colour_move" << "\n";
	}

	if (p_test_bitboard->taking_passage_yes != p_bitboard->taking_passage_yes)
	{
		std::cout << "white test_taking_passage_yes != bitboard->taking_passage_yes" << "\n";
	}

	if (p_test_bitboard->taking_passage_x != p_bitboard->taking_passage_x)
	{
		std::cout << "white test_taking_passage_x != bitboard->taking_passage_x" << "\n";
	}

	if (p_test_bitboard->taking_passage_y != p_bitboard->taking_passage_y)
	{
		std::cout << "white test_taking_passage_y != bitboard->taking_passage_y" << "\n";
	}

	if (p_test_bitboard->castling_Q != p_bitboard->castling_Q)
	{
		std::cout << "white test_castling_Q != bitboard->castling_Q" << "\n";
	}

	if (p_test_bitboard->castling_K != p_bitboard->castling_K)
	{
		std::cout << "white test_castling_K != bitboard->castling_K" << "\n";
	}

	if (p_test_bitboard->castling_q != p_bitboard->castling_q)
	{
		std::cout << "white test_castling_q != bitboard->castling_q" << "\n";
	}

	if (p_test_bitboard->castling_k != p_bitboard->castling_k)
	{
		std::cout << "white test_castling_k != bitboard->castling_k" << "\n";
	}

	if (p_test_bitboard->hash_key !=  p_bitboard->hash_key)
	{
		std::cout << "white test_hash_key !=  bitboard->hash_key" << "\n";
	}

	if (p_test_bitboard->value_white_material != p_bitboard->value_white_material)
	{
		std::cout << "white test_value_white_material != bitboard->value_white_material" << "\n";
	}

	if (p_test_bitboard->value_black_material != p_bitboard->value_black_material)
	{
		std::cout << "white test_value_black_material != bitboard->value_black_material" << "\n";
	}



	for (unsigned __int8 n2 = 0; n2 < 7; n2++ )
	{
		if (p_test_bitboard->white_piece[n2] != p_bitboard->white_piece[n2])
		{
			std::cout << "white test_white_piece[n] = bitboard->white_piece[n]" << n2 << "\n";
		}

		if (p_test_bitboard->black_piece[n2] != p_bitboard->black_piece[n2])
		{
			std::cout << "white test_black_piece[n] = bitboard->black_piece[n]" << n2 << "\n";
		}
	}

	for (unsigned __int8 k2 = 0; k2 < 64; k2++ )
	{
		if (p_test_bitboard->white_name_from_coord[k2] != p_bitboard->white_name_from_coord[k2])
		{
			std::cout << "white test_white_name_from_coord[k] = bitboard->white_name_from_coord[k]" << k2 << "\n";
		}

		if (p_test_bitboard->black_name_from_coord[k2] != p_bitboard->black_name_from_coord[k2])
		{
			std::cout << "white test_black_name_from_coord[k] = bitboard->black_name_from_coord[k]" << k2 << "\n";
		}
	}

}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// максимальная глубина, глубина перебора, доска,
#if TEST_L

void ROOTSEARCH::List_Print
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