
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 10.02.2008  11.11.2009 11.07.10 24.5.2012

//!
// �������� �������������� ������� �������� �� �� ������������� ������� 
// ��� ������������� #if #endif
// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 

#include <fstream>
#include <iostream>

#include "_search_switch.h"
#include "a_move_list_struct.h"
#include "iterative_deepening.h"
#include "search_root.h"
#include "search.h"
#include "evaluate.h"
#include "move_generation.h"
#include "killer_heuristic.h"
#include "history_heuristic.h"
#include "zobrist_hashing.h"
#include "pv_save.h"
#include "time_management.h"
#include "uci_engine_to_gui.h"
#include "b_search_const.h"

/// ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_D 0; 

/// ��������-���������(1-0) ����� �������� ����������
/// ���������� � ��������� �������
/// ���� ��� �������� ���� TEST_WRITE_READ � j_Hash.h
#define TEST_WRITE_READ_WN 0 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*
 ������� � ����������� ����������� �������

 ���������� ���������� ���������� ������� �� ��� ��� ���� �� ��������� �������.
 �.�. � ������ �� ������� 1 ����� 2 � �.�.
 ��� ����� �� ������ ��� ���������� ���-�������
 �� ������ ����� ����������� ����� ��������. �������� ���� �� �� ���������� �������
 ������� ������� �� �� ��������� � ������� � �������� ��� ������.
*/
void Iterative_deepening::loop
(
	struct Position & position,// ������������� �����
	unsigned __int8 & depth_limit,
	double & time_move_search_limit
)
{

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
	//Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	const __int32 Alpha = -INFINITE_SCORE;
	const __int32 Beta  =  INFINITE_SCORE;

	__int32 value_root = 0;// ������ ������� �������� � ��������� ����. 
	bool flag_mate = 0;// ������ ��� 
	__int64 nodes = 0;// ���������� ������������� �����(�������)

	__int32 test_eval = 0;// ���������� ������ ��������� ��� ��������
	unsigned __int64 key_control = 0;   // ���������� ���-���� ��� ��������

	// ����� ������� � ��������
	double time_current = -1.0;  // ������� ������� ������ ��� ������ ��������
	double remember_time_current = 1.0;  // ����������

	__int64 nodes_y;             /// ���������� ����� �� ������ �������
	__int64 nodes_previous;      /// ���������� ������ ���������� �����
	double koff_deepening;       /// ����������� ��������� ������ �� ������� 
							 /// ��������� ��������� �������

	bool flag_stop_search;
	__int32 bestmove;
	__int32 old_bestmove;

	// �� ������� �������� ������ ��� ������������ ���������� ������� �� ������
	// ������ ��� � ~ 5 ���
	Search::ini_deep_time_terminate( (time_move_search_limit * 15) );

	// ������� ��������. � ���������� ������ ������������ ������ ��� �������.
	position.value_white_material = Evaluate::eval_white_material(position);
	position.value_black_material = Evaluate::eval_black_material(position);

	//TEST
	// ��� �������� ��� ����� ����������� ������ ��������� � ��������������� ���������
    test_eval =  position.value_white_material 
		- position.value_black_material;

	// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
	Zobrist_hashing::start_position_random_key(position);

	//�������������� ��� - ������ �������� ������������� ������� 
	// ���-�� ��������� ������ ������������
	Zobrist_hashing::ini_hash_three(); 

	// ���������� ���� ��� ������������ ������� �� ������ �������
	Zobrist_hashing::save_hash_three(position.hash_key,0);

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << std::endl;

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
	//std::cout << "1111111 " << std::endl;

	// �������������� ���������� ����������
	Search::ini_global_loop();

#if SW_HISTORY
	// �������� �������
	History_heuristic::ini_history();
#endif // #if SW_HISTORY

	// �������������� ������ ��������
	Killer_heuristic::ini_killer(); 

	// TEST
	//std::cout << "22222222222 " << std::endl;
	//std::cout << " depth_max " << depth_max << std::endl;//�������

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
	// TEST
	// �������� ��������
	HASHM::Test_hash_ini();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------


	// ���� ������� ������ ������������ � �� ����� ���
	// � �� ���� ������� �� ��������� �� ���������� ����
	for (unsigned __int8 depth_max = 1; depth_max <= depth_limit; depth_max++)
	{
		// ���������� ������ ���������� ����� � ���������� �������� �.� �������
		nodes_previous = nodes;

		//���������� ������� �������
		Search_root::set_nodes_in_0();

		// ��������� ������
		Time_management::time_start();

		// ������������ �������
		if (position.colour_move == 1)
		{
			value_root = Search_root::white_root_searching_max_value
										(position,Alpha,Beta,depth_max);
		}
		else
		{
			value_root = Search_root::black_root_searching_min_value
										(position,Alpha,Beta,depth_max);
		}

		// ������������� ������
		Time_management::time_stop();

		// ������� ���������� �������
		time_current = Time_management::time_delta();

		// ��� � �� ����� ��� ���� �����
        koff_deepening = time_current/remember_time_current;
		remember_time_current = time_current;

		// ���������� ������������� �����
		nodes = Search_root::get_nodes();

		// ���������� ����� �� ���� ������� ��� ������ ���������� �����
		// ����� ������ ���������� ����� �� ���������� ��������
		nodes_y = nodes - nodes_previous ;

		flag_stop_search = false;

		// ������ ��� � ����� ��������
		bestmove = PV_save::get_pv_best().move[0];

		// ������� �� ������� �� ��������� �����
		if (Search::get_stop_search() != 0)
		{
			if (depth_max > 1) flag_stop_search = true;
			break;
		}

		// �������� ���������� � �����
		Uci_engine_to_gui::print_pv_line
			(position,nodes,time_current,depth_max,flag_mate,nodes_y);

		// TEST
		//// ������� �� ������� �� ��������� �����
		//if (Search::get_stop_search() != 0)
		//{
		//	if (depth_max > 1) flag_stop_search = true;
		//	break;
		//}

		// ���������� ������ ���
		old_bestmove = bestmove;

		// ������� �� �������
		if ((time_move_search_limit != -1) 
			&& (time_move_search_limit < time_current) 
			&& (depth_max >= 4)
			)
		{
			break;
		}

		// ������� �� ����
		if (flag_mate != 0)
		{
			break;
		}


		// ��������� ���������
		test_loop(position,value_root,test_eval,key_control);

//TEST
#if TEST_WRITE_READ_WN
		// TEST

		// ������� ��������� � ������� � ������� ������������� �.�. �� ������� �� ��������
		HASHM::TEST_Hash_index_print();

		// �������� ���� �� ������������� �����������
		HASHM::Test_hash_print();
#endif // #if HASHM::TEST_WRITE_READ_WN


	}

if (flag_stop_search)
{
	flag_stop_search = false;

	// ����� �� ������� ������� ��� ������� ������� ���������� ��� � ���������� �������
	bestmove = old_bestmove;
}

	// ������ ��� (�������� bestmove e2e4)
	Uci_engine_to_gui::do_bestmove(bestmove);


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}

/*
�������� ������ ������� �����, �������� ������ ���������
��������� ���-���� �� ������������, 
�������� ������������ ������� ���� ������� ���� ����� ��������
*/ 
void Iterative_deepening::test_loop
(
	struct Position & position,// ������������� �����
	__int32 value_root,
 	const __int32 test_eval,// ���������� ������ ��������� ��� ��������
	const unsigned __int64 key_control// ���������� ���-���� ��� ��������
)
{
	//TEST
	// �������� ������ ������� �����
	if (value_root != PV_save::get_pv_best().score_move)
	{
		std::cout << 
			"ERROR value_root != PV_save::get_pv_best().score_move(iterative_deepening.cpp)" 
			<< std::endl;

		std::cout << "value_root = "<< value_root << std::endl;
		std::cout << "PV_save::get_pv_best().score_move = " 
			<< PV_save::get_pv_best().score_move << std::endl;
	}

	//TEST
	// �������� ������ ���������
	if (test_eval != (position.value_white_material - position.value_black_material))
	{
		std::cout << 
			"ERROR test_eval != bitboard->eval(iterative_deepening.cpp)" 
			<< std::endl;

		std::cout << "test_eval = "<< test_eval << std::endl;
		std::cout << "(position.value_white_material - position.value_black_material) = " 
			<< (position.value_white_material - position.value_black_material) << std::endl;

	}
 
	//TEST
	// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� 
	// � ������� �� �������� � 
	// ��������� ������� �� ���� �� ���������� �� �������
	if (key_control != position.hash_key)
	{
		std::cout <<std::endl;
		std::cout << "ERROR HESH Test_Deepening(iterative_deepening.cpp) " ;
		std::cout <<std::endl;

		std::cout << "key_control = "<< key_control << std::endl;
		std::cout << "position.hash_key = " 
			<< position.hash_key << std::endl;
	}

	//TEST
	// �������� ������������ ������� ���� ������� ���� ����� ��������
	if (Search_root::get_from() 
	!= ((PV_save::get_pv_best().move[0]>>12)& 63))
	{
		std::cout << "ERROR from(iterative_deepening.cpp)" <<std::endl;
	}

	if (Search_root::get_to() 
	!= ((PV_save::get_pv_best().move[0]>>18)& 63))
	{
		std::cout << "ERROR to(iterative_deepening.cpp)" <<std::endl;
	}

}


