
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 12.06.2007  11.11.2009 19.12.2010 1.1.2011

#include <fstream>
#include <iostream>

#include "uci_gui_to_engine.h"
#include "uci_fen_parser.h"
#include "uci_go_parser.h"
#include "move_generation.h"
#include "make_move.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "test_chess_bitboard.h"
#include "uci_engine_to_gui.h"
#include "_version_engine.h"

/// ��������-���������(1-0)  ����� ������ � 
/// ����(_TEST_UCI_PARSER.txt) �������� ����������
#define TEST_UCI_PARSER 0

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_UCI_PARSER
std::ofstream Test_uci_parser;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

// ��������� �������� UCI ������� �� ����������� ��������� ��������(GUI)
__int32 Uci_gui_to_engine::parsing_command
(
	struct Position & position,// ������������� �����
	const std::string gui_string_command//�������� ������
)
{
	unsigned __int16 hash_size_Mb;// ������ ���-������� � ���������� 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_UCI_PARSER
	Test_uci_parser.open ("_TEST_UCI_PARSER.txt",std::ios::app);
	Test_uci_parser << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< std::endl;
	Test_uci_parser << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< std::endl;
	Test_uci_parser << string_in.c_str() << std::endl;
	Test_uci_parser << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< std::endl; 
	Test_uci_parser.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  
	// >1:ucinewgame
	if ( (int)gui_string_command.compare(0,10,"ucinewgame") == 0 )// ������ ����������
	{ 

		// TEST
		//std::cout << "string_in.compare(ucinewgame) == 0" << std::endl;

	}

	// �������� ���� ��� ��� ������ ������� ������
	// � ����� �������� ���: 
	// >1:uci
	else if ( (int)gui_string_command.compare(0,3,"uci") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(uci) == 0" << std::endl;

		std::cout << NAME_AND_VERSION_ENGINE << std::endl;
		std::cout << "id author Brenkman Andrey" << std::endl;

		//  std::cout << "option name Ponder type check default false"<< std::endl;

		// ������ �������� ������� ���� � ������ �� ���������  � ��
		std::cout << "option name Hash type spin default 1 min 1 max 1024" << std::endl;

		//std::cout << "option name NalimovCache type spin min 1 max 16 default 2"<< std::endl;
		//std::cout << "option name NalimovPath type string default <empty>"<< std::endl;
		//std::cout << "option name MultiPV type spin min 1 max 20 default 1"<< std::endl;
		//std::cout << "option name OwnBook type check default true"<< std::endl; 
		//option name Ponder type check default true
		// �� ��� ��������  uci �������� :)
		std::cout << "uciok" << std::endl;

	}

	// ������ �������� ��� ������ ����� �������� ������ � ��� ����� � ������� ����
	// � ����� �������� ���: 
	// >1:setoption name Hash value 2 (���� ������ ������� ������ � 2 ��)
	else if ( (int)gui_string_command.compare(0,9,"setoption") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(setoption ) == 0" << std::endl;

		/*
		���� ��������� �� �������� ������ ������ ���-������� 
		������� ��������� � ����� �� ���������� ��� � 
		�������� ������ ��� ���-�������
		*/
		// ��������� ������������� ������ ���-������� � Mb
		hash_size_Mb = hash_value_Mb_string_to_int(gui_string_command);

		if (hash_size_Mb != 0)
		{
			//�������� ������ ��� ���-�������
			Transposition_table::hash_size_resize(hash_size_Mb);

			// �������������� ��� ����� � ���-������� 0  
			Transposition_table::ini_hash_moves();
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
	else if ( (int)gui_string_command.compare(0,7,"isready") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(isready) == 0" << std::endl;

		// �������� ��� � ������ ������
		std::cout << "readyok " << std::endl;

	}

	// �������� ����� ������ �����. ��� ���������� �������
	// � ����� �������� ���: 
	// >1:position startpos (��� ��������� �������)
	// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (��� ������� ������)
	// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1
	// (��� ������������ �������)
	else if ( (int)gui_string_command.compare(0,8,"position") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(position) == 0" << std::endl;

		Uci_fen_parser::bitboard_ini(position);

		//  ��������� �������� ������� �������������� ���������� ����� ������   
		chess_position_string_to_internal_chess_board(position,gui_string_command); 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  ������� ���������� ����� ������� ��� ������ �������
#if TEST_UCI_PARSER
		Test_uci_parser.open ("_TEST_UCI_PARSER.txt",std::ios::app);
		Test_uci_parser << "position ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< std::endl;
		Test_uci_parser << string_in.c_str() << std::endl;
		Test_uci_parser << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< std::endl;
		Test_chess_bitboard::bitboard_print(position);
		Test_uci_parser.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}

	// ������� �� ����� ������� �������
	// � ����� �������� ���: 
	// >1:go depth 5 (��� ����� �� �������� ������� - � ������ ������ 5 ��������� )
	// >1:go wtime 300000 btime 300000 (���� 5 ��� - ������ ���)
	// >1:go movetime 5000 ( 5 ������ �� ���)
	// >1:go infinite (������ �������)
	else if ( (int)gui_string_command.compare(0,2,"go") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(go ) == 0" << std::endl;
		return 1;

	}

	// ������� �� �����
	// � ����� �������� ���: 
	// >1:quit
	else if ( (int)gui_string_command.compare(0,4,"quit") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(quit) == 0" << std::endl;

		// ����������� ������ �� ��� ���-�������
		Transposition_table::hash_size_free();

		// ������� ���������� ������ ����������
		exit(1);
	}
	else if ( (int)gui_string_command.compare(0,4,"stop") == 0 )
	{ 
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

	else
	{
		//���� ������ �� ����������
		std::cout << "UCI WARNING! Unknown command" << std::endl;
	}

		return 0;

}

//	===================================================================
// ��������� � ��������� ���� "Hash value 32" ������ ���-������� (� ������ ������ 32)
unsigned __int16 Uci_gui_to_engine::hash_value_Mb_string_to_int
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

		//std::cout << "hash_value = " << hash_value <<std::endl;

		return hash_value;
	}

	return 0;
}

//	===================================================================
// ��������� ��������� �������, �������������� � ������ � �������������� �����
void Uci_gui_to_engine::chess_position_string_to_internal_chess_board
(
	struct Position & position,// ������������� �����
	const std::string string_in//�������� ������
)
{
	// ���������������� ������� ������ ����� ���� ���� ����� 
	// "position startpos" ������ ��������� �������
	// "position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� 
												//   ������� � ������� 
	// "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� 
													 //  ����� ��� �������������

	__int8 move_string[] = {' ',' ',' ',' ',' ',' '};// ��������������� ������

	// ��������� ���-�������
	const std::string start_fen(
		"position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
								);

	unsigned __int8 cnt_move = 0;// ������� ����� � ������ ������ 
								 // ����� ��� ����������� ������

	if ( (int)string_in.compare(0,12,"position fen") == 0 )
	{
		//�������� � �������� ���-��������
		Uci_fen_parser::fen_to_board(position,string_in);

	}
	else
	{
		// � ��� ������������� ����������� ������� startpos
		// ��������� ��� �� ��� ���������� �� ������ ���� ������ ��������� ������� � ����
		// start_fen = "position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		Uci_fen_parser::fen_to_board(position,start_fen);
	}

	// �������������� ������ ������ �� ��������� ������ ������������ 
	// ��� ������ ������ ������������ ��� ����������� ������������ ���������� �������
	Zobrist_hashing::ini_hash_string_moves(); 

	// �� ������� ������� ���������� ��� - ����
	Zobrist_hashing::start_position_random_key(position);

	// ���������� ���-���� � ������ ��� - ������ �� ������ ����
	Zobrist_hashing::save_hash_string_moves(position.hash_key,cnt_move); 


	//---------------------------------------------

	// ������� ������ ��������� ����� moves
    __int16 find = (int)string_in.find("moves");
	//std::cout << "find= " << find <<std::endl;

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
			// ���������� ����� ���� � �������� �����
			// (� ������� ��� g)
			move_string[1] = string_in.at(i);
			//std::cout << "move_string[1]= " << move_string[1] <<std::endl;

			// ���������� ����� ���� � �������� �����
			// (� ������� ��� 2)
			move_string[2] = string_in.at(i + 1);
			//std::cout << "move_string[2]= " << move_string[2] <<std::endl;

			// ���������� ����� ���� �� ������� �����
			// (� ������� ��� h)
			move_string[3] = string_in.at(i + 2);
			//std::cout << "move_string[3]= " << move_string[3] <<std::endl;

			//  ���������� ����� ���� �� ������� �����
			// (� ������� ��� 1)
			move_string[4] = string_in.at(i + 3);
			//std::cout << "move_string[4]= " << move_string[4] <<std::endl;

			// ��� ������� ����� ���
			// string_in.at(string_in.length()) - ��� �������� ���������� 
			// ������ �� ������� ������
			// � ��� �������� ���� ������ � ���� � ������ ���������� �������� �� 
			// ������� ������ ��� ���������� ���������

			//  ���������� ������ ����������� (� ������� ��� q) ������ �� ���� ������
			// (unsigned) - ��� �� ���������� �� ������������
			if ( ( (unsigned)(i + 4) >= string_in.length() ) || string_in.at(i + 4) == ' ')
			{
				// ��� ������ �����������
				move_string[5] = '\0';
			}
			else
			{ 
				// ���������� ������ ����������� (� ������� ��� q)
				move_string[5] = string_in.at(i + 4);

				// ��������� ��� ���� ������ �����������
				i = i + 1;
			}

			//std::cout << "move_string[5]= " << move_string[5] <<std::endl;

			// �������� ��� � �������
			move_to_position(position,move_string);

			// �� ������� ������� ���������� ��� - ����
			Zobrist_hashing::start_position_random_key(position);

			// 
			cnt_move = cnt_move + 1;

			// ���������� ���-���� � ������ ��� -������ �� ������ ����
			// ���� ������ ������ ������������ ��� ����������� 
			// ������������ ���������� �������
			Zobrist_hashing::save_hash_string_moves(position.hash_key,cnt_move); 

			// ��� �������� � ���������� ���� ���������� �� 5 ����� 
			// ������ ��� ��� ���������� �������� ��� ����� � ������ 
			// ����������� ����� ���������
			// � �� ������ ������ ����������� �� ���������� ������
			// ������� ��� � ������� �� ��������� g2h1q
			// �.�. � ������� g2h1 + ������ ��� 5, 
			// � �� ������ ������ ����������� �� ���������� 
			// ������  ����� ���������� q
            i = i + 5;

		}
	}
}

//	===================================================================
//�������� � ������ ���� ���������� � ������� ������� �� ���������
void Uci_gui_to_engine::move_to_position
(
	struct Position & position,// ������������� �����
	const __int8 move_string[]//������ ���������� ����
)
{
	struct List list_surplus_moves; // ������ ��������� ����� 
									// (������������ ���� �� ������� ����)
	const __int8 depth = 0;//

	// ���� ���������� � ����
	// moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 

	// ������������ ������ ����������� �����
	// ������ ������ �����
	unsigned __int8 move_from_square = 8 * Uci_fen_parser::ch_koordinata_to_indeks(move_string[2]) 
		+ Uci_fen_parser::koordinata_to_indeks(move_string[1]);

	// ���� ������ �����
	unsigned __int8 move_to_square = 8 * Uci_fen_parser::ch_koordinata_to_indeks(move_string[4]) 
		+ Uci_fen_parser::koordinata_to_indeks(move_string[3]);

	// �� ����� ��� ��� �����
	//Move_generation::mobility_evaluation(position);

	// ������������ ��� ��������� ���� ������
	if (position.colour_move == 1)
	{
		Move_generation::generator_captures_white(list_surplus_moves,position);
		Move_generation::generator_move_white(list_surplus_moves,position);
	}
	else
	{
		Move_generation::generator_captures_black(list_surplus_moves,position);
		Move_generation::generator_move_black(list_surplus_moves,position);
	}

	__int16 number_move_in_list = -1; // ����� ���� �� ������

	// ����� �� ������ ���������� �����
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

		// ���� ��� � ������ ��������� � ����� �� ������(moves) �����..
		if (( move_from_square == ( (list_surplus_moves.move[i] >> 12) & 63 ) ) && // ���������� ������ ������ �����
		( move_to_square == ((list_surplus_moves.move[i] >> 18) & 63) ) )  // ���������� ���� ������ �����
		{
			/*
			� ����������� �� ���� ���� �� ����������� �� ������������ ������ ����
			��� �� ������ ���� ������ ����� ����� ��� ���������� ����������� �����
			� ������ ���������� �����
			��� ������ �� ��������� ����������� ����, �� � ���������� 
			������ ����� �� ������������� �� ��� �����������
			��� ��� ��� ���������� ������� ���� ����������� � �����
			������ ���� ����������� � �����
			������ ���� ����������� � �����
			������ ���� ����������� � ����
			����� ������� ��� � ������������� � ������
			*/
			switch (move_string[5])
			{
				// ����������� ��� ������� ������ �������� ����� ���� � ������
				case '\0' :
					// ������ ������
					number_move_in_list = i;
				break;

				case 'q' :
					// ����������� � �����
					number_move_in_list = i;
				break;

				case 'r' :
					// ����������� � �����
					number_move_in_list = i + 1;
				break;

				case 'b' :
					// ����������� � ����
					number_move_in_list = i + 2;
				break;

				case 'n' :
					// ����������� � ����
					number_move_in_list = i + 3;
				break;

				default:;//������ ������ �� ���������
   
			}

			// ������ ��� ����� ��� ��� ������ ������ �� ����� ������ ���
			// �������� ������� �� �����
			break;
		}
	
	}

	// ������ ��������� ��� �� ���������� �����
	if (number_move_in_list == -1)
	{
		std::cout << "WARNING ! moves error" << std::endl;
	}
	else
	{
		// ������ ���
		if (position.colour_move == 1)
		{
			Make_move::do_moves_white(position,(unsigned __int8)number_move_in_list
				,depth,list_surplus_moves);
		}
		else
		{
			Make_move::do_moves_black(position,(unsigned __int8)number_move_in_list
				,depth,list_surplus_moves);
		}

	}

}