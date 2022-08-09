#include <iostream>
#include <fstream>
//#include <cstring>      // используем для работы со строкой (сравнение строк)
//#include <stdlib.h>

#include "c_Go.h"
#include "j_Hash.h"

#include "g_Search_root.h"
#include "k_Util.h"

// отключил предупреждения которые выдаются из за использования
// функции 'strtok'
// 'strtok': This function or variable may be unsafe. 
// Consider using strtok_s instead. To disable deprecation, 
// use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable : 4996)

//-----------------------------------------------------------------------------------
/*+
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009
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
extern class Util util;

//	===================================================================
void Go::parse_go
(
	class Chess_bitboard * const p_bitboard,//доска
	const __int8 string_in[]//строка команд
)
{
	const __int8 * ptr;// вспомогательный указатель

	//int mate = -1 ;

	double wtime_go_command = -1.0;// оставшееся время на игру за белых
	double btime_go_command = -1.0;// оставшееся время на игру за черных
	double winc_go_command = 0;//  добавление времени за ход для белых
	double binc_go_command = 0;//  добавление времени за ход для черных

	__int8 depth_max_go_command = -1;// глубина перебора
	double movetime_go_command = -1.0;// время на ход
	__int8 movestogo_go_command = -1;//количество ходов за определенное время
	bool infinite_go_command = false;// флаг бесконечного анализа


	// копируем строку string_in в строку string_in_l это нужно потому что в дальнейшем
	// строка будет разрушена при анализе
	strcpy (string_in_l,string_in);

	// функция strtok работает как разбиватель(разделитель на фрагменты) 
	// строки причем при этом она разрушает строку
	// при первом вызове задаем строку а потом она уже продолжает сама 
	ptr = strtok(string_in_l," "); // пропускаем "go"
	 
	// тут мы бежим по строке выделяя фрагменты отделенные пробелами
	for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) 
	{

		if (util.string_equal(ptr,"depth"))
		{//глубина перебора
			ptr = strtok(NULL," ");// следующий за depth фрагмент отделенный пробелом(здесь число)
			depth_max_go_command = atoi(ptr);// функция atoi переводит строку в целое число
		}
		else if (util.string_equal(ptr,"wtime"))
		{// время которым располагают белые
			ptr = strtok(NULL," ");
			wtime_go_command = double(atoi(ptr)) / 1000.0;// миллисекунды переводим в секунды (/ 1000.0)
		}
		else if (util.string_equal(ptr,"btime"))
		{// время которым располагают черные
			ptr = strtok(NULL," ");
			btime_go_command = double(atoi(ptr)) / 1000.0;
		}
		else if (util.string_equal(ptr,"winc"))
		{// добавочное время для белых
			ptr = strtok(NULL," ");
			winc_go_command = double(atoi(ptr)) / 1000.0;
		}
		else if (util.string_equal(ptr,"binc"))
		{// добавочное время для черных
			ptr = strtok(NULL," ");
			binc_go_command = double(atoi(ptr)) / 1000.0;
		}
		else if (util.string_equal(ptr,"movestogo"))
		{//количество ходов на которое выделено время
			ptr = strtok(NULL," ");
			movestogo_go_command = atoi(ptr);

		}
		else if (util.string_equal(ptr,"movetime"))
		{// время на ход
			ptr = strtok(NULL," ");
			movetime_go_command = double(atoi(ptr)) / 1000.0;

		}
		else if (util.string_equal(ptr,"infinite"))
		{// бесконечный анализ
			infinite_go_command = true;

		// пока делать не буду ----------------------------------------------
		//}
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
		//-------------------------------------------------------------------

		}

	}


	ptr = NULL;

	// увеличиваем возраст позиции
	hash.aging();

	/////////////////////////////////////////////////////////////////////////////////

	// время уже в секундах
	// получаем позицию и параметры перебора
	deepening(p_bitboard,depth_max_go_command,wtime_go_command,btime_go_command
		,winc_go_command,binc_go_command,movestogo_go_command
		,movetime_go_command,infinite_go_command);

}