
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.09.2007 11.11.2009

#include <iostream>

#include "a_Header_constants.h"
#include "a_Position.h"
#include "b_Protocol_UCI.h"


/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%

 принимаем UCI команды от графической оболочки
 в рамках консольного ввода вывода

 организуем опрос входного консольного потока
 что бы не было задержек при вводе-выводе отключаем буферизацию
 программа реагирует только на команды в рамках UCI протокола
*/

/*
все классы используемые в программе

class c_FEN fen;
class c_Go go;
class d_Search_root search_root;
class e_Search search; 
class f_Search_quiescence search_quiescence;
class g_Evaluate evaluate;
class h_Generator_moves generator_moves;
class i_Make_move make_move;
class i_Test_chess_bitboard test_chess_bitboard;
class j_Sorting_moves sorting_moves;
class k_Killer killer;
class l_Hash hash;
class l_ZObrist zobrist;
class m_PV_save pv_save;
class n_Delta_pruning delta_pruning;
class o_Timer timer;



функции вне классов

int main(int argc, __int8 * argv[]) 
int public_bioskey(void);

*/

//
class b_Protocol_UCI protocol_UCI;//
struct Position position;// представление доски

__int8 string_in[LENGTH_STRING];// буфер куда помещаем строку из потока

int main
(
	int argc, 
	__int8 * argv[]
) 
{
	// отключаем буферизацию входного и выходного потоков
	std::cout.rdbuf()->pubsetbuf(NULL,0);
	std::cin.rdbuf()->pubsetbuf(NULL,0);

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

	// слушаем входной консольный поток и пишем его в string_in
	while (std::cin.getline(string_in, LENGTH_STRING, '\n'))
	{
		// TEST 
		// смотрим что же мы считали из выходного потока
		//std::cout << "--------------------" <<"\n";
		//std::cout << string_in <<"\n";
		//std::cout << string_in_size <<"\n";

		// обрабатываем UCI команды от оболочки 
		protocol_UCI.parse_protocol_UCI(position,string_in);
	}
	return 0;
}
