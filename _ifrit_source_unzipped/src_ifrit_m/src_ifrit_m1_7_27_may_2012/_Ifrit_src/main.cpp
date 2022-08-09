
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.09.2007 11.11.2009 19.05.2012

#include <iostream>
#include <fstream>
#include <assert.h>//TEST

#include "a_position_struct.h"
#include "uci_gui_to_engine.h"
#include "uci_engine_to_gui.h"
#include "b_move_generation_const.h"
#include "test_chess_bitboard.h"
#include "move_generation.h"
#include "uci_go_parser.h"
#include "transposition_table.h"
#include "zobrist_hashing.h"
#include "iterative_deepening.h"

//#define DEBUG

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%

 принимаем UCI команды от графической оболочки
 в рамках консольного ввода вывода

 организуем опрос входного консольного потока
 что бы не было задержек при вводе-выводе отключаем буферизацию
 программа реагирует только на команды в рамках UCI протокола
*/

/*
Getting Started
http://chessprogramming.wikispaces.com/Getting+Started

 Just getting started with a new engine? Congratulations! You are 
 in for a lot of fun, but there is a considerable amount of work 
 ahead of you.

 The foundation of a chess engine is the board representation. 
 This is the "back end" for the chess engine, which controls how 
 it keeps track of the board and the rules of the game. The very first 
 step to writing a chess engine is to write a complete, bug free board 
 representation that knows every rule of chess. While this can sometimes 
 be a pain, especially implementing the more complicated rules such as 
 castling and repetition draws, it is the backbone of the chess engine, 
 and your engine will not get far without it.

 When writing an engine, it is extremely important to write bug free code. 
 The best strategy when starting a new engine is to create a debugging 
 framework under it so that every single piece of code gets tested, no 
 matter how simple it looks. Many experienced engine authors have ended 
 up rewriting their engines because they have become unmanageable due to bugs.

 Once you have a nice solid foundation ready, you are ready to start 
 learning about the fun stuff: Search and Evaluation. These are the 
 "brains" behind a chess engine; what allows it to pick a good move.

 You'll probably also want to connect your program to a 
 GUI (Graphical User Interface). You are in luck, though; you don't 
 have to write your own. It is only necessary to understand some basic 
 text commands in order to communicate with the many available GUIs today. 
 Most engines use either the Chess Engine Communication Protocol or 
 Universal Chess Interface for this communication.

 If you want ideas and see how other programmers have done things, 
 take a look at some of the Open Source Engines. These can be very 
 helpful when translating rather vague algorithms into specific data 
 structures and code. Just be careful, and don't copy the code and say 
 it is your own! Clones are frowned upon by the computer chess community 
 as a whole.

 It is also a very good idea to join some of these Computer Chess Forums. 
 The chess programming community is very friendly and will help you out with 
 personalized advice. We are always happy to accept new members!

 Some other good computer chess references can be found in Recommended Reading.

end link(конец цитаты)---------------------
*/

/*
все пространства имен используемые в программе
пространства имен использую в качестве классов

namespace Evaluate
namespace History_heuristic
namespace Iterative_deepening
namespace Killer_heuristic
namespace Make_move
namespace Move_generation
namespace Move_ordering
namespace PV_save
namespace Search
namespace Search_quiescence
namespace Search_root
namespace Test_chess_bitboard
namespace Time_management
namespace Transposition_table
namespace Uci_engine_to_gui
namespace Uci_engine_to_gui
namespace Uci_fen_parser
namespace Uci_go_parser
namespace Uci_gui_to_engine
namespace WIN_32_TORD_ROMSTAD_BIOSKEY
namespace Zobrist_hashing


функции вне имен

int main(int argc, __int8 * argv[]) 

*/

int main
(
	int argc, __int8 * argv[]
) 
{
	__int32 command;
	struct Position position;// представление доски
	__int8 gui_string_command[LENGTH_STRING];// буфер куда помещаем строку из потока

	double max_time_move_search = -1.0;// время выделенное на ход
	unsigned __int8 depth_max = 0;//максимальная глубина перебора

/*
	Начальная инициализация программы.
	Непредсказуемость последовательности команд от оболочки с одной стороны
	и абсолютная необходимость начальной инициализации перед счетом заставила
	сделать часть инициализации независимой от команд оболочки.
*/

	// отключаем буферизацию входного и выходного потоков (по Dann Corbit)
	std::cout.rdbuf()->pubsetbuf(NULL,0);
	std::cin.rdbuf()->pubsetbuf(NULL,0);

	// инициализируем магический генератор
	Move_generation::ini_attacks_magic_masks_database();

	// возраст партии в 0
	Transposition_table::set_age_0();

	// инициализируем все ключи в хеш-таблице 0  
	Transposition_table::ini_hash_moves();

	// выделаем память под хеш-таблицу(по умолчанию 1 Мб)
	Transposition_table::hash_size_ini_default(1);

	// инициализируем массив хеш - ключей фигур
	// вообще по уму нужно заранее посчитать его в таблицу и озаботиться тем
	// что бы не было повторных значений ключа,
	// а пока мы его генерируем в данном месте
	Zobrist_hashing::ini_random_piese_key();


	// слушаем входной консольный поток и пишем его в gui_string_command
	while (std::cin.getline(gui_string_command, LENGTH_STRING, '\n'))
	{
		// TEST 
		// смотрим что же мы считали из выходного потока
		//std::cout << "--------------------" <<std::endl;
		//std::cout << string_in <<std::endl;
		//std::cout << string_in_size <<std::endl;

		// обрабатываем UCI команды от оболочки 
		command = Uci_gui_to_engine::parsing_command(position,gui_string_command);

		switch(command)
		{
			case 1:

				// разбираем команду go и определяем параметры дальнейшего поиска.
				Uci_go_parser::parsing(position,gui_string_command,depth_max,max_time_move_search);

				// увеличиваем возраст позиции
				Transposition_table::aging();

				//  запускаем движок на расчет позиции
				Iterative_deepening::loop(position,depth_max,max_time_move_search);

			break;
		}
	}
	return 0;
}