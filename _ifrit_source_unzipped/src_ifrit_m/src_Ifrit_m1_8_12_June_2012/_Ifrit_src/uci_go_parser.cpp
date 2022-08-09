
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009 25.12.2010 23.5.2012

#include <fstream>
#include <iostream>

#include "uci_go_parser.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "a_go_parameters_struct.h"

#define TEST_TIME 0 // включаем-выключаем(1-0) режим тестирования распределения
					// времени

#if TEST_TIME
// TEST
std::ofstream Test_TIME;
#endif//#if TEST_TIME

// структура содержащая параметры команды на обсчет (go)
struct Go_parameters go_parameters;

/*
	разбираем команду go и определяем параметры дальнейшего поиска.

	команды для блица типа 5 мин
	> go btime 300000 wtime 300000 
	for game in 5min. 

	команды для блица типа 5 мин + добавление за ход в секундах(в примере 2 сек)
	> go btime 302000 wtime 302000 winc 2000 binc 2000 
	with incremental clock 5 min + 2 sec. 

	время на определенное количество ходов (в примере 5 мин на 40 ходов)
	go movestogo 40 wtime 300000 btime 300000 
	for 40 moves in 5 min. 
	причем арена выдает в виде
	> go wtime 300000 btime 300000  movestogo 40

	бесконечный счет. применяется в случае анализирования позиции
	> go infinite 
	for analysing. 

	время на ход(в примере дается 5 мин на ход) 
	> go movetime 300000 
	Exactly 5min for the next move

	поиск на заданную глубину (в примере 5 полуходов)
	> go depth 5 

*/
void Uci_go_parser::parsing
(
	struct Position & position,// представление доски
	const std::string string_in,//строка команд
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{

	// инициализируем параметры команды на обсчет
	ini_struct_go_parameters(string_in);

	// проверяем что параметры игры инициализированны
	test_ini_struct_go_parameters();

	// инициализируем параметры циклического перебора
	ini_parameters_iterative_deepening(position,depth_max,max_time_move_search);

}

/*
	разбираем команду go и инициализируем структуру go_parameters
*/
void Uci_go_parser::ini_struct_go_parameters
(
	const std::string string_in//строка команд
)
{

	go_parameters.wtime = -1.0;// оставшееся время на игру за белых
	go_parameters.btime = -1.0;// оставшееся время на игру за черных
	go_parameters.winc = 0;//  добавление времени за ход для белых
	go_parameters.binc = 0;//  добавление времени за ход для черных

	go_parameters.depth = -1;// глубина перебора
	go_parameters.movetime = -1.0;// время на ход
	go_parameters.movestogo = -1;//количество ходов за определенное время
	go_parameters.infinite = false;// флаг бесконечного анализа

	// использую во время синтаксического анализа команды
	__int16 find;
	__int16 find_w;
	__int16 find_w2;
	__int16 delta_find_w;
	__int16 find_b;
	__int16 find_b2;
	__int16 delta_find_b;

	// неограниченный поиск. используется в режиме анализа
	//> go infinite

    // находим первое вхождение подстроки "infinite"
	find = (int)string_in.find("infinite");

	if (find != -1)
	{
		go_parameters.infinite = true;

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "go_parameters.infinite = " << go_parameters.infinite;
		//std::cout << std::endl;
	}

	// поиск на заданную глубину
    // > go depth 5

    // находим первое вхождение подстроки "depth"
	find = (int)string_in.find("depth");

	if (find != -1)
	{

		// пропускаем надпись depth и выходим на число
		find = find + 6;

		// выделяем подстроку содержащую число
		std::string string_value = string_in.substr(find);

        // функция atoi переводит массив символов в целое число;
		go_parameters.depth = atoi(string_value.c_str() );

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "go_parameters.depth = " << (int)go_parameters.depth;
		//std::cout << std::endl;

	}

	//время на ход
	// > go movetime 300000

    // находим первое вхождение подстроки "movetime"
	find = (int)string_in.find("movetime");

	if (find != -1)
	{

		// пропускаем надпись movetime и выходим на число
		find = find + 9;

		// выделяем подстроку содержащую число
		std::string string_value = string_in.substr(find);

        // функция atoi переводит массив символов в целое число;
		go_parameters.movetime = atoi(string_value.c_str());

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "go_parameters.movetime = " << (int)go_parameters.movetime;
		//std::cout << std::endl;

	}

	// > ----------------------------------------------------------
	// количество ходов за определенное время
	//> go movestogo 40 wtime 300000 btime 300000 
	//for 40 moves in 5 min. 
	//причем арена выдает в виде
	//go wtime 300000 btime 300000 winc 0 binc 0 movestogo 40

    // находим первое вхождение подстроки "movestogo"
	find = (int)string_in.find("movestogo");

	if (find != -1)
	{

		// пропускаем надпись movestogo и выходим на число
		find = find + 10;

		// выделяем подстроку содержащую число
		std::string string_value = string_in.substr(find);

        // функция atoi переводит массив символов в целое число;
		go_parameters.movestogo = atoi(string_value.c_str());

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "go_parameters.movestogo = " << (int)go_parameters.movestogo;
		//std::cout << std::endl;

	}

	// > ----------------------------------------------------------
	// команды для блица типа 5 мин либо 5 мин + добавление за ход в секундах
	//for game in 5min. 
    //> go btime 300000 wtime 300000

    // находим первое вхождение подстроки "wtime"
	find_w = (int)string_in.find("wtime");

	if (find_w != -1)
	{
		// пропускаем надпись wtime и выходим на число
		find_w = find_w + 6;
		find_w2 = find_w;

		while ( string_in.at(find_w2) != ' ' )
		{
			find_w2 = find_w2 + 1;

			if( find_w2 >= (int)string_in.length() ) break;
		}

		// находим длину подстроки содержащую число
		delta_find_w = find_w2 - find_w;

		// выделяем подстроку содержащую число
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		//
		go_parameters.wtime = double(atoi(string_value_w.c_str()));

		// ТЕСТ печатаем число
		//std::cout << "go_parameters.wtime = " << go_parameters.wtime;
		//std::cout << std::endl;

	}

    // находим первое вхождение подстроки "btime"
	find_b = (int)string_in.find("btime");

	if (find_b != -1)
	{

		// пропускаем надпись btime и выходим на число
		find_b = find_b + 6;
		find_b2 = find_b;

		while ( string_in.at(find_b2) != ' ' )
		{
			find_b2 = find_b2 + 1;

			if( find_b2 >= (int)string_in.length() ) break;
		}

		// находим длину подстроки содержащую число
		delta_find_b = find_b2 - find_b;

		// выделяем подстроку содержащую число
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		//
		go_parameters.btime = double(atoi(string_value_b.c_str()));

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "go_parameters.btime = " << go_parameters.btime;
		//std::cout << std::endl;

	}


	//> go btime 302000 wtime 302000 winc 2000 binc 2000 
	// with incremental clock 5 min + 2 sec. 

    // находим первое вхождение подстроки "winc"
	find_w = (int)string_in.find("winc");

	if (find_w != -1)
	{
		// пропускаем надпись winc и выходим на число
		find_w = find_w + 5;
		find_w2 = find_w;

		while ( string_in.at(find_w2) != ' ' )
		{
			find_w2 = find_w2 + 1;

			if( find_w2 >= (int)string_in.length() ) break;

		}

		// находим длину подстроки содержащую число
		delta_find_w = find_w2 - find_w;

		// выделяем подстроку содержащую число
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		//
		go_parameters.winc = double(atoi(string_value_w.c_str()));

		// ТЕСТ печатаем число
		//std::cout << "go_parameters.winc = " << go_parameters.winc;
		//std::cout << std::endl;

	}

    // находим первое вхождение подстроки "binc"
	find_b = (int)string_in.find("binc");

	if (find_b != -1)
	{
		// пропускаем надпись binc и выходим на число
		find_b = find_b + 5;
		find_b2 = find_b;

		while ( string_in.at(find_b2) != ' ' )
		{
			find_b2 = find_b2 + 1;

			if( find_b2 >= (int)string_in.length() ) break;
		}

		// находим длину подстроки содержащую число
		delta_find_b = find_b2 - find_b;

		// выделяем подстроку содержащую число
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		//
		go_parameters.binc = double(atoi(string_value_b.c_str()));

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "go_parameters.binc = " << go_parameters.binc;
		//std::cout << std::endl;

	}

	// > ----------------------------------------------------------


		// пока делать не буду ----------------------------------------------
		//else if (util.string_equal(ptr,"mate"))
		//{
		//	ptr = strtok(NULL," ");
		//	mate = atoi(ptr);
		//}
		//else if (util.string_equal(ptr,"nodes"))
		//{
		//	ptr = strtok(NULL," ");
		//	nodes = my_atoll(ptr);
		//}
		//else if (util.string_equal(ptr,"ponder"))
		//{
		//	ponder = true;
		//}
		//else if (util.string_equal(ptr,"searchmoves"))
		//{
		//}
		//-------------------------------------------------------------------

}

/*
	инициализируем параметры циклического перебора
	рассчитваем время и глубину поиска.
*/
void Uci_go_parser::ini_parameters_iterative_deepening
(
	struct Position & position,// представление доски
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{
	depth_max = MAX_DEPTH/2;

	//поиск на заданную глубину
	// > go depth 5 
	if (go_parameters.depth != -1)
	{
		// TEST
		//std::cout << "go_parameters.depth " << std::endl;

		// поиск на заданную глубину
		depth_max = go_parameters.depth;
	}

	// бесконечный счет. у меня завершается при достижении глубины 50.
	// > go infinite
	else if (go_parameters.infinite != false) 
	{
		// TEST
		//std::cout << "go_parameters.infinite " << std::endl;
	}

	// время на ход
	// > go movetime 300000 
	else if (go_parameters.movetime != -1)	
	{
		// TEST
		//std::cout << "movetime_go_command " << std::endl;

		//задаем время на ход в секундах
		// миллисекунды переводим в секунды (/ 1000.0)
		max_time_move_search = go_parameters.movetime / 1000.0;
	}

	// команда для блица типа 5 мин либо 5 мин + добавление за ход в секундах
	// > go btime 300000 wtime 300000 
	else if ( (go_parameters.wtime != -1.0) || (go_parameters.btime != -1.0)) 
	{
		// TEST
		//std::cout << "go_parameters.wtime or go_parameters.btime " << std::endl;
		//оставшееся время на игру

		// 	рассчитываем максимальное время выделенное на обсчет текущей позиции
		ini_max_time_move_search(position,depth_max,max_time_move_search);
	}
}

/*
 рассчитываем максимальное время выделенное на обсчет текущей позиции

 идею распределения опишу на примере когда дана 1 минута на партию
 1 мин = 60 сек
 время распределяем так что на первые 40 ходов 40 сек., а на доигрывание 20 сек.
 60 = 40 + 20
 так что первые 40 ходов делаются примерно ход в секунду
 а в дальнейшем т.е. после 40-го хода идет уменьшение времени на ход

 Почему пришел именно к такому алгоритму я уже не помню.
 Считаем, что 60 ходов нам хватит, чтобы определится в партии.
 Таким образом, на 60 ходов распределение времени на ход одинаковое,
 а потом идет уменьшение.
 Оставшееся время делим на количество оставшихся до 60 ходов, 
 однако их не меньше 20.
 Добавочное время делим на два.
 То что в итоге получилось делим еще на три.

 Понятно, что это место надо будет дорабатывать.

*/
void Uci_go_parser::ini_max_time_move_search
(
	struct Position & position,// представление доски
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{
	unsigned __int16 cnt_half_move_already_made;// количество уже сделанных в партии полуходов
	__int16 cnt_move_necessary_to_make;// сколько ходов осталось сделать

// TEST
#if TEST_TIME
	Test_TIME.open ("_TEST_TIME.txt",std::ios::app);
	Test_TIME << " "<< std::endl;
	Test_TIME << "start ini"<< std::endl;
#endif//#if TEST_TIME

// TEST
#if TEST_TIME
	Test_TIME << "go_parameters.wtime = " << go_parameters.wtime << std::endl;
	Test_TIME << "go_parameters.btime = " << go_parameters.btime << std::endl;

	Test_TIME << "go_parameters.winc = " << go_parameters.winc << std::endl;
	Test_TIME << "go_parameters.binc = " << go_parameters.binc << std::endl;
#endif//#if TEST_TIME

	// количество полуходов сделанных в партии
	cnt_half_move_already_made = Zobrist_hashing::get_save_cnt_move();

// TEST
#if TEST_TIME
	Test_TIME << "cnt_half_move_already_made = " << cnt_half_move_already_made << std::endl;
#endif//#if TEST_TIME

	cnt_move_necessary_to_make = 60 - cnt_half_move_already_made/2 ; // 80

// TEST
#if TEST_TIME
	Test_TIME << "cnt_move_necessary_to_make = " << cnt_move_necessary_to_make << std::endl;
#endif//#if TEST_TIME

	// время на первые 60 ходов примерно одинаковое
	// после 60 го хода идет уменьшение времени на ход что бы 
	// оттянуть просрочку по времени
	if (cnt_move_necessary_to_make < 20)
	{
		cnt_move_necessary_to_make = 20;
	}

// TEST
#if TEST_TIME
	Test_TIME << "limit to 20 cnt_move_necessary_to_make = " << cnt_move_necessary_to_make << std::endl;
#endif//#if TEST_TIME

	if (position.colour_move == 1)// 1-ход белых 0- ход черных
	{

		max_time_move_search = go_parameters.winc * 0.2 
			+ go_parameters.wtime/cnt_move_necessary_to_make;
	}
	else
	{
		max_time_move_search = go_parameters.binc * 0.2 
			+ go_parameters.btime/cnt_move_necessary_to_make;
	}

	// задаем время на ход в секундах
	// миллисекунды переводим в секунды (/ 1000.0)
	max_time_move_search = max_time_move_search / 1000.0;

// TEST
#if TEST_TIME
	Test_TIME << "max_time_move_search = " << max_time_move_search << std::endl;
#endif//#if TEST_TIME

	if (max_time_move_search != -1)
	{
		// мы сравниваем с временем за который уже посчитали на данной глубине.
		// предполагаем что на следующем уровне время счета будет в несколько раз
		// больше. поэтому мы делим время на эту цифру что бы не было просрочки
		max_time_move_search = max_time_move_search / 3;
	}

// TEST
#if TEST_TIME
	Test_TIME << "max_time_move_search/3 = " << max_time_move_search << std::endl;

	Test_TIME << "deep_time_terminate( it is max_time_move_search * 15) = "
		<< (max_time_move_search * 15) << std::endl;

	Test_TIME.close();
#endif//#if TEST_TIME
}

/*
	проверяем что параметры структуры go_parameters инициализированны
*/
void Uci_go_parser::test_ini_struct_go_parameters()
{
	//
	if(
		   (go_parameters.wtime == -1.0 && go_parameters.btime == -1.0) 
		&& (go_parameters.depth == -1)
		&& (go_parameters.movetime == -1.0)
		&& (go_parameters.infinite == false)
	  )
	{
		std::cout << "Ifrit: illegal go command. I am terminated." << std::endl;

		Transposition_table::hash_size_free();
		exit(1);
	}
}