
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.02.2008  11.11.2009 11.07.10

//!
//--------------------------------------
// отключил предупреждения которые выдаются из за использования условия 
// для препроцессора #if #endif
// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 

#include <fstream>
#include <iostream>

#include "c_Go.h"
#include "d_Search_root.h"
#include "e_Search.h"
#include "g_Evaluate.h"
#include "h_Killer.h"
#include "i_ZObrist.h"
#include "k_PV_save.h"
#include "l_Chess_bitboard.h"
#include "m_List_surplus_moves.h"
#include "n_Timer.h"

class Search_root search_root;
class Search search; 
class Evaluate evaluate;
class Killer killer;
class PV_save pv_save;
class List_surplus_moves root_list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
class Timer timer;

extern class ZObrist zobrist;
extern class Chess_bitboard bitboard;


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
//
void Go::deepening
(
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
	//Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	const __int32 Alpha = -9999999;
	const __int32 Beta  =  9999999;

	__int32 value_root = 0;// оценка лучшего варианта и делаемого хода. 
	bool flag_mate = 0;// найден мат 
	__int64 nodes = 0;// количество рассмотренных узлов(позиций)

	__int32 test_eval = 0;// запоминаем оценку материала для контроля
	unsigned __int64 key_control = 0;   // запоминаем хеш-ключ для контроля

	// время смотрим в секундах
	double time_current = -1.0;  // сколько времени прошло при данной итерации
	double remember_time_current = 1.0;  // запоминаем


	// на глубине обрубаем только при существенном превышении времени на расчет
	// сейчас это в ~ 5 раз
	search.ini_deep_time_terminate( (max_time_move_search * 15) );

	// считаем материал. в дальнейшем только корректируем оценку при взятиях.
	bitboard.set_value_white_material(evaluate.eval_white_material());
	bitboard.set_value_black_material(evaluate.eval_black_material());

	//TEST
	// для проверки что после возвращения оценка вернулась к первоначальному положению
    test_eval =  bitboard.get_value_white_material() 
		- bitboard.get_value_black_material();

	// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
	zobrist.start_position_random_key();

	//инициализируем хеш - массив повторов возрастающими числами 
	// что-бы исключить ложное срабатывание
	zobrist.ini_hash_three(); 

	// записываем ключ для последующего анализа на повтор позиций
	zobrist.save_hash_three(bitboard.get_hash_key(),0);

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << "\n";

	//TEST
	// записываем ключ для контроля при выныривании с глубины
	key_control = bitboard.get_hash_key();

	// TEST
	// здесь очищаем хеш-таблицу
	// инициализируем все ключи в хеш-таблице 0
	//   HASHM::Ini_hash_moves();

	// использую для тестирования подтверждением варианта
	// TEST
	//depth_max = depth_max - 1;

	// TEST
	//std::cout << "1111111 " << "\n";

	// инициализируем глобальные переменные
	search.ini_global_loop();

	//
	if (bitboard.get_colour_move() == 1)
	{  
		// белые 
		//Заполняем список избыточных ходов. Именно эти списки и составляют дерево перебора.
		root_list_surplus_moves.generator_captures_white(bitboard);// генерируем взятия и превращения
		root_list_surplus_moves.generator_move_white(bitboard);// генерируем остальные ходы
	}
	else
	{
		// черные
		//Заполняем список избыточных ходов. Именно эти списки и составляют дерево перебора.
		root_list_surplus_moves.generator_captures_black(bitboard);// генерируем взятия и превращения
		root_list_surplus_moves.generator_move_black(bitboard);// генерируем остальные ходы
	}

	// TEST
	//std::cout << "22222222222 " << "\n";
	//std::cout << " depth_max " << depth_max << "\n";//глубина

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
	// TEST
	// обнуляем счетчики
	HASHM::Test_hash_ini();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

	// если глубина меньше максимальной и не нашли мат и меньше 
	// 50 и не было команды на остановку то продолжаем счет
	for (unsigned __int8 depth = 1;

		// нет команды на конец счета
		( (depth <= depth_max) && // глубина меньше максимальной заданной на весь перебор
		(flag_mate == 0) && // нету мата
		(depth < 51) && // глубина меньше максимальной 
		(search.get_stop_search() == 0)// нет команды на остановку счета 
		);
	depth++
	)
	{
		// инициализируем массив киллеров
		killer.ini_killer(); 

		// запоминаем полное количество узлов с предыдущей итерации т.е глубины
		nodes_previous = nodes;

		//сбрасываем счетчик позиций
		search_root.set_nodes_in_0();

		// запускаем таймер
		timer.time_start();

		if (bitboard.get_colour_move() == 1)
		{
			value_root = search_root.white_root(Alpha,Beta,depth);
		}
		else
		{
			value_root = search_root.black_root(Alpha,Beta,depth);
		}

		// останавливаем таймер
		timer.time_stop();

		// считаем промежуток времени
		time_current = timer.time_delta();

        koff_deepening = time_current/remember_time_current;
		remember_time_current = time_current;

		nodes = search_root.get_nodes();

		// количество узлов на слое считаем как полное количество узлов
		// минус полное количество узлов на предыдущей итерации
		nodes_y = nodes - nodes_previous ;

		//
		test_loop(test_eval,key_control);

		// печатаем информацию в поток
		print_deepening_line(nodes,time_current,depth,value_root,flag_mate);

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
		// TEST

		// сколько элементов в таблице и сколько использованно т.е. во сколько мы записали
		HASHM::TEST_Hash_index_print();

		// печатаем инфу об эффективности кеширования
		HASHM::Test_hash_print();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

		// выходим по времени
		if ((max_time_move_search != -1) 
			&& (max_time_move_search < time_current) 
			&& (depth >= 4))
		{
			break;
		}

	}

	// делаем ход вида < bestmove e2e4 
	do_bestmove();

	// проверяем хеш-ключ на корректность и 
	// контроль соответствия первого хода первому ходу линии варианта
	test_end_deepening(key_control);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}

//	===================================================================
// делаем ход вида < bestmove e2e4 
// или при превращении в коня(к примеру) < bestmove d7d8N 
void Go::do_bestmove()
{
	std::cout << "bestmove ";
	std::cout << pv_save.x_to_notation(((search_root.get_pv_best().move[0]>>12)& 63)%8);
	std::cout << (1 + ((search_root.get_pv_best().move[0]>>12)& 63)/8);
	std::cout << pv_save.x_to_notation(((search_root.get_pv_best().move[0]>>18)& 63)%8);
	std::cout << (1 + ((search_root.get_pv_best().move[0]>>18)& 63)/8);

	// считываем тип хода
	__int32 d_m = ((search_root.get_pv_best().move[0]>>6)& 63);

	// если d_m больше 5 то значит имеем превращение пешки
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			// превращение пешки с взятием
			std::cout << pv_save.int_to_char_board((d_m - 20),0);
		}
		else
		{
			// превращение пешки
			std::cout << pv_save.int_to_char_board((d_m - 10),0);
		}
	}

	std::cout <<"\n";
}

//	===================================================================
// печатаем информацию в поток
void Go::print_deepening_line
(
	const __int64 nodes,// количество рассмотренных узлов(позиций)
	const double time_current,  // сколько времени прошло при данной итерации
	const unsigned __int8 depth,
	const __int32 value_root,// оценка лучшего варианта и делаемого хода.
	bool & flag_mate// найден мат 
 )
{

	__int8 inv;// флаг цвета хода

		std::cout << "info " ;
		std::cout << " nps " << (__int32)(nodes / time_current);// скорость из расчета по всем узлам на данной глубине
		std::cout << " depth " << (__int32)depth;//глубина

		// печатаем информацию о глубине форсированного варианта
		std::cout << " seldepth "<< (__int32)search_root.get_pv_best().depth_max;

		bitboard.get_colour_move() == 0 ? inv = -1 : inv = 1;

		// печатаем оценку. для мата специальное представление в виде дистанции
		if (inv * value_root > 900000)
		{ 
			flag_mate = 1;
			std::cout << " score mate "<< (__int32)search_root.get_pv_best().depth_max;// дистанция до мата в полуходах
		}
		else if (inv * value_root < -900000)
		{
			std::cout << " score mate "<< -(__int32)search_root.get_pv_best().depth_max;// дистанция до мата в полуходах
			flag_mate = 1;
		}
		else
		{
			std::cout << " score cp "<< inv * value_root;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
			flag_mate = 0;
		}
 
		// std::cout << " score cp "<< inv*value_root;// для теста матовой оценки

		// печатаем линию варианта
		//std::cout << " nodes " << (__int32)nodes; // полное количество узлов
		std::cout << " nodes "<< (__int32)nodes_y; // количество узлов на слое
		std::cout <<" pv ";

		for (unsigned __int8 n = 0; n < search_root.get_pv_best().depth_max; n++)
		{
			std::cout << pv_save.x_to_notation(((search_root.get_pv_best().move[n]>>12)& 63)%8);
			std::cout << (1 + ((search_root.get_pv_best().move[n]>>12)& 63)/8);
			std::cout << pv_save.x_to_notation(((search_root.get_pv_best().move[n]>>18)& 63)%8);
			std::cout << (1 + ((search_root.get_pv_best().move[n]>>18)& 63)/8);

			// считываем тип хода
			__int32 d_m = ((search_root.get_pv_best().move[n]>>6)& 63);

			// если d_m больше 5 то значит имеем превращение пешки
			if (d_m > 5)
			{
				if (d_m > 15)
				{
					std::cout << pv_save.int_to_char_board((d_m - 20),0);// превращение пешки с взятием
				}
				else
				{
					std::cout << pv_save.int_to_char_board((d_m - 10),0);// просто превращение пешки
				}
			}

			std::cout << " ";
		}

		std::cout <<"\n";
}

//	===================================================================
// 
void Go::test_end_deepening
(
		const unsigned __int64 key_control
)
{
	// TEST
	//std::cout << "cnt_move = " << cnt_move << "\n";
	//std::cout << "wtime = " << wtime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout <<"time_current = "<< time_current <<"\n";
	//std::cout <<"   " <<"\n";

	//TEST
	// проверяем хеш-ключ на корректность т.е. что после возвращения с глубины он вернулся к 
	// состоянию которое он имел до погружения на глубину
	if (key_control != bitboard.get_hash_key())
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}

	//TEST
	// контроль соответствия первого хода первому ходу линии варианта
	if (search_root.get_from() 
	!= ((search_root.get_pv_best().move[0]>>12)& 63))
	{
		std::cout << "ERROR from(in Iterative_Deepening.cpp)" <<"\n";
	}

	if (search_root.get_to() 
	!= ((search_root.get_pv_best().move[0]>>18)& 63))
	{
		std::cout << "ERROR to(in Iterative_Deepening.cpp)" <<"\n";
	}
}


//	===================================================================
// 
void Go::test_loop
(
 	const __int32 test_eval,// запоминаем оценку материала для контроля
	const unsigned __int64 key_control// запоминаем хеш-ключ для контроля
)
{
	//TEST
	// проверка оценки материала
	if (test_eval != (bitboard.get_value_white_material() - bitboard.get_value_black_material()))
	{
		std::cout << "ERROR test_eval != bitboard->eval(in Iterative_Deepening.cpp)" << "\n";
	}
 
	//TEST
	// проверяем хеш-ключ на корректность т.е. что после возвращения 
	// с глубины он вернулся к 
	// состоянию которое он имел до погружения на глубину
	if (key_control != bitboard.get_hash_key())
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}
}


