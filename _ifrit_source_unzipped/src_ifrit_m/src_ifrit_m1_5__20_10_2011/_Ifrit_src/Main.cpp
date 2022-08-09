
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 22.09.2007 11.11.2009

#include <iostream>
#include <fstream>

#include "Header_constants.h"
#include "Position.h"
#include "Protocol_UCI.h"
#include "Generator_moves_const.h"
#include "Test_chess_bitboard.h"
#include "Generator_moves.h"
#include <assert.h>//TEST



//#define DEBUG

/*
 ������� ����� ������� 90%

 ��������� UCI ������� �� ����������� ��������
 � ������ ����������� ����� ������

 ���������� ����� �������� ����������� ������
 ��� �� �� ���� �������� ��� �����-������ ��������� �����������
 ��������� ��������� ������ �� ������� � ������ UCI ���������
*/

/*
��� ������������ ���� ������������ � ���������

namespace FEN
namespace Go
namespace Search_root
namespace Search
namespace Search_quiescence
namespace Evaluate
namespace Generator_moves
namespace Make_move
namespace Test_chess_bitboard
namespace Sorting_moves
namespace History
namespace Killer
namespace Hash
namespace ZObrist
namespace PV_save
namespace Delta_pruning
namespace Timer

������� ��� ����

int main(int argc, __int8 * argv[]) 

*/

struct Position position;// ������������� �����

__int8 string_in[LENGTH_STRING];// ����� ���� �������� ������ �� ������

int main
(
	int argc, 
	__int8 * argv[]
) 
{
	// ��������� ����������� �������� � ��������� ������� (�� Dann Corbit)
	std::cout.rdbuf()->pubsetbuf(NULL,0);
	std::cin.rdbuf()->pubsetbuf(NULL,0);

	//
	Generator_moves::ini_attacks_magic_masks_database();

	//--------------------------------------------
	// TEST 
	// ��� ����� ������ ����� � ������
	//__int8 string_in_1[] = "uci";
	//Parse_Protokol_UCI(&bitboard,string_in_1,three_position,nodes);

	////__int8 string_in_2[] = "option name Hash type spin default 32 min 1 max 1024";
	////Parse_Protokol_UCI(&bitboard,string_in_2,three_position,nodes);

	//__int8 string_in_3[] = "position startpos moves e2e4 e7e5 g1f3 g8f6 d2d4 e5d4 e4e5 f6g8 f3d4 d8e7 d1e2 d7d6 f2f4 d6e5 f4e5 e7h4 e2f2 h4f2 e1f2 b8d7 e5e6 d7b6 f1b5 c7c6 d4c6 c8e6 c6a7 b6d7 b5d7 e8d7 h1d1 d7e7 c1e3 g8h6 f2g1 h6f5 ... e3c5";
    //__int8 string_in_3[] = "position fen n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1 moves g2h1q b7b8q h1e4 e2f2" 
	// ��� �� ������ (e2e4n) ��� ��� ���� ����������� ������� ����� ������ �� ������
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

	// ������� ������� ���������� ����� � ����� ��� � string_in
	while (std::cin.getline(string_in, LENGTH_STRING, '\n'))
	{
		// TEST 
		// ������� ��� �� �� ������� �� ��������� ������
		//std::cout << "--------------------" <<"\n";
		//std::cout << string_in <<"\n";
		//std::cout << string_in_size <<"\n";

		// ������������ UCI ������� �� �������� 
		Protocol_UCI::parse_protocol_UCI(position,string_in);
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
// �������� � ���� 
// ��������� ����� ��� �� �������� ��� ������������

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