
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009 25.12.2010 23.5.2012
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%

#include "a_position_struct.h"

/*
UCI protocol
http://wbec-ridderkerk.nl/html/UCIProtocol.html

The UCI protocol as publiced by Stefan-Meyer Kahlen (ShredderChess):
Description of the universal chess interface (UCI)    April 2004


GUI to engine:
--------------

* go
	start calculating on the current position set up with the "position" command.
	There are a number of commands that can follow this command, all will be 
	sent in the same string.
	If one command is not send its value should be interpreted as it would not 
	influence the search.

	* searchmoves  
	.... 

	* ponder 
	....

	* wtime 
		white has x msec left on the clock

	* btime 
		black has x msec left on the clock

	* winc 
		white increment per move in mseconds if x > 0

	* binc 
		black increment per move in mseconds if x > 0

	* movestogo 
      there are x moves to the next time control,
		this will only be sent if x > 0,
		if you don't get this and get the wtime and btime it's sudden death

	* depth 
		search x plies only.

	* nodes 
	....

	* mate 
	....

	* movetime 
		search exactly x mseconds

	* infinite
		search until the "stop" command. Do not exit the search without 
		being told so in this mode!

end link(конец цитаты)---------------------
*/

namespace Uci_go_parser
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

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
	void parsing
	(
		struct Position & position,// представление доски
		const std::string string_in,//строка команд
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

//////////////////////////////////////////////////////////////////////////////////////
//private:

/*
	разбираем команду go и инициализируем структуру go_parameters
*/
	void ini_struct_go_parameters
	(
		const std::string string_in//строка команд
	);

/*
	инициализируем параметры циклического перебора
	рассчитваем время и глубину поиска.
*/
	void ini_parameters_iterative_deepening
	(
		struct Position & position,// представление доски
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

/*
	рассчитываем максимальное время выделенное на обсчет текущей позиции
*/
	void ini_max_time_move_search
	(
		struct Position & position,// представление доски
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

/*
	проверяем что параметры структуры go_parameters инициализированны
*/
	void test_ini_struct_go_parameters();

};
