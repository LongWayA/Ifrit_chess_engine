#include <iostream>
#include <fstream>  // TEST

#include "Go.h"
#include "Util.h"
#include "Timer.h"
#include "Evaluate.h"
#include "Hash.h"

#include "ZObrist.h"
#include "Search_root.h"

//--------------------------------------
// �������� �������������� ������� �������� �� �� ������������� ������� 
// ��� ������������� #if #endif
// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 10.02.2008  11.11.2009 11.07.10
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

������� � ����������� ����������� �������
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


extern class Evaluate evaluate;
class Search_root search_root;
extern class ZObrist zobrist;
extern class Timer timer;
extern class Util util;

//	===================================================================
//
void Go::deepening
(
	class Chess_bitboard & bitboard,//������������� �����
	const __int8 depth_max_go_command,//������������ ������� ��������
	const double wtime_go_command,//���������� ����� �� ���� �� �����
	const double btime_go_command,//���������� ����� �� ���� �� ������
	const double winc_go_command,//���������� ������� �� ��� ��� �����
	const double binc_go_command,//���������� ������� �� ��� ��� ������
	const __int8 movestogo_go_command,//���������� ����� �� ������� �������� �����
	const double movetime_go_command,//����� �� ���
	const bool infinite_go_command//����������� ������
)
{

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
	//Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



	const __int32 Alpha = -9999999;
	const __int32 Beta  =  9999999;

	__int8 inv = 0;// ���� ����� ����
	__int32 value_root = 0;// ������ ������� �������� � ��������� ����. 
	__int32 d_m = 0; // ��� ����. ����� ��� ����������� ���� ������ � ������������
	__int32 test_eval = 0;// ���������� ������ ��������� ��� ��������
	bool flag_mate = 0;// ������ ��� 
	unsigned __int64 key_control = 0;   // ���������� ���-���� ��� ��������
	class List_surplus_moves root_list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)

	__int64 nodes = 0;// ���������� ������������� �����(�������)

	// ����� ������� � ��������
	double max_time_move_search = -1.0;// ����� ���������� �� ���
	double time_current = -1.0;  // ������� ������� ������ ��� ������ ��������
	unsigned __int16 cnt_move ;// ���������� ��� ��������� � ������ ���������
	__int16 limit_cnt_move ;// ������� ����� �������� �������
	unsigned __int8 depth_max = 0;//������������ ������� ��������


	double remember_time_current = 1.0;  // ����������

	/*
		��� ������������� ������� ���� �� ����� ������ � ����� ���� 
		��� ��� �� ���������
		����� ��� ����� ������� ������ �� ������ ���������:

	*/
 
		// TEST ---------
		//std::cout << "depth_max_go_command = " << (int)depth_max_go_command << "\n";
		//std::cout << "infinite_go_command = " << infinite_go_command << "\n";
		//std::cout << "movetime_go_command = " << movetime_go_command << "\n";
		//std::cout << "movestogo_go_command = " << (int)movestogo_go_command << "\n";

		//std::cout << "winc_go_command = " << winc_go_command << "\n";
		//std::cout << "binc_go_command = " << binc_go_command << "\n";
		//std::cout << "wtime_go_command = " << wtime_go_command << "\n";
		//std::cout << "btime_go_command = " << btime_go_command << "\n";
		// ---------------

		// TEST
		//std::cout << "koff_deepening = " << koff_deepening << "\n";

	if (depth_max_go_command != -1) // > go depth 5 
	{
		// TEST
		//std::cout << "depth_max_go_command " << "\n";

		// ����� �� �������� �������
		depth_max = depth_max_go_command;
	}
	else if (infinite_go_command != false) // > go infinite
	{
		// TEST
		//std::cout << "infinite_go_command " << "\n";

		// ������ ��� ����������� �������. 50 ���� ����� ����� ����������
		depth_max = 50;
	}
	else if (movetime_go_command != -1)	// > go movetime 300000 
	{
		// TEST
		//std::cout << "movetime_go_command " << "\n";

		//������ ����� �� ���
		max_time_move_search = movetime_go_command;
		depth_max = 50;
	}
	//else if (movestogo_go_command != -1) // > go movestogo 40 wtime 300000 btime 300000 
	//{
	//// ������ �� �� ��� ����������� �� ������� 
	//// ������� ����� ��������, �.�. �� ��������� movestogo
 //   // ������ ��������� �� ������ �����. ��� ��� ���� ��� ��������
 //   //----------------------------------------------------------------------------------
	//	//// TEST
	//	////std::cout << "movestogo_go_command " << "\n";

	//	//// ����� �� ������������ ���������� �����
	//	//// ��������(�������� �����) ���� ������� ������� ����� 
	//	//// ��������, �.�. ��������� movestogo

	//	//// ���������� ��������� ��������� � ������
	//	//cnt_move = zobrist.get_save_cnt_move();

	//	//// ��� ���������� ���������� �����
	//	//limit_cnt_move = movestogo_go_command;

	//	//// TEST
	//	////std::cout << "cnt_move = " << cnt_move << "\n";
	//	////std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";

	//	//// ��������� ��������� �����
	//	//if (limit_cnt_move < 5)
	//	//{
	//	//	limit_cnt_move = 5;
	//	//}

	//	//// ����� ���������� �� ������ ����� �� ���������� ����� 
	//	//// ������ ��� ���������� ��������. ����� ����� �������� �������
	//	//if (bitboard.colour_move == 1)// 1-��� ����� 0- ��� ������
	//	//{

	//	//	max_time_move_search = winc_go_command +  wtime_go_command/limit_cnt_move;
	//	//}
	//	//else
	//	//{
	//	//	max_time_move_search = binc_go_command +  btime_go_command/limit_cnt_move;
	//	//}

 //	//	depth_max = 50;
 //   //----------------------------------------------------------------------------------

 //   //----------------------------------------------------------------------------------
	//	// ���������� ��������� ��������� � ������
	//	cnt_move = zobrist.get_save_cnt_move();

	//	// �� ������������ ��� ��� ������ ���� �� ������������ ���������� ����� (80)
	//	// ��� ���������� ���������� �����
	//	limit_cnt_move = 80 - cnt_move/2 ;

	//	// ����� �� ������ 60 ����� �������� ����������
	//	// ����� 60 �� ���� ���� ���������� ������� �� ��� ��� �� 
	//	// �������� ��������� �� �������
	//	if (limit_cnt_move < 20)
	//	{
	//		limit_cnt_move = 20;
	//	}

	//	//limit_cnt_move = 20;

	//	if (bitboard.get_colour_move() == 1)// 1-��� ����� 0- ��� ������
	//	{

	//		max_time_move_search = winc_go_command + wtime_go_command/limit_cnt_move;
	//	}
	//	else
	//	{
	//		max_time_move_search = binc_go_command + btime_go_command/limit_cnt_move;
	//	}

 //		depth_max = 50;
 //   //----------------------------------------------------------------------------------
	//}
	else if ( (wtime_go_command != -1.0) || (btime_go_command != -1.0)) // > go btime 300000 wtime 300000 
	{
		// TEST
		//std::cout << "wtime_go_command or btime_go_command " << "\n";
		//���������� ����� �� ����

		// ���������� ��������� ��������� � ������
		cnt_move = zobrist.get_save_cnt_move();

		// �� ������������ ��� ��� ������ ���� �� ������������ ���������� ����� (80)
		// ��� ���������� ���������� �����
		limit_cnt_move = 80 - cnt_move/2 ;

		// ����� �� ������ 60 ����� �������� ����������
		// ����� 60 �� ���� ���� ���������� ������� �� ��� ��� �� 
		// �������� ��������� �� �������
		if (limit_cnt_move < 20)
		{
			limit_cnt_move = 20;
		}

		//limit_cnt_move = 20;

		if (bitboard.get_colour_move() == 1)// 1-��� ����� 0- ��� ������
		{

			max_time_move_search = winc_go_command + wtime_go_command/limit_cnt_move;
		}
		else
		{
			max_time_move_search = binc_go_command + btime_go_command/limit_cnt_move;
		}

 		depth_max = 50;
	}


		// TEST
		//std::cout << "max_time_move_search = " << max_time_move_search << "\n";

		if (max_time_move_search != -1)
		{
			// �� ���������� � �������� �� ������� ��� ��������� �� ������ �������.
			// ������������ ��� �� ��������� ������ ����� ����� ����� � ��������� ���
			// ������. ������� �� ����� ����� �� ��� ����� ��� �� �� ���� ���������
			max_time_move_search = max_time_move_search/3;
		}

		// TEST
		//std::cout << "koff max_time_move_search = " << max_time_move_search << "\n";

	// �� ������� �������� ������ ��� ������������ ���������� ������� �� ������
	// ������ ��� � ~17 ���
	search_root.ini_deep_time_terminate( (max_time_move_search * 20) );

	//-------------------------------------------------------------------------------


	// ������� ��������. � ���������� ������ ������������ ������ ��� �������.
	bitboard.set_value_white_material(evaluate.eval_white_material(bitboard));
	bitboard.set_value_black_material(evaluate.eval_black_material(bitboard));

	// ��� �������� ��� ����� ����������� ������ ��������� � ��������������� ���������
    test_eval =  bitboard.get_value_white_material() 
		- bitboard.get_value_black_material();

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << "\n";

	// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
	zobrist.start_position_random_key(bitboard);

	// ���������� ���� ��� �������� ��� ����������� � �������
	key_control = bitboard.get_hash_key();

	//�������������� ��� - ������ �������� ������������� ������� 
	// ���-�� ��������� ������ ������������
	zobrist.ini_hash_three(); 

	// ���������� ���� ��� ������������ ������� �� ������ �������
	zobrist.save_hash_three(bitboard.get_hash_key(),0);

	// TEST
	// ����� ������� ���-�������
	// �������������� ��� ����� � ���-������� 0
	//   HASHM::Ini_hash_moves();

	// ��������� ��� ������������ �������������� ��������
	// TEST
	//depth_max = depth_max - 1;

	// TEST
	//std::cout << "1111111 " << "\n";

	// �������������� ���������� ���������� ������ loop
	search_root.ini_global_loop(depth_max);

	if (bitboard.get_colour_move() == 1)
	{  
		// ����� 
		//��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
		root_list_surplus_moves.generator_captures_white(bitboard);// ���������� ������ � �����������
		root_list_surplus_moves.generator_move_white(bitboard);// ���������� ��������� ����
	}
	else
	{
		// ������
		//��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
		root_list_surplus_moves.generator_captures_black(bitboard);// ���������� ������ � �����������
		root_list_surplus_moves.generator_move_black(bitboard);// ���������� ��������� ����
	}

	// TEST
	//std::cout << "22222222222 " << "\n";
	//std::cout << " depth_max " << depth_max << "\n";//�������

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
	// TEST
	// �������� ��������
	HASHM::Test_hash_ini();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

	// ���� ������� ������ ������������ � �� ����� ��� � ������ 
	// 50 � �� ���� ������� �� ��������� �� ���������� ����
	for (unsigned __int8 depth = 1;

	// ��� ������� �� ����� �����
	( (depth <= depth_max) && // ������� ������ ������������ �������� �� ���� �������
	(flag_mate == 0) && // ���� ����
	(depth < 51) && // ������� ������ ������������ 
	(search_root.get_stop_search() == 0)// ��� ������� �� ��������� ����� 
	);

	depth++
	)
	{

		// ���������� ������ ���������� ����� � ���������� �������� �.� �������
		nodes_previous = nodes;

		//���������� ������� �������
		search_root.set_nodes_in_0();

		// ��������� ������
		timer.time_start();

		if (bitboard.get_colour_move() == 1)
		{
			value_root = search_root.white_root(bitboard,
				root_list_surplus_moves,Alpha,Beta,depth);
		}
		else
		{
			value_root = search_root.black_root(bitboard,
				root_list_surplus_moves,Alpha,Beta,depth);
		}

		// ������������� ������
		timer.time_stop();
		time_current = timer.time_delta();

        koff_deepening = time_current/remember_time_current;
		remember_time_current = time_current;

		nodes = search_root.get_nodes();

		// ���������� ����� �� ���� ������� ��� ������ ���������� �����
		// ����� ������ ���������� ����� �� ���������� ��������
		nodes_y = nodes - nodes_previous ;

		// �������� ������ ���������
		if (test_eval != (bitboard.get_value_white_material() - bitboard.get_value_black_material()))
		{
			std::cout << "ERROR test_eval != bitboard->eval(in Iterative_Deepening.cpp)" << "\n";
		}
 
		//-----------------------------------------------
		// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� 
		// � ������� �� �������� � 
		// ��������� ������� �� ���� �� ���������� �� �������
		if (key_control != bitboard.get_hash_key())
		{
			std::cout <<"\n";
			std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
			std::cout <<"\n";
		}
		//-----------------------------------------------

		//---------------------------------------------------------------
		// �������� ���������� � �����
		std::cout << "info " ;
		std::cout << " nps " << (__int32)(nodes / time_current);// �������� �� ������� �� ���� ����� �� ������ �������
		std::cout << " depth " << (__int32)depth;//�������

		// �������� ���������� � ������� �������������� ��������
		std::cout << " seldepth "<< (__int32)search_root.get_pv_best().depth_max;

		bitboard.get_colour_move() == 0 ? inv = -1 : inv = 1;

		// �������� ������. ��� ���� ����������� ������������� � ���� ���������
		if (inv * value_root > 900000)
		{ 
			flag_mate = 1;
			std::cout << " score mate "<< (__int32)search_root.get_pv_best().depth_max;// ��������� �� ���� � ���������
		}
		else if (inv * value_root < -900000)
		{
			std::cout << " score mate "<< -(__int32)search_root.get_pv_best().depth_max;// ��������� �� ���� � ���������
			flag_mate = 1;
		}
		else
		{
			std::cout << " score cp "<< inv * value_root;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
			flag_mate = 0;
		}
 
		// std::cout << " score cp "<< inv*value_root;// ��� ����� ������� ������

		// �������� ����� ��������
		//std::cout << " nodes " << (__int32)nodes; // ������ ���������� �����
		std::cout << " nodes "<< (__int32)nodes_y; // ���������� ����� �� ����
		std::cout <<" pv ";

		for (unsigned __int8 n = 0; n < search_root.get_pv_best().depth_max; n++)
		{
			std::cout << util.x_to_notation(((search_root.get_pv_best().move[n]>>12)& 63)%8);
			std::cout << (1 + ((search_root.get_pv_best().move[n]>>12)& 63)/8);
			std::cout << util.x_to_notation(((search_root.get_pv_best().move[n]>>18)& 63)%8);
			std::cout << (1 + ((search_root.get_pv_best().move[n]>>18)& 63)/8);

			// ��������� ��� ����
			d_m = ((search_root.get_pv_best().move[n]>>6)& 63);

			// ���� d_m ������ 5 �� ������ ����� ����������� �����
			if (d_m > 5)
			{
				if (d_m > 15)
				{
					std::cout << util.int_to_char_board((d_m - 20),0);// ����������� ����� � �������
				}
				else
				{
					std::cout << util.int_to_char_board((d_m - 10),0);// ������ ����������� �����
				}
			}

			std::cout << " ";
		}

		std::cout <<"\n";

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
		// TEST

		// ������� ��������� � ������� � ������� ������������� �.�. �� ������� �� ��������
		HASHM::TEST_Hash_index_print();

		// �������� ���� �� ������������� �����������
		HASHM::Test_hash_print();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

		// ������� �� �������
		if ((max_time_move_search != -1) && (max_time_move_search < time_current) && (depth >= 4))
		{
			break;
		}

	}

	// TEST
	//std::cout << "cnt_move = " << cnt_move << "\n";
	//std::cout << "wtime = " << wtime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout <<"time_current = "<< time_current <<"\n";
	//std::cout <<"   " <<"\n";

	//-----------------------------------------------
	// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
	// ��������� ������� �� ���� �� ���������� �� �������
	if (key_control != bitboard.get_hash_key())
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}
	//-----------------------------------------------


	//---------------------------------------------------------------------
	// ������ ��� ���� < bestmove e2e4 
	// ��� ��� ����������� � ����(� �������)  < bestmove d7d8N 
	std::cout << "bestmove ";
	std::cout << util.x_to_notation(((search_root.get_pv_best().move[0]>>12)& 63)%8);
	std::cout << (1 + ((search_root.get_pv_best().move[0]>>12)& 63)/8);
	std::cout << util.x_to_notation(((search_root.get_pv_best().move[0]>>18)& 63)%8);
	std::cout << (1 + ((search_root.get_pv_best().move[0]>>18)& 63)/8);

	// ��������� ��� ����
	d_m = ((search_root.get_pv_best().move[0]>>6)& 63);

	// ���� d_m ������ 5 �� ������ ����� ����������� �����
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			// ����������� ����� � �������
			std::cout << util.int_to_char_board((d_m - 20),0);
		}
		else
		{
			// ����������� �����
			std::cout << util.int_to_char_board((d_m - 10),0);
		}
	}

	std::cout <<"\n";

	// �������� ������������ ������� ���� ������� ���� ����� ��������
	if (search_root.get_from() 
	!= ((search_root.get_pv_best().move[0]>>12)& 63))
	{
		std::cout << "ERROR from(in Iterative_Deepening.cpp)" <<"\n";
	}

	if (search_root.get_to() 
	!= ((search_root.get_pv_best().move[0]>>18)& 63))
	{
		std::cout << "ERROR to(in Iterative_Deepening.cpp)" <<"\n";
	}
//---------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}