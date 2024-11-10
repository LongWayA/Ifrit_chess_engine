#include <iostream>
#include <cstring>

using namespace std;

#include "Structure.h"
#include "Protocol_UCI.h"
#include "Hash.h"

//-----------------------------------------------------------------------------------
/*++
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.09.2007 11.11.2009
*/
//-----------------------------------------------------------------------------------

/*
 ������� ����� ������� 90%

 ��������� UCI ������� �� ����������� ��������
 � ������ ����������� ����� ������

 ���������� ����� �������� ����������� ������
 ��� �� �� ���� �������� ��� �����-������ ��������� �����������
 ��������� ��������� ������ �� ������� � ������ UCI ���������
*/


	__int8 string_in[LENGTH_STRING];// ����� ���� �������� ������ �� ������

int main
(
	int argc, 
	__int8 * argv[]
) 
{

	class Chess_bitboard bitboard;//���������� ������� ����� ������ 
	class Protocol_UCI protocol_UCI;//
	//unsigned __int16 string_in_size;

	bitboard.bitboard_ini_t();

	// ��������� ����������� �������� � ��������� �������
	std::cout.rdbuf()->pubsetbuf(NULL,0);
	std::cin.rdbuf()->pubsetbuf(NULL,0);

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

//std::cin.getline(string_in, LENGTH_STRING, '\n');
//std::string _string_in(string_in);
//
//int ss = _string_in.size();
//
//		std::cout << "--------------------" << "\n";
//		std::cout << string_in << "\n";
//		std::cout << ss << "\n";
//
// for (unsigned int i = 0; i < _string_in.length(); i++)
//  {
//    cout << _string_in.at(i);
//  }
//
//std::cout << "\n";
//std::cout << "--------------------" <<"\n";
//
//// 0 ������ ���������� 1 ���� ����������
//cout << (int)_string_in.find("uci");


	// ������� ������� ���������� ����� � ����� ��� � string_in
	while (std::cin.getline(string_in, LENGTH_STRING, '\n'))
	{
		// ������ ��������� ������. ������ ����� ��� �������� ������ ���� ���������
		//string_in_size = std::cin.gcount();

		// ������� ��� �� �� ������� �� ��������� ������
		//std::cout << "--------------------" <<"\n";
		//std::cout << string_in <<"\n";
		//std::cout << string_in_size <<"\n";

		// ������������ UCI ������� �� �������� 
		protocol_UCI.parse_protokol_UCI(bitboard,string_in);

	}
	return 0;
}
