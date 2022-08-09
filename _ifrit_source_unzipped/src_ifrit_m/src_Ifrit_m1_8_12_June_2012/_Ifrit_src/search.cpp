
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 2.11.2009 20.12.2009 25.07.2010 17.10.2010

#include <fstream>
#include <iostream>

#include "_search_switch.h"
#include "a_move_list_struct.h"
#include "b_search_const.h"
#include "search.h"
#include "search_quiescence.h"
#include "evaluate.h"
#include "move_generation.h"
#include "make_move.h"
#include "move_ordering.h"
#include "killer_heuristic.h"
#include "history_heuristic.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "b_constants.h"
#include "time_management.h"
#include "win_32.h"
#include "pv_save.h"
#include "b_move_generation_const.h"
#include "uci_engine_to_gui.h"

	unsigned __int16 audit;/// �������� ������ ������� ����������� ������
	__int8 string_in2[65536];///

	unsigned __int8 depth_max_rem;///
	bool flag_stop_search;/// ���������� ��� ������� �������� ��� ���������� ������
	__int64 nodes;/// ���������� �����

	//int return_hash_score;

	double deep_time_terminate; /// �������� �� ������� ����� ��������� ����� 


/*
 ���� �������� � ����. ������� �� ����� 

1.������� �� �������� ������� stop ��� quit. �������, �� 
   ���� �� ������� stop ��� quit. ���� ������� ����, �� ������� 
   �� ������ � ������� �������. ����� ��������� ������ ��� ���� 
   �������� �� �� ���������.

2.������� �� ���������� �������. ���� ������� �����������, 
   �� ������� �� ������ � ������� �����. ������ ������� 
   ����������� ����� ������ ��� ������ ������� �� ����� ��������. 

3.������ �� ������� ������������ (���-�������). �������� 
   ��������������� ����������� ����������� ������. ������� 
   � ���-�������. ���� ����� ������� ��� �����������, � 
   ��� ���� ����������� �� ����� �� ��� ������� ������� 
   ����� ����������� ��� ������. ���� ��� ������ ���������, 
   � ��� �������� �� ����� ���� ��������, �� ���������� ��. 
   ������ ����� ���� ���� ����� (�������� �� ������� �����). 
   ������� �� ����, �� ������  �������� �����, ������ ������, 
   �.�. �������� �����, �� �� ���������� �� ���� (��� ����� � ����� ���� ��������). 
   ������� �� ������ ��������� ��� ������. �� ���������� ������� � ������� ��������.

4.����������� �����. ���� �������� ������������ �������, �� �������� 
   ����������� �����. � ����������� ������ ������� ������ �������� 
   �������. �.�. ���������� ������� ������ � ������� ������ �������.

5.������� �� razoring. �������� �� ������� �����. ���� ����������� 
   ������ ������� ���� ����� ��� ����� ������ ���� �� ������� 
   ����������� �������. ����� ��� ���� ����� ��� ������. ����� 
   ����� ����� ���������� ���������, �� ��� ��� ����� ��� ���������� ����� 
   �� ��� ������� ��� ���. ���� �����, �� ���������� �����, � ���� ���, 
   �� ���������� ����������� ������. ������� �� ������ �� ��� ��������. 
   ������� ��� ��������� ��������� ������ � ��������� ��������. � ������ 
   ���������� �� ������������ ��������� ������ �������, ������ ��� �� �� 
   ������� ���� � ����������� ������.

6.������� �� futility. �������� �� ������� �����. ���� ����������� ������ 
   ������� ����� ����� ��� ����� ������ ��� ����� ���� �� ������ �� 
   �������. �� ������������, ���, ��� ��� � ��� ������� � ���� 
   ����� ��� ������ � ������ ��� ��� �� �� ������ ����� ����, 
   ������� ��� ������� ������� ��� �������. ������ ������������� �� 
   ����� ���, ��� ��� ������ ����� ���� ������������ ������ � 
   ������ ������� ������.

7.������� �� �������� ����. �� ���������� ���� ��� � ������ �� 
   ������ ������. ����� �������, ������ ������ ��� ���� ������. 
   ���� � ��� ����� ������ ������ �������� ������ ����, �� ����� 
   �� �������. �������� ���������� ������� ��  futility.

8.������ ���������� �����. ������� ���������, ������ �� ����������� 
   ������ ���������� �����. ������ ���������, ������ ��� � ��� 
   ���� ���� ��� ��� � ��� �����.
  ���������� �����. ������ �� ��������� �� ��������� 
  MVV-LVA (Most Valuable Victim - Least Valuable Aggressor). 
  ����� ���� ��������� �� ��������� history heuristic. 
  �������� (killer moves) �������� ����� ���� ������. ��� �� ������� 
  ������������ (transposition table) �������� � ����� ������.

9.������� ������ ���������� �����. 
 9.1 ������ ���. 
 9.2 ������ �����, ��������� ��� ����� �� ��� ������ ����� ����. 
      ���� �� �� �������� ��� ��� ������������ � ������� ���������.
 9.3 ���� ���� ���, �� ��� ���� ����� ����������� ������������ 
      ������� �������� �� �������.
 9.4 ������� ������� ��������� ����� Late Move Reductions (LMR). 
      �������� ���������� ��� ��������.
 9.5 ���������� ���.
 9.6 �������� �� ������� �����. ���������� ������ ���������� � 
      ��������� ����������. ���� ��� ������ �� �������� ������������ 
	  � ���. ����� ������ ���������� � ������. ���� ��� ������ �� 
	  ����� ������������ � ���. ����� ������ ���������� � �����. 
	  ���� ��� ������ ��� �����, �� �������� �����, ��������� ������ 
	  �������������� ������� ������ � ������� ������������ � ������� �������.

10.��� ���� �����������. ���� ��������� ����� �� ���� ������ ��� ������ 
    ���� ��� ����. ���� ����� ���, � ��� ������ ��� ����� ������ ��� ���. 
	���� ����� ��� � ���� ��� ���, ������ ��� ���. ���� ��������� ���� ���� 
	�� ��������� ������� ������������ � ������� �� ������ 
	��������� ��������� ��������.


�������� ������ Late Move Reductions (LMR).(�� ������� �����)

������ ����� �������� ������ � ������ ����� �������� ����. � ��� ��� 
�����, ������� ���������� �� ��� ����. ����� �������� ����� �� ������ 
������� � � ������ �����.

���� �� �� � ������ ����� �������� ���� ����� �������, ����� 
�� �� ��������� ��� �����.
���� ��� ����, ���� ��� �� �� ������� ������������, ���� ��� �� ������, 
���� ��� �������, �.�. �� ������, ��������� � �.�. ���� ������� �������  
��� ��������, ����� �������� ����� � ����������� ������� �� ���� ������� 
� � ������� �����. 
���� ���������� ������ ������ ����� ����� �������� ��������� ����� �� 
������ �������, �� ��-�������� � ������� �����.
���� ���������� ������ ������ ����� � �� � ������� ���� �� �������� ����� 
�� ������ ������� � � ������ �����. �� ����� ����� ������� �����.

���� �� ����� �� �������� ����������� ����� ��������� ������, �� �������� ����� 
�� ������ ������� � ������� �����. ���� ���������� ������ ������ ����� � �� � 
������� ����, �� �������� ����� �� ������ ������� � � ������ �����. �� ����� ����� 
������� �����.

*/
__int32 Search::white_searching_max_value
(
	struct Position & position,// ������������� �����
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth,// �������
	const unsigned __int8 depth_max,// ������������ �������
	const bool flag_pv_node,// �������� �� ��� ������� (pv)

	const __int32 old_description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_old_check,//���� ����
	const bool flag_null_m// ���� �������� ����
)
{
	struct List list_surplus_moves;// ������ ��������� ����� 
								   //(������������ ���� �� ������� ����)

	__int32 value;// ������� ������ �������
	__int32 value_max = -INFINITE_SCORE;// ������ ������� ������ � ���� 
	const unsigned __int8 new_depth = depth + 1;// ��������� �������
	unsigned __int8 new_depth_max;//����� ������� ��������

	// ���������� ��������� � ������ LMR � PVS
	__int32 beta_null_window;//
	unsigned __int8 new_lmr_depth_max;// ������������ �������
	bool flag_pv_move;// �������� �� ��� ������� (pv)

	bool flag_do_pv_search;
	bool flag_do_null_window_search;
	bool flag_do_lmr_search;

	bool flag_is_legal_moves = false;// ���� ������������� ��������� �����
	bool flag_alpha_updated = false;// ����� �� ������ �������
	bool flag_check = false;//���� ����


//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
 
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������

	  
	// ��� ��� �����������
	const unsigned __int64 key_undo = position.hash_key;// ���������� ���-���� �������� ����
	unsigned __int8  flag_insert_hash_move = 0;// ��������� �� ��� �� ���-������� �� ������ �������
	__int32 hash_score = -1000000000;// ���-������ �������
	unsigned __int8 i_hash_moves_1;// ��� ������� �������� ����� (1- �������� �����) ����� ��� ����������� 
	unsigned __int8 i_hash_moves_3;// ��� ������ ��������� �������� (3-������ ��������� �������� )����� ��� �����������


	//unsigned __int64 test_key_control;
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 
//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

#if HASH_TEST_SCORE
	unsigned __int8 hash_moves_test13;// ��� �����
#endif// #if HASH_TEST_SCORE

#if (HASH_SCORE && SAVE_LINE)
	// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 
	// 2-������� �� ����(�����))
	unsigned __int8 test_flag_hash_moves;
	unsigned __int8 hash_depth;// ������������ �������
	unsigned __int8 hash_depth_max;// ������������ �������
#endif//#if (HASH_SCORE)


//----------------------------------------------------------
// 1 ������� �� ������� �� ��������� �����

#if SAVE_LINE

	// ��� ������� ��� ���������� ������
	if (flag_stop_search == true) return DRAWN_SCORE;
	// 
	quit_or_stop();

#endif//#if SAVE_LINE 

//----------------------------------------------------------
// 2 ������� �� ���������� �������

#if (REPETITION && ALPHA_BETA_ON && SAVE_LINE)
	// ���������� ���������� �������
	if (Zobrist_hashing::analysis_hash_string_moves(depth))
	{
		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ 
		// ���� �������
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(DRAWN_SCORE // ������ �����
													,depth);
		}

		return 0;
	}
#endif//#if REPETITION

//----------------------------------------------------------
// 3 ������� �� ���-������

#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// ������� �� ��� ������ �� ���������� ��� �������� ��������
	// ������� ������� ��� ������������� ������ � ����� �������� � ��� ������� �������
	// ������ ���������� ������� ������� ������������.
	if (flag_pv_node == PV_NO)
	{ 
		// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� 
		// �������� ��������� ����������� ������
		hash_score = Transposition_table::look_hash_score_white(
			position.hash_key,depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (Transposition_table::get_flag_found_score())
		{
			//return_hash_score = return_hash_score + 1 ;
#if (!HASH_TEST_SCORE)
			return hash_score;
#endif//#if (!HASH_TEST_SCORE)

		}
	}

#endif//#if HASH
#endif//#if (HASH_SCORE)

//----------------------------------------------------------
// 4 ������� �� ������������ ������� (������ �������)

	// �� ���������� ������� ���������� ����������� ������ �������
	if (depth >= depth_max)
	{
#if SAVE_LINE

	// ����������
	// ���������� ������������� �������
#if FORCED
		// �������� ����� �������� ������ ������
		value = Search_quiescence::forced_variant_white(position,alpha,beta
													,depth,flag_pv_node);
#endif//#if FORCED

	// �������� ������ ��� ����������
#if !FORCED 
		value = max_depth_white(position,alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

//----------------------------------------------------------
// 5 6 ������� �� razoring � futility

#if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING ))

/*
�� ������� ����� �� ���������� ������ �� beta
������������� ����� ���� ��� ��������
1. 
value = value_m - FUTILITY_MARGIN 
if (value > beta) return value;

����� ������ ��� ����� ��� ������ ��� ����� ������ �� �������
������ ��� ��� � ��� ����� �������� �� ������ ��������.

2.
value = value_m + FUTILITY_MARGIN 
if (value < beta) return value;

����������� ������ ������� ��� ����� ��������� ����� ��� ���� ����� �� �������
����� ������� ��������� ���� ������ �������� ��������. ����� ��� ��� ��� �����
��� ���������� ����� �� �� ������� �������� ��� ���.
� ����� ������ � ������ ���� �������� ������ ����� ������������ ����������� �������.

*/

	if (flag_pv_node == PV_NO)
	{ 
		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
		//&& (description_move_up != MOVE_IS_CAPTURE) // ���� �� ������
		&& (flag_old_check == CHECK_NO)) // ���� ���� ���

		{
			
				value = position.value_white_material 
					- position.value_black_material
					+ Evaluate::eval_state(position);

#if RAZORING
			// razoring
			// ��� ����������� �� ���� �������.
			// ���� ��� ��������� �������� ���� ��� ������. 
			// ������ ��� ��� ����� �� ����������� ������. 
			// � � ���� � ��� ���� ��� ��������� �����.

			//--
			__int32 beta_q_margin =  beta - RAZORING_MARGIN[(depth_max - depth)];

			if (value < beta_q_margin)
			{

				value = Search_quiescence::forced_variant_white(position,
							(beta_q_margin - 1),beta_q_margin,depth,flag_pv_node);

				if ( value < beta_q_margin )
				{
					return value; 
				}
			}
			//--
#endif// #if RAZORING


#if FUTILITY
			//futility
			//--
			if (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
			{
				__int32 beta_margin =  beta + FUTILITY_MARGIN[(depth_max - depth)];

				if (value >= beta_margin)
				{
					return value;  
				}
			}
			//--
#endif// #if FUTILITY

		}
	}

#endif// #if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING ))

//----------------------------------------------------------
// 7 ������� �� �������� ����
/*

*/
#if (NULL_MOVE && SAVE_LINE)
	// ����� ����������� ��������� ��������� ��� ������ �������� ����
	// � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
	// ���� ��� � ��� ���
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	value = white_null_move(position,alpha,beta,depth_max,depth,old_description_move,
							flag_pv_node,flag_old_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE


//----------------------------------------------------------
// 8 ���������� � ��������� ������ ���������� �����

	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	Move_generation::generator_captures_white(list_surplus_moves,position);
	Move_generation::generator_move_white(list_surplus_moves,position);
	Move_ordering::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Move_ordering::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY
 
#if (KILLER && SAVE_LINE)

	Killer_heuristic::insert_killer
		(list_surplus_moves,depth,list_surplus_moves.end_captures);

#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� 
	// ������� ������
	// ������ �� ��������� ��� �� ��� ������� �� ������ ����� ������� ��������� ����
	Transposition_table::look_hash_move(list_surplus_moves,position.hash_key,depth
						,flag_insert_hash_move);

// ��� ������� ����� ����� ��������� ������� �� ������
#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	if (flag_insert_hash_move == 1)
	{
		std::cout << "�������� ���� �����" << std::endl;
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH

/*
 ����    Internal iterative deepening
*/

//----------------------------------------------------------
// 9 ����� �� ������ ���������� �����

	// ����� �� ������ ���������� �����
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)

			if (( !flag_pv_f )
			&& (description_move_up  == MOVE_IS_SIMPLE)//
			&& (i > 3)//
			&& (old_flag_check == CHECK_NO))
			{
				// ������� ���������� �� �������� �����
				if((depth_max - depth) < 11)
				{
					if ( i >= futility_move_count[(depth_max - depth)]) continue;
				}
			}

#endif//#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)

		// ��������� ��� �� ������ ������� ������� ���������� �������
		Make_move::do_moves_white(position,i,depth,list_surplus_moves);

		// ���� ������ ��� ����� �� 
		if (Move_generation::king_white_check(position) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			Make_move::undo_moves_white(position,i,depth,list_surplus_moves);

            // ���� ������ �� ������ 
			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes = nodes + 1;

		// ��� ������� ������ ���� ��������������� �������
		// ���-�� ��������� �������� ������ ��� ���������� ����
		new_depth_max = depth_max;

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

		// ��� ����: 0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
		// 4 � �������� ��������� 5 � ������ �� ������� � �.�.
		const __int32 description_move = ( list_surplus_moves.move[i] >> 6 ) & 63;

		// ������� ������ 0 - ��� ������ 1 - ����� 2 - ����  3 - ����
		// 4 - ����� 5 - ����� 6 - ������
		//const __int32 piece_move = ( list_surplus_moves.move[i] >> 24 ) & 7;

#if SAVE_LINE


		// �� ������ ��������� ����� �������� ������� ��� � 
		// ������� ������� �� ������� �������.
		if (flag_pv_node)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		Zobrist_hashing::modification_random_key(position,1,i,list_surplus_moves
										,Make_move::get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = position.hash_key;

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		Zobrist_hashing::start_position_random_key(position);

		if (test_key_control != position.hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << std::endl;
			std::cout << "collision white depth = " << depth << std::endl;
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		Zobrist_hashing::save_hash_three(position.hash_key,new_depth); 


#endif//#if SAVE_LINE 

/*
�� �����.
*/
//-------------------------------------------------------------

		if (flag_pv_node && (i == 0))
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
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_1(depth))
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_2(depth))
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
			value = black_searching_min_value(position,alpha,beta_null_window,
								new_depth,new_lmr_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);
				
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
			value = black_searching_min_value(position,alpha,beta_null_window,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);

			// ��� �������� �������� ������������� � ������ ����� � ��������
			// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
			// ����� ������� ������� ��� � ����������� ����� �.�. ������� �����
			// ���� ��� ���� ������� �� ������������� c ������ �����
			// ����� �� ����� ��������� ������ �������� ��������
			flag_do_pv_search = flag_pv_node && (value > alpha); 
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

			value = black_searching_min_value(position,alpha,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);
		}
//-------------------------------------------------------------

		// ��������������� �������
		Make_move::undo_moves_white(position,i,depth,list_surplus_moves);


#if SAVE_LINE
		// ��������������� ���-���� �����
		position.hash_key = key_undo;

		//--------------------------------------------
		if (value > value_max)
		{
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if ALPHA_BETA_ON
			if (value > alpha)
			{					
				if (value >= beta)
				{        
					// ����� ������� ���� ��� �� ������
					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
#if KILLER
						Killer_heuristic::save_killer(i,list_surplus_moves,depth);
#endif//#if KILLER
#if SW_HISTORY
						History_heuristic::white_history_good_save(i,
							list_surplus_moves,depth,depth_max);
#endif//#if SW_HISTORY
					}


#if HASH
					// (value >= beta)  (value > alpha) 
					Transposition_table::save_best_hash_score_or_move(i
						, position.hash_key, list_surplus_moves, 
						value, BETA_CUTOFF, depth, depth_max);
#endif//#if HASH

					// TEST
					//if (( Transposition_table::get_flag_found_score() ) && ( test_flag_hash_moves == 3 ))
					//{
						//std::cout << "��������� �� ��� ������� ����� " << std::endl;
					//}

					return value; 
				}

// TEST------------
#if HASH_TEST_SCORE
				// ������� ��� �� ���� ���� ������� ������ ���� ���������� 
				//�� ��� ������ :)
				// ���� �� ������ ���������� �� ��� �� ����� ��� �� ������ 
				//������� �� ����� �� ����
				// ��������� �� �����
				// ���� � ��� ��� �� ����� ������ �� ���-������� � ������� ��� 
				//��� ������ ���� �.�. �������
				// ���������. ������ �� ������������ �� � ���� ������ 
				//�������������� �� �� ����� �����
				// ���� �� ������ ��� ��� ��������� ������
				if (( Transposition_table::get_flag_found_score()) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "������ ��������� �� ��� ������� ����� " << std::endl;
					std::cout << "depth= " << depth << std::endl;
					std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
					std::cout << "hash_depth= " << (__int32)hash_depth << std::endl;
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max << std::endl;
					std::cout << "hash_score= " << hash_score  << std::endl;
					std::cout << "value= " << value << std::endl;
					std::cout << "alpha= " << alpha << std::endl;
					std::cout << "beta= " << beta << std::endl;
					std::cout << "  " << std::endl;

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-----------------


				alpha = value;
				flag_alpha_updated = true;
				i_hash_moves_1 = i; // ��� ������� �������� ����� (1- �������� �����) 
									// ����� ��� ����������� 

				// ����� �� �������������� ������ ������� ��������������� 
				// ������ ������
				if (flag_pv_node)
				{
					PV_save::update_PV_best_point(pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			if (flag_pv_f)
			{
				PV_save::update_PV_best_point(pv_best_point);
			}

			flag_save_best = true;
			i_hash_moves_1 = i;// ��� ������������ ������� ��������
#endif// #if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_max = value;
			i_hash_moves_3 = i; // ��� ������ ��������� �������� 
								//(3-������ ��������� ��������)����� ��� �����������

		}
#endif//#if SAVE_LINE 

	}


//----------------------------------------------------------
// 10 ��������� ������ ����� �� ���� ��������� �����
//   ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
//   ��� ������ ���� ��� ����

	if (flag_is_legal_moves == false)
	{
#if SAVE_LINE
		value = 0;
		if (Move_generation::king_white_check(position) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ 
		// ���� �������
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_alpha_updated == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� 
		// �� ����� ������� �������
		if (flag_pv_node)
		{
			PV_save::update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)

	//History_heuristic::white_history_bad_save(list_surplus_moves,depth,depth_max);

	if (flag_alpha_updated == true)
	{   // (value > alpha) (value < beta)
		Transposition_table::save_best_hash_score_or_move(i_hash_moves_1
			, position.hash_key, list_surplus_moves, 
			alpha, ALPHA_UPDATED, depth, depth_max);
	}
	else
	{  // (value <= alpha) (value < beta)
		Transposition_table::save_best_hash_score_or_move(i_hash_moves_3
			, position.hash_key, list_surplus_moves, 
			value_max, LOCAL_MAX, depth, depth_max);
	}

	//if (flag_alpha_updated == true)
	//{   // (value > alpha) (value < beta)
	//	Transposition_table::save_best_hash_move(i_hash_moves_1
	//		, position.hash_key, list_surplus_moves, 
	//		alpha, ALPHA_UPDATED, depth, depth_max);
	//}
	//else
	//{  // (value <= alpha) (value < beta)
	//	Transposition_table::save_best_hash_move(i_hash_moves_3
	//		, position.hash_key, list_surplus_moves, 
	//		value_max, LOCAL_MAX, depth, depth_max);
	//}

#endif//#if HASH

	// ��� ���� ������� ��� ���� ������� ��� ���������� ���� � ������
	// � ���� ���� � � ��������� � ����� ������ �� �� � ��� �������� ����
	// ��� ����� ���� ��� � ������ ����� ���� �������
	// ��� ��� ��� ������ ������ ������ �����
// TEST------------
#if HASH_TEST_SCORE
	// ������� ��� �� ���� ���� ������� ������ ���� ���������� �� ��� ������ :)
	/*
	������ �� ���-������� ������ ������ �����
	���� � �������� ������ ��� ����� ����� ������ ����� �� �� �� ���� �����
	*/
	if (Transposition_table::get_flag_found_score())
	//if ( 0 )
	{
		hash_moves_test13 = i_hash_moves_3;
		if (flag_save_best == 1) hash_moves_test13 = i_hash_moves_1;

		//if ( hash_moves_test13 != 0 )
		//if ( hash_score != value_max )
		if (value_max > alpha) 
		{
			std::cout << "������������ � ��������� ���� ������ �������� ������ ����� " << std::endl;
			std::cout << "bitboard.get_hash_key()= " << position.hash_key << std::endl;
			std::cout << "depth= " << depth << std::endl;
			std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
			std::cout << "hash_depth= " << (__int32)hash_depth << std::endl;
			std::cout << "hash_depth_max= " << (__int32)hash_depth_max << std::endl;
			std::cout << "test_flag_hash_moves= " << test_flag_hash_moves  << std::endl;
			std::cout << "hash_score= " << hash_score  << std::endl;
			std::cout << "value_max= " << value_max << std::endl;
			std::cout << "alpha= " << alpha << std::endl;
			std::cout << "beta= " << beta << std::endl;

			std::cout << "��� �� ��� ������� " << std::endl;
			std::cout << "((list_surplus_moves.move[0]>>6)& 63)= " 
				<< ((list_surplus_moves.move[0]>>6)& 63) << std::endl;

			std::cout << "������� ������ " 
				<< Uci_engine_to_gui::int_to_char_board(((list_surplus_moves.move[0]>>24)& 7),1)
				<< std::endl;
               
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[0]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>12)& 63)/8);
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[0]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>18)& 63)/8) << std::endl;

			std::cout << "��� ��������� � ������ " << std::endl;
			std::cout << "hash_moves_test12= " << hash_moves_test13  << std::endl;
			std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				<< ((list_surplus_moves.move[hash_moves_test13]>>6)& 63) << std::endl;
			std::cout << "������� ������ " 
				<< Uci_engine_to_gui::int_to_char_board(((
					list_surplus_moves.move[hash_moves_test13]>>24)& 7),1)<< std::endl;
               
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>12)& 63)/8);
			std::cout <<Uci_engine_to_gui::x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>18)& 63)/8) << std::endl;
			std::cout << "  " << std::endl; 

			exit(1);
		}
	}
#endif //#if HASH_TEST_SCORE
//-----------------

	return value_max;
}



/*
 ���� ������� � ����. ������� �� ������
*/
__int32 Search::black_searching_min_value
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth,// �������
	const unsigned __int8 depth_max,// ������������ �������
	const bool flag_pv_node,// �������� �� ��� ������� (pv)

	const __int32 old_description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_old_check,//���� ����
	const bool flag_null_m// ���� �������� ����
)
{
	struct List list_surplus_moves;// ������ ��������� ����� 
								   //(������������ ���� �� ������� ����)
	__int32 value;              // ������� ������ �������
	__int32 value_min = INFINITE_SCORE;   // ������ ������� ������ � ���� 
	const unsigned __int8 new_depth = depth + 1; // ��������� �������
	unsigned __int8 new_depth_max;//����� ������� ��������

	// ���������� ��������� � ������ LMR � PVS
	__int32 alpha_null_window;//
	unsigned __int8 new_lmr_depth_max;// ������������ �������
	bool flag_pv_move;// �������� �� ��� ������� (pv)

	bool flag_do_pv_search;
	bool flag_do_null_window_search;
	bool flag_do_lmr_search;

	bool flag_is_legal_moves = false;           // ���� ������������� ��������� �����
	bool flag_beta_updated = false;           // ����� �� ������ �������
	bool flag_check = false;//���� ����


#if SAVE_LINE
 
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	bool taking_passage_yes_mem = false;
	unsigned __int8 taking_passage_y_mem = 0;
	unsigned __int8 taking_passage_x_mem = 0;

	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = position.hash_key;   // ���������� ���-���� �������� ����
	__int32 hash_score = -1000000000;// ���-������ �������
	unsigned __int8 i_hash_moves_1;// ��� ������� �������� ���� (1- �������� ����) ����� ��� ����������� 
	unsigned __int8 i_hash_moves_3;// ��� ������ ��������� ������� (3-������ ��������� �������)����� ��� �����������

	//unsigned __int64 test_key_control;// ���������� ���-���� �������� ����

#endif//#if SAVE_LINE 

#if (HASH_SCORE && SAVE_LINE)
	// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 
	// 2-������� �� ����(�����))
	unsigned __int8 test_flag_hash_moves;
	unsigned __int8 hash_depth;// ������������ �������
	unsigned __int8 hash_depth_max;// ������������ �������
#endif//#if (HASH_SCORE)


//----------------------------------------------------------
// 1 ������� �� ������� �� ��������� �����
// ������ � �����

//----------------------------------------------------------
// 2 ������� �� ���������� �������
#if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
	// ���������� ���������� �������
	if (Zobrist_hashing::analysis_hash_string_moves(depth))
	{
		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(DRAWN_SCORE // ������ �����
														,depth);
		}

		return 0;
	}
#endif//#if ALPHA_BETA_ON

//----------------------------------------------------------
// 3 ������� �� ���-������
#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// ������� �� ��� ������ �� ���������� ��� �������� ��������
	// ������� ������� ��� ������������� ������ � ����� �������� � ��� ������� �������
	// ������ ���������� ������� ������� ������������.
	if (flag_pv_node == PV_NO)
	{ 
		// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� 
		// �������� ��������� ����������� ������
		hash_score = Transposition_table::look_hash_score_black(
			position.hash_key,depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (Transposition_table::get_flag_found_score())
		{
			//return_hash_score = return_hash_score + 1 ;
#if (!HASH_TEST_SCORE)
			return hash_score;
#endif//#if (!HASH_TEST_SCORE)
		}
	}
#endif//#if HASH
#endif//#if (HASH_SCORE)

//----------------------------------------------------------
// 4 ������� �� ������������ ������� (������ �������)
// �� ���������� ������� ���������� ����������� ������ �������
	if (depth >= depth_max)
	{
#if SAVE_LINE

	//  ����������
	//  ���������� ������������� �������
#if FORCED 
		// �������� ����� ����������(�������� ������ ������)
		value = Search_quiescence::forced_variant_black(position,alpha,beta
													,depth,flag_pv_node);
#endif//#if FORCED 


	// �������� ������ ��� ����������
#if !FORCED
		value = max_depth_black(position,alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

//----------------------------------------------------------
// 5 6 ������� �� razoring � futility
#if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING ))

	if (flag_pv_node == PV_NO)
	{ 

		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
		//&& (description_move_up != MOVE_IS_CAPTURE) // ���� �� ������
		&& (flag_old_check == CHECK_NO)) // ���� ���� ���
		{
			
				value = position.value_white_material 
					- position.value_black_material
					+ Evaluate::eval_state(position);
#if RAZORING
			//razoring 

			//--
			__int32 alpha_q_margin =  alpha + RAZORING_MARGIN[(depth_max - depth)];

				if(value > alpha_q_margin)
				{

					value = Search_quiescence::forced_variant_black(position,
								alpha_q_margin,(alpha_q_margin + 1),depth,flag_pv_node);

					if (value > alpha_q_margin)
					{
						return value; 
					}
				}
			//--
#endif// #if RAZORING


#if FUTILITY

	/*
	��� ��� ����� �� ����� ������ �������������:
	Ernst A. Heinz
	Given a suitable value for max posn score, 
	it is therefore safe to simply cut off 
	all non-checking moves at frontier nodes 
	for which the futility condition 
	mat balance(node) + mat gain(move) + max posn score  <= alpha(node) holds. 

	*/
			//futility
			if (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
			{
				//--
				__int32 alpha_margin =  alpha - FUTILITY_MARGIN[(depth_max - depth)];

				if (value <= alpha_margin)
				{
					return value;
				}
			}
			//--
#endif// #if FUTILITY
		}
	}

#endif// #if (SAVE_LINE && ALPHA_BETA_ON && (FUTILITY ||RAZORING )) 

//----------------------------------------------------------
// 7 ������� �� �������� ����
#if (NULL_MOVE && SAVE_LINE)
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	value = black_null_move(position,alpha,beta,depth_max,depth,old_description_move,
							flag_pv_node,flag_old_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE

//----------------------------------------------------------
// 8 ���������� � ��������� ������ ���������� �����

	//Move_generation::mobility_evaluation(position);

	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	Move_generation::generator_captures_black(list_surplus_moves,position);
	Move_generation::generator_move_black(list_surplus_moves,position);
	Move_ordering::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Move_ordering::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY


#if (KILLER && SAVE_LINE)
	Killer_heuristic::insert_killer(list_surplus_moves,depth,list_surplus_moves.end_captures);
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� 
	// �� �� ��� ������� ������
	Transposition_table::look_hash_move(list_surplus_moves
						,position.hash_key,depth,flag_insert_hash_move);

// ��� ������� ����� ����� ��������� ������� �� ������
#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	if (flag_insert_hash_move == 1)
	{
		std::cout << "�������� ���� ������" << std::endl;
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH

//----------------------------------------------------------
// 9 ����� �� ������ ���������� �����
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)


			if (( !flag_pv_f )
			&& (description_move_up  == MOVE_IS_SIMPLE)//
			&& (i > 3)//
			&& (old_flag_check == CHECK_NO))
			{
				//Move count based pruning
				if((depth_max - depth) < 11)
				{
					if ( i >= futility_move_count[(depth_max - depth)]) continue;
				}
			}

#endif//#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY_IN)

		// ��������� ��� �� ������ ������� ������� ���������� �������
		Make_move::do_moves_black(position,i,depth,list_surplus_moves);

		// ���� ������ �� ����� ��
		if (Move_generation::king_black_check(position)!= CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			Make_move::undo_moves_black(position,i,depth,list_surplus_moves);

			// ���� ������ �� ������ 
			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes = nodes + 1;

		new_depth_max = depth_max;

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

		const __int32 description_move = ( list_surplus_moves.move[i] >> 6 ) & 63;
		//const __int32 piece_move = ( list_surplus_moves.move[i] >> 24 ) & 7;

#if SAVE_LINE
		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� 
		// �� ������� �������.
		if (flag_pv_node)
		{
			PV_save::update_PV_current(i,depth,list_surplus_moves);
		}

		Zobrist_hashing::modification_random_key(
			position,0,i,list_surplus_moves,Make_move::get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = position.hash_key;

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		Zobrist_hashing::start_position_random_key(position);

		if (test_key_control != position.hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << std::endl;
			std::cout << "collision black depth = " << depth << std::endl;
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		Zobrist_hashing::save_hash_three(position.hash_key,new_depth); 

#endif//#if SAVE_LINE 


//-------------------------------------------------------------

// ���� �� � ������� ����(pv) �� ������ ��� ���� ������ �������
		if (flag_pv_node && (i == 0))
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
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_1(depth))
				&& (list_surplus_moves.move[i] != 
					Killer_heuristic::get_killer_moves_2(depth))
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

			value = white_searching_max_value(position,alpha_null_window,beta,
								new_depth,new_lmr_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);

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

			value = white_searching_max_value(position,alpha_null_window,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);

			// ���� ������ ���������� ������� � �� � ������� ����
			// �� ���� ����� �� ������� c ������ ����� � ��������
			// ����� �� ���� �����
			flag_do_pv_search = flag_pv_node && (value < beta);
		}
#else
		do_pv_search = true;
#endif//#if ALPHA_BETA_ON

		// ������� ��������� ������� ������ � ������ ����� � ��������
		if (flag_do_pv_search)
		{
			flag_pv_move = PV_YES;// �������� �� ��� ������� (pv)

			value = white_searching_max_value(position,alpha,beta,
								new_depth,new_depth_max,flag_pv_move,
								description_move,flag_check,flag_null_m);
		}
//-------------------------------------------------------------

		// ��������������� �������
		Make_move::undo_moves_black(position,i,depth,list_surplus_moves);


#if SAVE_LINE
		// ��������������� ���-���� �����
		position.hash_key = key_undo;

		//-----------------------------------------------
		if (value < value_min)
		{

//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if ALPHA_BETA_ON
			if (value < beta)
			{
				//-----------------------------------------------
				if (value <= alpha)
				{

					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
#if KILLER
						Killer_heuristic::save_killer(i,list_surplus_moves,depth);
#endif//#if KILLER
#if SW_HISTORY
						History_heuristic::black_history_good_save(
							i,list_surplus_moves,depth,depth_max);
#endif//#if SW_HISTORY
					}


#if HASH
					//(value <= alpha)
					Transposition_table::save_best_hash_score_or_move(i, position.hash_key
													  ,list_surplus_moves, 
													  value, ALPHA_CUTOFF, depth, depth_max);	
#endif//#if HASH

					return value; 
				}
				//-----------------------------------------------

// TEST------------
#if HASH_TEST_SCORE
				// ������� ��� �� ���� ���� ������� ������ ���� ���������� �� ��� ������ :)
				// ���� �� ������ ���������� �� ��� �� ����� ��� �� ������ ������� �� ����� �� ����
				// ��������� �� ������
				// ���� � ��� ��� �� ����� ������ �� ���-������� � ������� ��� ��� ������ ����� �.�. �������
				// ���������. ������ �� ������������ �� � ���� ������ �������������� �� �� ����� �����
				// ���� �� ������ ��� ��� ��������� ������
				if (( Transposition_table::get_flag_found_score() ) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "������ ��������� �� ��� ������� ������" << std::endl;
					std::cout << "depth= " << depth << std::endl;
					std::cout << "depth_max_rem= " << depth_max_rem << std::endl;
					std::cout << "hash_depth= " << (__int32)hash_depth << std::endl;
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max 
						<< std::endl;

					std::cout << "hash_score= " << hash_score  << std::endl;
					std::cout << "value= " << value << std::endl;
					std::cout << "alpha= " << alpha << std::endl;
					std::cout << "beta= " << beta << std::endl;
					std::cout << "  " << std::endl;

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-------------------

				beta = value;
				flag_beta_updated = true;
				i_hash_moves_1 = i; //��� ������� �������� ���� (1- �������� ����) 
									//����� ��� ����������� 

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				if (flag_pv_node)
				{
					PV_save::update_PV_best_point(pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			if (flag_pv_f)
			{
				PV_save::update_PV_best_point(pv_best_point);
			}

			flag_save_best = true;
			i_hash_moves_1 = i;// ��� ������������ ������� ��������
#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_min = value;
			i_hash_moves_3 = i; // ��� ������ ��������� ������� 
								//(3-������ ��������� �������)����� ��� �����������

		}
#endif//#if SAVE_LINE 

	}

	//-----------------------------------------------
	// ��������� ������ ����� �� ���� ��������� �����
	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == false)
	{
#if SAVE_LINE
		value = 0;
		if (Move_generation::king_black_check(position) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_node)
		{
			PV_save::update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_beta_updated == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� 
		// ����� ������� �������
		if (flag_pv_node)
		{
			PV_save::update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)

	//History_heuristic::black_history_bad_save(list_surplus_moves,depth,depth_max);

	if ( flag_beta_updated == true ) 
	{   // (value < beta) � (value > alpha)
		Transposition_table::save_best_hash_score_or_move(
			i_hash_moves_1,position.hash_key,list_surplus_moves, 
			beta, BETA_UPDATED, depth, depth_max);
	}
	else
	{	// (value >= beta) � (value > alpha)
		Transposition_table::save_best_hash_score_or_move(
			i_hash_moves_3,position.hash_key,list_surplus_moves, 
			value_min, LOCAL_MIN, depth, depth_max);
	}

	//if ( flag_beta_updated == true ) 
	//{   // (value < beta) � (value > alpha)
	//	Transposition_table::save_best_hash_move(
	//		i_hash_moves_1,position.hash_key,list_surplus_moves, 
	//		beta, BETA_UPDATED, depth, depth_max);
	//}
	//else
	//{	// (value >= beta) � (value > alpha)
	//	Transposition_table::save_best_hash_move(
	//		i_hash_moves_3,position.hash_key,list_surplus_moves, 
	//		value_min, LOCAL_MIN, depth, depth_max);
	//}

#endif//#if HASH

	return value_min;
}

/*
 ����� ����������� ��������� ��������� ��� ������ �������� ����
 � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
 ���� ��� � ��� ���
 �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
 ������� ���� ��� ����� ����� ���������
 �� �����
*/
inline __int32 Search::white_null_move
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 old_description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_node,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m,// ���� �������� ����
    const unsigned __int8 new_depth,// ��������� �������
	const unsigned __int64 key_undo
)
{

	if ((flag_pv_node == PV_NO) // ���� �� �� � ������� ��������
	  &&(flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
	  &&((depth_max - new_depth) > NULL_REDUCTION ) // ���� �� ������������ ������� 
											   // ���������� �����������
	&& (old_description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
	&& (old_flag_check == CHECK_NO) // ���� ���� ���
	//&& (bitboard- >= beta) // ����� ������ �������������. ������ �� �������
									// �������� ���� � ��� ��� � �� ��������� �����������
									// ������������ ������ � ������ ��������
	)
	{// ����� �������� ������� ���


		// ���������� ����� ������ �� ������� � �������� ����������.
		bool taking_passage_yes_mem = position.taking_passage_yes;
		unsigned __int8 taking_passage_x_mem = position.taking_passage_x;
		unsigned __int8 taking_passage_y_mem = position.taking_passage_y;

        unsigned __int64 hash_key_temp = position.hash_key;

		// ������ ���-���� ��� ������� ��� -------------------------
		// ��������� ������ �� �������
		if (taking_passage_yes_mem)
		{
			hash_key_temp = hash_key_temp ^ 11782889425078278382;
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ 6567344619664815343);

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ 10614454250492056336);
		}

		// ��������� ���� ����
		// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
		// ��������� ������� ���������� ������ ������ ����
		hash_key_temp = hash_key_temp ^ 8935749541958697872;
		position.hash_key = hash_key_temp;


		position.taking_passage_yes = 0;
		position.taking_passage_x = 0;
		position.taking_passage_y = 0;

		position.colour_move = !position.colour_move;

		const __int64 nodes_null_rem = nodes;

		// TEST ���������� ��������� ���-�����
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//Zobrist_hashing::start_position_random_key(bitboard);

		// �� ������������ ��������
		__int32 value = black_searching_min_value(position,alpha,(alpha + 1),
								new_depth,(depth_max - NULL_REDUCTION),PV_NO,
								old_description_move,old_flag_check,NULL_YES);

		//--------------------------------------------------------
		// ������������ �������� �������� ���� ��� ��������� �� ������������ �������
		// ������ ������ 5 ���������� 4 ����� 3
		//unsigned __int8 dynamic_null_move_reduction = 3 +
		//	( depth >= SWITCH_NULL_REDUCTION_1_2 
		//	? depth / 4 : 0); // 

		//dynamic_null_move_reduction = (dynamic_null_move_reduction >= (depth_max - depth))
		//	? (depth_max - depth - 1) : dynamic_null_move_reduction;
		////
		//__int32 value = black_searching_min_value(position,alpha,(alpha + 1),
		//						new_depth,(depth_max - dynamic_null_move_reduction),PV_NO,
		//						old_description_move,old_flag_check,NULL_YES);
		//--------------------------------------------------------

		// ���� ����������� � ������� ������ �� ���������
		nodes = nodes_null_rem;

		// TEST
		//std::cout << "bitboard- "<< bitboard- << std::endl;
		//std::cout << "i_move= "<< i_move << std::endl;
		//std::cout << "description_move= "<< description_move << std::endl;
		//std::cout << "check= "<< check << std::endl;
		//std::cout << "       "<< std::endl;
		//
		//if (value < beta)
		//{
		//	std::cout << "�������� "<< std::endl;
		//	std::cout << "depth_max= "<< depth_max <<std::endl;
		//	std::cout << "depth= "<< depth <<std::endl;
		//	std::cout << "value= "<< value <<std::endl;
		//	std::cout << "beta= "<< beta <<std::endl;
		//	std::cout << "       "<< std::endl;
		//}
		//--------------------------------------------------------

		position.taking_passage_yes = taking_passage_yes_mem;
		position.taking_passage_x = taking_passage_x_mem;
		position.taking_passage_y = taking_passage_y_mem;

		position.colour_move = !position.colour_move;

		// ��������������� ���-���� �����
		position.hash_key = key_undo;


		// ��� ��� ������ ��� ������� ��� �������� ����� ����������� ������ �.�. �������� ����
		// ����� ���������� ���� ��� ��� ����� ������� ������ ����
		// ������ ����� ��� ������ � ���� �� �� ������ �������� ���� �.�. ���� ������ ������
		// ��� ����� ���� ������ ����� ������ �.�. ����� ��� �� ���������
		// ������ ��� ���� ���� � ����� ����� �� ������ �������� �� ��� ���� � ������� �� ������ 
		if (value >= beta)
		{

			//TEST
			//std::cout << "������� "<< std::endl;
			//std::cout << "depth_max= "<< depth_max <<std::endl;
			//std::cout << "depth= "<< depth <<std::endl;
			//std::cout << "value= "<< value <<std::endl;
			//std::cout << "beta= "<< beta <<std::endl;
			//std::cout << "       "<< std::endl;
			//return beta;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return beta;
			}

			// ������� ������ ��������
			value = white_searching_max_value(position,(beta - 1),beta,
								depth,(depth_max - VERIFICATOIN_SEARCH_REDUCTION)
								,PV_NO,old_description_move,old_flag_check,NULL_YES);

			if (value >= beta)
			{
				return beta;
			}
		}
	}

	return -1000000000;
}

/*
 �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
 ������� ���� ��� ����� ����� ���������
 �� ������
*/
inline __int32 Search::black_null_move
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 old_description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_node,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m,// ���� �������� ����
    const unsigned __int8 new_depth,// ��������� �������
	const unsigned __int64 key_undo
)
{

	if ((flag_pv_node == PV_NO) // ���� �� �� � ������� ��������
	  &&(flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
	  &&((depth_max - new_depth) > NULL_REDUCTION ) // ���� �� ������������ ������� 
											   // ���������� �����������
	&& (old_description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
	&& (old_flag_check == CHECK_NO) // ���� ���� ���
	//&& (bitboard- <= alpha) //
	)
	{

		// ����� �������� ������� ���
		const bool taking_passage_yes_mem = position.taking_passage_yes;
		const unsigned __int8 taking_passage_x_mem = position.taking_passage_x;
		const unsigned __int8 taking_passage_y_mem = position.taking_passage_y;


       unsigned __int64 hash_key_temp = position.hash_key;

		// ������ ���-���� ��� ������� ��� -------------------------
		// ��������� ������ �� �������
		if ( taking_passage_yes_mem)
		{
			hash_key_temp = hash_key_temp ^ 11782889425078278382;
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ 6567344619664815343);

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ 10614454250492056336);
		}

		// ��������� ���� ����
		// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
		// ��������� ������� ���������� ������ ������ ����
		hash_key_temp = hash_key_temp ^ 8935749541958697872;

		position.hash_key = hash_key_temp;

//--------

		position.taking_passage_yes = 0;
		position.taking_passage_x = 0;
		position.taking_passage_y = 0;

		position.colour_move = !position.colour_move;
	
		const __int64 nodes_null_rem = nodes;

		// TEST ���������� ��������� ���-�����
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//Zobrist_hashing::start_position_random_key(bitboard);

		__int32 value = white_searching_max_value(position,(beta - 1),beta,
								new_depth,(depth_max - NULL_REDUCTION)
								,PV_NO,old_description_move,old_flag_check,NULL_YES);
			
 
		//--------------------------------------------------------
		//unsigned __int8 dynamic_null_move_reduction = 3 +
		//	( depth >= SWITCH_NULL_REDUCTION_1_2 ? depth / 4 : 0); // 

		//dynamic_null_move_reduction = (dynamic_null_move_reduction >= (depth_max - depth))
		//	? (depth_max - depth - 1) : dynamic_null_move_reduction;

		//__int32 value = white_searching_max_value(position,(beta - 1),beta,
		//						new_depth,(depth_max - dynamic_null_move_reduction)
		//						,PV_NO,old_description_move,old_flag_check,NULL_YES);

		//--------------------------------------------------------

		nodes = nodes_null_rem;

		position.taking_passage_yes = taking_passage_yes_mem;
		position.taking_passage_x = taking_passage_x_mem;
		position.taking_passage_y = taking_passage_y_mem;

		position.colour_move = !position.colour_move;

		// ��������������� ���-���� �����
		position.hash_key = key_undo;


		//��� ��� ����� ��� ������� ��� �������� ����� ������������ ������ �.�. �������� �����
		//������ ���������� ���
		//������ ����� ��� ����� � ���� �� �� ������ �������� ����� �.�. ���� ������ ������
		//��� ����� ����� ������ ����� ������ �.�. ����� ��� �� ���������
		if (value <= alpha)
		{ 
			//std::cout << "������� "<< std::endl;
			//std::cout << "depth_max= "<< depth_max <<std::endl;
			//std::cout << "depth= "<< depth <<std::endl;
			//std::cout << "value= "<< value <<std::endl;
			//std::cout << "alpha= "<< alpha <<std::endl;
			//std::cout << "       "<< std::endl;

			//return alpha;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return alpha;
			}

			// ������� ������ ��������
			value = black_searching_min_value(position,alpha,(alpha + 1),
								depth,(depth_max - VERIFICATOIN_SEARCH_REDUCTION)
								,PV_NO,old_description_move,old_flag_check,NULL_YES);

			if (value <= alpha)
			{
				return alpha;
			}
		}
	}

	return -1000000000;
}


/*
 ���������� ������� �� ������������ �������. �� �����
*/
inline __int32 Search::max_depth_white
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_node,// �������� �� ��� ������� (pv)
	const unsigned __int8 depth// �������
)
{
	__int32 score = 0;
	__int32 score_f = 0;

	score = Evaluate::eval_state(position) 
		+ (position.value_white_material 
		- position.value_black_material)
	;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(bitboard) != bitboard-)
	//{
	//	std::cout << "WHITE  Eval_forced(bitboard) != bitboard-" <<std::endl;
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<std::endl;
	//	std::cout << "bitboard-= "<< bitboard-<<std::endl;
	//}
	//--------------------------------------------------------------------------

	//� ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
	//���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
	if (flag_pv_node)
	{
		PV_save::update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
	//�����  ������ ���� � ����
	//Print_std::cout(list_surplus_moves,pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score ;
}

/*
 ���������� ������� �� ������������ �������. �� ������
*/
inline __int32 Search::max_depth_black
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_node,// �������� �� ��� ������� (pv)
	const unsigned __int8 depth// �������
)
{
	__int32 score = 0;
	__int32 score_f = 0;

	score = Evaluate::eval_state(position) 
		+ (position.value_white_material
		- position.value_black_material)
	;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(bitboard) != bitboard-)
	//{
	//	std::cout << "BLACK  Eval_forced(bitboard) != bitboard-" <<std::endl;
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<std::endl;
	//	std::cout << "bitboard-= "<< bitboard-<<std::endl;
	//}
	//--------------------------------------------------------------------------

	//� ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
	//���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
	if (flag_pv_node)
	{
		PV_save::update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
	//�����  ������ ���� � ����
	//Print_std::cout(list_surplus_moves,pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score;
}


/*
 ������� ������ ���. ���������� �����
*/
inline void Search::quit_or_stop()
{
	double delta_time_current;

	//-----------------------------------------------
	// ��������� ������ ����������� ���������� ����� 
	if (audit > 50000)
	//if (0)
	{
		//���� ��������� ������� �� ��� ��� quit ��� stop
		if (public_bioskey())
		//if (0)
		{
			std::cin.getline(string_in2,LENGTH_STRING, '\n');

			if (strcmp(string_in2, "quit") == 0)
			{
				std::cout << "Ifrit: quit command. I am terminated." << std::endl;
				Transposition_table::hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// ��������� ������
				flag_stop_search = 1;
			}
		}

		//���������� ����� � �������. ���������� ����� �� �����
		Time_management::time_stop();
		delta_time_current = Time_management::time_delta();

		//������� �� �������
		if ((deep_time_terminate > 0) && (deep_time_terminate < delta_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;

}

/*

*/
void Search::set_depth_max_rem
(
	unsigned __int8 _depth_max_rem//
)
{
	depth_max_rem = _depth_max_rem;
}

/*

*/
void Search::ini_deep_time_terminate
(
	double _deep_time_terminate//
)
{
	deep_time_terminate = _deep_time_terminate;
}

/*

*/
void Search::ini_global_loop()
{
	audit = 0;// �������� ������ ������� ����������� ������
	flag_stop_search = 0;//
}

/*
 ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
 �� ��������� �� �������� �� ������
*/
bool Search::get_stop_search()
{
	return flag_stop_search;
}

/*
 ���������� ���������� ������������� �����
*/
__int64  Search::get_nodes()
{
	return nodes;// ���������� �����;
}

/*
 �������� ���������� ������������� �����
*/
void Search::set_nodes_in_0()
{
	nodes = 0;// ���������� �����;
}


