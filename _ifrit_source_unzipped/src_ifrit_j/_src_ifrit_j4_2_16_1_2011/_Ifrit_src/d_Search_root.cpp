
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009

#include <fstream>
#include <iostream>

#include "d_Search_root.h"
#include "e_Search.h"
#include "h_Killer.h"
#include "i_ZObrist.h"
#include "j_Hash.h"
#include "k_PV_save.h"
#include "l_Chess_bitboard.h"
#include "m_List_surplus_moves.h"

class Search search; 

extern class Killer killer;
extern class ZObrist zobrist;
extern class Hash hash;
extern class PV_save pv_save;
extern class Chess_bitboard bitboard;
extern class List_surplus_moves root_list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)



//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
	std::ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// поиск за белых. белые стремятся найти максимум
// перебор на нулевом уровне
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
__int32 Search_root::white_root
(
	__int32 alpha,//
	const __int32 beta,// 
	const unsigned __int8 depth_max// максимальная глубина
)
{

	__int32 value;// текущая оценка позиции
	__int32 value_max = -9999999;//лучшая в узле  оценка позиции  
	unsigned __int8 depth_max_new;//новая глубина перебора
	bool flag_save_best = false;// пишем ли лучший вариант
	bool flag_is_legal_moves = false;// флаг существования легальных ходов (если 0 то легальных ходов не было)
	bool flag_check = false;//флаг шаха
	const unsigned __int8 depth = 0;// глубина
	const unsigned __int8 new_depth = 1; // следующая глубина

	// переменные параметры в поиске NegaScout
	__int32 beta_scout;//
	bool flag_pv_f_scout;// основной ли это вариант (pv)

#if SAVE_LINE 
	unsigned __int8 nodes_0 = 1;            // количество ходов на нулевом уровне
	struct PV_line pv_best_point;// лучший для данного узла вариант
	class Chess_bitboard test_bitboard;// представление доски. использую для теста
	unsigned __int8 flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // запоминаем хеш-ключ даннного узла
	unsigned __int8 i_hash_move = 0; // номер кешируемого хода
#endif//#if SAVE_LINE 


	//
	search.set_depth_max_rem(depth_max);

#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (search.get_stop_search() == true)
	{
		return 0;
	}

#endif//#if SAVE_LINE 

	root_list_surplus_moves.sorting_moves();
	//Sorting_white_moves_root(&root_list_surplus_moves);// сортируем ходы

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// тестовая печать списка и доски на нулевом уровне
#if (TEST_L && SAVE_LINE)
	list_print(depth_max,root_list_surplus_moves);
	//if(depth_max == 4) List_Print(depth,&root_list_surplus_moves);
	//Bitboard_print(bitboard);
#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------
#if (HASH && SAVE_LINE)
	// работаем с хеш-таблицей
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// т.е. мы в списке ход из хеш-таблицы ставим первым а остальные ходы сдвигаем
	hash.look_hash_move(root_list_surplus_moves,bitboard.get_hash_key()
						,depth,flag_insert_hash_move);

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
	//List_Print(depth,&root_list_surplus_moves);
	////if(depth_max == 4) List_Print(depth,&root_list_surplus_moves);
	////Bitboard_print(bitboard);
//#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


#if SAVE_LINE

	// TEST
	// запоминаем доску для дальнейшей проверки ее идентичности
	// после возврата с глубины 
	test_bitboard.save_test_bitboard(bitboard);

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
	//if( (bitboard.white_piece[5] == 0) && // если у белых нет ферзей
	//(bitboard.white_piece[4] == 0) && // нет ладей
	//(bitboard.white_piece[3] == 0))  // нет слонов
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

	search.ini_flag_stop_null_move(1);

#endif//#if SAVE_LINE 


	//бежим по списку избыточных ходов
	// старт и конец цикла; должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < root_list_surplus_moves.get_end_list(); i++)
	{
		// реализуем ход из списка или другими словами генерируем позицию
		bitboard.do_moves_white(i,depth,root_list_surplus_moves);
		//Bitboard_print(bitboard);

		// если король под шахом то (детектор шахов возвращает имя шахующей фигуры)
		if (root_list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			bitboard.undo_moves_white(i,depth,root_list_surplus_moves);

			// записываем оценку в корневой список ходов
			root_list_surplus_moves.set_sorting_score(i,-1000000);

			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = true;
		nodes_root = nodes_root + 1;

#if SAVE_LINE
		// печатаем информацию о рассматриваемом ходе и глубине перебора
		pv_save.print_currmove(root_list_surplus_moves,nodes_0,i,depth_max);

		// из списка возможных ходов копируем текущий ход в текущий вариант 
		// на текущей глубине
		pv_save.update_PV_current(i,depth,root_list_surplus_moves);

		// обновляем хеш ключ
		zobrist.modification_random_key(1,i,root_list_surplus_moves
										,bitboard.get_d_undo(depth));

		// TEST-----------
		// тут ключ считается полностью по новой
		// тестировал хеш-таблицу
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// записываем ключ для последующего анализа на повтор позиций
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth);

#endif//#if SAVE_LINE 

		depth_max_new = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// если король под шахом то увеличиваем максимальную глубину перебора 
		search.extension_for_chek_white(depth_max,depth_max_new,flag_check
										,root_list_surplus_moves);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

		// тип хода: 0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
		// 4 – короткая рокировка 5 – взятие на проходе и т.д.
		const __int32 description_move = ( root_list_surplus_moves.get_move(i) >> 6 ) & 63;

//_scout_scout white
		// здесь у нас классический негаскаут(NegaScout)
		// первую линию считаем главной и смотрим с полным окном остальные пытаемся
		// отсечь поиском с нулевым окном и если вдруг оценка улучшилась то пересчитываем
		// с полным окном
#if (ALPHA_BETA_ON && SAVE_LINE)
		if (i == 0)
#else
		// в режиме полного перебора смотрим все и все линии считаем главными
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			// первую линию считаем главной и смотрим с полным окном
			beta_scout = beta;
			flag_pv_f_scout = PV_YES;
		}
		else
		{
			// вызываем функцию  с нулевым окном. это уже не главный вариант
			beta_scout = alpha + 1;
			flag_pv_f_scout = PV_NO;
		}

		//рекурсивный вызов функции
		value = search.black(alpha,beta_scout,depth_max_new,new_depth,description_move,
							 flag_pv_f_scout,flag_check,NULL_NO);

#if (ALPHA_BETA_ON && SAVE_LINE)
		if( (value > alpha) && (i != 0) )
		{
			// если вдруг alpha улчшили то смотрим по новой как главный вариант
			beta_scout = beta;
			flag_pv_f_scout = PV_YES;

			//рекурсивный вызов функции
			value = search.black(alpha,beta_scout,depth_max_new,new_depth,description_move,
								 flag_pv_f_scout,flag_check,NULL_NO);
		}

#endif//#if ALPHA_BETA_ON



		// восстанавливаем позицию
		bitboard.undo_moves_white(i,depth,root_list_surplus_moves);

		// записываем оценку в корневой список ходов
		root_list_surplus_moves.set_sorting_score(i, value);


#if SAVE_LINE

		// восстанавливаем хеш-ключ доски
		bitboard.set_hash_key(key_undo);

		// ищем лучшую позицию. за белых ищем максимум
		if (value > value_max)
		{
			value_max = value;
			i_hash_move = i;

			// TEST
			from = ((root_list_surplus_moves.get_move(i)>>12)& 63);
			to = ((root_list_surplus_moves.get_move(i)>>18)& 63);

			// TEST		 
			//-----------------------------------------------
			//std::cout << "улучшили максимум. ход записанный в хеш " << "\n";
			//std::cout << "depth= " << depth << "\n";
			//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
			//std::cout << "((root_list_surplus_moves.move[i]>>6)& 63)= " 
			//	<< ((root_list_surplus_moves.move[i]>>6)& 63) << "\n";
			//std::cout << "Ходящая фигура " << util_o.public_int_to_char_board(((root_list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
           
			//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>12)& 63)%8);
			//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>12)& 63)/8);
			//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>18)& 63)%8);
			//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
			//std::cout << "  " << "\n"; 
			//-----------------------------------------------



//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// белые работают с альфой и пытаются ее улучшить
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			if (value > alpha)
			{ 
				alpha = value;       
				flag_save_best = true;

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				pv_save.update_PV_best_point(pv_best_point);

				// TEST
				// проверяем совпадение варианта и оценки
				if (value != pv_save.get_pv_best().score_move)
				{
					std::cout << "ERROR white value " ;
					std::cout << "\n";
				}

				// TEST
				//-----------------------------------------------
				//std::cout << "улучшили альфу. ход записанный в хеш " << "\n";
				//std::cout << "depth= " << depth << "\n";
				//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
				//std::cout << "((root_list_surplus_moves.move[i]>>6)& 63)= " 
				//	<< ((root_list_surplus_moves.move[i]>>6)& 63) << "\n";
				//std::cout << "Ходящая фигура " << util_o.public_int_to_char_board(((root_list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
               
				//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>12)& 63)%8);
				//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>12)& 63)/8);
				//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>18)& 63)%8);
				//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
				//std::cout << "  " << "\n";     
				//-----------------------------------------------

			}
#else///#if ALPHA_BETA_ON

			flag_save_best = true;

			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			pv_save.update_PV_best_point(pv_best_point);

			// TEST
			// проверяем совпадение варианта и оценки
			if (value != pv_save.get_pv_best().score_move)
			{
				std::cout << "ERROR white value " ;
				std::cout << "\n";
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// выводим ход который обдумываем делаем это только на нулевой глубине 
		// т.е. у самой поверхности
		if (search.get_stop_search() != true)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// для теста вариантов
			pv_save.print_pv_line(1,root_list_surplus_moves,pv_save.get_pv_best()
								  ,nodes_root,i,depth_max,value,1);
#else
			// для рабочего режима
			pv_save.print_pv_line(1,root_list_surplus_moves,pv_best_point
								  ,nodes_root,i,depth_max,value_max,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 

	}


#if SAVE_LINE
	// TEST
	// проверяем восстановилась ли доска после прохождения глубины
	test_bitboard.compare_test_bitboard(bitboard);
#endif//#if SAVE_LINE 

	// если не одного хода сделано не было то производим статическую оценку позиции
	// это случай пата или мата
	if (flag_is_legal_moves == false)
	{
		value = 0;
		if (root_list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		pv_save.update_PV_best_max_depth(value,depth);

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == true)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на более 
		// высокий уровень
		pv_save.update_PV_best(pv_best_point);
	}

#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	// кэшируем лучший ход
	hash.save_best_hash_score_or_move(i_hash_move, bitboard.get_hash_key()
									  ,root_list_surplus_moves, 
									  value_max, UPPER, depth, depth_max);

#endif//#if HASH

	return value_max;

}

//	===================================================================
// перебор на нулевом уровне за черных ищем минимум
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
__int32 Search_root::black_root
(
	const __int32 alpha,//
	__int32 beta,// 
	const unsigned __int8 depth_max// максимальная глубина
)
{
	__int32 value;              // текущая оценка позиции

	__int32 value_min = 9999999;// лучшая в узле  оценка позиции  
	unsigned __int8 depth_max_new;//новая глубина перебора
	bool flag_save_best = false;// пишем ли лучший вариант
	bool flag_is_legal_moves = false;// флаг существования легальных ходов (если 0 то легальных ходов не было)
	bool flag_check = false;//флаг шаха

	const unsigned __int8 depth = 0;// глубина
	const unsigned __int8 new_depth = 1; // следующая глубина

	// переменные параметры в поиске NegaScout
	__int32 alpha_scout;//
	bool flag_pv_f_scout;// основной ли это вариант (pv)

#if SAVE_LINE
	unsigned __int8 nodes_0 = 1;            // количество ходов на нулевом уровне
	struct PV_line pv_best_point;// лучший для данного узла вариант
	class Chess_bitboard test_bitboard;// представление доски. использую для теста
	unsigned __int8  flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // запоминаем хеш-ключ даннного узла
	unsigned __int8 i_hash_move = 0; // номер кешируемого хода
#endif//#if SAVE_LINE 


	search.set_depth_max_rem(depth_max);

#if SAVE_LINE

	// это отсечка при экстренном выходе
	if (search.get_stop_search() == true)
	{
		return 0;
	}

#endif//#if SAVE_LINE 


	root_list_surplus_moves.sorting_moves();
	//Sorting_black_moves_root(&root_list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// тестовая печать списка и доски на нулевом уровне
#if (TEST_L && SAVE_LINE)
	list_print(depth_max,root_list_surplus_moves);
	//if (depth_max == 3) 
		//List_Print(depth,&root_list_surplus_moves);

	//	Bitboard_print(bitboard);
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if (HASH && SAVE_LINE)
	// работаем с хеш-таблицей
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// т.е. мы в списке ход из хеш-таблицы ставим первым а остальные ходы сдвигаем
	hash.look_hash_move(root_list_surplus_moves,bitboard.get_hash_key()
						,depth,flag_insert_hash_move);

#if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

	if (flag_insert_hash_move == 0)
	{
		std::cout << "хеша не нашли корень черные+++++++++++++++++++++++++++++++++" << "\n";   
	}

	if (flag_insert_hash_move == 1) 
	{
		std::cout << "коллизия хеша корень черные+++++++++++++++++++++++++++++++++" << "\n";
		exit(1); 
	}

#endif//#if HASH_TEST
#endif//#if HASH


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//тестовая печать списка и доски на нулевом уровне
//#if (TEST_L && SAVE_LINE)
	//if (depth_max == 3)
	//{
		//List_Print(depth,&root_list_surplus_moves);
	//}

	////Bitboard_print(bitboard);
//#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



#if SAVE_LINE

	// TEST
	//запоминаем доску для дальнейшей проверки ее идентичности
	test_bitboard.save_test_bitboard(bitboard);

#endif//#if SAVE_LINE 


	// оцениваем количество и тип фигур если меньше чем нужно то отключаем нулевой ход
	//if ((bitboard.black_piece[5] == 0) && // если у белых нет ферзей
		//(bitboard.black_piece[4] == 0) && // нет ладей
		//(bitboard.black_piece[3] == 0))  // нет слонов
	//{ 
		//std::cout <<"нулевой ход черные выключили ";
		////std::cout <<"\n";
		//flag_stop_null_move = 0; // то выключаем нулевой ход
	//}
	//else
	//{
		//flag_stop_null_move = 1; // иначе он включен
	//}

	search.ini_flag_stop_null_move(1); // иначе он включен


	// бежим по списку избыточных ходов
	// старт и конец цикла; должен быть согласован с генератором списка
	for (unsigned __int8 i = 0; i < root_list_surplus_moves.get_end_list(); i++)
	{
		// реализуем ход из списка или другими словами генерируем позицию
		bitboard.do_moves_black(i,depth,root_list_surplus_moves);
		//Bitboard_print(bitboard);

		// если король под шахом то
		if (root_list_surplus_moves.king_black_check(bitboard) != CHECK_NO)
		{
			// мы отменяем этот ход как некорректный
			bitboard.undo_moves_black(i,depth,root_list_surplus_moves);

			// записываем оценку в корневой список ходов
			root_list_surplus_moves.set_sorting_score(i, 1000000);

			continue;
		}

		// ставим флаг что есть легальные ходы и засчитываем позицию
		flag_is_legal_moves = true;
		nodes_root = nodes_root + 1;

#if SAVE_LINE
		// печатаем информацию о рассматриваемом ходе и глубине перебора
		pv_save.print_currmove(root_list_surplus_moves,nodes_0,i,depth_max);

		// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
		pv_save.update_PV_current(i,depth,root_list_surplus_moves);

		// обновляем хеш ключ
		zobrist.modification_random_key(0,i,root_list_surplus_moves
										,bitboard.get_d_undo(depth));

		// TEST-----------
		// тут ключ считается полностью по новой
		// тестировал хеш-таблицу
		// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// записываем ключ для последующего анализа на повтор позиций
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth);

#endif//#if SAVE_LINE 

		depth_max_new = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// если король под шахом то увеличиваем максимальную глубину перебора 
		search.extension_for_chek_black(depth_max,depth_max_new,flag_check
										,root_list_surplus_moves);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

		const __int32 description_move = ( root_list_surplus_moves.get_move(i) >> 6 ) & 63;

//_scout_scout black
#if (ALPHA_BETA_ON && SAVE_LINE)
		if (i == 0)
#else
		if (1)
#endif//#if ALPHA_BETA_ON

		{
			alpha_scout = alpha;
			flag_pv_f_scout = PV_YES;
		}
		else
		{
			alpha_scout = beta - 1;
			flag_pv_f_scout = PV_NO;
		}

		value = search.white(alpha_scout,beta,depth_max_new,new_depth,description_move,
							 flag_pv_f_scout,flag_check,NULL_NO);

#if (ALPHA_BETA_ON && SAVE_LINE)
		if ( (value < beta) && (i != 0) )
		{
			alpha_scout = alpha;
			flag_pv_f_scout = PV_YES;

			value = search.white(alpha_scout,beta,depth_max_new,new_depth,description_move,
								 flag_pv_f_scout,flag_check,NULL_NO);
		}
#endif//#if ALPHA_BETA_ON

		//восстанавливаем позицию
		bitboard.undo_moves_black(i,depth,root_list_surplus_moves);

		// записываем оценку в корневой список ходов
		root_list_surplus_moves.set_sorting_score(i, value);


#if SAVE_LINE

		// восстанавливаем хеш-ключ доски
		bitboard.set_hash_key(key_undo);

		//ищем лучшую позицию. за черных ищем минимум
		if (value < value_min)
		{
			value_min = value;
			i_hash_move = i; // номер кешируемого хода

			// TEST
			from = ((root_list_surplus_moves.get_move(i) >> 12) & 63);
			to = ((root_list_surplus_moves.get_move(i) >> 18) & 63);
  
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			// черные пытаются улучшить бету
			if (value < beta)
			{
				beta = value;
				flag_save_best = true;

				// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
				pv_save.update_PV_best_point(pv_best_point);

				// TEST
				// проверяем совпадение варианта и оценки
				if (value != pv_save.get_pv_best().score_move)
				{
					std::cout << "ERROR black value " ;
					std::cout << "\n";
				}
			}
#else//#if ALPHA_BETA_ON

			flag_save_best = 1;

			// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
			pv_save.update_PV_best_point(pv_best_point);

			// TEST
			// проверяем совпадение варианта и оценки
			if (value != pv_save.get_pv_best().score_move)
			{
				std::cout << "ERROR black value " ;
				std::cout << "\n";
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// выводим ход который обдумываем делаем это только на нулевой глубине 
		// т.е. у самой поверхности
		if (search.get_stop_search() != 1)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// для теста вариантов
			pv_save.print_pv_line(0,root_list_surplus_moves,pv_save.get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// для рабочего режима
			pv_save.print_pv_line(0,root_list_surplus_moves,pv_best_point,nodes_root
								  ,i,depth_max,value_min,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 

	}

#if SAVE_LINE
	// TEST
	// проверяем восстановилась ли доска после прохождения глубины
	test_bitboard.compare_test_bitboard(bitboard);
#endif//#if SAVE_LINE 

	// если не одного хода сделано не было то производим статическую оценку позиции
	// это случай пата или мата
	if (flag_is_legal_moves == false)
	{
		value = 0;
		if (root_list_surplus_moves.king_black_check(bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);// может это слишком большое число?
		}

		// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
		pv_save.update_PV_best_max_depth(value,depth);

		return value;
	}


#if SAVE_LINE
	if (flag_save_best == true)
	{
		// теперь мы ее скопируем в структуру PV_best что бы передать на 
		// более высокий уровень
		pv_save.update_PV_best(pv_best_point);
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	//кэшируем лучший ход
	hash.save_best_hash_score_or_move(i_hash_move, bitboard.get_hash_key()
									  ,root_list_surplus_moves, 
									   value_min, UPPER, depth, depth_max);
#endif//#if HASH


	return value_min;
}

//	===================================================================
//
void Search_root::ini_global_loop
(
	unsigned __int8 depth_max//
)
{
	// инициализируем глобальные переменные модуля loop
	search.ini_global_loop();
}

//	===================================================================
//
void Search_root::ini_deep_time_terminate
(
	double deep_time_terminate_v//
)
{
	search.ini_deep_time_terminate(deep_time_terminate_v);
}

//	===================================================================
// возвращаем флаг остановки поиска. так если в глубине поиска получили команду
// на остановку то передаем ее наружу
bool Search_root::get_stop_search()
{
	return search.get_stop_search();
}

//	===================================================================
// возвращаем лучший вариант
struct PV_line & Search_root::get_pv_best()
{
	return pv_save.get_pv_best();
}

//	===================================================================
// возвращаем количество рассмотренных узлов
__int64 Search_root::get_nodes()
{
	return (nodes_root + search.get_nodes());// количество узлов;
}

//	===================================================================
// обнуляем количество рассмотренных узлов
void Search_root::set_nodes_in_0()
{
	nodes_root = 0;// количество узлов;
	search.set_nodes_in_0();
}

//	===================================================================
// откуда делается ход
unsigned __int8 Search_root::get_from()
{
	return from;
}

//	===================================================================
// куда делается ход
unsigned __int8 Search_root::get_to()
{
	return to;
}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// максимальная глубина, глубина перебора, доска,
#if TEST_L

void Search_root::list_print
(
	int depth,// текущая глубина
	class List_surplus_moves & list_surplus_moves// список возможных ходов
)
{
	Test_Loop.open ("_TEST_LOOP.txt",std::ios::app);	
	//Test_Loop.open ("_TEST_LOOP.txt");	
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "------------------------------------------------- "<<"\n";
	Test_Loop <<"depth= "<< depth << "\n";
	Test_Loop << "\n";
	Test_Loop << "Список возможных ходов фигуры "<<"\n";

	for (int i = 0 ; i < list_surplus_moves.get_end_list() ; i++)
	{	
		Test_Loop << "Номер хода по списку i= " << i << "\n";  
		Test_Loop << "Ходящая фигура " << pv_save.int_to_char_board(((list_surplus_moves.get_move(i)>>24)& 7),1)<< "\n";
		Test_Loop << "Оценка хода " << list_surplus_moves.get_sorting_score(i) << "\n";

		// вывод хода вида e2-e4
		//------------------------------
		Test_Loop << " " << (pv_save.x_to_notation(((list_surplus_moves.get_move(i)>>12)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.get_move(i)>>12)& 63)/8);//<< "\n";
		//Test_Loop<< "\n";
		Test_Loop << " " << (pv_save.x_to_notation(((list_surplus_moves.get_move(i)>>18)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.get_move(i)>>18)& 63)/8)<< "\n";   
		//------------------------------
		//тип хода 
		//0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка 4 – короткая рокировка 5 – взятие на проходе
		//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
		//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
		//25 - взятие с превращением в ферзь 
		Test_Loop << "вид хода " << ((list_surplus_moves.get_move(i)>>6)& 63)  << "\n";
		Test_Loop << "Взятая фигура " << pv_save.int_to_char_board(( list_surplus_moves.get_move(i) & 63),1)<< "\n";
		Test_Loop << "начальная положение " << ((list_surplus_moves.get_move(i)>>12)& 63)  << "\n";
		Test_Loop << "конечное положение " << ((list_surplus_moves.get_move(i)>>18)& 63)  << "\n";
     
		Test_Loop << "\n";
	}

	Test_Loop << "list_surplus_moves.end_captures= " <<list_surplus_moves.get_end_captures() << "\n";
	Test_Loop << "list_surplus_moves.end_list= " <<list_surplus_moves.get_end_list() << "\n";
	Test_Loop << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop.close();
}
#endif