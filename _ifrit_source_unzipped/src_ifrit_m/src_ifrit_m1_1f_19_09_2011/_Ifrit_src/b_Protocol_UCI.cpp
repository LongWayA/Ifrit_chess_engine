
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 12.06.2007  11.11.2009 19.12.2010 1.1.2011

#include <fstream>
#include <iostream>

#include "b_Protocol_UCI.h"
#include "c_FEN.h"
#include "c_Go.h"
#include "h_Generator_moves.h"
#include "i_Make_move.h"
#include "l_ZObrist.h"
#include "l_Hash.h"


class c_FEN fen;//
class c_Go go;
class h_Generator_moves generator_moves;//
class i_Make_move make_move;//
class l_ZObrist zobrist;
class l_Hash hash;




//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
std::ofstream Test_FEN;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// ��������� �������� UCI ������� �� ����������� ��������� ��������
void b_Protocol_UCI::parse_protocol_UCI
(
	struct Position & position,// ������������� �����
	const std::string string_in//�������� ������
)
{
	unsigned __int16 hash_size_Mb ;// ������ ���-������� � ���������� 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
	Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
	Test_FEN << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";
	Test_FEN << string_in.c_str() << "\n";
	Test_FEN << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n"; 
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

		//
		generator_moves.ini_attacks_magic_masks_database();

		// �� ��� ��������  uci �������� :)
		std::cout << "uciok \n";

	}

	// ������ �������� ��� ������ ����� �������� ������ � ��� ����� � ������� ����
	// � ����� �������� ���: 
	// >1:setoption name Hash value 2 (���� ������ ������� ������ � 2 ��)
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

		// ��������� ������������� ������ ���-������� � Mb
		hash_size_Mb = read_hash_size_Mb(string_in);

		if (hash_size_Mb != 0)
		{
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

	}

	// � ����� �������� ��� ������ ����� ���� � ��� �� ����� ������ ���
	// � ����� �������� ���: 
	// >1:isready
	else if ( (int)string_in.compare(0,7,"isready") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(isready) == 0" << "\n";

		//
		generator_moves.ini_attacks_magic_masks_database();

		// �������� ��� � ������ ������
		std::cout << "readyok \n" << "\n";

	}

	// �������� ����� ������ �����. ��� ���������� �������
	// � ����� �������� ���: 
	// >1:position startpos (��� ��������� �������)
	// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (��� ������� ������)
	// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1
	// (��� ������������ �������)
	else if ( (int)string_in.compare(0,8,"position") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(position) == 0" << "\n";

		fen.bitboard_ini(position);

		//  ��������� �������� ������� �������������� ���������� ����� ������   
		parse_position_FEN(position,string_in); 
 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  ������� ���������� ����� ������� ��� ������ �������
#if TEST_P
		Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
		Test_FEN<< "position ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
		Test_FEN << string_in.c_str() << "\n";
		Test_FEN<< "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
		bitboard.bitboard_print();
		Test_FEN.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}

	// ������� �� ����� ������� �������
	// � ����� �������� ���: 
	// >1:go depth 5 (��� ����� �� �������� ������� - � ������ ������ 5 ��������� )
	// >1:go wtime 300000 btime 300000 (���� 5 ��� - ������ ���)
	// >1:go movetime 5000 ( 5 ������ �� ���)
	// >1:go infinite (������ �������)
	else if ( (int)string_in.compare(0,2,"go") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(go ) == 0" << "\n";

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  ������� ���������� ����� ������� ��� ������ �������
#if TEST_P
		Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
		Test_FEN << "go /////////////////////////////////////////////"<< "\n";
		Test_FEN << string_in.c_str() << "\n";
		Test_FEN << "////////////////////////////////////////////////"<< "\n";
    
		Test_FEN.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

		//  ��������� ������ �� ������ �������
		go.parse_go(position,string_in);

	}

	// ������� �� �����
	// � ����� �������� ���: 
	// >1:quit
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
unsigned __int16 b_Protocol_UCI::read_hash_size_Mb
(
	const std::string string_in//�������� ������
)
{
	unsigned __int16 hash_value;

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
		//std::cout << "hash_value = " << hash_value <<"\n";

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

//	===================================================================
// ��������� ��� ������ ��� � �������������� �����
void b_Protocol_UCI::parse_position_FEN
(
	struct Position & position,// ������������� �����
	const std::string string_in//�������� ������
)
{
	// ���������������� ������� ������ ����� ���� ���� ����� 
	//"position startpos" ������ ��������� �������
	//"position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� ������� � ������� 
	//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� ����� ��� �������������

	__int8 move_string[] = {' ',' ',' ',' ',' ',' '};// ��������������� ������
	const std::string start_fen("position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");// ��������� ���-�������

	unsigned __int8 cnt_move = 0;// ������� ����� � ������ ������ ����� ��� ����������� ������

	if ( (int)string_in.compare(0,12,"position fen") == 0 )
	{
		//�������� � �������� ���-��������
		fen.fen_to_board(position,string_in);

	}
	else
	{
		// � ��� ������������� ����������� ������� startpos
		// ��������� ��� �� ��� ���������� �� ������ ���� ������ ��������� ������� � ����
		// start_fen = "position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		fen.fen_to_board(position,start_fen);
	}

	// �������������� ������ ������ �� ��������� ������ ������������ 
	// ��� ������ ������ ������������ ��� ����������� ������������ ���������� �������
	zobrist.ini_hash_string_moves(); 

	// �� ������� ������� ���������� ��� - ����
	zobrist.start_position_random_key(position);

	// ���������� ���-���� � ������ ��� - ������ �� ������ ����
	zobrist.save_hash_string_moves(position.hash_key,cnt_move); 


	//---------------------------------------------

	// ������� ������ ��������� ����� moves
    __int16 find = (int)string_in.find("moves");
	//std::cout << "find= " << find <<"\n";

	// ���� ���������� � ����
	//position startpos moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
    //position fen n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1 moves g2h1q b7b8q h1e4 e2f2
	// g2h1q - ����������� � ����� , r- ����� b - ���� n- ����   

    unsigned __int16 i = find;

	if (find != -1)
	{
		// ����������� moves � ������ 
		i = i + 6;

		// ��� ������� ������� g2h1q
		// �� ��� ��� ���� ����� ������ ������� ��� ���������� ����
		while (i < string_in.length() - 3)	
		{ 
			// ���������� ����� ���� � �������� ����� � ������� ��� g
			move_string[1] = string_in.at(i);
			//std::cout << "move_string[1]= " << move_string[1] <<"\n";

			//  ���������� ����� ���� � �������� ����� � ������� ��� 2
			move_string[2] = string_in.at(i + 1);
			//std::cout << "move_string[2]= " << move_string[2] <<"\n";

			//  ���������� ����� ���� �� ������� ����� � ������� ��� h
			move_string[3] = string_in.at(i + 2);
			//std::cout << "move_string[3]= " << move_string[3] <<"\n";

			//  ���������� ����� ���� �� ������� ����� � ������� ��� 1
			move_string[4] = string_in.at(i + 3);
			//std::cout << "move_string[4]= " << move_string[4] <<"\n";

			// ��� ������� ����� ���
			// string_in.at(string_in.length()) - ��� �������� ���������� ������ �� ������� ������
			// � ��� �������� ���� ������ � ���� � ������ ���������� �������� �� 
			// ������� ������ ��� ���������� ���������

			//  ���������� ������ ����������� � ������� ��� q ������ �� ���� ������
			// (unsigned) - ��� �� ���������� �� ������������
			if ( ( (unsigned)(i + 4) >= string_in.length() ) || string_in.at(i + 4) == ' ')
			{
				move_string[5] = '\0';
			}
			else
			{ 
				move_string[5] = string_in.at(i + 4);

				// ���� ������ ����������� ���� �� ���������� ����� �� ���� ������ ������
				i = i + 1;
			}

			//std::cout << "move_string[5]= " << move_string[5] <<"\n";

			//
			move_to_board(position,move_string);

			//---------------------------------------------
			cnt_move = cnt_move + 1;

			// �� ������� ������� ���������� ��� - ����
			zobrist.start_position_random_key(position);

			// ���������� ���-���� � ������ ��� -������ �� ������ ����
			// ���� ������ ������ ������������ ��� ����������� ������������ ���������� �������
			zobrist.save_hash_string_moves(position.hash_key,cnt_move); 
			//---------------------------------------------

			// � ������� g2h1q
			// ���������� �� 5 ����� �.�. � ������� g2h1 + ������, 
			// � �� ������ ������ ����������� �� ���������� 
			// ������  ����� ���������� q
            i = i + 5;

		}
	}
}

//	===================================================================
//�������� � ������ ������(moves) �.�. ���� ���������� � ������� ������� �� ���������
void b_Protocol_UCI::move_to_board
(
	struct Position & position,// ������������� �����
	const __int8 move_string[]//������ ���������� ����
)
{
	struct List list_surplus_moves; // ������ ��������� ����� (������������ ���� �� ������� ����)
	const __int8 depth = 0;//

	// ���� ���������� � ����
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	// ������������ ������ ����������� �����
	unsigned __int8 n = 8 * fen.ch_koordinata_to_indeks(move_string[2]) 
		+ fen.koordinata_to_indeks(move_string[1]);// ������ ������ �����

	unsigned __int8 n_2 = 8 * fen.ch_koordinata_to_indeks(move_string[4]) 
		+ fen.koordinata_to_indeks(move_string[3]);// ���� ������ �����


	//generator_moves.mobility_evaluation(position);

	// ������������ ��� ��������� ���� ������
	if (position.colour_move == 1)
	{
		generator_moves.generator_captures_white(list_surplus_moves,position);
		generator_moves.generator_move_white(list_surplus_moves,position);
	}
	else
	{
		generator_moves.generator_captures_black(list_surplus_moves,position);
		generator_moves.generator_move_black(list_surplus_moves,position);
	}

	__int16 n_3 = -1; // ����� ���� �� ������

	// ����� �� ������ ���������� �����
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

		// ���� ��� � ������ ��������� � ����� �� ������(moves) �����..
		if (( n == ( (list_surplus_moves.move[i] >> 12) & 63 ) ) && // ���������� ������ ������ �����
		( n_2 == ((list_surplus_moves.move[i] >> 18) & 63) ) )  // ���������� ���� ������ �����
		{
			/*
			� ����������� �� ���� ���� �� ����������� �� ������������ ������ ����
			��� �� ������ ���� ������ ����� ����� ��� ���������� ����������� �����
			� ������ ���������� �����
			��� ������ �� ��������� ����������� ���� �� � ���������� 
			������ ����� �� �������������
			��� ��� ��� ���������� ������� ���� ����������� � �����
			����� ���������� ���� ����������� � ����� ����� � ����
			����� ������� ��� � ������������� � ������
			*/
			switch (move_string[5])
			{
				// ����������� ��� ������� ������ �������� ����� ���� � ������
				case '\0' :
					// ������ ������
					n_3 = i;
				break;

				case 'q' :
					// ����������� � �����
					n_3 = i;
				break;

				case 'r' :
					// ����������� � �����
					n_3 = i + 1;
				break;

				case 'b' :
					// ����������� � ����
					n_3 = i + 2;
				break;

				case 'n' :
					// ����������� � ����
					n_3 = i + 3;
				break;

				default:;//������ ������ �� ���������
   
			}

			// ������ ��� ����� ��� ��� ������ ������ �� ����� ������ ���
			// �������� ������� �� �����
			break;
		}
	

	}

	//----------------------------------------------------------
	if (n_3 == -1)
	{
		std::cout << "WARNING ! moves error" << "\n";
	}
	else
	{
		// ������ ���
		if (position.colour_move == 1)
		{
			make_move.do_moves_white(position,(unsigned __int8)n_3
				,depth,list_surplus_moves);
		}
		else
		{
			make_move.do_moves_black(position,(unsigned __int8)n_3
				,depth,list_surplus_moves);
		}

	}

}

