
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 10.02.2008  11.11.2009 11.07.10

//!
//--------------------------------------
// �������� �������������� ������� �������� �� �� ������������� ������� 
// ��� ������������� #if #endif
// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 

#include <fstream>
#include <iostream>

#include "Search_switch.h"
#include "List.h"
#include "Go.h"
#include "Search_root.h"
#include "Search.h"
#include "Evaluate.h"
#include "Generator_moves.h"
#include "Killer.h"
#include "History.h"
#include "ZObrist.h"
#include "PV_save.h"
#include "Timer.h"


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


	__int64 nodes_y;             /// ���������� ����� �� ������ �������
	__int64 nodes_previous;      /// ���������� ������ ���������� �����
	double koff_deepening;       /// ����������� ��������� ������ �� ������� 
							     /// ��������� ��������� �������

//	===================================================================
//
void Go::deepening
(
	struct Position & position,// ������������� �����
	unsigned __int8 & depth_max,
	double & max_time_move_search
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

	__int32 value_root = 0;// ������ ������� �������� � ��������� ����. 
	bool flag_mate = 0;// ������ ��� 
	__int64 nodes = 0;// ���������� ������������� �����(�������)

	__int32 test_eval = 0;// ���������� ������ ��������� ��� ��������
	unsigned __int64 key_control = 0;   // ���������� ���-���� ��� ��������

	// ����� ������� � ��������
	double time_current = -1.0;  // ������� ������� ������ ��� ������ ��������
	double remember_time_current = 1.0;  // ����������


	// �� ������� �������� ������ ��� ������������ ���������� ������� �� ������
	// ������ ��� � ~ 5 ���
	Search::ini_deep_time_terminate( (max_time_move_search * 15) );

	// ������� ��������. � ���������� ������ ������������ ������ ��� �������.
	position.value_white_material = Evaluate::eval_white_material(position);
	position.value_black_material = Evaluate::eval_black_material(position);

	//TEST
	// ��� �������� ��� ����� ����������� ������ ��������� � ��������������� ���������
    test_eval =  position.value_white_material 
		- position.value_black_material;

	// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
	ZObrist::start_position_random_key(position);

	//�������������� ��� - ������ �������� ������������� ������� 
	// ���-�� ��������� ������ ������������
	ZObrist::ini_hash_three(); 

	// ���������� ���� ��� ������������ ������� �� ������ �������
	ZObrist::save_hash_three(position.hash_key,0);

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << "\n";

	//TEST
	// ���������� ���� ��� �������� ��� ����������� � �������
	key_control = position.hash_key;

	// TEST
	// ����� ������� ���-�������
	// �������������� ��� ����� � ���-������� 0
	//   HASHM::Ini_hash_moves();

	// ��������� ��� ������������ �������������� ��������
	// TEST
	//depth_max = depth_max - 1;

	// TEST
	//std::cout << "1111111 " << "\n";

	// �������������� ���������� ����������
	Search::ini_global_loop();

#if SW_HISTORY
	// �������� �������
	History::ini_history();
#endif // #if SW_HISTORY

	// �������������� ������ ��������
	Killer::ini_killer(); 

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
		(Search::get_stop_search() == 0)// ��� ������� �� ��������� ����� 
		);
	depth++
	)
	{
		// ���������� ������ ���������� ����� � ���������� �������� �.� �������
		nodes_previous = nodes;

		//���������� ������� �������
		Search_root::set_nodes_in_0();

		// ��������� ������
		Timer::time_start();

		if (position.colour_move == 1)
		{
			value_root = Search_root::white_root(position,Alpha,Beta,depth);
		}
		else
		{
			value_root = Search_root::black_root(position,Alpha,Beta,depth);
		}

		// ������������� ������
		Timer::time_stop();

		// ������� ���������� �������
		time_current = Timer::time_delta();

        koff_deepening = time_current/remember_time_current;
		remember_time_current = time_current;

		nodes = Search_root::get_nodes();

		// ���������� ����� �� ���� ������� ��� ������ ���������� �����
		// ����� ������ ���������� ����� �� ���������� ��������
		nodes_y = nodes - nodes_previous ;

		//
		test_loop(position,test_eval,key_control);

		// �������� ���������� � �����
		print_deepening_line(position,nodes,time_current,depth,value_root,flag_mate);

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
		if ((max_time_move_search != -1) 
			&& (max_time_move_search < time_current) 
			&& (depth >= 4))
		{
			break;
		}

	}

	// ������ ��� ���� < bestmove e2e4 
	do_bestmove();

	// ��������� ���-���� �� ������������ � 
	// �������� ������������ ������� ���� ������� ���� ����� ��������
	test_end_deepening(position,key_control);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}

//	===================================================================
// ������ ��� ���� < bestmove e2e4 
// ��� ��� ����������� � ����(� �������) < bestmove d7d8N 
void Go::do_bestmove()
{
	std::cout << "bestmove ";
	std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[0]>>12)& 63)%8);
	std::cout << (1 + ((Search_root::get_pv_best().move[0]>>12)& 63)/8);
	std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[0]>>18)& 63)%8);
	std::cout << (1 + ((Search_root::get_pv_best().move[0]>>18)& 63)/8);

	// ��������� ��� ����
	__int32 d_m = ((Search_root::get_pv_best().move[0]>>6)& 63);

	// ���� d_m ������ 5 �� ������ ����� ����������� �����
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			// ����������� ����� � �������
			std::cout << PV_save::int_to_char_board((d_m - 20),0);
		}
		else
		{
			// ����������� �����
			std::cout << PV_save::int_to_char_board((d_m - 10),0);
		}
	}

	std::cout <<"\n";
}

//	===================================================================
// �������� ���������� � �����
void Go::print_deepening_line
(
	struct Position & position,// ������������� �����
	const __int64 nodes,// ���������� ������������� �����(�������)
	const double time_current,  // ������� ������� ������ ��� ������ ��������
	const unsigned __int8 depth,
	const __int32 value_root,// ������ ������� �������� � ��������� ����.
	bool & flag_mate// ������ ��� 
 )
{

	__int8 inv;// ���� ����� ����

		std::cout << "info " ;
		std::cout << " nps " << (__int32)(nodes / time_current);// �������� �� ������� �� ���� ����� �� ������ �������
		std::cout << " depth " << (__int32)depth;//�������

		// �������� ���������� � ������� �������������� ��������
		std::cout << " seldepth "<< (__int32)Search_root::get_pv_best().depth_max;

		position.colour_move == 0 ? inv = -1 : inv = 1;

		// �������� ������. ��� ���� ����������� ������������� � ���� ���������
		if (inv * value_root > 900000)
		{ 
			flag_mate = 1;
			std::cout << " score mate "<< (__int32)Search_root::get_pv_best().depth_max;// ��������� �� ���� � ���������
		}
		else if (inv * value_root < -900000)
		{
			std::cout << " score mate "<< -(__int32)Search_root::get_pv_best().depth_max;// ��������� �� ���� � ���������
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

		for (unsigned __int8 n = 0; n < Search_root::get_pv_best().depth_max; n++)
		{
			std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[n]>>12)& 63)%8);
			std::cout << (1 + ((Search_root::get_pv_best().move[n]>>12)& 63)/8);
			std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[n]>>18)& 63)%8);
			std::cout << (1 + ((Search_root::get_pv_best().move[n]>>18)& 63)/8);

			// ��������� ��� ����
			__int32 d_m = ((Search_root::get_pv_best().move[n]>>6)& 63);

			// ���� d_m ������ 5 �� ������ ����� ����������� �����
			if (d_m > 5)
			{
				if (d_m > 15)
				{
					std::cout << PV_save::int_to_char_board((d_m - 20),0);// ����������� ����� � �������
				}
				else
				{
					std::cout << PV_save::int_to_char_board((d_m - 10),0);// ������ ����������� �����
				}
			}

			std::cout << " ";
		}

		std::cout <<"\n";
}

//	===================================================================
// 
void Go::test_end_deepening
(
		struct Position & position,// ������������� �����
		const unsigned __int64 key_control
)
{
	// TEST
	//std::cout << "cnt_move = " << cnt_move << "\n";
	//std::cout << "wtime = " << wtime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout <<"time_current = "<< time_current <<"\n";
	//std::cout <<"   " <<"\n";

	//TEST
	// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
	// ��������� ������� �� ���� �� ���������� �� �������
	if (key_control != position.hash_key)
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}

	//TEST
	// �������� ������������ ������� ���� ������� ���� ����� ��������
	if (Search_root::get_from() 
	!= ((Search_root::get_pv_best().move[0]>>12)& 63))
	{
		std::cout << "ERROR from(in Iterative_Deepening.cpp)" <<"\n";
	}

	if (Search_root::get_to() 
	!= ((Search_root::get_pv_best().move[0]>>18)& 63))
	{
		std::cout << "ERROR to(in Iterative_Deepening.cpp)" <<"\n";
	}
}


//	===================================================================
// 
void Go::test_loop
(
	struct Position & position,// ������������� �����
 	const __int32 test_eval,// ���������� ������ ��������� ��� ��������
	const unsigned __int64 key_control// ���������� ���-���� ��� ��������
)
{
	//TEST
	// �������� ������ ���������
	if (test_eval != (position.value_white_material - position.value_black_material))
	{
		std::cout << "ERROR test_eval != bitboard->eval(in Iterative_Deepening.cpp)" << "\n";
	}
 
	//TEST
	// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� 
	// � ������� �� �������� � 
	// ��������� ������� �� ���� �� ���������� �� �������
	if (key_control != position.hash_key)
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}
}


