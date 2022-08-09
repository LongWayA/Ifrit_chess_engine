#include <iostream> // TEST
#include <fstream>  // TEST
//#include <cstring>    // ���������� ��� ������ �� ������� (��������� �����)
#include <stdlib.h>   // ������������ ���������� ������ �������� exit(1)


#include "k_Util.h"
#include "e_Move.h"
#include "h_Evaluate.h"
#include "i_Quiescence_search.h"
#include "k_Win_32.h"
#include "g_Search.h"
#include "j_Hash.h"
#include "j_Killer.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"
#include "j_ZObrist.h"
#include "g_Search_switch.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 2.11.2009 20.12.2009
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


namespace 
{
	// NULL MOVE
	const unsigned __int8 NULL_YES = 1; // �� ������ �������� ���� 
	const unsigned __int8 NULL_NO = 0; // �� ������� �������� ���� 
	const unsigned __int8 NULL_DEPTH = 2; // ��������� �� ��� ������ ������� NULL �� ����������
	const unsigned __int8 SWITCH_NULL_REDUCTION_1_2 = 5; // ������� �� ������� �������� �������� �������� ���� 
										// � NULL_REDUCTION_ONE �� NULL_REDUCTION_TWO
	const unsigned __int8 NULL_REDUCTION = 4; // 4 ���������� ������� �������� ��� NULL
	const unsigned __int8 NULL_REDUCTION_ONE = 4; // 4 ���������� ������� �������� ��� ������������� NULL
	const unsigned __int8 NULL_REDUCTION_TWO = 3; // 3 ���������� ������� �������� ��� ������������� NULL
	const unsigned __int8 VERIFICATOIN_SEARCH_REDUCTION = 5; // ���������� ������� �������� 
											//��� ������ �� ����������� ���������
	// LMR
	const unsigned __int8 LMR_DEPTH = 3; // ��������� �� ��� ������ ������� LMR �� ����������
	const unsigned __int8 LMR_REDUCTION_D = 1; // ���������� ������� �������� ��� LMR
	const unsigned __int8 FULL_SEARCH = 2;  // ���� 2
											// ����� ����� � ������ ������� ��� ���������� 
											// ��� �� ���-�������, ������ ������,
											// ��� �������, ��� ������ �� ������������
											// ���-��� ��� ������� ��� 3 � ����� ���� ������ ��� � ���� 
											// ���� ��� ������� ��� ������� �������� i > lmr_reduction

	// ���������
	const unsigned __int8 CHECK_YES = 1; // ���� ���
	const unsigned __int8 CHECK_NO = 0; // ��� ����


	const unsigned __int8 MOVE_IS_CAPTURE = 2; // ��� ��� ������
	const unsigned __int8 CAPTURE_NO = 0; // ��� ������

	const unsigned __int8 PV_YES = 1; // �������� �������
	const unsigned __int8 PV_NO = 0; // 

	const  __int32 VALUE_MATE = 1000000; // ������ ����
	const  __int16 DELTA_MATE = 1000; // ������ ��� ���������� ���������� ����

	//-------------------------------------------------------------------------

	unsigned __int16 audit;// �������� ������ ������� ����������� ������
	unsigned __int8 depth_max_rem;//
	bool flag_stop_search;// ���������� ��� ������� �������� ��� ���������� ������
	__int64 nodes;// ���������� �����
	char string_in2[65536];//
	unsigned __int8 from;// ������ ����� ������
	unsigned __int8 to;// ���� ����� ������
	bool flag_stop_null_move;// ������������� ������������� �������� ����
	//int return_hash_score;

	double deep_time_terminate; // �������� �� ������� ����� ��������� ����� 
}


//	===================================================================
// ���� �������� � ����. ������� �� ����� 
__int32 FULLSEARCH::public_white
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
	struct T_undo undo[1]; // ��������� ��� �������� ����
	struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
	bool flag_is_legal_moves = 0;           // ���� ������������� ��������� �����
	bool flag_save_best = 0;           // ����� �� ������ �������
	const unsigned __int8 new_depth = depth + 1; // ��������� �������
	bool flag_check = 0;//���� ����

//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//����� ������� ��������
 
	struct T_PV pv_best_point;// ������ ��� ������� ���� �������

	  
	// ��� ��� �����������
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
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
		value = max_depth_white(p_bitboard,alpha,beta,flag_pv_f,depth);
#endif//#if SAVE_LINE 

		return value ;
	}

	// 3
	// ������� �� ���������� �������

#if (REPETITION && ALPHA_BETA_ON && SAVE_LINE)
	// ���������� ���������� �������
	if (ZOBRIST::public_analysis_hash_string_moves(depth))
	{
		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(0 // ������ �����
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
		hash_score = HASHM::public_look_hash_score_white(
			p_bitboard->hash_key,depth,depth_max,
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
	// ������� �� �������� ����

#if (NULL_MOVE && SAVE_LINE)
	// ����� ����������� ��������� ��������� ��� ������ �������� ����
	// � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
	// ���� ��� � ��� ���
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	value = null_move_white(p_bitboard,alpha,beta,depth_max,depth,description_move,
		flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE




	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	BITBOARDGENERATOR::public_generator_captures_white(p_bitboard,&list_surplus_moves);
	BITBOARDGENERATOR::public_generator_move_white(p_bitboard,&list_surplus_moves);
	sorting_moves(&list_surplus_moves);

 
#if (KILLER && SAVE_LINE)
	// ���� ��� ������ ��� ������ ������
	// �� ������ ��������� �� ������ ������� ����� �� ������
	if ((list_surplus_moves.end_captures == CAPTURE_NO) 
	|| (list_surplus_moves.sorting_score[0] < 0))
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,0);
	}
	else
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// ������ �� ��������� ��� �� ��� ������� �� ������ ����� ������� ��������� ����
	HASHM::public_look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

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
	lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


	//����� �� ������ ���������� �����
	// ����� � ����� ����� ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

		// ��������� ��� �� ������ ������� ������� ���������� �������
		MOVES::public_onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);

		// ���� ������ ��� ����� �� 
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

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
			PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);
		}

		ZOBRIST::public_modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = p_bitboard->hash_key;

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		ZOBRIST::public_start_position_random_key(p_bitboard);

		if (test_key_control != p_bitboard->hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision white depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth); 

		// ������ ��� ���������� ����� ������������ ������� ��� �� ���������� ����� 
		// � �� ���� ����
		depth_max_new = depth_max;
#endif//#if SAVE_LINE 

#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� �� 1 ������������ ������� ��������
		extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK


#if !SAVE_LINE 
		// ����������� ����� �������
		value = public_black(p_bitboard,alpha,beta,depth_max,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
			flag_check,flag_null_m);
#endif//#if !SAVE_LINE


#if (!LMR && SAVE_LINE)
		PVS_white(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check);
#endif//#if !LMR


#if (LMR && SAVE_LINE)
		LMR_white(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check,lmr_reduction);
#endif//#if  LMR

		// ��������������� �������
		MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

#if SAVE_LINE
		// ��������������� ���-���� �����
		p_bitboard->hash_key = key_undo;

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
					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
						SKILLER::public_save_killer(i,&list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// �������� ������ �� ������ ������ ���
					HASHM::public_save_best_hash_score_or_move(i
						, p_bitboard->hash_key, &list_surplus_moves, 
						value, HASHM::LOWER, depth, depth_max);
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
					PVSAVE::public_update_PV_best_point(&pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_point(&pv_best_point);
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
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(value,depth);
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
			PVSAVE::public_update_PV_best(&pv_best_point);
		}
	}
#endif//#if SAVE_LINE 

#if (HASH && SAVE_LINE)
	if (flag_save_best == 1)
	{
		// �������� ��� ���������� ����� �.�. ���� 1
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_1
			, p_bitboard->hash_key, &list_surplus_moves, 
			alpha, HASHM::EXACT, depth, depth_max);
	}
	else
	{
		// �������� ��� ������ ��������� �������� �.�. ���� 3
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_3
			, p_bitboard->hash_key, &list_surplus_moves, 
			value_max, HASHM::UPPER, depth, depth_max);
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
			std::cout << "p_bitboard->hash_key= " << p_bitboard->hash_key << "\n";
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
				<< ((list_surplus_moves.move[0]>>6)& 63) << "\n";

			std::cout << "������� ������ " 
				<< UTIL::public_int_to_char_board(((list_surplus_moves.move[0]>>24)& 7),1)
				<< "\n";
               
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[0]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>12)& 63)/8);
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[0]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[0]>>18)& 63)/8) << "\n";

			std::cout << "��� ��������� � ������ " << "\n";
			std::cout << "hash_moves_test12= " << hash_moves_test13  << "\n";
			std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				<< ((list_surplus_moves.move[hash_moves_test13]>>6)& 63) << "\n";
			std::cout << "������� ������ " 
				<< UTIL::public_int_to_char_board(((
					list_surplus_moves.move[hash_moves_test13]>>24)& 7),1)<< "\n";
               
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>12)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>12)& 63)/8);
			std::cout <<UTIL::public_x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>18)& 63)%8);
			std::cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>18)& 63)/8) << "\n";
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
__int32 FULLSEARCH::public_black
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
	struct T_undo undo[1];
	struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
	bool flag_is_legal_moves = 0;           // ���� ������������� ��������� �����
	bool flag_save_best = 0;           // ����� �� ������ �������
	const unsigned __int8 new_depth = depth + 1; // ��������� �������
	bool flag_check = 0;//���� ����


#if SAVE_LINE
	unsigned __int8 depth_max_new = 0;//����� ������� ��������
 
	struct T_PV pv_best_point;// ������ ��� ������� ���� �������
	bool taking_passage_yes_mem = 0;
	unsigned __int8 taking_passage_y_mem = 0;
	unsigned __int8 taking_passage_x_mem = 0;

	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
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
		value = max_depth_black(p_bitboard,alpha,beta,flag_pv_f,depth);
#endif//#if SAVE_LINE 

		return value ;
	}


#if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
	// ���������� ���������� �������
	if (ZOBRIST::public_analysis_hash_string_moves(depth))
	{
		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(0 // ������ �����
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
		hash_score = HASHM::public_look_hash_score_black(
			p_bitboard->hash_key,depth,depth_max,
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


#if (NULL_MOVE && SAVE_LINE)
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	value = null_move_black(p_bitboard,alpha,beta,depth_max,depth,description_move,
		flag_pv_f,old_flag_check,flag_null_m,new_depth,key_undo);

	if (value != -1000000000 )
	{
		return value;
	}
#endif//#if NULL_MOVE


	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	BITBOARDGENERATOR::public_generator_captures_black(p_bitboard,&list_surplus_moves);
	BITBOARDGENERATOR::public_generator_move_black(p_bitboard,&list_surplus_moves);
	sorting_moves(&list_surplus_moves);


#if (KILLER && SAVE_LINE)
	// ���� ��� ������ ��� ������ �������������
	// �� ������ ��������� �� ������ ������� ����� �� ������
	if ((list_surplus_moves.end_captures == CAPTURE_NO)
	|| (list_surplus_moves.sorting_score[0] < 0))
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,0);
	}
	else
	{
		SKILLER::public_insert_killer(&list_surplus_moves,depth,1);
	}
#endif//#if KILLER


#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	HASHM::public_look_hash_move(&list_surplus_moves
		,p_bitboard->hash_key,depth,flag_insert_hash_move);

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
	lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
	//lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR


	// ����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

		// ��������� ��� �� ������ ������� ������� ���������� �������
		MOVES::public_onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);

		// ���� ������ �� ����� ��
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard)!= CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

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
			PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);
		}

		ZOBRIST::public_modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY
		unsigned __int64 test_key_control = p_bitboard->hash_key;

		//��� ���� ��������� ��������� �� �����
		//���������� ���-�������
		//�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		ZOBRIST::public_start_position_random_key(p_bitboard);

		if (test_key_control != p_bitboard->hash_key)
		{
			std::cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
			std::cout << "collision black depth = " << depth << "\n";
			exit(1);
		}
#endif//#if HASH_TEST_KEY 
//----------------------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth); 

		depth_max_new = depth_max;
#endif//#if SAVE_LINE 


#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
		extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK


#if !SAVE_LINE
		// ����������� ����� �������
		value = public_white(p_bitboard,alpha,beta,depth_max,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),PV_YES,flag_check,
			flag_null_m);
#endif//#if !SAVE_LINE


#if (!LMR && SAVE_LINE)
		PVS_black(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check);
#endif//#if (!LMR && SAVE_LINE)


#if (LMR && SAVE_LINE)
		LMR_black(p_bitboard,alpha,beta,depth_max,depth,description_move,flag_pv_f,
			old_flag_check,flag_null_m,new_depth,key_undo,i,value,depth_max_new,
			list_surplus_moves,flag_check,lmr_reduction);
#endif//#if  LMR

		// ��������������� �������
		MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

#if SAVE_LINE
		// ��������������� ���-���� �����
		p_bitboard->hash_key = key_undo;

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
					if (((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
					{
						SKILLER::public_save_killer(i,&list_surplus_moves,depth);
					}
#endif//#if KILLER

#if HASH
					// �������� ������ �� ������ ������ ���
					HASHM::public_save_best_hash_score_or_move(i, p_bitboard->hash_key, &list_surplus_moves, 
						value, HASHM::LOWER, depth, depth_max);	
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
					PVSAVE::public_update_PV_best_point(&pv_best_point);
				}
			}
#else // #if ALPHA_BETA_ON
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			if (flag_pv_f)
			{
				PVSAVE::public_update_PV_best_point(&pv_best_point);
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
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		if (flag_pv_f)
		{
			PVSAVE::public_update_PV_best_max_depth(value,depth);
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
			PVSAVE::public_update_PV_best(&pv_best_point);
		}
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	if ( flag_save_best == 1 ) 
	{
		// �������� ��� ���������� ���� �.�. ���� 1
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_1, p_bitboard->hash_key, &list_surplus_moves, 
			beta, HASHM::EXACT, depth, depth_max);
	}
	else
	{
		// �������� ��� ������ ��������� ������� �.�. ���� 3
		HASHM::public_save_best_hash_score_or_move(i_hash_moves_3, p_bitboard->hash_key, &list_surplus_moves, 
			value_min, HASHM::UPPER, depth, depth_max);
	}
#endif//#if HASH

	return value_min;
}

//	===================================================================

// �� �����
inline void FULLSEARCH::PVS_white
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
    const struct T_list_surplus_moves list_surplus_moves,
	const bool flag_check
)
{

#if (ALPHA_BETA_ON && SAVE_LINE)
	if (i == 0)
#else
	if (1)
#endif//#if ALPHA_BETA_ON
	{
		value = public_black(p_bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),
			flag_pv_f,flag_check,flag_null_m);
	}
	else
	{
		// ����������� ����� �������
		value = public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,

		// TEST
		//value = Black(p_bitboard,alpha,beta,depth_max_new,

		new_depth,((list_surplus_moves.move[i]>>6) & 63),
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

			value = public_black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				flag_pv_f,flag_check,flag_null_m);

			//std::cout << "value mod = " << value << "\n";
			//std::cout << "   " <<"\n";

		}
	}
}


//	===================================================================

//  �� ������
inline void FULLSEARCH::PVS_black
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
    const struct T_list_surplus_moves list_surplus_moves,
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
		value = public_white(p_bitboard,alpha,beta,depth_max_new,new_depth,
			((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
			flag_check,flag_null_m);

	}
	else
	{
		value = public_white(p_bitboard,(beta - 1),beta,depth_max_new,

		// TEST
		//value = White(p_bitboard,alpha,beta,depth_max_new,

			new_depth,((list_surplus_moves.move[i]>>6) & 63),
			PV_NO,flag_check,flag_null_m);

		if ((value > alpha) && (value < beta))
		{
			value = public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6)& 63),
				flag_pv_f,flag_check,flag_null_m);

		}
	}
}

//	===================================================================

// �� �����
inline void FULLSEARCH::LMR_white
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
    const struct T_list_surplus_moves list_surplus_moves,
	const bool flag_check,
	const unsigned __int8 lmr_reduction
)
{
	if (flag_pv_f)
	{
		// ���� �� � ������� �������� �� ������� �� �������� � ������� �����
		// �.�. �� alpha �� beta = alpha + 1

		if (i == 0) 
		{
			// ������ ��� ������� �� ������ ������� � � ������ �����
			// �� ������������ ��� ���� �� ������ �� ������ � ������� ������� ���������
			value = public_black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			// �������� �������� ��������� � ������� �����
			value = public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
			// ����� ������� ������� ��� � ����������� ����� �.�. ������� �����
			//  ���� ��� ���� ������� �� ������������� c ������ �����
			if (value > alpha)
			{
				//
				value = public_black(p_bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.move[i]>>6) & 63),
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
			value = public_black(p_bitboard,alpha,(alpha + 1),(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
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
			value = public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}

	}
}


//	===================================================================

//  �� ������
inline void FULLSEARCH::LMR_black
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
    const struct T_list_surplus_moves list_surplus_moves,
	const bool flag_check,
	const unsigned __int8 lmr_reduction
)
{
	if(flag_pv_f)
	{
		// ���� �� � ������� �������� �� ������� �� �������� � ������� �����

		if (i == 0)
		{
			//
			value = public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,flag_null_m);
		}
		else
		{
			value = public_white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);

			if (value < beta)
			{
				value = public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6)& 63),
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
			value = public_white(p_bitboard,(beta - 1),beta,(depth_max_new - LMR_REDUCTION_D),
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,flag_null_m);
		}
		else
		{
			value = beta - 1;
		}

		if (value < beta)
		{
			value = public_white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6)& 63),
				PV_NO,flag_check,flag_null_m);
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
inline __int32 FULLSEARCH::null_move_white
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
	//&& (p_bitboard- >= beta) // ����� ������ �������������. ������ �� �������
									// �������� ���� � ��� ��� � �� ��������� �����������
									// ������������ ������ � ������ ��������
	)
	{// ����� �������� ������� ���

		// ���������� ����� ������ �� ������� � �������� ����������.
		bool taking_passage_yes_mem = p_bitboard->taking_passage_yes;
		unsigned __int8 taking_passage_x_mem = p_bitboard->taking_passage_x;
		unsigned __int8 taking_passage_y_mem = p_bitboard->taking_passage_y;

		// ������ ���-���� ��� ������� ��� -------------------------
		// ��������� ������ �� �������
		if (p_bitboard->taking_passage_yes)
		{
			p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));

			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
		}

		// ��������� ���� ����
		// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
		// ��������� ������� ���������� ������ ������ ����
		p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);

		p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

		p_bitboard->colour_move = !p_bitboard->colour_move;

		const __int64 nodes_null_rem = nodes;

		// TEST ���������� ��������� ���-�����
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(p_bitboard);

		// �� ������������ ��������
		__int32 value = public_black(p_bitboard,alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES); 

		//--------------------------------------------------------
		// ������������ �������� �������� ���� ��� ��������� �� ������������ �������
		// ������ ������ 5 ���������� 4 ����� 3
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
		//
		//value = Black(p_bitboard,alpha,(alpha + 1),
		//	(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES); 
		//--------------------------------------------------------

		// ���� ����������� � ������� ������ �� ���������
		nodes = nodes_null_rem;

		// TEST
		//std::cout << "p_bitboard- "<< p_bitboard- << "\n";
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

		p_bitboard->taking_passage_yes = taking_passage_yes_mem;
		p_bitboard->taking_passage_x = taking_passage_x_mem ;
		p_bitboard->taking_passage_y = taking_passage_y_mem ;

		p_bitboard->colour_move = !p_bitboard->colour_move;

		// ��������������� ���-���� �����
		p_bitboard->hash_key = key_undo;

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
			value = public_white(p_bitboard,(beta - 1),beta
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
inline __int32 FULLSEARCH::null_move_black
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
	//&& (p_bitboard- <= alpha) //
	)
	{
		// ����� �������� ������� ���
		bool taking_passage_yes_mem = p_bitboard->taking_passage_yes;
		unsigned __int8 taking_passage_x_mem = p_bitboard->taking_passage_x;
		unsigned __int8 taking_passage_y_mem = p_bitboard->taking_passage_y;

		// ������ ���-���� ��� ������� ��� -------------------------
		// ��������� ������ �� �������
		if (p_bitboard->taking_passage_yes)
		{
			p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));

			p_bitboard->hash_key = p_bitboard->hash_key 
				^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
		}

		// ��������� ���� ����
		// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
		// ��������� ������� ���������� ������ ������ ����
		p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);

		//---------------------------------------------
		p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

		p_bitboard->colour_move = !p_bitboard->colour_move;
	
		const __int64 nodes_null_rem = nodes;

		// TEST ���������� ��������� ���-�����
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(p_bitboard);

		__int32 value = public_white(p_bitboard,(beta - 1),beta,(depth_max - NULL_REDUCTION),
			new_depth,description_move, 
			PV_NO,old_flag_check,NULL_YES);

		//--------------------------------------------------------
		//dynamic_null_move_reduction = 
		//	((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
		//	? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 

		//value = White(p_bitboard,(beta - 1),beta
		//	,(depth_max - dynamic_null_move_reduction),
		//	new_depth,description_move, 
		//	PV_NO,flag_check,NULL_YES);
		//--------------------------------------------------------

		nodes = nodes_null_rem;

		p_bitboard->taking_passage_yes = taking_passage_yes_mem;
		p_bitboard->taking_passage_x = taking_passage_x_mem ;
		p_bitboard->taking_passage_y = taking_passage_y_mem ;

		p_bitboard->colour_move = !p_bitboard->colour_move;

		// ��������������� ���-���� �����
		p_bitboard->hash_key = key_undo;


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
			value = public_black(p_bitboard,alpha,(alpha + 1)
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
inline __int32 FULLSEARCH::max_depth_white
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
				HASHM::public_hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// ��������� ������
				flag_stop_search = 1;
			}
		}

		//���������� ����� � �������. ���������� ����� �� �����
		TIMER::public_time_stop();
		deep_time_current = TIMER::public_time_delta();

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
	score = QUIESCENCESEARCH::public_forced_variant_white(p_bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;
#endif//#if FORCED


	// �������� ������ ��� ����������(�������� ������ ������)
#if !FORCED 
	score = EVALUATE::public_eval_state(p_bitboard) + (p_bitboard->value_white_material - p_bitboard->value_black_material);
	//score = p_bitboard-;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(p_bitboard) != p_bitboard-)
	//{
	//	std::cout << "WHITE  Eval_forced(p_bitboard) != p_bitboard-" <<"\n";
	//	std::cout << "Eval_forced(p_bitboard)= "<< EVALUATE::Eval_forced(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard-= "<< p_bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//� ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
	//���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
	if (flag_pv_f)
	{
		PVSAVE::public_update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
	//�����  ������ ���� � ����
	//Print_std::cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score ;

#endif//#if !FORCED 

}

//	===================================================================
// ���������� ������� �� ������������ �������. �� ������
inline __int32 FULLSEARCH::max_depth_black
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
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
			//char string_in2[65536];
			std::cin.getline(string_in2,LENGTH_STRING, '\n');

			if (strcmp(string_in2, "quit") == 0)
			{
				HASHM::public_hash_size_free();
				exit(1);
			}

			if ((strcmp(string_in2, "stop") == 0))
			{
				// ��������� ������
				flag_stop_search = 1;
			}
		}

		// ���������� ����� � �������. ���������� ����� �� �����
		TIMER::public_time_stop();
		deep_time_current = TIMER::public_time_delta();

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

	score = QUIESCENCESEARCH::public_forced_variant_black(p_bitboard,alpha,beta
		,depth,flag_pv_f);

	return score ;

#endif//#if FORCED 


	// �������� ������ ��� ����������(�������� ������ ������)
#if !FORCED
	score = EVALUATE::public_eval_state(p_bitboard) + (p_bitboard->value_white_material - p_bitboard->value_black_material);
	//score = p_bitboard-;

	//--------------------------------------------------------------------------
	// TEST
	//if (EVALUATE::Eval_forced(p_bitboard) != p_bitboard-)
	//{
	//	std::cout << "BLACK  Eval_forced(p_bitboard) != p_bitboard-" <<"\n";
	//	std::cout << "Eval_forced(p_bitboard)= "<< EVALUATE::Eval_forced(p_bitboard)<<"\n";
	//	std::cout << "p_bitboard-= "<< p_bitboard-<<"\n";
	//}
	//--------------------------------------------------------------------------

	//� ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
	//���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
	if (flag_pv_f)
	{
		PVSAVE::public_update_PV_best_max_depth(score,depth);
	}

	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
	//������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
	//�����  ������ ���� � ����
	//Print_std::cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
	//TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST

	return score;

#endif//#if !FORCED 

}

//	===================================================================
// ��������� ��������� �� ����� -
inline void FULLSEARCH::extension_for_chek_white
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	const unsigned __int8 depth_max,// ������������ �������
	unsigned __int8 & depth_max_new,// ����� ������������ �������
	bool & flag_check// ���� ����
)
{
	// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
	if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
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
inline void FULLSEARCH::extension_for_chek_black
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	const unsigned __int8 depth_max,// ������������ �������
	unsigned __int8 & depth_max_new,// ����� ������������ �������
	bool & flag_check// ���� ����
)
{
	// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
	if(BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
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
// ������ ����������
inline void FULLSEARCH::sorting_moves
(
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{
	__int32 move_s;// ���������� ��� ����������� ����
	__int32 score_s;// ���������� ��� ����������� ������ ����
	unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//���������� ������
	unsigned __int8 end_list = p_list_surplus_moves->end_list;//���������� �����

	/*
	������ � ��� ���������� � 0 � ����� ���
	0 ������
	1 ������
	2 ������
	3 ���          p_list_surplus_moves->end_captures
	�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
	4 ���
	5 ���
	6 ���
	7              p_list_surplus_moves->end_list
	���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

	������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����

	*/

	// 1 ��������� ������----------------------------------
	/*
	���� � ��� ���� ������ ����� �� �� ��������� �� ������ ����������� ��� � ���������� ������
	���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
	������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
	���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
	����� ������� ����� �� ������� �� ����� ��� ������ � ��� ������������� �� ���������.
	*/
	for (unsigned __int8 k = 0; k < end_captures; k++)
	{	
		for (unsigned __int8 i = k + 1; i < end_captures; i++)
		{
			if (p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i])
			{ 
				//��������� ������   
				move_s = p_list_surplus_moves->move[k];
				score_s = p_list_surplus_moves->sorting_score[k];

				//�������� �� i � k
				p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
				p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

				//��������������� ������
				p_list_surplus_moves->move[i] = move_s;
				p_list_surplus_moves->sorting_score[i] = score_s;
			}
		}
	}

}

//	===================================================================
//
void FULLSEARCH::public_ini_global_loop()
{
	audit = 0;// �������� ������ ������� ����������� ������
	flag_stop_search = 0;//
}

//	===================================================================
// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
// �� ��������� �� �������� �� ������
bool FULLSEARCH::public_get_stop_search()
{
	return flag_stop_search;
}

//	===================================================================
// ���������� ������ �������
struct T_PV * FULLSEARCH::public_public_get_pv_best()
{
	return PVSAVE::public_get_pv_best();
}

//	===================================================================
// ���������� ���������� ������������� �����
__int64  FULLSEARCH::public_get_nodes()
{
	return nodes;// ���������� �����;
}

//	===================================================================
// �������� ���������� ������������� �����
void FULLSEARCH::public_set_nodes_in_0()
{
	nodes = 0;// ���������� �����;
}

//	===================================================================
//
void FULLSEARCH::public_ini_deep_time_terminate
(
	double deep_time_terminate_v//
)
{
	deep_time_terminate = deep_time_terminate_v;
}

//	===================================================================
//
void FULLSEARCH::public_ini_flag_stop_null_move
(
	bool flag_stop_null_move_v//
)
{
	flag_stop_null_move = flag_stop_null_move_v;
}

//	===================================================================
//
void FULLSEARCH::public_set_depth_max_rem
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

void FULLSEARCH::List_Print
(
	int depth,// ������� �������
	struct T_list_surplus_moves * p_list_surplus_moves// ������ �����
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

	for (int i = 0 ; i < p_list_surplus_moves->end_list ; i++)
	{	
		Test_Loop << "����� ���� �� ������ i= " << i << "\n";  
		Test_Loop << "������� ������ " << util_o.public_int_to_char_board(((p_list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
		Test_Loop << "������ ���� " << p_list_surplus_moves->sorting_score[i] << "\n";

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Loop << " " << (util_o.public_x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8));
		Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
		//Test_Loop<< "\n";
		Test_Loop << " " << (util_o.public_x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8));
		Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
		//------------------------------
		//��� ���� 
		//0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
		//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
		//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
		//25 - ������ � ������������ � ����� 
		Test_Loop << "��� ���� " << ((p_list_surplus_moves->move[i]>>6)& 63)  << "\n";
		Test_Loop << "������ ������ " << util_o.public_int_to_char_board(( p_list_surplus_moves->move[i] & 63),1)<< "\n";
		Test_Loop << "��������� ��������� " << ((p_list_surplus_moves->move[i]>>12)& 63)  << "\n";
		Test_Loop << "�������� ��������� " << ((p_list_surplus_moves->move[i]>>18)& 63)  << "\n";
     
		Test_Loop << "\n";
	}

	Test_Loop << "p_list_surplus_moves->end_captures= " <<p_list_surplus_moves->end_captures << "\n";
	Test_Loop << "p_list_surplus_moves->end_list= " <<p_list_surplus_moves->end_list << "\n";
	Test_Loop << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop.close();
}
#endif