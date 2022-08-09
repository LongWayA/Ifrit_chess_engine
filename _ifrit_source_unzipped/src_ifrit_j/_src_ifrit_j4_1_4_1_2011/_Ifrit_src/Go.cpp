#include <iostream>
#include <fstream>
//#include <cstring>      // используем для работы со строкой (сравнение строк)
//#include <stdlib.h>

#include "Go.h"
#include "Hash.h"

#include "Search_root.h"

//-----------------------------------------------------------------------------------
/*+
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009 25.12.2010
*/
//-----------------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%

 обрабатываем команду на старт

// разбираем команду go и устанавливаем режим игры go command 

 команды для блица типа 5 мин либо 5 мин + добавление за ход в секундах
 > go btime 300000 wtime 300000 
 for game in 5min. 

 > go btime 302000 wtime 302000 winc 2000 binc 2000 
 with incremental clock 5 min + 2 sec. 

 время на определенное количество ходов
 > go movestogo 40 wtime 300000 btime 300000 
 for 40 moves in 5 min. 
 причем арена выдает в виде
 go wtime 300000 btime 300000 winc 0 binc 0 movestogo 40

 это понятно что для анализа
 > go infinite 
 for analysing. 

 время на ход говорят что фриц не поддерживает а по моему все он держит надо проверять
 > go movetime 300000 
 Exactly 5min for the next move, not supported by Fritz. 

 поиск на заданную глубину
 > go depth 5 
*/


extern class Hash hash;

//	===================================================================
void Go::parse_go
(
	class Chess_bitboard & bitboard,//доска
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

	// 
	ini_go_command(string_in);

	//
	if(
		   (wtime_go_command == -1.0 && btime_go_command == -1.0) 
		&& (depth_max_go_command == -1)
		&& (movetime_go_command == -1.0)
		&& (infinite_go_command == false)
	  )
	{
		std::cout << "Ifrit: illegal go command. I am terminated." << "\n";

		hash.hash_size_free();
		exit(1);
	}

	// увеличиваем возраст позиции
	hash.aging();

	/////////////////////////////////////////////////////////////////////////////////

	// время уже в секундах
	// получаем позицию и параметры перебора
	deepening(bitboard);

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