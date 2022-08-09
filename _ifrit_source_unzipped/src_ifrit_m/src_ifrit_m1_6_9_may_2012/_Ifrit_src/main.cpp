
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.09.2007 11.11.2009

#include <iostream>
#include <fstream>

#include "constants_header.h"
#include "position_struct.h"
#include "uci_parser.h"
#include "move_generation_const.h"
#include "test_chess_bitboard.h"
#include "move_generation.h"
#include <assert.h>//TEST



//#define DEBUG

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


 УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%

 принимаем UCI команды от графической оболочки
 в рамках консольного ввода вывода

 организуем опрос входного консольного потока
 что бы не было задержек при вводе-выводе отключаем буферизацию
 программа реагирует только на команды в рамках UCI протокола
*/

/*
все пространства имен используемые в программе
использую вместо классов

namespace Evaluate
namespace Fen_parser
namespace History_heuristic
namespace Killer_heuristic
namespace Make_move
namespace Move_ordering
namespace Move_generation
namespace PV_save
namespace Search
namespace Search_quiescence
namespace Search_root
namespace Test_chess_bitboard
namespace Time_management
namespace Transposition_table
namespace Uci_go
namespace Uci_parser
namespace WIN_32_TORD_ROMSTAD_BIOSKEY
namespace Zobrist_hashing

функции вне имен

int main(int argc, __int8 * argv[]) 

*/

struct Position position;// представление доски

__int8 string_in[LENGTH_STRING];// буфер куда помещаем строку из потока

int main
(
	int argc, __int8 * argv[]
) 
{
	// отключаем буферизацию входного и выходного потоков (по Dann Corbit)
	std::cout.rdbuf()->pubsetbuf(NULL,0);
	std::cin.rdbuf()->pubsetbuf(NULL,0);

	//
	Move_generation::ini_attacks_magic_masks_database();

	//--------------------------------------------
	// TEST 
	// для теста движка прямо в студии
	//__int8 string_in_1[] = "uci";
	//Parse_Protokol_UCI(&bitboard,string_in_1,three_position,nodes);

	////__int8 string_in_2[] = "option name Hash type spin default 32 min 1 max 1024";
	////Parse_Protokol_UCI(&bitboard,string_in_2,three_position,nodes);

	//__int8 string_in_3[] = "position startpos moves e2e4 e7e5 g1f3 g8f6 d2d4 e5d4 e4e5 f6g8 f3d4 d8e7 d1e2 d7d6 f2f4 d6e5 f4e5 e7h4 e2f2 h4f2 e1f2 b8d7 e5e6 d7b6 f1b5 c7c6 d4c6 c8e6 c6a7 b6d7 b5d7 e8d7 h1d1 d7e7 c1e3 g8h6 f2g1 h6f5 ... e3c5";
    //__int8 string_in_3[] = "position fen n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1 moves g2h1q b7b8q h1e4 e2f2" 
	// так не бывает (e2e4n) так что блок реализующий позицию будет вопить об ошибке
	//__int8 string_in_3[] = "position startpos moves e2e4n e7e5 g1f3 g8f6 d2d4 e5d4 e4e5 f6g8 f3d4 d8e7q d1e2 d7d6 f2f4 d6e5 f4e5 e7h4 e2f2 h4f2 e1f2 b8d7 e5e6 d7b6 f1b5 c7c6 d4c6 c8e6 c6a7 b6d7 b5d7 e8d7 h1d1 d7e7 c1e3 g8h6 f2g1 h6f5 e3c5";
	//__int8 string_in_3[] = "position fen rnbqkbnr/qqqqqqqq/8/8/8/8/QQQQQQQQ/RNBQKBNR w KQkq - 0 1";
	//protocol_UCI.parse_protokol_UCI(bitboard,string_in_3);

	////__int8 string_in_4[] = "go wtime 1 btime 13333";
	//__int8 string_in_4[] = "go depth 1";
	//Parse_Protokol_UCI(&bitboard,string_in_4,three_position,nodes);
	//--------------------------------------------

//std::cout << (int)0x28 <<"\n";

//assert(true);
//assert(false);
//assert(("Main", 1==0)); 
//assert(1); 

//Test_chess_bitboard::print_bitboard_map(0x30);

	// слушаем входной консольный поток и пишем его в string_in
	while (std::cin.getline(string_in, LENGTH_STRING, '\n'))
	{
		// TEST 
		// смотрим что же мы считали из выходного потока
		//std::cout << "--------------------" <<"\n";
		//std::cout << string_in <<"\n";
		//std::cout << string_in_size <<"\n";

		// обрабатываем UCI команды от оболочки 
		Uci_parser::parse_protocol_UCI(position,string_in);
	}
	return 0;
}

// unsigned __int8 : 0 to 255
//          __int8 : -128 to 127

// unsigned __int16 :  0 to 65535
//          __int16 : -32768 to 32767

// unsigned __int32 : 0 to 4294967295
//          __int32 : -2147483648 to 2147483647

// unsigned __int64 : 0 to 
//          __int64 : - to 



/*
// печатаем в файл 
// использую когда что то тестирую или разрабатываю

std::ofstream Print_Main;
Print_Main.open ("_MOBILITY.txt",std::ios::app);	

for ( int i = 0; i < 64; i++ )
{
	Print_Main << rook_full_masks[i] << ",// from = " << i << "\n";
}

//Print_Main <<"depth= "<< rook_full_masks[] << "\n";
Print_Main.close();

//Test_chess_bitboard::print_bitboard_map(571347909858961602);


*/

//for ( int i = 0; i < 30; i++ )
//{
//    __int64 k = (__int64)1 << i;
//	std::cout << "i= " << i <<"\n";
//	std::cout << int(k) <<"\n";
//	std::cout <<"\n";
//}