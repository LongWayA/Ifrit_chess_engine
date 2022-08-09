
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009 25.12.2010

#define TEST_TIME 0 // включаем-выключаем(1-0) режим тестирования распределения
					// времени

#include <fstream>
#include <iostream>

#include "Go.h"
#include "ZObrist.h"
#include "Hash.h"


#if TEST_TIME
// TEST
std::ofstream Test_TIME;
#endif//#if TEST_TIME

	double wtime_go_command;     /// оставшееся время на игру за белых
	double btime_go_command;     /// оставшееся время на игру за черных
	double winc_go_command;      ///  добавление времени за ход для белых
	double binc_go_command;      ///  добавление времени за ход для черных

	__int8 depth_max_go_command; /// глубина перебора
	double movetime_go_command;  /// время на ход
	__int8 movestogo_go_command; /// количество ходов за определенное время
	bool infinite_go_command;    /// флаг бесконечного анализа


//	===================================================================
void Go::parse_go
(
	struct Position & position,// представление доски
	const std::string string_in//строка команд
)
{

	wtime_go_command = -1.0;// оставшееся время на игру за белых
	btime_go_command = -1.0;// оставшееся время на игру за черных
	winc_go_command = 0;//  добавление времени за ход для белых
	binc_go_command = 0;//  добавление времени за ход для черных

	depth_max_go_command = -1;// глубина перебора
	movetime_go_command = -1.0;// время на ход
	movestogo_go_command = -1;//количество ходов за определенное время
	infinite_go_command = false;// флаг бесконечного анализа

	double max_time_move_search = -1.0;// время выделенное на ход
	unsigned __int8 depth_max = 0;//максимальная глубина перебора

	// инициализируем параметры игры
	ini_go_command(string_in);

	// проверяем что параметры игры инициализированны
	test_ini_go_command();


	// инициализируем параметры циклического перебора
	ini_deepening(position,depth_max,max_time_move_search);

	// увеличиваем возраст позиции
	Hash::aging();

	/////////////////////////////////////////////////////////////////////////////////

	// время уже в секундах
	// получаем позицию и параметры перебора
	deepening(position,depth_max,max_time_move_search);

}



//	===================================================================
void Go::ini_go_command
(
	const std::string string_in//строка команд
)
{
	__int16 find;
	__int16 find_w;
	__int16 find_b;
	__int16 find_w2;
	__int16 find_b2;
	__int16 delta_find_w;
	__int16 delta_find_b;


	// неограниченный поиск. используется в режиме анализа
	//> go infinite ----------------------------------------------- 

    // находим первое вхождение подстроки "infinite"
	find = (int)string_in.find("infinite");

	if (find != -1)
	{
		infinite_go_command = true;

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "infinite_go_command = " << infinite_go_command;
		//std::cout << "\n";
	}
	// > ----------------------------------------------------------


	// поиск на заданную глубину
    // > go depth 5 -----------------------------------------------

    // находим первое вхождение подстроки "depth"
	find = (int)string_in.find("depth");

	if (find != -1)
	{

		// пропускаем надпись depth и выходим на число
		find = find + 6;

		// выделяем подстроку содержащую число
		std::string string_value = string_in.substr(find);

        // функция atoi переводит массив символов в целое число;
		depth_max_go_command = atoi(string_value.c_str() );

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "depth_max_go_command = " << (int)depth_max_go_command;
		//std::cout << "\n";

	}
	// > ----------------------------------------------------------


	//время на ход
	// > go movetime 300000 ---------------------------------------

    // находим первое вхождение подстроки "movetime"
	find = (int)string_in.find("movetime");

	if (find != -1)
	{

		// пропускаем надпись movetime и выходим на число
		find = find + 9;

		// выделяем подстроку содержащую число
		std::string string_value = string_in.substr(find);

        // функция atoi переводит массив символов в целое число;
		// миллисекунды переводим в секунды (/ 1000.0)
		movetime_go_command = atoi(string_value.c_str())/ 1000.0;

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "movetime_go_command = " << (int)movetime_go_command;
		//std::cout << "\n";

	}

	// > ----------------------------------------------------------

	//время на определенное количество ходов
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
		movestogo_go_command = atoi(string_value.c_str());

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "movestogo_go_command = " << (int)movestogo_go_command;
		//std::cout << "\n";

	}

	// > ----------------------------------------------------------



	// команды для блица типа 5 мин либо 5 мин + добавление за ход в секундах
	//for game in 5min. 
    //> go btime 300000 wtime 300000 ------------------------------

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

		delta_find_w = find_w2 - find_w;

		// выделяем подстроку содержащую число
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		// миллисекунды переводим в секунды (/ 1000.0)
		wtime_go_command = double(atoi(string_value_w.c_str())) / 1000.0;

		// ТЕСТ печатаем число
		//std::cout << "wtime_go_command = " << wtime_go_command;
		//std::cout << "\n";

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

		delta_find_b = find_b2 - find_b;

		// выделяем подстроку содержащую число
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		// миллисекунды переводим в секунды (/ 1000.0)
		btime_go_command = double(atoi(string_value_b.c_str())) / 1000.0;

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "btime_go_command = " << btime_go_command;
		//std::cout << "\n";

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

		delta_find_w = find_w2 - find_w;

		// выделяем подстроку содержащую число
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		// миллисекунды переводим в секунды (/ 1000.0)
		winc_go_command = double(atoi(string_value_w.c_str())) / 1000.0;

		// ТЕСТ печатаем число
		//std::cout << "winc_go_command = " << winc_go_command;
		//std::cout << "\n";

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

		delta_find_b = find_b2 - find_b;

		// выделяем подстроку содержащую число
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		// миллисекунды переводим в секунды (/ 1000.0)
		binc_go_command = double(atoi(string_value_b.c_str())) / 1000.0;

		// ТЕСТ печатаем выделенную подстроку
		//std::cout << "binc_go_command = " << binc_go_command;
		//std::cout << "\n";

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

//	===================================================================
void Go::ini_deepening
(
	struct Position & position,// представление доски
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{

	unsigned __int16 cnt_move ;// количество уже сделанных в партии полуходов
	__int16 limit_cnt_move ;// сколько ходов осталось сделать

	/*
		это распределение времени тоже не очень удачно и кроме того 
		все как то запутанно
		нужно все четко описать исходя из данных переменых:

	*/
 
		// TEST ---------
		//std::cout << "depth_max_go_command = " << (int)depth_max_go_command << "\n";
		//std::cout << "infinite_go_command = " << infinite_go_command << "\n";
		//std::cout << "movetime_go_command = " << movetime_go_command << "\n";
		//std::cout << "movestogo_go_command = " << (int)movestogo_go_command << "\n";

		//std::cout << "winc_go_command = " << winc_go_command << "\n";
		//std::cout << "binc_go_command = " << binc_go_command << "\n";
		//std::cout << "wtime_go_command = " << wtime_go_command << "\n";
		//std::cout << "btime_go_command = " << btime_go_command << "\n";
		// ---------------

		// TEST
		//std::cout << "koff_deepening = " << koff_deepening << "\n";

	// TEST
#if TEST_TIME
	Test_TIME.open ("_TEST_TIME.txt",std::ios::app);
	Test_TIME << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";
#endif//#if TEST_TIME

	//поиск на заданную глубину
	// > go depth 5 
	if (depth_max_go_command != -1)
	{
		// TEST
		//std::cout << "depth_max_go_command " << "\n";

		// поиск на заданную глубину
		depth_max = depth_max_go_command;
	}

	// бесконечный счет. у меня завершается на глубине 50.
	// > go infinite
	else if (infinite_go_command != false) 
	{
		// TEST
		//std::cout << "infinite_go_command " << "\n";

		// предел при бесконечном анализе. 50 полу ходов думаю достаточно
		depth_max = 50;
	}

	// время на ход
	// > go movetime 300000 
	else if (movetime_go_command != -1)	
	{
		// TEST
		//std::cout << "movetime_go_command " << "\n";

		//задаем время на ход
		max_time_move_search = movetime_go_command;
		depth_max = 50;
	}

	// время на определенное количество ходов
	// > go movestogo 40 wtime 300000 btime 300000 
	//else if (movestogo_go_command != -1) 
	//{
	//// похоже на то что переходники не считают 
	//// сколько ходов осталось, т.е. не уменьшают movestogo
 //   // отсюда просрочки на ровном месте. так что пока это отключил
 //   //----------------------------------------------------------------------------------
	//	//// TEST
	//	////std::cout << "movestogo_go_command " << "\n";

	//	//// время на определенное количество ходов
	//	//// оболочка(например Арена) сама считает сколько ходов 
	//	//// осталось, т.е. уменьшает movestogo

	//	//// количество полуходов сделанных в партии
	//	//cnt_move = ZObrist::get_save_cnt_move();

	//	//// это оставшееся количество ходов
	//	//limit_cnt_move = movestogo_go_command;

	//	//// TEST
	//	////std::cout << "cnt_move = " << cnt_move << "\n";
	//	////std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";

	//	//// оставляем некоторый запас
	//	//if (limit_cnt_move < 5)
	//	//{
	//	//	limit_cnt_move = 5;
	//	//}

	//	//// время оставшееся на партию делим на количество ходов 
	//	//// нужных для достижения контроля. потом будет прибавка времени
	//	//if (bitboard.colour_move == 1)// 1-ход белых 0- ход черных
	//	//{

	//	//	max_time_move_search = winc_go_command +  wtime_go_command/limit_cnt_move;
	//	//}
	//	//else
	//	//{
	//	//	max_time_move_search = binc_go_command +  btime_go_command/limit_cnt_move;
	//	//}

 //	//	depth_max = 50;
 //   //----------------------------------------------------------------------------------

 //   //----------------------------------------------------------------------------------
	//	// количество полуходов сделанных в партии
	//	cnt_move = ZObrist::get_save_cnt_move();

	//	// мы предполагаем что все станет ясно за определенное количество ходов (80)
	//	// это оставшееся количество ходов
	//	limit_cnt_move = 80 - cnt_move/2 ;

	//	// время на первые 60 ходов примерно одинаковое
	//	// после 60 го хода идет уменьшение времени на ход что бы 
	//	// оттянуть просрочку по времени
	//	if (limit_cnt_move < 20)
	//	{
	//		limit_cnt_move = 20;
	//	}

	//	//limit_cnt_move = 20;

	//	if (bitboard.get_colour_move() == 1)// 1-ход белых 0- ход черных
	//	{

	//		max_time_move_search = winc_go_command + wtime_go_command/limit_cnt_move;
	//	}
	//	else
	//	{
	//		max_time_move_search = binc_go_command + btime_go_command/limit_cnt_move;
	//	}

 //		depth_max = 50;
 //   //----------------------------------------------------------------------------------
	//}



	// команда для блица типа 5 мин либо 5 мин + добавление за ход в секундах
	// > go btime 300000 wtime 300000 
	else if ( (wtime_go_command != -1.0) || (btime_go_command != -1.0)) 
	{
		// TEST
		//std::cout << "wtime_go_command or btime_go_command " << "\n";
		//оставшееся время на игру

#if TEST_TIME
		// TEST
		Test_TIME << "wtime_go_command = " << wtime_go_command << "\n";
		Test_TIME << "btime_go_command = " << btime_go_command << "\n";

		// TEST
		Test_TIME << "winc_go_command = " << winc_go_command << "\n";
		Test_TIME << "binc_go_command = " << binc_go_command << "\n";
#endif//#if TEST_TIME

		// количество полуходов сделанных в партии
		cnt_move = ZObrist::get_save_cnt_move();

#if TEST_TIME
		// TEST
		Test_TIME << "cnt_move = " << cnt_move << "\n";
#endif//#if TEST_TIME

/*
идею распределения опишу на примере когда дана 1 минута на партию

1 мин = 60 сек
время распределяем так что на первые 40 ходов 40 сек., а на доигрывание 20 сек.
60 = 40 + 20
так что первые 40 ходов делаются примерно ход в секунду
а в дальнейшем т.е. после 40-го хода идет уменьшение времени на ход
*/
		limit_cnt_move = 60 - cnt_move/2 ; // 80

#if TEST_TIME
		// TEST
		Test_TIME << "limit_cnt_move 1 = " << limit_cnt_move << "\n";
#endif//#if TEST_TIME

		// время на первые 60 ходов примерно одинаковое
		// после 60 го хода идет уменьшение времени на ход что бы 
		// оттянуть просрочку по времени
		if (limit_cnt_move < 20)
		{
			limit_cnt_move = 20;
		}

#if TEST_TIME
		// TEST
		Test_TIME << "limit_cnt_move 2 = " << limit_cnt_move << "\n";
#endif//#if TEST_TIME

		if (position.colour_move == 1)// 1-ход белых 0- ход черных
		{

			max_time_move_search = winc_go_command * 0.2 
				+ wtime_go_command/limit_cnt_move;
		}
		else
		{
			max_time_move_search = binc_go_command * 0.2 
				+ btime_go_command/limit_cnt_move;
		}

 		depth_max = 50;
	}

#if TEST_TIME
	// TEST
	Test_TIME << "max_time_move_search 1 = " << max_time_move_search << "\n";
#endif//#if TEST_TIME

	if (max_time_move_search != -1)
	{
		// мы сравниваем с временем за который уже посчитали на данной глубине.
		// предполагаем что на следующем уровне время счета будет в несколько раз
		// больше. поэтому мы делим время на эту цифру что бы не было просрочки
		max_time_move_search = max_time_move_search / 3;
	}

#if TEST_TIME
	// TEST
	Test_TIME << "max_time_move_search 2 = " << max_time_move_search << "\n";

	// TEST
	Test_TIME << "deep_time_terminate = " << (max_time_move_search * 15) << "\n";

	// TEST
	Test_TIME.close();
#endif//#if TEST_TIME

	// TEST
	//std::cout << "koff max_time_move_search = " << max_time_move_search << "\n";

}

//	===================================================================
void Go::test_ini_go_command()
{

	//
	if(
		   (wtime_go_command == -1.0 && btime_go_command == -1.0) 
		&& (depth_max_go_command == -1)
		&& (movetime_go_command == -1.0)
		&& (infinite_go_command == false)
	  )
	{
		std::cout << "Ifrit: illegal go command. I am terminated." << "\n";

		Hash::hash_size_free();
		exit(1);
	}
}