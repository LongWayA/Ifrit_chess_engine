
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.02.2008  11.11.2009 11.07.10 24.5.2012

//!
// отключил предупреждения которые выдаются из за использования условия 
// для препроцессора #if #endif
// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 

#include <fstream>
#include <iostream>

#include "_search_switch.h"
#include "a_move_list_struct.h"
#include "iterative_deepening.h"
#include "search_root.h"
#include "search.h"
#include "evaluate.h"
#include "move_generation.h"
#include "killer_heuristic.h"
#include "history_heuristic.h"
#include "zobrist_hashing.h"
#include "pv_save.h"
#include "time_management.h"
#include "uci_engine_to_gui.h"
#include "b_search_const.h"

/// включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_D 0; 

/// включаем-выключаем(1-0) режим подсчета количества
/// записанных и считанных позиций
/// надо еще включить флаг TEST_WRITE_READ в j_Hash.h
#define TEST_WRITE_READ_WN 0 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*
 перебор с циклическим увеличением глубины

 циклически перебираем увеличивая глубину до тех пор пока не сработает останов.
 т.е. в начале на глубине 1 потом 2 и т.д.
 это нужно во первых для заполнения хеш-таблицы
 во вторых чтобы чувствовать время перебора. например если мы на предыдущей глубине
 считали секунду то на следующем в среднем в двадцать раз дольше.
*/
void Iterative_deepening::loop
(
	struct Position & position,// представление доски
	unsigned __int8 & depth_limit,
	double & time_move_search_limit
)
{

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
	//Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	const __int32 Alpha = -INFINITE_SCORE;
	const __int32 Beta  =  INFINITE_SCORE;

	__int32 value_root = 0;// оценка лучшего варианта и делаемого хода. 
	bool flag_mate = 0;// найден мат 
	__int64 nodes = 0;// количество рассмотренных узлов(позиций)

	__int32 test_eval = 0;// запоминаем оценку материала для контроля
	unsigned __int64 key_control = 0;   // запоминаем хеш-ключ для контроля

	// время смотрим в секундах
	double time_current = -1.0;  // сколько времени прошло при данной итерации
	double remember_time_current = 1.0;  // запоминаем

	__int64 nodes_y;             /// количество узлов на данной глубине
	__int64 nodes_previous;      /// запоминаем полное количество узлов
	double koff_deepening;       /// высчитываем насколько дольше по времени 
							 /// считается следующая глубина

	bool flag_stop_search;
	__int32 bestmove;
	__int32 old_bestmove;

	// на глубине обрубаем только при существенном превышении времени на расчет
	// сейчас это в ~ 5 раз
	Search::ini_deep_time_terminate( (time_move_search_limit * 15) );

	// считаем материал. в дальнейшем только корректируем оценку при взятиях.
	position.value_white_material = Evaluate::eval_white_material(position);
	position.value_black_material = Evaluate::eval_black_material(position);

	//TEST
	// для проверки что после возвращения оценка вернулась к первоначальному положению
    test_eval =  position.value_white_material 
		- position.value_black_material;

	// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
	Zobrist_hashing::start_position_random_key(position);

	//инициализируем хеш - массив повторов возрастающими числами 
	// что-бы исключить ложное срабатывание
	Zobrist_hashing::ini_hash_three(); 

	// записываем ключ для последующего анализа на повтор позиций
	Zobrist_hashing::save_hash_three(position.hash_key,0);

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << std::endl;

	//TEST
	// записываем ключ для контроля при выныривании с глубины
	key_control = position.hash_key;

	// TEST
	// здесь очищаем хеш-таблицу
	// инициализируем все ключи в хеш-таблице 0
	//   HASHM::Ini_hash_moves();

	// использую для тестирования подтверждением варианта
	// TEST
	//depth_max = depth_max - 1;

	// TEST
	//std::cout << "1111111 " << std::endl;

	// инициализируем глобальные переменные
	Search::ini_global_loop();

#if SW_HISTORY
	// обнуляем историю
	History_heuristic::ini_history();
#endif // #if SW_HISTORY

	// инициализируем массив киллеров
	Killer_heuristic::ini_killer(); 

	// TEST
	//std::cout << "22222222222 " << std::endl;
	//std::cout << " depth_max " << depth_max << std::endl;//глубина

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
	// TEST
	// обнуляем счетчики
	HASHM::Test_hash_ini();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------


	// если глубина меньше максимальной и не нашли мат
	// и не было команды на остановку то продолжаем счет
	for (unsigned __int8 depth_max = 1; depth_max <= depth_limit; depth_max++)
	{
		// запоминаем полное количество узлов с предыдущей итерации т.е глубины
		nodes_previous = nodes;

		//сбрасываем счетчик позиций
		Search_root::set_nodes_in_0();

		// запускаем таймер
		Time_management::time_start();

		// просчитываем позицию
		if (position.colour_move == 1)
		{
			value_root = Search_root::white_root_searching_max_value
										(position,Alpha,Beta,depth_max);
		}
		else
		{
			value_root = Search_root::black_root_searching_min_value
										(position,Alpha,Beta,depth_max);
		}

		// останавливаем таймер
		Time_management::time_stop();

		// считаем промежуток времени
		time_current = Time_management::time_delta();

		// это я не помню для чего делал
        koff_deepening = time_current/remember_time_current;
		remember_time_current = time_current;

		// количество рассмотренных узлов
		nodes = Search_root::get_nodes();

		// количество узлов на слое считаем как полное количество узлов
		// минус полное количество узлов на предыдущей итерации
		nodes_y = nodes - nodes_previous ;

		flag_stop_search = false;

		// первый ход в линии варианта
		bestmove = PV_save::get_pv_best().move[0];

		// выходим по команде на остановку счета
		if (Search::get_stop_search() != 0)
		{
			if (depth_max > 1) flag_stop_search = true;
			break;
		}

		// печатаем информацию в поток
		Uci_engine_to_gui::print_pv_line
			(position,nodes,time_current,depth_max,flag_mate,nodes_y);

		// TEST
		//// выходим по команде на остановку счета
		//if (Search::get_stop_search() != 0)
		//{
		//	if (depth_max > 1) flag_stop_search = true;
		//	break;
		//}

		// запоминаем лучший ход
		old_bestmove = bestmove;

		// выходим по времени
		if ((time_move_search_limit != -1) 
			&& (time_move_search_limit < time_current) 
			&& (depth_max >= 4)
			)
		{
			break;
		}

		// выходим по мату
		if (flag_mate != 0)
		{
			break;
		}


		// тестируем параметры
		test_loop(position,value_root,test_eval,key_control);

//TEST
#if TEST_WRITE_READ_WN
		// TEST

		// сколько элементов в таблице и сколько использованно т.е. во сколько мы записали
		HASHM::TEST_Hash_index_print();

		// печатаем инфу об эффективности кеширования
		HASHM::Test_hash_print();
#endif // #if HASHM::TEST_WRITE_READ_WN


	}

if (flag_stop_search)
{
	flag_stop_search = false;

	// поиск на текущей глубине был прерван поэтому используем ход с предыдущей глубины
	bestmove = old_bestmove;
}

	// делаем ход (например bestmove e2e4)
	Uci_engine_to_gui::do_bestmove(bestmove);


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}

/*
проверка оценки главной линии, проверка оценки материала
проверяем хеш-ключ на корректность, 
контроль соответствия первого хода первому ходу линии варианта
*/ 
void Iterative_deepening::test_loop
(
	struct Position & position,// представление доски
	__int32 value_root,
 	const __int32 test_eval,// запоминаем оценку материала для контроля
	const unsigned __int64 key_control// запоминаем хеш-ключ для контроля
)
{
	//TEST
	// проверка оценки главной линии
	if (value_root != PV_save::get_pv_best().score_move)
	{
		std::cout << 
			"ERROR value_root != PV_save::get_pv_best().score_move(iterative_deepening.cpp)" 
			<< std::endl;

		std::cout << "value_root = "<< value_root << std::endl;
		std::cout << "PV_save::get_pv_best().score_move = " 
			<< PV_save::get_pv_best().score_move << std::endl;
	}

	//TEST
	// проверка оценки материала
	if (test_eval != (position.value_white_material - position.value_black_material))
	{
		std::cout << 
			"ERROR test_eval != bitboard->eval(iterative_deepening.cpp)" 
			<< std::endl;

		std::cout << "test_eval = "<< test_eval << std::endl;
		std::cout << "(position.value_white_material - position.value_black_material) = " 
			<< (position.value_white_material - position.value_black_material) << std::endl;

	}
 
	//TEST
	// проверяем хеш-ключ на корректность т.е. что после возвращения 
	// с глубины он вернулся к 
	// состоянию которое он имел до погружения на глубину
	if (key_control != position.hash_key)
	{
		std::cout <<std::endl;
		std::cout << "ERROR HESH Test_Deepening(iterative_deepening.cpp) " ;
		std::cout <<std::endl;

		std::cout << "key_control = "<< key_control << std::endl;
		std::cout << "position.hash_key = " 
			<< position.hash_key << std::endl;
	}

	//TEST
	// контроль соответствия первого хода первому ходу линии варианта
	if (Search_root::get_from() 
	!= ((PV_save::get_pv_best().move[0]>>12)& 63))
	{
		std::cout << "ERROR from(iterative_deepening.cpp)" <<std::endl;
	}

	if (Search_root::get_to() 
	!= ((PV_save::get_pv_best().move[0]>>18)& 63))
	{
		std::cout << "ERROR to(iterative_deepening.cpp)" <<std::endl;
	}

}


