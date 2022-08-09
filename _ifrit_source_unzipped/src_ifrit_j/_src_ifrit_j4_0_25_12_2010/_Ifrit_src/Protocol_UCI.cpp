#include <iostream>
#include <cstring>      // ���������� ��� ������ �� ������� (��������� �����)
#include <fstream>    // TEST

#include "Protocol_UCI.h"
#include "Go.h"
#include "Hash.h"
#include "Evaluate.h"
#include "ZObrist.h"

//---------------------------------------------------------------------------
/* ++
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 12.06.2007  11.11.2009 19.12.2010
 */
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

 ������������ ���������� �� ����������� �������� � ������ ��������� UCI

 ������������
 �������� ���������� ������� � ���� _TEST_FEN.txt
 �������� �������� ��� ������ � ���������� ����� ������� ������ �������
 ��������� ������ c_FEN 
 ����� ����� �������� �������� ������������ ������������� ���������� �����
 ������ �� �������� UCI �������
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
std::ofstream Test_FEN;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

class Go go;//
extern class Hash hash;
extern class ZObrist zobrist;
extern class Util util;

//	===================================================================
// ��������� �������� UCI ������� �� ����������� ��������� ��������
void Protocol_UCI::parse_protokol_UCI
(
	class Chess_bitboard & bitboard,//����� 
	std::string string_in//�������� ������
)
{

	unsigned __int16 hash_size_Mb ;// ������ ���-������� � ���������� 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
	Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
	Test_FEN<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";
	Test_FEN<<string_in<< "\n";
	Test_FEN<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";  
	Test_FEN.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  
	// // >1:ucinewgame
	if ( (int)string_in.compare(0,10,"ucinewgame") == 0 )
	{ 

		// TEST
		//std::cout << "string_in.compare(ucinewgame) == 0" << "\n";

		//������ ���� �������������
		// ������� ������ � 0
		hash.set_age_0();

		// �������������� ��� ����� � ���-������� 0  
		hash.ini_hash_moves();

	}
	// �������� ���� ��� ��� ������ ������� ������
	// � ����� �������� ���: 
	// >1:uci
	else if ( (int)string_in.compare(0,3,"uci") == 0 )// ������ ����������
	{

		// TEST
		//std::cout << "string_in.compare(uci) == 0" << "\n";

		std::cout << NAME_OF_PROGRAM_AND_DATA_OF_RELEASE << "\n";
		std::cout << "id author Brenkman Andrey" << "\n";

		//  std::cout << "option name Ponder type check default false"<< "\n";

		// ������ �������� ������� ���� � ������ �� ���������  � ��
		std::cout << "option name Hash type spin default 1 min 1 max 1024" << "\n";

		//std::cout << "option name NalimovCache type spin min 1 max 16 default 2"<< "\n";
		//std::cout << "option name NalimovPath type string default <empty>"<< "\n";
		//std::cout << "option name MultiPV type spin min 1 max 20 default 1"<< "\n";
		//std::cout << "option name OwnBook type check default true"<< "\n"; 


		// �������������� ������ ��� - ������ �����
		/* ������ ��� ���� ��� 
		������ �� ��� ����� ������� ��������� ��� � ������� � ����������� ���
		��� �� �� ���� ��������� �������� ����� � ���� �� ��� ���������� � ������ �����
		*/
		zobrist.ini_random_piese_key();

		//--------------------------------------------------------------------------
		// �� ������ ������ �������� ������ ��� ���-�������(�� ��������� 1 ��)
		//(���� ����� �� ������� ������� setoption)
		// � ���� �����������! ���� � ��� ��� ����� ������� 1 �� �� ����� �� ������ ������� setoption
		hash.hash_size_ini_default(1);

		// �������������� ��� ����� � ���-������� 0
		hash.ini_hash_moves();
		//--------------------------------------------------------------------------

		// ������� ������ � 0
		hash.set_age_0();

		// �� ��� ��������  uci �������� :)
		std::cout << "uciok \n";


	// ������ �������� ��� ������ ����� �������� ������ � ��� ����� � ������� ����
	// � ����� �������� ���: 
	// >1:setoption name Hash value 2 (���� ������ ������� ������ � 2 ��)
	}
	else if ( (int)string_in.compare(0,9,"setoption") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(setoption ) == 0" << "\n";

		// TEST
		// std::cout << "Read_hash_size_Mb(string_in) = " << Read_hash_size_Mb(string_in) << "\n";

		/*
		���� ��������� �� �������� ������ ������ ���-������� 
		������� ��������� � �����(Hash_size(string_in) != -1) 
		�� ���������� ��� � �������� ������ ��� ���-�������
		*/

		if (read_hash_size_Mb(string_in) != 0)
		{
			// ��������� ������������� ������ ���-������� � Mb
			hash_size_Mb = read_hash_size_Mb(string_in);

			//�������� ������ ��� ���-�������
			hash.hash_size_resize(hash_size_Mb);

			// �������������� ��� ����� � ���-������� 0  
			hash.ini_hash_moves();
		}

		/*
		> setoption name Ponder value true
		> setoption name Hash value 16
		> setoption name NalimovCache value 4
		> setoption name NalimovPath value D:\tbs
		> setoption name Position Learning value true
		> setoption name BookFile value dbbook.bin
		> setoption name Book Size value Big
		> setoption name Selectivity value 5
		*/


	// � ����� �������� ��� ������ ����� ���� � ��� �� ����� ������ ���
	// � ����� �������� ���: 
	// >1:isready
	}
	else if ( (int)string_in.compare(0,7,"isready") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(isready) == 0" << "\n";

		// �������� ��� � ������ ������
		std::cout << "readyok \n" << "\n";

	// �������� ����� ������ �����. ��� ���������� �������
	// � ����� �������� ���: 
	// >1:position startpos (��� ��������� �������)
	// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (��� ������� ������)
	// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1 (��� ������������ �������)
	}
	else if ( (int)string_in.compare(0,8,"position") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(position) == 0" << "\n";

		//  ��������� �������� ������� �������������� ���������� ����� ������   
		bitboard.parse_position(string_in); 
 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  ������� ���������� ����� ������� ��� ������ �������
#if TEST_P
		Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
		Test_FEN<<"position ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
		Test_FEN<<string_in<< "\n";
		Test_FEN<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
		BITBOARDGENERATOR::Bitboard_print(bitboard);
		Test_FEN.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



	// ������� �� ����� ������� �������
	// � ����� �������� ���: 
	// >1:go depth 5 (��� ����� �� �������� ������� - � ������ ������ 5 ��������� )
	// >1:go wtime 300000 btime 300000 (���� 5 ��� - ������ ���)
	// >1:go movetime 5000 ( 5 ������ �� ���)
	// >1:go infinite (������ �������)
	}
	else if ( (int)string_in.compare(0,2,"go") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(go ) == 0" << "\n";

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  ������� ���������� ����� ������� ��� ������ �������
#if TEST_P
		Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
		Test_FEN<<"go /////////////////////////////////////////////////////////"<< "\n";
		Test_FEN<<string_in<< "\n";
		Test_FEN<<"/////////////////////////////////////////////////////////"<< "\n";
    
		Test_FEN.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

		//  ��������� ������ �� ������ �������
		go.parse_go(bitboard,string_in);

	// ������� �� �����
	// � ����� �������� ���: 
	// >1:quit
	}
	else if ( (int)string_in.compare(0,4,"quit") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(quit) == 0" << "\n";

		// ����������� ������ �� ��� ���-�������
		hash.hash_size_free();

		// ������� ���������� ������ ����������
		exit(1);
	}

	//------------------------------------------------------------------------------------
	// ��� �� ����������
	//else if (util.string_start_with(string_in,"debug "))
	//{
	//	// ����� ������� �� ���� ��� ������������ ����� � ����� 
	//	//����� ��� ������ ������� ������
	//	// dummy

	//}
	//else if (util.string_equal(string_in,"ponderhit"))
	//{
	//	// �� ���� ��� ���

	//	// � ����� �������� ���: 
	//	//>1:stop
	//}
	//else if (util.string_equal(string_in,"stop"))
	//{
	//	// ������� ���� ����������� ���� � ������ �����  
	//}
	else
	{
		//���� ������ �� ����������
		std::cout << "WARNING ! Unknown command" << "\n";
	}

}

//	===================================================================
// ��������� � ��������� ���� "Hash value 32" ������ ���-������� (� ������ ������ 32)
unsigned __int16 Protocol_UCI::read_hash_size_Mb
(
	std::string string_in//�������� ������
)
{
	__int32 hash_value;

	// ������� ������ ��������� ���������  Hash value
	__int16 find_hash_value = (int)string_in.find("Hash value");

	if (find_hash_value != -1)
	{
		// ���������� ������� Hash value � ������� �� �����(������ ���-�������)
		find_hash_value = find_hash_value + 11;

		// �������� ��������� ���������� �����
		std::string string_hash_value = string_in.substr(find_hash_value);

        // ������� atoi ��������� ������ �������� � ����� �����;
		hash_value = atoi(string_hash_value.c_str());
		std::cout << "hash_value = " << hash_value <<"\n";

		//------------------------
		// ���� �������� �������� ������

		//std::cout << " ���� �������� �������� ������---------------";
		//std::cout << "\n";

		//for (unsigned int i = 0; i < string_in.length(); i++)
		//{
		//	std::cout << string_in.at(i);
		//}
		//std::cout << "\n";


		//// ���� �������� ���������� ���������

		//std::cout << " ���� �������� ���������� ���������---------------";
		//std::cout << "\n";

		//for (unsigned int i = 0; i < string_hash_value.length(); i++)
		//{
		//	std::cout << string_hash_value.at(i);
		//}
		//std::cout << "\n";

		//------------------------

		return hash_value;//
	}


	return 0;
}




