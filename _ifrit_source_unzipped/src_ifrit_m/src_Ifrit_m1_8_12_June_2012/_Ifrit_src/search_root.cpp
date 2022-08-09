
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 2.11.2009 20.12.2009 27.09.2011

#include <fstream>
#include <iostream>

#include "_search_switch.h"
#include "b_search_const.h"
#include "search_root.h"
#include "search.h"
#include "move_generation.h"
#include "make_move.h"
#include "test_chess_bitboard.h"
#include "move_ordering.h"
#include "killer_heuristic.h"
#include "history_heuristic.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "b_constants.h"
#include "pv_save.h"
#include "uci_engine_to_gui.h"


//
#define TEST_LIST 0

	static __int64 nodes_root;/// ���������� �����

	///TEST
	/// ��������� ������ ��� �������� ���������� ���� � ��� ��� �������� � ����� ����
	static unsigned __int8 from;/// ������ ����� ������
	static unsigned __int8 to;/// ���� ����� ������

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_LIST
	std::ofstream Test_Loop;
#endif//#if TEST_LIST 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


/*
 ����� �� �����. ����� ��������� ����� ��������
 ������� �� ������� ������
 ������� �� �� ������ ���������� � ��������. 
 ��������� ������ ��� ������ �� ������� ������
*/
__int32 Search_root::white_root_searching_max_value
(
	struct Position & position,// ������������� �����
	__int32 alpha,//
	const __int32 beta,// 
	const unsigned __int8 depth_max// ������������ �������
)
{

	__int32 value;// ������� ������ �������
	__int32 value_max = -INFINITE_SCORE;//������ � ����  ������ �������  
	bool flag_save_best = false;// ����� �� ������ �������
	bool flag_is_legal_moves = false;// ���� ������������� ��������� ����� 
										//(���� 0 �� ��������� ����� �� ����)
	bool flag_check = false;//���� ����
	const unsigned __int8 depth = 0;// �������
	const unsigned __int8 new_depth = 1; // ��������� �������
	unsigned __int8 new_depth_max;//����� ������� ��������

	bool flag_do_pv_search;
	bool flag_do_null_window_search;
	bool flag_do_lmr_search;

	// ���������� ��������� � ������ LMR � PVS
	__int32 beta_null_window;//
	unsigned __int8 new_lmr_depth_max;// ������������ �������
	bool flag_pv_move;// �������� �� ��� ������� (pv)

	struct List list_surplus_moves; // ������ ��������� ����� 
									//(������������ ���� �� ������� ����)


#if SAVE_LINE 
	struct Position position_save;// ������������� �����
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	unsigned __int8 flag_insert_hash_move = 0;// ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = position.hash_key;// ���������� ���-���� �������� ����
	unsigned __int8 i_hash_move = 0; // ����� ����������� ����
#endif//#if SAVE_LINE 


	//
	Search::set_depth_max_rem(depth_max);

#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (Search::get_stop_search() == true)
	{
		return 0;
	}

#endif//#if SAVE_LINE 

	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	Move_generation::generator_captures_white(list_surplus_moves,position);
	Move_generation::generator_move_white(list_surplus_moves,position);
	Move_ordering::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Move_ordering::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY

	//Sorting_white_moves_root(&root_list_surplus_moves);// ��������� ����

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_LIST && SAVE_LINE)
	list_print(depth_max,list_surplus_moves);
	History_heuristic::test_print(depth_max);
	//if(depth_max == 4) List_Print(depth,list_surplus_moves);
	//Bitboard_print(bitboard);
#endif//#if TEST_LIST
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------
#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	Transposition_table::look_hash_move(list_surplus_moves,position.hash_key
						,depth,flag_insert_hash_move);

// ��� ������� ����� ����� ��������� ������� �� ������
#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	//std::cout << "���� ������ ����� flag_insert_hash_move " << flag_insert_hash_move << std::endl;

	if (flag_insert_hash_move == 0)
	{
		std::cout << "���� �� ����� ������ �����++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;   
	}
	
	if (flag_insert_hash_move == 1)
	{
		std::cout << "�������� ���� ������ �����++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
		exit(1); 
	}
#endif//#if HASH_TEST

#endif//#if HASH
//-----------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
//#if (TEST_LIST && SAVE_LINE)
	//List_Print(depth,&root_list_surplus_moves);
	////if(depth_max == 4) List_Print(depth,&root_list_surplus_moves);
	////Bitboard_print(bitboard);
//#endif//#if TEST_LIST
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


#if SAVE_LINE

	// TEST
	// ���������� ����� ��� ���������� �������� �� ������������
	// ����� �������� � ������� 
	Test_chess_bitboard::save_test_bitboard(position_save,position);

#endif//#if SAVE_LINE 


	//����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		Make_move::do_moves_white(position,i,depth,list_surplus_moves);
		//Bitboard_print(bitboard);

		// ���� ������ ��� ����� �� (�������� ����� ���������� ��� �������� ������)
		if (Move_generation::king_white_check(position) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			Make_move::undo_moves_white(position,i,depth,list_surplus_moves);

			// ���������� ������ � �������� ������ �����
			//list_surplus_moves.sorting_score[i] = -1000000;

			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes_root = nodes_root + 1;

		new_depth_max = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
		if (Move_generation::king_black_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES; 
			new_depth_max = depth_max + 1;
		}
		else
		{
			flag_check = CHECK_NO;
		}
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

		// ��� ����: 0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
		// 4 � �������� ��������� 5 � ������ �� ������� � �.�.
		const __int32 description_move = ( list_surplus_moves.move[i] >> 6 ) & 63;

#if SAVE_LINE

		if(depth_max > 9)
		{
			// �������� ���������� � ��������������� ���� � ������� ��������
			Uci_engine_to_gui::print_currmove
				(list_surplus_moves,nodes_0,i,depth_max);
		}

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� 
		// �� ������� �������
		PV_save::update_PV_current(i,depth,list_surplus_moves);

		// ��������� ��� ����
		Zobrist_hashing::modification_random_key(position,1,i,list_surplus_moves
										,Make_move::get_d_undo(depth));

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		Zobrist_hashing::save_hash_three(position.hash_key,new_depth);

#endif//#if SAVE_LINE 

//-------------------------------------------------------------

		if (PV_YES && (i == 0))
		{
			flag_do_pv_search = true;
			flag_do_lmr_search = false;
			flag_do_null_window_search = false;
		}
		else
		{
			flag_do_pv_search = false;
			flag_do_null_window_search = true;
		}

// ���� ������ ����� �� ������� ����� ������� ����� �� �� ������������ lmr-�������
		if (!flag_do_pv_search)
		{
			// ������� ����� �� ������������ lmr
			flag_do_lmr_search =
				(i != 0)
				&&(description_move == MOVE_IS_SIMPLE)// ������� ���
				&& (flag_check == CHECK_NO)
				&& ((new_depth_max - new_depth) > LMR_REDUCTION_D )
				;
		}

#if (LMR && SAVE_LINE)
		// ������ ���������  lmr-�������
		if (flag_do_lmr_search)
		{
			// ���� � ��������� ������� � � ������� �����
			beta_null_window = alpha + 1;
			flag_pv_move = PV_NO;
			new_lmr_depth_max = new_depth_max - LMR_REDUCTION_D;

			//
			value = Search::black_searching_min_value(position,alpha,beta_null_window,
								new_depth,new_lmr_depth_max,flag_pv_move,
								description_move,flag_check,NULL_NO);
				
			// ���� ������ ���������� ������� �� ���� ����� �� ������� � ������� �����
			// ����� �� ���� �����
			flag_do_null_window_search = (value > alpha);
		}
#else
		do_null_window_search = true;
#endif//#if  LMR

#if (ALPHA_BETA_ON && SAVE_LINE)
		// ������ ��������� ������� ������ � ������� �����
		if (flag_do_null_window_search)
		{
			beta_null_window = alpha + 1;
			flag_pv_move = PV_NO;

			//
			value = Search::black_searching_min_value(position,alpha,beta_null_window,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,NULL_NO);

			// ��� �������� �������� ������������� � ������ ����� � ��������
			// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
			// ����� ������� ������� ��� � ����������� ����� �.�. ������� �����
			// ���� ��� ���� ������� �� ������������� c ������ �����
			// ����� �� ����� ��������� ������ �������� ��������
			flag_do_pv_search = PV_YES && (value > alpha); 
		}
#else
		do_pv_search = true;
#endif//#if ALPHA_BETA_ON

		// ������� ��������� ������� ������ � ������ ����� � ��������
		if (flag_do_pv_search)
		{
			// ���� �� � ������� �������� ��
			// ������ ��� ������� �� ������ ������� � � ������ �����
			// �� ������������ ��� ���� �� ������ �� ������ � ������� ������� 
			// ���������
			// ����� ���� ������ ��� ��� ��� �� ���-�������
			flag_pv_move = PV_YES;

			value = Search::black_searching_min_value(position,alpha,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,NULL_NO);
		}
//-------------------------------------------------------------

		// ��������������� �������
		Make_move::undo_moves_white(position,i,depth,list_surplus_moves);

		// ���������� ������ � �������� ������ �����
		//list_surplus_moves.sorting_score[i] = value;


#if SAVE_LINE

		// ��������������� ���-���� �����
		position.hash_key = key_undo;

		// ���� ������ �������. �� ����� ���� ��������
		if (value > value_max)
		{
			value_max = value;
			i_hash_move = i;

			// TEST
			from = ((list_surplus_moves.move[i]>>12)& 63);
			to = ((list_surplus_moves.move[i]>>18)& 63);

			// TEST		 
			//-----------------------------------------------
			//std::cout << "�������� ��������. ��� ���������� � ��� " << std::endl;
			//std::cout << "depth= " << depth << std::endl;
			//std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
			//std::cout << "((root_list_surplus_moves.move[i]>>6)& 63)= " 
			//	<< ((root_list_surplus_moves.move[i]>>6)& 63) << std::endl;
			//std::cout << "������� ������ " << util_o.public_int_to_char_board(((root_list_surplus_moves.move[i]>>24)& 7),1)<< std::endl;
           
			//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>12)& 63)%8);
			//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>12)& 63)/8);
			//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>18)& 63)%8);
			//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>18)& 63)/8) << std::endl;
			//std::cout << "  " << std::endl; 
			//-----------------------------------------------



//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// ����� �������� � ������ � �������� �� ��������
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			if (value > alpha)
			{ 
				alpha = value;       
				flag_save_best = true;

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				PV_save::update_PV_best_point(pv_best_point);

				// TEST
				// ��������� ���������� �������� � ������
				if (value != PV_save::get_pv_best().score_move)
				{
					std::cout << "ERROR white value (search_root.cpp)" ;
					std::cout << std::endl;
				}

				// TEST
				//-----------------------------------------------
				//std::cout << "�������� �����. ��� ���������� � ��� " << std::endl;
				//std::cout << "depth= " << depth << std::endl;
				//std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
				//std::cout << "((root_list_surplus_moves.move[i]>>6)& 63)= " 
				//	<< ((root_list_surplus_moves.move[i]>>6)& 63) << std::endl;
				//std::cout << "������� ������ " << util_o.public_int_to_char_board(((root_list_surplus_moves.move[i]>>24)& 7),1)<< std::endl;
               
				//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>12)& 63)%8);
				//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>12)& 63)/8);
				//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>18)& 63)%8);
				//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>18)& 63)/8) << std::endl;
				//std::cout << "  " << std::endl;     
				//-----------------------------------------------

			}
#else///#if ALPHA_BETA_ON

			flag_save_best = true;

			// ����� �� �������������� ������ ������� ��������������� ������ ������
			PV_save::update_PV_best_point(pv_best_point);

			// TEST
			// ��������� ���������� �������� � ������
			if (value != PV_save::get_pv_best().score_move)
			{
				std::cout << "ERROR white value (search_root.cpp)" ;
				std::cout << std::endl;
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� 
		// �.�. � ����� �����������
		if (Search::get_stop_search() != true)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// ��� ����� ���������
			Uci_engine_to_gui::print_pv_line_s(1,list_surplus_moves,PV_save::get_pv_best()
								  ,nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			//Uci_engine_to_gui::print_pv_line_s(1,list_surplus_moves,
				//pv_best_point,nodes_root,i,depth_max,value_max,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 

	}


#if SAVE_LINE
	// TEST
	// ��������� �������������� �� ����� ����� ����������� �������
	Test_chess_bitboard::compare_test_bitboard(position_save,position);
#endif//#if SAVE_LINE 

	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == false)
	{
		value = 0;
		if (Move_generation::king_white_check(position) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		PV_save::update_PV_best_max_depth(value,depth);

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� 
		// ������� �������
		PV_save::update_PV_best(pv_best_point);
	}

#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	// (value > alpha)
	Transposition_table::save_best_hash_score_or_move(i_hash_move, position.hash_key
									  ,list_surplus_moves, 
									  value_max, ALPHA_UPDATED, depth, depth_max);

#endif//#if HASH


	return value_max;

}

/*
 ������� �� ������� ������ �� ������ ���� �������
 ������� �� �� ������ ���������� � ��������. ��������� 
 ������ ��� ������ �� ������� ������
*/
__int32 Search_root::black_root_searching_min_value
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	__int32 beta,// 
	const unsigned __int8 depth_max// ������������ �������
)
{
	__int32 value;              // ������� ������ �������

	__int32 value_min = INFINITE_SCORE;// ������ � ����  ������ �������  
	bool flag_save_best = false;// ����� �� ������ �������
	bool flag_is_legal_moves = false;// ���� ������������� ��������� ����� (���� 0 �� ��������� ����� �� ����)
	bool flag_check = false;//���� ����

	const unsigned __int8 depth = 0;// �������
	const unsigned __int8 new_depth = 1; // ��������� �������
	unsigned __int8 new_depth_max;//����� ������� ��������

	bool flag_do_pv_search;
	bool flag_do_null_window_search;
	bool flag_do_lmr_search;

	// ���������� ��������� � ������ LMR � PVS
	__int32 alpha_null_window;//
	unsigned __int8 new_lmr_depth_max;// ������������ �������
	bool flag_pv_move;// �������� �� ��� ������� (pv)

	struct List list_surplus_moves; // ������ ��������� ����� (������������ ���� �� ������� ����)


#if SAVE_LINE
	struct Position position_save;// ������������� �����
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	unsigned __int8  flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = position.hash_key;   // ���������� ���-���� �������� ����
	unsigned __int8 i_hash_move = 0; // ����� ����������� ����
#endif//#if SAVE_LINE 


	Search::set_depth_max_rem(depth_max);

#if SAVE_LINE

	// ��� ������� ��� ���������� ������
	if (Search::get_stop_search() == true)
	{
		return 0;
	}

#endif//#if SAVE_LINE 

	//Move_generation::mobility_evaluation(position);

	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	Move_generation::generator_captures_black(list_surplus_moves,position);
	Move_generation::generator_move_black(list_surplus_moves,position);
	Move_ordering::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Move_ordering::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY

	//Sorting_black_moves_root(&root_list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_LIST && SAVE_LINE)
	list_print(depth_max,list_surplus_moves);
	History_heuristic::test_print(depth_max);
	//if (depth_max == 3) 
		//List_Print(depth,list_surplus_moves);

	//	Bitboard_print(bitboard);
#endif//#if TEST_LIST
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	Transposition_table::look_hash_move(list_surplus_moves,position.hash_key
						,depth,flag_insert_hash_move);

#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	if (flag_insert_hash_move == 0)
	{
		std::cout << "���� �� ����� ������ ������+++++++++++++++++++++++++++++++++" << std::endl;   
	}

	if (flag_insert_hash_move == 1) 
	{
		std::cout << "�������� ���� ������ ������+++++++++++++++++++++++++++++++++" << std::endl;
		exit(1); 
	}

#endif//#if HASH_TEST
#endif//#if HASH


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//�������� ������ ������ � ����� �� ������� ������
//#if (TEST_LIST && SAVE_LINE)
	//if (depth_max == 3)
	//{
		//List_Print(depth,&root_list_surplus_moves);
	//}

	////Bitboard_print(bitboard);
//#endif//#if TEST_LIST 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



#if SAVE_LINE

	// TEST
	//���������� ����� ��� ���������� �������� �� ������������
	Test_chess_bitboard::save_test_bitboard(position_save,position);

#endif//#if SAVE_LINE 


	// ����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		Make_move::do_moves_black(position,i,depth,list_surplus_moves);
		//Bitboard_print(bitboard);

		// ���� ������ ��� ����� ��
		if (Move_generation::king_black_check(position) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			Make_move::undo_moves_black(position,i,depth,list_surplus_moves);

			// ���������� ������ � �������� ������ �����
			//list_surplus_moves.sorting_score[i] = 1000000;

			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes_root = nodes_root + 1;

		new_depth_max = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
		if(Move_generation::king_white_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES;
			new_depth_max = depth_max + 1;
		}
		else
		{
			flag_check = CHECK_NO;
		}
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

		const __int32 description_move = ( list_surplus_moves.move[i] >> 6 ) & 63;

#if SAVE_LINE

		if(depth_max > 9)
		{
			// �������� ���������� � ��������������� ���� � ������� ��������
			Uci_engine_to_gui::print_currmove
				(list_surplus_moves,nodes_0,i,depth_max);
		}

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
		PV_save::update_PV_current(i,depth,list_surplus_moves);

		// ��������� ��� ����
		Zobrist_hashing::modification_random_key(position,0,i,list_surplus_moves
										,Make_move::get_d_undo(depth));

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		Zobrist_hashing::save_hash_three(position.hash_key,new_depth);

#endif//#if SAVE_LINE 

//-------------------------------------------------------------

// ���� �� � ������� ����(pv) �� ������ ��� ���� ������ �������
		if (PV_YES && (i == 0))
		{
			flag_do_pv_search = true;
			flag_do_lmr_search = false;
			flag_do_null_window_search = false;
		}
		else
		{
			flag_do_pv_search = false;
			flag_do_null_window_search = true;
		}

// ���� ������ ����� �� ������� ����� ������� ����� �� �� ������������ lmr-�������
		if (!flag_do_pv_search)
		{
			// ������� ����� �� ������������ lmr
			flag_do_lmr_search =
				(i != 0)
				&&(description_move == MOVE_IS_SIMPLE)// ������� ���
				&& (flag_check == CHECK_NO)
				&& ((new_depth_max - new_depth) > LMR_REDUCTION_D )
				;
		}

#if (LMR && SAVE_LINE)
		// ������ ���������  lmr-�������
		if (flag_do_lmr_search)
		{
			alpha_null_window = beta - 1;//
			flag_pv_move = PV_NO;//
			new_lmr_depth_max = new_depth_max - LMR_REDUCTION_D;// ������������ �������

			value = Search::white_searching_max_value(position,alpha_null_window,beta,
								new_depth,new_lmr_depth_max,flag_pv_move,
								description_move,flag_check,NULL_NO);

			// ���� ������ ���������� ������� �� ���� ����� �� ������� � ������� �����
			// ����� �� ���� �����
			flag_do_null_window_search = (value < beta);
		}
#else
		do_null_window_search = true;
#endif//#if  LMR

#if (ALPHA_BETA_ON && SAVE_LINE)
		// ������ ��������� ������� ������ � ������� �����
		if (flag_do_null_window_search)
		{
			alpha_null_window = beta - 1;//
			flag_pv_move = PV_NO;//

			value = Search::white_searching_max_value(position,alpha_null_window,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,NULL_NO);

			// ���� ������ ���������� ������� � �� � ������� ����
			// �� ���� ����� �� ������� c ������ ����� � ��������
			// ����� �� ���� �����
			flag_do_pv_search = PV_YES && (value < beta);
		}
#else
		do_pv_search = true;
#endif//#if ALPHA_BETA_ON

		// ������� ��������� ������� ������ � ������ ����� � ��������
		if (flag_do_pv_search)
		{
			flag_pv_move = PV_YES;// �������� �� ��� ������� (pv)

			value = Search::white_searching_max_value(position,alpha,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,NULL_NO);
		}
//-------------------------------------------------------------

		//��������������� �������
		Make_move::undo_moves_black(position,i,depth,list_surplus_moves);


		// ���������� ������ � �������� ������ �����
		//list_surplus_moves.sorting_score[i] = value;


#if SAVE_LINE

		// ��������������� ���-���� �����
		position.hash_key = key_undo;

		//���� ������ �������. �� ������ ���� �������
		if (value < value_min)
		{
			value_min = value;
			i_hash_move = i; // ����� ����������� ����

			// TEST
			from = ((list_surplus_moves.move[i] >> 12) & 63);
			to = ((list_surplus_moves.move[i] >> 18) & 63);
  
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			// ������ �������� �������� ����
			if (value < beta)
			{
				beta = value;
				flag_save_best = true;

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				PV_save::update_PV_best_point(pv_best_point);

				// TEST
				// ��������� ���������� �������� � ������
				if (value != PV_save::get_pv_best().score_move)
				{
					std::cout << "ERROR black value (search_root.cpp)" ;
					std::cout << std::endl;
				}
			}
#else//#if ALPHA_BETA_ON

			flag_save_best = 1;

			// ����� �� �������������� ������ ������� ��������������� ������ ������
			PV_save::update_PV_best_point(pv_best_point);

			// TEST
			// ��������� ���������� �������� � ������
			if (value != PV_save::get_pv_best().score_move)
			{
				std::cout << "ERROR black value (search_root.cpp)" ;
				std::cout << std::endl;
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� 
		// �.�. � ����� �����������
		if (Search::get_stop_search() != 1)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// ��� ����� ���������
			Uci_engine_to_gui::print_pv_line_s(0,list_surplus_moves,
				PV_save::get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			//Uci_engine_to_gui::print_pv_line_s(0,list_surplus_moves,
				//pv_best_point,nodes_root,i,depth_max,value_min,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 

	}

#if SAVE_LINE
	// TEST
	// ��������� �������������� �� ����� ����� ����������� �������
	Test_chess_bitboard::compare_test_bitboard(position_save,position);
#endif//#if SAVE_LINE 

	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == false)
	{
		value = 0;
		if (Move_generation::king_black_check(position) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);// ����� ��� ������� ������� �����?
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		PV_save::update_PV_best_max_depth(value,depth);

		return value;
	}


#if SAVE_LINE
	if (flag_save_best == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� 
		// ����� ������� �������
		PV_save::update_PV_best(pv_best_point);
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	//(value < beta)
	Transposition_table::save_best_hash_score_or_move(i_hash_move,position.hash_key
									  ,list_surplus_moves, 
									   value_min, BETA_UPDATED, depth, depth_max);
#endif//#if HASH

	return value_min;
}


/*
 ���������� ���������� ������������� �����
*/
__int64 Search_root::get_nodes()
{
	return (nodes_root + Search::get_nodes());// ���������� �����;
}

/*
 �������� ���������� ������������� �����
*/
void Search_root::set_nodes_in_0()
{
	nodes_root = 0;// ���������� �����;
	Search::set_nodes_in_0();
}

/*
 ������ �������� ���
*/
unsigned __int8 Search_root::get_from()
{
	return from;
}

/*
 ���� �������� ���
*/
unsigned __int8 Search_root::get_to()
{
	return to;
}

/*
 TEST
 �������� ������
 ������������ �������, ������� ��������, �����
*/
void Search_root::list_print
(
	int depth,// ������� �������
	struct List & list_surplus_moves// ������ ��������� �����
)
{
#if TEST_LIST
	Test_Loop.open ("_TEST_LOOP.txt",std::ios::app);	
	//Test_Loop.open ("_TEST_LOOP.txt");	
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop << std::endl;
	Test_Loop << std::endl;
	Test_Loop << std::endl;
	Test_Loop << "------------------------------------------------- "<<std::endl;
	Test_Loop <<"depth= "<< depth << std::endl;
	Test_Loop << std::endl;
	Test_Loop << "������ ��������� ����� ������ "<<std::endl;

	for (int i = 0 ; i < list_surplus_moves.end_list ; i++)
	{	
		Test_Loop << "����� ���� �� ������ i= " << i << std::endl;  
		Test_Loop << "������� ������ " << PV_save::int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< std::endl;
		Test_Loop << "������ ���� " << list_surplus_moves.sorting_score[i] << std::endl;

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Loop << " " << (PV_save::x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.move[i]>>12)& 63)/8);//<< std::endl;
		//Test_Loop<< std::endl;
		Test_Loop << " " << (PV_save::x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.move[i]>>18)& 63)/8)<< std::endl;   
		//------------------------------
		//��� ���� 
		//0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
		//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
		//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
		//25 - ������ � ������������ � ����� 
		Test_Loop << "��� ���� " << ((list_surplus_moves.move[i]>>6)& 63)  << std::endl;
		Test_Loop << "������ ������ " << PV_save::int_to_char_board(( list_surplus_moves.move[i] & 63),1)<< std::endl;
		Test_Loop << "��������� ��������� " << ((list_surplus_moves.move[i]>>12)& 63)  << std::endl;
		Test_Loop << "�������� ��������� " << ((list_surplus_moves.move[i]>>18)& 63)  << std::endl;
     
		Test_Loop << std::endl;
	}

	Test_Loop << "list_surplus_moves.end_captures= " <<int(list_surplus_moves.end_captures) << std::endl;
	Test_Loop << "list_surplus_moves.end_list= " <<int(list_surplus_moves.end_list) << std::endl;
	Test_Loop << "------------------------------------------------- " << std::endl;
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop.close();
#endif//#if TEST_LIST 
}