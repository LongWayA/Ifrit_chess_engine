#include <iostream> // TEST
#include <fstream>  // TEST
//#include <cstring>    // ���������� ��� ������ �� ������� (��������� �����)
#include <stdlib.h>   // ������������ ���������� ������ �������� exit(1)


#include "Search_quiescence.h"
#include "Evaluate.h"
#include "Win_32.h"
#include "Search.h"
#include "Hash.h"
#include "Killer.h"
#include "PV_save.h"
#include "Timer.h"
#include "ZObrist.h"
#include "Search_switch.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 2.11.2009 20.12.2009 25.07.2010 17.10.2010
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

 ������� �������

 ��� �������� ��������� �������:
 1 � ������ ������������ ������ ���������� �����. ���������� ������ ��� ��� �������� �� ���
   � � ��� ���� ����� ���� ��� ��� ������ ��� ���
 2 ������ �� ��������� ���� ������ ������ ������ �� ���� � ��� �� ���-������� ���� �� ���� � ������
 3 ������ �� ��������� ��� � ��������� ������� �� ��� ���� 
   ��� ���� ��� ������ �� ��� ���������� ����� ���� ������
 4 ������������ ����������
 5 ����� ��������� � ������� ���������� ���
 6 ���� ����� �� ���� �� ��� ������ ���� ��� ����

*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
   ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

class Search_quiescence search_quiescence;

extern class Evaluate evaluate;
extern class Hash hash;
extern class ZObrist zobrist;
extern class Timer timer;
extern class PV_save pv_save;
extern class Killer killer;

class List_surplus_moves search_list_surplus_moves[MAX_DEPTH];// ������ ��������� ����� (������������ ���� �� ������� ����)

//	===================================================================
// ���� �������� � ����. ������� �� ����� 
__int32 Search::white
(
	class Chess_bitboard & bitboard,// ������������� �����
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m// ���� �������� ����
)
{
	__int32 value;              // ������� ������ �������
	__int32 value_max = -9999999;   // ������ ������� ������ � ���� 

	class List_surplus_moves & list_surplus_moves 
		= search_list_surplus_moves[depth];// ������ ��������� ����� (������������ ���� �� ������� ����)

	bool flag_is_legal_moves = 0;           // ���� ������������� ��������� �����
	bool flag_save_best = 0;           // ����� �� ������ �������
	const unsigned __int8 new_depth = depth + 1; // ��������� �������
	bool flag_check = 0;//���� ����

//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//����� ������� ��������
 
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������

	  
	// ��� ��� �����������
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // ���������� ���-���� �������� ����
	unsigned __int8  flag_insert_hash_move = 0; // ��������� �� ��� �� ���-������� �� ������ �������
	__int32 hash_score = -1000000000;// ���-������ �������
	unsigned __int8 i_hash_moves_1;// ��� ������� �������� ����� (1- �������� �����) ����� ��� ����������� 
	unsigned __int8 i_hash_moves_3;// ��� ������ ��������� �������� (3-������ ��������� �������� )����� ��� �����������

#if (LMR && SAVE_LINE)
	unsigned __int8 lmr_reduction;//
#endif//#if (LMR

	//unsigned __int64 test_key_control;
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 
//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

#if HASH_TEST_SCORE
	unsigned __int8 hash_moves_test13;// ��� �����
#endif// #if HASH_TEST_SCORE

#if (HASH_SCORE && SAVE_LINE)
	unsigned __int8 test_flag_hash_moves;// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 hash_depth;// ������������ �������
	unsigned __int8 hash_depth_max;// ������������ �������
#endif//#if (HASH_SCORE)


	// 1
	// ������� �� ������� �� ��������� �����

#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (flag_stop_search == 1)
	{
		return 0;
	}
#endif//#if SAVE_LINE 

	// 2
	// ������� �� ������������ ������� (������ �������)

	// �� ���������� ������� ���������� ����������� ������ �������
	if (depth >= depth_max)
	{

#if SAVE_LINE
		value = max_depth_white(bitboard,alpha,beta,flag_pv_f,depth);
#endif//#if SAVE_LINE 

		return value ;
	}

	// 3
	// ������� �� ���������� �������

#if (REPETITION && ALPHA_BETA_ON && SAVE_LINE)
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
������ ���-�� ��� �� ��������.


*/

	if (!flag_pv_f)
	{ 
		if (((depth_max - depth) < FUTILITY_DEPTH)//
		//&& (description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
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
				value = search_quiescence.forced_variant_white(bitboard
							,alpha,beta,depth,flag_pv_f);

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
					+ evaluate.eval_state(bitboard);

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
	value = null_move_white(bitboard,alpha,beta,depth_max,depth,description_move,
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
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// ������ �� ��������� ��� �� ��� ������� �� ������ ����� ������� ��������� ����
	hash.look_hash_move(list_surplus_moves,bitboard.get_hash_key(),depth,flag_insert_hash_move);

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


#if (LMR && SAVE_LINE)
	// ��� �� ���-�������, ������ ������, ��� �������, ��� ������ �� ������������
	lmr_reduction = FULL_SEARCH + list_surplus_moves.get_end_captures() ; // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


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
		flag_is_legal_moves = 1;
		nodes = nodes + 1;

#if SAVE_LINE
		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		zobrist.modification_random_key(1,i,list_surplus_moves,bitboard,bitboard.get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = bitboard.get_hash_key();

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		zobrist.start_position_random_key(bitboard);

		if (test_key_control != bitboard.get_hash_key())
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision white depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth); 

		// ������ ��� ���������� ����� ������������ ������� ��� �� ���������� ����� 
		// � �� ���� ����
		depth_max_new = depth_max;
#endif//#if SAVE_LINE 

#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� �� 1 ������������ ������� ��������
		extension_for_chek_white(bitboard,depth_max,depth_max_new,flag_check,list_surplus_moves);
#endif//#if EXTENSION_CHEK


#if !SAVE_LINE 
		// ����������� ����� �������
		value = public_black(bitboard,alpha,beta,depth_max,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
			flag_check,flag_null_m);
#endif//#if !SAVE_LINE


#if (!LMR && SAVE_LINE)
		PVS_white(bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check);
#endif//#if !LMR


#if (LMR && SAVE_LINE)
	if (flag_pv_f)
	{
		// ���� �� � ������� �������� �� ������� �� �������� � ������� �����
		// �.�. �� alpha �� beta = alpha + 1

		if (i == 0) 
		{
			// ������ ��� ������� �� ������ ������� � � ������ �����
			// �� ������������ ��� ���� �� ������ �� ������ � ������� ������� ���������
			value = black(bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			// �������� �������� ��������� � ������� �����
			value = black(bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
			// ����� ������� ������� ��� � ����������� ����� �.�. ������� �����
			//  ���� ��� ���� ������� �� ������������� c ������ �����
			if (value > alpha)
			{
				//
				value = black(bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
					PV_YES,flag_check,flag_null_m);
			}
		}

	}
	else//if(flag_pv_f)
	{
		// ���� �� �� � ������� �������� �� ������� �� �������� LMR

		// ����� ���������� ������ ��� �� ��� ������� ������ ��� 
		// ������ ������ ������ ��������� ��� ������� ������ ���� 
		// ������ ������ � ������ ��� ��������� ����
		// ��� ��� ����������� ��� ������� lmr_reduction
		// �� ���������� ������ ���������(�����) ���� ���� ��� �� ��� 
		// �� ���� � ���������� �������
		if ((i > lmr_reduction) 
		&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
		&& ((depth_max_new - depth) >= LMR_DEPTH ))
		{
			// ���� � ��������� ������� � � ������� �����
			value = black(bitboard,alpha,(alpha + 1),(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}
		else
		{
			// ��� ������ ���������� ����� ��� �� ����������� ������� value > alpha
			// ��� ������ ����� ��� �� ���������� �� ���(lmr)
			value = alpha + 1; 
		}

		// ������� �������� ����������� ��������
		// � ������ ���� if((value > alpha) && (value < beta)) ��� ��������� ������ ������ ���
		// ������� �� �������������� � ������ value >= beta � ���������� ������������� ���������
		// ����� ���� value >= beta �� ��� ������ ������ ��������� ���� �������� ���������
		// ������ � ��� value > alpha ��� ����� ��������.

		if (value > alpha)
		{
			//
			value = black(bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}

	}
#endif//#if  LMR

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
				flag_save_best = 1;
				i_hash_moves_1 = i;// ��� ������� �������� ����� (1- �������� �����) ����� ��� ����������� 

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

			flag_save_best = 1;
			i_hash_moves_1 = i;// ��� ������������ ������� ��������
#endif// #if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_max = value;
			i_hash_moves_3 = i;// ��� ������ ��������� �������� (3-������ ��������� ��������)����� ��� �����������

		}
#endif//#if SAVE_LINE 

	}


	// ��������� ������ ����� �� ���� ��������� �����
	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == 0)
	{
#if SAVE_LINE
		value = 0;
		if (list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
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
	if (flag_save_best == 1)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)
	if (flag_save_best == 1)
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
	class Chess_bitboard & bitboard,// ������������� �����
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m// ���� �������� ����
)
{
	__int32 value;              // ������� ������ �������
	__int32 value_min = 9999999;   // ������ ������� ������ � ���� 

	class List_surplus_moves & list_surplus_moves 
		= search_list_surplus_moves[depth];// ������ ��������� ����� (������������ ���� �� ������� ����)

	bool flag_is_legal_moves = 0;           // ���� ������������� ��������� �����
	bool flag_save_best = 0;           // ����� �� ������ �������
	const unsigned __int8 new_depth = depth + 1; // ��������� �������
	bool flag_check = 0;//���� ����


#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//����� ������� ��������
 
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	bool taking_passage_yes_mem = 0;
	unsigned __int8 taking_passage_y_mem = 0;
	unsigned __int8 taking_passage_x_mem = 0;

	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // ���������� ���-���� �������� ����
	__int32 hash_score = -1000000000;// ���-������ �������
	unsigned __int8 i_hash_moves_1;// ��� ������� �������� ���� (1- �������� ����) ����� ��� ����������� 
	unsigned __int8 i_hash_moves_3;// ��� ������ ��������� ������� (3-������ ��������� �������)����� ��� �����������

#if (LMR && SAVE_LINE)
	unsigned __int8 lmr_reduction;//
#endif//#if (LMR

	//unsigned __int64 test_key_control;// ���������� ���-���� �������� ����
	//int dynamic_null_move_reduction;
#endif//#if SAVE_LINE 

#if (HASH_SCORE && SAVE_LINE)
	unsigned __int8 test_flag_hash_moves;// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 hash_depth;// ������������ �������
	unsigned __int8 hash_depth_max;// ������������ �������
#endif//#if (HASH_SCORE)


#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (flag_stop_search == 1)
	{
		return 0;
	}
#endif//#if SAVE_LINE 

	// �� ���������� ������� ���������� ����������� ������ �������
	if (depth >= depth_max)
	{
#if SAVE_LINE
		value = max_depth_black(bitboard,alpha,beta,flag_pv_f,depth);
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
		//&& (description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
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
				value = search_quiescence.forced_variant_black(bitboard
							,alpha,beta,depth,flag_pv_f);

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
					+ evaluate.eval_state(bitboard);

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
	value = null_move_black(bitboard,alpha,beta,depth_max,depth,description_move,
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
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
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


#if (LMR && SAVE_LINE)
	// ��� �� ���-�������, ������ ������, ��� �������, ��� ������ �� ������������
	lmr_reduction = FULL_SEARCH + list_surplus_moves.get_end_captures(); // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


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
		flag_is_legal_moves = 1;
		nodes = nodes + 1;

#if SAVE_LINE
		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		zobrist.modification_random_key(0,i,list_surplus_moves,bitboard,bitboard.get_d_undo(depth));

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = bitboard.get_hash_key();

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		zobrist.start_position_random_key(bitboard);

		if (test_key_control != bitboard.get_hash_key())
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision black depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth); 

		depth_max_new = depth_max;
#endif//#if SAVE_LINE 


#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
		extension_for_chek_black(bitboard,depth_max,depth_max_new,flag_check,list_surplus_moves);
#endif//#if EXTENSION_CHEK


#if !SAVE_LINE
		// ����������� ����� �������
		value = public_white(bitboard,alpha,beta,depth_max,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),PV_YES,flag_check,
			flag_null_m);
#endif//#if !SAVE_LINE


#if (!LMR && SAVE_LINE)
		PVS_black(bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check);
#endif//#if (!LMR && SAVE_LINE)


#if (LMR && SAVE_LINE)
	if(flag_pv_f)
	{
		// ���� �� � ������� �������� �� ������� �� �������� � ������� �����
		if (i == 0)
		{
			//
			value = white(bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			value = white(bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			if (value < beta)
			{
				value = white(bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6)& 63),
				PV_YES,flag_check,flag_null_m);
			}
		}
	}
	else
	{
		// ���� �� �� � ������� �������� �� ������� �� �������� LMR
		if ((i > lmr_reduction)
		&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
		&& ((depth_max_new - depth) >= LMR_DEPTH )) 
		{
			value = white(bitboard,(beta - 1),beta,(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}
		else
		{
			value = beta - 1;
		}

		if (value < beta)
		{
			value = white(bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6)& 63),
				PV_NO,flag_check,flag_null_m);
		}
	}
#endif//#if  LMR

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
					hash.save_best_hash_score_or_move(i, bitboard.get_hash_key(), list_surplus_moves, 
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
				flag_save_best = 1;
				i_hash_moves_1 = i;// ��� ������� �������� ���� (1- �������� ����) ����� ��� ����������� 

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

			flag_save_best = 1;
			i_hash_moves_1 = i;// ��� ������������ ������� ��������
#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			value_min = value;
			i_hash_moves_3 = i;// ��� ������ ��������� ������� (3-������ ��������� �������)����� ��� �����������

		}
#endif//#if SAVE_LINE 

	}

	//-----------------------------------------------
	// ��������� ������ ����� �� ���� ��������� �����
	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == 0)
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
	if (flag_save_best == 1)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
		if (flag_pv_f)
		{
			pv_save.update_PV_best(pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	if ( flag_save_best == 1 ) 
	{
		// �������� ��� ���������� ���� �.�. ���� 1
		hash.save_best_hash_score_or_move(i_hash_moves_1, bitboard.get_hash_key(), list_surplus_moves, 
			beta, EXACT, depth, depth_max);
	}
	else
	{
		// �������� ��� ������ ��������� ������� �.�. ���� 3
		hash.save_best_hash_score_or_move(i_hash_moves_3, bitboard.get_hash_key(), list_surplus_moves, 
			value_min, UPPER, depth, depth_max);
	}
#endif//#if HASH

	return value_min;
}

//	===================================================================

// �� �����
inline void Search::PVS_white
(
	class Chess_bitboard & bitboard,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m,// ���� �������� ����
    const unsigned __int8 new_depth,// ��������� �������
	const unsigned __int64 key_undo,
	const unsigned __int8 i,
	__int32 & value,// ������� ������ �������
	const unsigned __int8 depth_max_new,
    class List_surplus_moves & list_surplus_moves,
	const bool flag_check
)
{

#if (ALPHA_BETA_ON && SAVE_LINE)
	if (i == 0)
#else
	if (1)
#endif//#if ALPHA_BETA_ON
	{
		value = black(bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.get_move(i)>>6) & 63),
			flag_pv_f,flag_check,flag_null_m);
	}
	else
	{
		// ����������� ����� �������
		value = black(bitboard,alpha,(alpha + 1),depth_max_new,

		// TEST
		//value = Black(bitboard,alpha,beta,depth_max_new,

		new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
			PV_NO,flag_check,flag_null_m);

		// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
		// ����� ������� ������� ���
		// � ����������� ����� ����� ���� ��� ���� ������� 
		// �� ������������� �� �������
		if ((value > alpha) && (value < beta))
		{ 
			//std::cout << "white depth = " << depth << "\n";
			//std::cout << "value = " << value << "\n";
			//std::cout << "alpha = " << alpha << "\n";
			//std::cout << "beta = " << beta << "\n";

			value = black(bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
				flag_pv_f,flag_check,flag_null_m);

			//std::cout << "value mod = " << value << "\n";
			//std::cout << "   " <<"\n";

		}
	}
}


//	===================================================================

//  �� ������
inline void Search::PVS_black
(
	class Chess_bitboard & bitboard,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth_max,// ������������ �������
	const unsigned __int8 depth,// �������
	const __int32 description_move,// ��� ���� ������� ������ � ������ �������
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const bool old_flag_check,//���� ����
	const bool flag_null_m,// ���� �������� ����
    const unsigned __int8 new_depth,// ��������� �������
	const unsigned __int64 key_undo,
	const unsigned __int8 i,
	__int32 & value,// ������� ������ �������
	const unsigned __int8 depth_max_new,
    class List_surplus_moves & list_surplus_moves,
	const bool flag_check
)
{

	// ����������� ����� �������	
	//depth_max_z
#if ALPHA_BETA_ON
	if (i == 0)
#else
	if (1)
#endif//#if ALPHA_BETA_ON
	{
		value = white(bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.get_move(i)>>6) & 63),flag_pv_f,
			flag_check,flag_null_m);

	}
	else
	{
		value = white(bitboard,(beta - 1),beta,depth_max_new,

		// TEST
		//value = White(bitboard,alpha,beta,depth_max_new,

			new_depth,((list_surplus_moves.get_move(i)>>6) & 63),
			PV_NO,flag_check,flag_null_m);

		if ((value > alpha) && (value < beta))
		{
			value = white(bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.get_move(i)>>6)& 63),
				flag_pv_f,flag_check,flag_null_m);

		}
	}
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
	class Chess_bitboard & bitboard,// ������������� �����
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
			hash_key_temp = hash_key_temp ^ 0xA3853B80F17F58EEui64;
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ 0x5B23E522E4B1B4EFui64);

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ 0x934E1DE72D36D310ui64);
		}

		// ��������� ���� ����
		// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
		// ��������� ������� ���������� ������ ������ ����
		hash_key_temp = hash_key_temp ^ 0x7C0228DD624EC390ui64;
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
		__int32 value = black(bitboard,alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
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
			value = white(bitboard,(beta - 1),beta
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
	class Chess_bitboard & bitboard,// ������������� �����
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
			hash_key_temp = hash_key_temp ^ 0xA3853B80F17F58EEui64;
			hash_key_temp = hash_key_temp 
				^ (taking_passage_x_mem ^ 0x5B23E522E4B1B4EFui64);

			hash_key_temp = hash_key_temp 
				^ (taking_passage_y_mem ^ 0x934E1DE72D36D310ui64);
		}

		// ��������� ���� ����
		// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
		// ��������� ������� ���������� ������ ������ ����
		hash_key_temp = hash_key_temp ^ 0x7C0228DD624EC390ui64;

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

		__int32 value = white(bitboard,(beta - 1),beta,(depth_max - NULL_REDUCTION),
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
			value = black(bitboard,alpha,(alpha + 1)
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
	class Chess_bitboard & bitboard,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const unsigned __int8 depth// �������
)
{
	__int32 score = 0;
	__int32 score_f = 0;
	double deep_time_current;

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
		deep_time_current = timer.time_delta();

		//������� �� �������
		if ((deep_time_terminate > 0) && (deep_time_terminate < deep_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;
	//-----------------------------------------------

	//------------------------------------------------------
	// ����������
	//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	// ���������� ������������� �������
	// �� ��� ����������� �� ���� ��� ��� ��� ������� �������� �� ��
#if FORCED // �������� ����� �������� ������ ������
	score = search_quiescence.forced_variant_white(bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;
#endif//#if FORCED


	// �������� ������ ��� ����������(�������� ������ ������)
#if !FORCED 
	score = evaluate.eval_state(bitboard) + (bitboard.get_value_white_material() 
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

#endif//#if !FORCED 

}

//	===================================================================
// ���������� ������� �� ������������ �������. �� ������
inline __int32 Search::max_depth_black
(
	class Chess_bitboard & bitboard,// ������������� �����
	const __int32 alpha,//
	const __int32 beta,//
	const bool flag_pv_f,// �������� �� ��� ������� (pv)
	const unsigned __int8 depth// �������
)
{
	__int32 score = 0;
	__int32 score_f = 0;
	double deep_time_current;

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

		// ���������� ����� � �������. ���������� ����� �� �����
		timer.time_stop();
		deep_time_current = timer.time_delta();

		// ������� �� �������
		if ((deep_time_terminate  > 0) && (deep_time_terminate < deep_time_current))
		{
			flag_stop_search = 1;
		}

		audit = 0;
	}

	audit = audit + 1;
	//-----------------------------------------------

	//------------------------------------------------------
	//  ����������
	//  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	//  ���������� ������������� �������
	//  �� ��� ����������� �� ���� ��� ��� ��� ������� �������� �� ��
#if FORCED // �������� ����� ����������(�������� ������ ������)

	score = search_quiescence.forced_variant_black(bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;

#endif//#if FORCED 


	// �������� ������ ��� ����������(�������� ������ ������)
#if !FORCED
	score = evaluate.eval_state(bitboard) + (bitboard.get_value_white_material() 
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

#endif//#if !FORCED 

}

//	===================================================================
// ��������� ��������� �� ����� -
inline void Search::extension_for_chek_white
(
	const class Chess_bitboard & bitboard,// ������������� �����
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
	const class Chess_bitboard & bitboard,// ������������� �����
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
	double deep_time_terminate_v//
)
{
	deep_time_terminate = deep_time_terminate_v;
}

//	===================================================================
//
void Search::ini_flag_stop_null_move
(
	bool flag_stop_null_move_v//
)
{
	flag_stop_null_move = flag_stop_null_move_v;
}

//	===================================================================
//
void Search::set_depth_max_rem
(
	unsigned __int8 depth_max_rem_v//
)
{
	depth_max_rem = depth_max_rem_v;
}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������������ �������, ������� ��������, �����,
#if TEST_L

void Search::list_print
(
	int depth,// ������� �������
	class List_surplus_moves & list_surplus_moves// ������ �����
)
{
	Test_Loop.open ("_TEST_LOOP.txt",ios::app);	
	//Test_Loop.open ("_TEST_LOOP.txt");	
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "------------------------------------------------- "<<"\n";
	Test_Loop <<"depth= "<< depth << "\n";
	Test_Loop << "\n";
	Test_Loop << "������ ��������� ����� ������ "<<"\n";

	for (int i = 0 ; i < list_surplus_moves.end_list ; i++)
	{	
		Test_Loop << "����� ���� �� ������ i= " << i << "\n";  
		Test_Loop << "������� ������ " << util_o.public_int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
		Test_Loop << "������ ���� " << list_surplus_moves.sorting_score[i] << "\n";

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Loop << " " << (util_o.public_x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.move[i]>>12)& 63)/8);//<< "\n";
		//Test_Loop<< "\n";
		Test_Loop << " " << (util_o.public_x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.move[i]>>18)& 63)/8)<< "\n";   
		//------------------------------
		//��� ���� 
		//0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
		//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
		//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
		//25 - ������ � ������������ � ����� 
		Test_Loop << "��� ���� " << ((list_surplus_moves.move[i]>>6)& 63)  << "\n";
		Test_Loop << "������ ������ " << util_o.public_int_to_char_board(( list_surplus_moves.move[i] & 63),1)<< "\n";
		Test_Loop << "��������� ��������� " << ((list_surplus_moves.move[i]>>12)& 63)  << "\n";
		Test_Loop << "�������� ��������� " << ((list_surplus_moves.move[i]>>18)& 63)  << "\n";
     
		Test_Loop << "\n";
	}

	Test_Loop << "list_surplus_moves.end_captures= " <<list_surplus_moves.end_captures << "\n";
	Test_Loop << "list_surplus_moves.end_list= " <<list_surplus_moves.end_list << "\n";
	Test_Loop << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop.close();
}
#endif