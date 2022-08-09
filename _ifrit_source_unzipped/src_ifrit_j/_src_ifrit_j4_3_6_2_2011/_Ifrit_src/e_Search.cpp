
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 2.11.2009 20.12.2009 25.07.2010 17.10.2010

#include <fstream>
#include <iostream>

#include "a_Search_switch.h"

#include "e_Search.h"
#include "f_Search_quiescence.h"
#include "g_Evaluate.h"
#include "h_Killer.h"
#include "i_ZObrist.h"
#include "j_Hash.h"
#include "k_PV_save.h"
#include "l_Chess_bitboard.h"
#include "m_List_surplus_moves.h"
#include "n_Timer.h"
#include "o_Win_32.h"

class Search_quiescence search_quiescence;
class List_surplus_moves search_list_surplus_moves[MAX_DEPTH];// ������ ��������� ����� (������������ ���� �� ������� ����)

extern class Evaluate evaluate;
extern class Killer killer;
extern class ZObrist zobrist;
extern class Hash hash;
extern class PV_save pv_save;
extern class Chess_bitboard bitboard;
extern class Timer timer;


//	===================================================================
// ���� �������� � ����. ������� �� ����� 
__int32 Search::white
(
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move_up,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m// ���� �������� ����
)
{
	__int32 value;              // ������� ������ �������
	__int32 value_max = -9999999;   // ������ ������� ������ � ���� 

	class List_surplus_moves & list_surplus_moves 
		= search_list_surplus_moves[depth];// ������ ��������� ����� (������������ ���� �� ������� ����)

	bool flag_is_legal_moves = false;           // ���� ������������� ��������� �����
	bool flag_save_best = false;           // ����� �� ������ �������
	const unsigned __int8 new_depth = depth + 1; // ��������� �������
	bool flag_check = false;//���� ����

	unsigned __int8 depth_max_new;//����� ������� ��������

	// ���������� ��������� � ������ NegaScout � LMR
		__int32 beta_scout;//
		unsigned __int8 depth_max_new_scout;// ������������ �������
		bool flag_pv_f_scout;// �������� �� ��� ������� (pv)
		bool search_scout;


//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
 
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������

	  
	// ��� ��� �����������
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // ���������� ���-���� �������� ����
	unsigned __int8  flag_insert_hash_move = 0; // ��������� �� ��� �� ���-������� �� ������ �������
	__int32 hash_score = -1000000000;// ���-������ �������
	unsigned __int8 i_hash_moves_1;// ��� ������� �������� ����� (1- �������� �����) ����� ��� ����������� 
	unsigned __int8 i_hash_moves_3;// ��� ������ ��������� �������� (3-������ ��������� �������� )����� ��� �����������

//#if (LMR && SAVE_LINE)
//	unsigned __int8 lmr_reduction;//
//#endif//#if (LMR

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


	// 1
	// ������� �� ������� �� ��������� �����

#if SAVE_LINE


	// ��� ������� ��� ���������� ������
	if (flag_stop_search == true)
	{
		return 0;
	}

	// 
	quit_or_stop();

#endif//#if SAVE_LINE 

	// 2
	// ������� �� ������������ ������� (������ �������)

	// �� ���������� ������� ���������� ����������� ������ �������
	if (depth >= depth_max)
	{

#if SAVE_LINE

	// ����������
	// ���������� ������������� �������
#if FORCED
		// �������� ����� �������� ������ ������
		value = search_quiescence.forced_variant_white(alpha,beta
													,depth,flag_pv_f);
#endif//#if FORCED

	// �������� ������ ��� ����������
#if !FORCED 
		value = max_depth_white(alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

	// 3
	// ������� �� ���������� �������

#if (REPETITION && ALPHA_BETA_ON && SAVE_LINE)
	// ���������� ���������� �������
	if (zobrist.analysis_hash_string_moves(depth))
	{
		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ 
		// ���� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(0 // ������ �����
													,depth);
		}

		return 0;
	}
#endif//#if REPETITION

	// 4
	// ������� �� ���-������

#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// ������� �� ��� ������ �� ���������� ��� �������� ��������
	// ������� ������� ��� ������������� ������ � ����� �������� � ��� ������� �������
	// ������ ���������� ������� ������� ������������.
	if (!flag_pv_f)
	{ 
		// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� 
		// �������� ��������� ����������� ������
		hash_score = hash.look_hash_score_white(
			bitboard.get_hash_key(),depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (hash_score != -1000000000 )
		{
			//return_hash_score = return_hash_score + 1 ;
#if (!HASH_TEST_SCORE)
			return hash_score;
#endif//#if (!HASH_TEST_SCORE)

		}
	}

#endif//#if HASH
#endif//#if (HASH_SCORE)

	// 5
	// �������
#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY)

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

	if (!flag_pv_f)
	{ 
		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
		&& (old_flag_check == CHECK_NO)) // ���� ���� ���
		{
			
			value = bitboard.get_value_white_material() 
				- bitboard.get_value_black_material();

			//razoring
			//--
			__int32 beta_q_margin =  beta - RAZORING_MARGIN[(depth_max - depth)];

			if (value < beta_q_margin)
			{
				value = search_quiescence.forced_variant_white(
							alpha,beta,depth,flag_pv_f);

				if ( value < beta_q_margin )
				{
					return value; 
				}
			}
			//--

			//futility
			//--
			__int32 beta_margin =  beta + FUTILITY_MARGIN[(depth_max - depth)];

			if (value >= beta_margin)
			{
				value = bitboard.get_value_white_material() 
					- bitboard.get_value_black_material()
					+ evaluate.eval_state();

				if (value >= beta_margin )  
				{
					return value;  
				}
			}
			//--
		}
	}

#endif//#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY)

	// 6
	// ������� �� �������� ����

#if (NULL_MOVE && SAVE_LINE)
	// ����� ����������� ��������� ��������� ��� ������ �������� ����
	// � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
	// ���� ��� � ��� ���
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	value = null_move_white(alpha,beta,depth_max,depth,description_move_up,
							flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE




	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	list_surplus_moves.generator_captures_white(bitboard);
	list_surplus_moves.generator_move_white(bitboard);
	list_surplus_moves.sorting_moves();

 
#if (KILLER && SAVE_LINE)
	// ���� ��� ������ ��� ������ ������
	// �� ������ ��������� �� ������ ������� ����� �� ������
	if ((list_surplus_moves.get_end_captures() == CAPTURE_NO) 
	|| (list_surplus_moves.get_sorting_score(0) < 0))
	{
		killer.insert_killer(list_surplus_moves,depth,0);
	}
	else
	{
		killer.insert_killer(list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� 
	// ������� ������
	// ������ �� ��������� ��� �� ��� ������� �� ������ ����� ������� ��������� ����
	hash.look_hash_move(list_surplus_moves,bitboard.get_hash_key(),depth
						,flag_insert_hash_move);

// ��� ������� ����� ����� ��������� ������� �� ������
#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	if (flag_insert_hash_move == 1)
	{
		std::cout << "�������� ���� �����" << "\n";
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH


//#if (LMR && SAVE_LINE)
//	// ��� �� ���-�������, ������ ������, ��� �������, ��� ������ �� ������������
//	lmr_reduction = FULL_SEARCH + list_surplus_moves.get_end_captures() ; // 
//	//lmr_reduction = FULL_SEARCH ; // 
//#endif//#if LMR


	//����� �� ������ ���������� �����
	// ����� � ����� ����� ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.get_end_list(); i++)
	{
		// ��������� ��� �� ������ ������� ������� ���������� �������
		bitboard.do_moves_white(i,depth,list_surplus_moves);

		// ���� ������ ��� ����� �� 
		if (list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			bitboard.undo_moves_white(i,depth,list_surplus_moves);

            // ���� ������ �� ������ 
			continue;
		}


		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes = nodes + 1;

#if SAVE_LINE
		// �� ������ ��������� ����� �������� ������� ��� � 
		// ������� ������� �� ������� �������.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		zobrist.modification_random_key(1,i,list_surplus_moves
										,bitboard.get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = bitboard.get_hash_key();

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		zobrist.start_position_random_key();

		if (test_key_control != bitboard.get_hash_key())
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision white depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth); 

		// ������ ��� ���������� ����� ������������ ������� ��� �� ���������� ����� 
		// � �� ���� ����

#endif//#if SAVE_LINE 

		depth_max_new = depth_max;

#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� �� 1 ������������ ������� ��������
		extension_for_chek_white(depth_max,depth_max_new,flag_check,list_surplus_moves);
#endif//#if EXTENSION_CHEK

		// ��� ����: 0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
		// 4 � �������� ��������� 5 � ������ �� ������� � �.�.
		const __int32 description_move = ( list_surplus_moves.get_move(i) >> 6 ) & 63;

		depth_max_new_scout = depth_max_new;

//_scout_scout white
#if (ALPHA_BETA_ON && SAVE_LINE)
		if ( (flag_pv_f) && (i == 0) ) 
#else
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			// ���� �� � ������� �������� ��
			// ������ ��� ������� �� ������ ������� � � ������ �����
			// �� ������������ ��� ���� �� ������ �� ������ � ������� ������� 
			// ���������
			// ����� ���� ������ ��� ��� ��� �� ���-�������
			beta_scout = beta;
			flag_pv_f_scout = PV_YES;

			value = black(alpha,beta_scout,depth_max_new_scout,new_depth
							,description_move,
							flag_pv_f_scout,flag_check,flag_null_m);
		}
		else
		{
			//
			search_scout = true;

			// ������� �� �������� LMR

			// ����� ���������� ������ ��� �� ��� ������� ������ ��� 
#if (LMR && SAVE_LINE)
			//if ((i > lmr_reduction) 
			//&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
			//&& ((depth_max_new - depth) >= LMR_DEPTH ))
			//{
			if ((description_move != MOVE_IS_CAPTURE)// �� ������
			&& (description_move < 10)// �� �����������
			&& (i > 0)// �� ������ (��� �� ��� �������)
			&& (list_surplus_moves.get_move(i) != killer.get_killer_moves_1(i))
			&& (list_surplus_moves.get_move(i) != killer.get_killer_moves_2(i))
			&& (flag_check == CHECK_NO) 
			&& (old_flag_check == CHECK_NO)
			&& ((depth_max_new - depth) >= LMR_DEPTH ))
			{
				// ���� � ��������� ������� � � ������� �����
				beta_scout = alpha + 1;
				flag_pv_f_scout = PV_NO;
				depth_max_new_scout = depth_max_new - LMR_REDUCTION_D;

				//
				value = black(alpha,beta_scout,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
				//
				search_scout = (value > alpha);

				depth_max_new_scout = depth_max_new;
			}
#endif//#if  LMR

			if (search_scout)
			{
				beta_scout = alpha + 1;
				flag_pv_f_scout = PV_NO;
				//
				value = black(alpha,beta_scout,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
			}

			// ��� �������� �������� ������������� � ������ ����� � ��������
			// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
			// ����� ������� ������� ��� � ����������� ����� �.�. ������� �����
			// ���� ��� ���� ������� �� ������������� c ������ �����
			// ����� �� ����� ��������� ������ �������� ��������
			//if ( (flag_pv_f) && (value > alpha) && (i != 0) )
			if ( (flag_pv_f) && (value > alpha) && (value < beta) && (i != 0) )
			{
				beta_scout = beta;
				flag_pv_f_scout = PV_YES;

				//
				value = black(alpha,beta_scout,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
			}
		}


		// ��������������� �������
		bitboard.undo_moves_white(i,depth,list_surplus_moves);

#if SAVE_LINE
		// ��������������� ���-���� �����
		bitboard.set_hash_key(key_undo);

		//--------------------------------------------
		if (value > value_max)
		{
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if ALPHA_BETA_ON
			if (value > alpha)
			{
					
				if (value >= beta)
				{        
#if KILLER
					// ����� ������� ���� ��� �� ������
					if (((list_surplus_moves.get_move(i)>>6)& 63) != MOVE_IS_CAPTURE)
					{
						killer.save_killer(i,list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// �������� ������ �� ������ ������ ���
					hash.save_best_hash_score_or_move(i
						, bitboard.get_hash_key(), list_surplus_moves, 
						value, LOWER, depth, depth_max);
#endif//#if HASH

					// TEST
					//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 ))
					//{
						//std::cout << "��������� �� ��� ������� ����� " << "\n";
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
				if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "������ ��������� �� ��� ������� ����� " << "\n";
					std::cout << "depth= " << depth << "\n";
					std::cout << "depth_max_rem= " << depth_max_rem << "\n";
					std::cout << "hash_depth= " << (__int32)hash_depth << "\n";
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
					std::cout << "hash_score= " << hash_score  << "\n";
					std::cout << "value= " << value << "\n";
					std::cout << "alpha= " << alpha << "\n";
					std::cout << "beta= " << beta << "\n";
					std::cout << "  " << "\n";

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-----------------


				alpha = value;
				flag_save_best = true;
				i_hash_moves_1 = i; // ��� ������� �������� ����� (1- �������� �����) 
									// ����� ��� ����������� 

				// ����� �� �������������� ������ ������� ��������������� 
				// ������ ������
				if (flag_pv_f)
				{
					pv_save.update_PV_best_point(pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			if (flag_pv_f)
			{
				pv_save.update_PV_best_point(pv_best_point);
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


	// ��������� ������ ����� �� ���� ��������� �����
	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == false)
	{
#if SAVE_LINE
		value = 0;
		if (list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ 
		// ���� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� 
		// �� ����� ������� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)
	if (flag_save_best == true)
	{
		// �������� ��� ���������� ����� �.�. ���� 1
		hash.save_best_hash_score_or_move(i_hash_moves_1
			, bitboard.get_hash_key(), list_surplus_moves, 
			alpha, EXACT, depth, depth_max);
	}
	else
	{
		// �������� ��� ������ ��������� �������� �.�. ���� 3
		hash.save_best_hash_score_or_move(i_hash_moves_3
			, bitboard.get_hash_key(), list_surplus_moves, 
			value_max, UPPER, depth, depth_max);
	}
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
	if (hash_score != -1000000000)
	//if ( 0 )
	{
		hash_moves_test13 = i_hash_moves_3;
		if (flag_save_best == 1) hash_moves_test13 = i_hash_moves_1;

		//if ( hash_moves_test13 != 0 )
		//if ( hash_score != value_max )
		if (value_max > alpha) 
		{
			std::cout << "������������ � ��������� ���� ������ �������� ������ ����� " << "\n";
			std::cout << "bitboard.get_hash_key()= " << bitboard.get_hash_key() << "\n";
			std::cout << "depth= " << depth << "\n";
			std::cout << "depth_max_rem= " << depth_max_rem << "\n";
			std::cout << "hash_depth= " << (__int32)hash_depth << "\n";
			std::cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
			std::cout << "test_flag_hash_moves= " << test_flag_hash_moves  << "\n";
			std::cout << "hash_score= " << hash_score  << "\n";
			std::cout << "value_max= " << value_max << "\n";
			std::cout << "alpha= " << alpha << "\n";
			std::cout << "beta= " << beta << "\n";

			std::cout << "��� �� ��� ������� " << "\n";
			std::cout << "((list_surplus_moves.move[0]>>6)& 63)= " 
				<< ((list_surplus_moves.get_move(0)>>6)& 63) << "\n";

			std::cout << "������� ������ " 
				<< pv_save.int_to_char_board(((list_surplus_moves.get_move(0)>>24)& 7),1)
				<< "\n";
               
			std::cout <<pv_save.x_to_notation(((list_surplus_moves.get_move(0)>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(0)>>12)& 63)/8);
			std::cout <<pv_save.x_to_notation(((list_surplus_moves.get_move(0)>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(0)>>18)& 63)/8) << "\n";

			std::cout << "��� ��������� � ������ " << "\n";
			std::cout << "hash_moves_test12= " << hash_moves_test13  << "\n";
			std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				<< ((list_surplus_moves.get_move(hash_moves_test13)>>6)& 63) << "\n";
			std::cout << "������� ������ " 
				<< pv_save.int_to_char_board(((
					list_surplus_moves.get_move(hash_moves_test13)>>24)& 7),1)<< "\n";
               
			std::cout <<pv_save.x_to_notation(((list_surplus_moves.get_move(hash_moves_test13)>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(hash_moves_test13)>>12)& 63)/8);
			std::cout <<pv_save.x_to_notation(((list_surplus_moves.get_move(hash_moves_test13)>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.get_move(hash_moves_test13)>>18)& 63)/8) << "\n";
			std::cout << "  " << "\n"; 

			exit(1);
		}
	}
#endif //#if HASH_TEST_SCORE
//-----------------

	return value_max;
}

//	===================================================================
// ���� ������� � ����. ������� �� ������
__int32 Search::black
(
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move_up,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m// ���� �������� ����
)
{
	__int32 value;              // ������� ������ �������
	__int32 value_min = 9999999;   // ������ ������� ������ � ���� 

	class List_surplus_moves & list_surplus_moves 
		= search_list_surplus_moves[depth];// ������ ��������� ����� (������������ ���� �� ������� ����)

	bool flag_is_legal_moves = false;           // ���� ������������� ��������� �����
	bool flag_save_best = false;           // ����� �� ������ �������
	const unsigned __int8 new_depth = depth + 1; // ��������� �������
	bool flag_check = false;//���� ����

	unsigned __int8 depth_max_new;//����� ������� ��������

	// ���������� ��������� � ������ NegaScout � LMR
		__int32 alpha_scout;//
		unsigned __int8 depth_max_new_scout;// ������������ �������
		bool flag_pv_f_scout;// �������� �� ��� ������� (pv)
		bool search_scout;

#if SAVE_LINE
 
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	bool taking_passage_yes_mem = false;
	unsigned __int8 taking_passage_y_mem = 0;
	unsigned __int8 taking_passage_x_mem = 0;

	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // ���������� ���-���� �������� ����
	__int32 hash_score = -1000000000;// ���-������ �������
	unsigned __int8 i_hash_moves_1;// ��� ������� �������� ���� (1- �������� ����) ����� ��� ����������� 
	unsigned __int8 i_hash_moves_3;// ��� ������ ��������� ������� (3-������ ��������� �������)����� ��� �����������

//#if (LMR && SAVE_LINE)
//	unsigned __int8 lmr_reduction;//
//#endif//#if (LMR

	//unsigned __int64 test_key_control;// ���������� ���-���� �������� ����
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 

#if (HASH_SCORE && SAVE_LINE)
	// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 
	// 2-������� �� ����(�����))
	unsigned __int8 test_flag_hash_moves;
	unsigned __int8 hash_depth;// ������������ �������
	unsigned __int8 hash_depth_max;// ������������ �������
#endif//#if (HASH_SCORE)


//#if SAVE_LINE
//	// ��� ������� ��� ���������� ������
//	if (flag_stop_search == true)
//	{
//		return 0;
//	}
//#endif//#if SAVE_LINE 

	// �� ���������� ������� ���������� ����������� ������ �������
	if (depth >= depth_max)
	{
#if SAVE_LINE

	//  ����������
	//  ���������� ������������� �������
#if FORCED 
		// �������� ����� ����������(�������� ������ ������)
		value = search_quiescence.forced_variant_black(alpha,beta
													,depth,flag_pv_f);
#endif//#if FORCED 


	// �������� ������ ��� ����������
#if !FORCED
		value = max_depth_black(alpha,beta,flag_pv_f,depth);
#endif//#if !FORCED 

#endif//#if SAVE_LINE 

		return value ;
	}

#if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
	// ���������� ���������� �������
	if (zobrist.analysis_hash_string_moves(depth))
	{
		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(0 // ������ �����
														,depth);
		}

		return 0;
	}
#endif//#if ALPHA_BETA_ON

#if (HASH_SCORE)
#if (HASH && SAVE_LINE)
	// ������� �� ��� ������ �� ���������� ��� �������� ��������
	// ������� ������� ��� ������������� ������ � ����� �������� � ��� ������� �������
	// ������ ���������� ������� ������� ������������.
	if (!flag_pv_f)
	{ 
		// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� 
		// �������� ��������� ����������� ������
		hash_score = hash.look_hash_score_black(
			bitboard.get_hash_key(),depth,depth_max,
			alpha,beta,test_flag_hash_moves,hash_depth,
			hash_depth_max);

		if (hash_score != -1000000000 ) 
		{
			//return_hash_score = return_hash_score + 1 ;
#if (!HASH_TEST_SCORE)
			return hash_score;
#endif//#if (!HASH_TEST_SCORE)
		}
	}
#endif//#if HASH
#endif//#if (HASH_SCORE)

#if (SAVE_LINE && ALPHA_BETA_ON && FUTILITY)  

	/*
	��� ��� ����� �� ����� ������ �������������:
	Ernst A. Heinz
	Given a suitable value for max posn score, 
	it is therefore safe to simply cut off 
	all non-checking moves at frontier nodes 
	for which the futility condition 
	mat balance(node) + mat gain(move) + max posn score  <= alpha(node) holds. 

	*/

	if (!flag_pv_f)
	{ 

		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
		&& (old_flag_check == CHECK_NO)) // ���� ���� ���
		{
			
			value = bitboard.get_value_white_material() 
				- bitboard.get_value_black_material();

			//razoring 

			//--
			__int32 alpha_q_margin =  alpha + RAZORING_MARGIN[(depth_max - depth)];

			if (value > alpha_q_margin)
			{
				value = search_quiescence.forced_variant_black(
							alpha,beta,depth,flag_pv_f);

				if (value > alpha_q_margin )
				{
					return value; 
				}
			}
			//--

			//futility

			//--
			__int32 alpha_margin =  alpha - FUTILITY_MARGIN[(depth_max - depth)];

			if (value <= alpha_margin)
			{
				value = bitboard.get_value_white_material() 
					- bitboard.get_value_black_material()
					+ evaluate.eval_state();

				if (value <= alpha_margin )  
				{
					return value;
				}
			}
			//--
		}
	}

#endif// (SAVE_LINE && ALPHA_BETA_ON && FUTILITY) 


#if (NULL_MOVE && SAVE_LINE)
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	value = null_move_black(alpha,beta,depth_max,depth,description_move_up,
							flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE


	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	list_surplus_moves.generator_captures_black(bitboard);
	list_surplus_moves.generator_move_black(bitboard);
	list_surplus_moves.sorting_moves();


#if (KILLER && SAVE_LINE)
	// ���� ��� ������ ��� ������ �������������
	// �� ������ ��������� �� ������ ������� ����� �� ������
	if ((list_surplus_moves.get_end_captures() == CAPTURE_NO)
	|| (list_surplus_moves.get_sorting_score(0) < 0))
	{
		killer.insert_killer(list_surplus_moves,depth,0);
	}
	else
	{
		killer.insert_killer(list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� 
	// �� �� ��� ������� ������
	hash.look_hash_move(list_surplus_moves
						,bitboard.get_hash_key(),depth,flag_insert_hash_move);

// ��� ������� ����� ����� ��������� ������� �� ������
#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	if (flag_insert_hash_move == 1)
	{
		std::cout << "�������� ���� ������" << "\n";
		exit(1); 
	}
#endif//#if HASH_TEST
#endif//#if HASH


//#if (LMR && SAVE_LINE)
//	// ��� �� ���-�������, ������ ������, ��� �������, ��� ������ �� ������������
//	lmr_reduction = FULL_SEARCH + list_surplus_moves.get_end_captures(); // 
//	//lmr_reduction = FULL_SEARCH ; // 
//#endif//#if LMR


	// ����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.get_end_list(); i++)
	{
		// ��������� ��� �� ������ ������� ������� ���������� �������
		bitboard.do_moves_black(i,depth,list_surplus_moves);

		// ���� ������ �� ����� ��
		if (list_surplus_moves.king_black_check(bitboard)!= CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			bitboard.undo_moves_black(i,depth,list_surplus_moves);

			// ���� ������ �� ������ 
			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes = nodes + 1;

#if SAVE_LINE
		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� 
		// �� ������� �������.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		zobrist.modification_random_key(0,i,list_surplus_moves,bitboard.get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = bitboard.get_hash_key();

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		zobrist.start_position_random_key();

		if (test_key_control != bitboard.get_hash_key())
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision black depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth); 

#endif//#if SAVE_LINE 


		depth_max_new = depth_max;

#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
		extension_for_chek_black(depth_max,depth_max_new,flag_check,list_surplus_moves);
#endif//#if EXTENSION_CHEK

		// ��� ����: 0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
		// 4 � �������� ��������� 5 � ������ �� ������� � �.�.
		const __int32 description_move = ( list_surplus_moves.get_move(i) >> 6 ) & 63;

		depth_max_new_scout = depth_max_new;

//_scout_scout black
#if (ALPHA_BETA_ON && SAVE_LINE)
		if ( (flag_pv_f) && (i == 0) ) 
#else
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			alpha_scout = alpha;//
			flag_pv_f_scout = PV_YES;// �������� �� ��� ������� (pv)
	
			value = white(alpha_scout,beta,depth_max_new_scout,new_depth,description_move,
							flag_pv_f_scout,flag_check,flag_null_m);
		}
		else
		{
			//
			search_scout = true;

#if (LMR && SAVE_LINE)
			// ������� �� �������� LMR
			//if ((i > lmr_reduction)
			//&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
			//&& ((depth_max_new - depth) >= LMR_DEPTH )) 
			//{
			if ((description_move != MOVE_IS_CAPTURE)// �� ������
			&& (description_move < 10)// �� �����������
			&& (i > 0)//
			&& (list_surplus_moves.get_move(i) != killer.get_killer_moves_1(i))//
			&& (list_surplus_moves.get_move(i) != killer.get_killer_moves_2(i))//
			&& (flag_check == CHECK_NO) 
			&& (old_flag_check == CHECK_NO)//
			&& ((depth_max_new - depth) >= LMR_DEPTH ))//
			{
				alpha_scout = beta - 1;//
				flag_pv_f_scout = PV_NO;//
				depth_max_new_scout = depth_max_new - LMR_REDUCTION_D;// ������������ �������

				value = white(alpha_scout,beta,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
				//
				search_scout = (value < beta);

				depth_max_new_scout = depth_max_new;

			}
#endif//#if  LMR

			if (search_scout)
			{
				alpha_scout = beta - 1;//
				flag_pv_f_scout = PV_NO;//

				value = white(alpha_scout,beta,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
			}

			//if (  (flag_pv_f) && (value < beta) && (i != 0) )
			if ( (flag_pv_f) && (value > alpha) && (value < beta) && (i != 0) )
			{
				alpha_scout = alpha;//
				flag_pv_f_scout = PV_YES;//

				value = white(alpha_scout,beta,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,flag_null_m);
			}

		}

		// ��������������� �������
		bitboard.undo_moves_black(i,depth,list_surplus_moves);

#if SAVE_LINE
		// ��������������� ���-���� �����
		bitboard.set_hash_key(key_undo);

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
#if KILLER
					if (((list_surplus_moves.get_move(i)>>6)& 63) != MOVE_IS_CAPTURE)
					{
						killer.save_killer(i,list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// �������� ������ �� ������ ������ ���
					hash.save_best_hash_score_or_move(i, bitboard.get_hash_key()
													  ,list_surplus_moves, 
													  value, LOWER, depth, depth_max);	
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
				if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 ))
				{
					std::cout << "������ ��������� �� ��� ������� ������" << "\n";
					std::cout << "depth= " << depth << "\n";
					std::cout << "depth_max_rem= " << depth_max_rem << "\n";
					std::cout << "hash_depth= " << (__int32)hash_depth << "\n";
					std::cout << "hash_depth_max= " << (__int32)hash_depth_max 
						<< "\n";

					std::cout << "hash_score= " << hash_score  << "\n";
					std::cout << "value= " << value << "\n";
					std::cout << "alpha= " << alpha << "\n";
					std::cout << "beta= " << beta << "\n";
					std::cout << "  " << "\n";

					exit(1);
				}
#endif//#if HASH_TEST_SCORE
//-------------------

				beta = value;
				flag_save_best = true;
				i_hash_moves_1 = i; //��� ������� �������� ���� (1- �������� ����) 
									//����� ��� ����������� 

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				if (flag_pv_f)
				{
					pv_save.update_PV_best_point(pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			if (flag_pv_f)
			{
				pv_save.update_PV_best_point(pv_best_point);
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
		if (list_surplus_moves.king_black_check(bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best_max_depth(value,depth);
		}
#endif//#if SAVE_LINE 

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� 
		// ����� ������� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	if ( flag_save_best == true ) 
	{
		// �������� ��� ���������� ���� �.�. ���� 1
		hash.save_best_hash_score_or_move(i_hash_moves_1,bitboard.get_hash_key(),list_surplus_moves, 
			beta, EXACT, depth, depth_max);
	}
	else
	{
		// �������� ��� ������ ��������� ������� �.�. ���� 3
		hash.save_best_hash_score_or_move(i_hash_moves_3,bitboard.get_hash_key(),list_surplus_moves, 
			value_min, UPPER, depth, depth_max);
	}
#endif//#if HASH

	return value_min;
}

//	===================================================================
// ����� ����������� ��������� ��������� ��� ������ �������� ����
// � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
// ���� ��� � ��� ���
// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
// ������� ���� ��� ����� ����� ���������
// �� �����
inline __int32 Search::null_move_white
(
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m,// ���� �������� ����
    const unsigned __int8 new_depth,// ��������� �������
	const unsigned __int64 key_undo
)
{

	if ((flag_stop_null_move == NULL_YES) // ���� �������� ��� �� ������
	&& (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
	&& (depth_max > NULL_REDUCTION ) // ����� ������ � ����� � � ��� ������� ������ ������������
	&& ((depth_max - depth) >= NULL_DEPTH ) // ���� �� ������������ ������� ���������� �����������
	&& (flag_pv_f == PV_NO) // ���� �� �� � ������� ��������
	&& (description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
	&& (old_flag_check == CHECK_NO) // ���� ���� ���
	//&& (bitboard- >= beta) // ����� ������ �������������. ������ �� �������
									// �������� ���� � ��� ��� � �� ��������� �����������
									// ������������ ������ � ������ ��������
	)
	{// ����� �������� ������� ���


		// ���������� ����� ������ �� ������� � �������� ����������.
		bool taking_passage_yes_mem = bitboard.get_taking_passage_yes();
		unsigned __int8 taking_passage_x_mem = bitboard.get_taking_passage_x();
		unsigned __int8 taking_passage_y_mem = bitboard.get_taking_passage_y();

        unsigned __int64 hash_key_temp = bitboard.get_hash_key();

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
		bitboard.set_hash_key(hash_key_temp);


		bitboard.set_taking_passage_yes(0);
		bitboard.set_taking_passage_x(0);
		bitboard.set_taking_passage_y(0);

		bitboard.set_colour_move(!bitboard.get_colour_move());

		const __int64 nodes_null_rem = nodes;

		// TEST ���������� ��������� ���-�����
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//zobrist.start_position_random_key(bitboard);

		// �� ������������ ��������
		__int32 value = black(alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES); 

		//--------------------------------------------------------
		// ������������ �������� �������� ���� ��� ��������� �� ������������ �������
		// ������ ������ 5 ���������� 4 ����� 3
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
		//
		//value = Black(bitboard,alpha,(alpha + 1),
		//	(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES); 
		//--------------------------------------------------------

		// ���� ����������� � ������� ������ �� ���������
		nodes = nodes_null_rem;

		// TEST
		//std::cout << "bitboard- "<< bitboard- << "\n";
		//std::cout << "i_move= "<< i_move << "\n";
		//std::cout << "description_move= "<< description_move << "\n";
		//std::cout << "check= "<< check << "\n";
		//std::cout << "       "<< "\n";
		//
		//if (value < beta)
		//{
		//	std::cout << "�������� "<< "\n";
		//	std::cout << "depth_max= "<< depth_max <<"\n";
		//	std::cout << "depth= "<< depth <<"\n";
		//	std::cout << "value= "<< value <<"\n";
		//	std::cout << "beta= "<< beta <<"\n";
		//	std::cout << "       "<< "\n";
		//}
		//--------------------------------------------------------

		bitboard.set_taking_passage_yes(taking_passage_yes_mem);
		bitboard.set_taking_passage_x(taking_passage_x_mem);
		bitboard.set_taking_passage_y(taking_passage_y_mem);

		bitboard.set_colour_move(!bitboard.get_colour_move());

		// ��������������� ���-���� �����
		bitboard.set_hash_key(key_undo);


		// ��� ��� ������ ��� ������� ��� �������� ����� ����������� ������ �.�. �������� ����
		// ����� ���������� ���� ��� ��� ����� ������� ������ ����
		// ������ ����� ��� ������ � ���� �� �� ������ �������� ���� �.�. ���� ������ ������
		// ��� ����� ���� ������ ����� ������ �.�. ����� ��� �� ���������
		// ������ ��� ���� ���� � ����� ����� �� ������ �������� �� ��� ���� � ������� �� ������ 
		if (value >= beta)
		{

			//TEST
			//std::cout << "������� "<< "\n";
			//std::cout << "depth_max= "<< depth_max <<"\n";
			//std::cout << "depth= "<< depth <<"\n";
			//std::cout << "value= "<< value <<"\n";
			//std::cout << "beta= "<< beta <<"\n";
			//std::cout << "       "<< "\n";
			//return beta;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return beta;
			}

			// ������� ������ ��������
			value = white((beta - 1),beta
				,(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
				depth,description_move, 
				PV_NO,old_flag_check,NULL_YES);

			if (value >= beta)
			{
				return beta;
			}
		}
	}

	return -1000000000;
}

//	===================================================================
// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
// ������� ���� ��� ����� ����� ���������
// �� ������
inline __int32 Search::null_move_black
(
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m,// ���� �������� ����
    const unsigned __int8 new_depth,// ��������� �������
	const unsigned __int64 key_undo
)
{

	if ((flag_stop_null_move == NULL_YES) // ���� �������� ��� �� ������
	&& (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
	&& (depth_max > NULL_REDUCTION ) // ����� ������ � ����� � � ��� ������� ������ ������������
	&& ((depth_max - depth) >= NULL_DEPTH ) // ���� �� ������������ ������� ���������� �����������
	&& (flag_pv_f == PV_NO) // ���� �� �� � ������� ��������
	&& (description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
	&& (old_flag_check == CHECK_NO) // ���� ���� ���
	//&& (bitboard- <= alpha) //
	)
	{

		// ����� �������� ������� ���
		const bool taking_passage_yes_mem = bitboard.get_taking_passage_yes();
		const unsigned __int8 taking_passage_x_mem = bitboard.get_taking_passage_x();
		const unsigned __int8 taking_passage_y_mem = bitboard.get_taking_passage_y();


       unsigned __int64 hash_key_temp = bitboard.get_hash_key();

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

		bitboard.set_hash_key(hash_key_temp);

//--------

		bitboard.set_taking_passage_yes(0);
		bitboard.set_taking_passage_x(0);
		bitboard.set_taking_passage_y(0);

		bitboard.set_colour_move(!bitboard.get_colour_move());
	
		const __int64 nodes_null_rem = nodes;

		// TEST ���������� ��������� ���-�����
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//zobrist.start_position_random_key(bitboard);

		__int32 value = white((beta - 1),beta,(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES);

		//--------------------------------------------------------
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 

		//value = White(bitboard,(beta - 1),beta
		//	,(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES);
		//--------------------------------------------------------

		nodes = nodes_null_rem;

		bitboard.set_taking_passage_yes(taking_passage_yes_mem);
		bitboard.set_taking_passage_x(taking_passage_x_mem);
		bitboard.set_taking_passage_y(taking_passage_y_mem);

		bitboard.set_colour_move(!bitboard.get_colour_move());

		// ��������������� ���-���� �����
		bitboard.set_hash_key(key_undo);


		//��� ��� ����� ��� ������� ��� �������� ����� ������������ ������ �.�. �������� �����
		//������ ���������� ���
		//������ ����� ��� ����� � ���� �� �� ������ �������� ����� �.�. ���� ������ ������
		//��� ����� ����� ������ ����� ������ �.�. ����� ��� �� ���������
		if (value <= alpha)
		{ 
			//std::cout << "������� "<< "\n";
			//std::cout << "depth_max= "<< depth_max <<"\n";
			//std::cout << "depth= "<< depth <<"\n";
			//std::cout << "value= "<< value <<"\n";
			//std::cout << "alpha= "<< alpha <<"\n";
			//std::cout << "       "<< "\n";

			//return alpha;
			//------------------------------

			if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1))
			{
				return alpha;
			}

			// ������� ������ ��������
			value = black(alpha,(alpha + 1)
				,(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
				depth,description_move, 
				PV_NO,old_flag_check,NULL_YES);

			if (value <= alpha)
			{
				return alpha;
			}
		}
	}

	return -1000000000;
}


//	===================================================================
// ���������� ������� �� ������������ �������. �� �����
inline __int32 Search::max_depth_white
(
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const unsigned __int8 depth// �������
)
{
	__int32 score = 0;
	__int32 score_f = 0;

	score = evaluate.eval_state() + (bitboard.get_value_white_material() 
		- bitboard.get_value_black_material());
	//score = bitboard-;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(bitboard) != bitboard-)
	//{
	//	std::cout << "WHITE  Eval_forced(bitboard) != bitboard-" <<"\n";
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<"\n";
	//	std::cout << "bitboard-= "<< bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//� ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
	//���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
	if (flag_pv_f)
	{
		pv_save.update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
	//�����  ������ ���� � ����
	//Print_std::cout(list_surplus_moves,pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score ;
}

//	===================================================================
// ���������� ������� �� ������������ �������. �� ������
inline __int32 Search::max_depth_black
(
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const unsigned __int8 depth// �������
)
{
	__int32 score = 0;
	__int32 score_f = 0;

	score = evaluate.eval_state() + (bitboard.get_value_white_material() 
		- bitboard.get_value_black_material());
	//score = bitboard-;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(bitboard) != bitboard-)
	//{
	//	std::cout << "BLACK  Eval_forced(bitboard) != bitboard-" <<"\n";
	//	std::cout << "Eval_forced(bitboard)= "<< EVALUATE::Eval_forced(bitboard)<<"\n";
	//	std::cout << "bitboard-= "<< bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//� ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
	//���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
	if (flag_pv_f)
	{
		pv_save.update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
	//�����  ������ ���� � ����
	//Print_std::cout(list_surplus_moves,pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score;
}

//	===================================================================
// ��������� ��������� �� ����� -
inline void Search::extension_for_chek_white
(
	const unsigned __int8 depth_max,// ������������ �������
	unsigned __int8 & depth_max_new,// ����� ������������ �������
	bool & flag_check,// ���� ����
	class List_surplus_moves & list_surplus_moves// ������ ��������� �����
)
{
	

	// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
	if (list_surplus_moves.king_black_check(bitboard) != CHECK_NO)
	{
		flag_check = CHECK_YES;
		depth_max_new = depth_max + 1;
	}
	else
	{
		flag_check = CHECK_NO;
	}

}

//	===================================================================
// ��������� ��������� �� ����� -
inline void Search::extension_for_chek_black
(
	const unsigned __int8 depth_max,// ������������ �������
	unsigned __int8 & depth_max_new,// ����� ������������ �������
	bool & flag_check,// ���� ����
	class List_surplus_moves & list_surplus_moves// ������ ��������� �����
)
{

	// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
	if(list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
	{
		flag_check = CHECK_YES;
		depth_max_new = depth_max + 1;
	}
	else
	{
		flag_check = CHECK_NO;
	}

}


//	===================================================================
//
void Search::ini_global_loop()
{
	audit = 0;// �������� ������ ������� ����������� ������
	flag_stop_search = 0;//
}

//	===================================================================
// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
// �� ��������� �� �������� �� ������
bool Search::get_stop_search()
{
	return flag_stop_search;
}

//	===================================================================
// ���������� ������ �������
struct PV_line & Search::get_pv_best()
{
	return pv_save.get_pv_best();
}

//	===================================================================
// ���������� ���������� ������������� �����
__int64  Search::get_nodes()
{
	return nodes;// ���������� �����;
}

//	===================================================================
// �������� ���������� ������������� �����
void Search::set_nodes_in_0()
{
	nodes = 0;// ���������� �����;
}

//	===================================================================
//
void Search::ini_deep_time_terminate
(
	double _deep_time_terminate//
)
{
	deep_time_terminate = _deep_time_terminate;
}

//	===================================================================
//
void Search::ini_flag_stop_null_move
(
	bool _flag_stop_null_move//
)
{
	flag_stop_null_move = _flag_stop_null_move;
}

//	===================================================================
//
void Search::set_depth_max_rem
(
	unsigned __int8 _depth_max_rem//
)
{
	depth_max_rem = _depth_max_rem;
}

//	===================================================================
// ������� ������ ���. ���������� �����
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
				std::cout << "Ifrit: quit command. I am terminated." << "\n";
				hash.hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// ��������� ������
				flag_stop_search = 1;
			}
		}

		//���������� ����� � �������. ���������� ����� �� �����
		timer.time_stop();
		delta_time_current = timer.time_delta();

		//������� �� �������
		if ((deep_time_terminate > 0) && (deep_time_terminate < delta_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;

}