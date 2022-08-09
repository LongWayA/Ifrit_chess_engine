
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 10.5.2012

#include <fstream>
#include <iostream>

#include "uci_engine_to_gui.h"
#include "uci_gui_to_engine.h"
#include "uci_fen_parser.h"
#include "uci_go_parser.h"
#include "move_generation.h"
#include "make_move.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "test_chess_bitboard.h"
#include "pv_save.h"
#include "time_management.h"

#define TEST_UCI_INFO 0 // ��������-���������(1-0) ����� 
						// ������������ ������ ���������� � �����

#if TEST_UCI_INFO
// TEST
std::ofstream Test_info;
#endif//#if TEST_UCI_INFO


__int32 save_value; /// ���������� ������ ���������� ����������� ��������
unsigned __int8 save_Depth_Max; /// ���������� ������� ���������� ����������� ��������

/*
 ������ ��� 
 �� ���������� � ���� <  <1:bestmove e2e4
 ��� ��� ����������� � ���� <1:bestmove d7d8N

� ���������� move ���������, ������ �����, 
���� �����, ��� ����, ���� �� �����������.
� ���� ������� �� ��� ������������� � �������� � �������� �����.
�������� � ������� �������� ��. a_move_list_struct.h
*/
void Uci_engine_to_gui::do_bestmove(__int32 bestmove)
{
	std::cout << "bestmove ";
	std::cout << x_to_notation(((bestmove >> 12) & 63) % 8);
	std::cout << (1 + ((bestmove >> 12) & 63) / 8 );
	std::cout << x_to_notation(((bestmove >> 18) & 63) % 8);
	std::cout << (1 + ((bestmove >> 18) & 63) / 8);

	// ��������� ��� ����
	__int32 d_m = ((bestmove >> 6) & 63);

	// ���� d_m ������ 5 �� ������ ����� ����������� �����
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			// ����������� ����� � �������
			std::cout << int_to_char_board((d_m - 20),0);
		}
		else
		{
			// ����������� �����
			std::cout << int_to_char_board((d_m - 10),0);
		}
	}

	//std::cout << std::endl;
	std::cout << std::endl;

	//if (PV_save::get_pv_best().depth_max > 1)
	//{
	//	// �������� ponder
	//	// ������ ��� � ����� ��������
	//	__int32 bestmove = PV_save::get_pv_best().move[1];

	//	std::cout << " ponder ";
	//	std::cout << x_to_notation(((bestmove >> 12) & 63) % 8);
	//	std::cout << (1 + ((bestmove >> 12) & 63) / 8 );
	//	std::cout << x_to_notation(((bestmove >> 18) & 63) % 8);
	//	std::cout << (1 + ((bestmove >> 18) & 63) / 8);

	//	// ��������� ��� ����
	//	__int32 d_m = ((bestmove >> 6) & 63);

	//	// ���� d_m ������ 5 �� ������ ����� ����������� �����
	//	if (d_m > 5)
	//	{
	//		if (d_m > 15)
	//		{
	//			// ����������� ����� � �������
	//			std::cout << int_to_char_board((d_m - 20),0);
	//		}
	//		else
	//		{
	//			// ����������� �����
	//			std::cout << int_to_char_board((d_m - 10),0);
	//		}
	//	}
	//}
	//	std::cout << std::endl;
//std::endl
}

/*
 �������� ���������� � �����
 � ����

 <1:info depth 2 seldepth 2 score cp 12 nodes 160 nps 5161 time 31 pv g1f3 g8f6
         ������� ��������� ������       ����      �������� �����   ����� ��������
*/
void Uci_engine_to_gui::print_pv_line
(
	struct Position & position,// ������������� �����
	const __int64 nodes,// ���������� ������������� �����(�������)
	const double time_current,  // ������� ������� ������ ��� ������ ��������
	const unsigned __int8 depth,
	bool & flag_mate,// ������ ��� 
	const __int64 nodes_y
 )
{

	__int8 inv;// ���� ����� ����
	__int32 move_n;

	__int32 value_root = PV_save::get_pv_best().score_move;

// TEST
#if TEST_UCI_INFO
	Test_info.open ("_TEST_UCI_INFO.txt",std::ios::app);

	Test_info << " " << std::endl;
	Test_info << "-------------------------------------" << std::endl;

	Test_info << " depth " << (__int32)depth << std::endl;
	Test_info << " seldepth " << (__int32)PV_save::get_pv_best().depth_max << std::endl;

	position.colour_move == 0 ? inv = -1 : inv = 1;

	// �������� ������. ��� ���� ����������� ������������� � ���� ���������
	if (inv * value_root > 900000)
	{ 
		Test_info  << " score mate "<< (__int32)PV_save::get_pv_best().depth_max;// ��������� �� ���� � ���������
	}
	else if (inv * value_root < -900000)
	{
		Test_info  << " score mate "<< -(__int32)PV_save::get_pv_best().depth_max;// ��������� �� ���� � ���������
	}
	else
	{
		// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
		Test_info  << " score cp "<< inv * value_root << std::endl;
	}

	Test_info << " nodes_y "<< (__int32)nodes_y << std::endl;
	Test_info << " nodes "<< (__int32)nodes << std::endl;
	Test_info << " nps " << (__int32)(nodes / time_current) << std::endl;
	Test_info << " time ms " << 
		(__int32) (time_current * Time_management::get_clocks_per_sec()) << std::endl;
	Test_info << " time double " << time_current << std::endl;

	// �������� ����� ��������
	Test_info << " pv ";

	for (unsigned __int8 n = 0; n < PV_save::get_pv_best().depth_max; n++)
	{
		move_n = PV_save::get_pv_best().move[n];
		Test_info << x_to_notation(((move_n >> 12) & 63) % 8);
		Test_info << (1 + ((move_n >> 12) & 63) / 8);
		Test_info << x_to_notation((( move_n >> 18) & 63) % 8);
		Test_info << (1 + ((move_n >> 18) & 63) / 8);

		// ��������� ��� ����
		__int32 d_m = ((move_n >> 6) & 63);

		// ���� d_m ������ 5 �� ������ ����� ����������� �����
		if (d_m > 5)
		{
			if (d_m > 15)
			{
				Test_info << int_to_char_board((d_m - 20),0);// ����������� ����� � �������
			}
			else
			{
				Test_info << int_to_char_board((d_m - 10),0);// ������ ����������� �����
			}
		}
		Test_info << " ";
	}
	Test_info << std::endl;

	Test_info.close();
#endif//#if TEST_UCI_INFO


	std::cout << "info " ;

	//�������
	std::cout << " depth " << (__int32)depth;

	// ������� �������������� ��������
	std::cout << " seldepth " << (__int32)PV_save::get_pv_best().depth_max;

	// � ���� ������ ����������(������ �� ������� �����) 
	// � UCI ������� �� ������� ��������
	position.colour_move == 0 ? inv = -1 : inv = 1;

	// �������� ������. ��� ���� ����������� ������������� � ���� ���������
	if (inv * value_root > 900000)
	{ 
		flag_mate = 1;

		// ��������� �� ���� � ���������
		std::cout << " score mate " << 
			(__int32)(PV_save::get_pv_best().depth_max / 2);
	}
	else if (inv * value_root < -900000)
	{
		flag_mate = 1;

		// ��������� �� ���� � ���������
		std::cout << " score mate " << 
			-(__int32)(PV_save::get_pv_best().depth_max / 2);
	}
	else
	{
		flag_mate = 0;

		// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
		std::cout << " score cp " << inv * value_root;
	}
 
	// ��� ����� ������� ������
	// std::cout << " score cp " << inv * value_root;

	// ������ ���������� �����
	std::cout << " nodes " << (__int32)nodes; 

	// ���������� ����� �� ����
	//std::cout << " nodes "<< (__int32)nodes_y;

	// �������� �� ������� �� ���� ����� �� ������ �������
	std::cout << " nps " << (__int32)(nodes / time_current);

	// ����� ����� �� ���� ����� �� ������ ������� � ms
	std::cout << " time " << 
		(__int32) (time_current * Time_management::get_clocks_per_sec());

	// �������� ����� ��������
	std::cout << " pv ";

	for (unsigned __int8 n = 0; n < PV_save::get_pv_best().depth_max; n++)
	{
		move_n = PV_save::get_pv_best().move[n];

		std::cout << x_to_notation(((move_n >> 12) & 63) % 8);
		std::cout << (1 + ((move_n >> 12) & 63) / 8);
		std::cout << x_to_notation((( move_n >> 18) & 63) % 8);
		std::cout << (1 + ((move_n >> 18) & 63) / 8);

		// ��������� ��� ����
		__int32 d_m = ((move_n >> 6) & 63);

		// ���� d_m ������ 5 �� ������ ����� ����������� �����
		if (d_m > 5)
		{
			if (d_m > 15)
			{
				// ����������� ����� � �������
				std::cout << int_to_char_board((d_m - 20),0);
			}
			else
			{
				// ������ ����������� �����
				std::cout << int_to_char_board((d_m - 10),0);
			}
		}
		std::cout << " ";
	}

	//std::cout << std::endl;
	std::cout << std::endl;
}


/*
 ������� ������� ��� � ��������
*/
void Uci_engine_to_gui::print_currmove
(
	const struct List & list_surplus_moves,//������ ��������� �����
	const unsigned __int8 nodes_0,// ���������� ����� �� ������� �������
	const unsigned __int8 i,// ����� ���� �� ������
	const unsigned __int8 Depth_Max// ������������ �������
)
{
	// �������������� � ������ ������ ���
	std::cout << "info currmove " ;
	std::cout <<x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
	std::cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
	std::cout <<x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
	std::cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8);
	// ���������� ����� ��������������� ����
	std::cout << " currmovenumber " ;
	std::cout << (__int32)nodes_0;
	// �������� ���������� � ������� ���������
	std::cout << " depth " << (__int32)Depth_Max;
	std::cout << std::endl;
	std::cout.flush();
}


/*
 ������� ������� ������� � ��������
*/
void Uci_engine_to_gui::print_pv_line_s
(
	const bool colour_move,// ���� ���������� �������
	const struct List & list_surplus_moves,//������ ��������� �����
	const struct PV_line & PV_best_point,// ������ �������
	const __int64 print_nodes,// ���������� �����
	const unsigned __int8 i,// ����� ���� � ������
	const unsigned __int8 Depth_Max,// ������������ �������
	const __int32 value,// ������ �������
    const bool all_line// ����� ���� �����
		//,const double time_nodes// ����� ������� ��������
)
{
	__int32 inv = 0;// ��������� ���� ������� �������
	__int32 d_m = 0;// ������������ ������ ����������� �����

	/*
	// Here are examples of the possible info output: 
	< info currmove e8e7 (current calculated move) +
	< info currmovenumber 29 +
	< info nps 291506 (nodes per sec, Fritz calculates the nps itself)
	< info tbhits 830
	< ino depth 9
	< info score cp12 (in centipawns)
	< info score mate 5 (or mate -5)
	< info nodes 288973
	< info time 11192 (milliseconds)
	< info pv e2e4 e7e5 g1f3 b8c6 f1d3 f8c5 e1g1 g8e7 b1c3 
	*/

	if (save_value == value)
	{
		std::cout <<std::endl;
	}

	// ������� ������� ������ � ��� ������ ���� ������ ������ � ������ � ������
	// ������� ���������� ��� �� �� ���� ������ ������ � ���� �� ��� ��� 

	// ���� ������ ���������� ��� ���������� ������� �� ������� �������
	if (all_line ||
		((save_value != value) // ���������� ������
		||(save_Depth_Max != Depth_Max)) // ���������� �������
		)
	{
		save_value = value;
		save_Depth_Max = Depth_Max;


		std::cout << "info " ;
		//std::cout << " nps "<<(int)(nodes/time_nodes);
		std::cout << " depth "<< (__int32)Depth_Max;
		//�������� ���������� � ������� �������������� ��������
		std::cout << " seldepth "<< (__int32)PV_best_point.depth_max;

		//�������� ���������� � ������ �������(��� ������ �� ������ ������ ������ �������� � ������ ���������)
		colour_move == 0 ? inv = -1 : inv = 1;

		if (inv * value > 900000)
		{
			std::cout << " score mate " << (__int32)PV_best_point.depth_max;// ��������� �� ���� � ���� �����
		} 
		else if(inv * value < -900000)
		{
			std::cout << " score mate " << -(__int32)PV_best_point.depth_max;// ��������� �� ���� � ���� �����
		}
		else
		{
			std::cout << " score cp " << inv * value;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
		}

		//�������� ���������� � ���������� ������������� �����
		std::cout << " nodes "<<(__int32)print_nodes;

		//��������� �� ������ ������ ��� �� �� ��������� �� ����� ������� �� ������
		if ( (value * value) != (PV_best_point.score_move * PV_best_point.score_move) ) 
		//if(0)
		{
			std::cout << std::endl;
			std::cout << "ERROR LINE " ;
			std::cout << std::endl;
			std::cout << " value = " << value;
			std::cout << std::endl;
			std::cout << " PV_best_point.score_move = " << PV_best_point.score_move;
			std::cout << std::endl;
			std::cout << "**********************"<<std::endl;
			std::cout << "  " << std::endl;
		}
		else
		{
			//����� ����� ��������
			std::cout << " pv ";

			for (unsigned __int8 n = 0; n < PV_best_point.depth_max; n++)
			{
				std::cout << x_to_notation(((PV_best_point.move[n]>>12)& 63)%8);
				std::cout << (1 + ((PV_best_point.move[n]>>12)& 63)/8);
				std::cout << x_to_notation(((PV_best_point.move[n]>>18)& 63)%8);
				std::cout << (1 + ((PV_best_point.move[n]>>18)& 63)/8);

				d_m = ((PV_best_point.move[n]>>6)& 63);

				//���������� ������ ����������� �����
				if (d_m != 5)
				{
					if (d_m > 15)
					{
						std::cout <<int_to_char_board((d_m - 20),0);
					}
					else
					{
						std::cout <<int_to_char_board((d_m - 10),0);
					}
				}
				std::cout <<" ";
			}
		}
		std::cout <<std::endl;
	}
	std::cout.flush();// ��� ��� �� ����� �� ��� ����� ������� :)
}


/*
 ������� ����������� � ���� ��� ����(Andchess) �� 9.01.06 
 ������� ��� ������������ �������� :)) 
 ������������ ����� ������ � �����
 0-��� ������, 1-�����, 2-����, 3-����, 4-�����, 5-�����, 6-������	
*/
__int8 Uci_engine_to_gui::int_to_char_board
(
	const int i,// ��� ������
	const int z// ���� ������
)
{
	__int8 ch = ' ';
	if (z == 0)
	{
		if (i == 0) ch = 'x'; 
		if (i == 1) ch = 'p'; 
		if (i == 2) ch = 'n'; 
		if (i == 3) ch = 'b'; 
		if (i == 4) ch = 'r'; 
		if (i == 5) ch = 'q'; 
		if (i == 6) ch = 'k'; 
	}
	else
	{
		if (i == 0) ch = 'x'; 
		if (i == 1) ch = 'P'; 
		if (i == 2) ch = 'N'; 
		if (i == 3) ch = 'B'; 
		if (i == 4) ch = 'R'; 
		if (i == 5) ch = 'Q'; 
		if (i == 6) ch = 'K'; 
	}
	return ch;
}

/*
 �� ���������� � ����� ������ ����� ������� �������� 0 - �	
*/
__int8 Uci_engine_to_gui::x_to_notation
(
	const int x//�������������� ���������� ����� (0-7)
)
{
	if (x == 0) return 'a';
	if (x == 1) return 'b';
	if (x == 2) return 'c';
	if (x == 3) return 'd';
	if (x == 4) return 'e';
	if (x == 5) return 'f';
	if (x == 6) return 'g';
	if (x == 7) return 'h';
	return '-';
}
