#include <iostream> // TEST
#include <fstream>  // TEST
//#include <cstring>    // ���������� ��� ������ �� ������� (��������� �����)
//#include <stdlib.h>   // ������������ ���������� ������ �������� exit(1)


#include "k_Util.h"
#include "e_Move.h"
#include "h_Evaluate.h"
#include "i_Quiescence_search.h"
#include "k_Win_32.h"
#include "j_Hash.h"
#include "j_Killer.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"

#include "j_ZObrist.h"
#include "g_Search_switch.h"
#include "g_Search_root.h"
#include "g_Search.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 2.11.2009 20.12.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

 ������� ������� � ����� �.�. �� ������� �������

*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
   ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


namespace 
{
	// NULL MOVE
	const unsigned __int8 NULL_NO = 0; // �� ������� �������� ���� 

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
	__int64 nodes_root;// ���������� �����

	// ��������� ������ ��� �������� ���������� ���� � ��� ��� �������� � ����� ����
	unsigned __int8 from;// ������ ����� ������
	unsigned __int8 to;// ���� ����� ������

}

//	===================================================================
// ����� �� �����. ����� ��������� ����� ��������
// ������� �� ������� ������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 ROOTSEARCH::public_white_root
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
	__int32 alpha,//
	const __int32 beta,// 
	const unsigned __int8 depth_max// ������������ �������
)
{

	__int32 value;// ������� ������ �������
	struct T_undo undo[1];//���������� ������������ ��� �������� ����
	__int32 value_max = -9999999;//������ � ����  ������ �������  
	unsigned __int8 depth_max_new = 0;//����� ������� ��������
	bool flag_save_best = 0;// ����� �� ������ �������
	bool flag_is_legal_moves = 0;// ���� ������������� ��������� ����� (���� 0 �� ��������� ����� �� ����)
	bool flag_check = 0;//���� ����
	const unsigned __int8 depth = 0;// �������
	const unsigned __int8 new_depth = 1; // ��������� �������


#if SAVE_LINE 
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct T_PV pv_best_point;// ������ ��� ������� ���� �������
	struct T_Bitboard test_bitboard;// ������������� �����. ��������� ��� �����
	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
	unsigned __int8 i_hash_move = 0; // ����� ����������� ����
#endif//#if SAVE_LINE 


	FULLSEARCH::public_set_depth_max_rem(depth_max);

#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (FULLSEARCH::public_get_stop_search() == 1)
	{
		return 0;
	}

	// �������������� ������ ��������
	SKILLER::public_ini_killer(); 

#endif//#if SAVE_LINE 

	FULLSEARCH::sorting_moves(&list_surplus_moves);
	//Sorting_white_moves_root(&list_surplus_moves);// ��������� ����

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_L && SAVE_LINE)
	List_Print(depth_max,&list_surplus_moves);
	//if(depth_max == 4) List_Print(depth,&list_surplus_moves);
	//Bitboard_print(p_bitboard);
#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------
#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	HASHM::public_look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

// ��� ������� ����� ����� ��������� ������� �� ������
#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	//std::cout << "���� ������ ����� flag_insert_hash_move " << flag_insert_hash_move << "\n";

	if (flag_insert_hash_move == 0)
	{
		std::cout << "���� �� ����� ������ �����++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
	}
	
	if (flag_insert_hash_move == 1)
	{
		std::cout << "�������� ���� ������ �����++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
		exit(1); 
	}
#endif//#if HASH_TEST

#endif//#if HASH
//-----------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
//#if (TEST_L && SAVE_LINE)
	//List_Print(depth,&list_surplus_moves);
	////if(depth_max == 4) List_Print(depth,&list_surplus_moves);
	////Bitboard_print(p_bitboard);
//#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


#if SAVE_LINE

	//���������� ����� ��� ���������� �������� �� ������������
	//����� �������� � ������� 
	save_test_bitboard(p_bitboard,&test_bitboard);

	/*
	��� ������
	0 - ��� ������
	1 - ����� 
	2 - ���� 
	3 - ����
	4 - ����� 
	5 - ����� 
	6 - ������
	*/

	// ��������� ���������� � ��� ����� ���� ������ ��� ����� �� ��������� ������� ���
	//if( (p_bitboard->white_piece[5] == 0) && // ���� � ����� ��� ������
	//(p_bitboard->white_piece[4] == 0) && // ��� �����
	//(p_bitboard->white_piece[3] == 0))  // ��� ������
	//)
	//{ 
		////std::cout <<"������� ��� ����� ��������� ";
		////std::cout <<"\n";
		//flag_stop_null_move = 0; // �� ��������� ������� ���
	//}
	//else 
	//{
		//flag_stop_null_move = 1; // ����� �� �������
	//}

	FULLSEARCH::public_ini_flag_stop_null_move(1);

#endif//#if SAVE_LINE 


	//����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		MOVES::public_onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);
		//Bitboard_print(p_bitboard);

		// ���� ������ ��� ����� �� (�������� ����� ���������� ��� �������� ������)
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

			// ���������� ������ � �������� ������ �����
			list_surplus_moves.sorting_score[i] = -1000000;

			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = 1;
		nodes_root = nodes_root + 1;
		depth_max_new = depth_max;


#if SAVE_LINE
		// �������� ���������� � ��������������� ���� � ������� ��������
		PVSAVE::public_print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������
		PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);

		// ��������� ��� ����
		ZOBRIST::public_modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(p_bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth);

#endif//#if SAVE_LINE 

    
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
		FULLSEARCH::extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------


#if (ALPHA_BETA_ON && SAVE_LINE)
		// ����� � ��� ������������ ���������(NegaScout)
		// ������ ����� ������� ������� � ������� � ������ ����� ��������� ��������
		// ������ ������� � ������� ����� � ���� ����� ������ ���������� �� �������������
		// � ������ �����
		if (i == 0)
#else
		// � ������ ������� �������� ������� ��� � ��� ����� ������� ��������
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			//����������� ����� �������
			value = FULLSEARCH::public_black(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,NULL_NO);
		}
		else
		{
			// �������� �������  � ������� �����. ��� ��� �� ������� �������
			value = FULLSEARCH::public_black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,NULL_NO);

			if(value > alpha)
			{
				// ���� ����� alpha ������� �� ������� �� ����� ��� ������� �������
				value = FULLSEARCH::public_black(p_bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.move[i]>>6) & 63),
					PV_YES,flag_check,NULL_NO);
			}
		}

		// ��������������� �������
		MOVES::public_backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

		// ���������� ������ � �������� ������ �����
		list_surplus_moves.sorting_score[i] = value;


#if SAVE_LINE

		// ��������������� ���-���� �����
		p_bitboard->hash_key = key_undo;

		// ���� ������ �������. �� ����� ���� ��������
		if (value > value_max)
		{
			value_max = value;
			i_hash_move = i;
			from = ((list_surplus_moves.move[i]>>12)& 63);
			to = ((list_surplus_moves.move[i]>>18)& 63);

			// TEST		 
			//-----------------------------------------------
			//std::cout << "�������� ��������. ��� ���������� � ��� " << "\n";
			//std::cout << "depth= " << depth << "\n";
			//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
			//std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
			//	<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
			//std::cout << "������� ������ " << util_o.public_int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
           
			//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
			//std::cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
			//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
			//std::cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
			//std::cout << "  " << "\n"; 
			//-----------------------------------------------



//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// ����� �������� � ������ � �������� �� ��������
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			if (value > alpha)
			{ 
				alpha = value;       
				flag_save_best = 1;

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				PVSAVE::public_update_PV_best_point(&pv_best_point);

				// ��������� ���������� �������� � ������
				if (value != PVSAVE::public_get_pv_best()->score_move)
				{
					std::cout << "ERROR white value " ;
					std::cout << "\n";
				}

				// TEST
				//-----------------------------------------------
				//std::cout << "�������� �����. ��� ���������� � ��� " << "\n";
				//std::cout << "depth= " << depth << "\n";
				//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
				//std::cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				//	<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
				//std::cout << "������� ������ " << util_o.public_int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
               
				//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
				//std::cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
				//std::cout <<util_o.public_x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
				//std::cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
				//std::cout << "  " << "\n";     
				//-----------------------------------------------

			}
#else///#if ALPHA_BETA_ON

			flag_save_best = 1;
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			PVSAVE::public_update_PV_best_point(&pv_best_point);

			// ��������� ���������� �������� � ������
			if (value != PVSAVE::public_get_pv_best()->score_move)
			{
				std::cout << "ERROR white value " ;
				std::cout << "\n";
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� �.�. � ����� �����������
		if (FULLSEARCH::public_get_stop_search() != 1)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// ��� ����� ���������
			PVSAVE::public_print_pv_line(1,&list_surplus_moves,PVSAVE::public_get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			PVSAVE::public_print_pv_line(1,&list_surplus_moves,&pv_best_point,nodes_root,i,depth_max,value_max,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 


	}


#if SAVE_LINE
	// ��������� �������������� �� ����� ����� ����������� �������
	compare_test_bitboard(p_bitboard,&test_bitboard);
#endif//#if SAVE_LINE 

	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == 0)
	{
		value = 0;
		if (BITBOARDGENERATOR::public_king_white_check(p_bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		PVSAVE::public_update_PV_best_max_depth(value,depth);

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == 1)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
		PVSAVE::public_update_PV_best(&pv_best_point);
	}

#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	// �������� ������ ���
	HASHM::public_save_best_hash_score_or_move(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
		value_max, HASHM::UPPER, depth, depth_max);

#endif//#if HASH

	return value_max;

}

//	===================================================================
// ������� �� ������� ������ �� ������ ���� �������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 ROOTSEARCH::public_black_root
(
	struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
	const __int32 alpha,//
	__int32 beta,// 
	const unsigned __int8 depth_max// ������������ �������
)
{
	__int32 value;              // ������� ������ �������
	struct T_undo undo[1];//���������� ������������ ��� �������� ����

	__int32 value_min = 9999999;// ������ � ����  ������ �������  
	unsigned __int8 depth_max_new = 0;//����� ������� ��������
	bool flag_save_best = 0;// ����� �� ������ �������
	bool flag_is_legal_moves = 0;// ���� ������������� ��������� ����� (���� 0 �� ��������� ����� �� ����)
	bool flag_check = 0;//���� ����

	const unsigned __int8 depth = 0;// �������
	const unsigned __int8 new_depth = 1; // ��������� �������

#if SAVE_LINE
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct T_PV pv_best_point;// ������ ��� ������� ���� �������
	struct T_Bitboard test_bitboard;// ������������� �����. ��������� ��� �����
	unsigned __int8  flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
	unsigned __int8 i_hash_move = 0; // ����� ����������� ����
#endif//#if SAVE_LINE 

	FULLSEARCH::public_set_depth_max_rem(depth_max);


#if SAVE_LINE

	// ��� ������� ��� ���������� ������
	if (FULLSEARCH::public_get_stop_search() == 1)
	{
		return 0;
	}

	// �������������� ������ ��������
	SKILLER::public_ini_killer(); 

#endif//#if SAVE_LINE 


	FULLSEARCH::sorting_moves(&list_surplus_moves);
	//Sorting_black_moves_root(&list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_L && SAVE_LINE)
	List_Print(depth_max,&list_surplus_moves);
	//if (depth_max == 3) 
		//List_Print(depth,&list_surplus_moves);

	//	Bitboard_print(p_bitboard);
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	HASHM::public_look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	if (flag_insert_hash_move == 0)
	{
		std::cout << "���� �� ����� ������ ������++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
	}

	if (flag_insert_hash_move == 1) 
	{
		std::cout << "�������� ���� ������ ������++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
		exit(1); 
	}

#endif//#if HASH_TEST
#endif//#if HASH


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//�������� ������ ������ � ����� �� ������� ������
//#if (TEST_L && SAVE_LINE)
	//if (depth_max == 3)
	//{
		//List_Print(depth,&list_surplus_moves);
	//}

	////Bitboard_print(p_bitboard);
//#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



#if SAVE_LINE

	//���������� ����� ��� ���������� �������� �� ������������
	save_test_bitboard(p_bitboard,&test_bitboard);

#endif//#if SAVE_LINE 


	// ��������� ���������� � ��� ����� ���� ������ ��� ����� �� ��������� ������� ���
	//if ((p_bitboard->black_piece[5] == 0) && // ���� � ����� ��� ������
		//(p_bitboard->black_piece[4] == 0) && // ��� �����
		//(p_bitboard->black_piece[3] == 0))  // ��� ������
	//{ 
		//std::cout <<"������� ��� ������ ��������� ";
		////std::cout <<"\n";
		//flag_stop_null_move = 0; // �� ��������� ������� ���
	//}
	//else
	//{
		//flag_stop_null_move = 1; // ����� �� �������
	//}

	FULLSEARCH::public_ini_flag_stop_null_move(1); // ����� �� �������


	//����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		MOVES::public_onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);
		//Bitboard_print(p_bitboard);

		// ���� ������ ��� ����� ��
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

			// ���������� ������ � �������� ������ �����
			list_surplus_moves.sorting_score[i] = 1000000;

			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = 1;
		nodes_root = nodes_root + 1;
		depth_max_new = depth_max;


#if SAVE_LINE
		// �������� ���������� � ��������������� ���� � ������� ��������
		PVSAVE::public_print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
		PVSAVE::public_update_PV_current(i,depth,&list_surplus_moves);

		// ��������� ��� ����
		ZOBRIST::public_modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(p_bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		ZOBRIST::public_save_hash_three(p_bitboard->hash_key,new_depth);

#endif//#if SAVE_LINE 


//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
		FULLSEARCH::extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

#if (ALPHA_BETA_ON && SAVE_LINE)
		if (i == 0)
#else
		if (1)
#endif//#if ALPHA_BETA_ON

		{
			value = FULLSEARCH::public_white(p_bitboard,alpha,beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_YES,flag_check,NULL_NO);

		}
		else
		{
			//����������� ����� �������
			value = FULLSEARCH::public_white(p_bitboard,(beta - 1),beta,depth_max_new,
				new_depth,((list_surplus_moves.move[i]>>6) & 63),
				PV_NO,flag_check,NULL_NO);

			if (value < beta)
			{
				//����������� ����� �������
				value = FULLSEARCH::public_white(p_bitboard,alpha,beta,depth_max_new,
					new_depth,((list_surplus_moves.move[i]>>6)& 63),
					PV_YES,flag_check,NULL_NO);
			}
		}

		//��������������� �������
		MOVES::public_backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

		// ���������� ������ � �������� ������ �����
		list_surplus_moves.sorting_score[i] = value;


#if SAVE_LINE

		// ��������������� ���-���� �����
		p_bitboard->hash_key = key_undo;

		//���� ������ ������� �� ������ ���� �������
		if (value < value_min)
		{
			value_min = value;
			i_hash_move = i; // ����� ����������� ����
			from = ((list_surplus_moves.move[i] >> 12) & 63);
			to = ((list_surplus_moves.move[i] >> 18) & 63);
  
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			// ������ �������� �������� ����
			if (value < beta)
			{
				beta = value;
				flag_save_best = 1;

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				PVSAVE::public_update_PV_best_point(&pv_best_point);

				// ��������� ���������� �������� � ������
				if (value != PVSAVE::public_get_pv_best()->score_move)
				{
					std::cout << "ERROR black value " ;
					std::cout << "\n";
				}
			}
#else//#if ALPHA_BETA_ON

			flag_save_best = 1;
			// ����� �� �������������� ������ ������� ��������������� ������ ������
			PVSAVE::public_update_PV_best_point(&pv_best_point);

			// ��������� ���������� �������� � ������
			if (value != PVSAVE::public_get_pv_best()->score_move)
			{
				std::cout << "ERROR black value " ;
				std::cout << "\n";
			}
#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� �.�. � ����� �����������
		if (FULLSEARCH::public_get_stop_search() != 1)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// ��� ����� ���������
			PVSAVE::public_print_pv_line(0,&list_surplus_moves,PVSAVE::public_get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			PVSAVE::public_print_pv_line(0,&list_surplus_moves,&pv_best_point,nodes_root,i,depth_max,value_min,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 


	}


#if SAVE_LINE
	// ��������� �������������� �� ����� ����� ����������� �������
	compare_test_bitboard(p_bitboard,&test_bitboard);
#endif//#if SAVE_LINE 

	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == 0)
	{
		value = 0;
		if (BITBOARDGENERATOR::public_king_black_check(p_bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);// ����� ��� ������� ������� �����?
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		PVSAVE::public_update_PV_best_max_depth(value,depth);

		return value;
	}


#if SAVE_LINE
	if (flag_save_best == 1)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
		PVSAVE::public_update_PV_best(&pv_best_point);
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	//�������� ������ ���
	HASHM::public_save_best_hash_score_or_move(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
		value_min, HASHM::UPPER, depth, depth_max);
#endif//#if HASH


	return value_min;
}

//	===================================================================
//
void ROOTSEARCH::public_ini_global_loop
(
	unsigned __int8 depth_max//
)
{
	// �������������� ���������� ���������� ������ loop
	FULLSEARCH::public_ini_global_loop();
}

//	===================================================================
//
void ROOTSEARCH::public_ini_deep_time_terminate
(
	double deep_time_terminate_v//
)
{
	FULLSEARCH::public_ini_deep_time_terminate(deep_time_terminate_v);
}

//	===================================================================
// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
// �� ��������� �� �������� �� ������
bool ROOTSEARCH::public_get_stop_search()
{
	return FULLSEARCH::public_get_stop_search();
}

//	===================================================================
// ���������� ������ �������
struct T_PV * ROOTSEARCH::public_get_pv_best()
{
	return PVSAVE::public_get_pv_best();
}

//	===================================================================
// ���������� ���������� ������������� �����
__int64  ROOTSEARCH::public_get_nodes()
{
	return (nodes_root + FULLSEARCH::public_get_nodes());// ���������� �����;
}

//	===================================================================
// �������� ���������� ������������� �����
void ROOTSEARCH::public_set_nodes_in_0()
{
	nodes_root = 0;// ���������� �����;
	FULLSEARCH::public_set_nodes_in_0();
}

//	===================================================================
// ������ �������� ���
unsigned __int8 ROOTSEARCH::public_get_from()
{
	return from;
}

//	===================================================================
// ���� �������� ���
unsigned __int8 ROOTSEARCH::public_get_to()
{
	return to;
}





//	===================================================================
// ������ ���������� ��������� ������
inline void ROOTSEARCH::sorting_white_moves_root
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

	// 1 ��������� �������� ����----------------------------------
	/*
	�� �� ��������� �� ������ ����������� �� ���������� ��������
	���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
	������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
	���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
	����� ������� ����� �� ������� �� ����� ��� ���� � ��� ������������� �� ���������.
	*/
	for (unsigned __int8 k = 0; k < end_list; k++)
	{	
		for (unsigned __int8 i = k + 1; i < end_list; i++)
		{
			if (p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i])
			{ 
				// ��������� ������   
				move_s = p_list_surplus_moves->move[k];
				score_s = p_list_surplus_moves->sorting_score[k];

				// �������� �� i � k
				p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
				p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

				// ��������������� ������
				p_list_surplus_moves->move[i] = move_s;
				p_list_surplus_moves->sorting_score[i] = score_s;
			}
		}
	}

}

//	===================================================================
// ������ ���������� ��������� ������
inline void ROOTSEARCH::sorting_black_moves_root
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

	// 1 ��������� �������� ����----------------------------------
	/*
	�� �� ��������� �� ������ ����������� �� ���������� ��������
	���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
	������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
	���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
	����� ������� ����� �� ������� �� ����� ��� ���� � ��� ������������� �� ���������.
	*/
	for (unsigned __int8 k = 0; k < end_list; k++)
	{	
		for (unsigned __int8 i = k + 1; i < end_list; i++)
		{
			if (p_list_surplus_moves->sorting_score[k] > p_list_surplus_moves->sorting_score[i])
			{ 
				// ��������� ������   
				move_s = p_list_surplus_moves->move[k];
				score_s = p_list_surplus_moves->sorting_score[k];

				// �������� �� i � k
				p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
				p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

				// ��������������� ������
				p_list_surplus_moves->move[i] = move_s;
				p_list_surplus_moves->sorting_score[i] = score_s;
			}
		}
	}
}

//	===================================================================
// ��������� ��� �����. ������ ��� �� ����� ��� ���� �� ��� ��� �����
void ROOTSEARCH::print_name_from_coord
(
	const struct T_Bitboard * const p_bitboard// ������������� �����
)
{

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << p_bitboard->white_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	std::cout << "\n";

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << p_bitboard->black_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	//for (int i= 0; i < 64; i++)
	//{
	//	std::cout << bitboard->white_name_from_coord[i];
	//	if( i%8 == 7)
	//{
	//		std::cout << "\n";
	//}
	//}
}

//	===================================================================
// ��������� ��� �����. ���������� �����
inline void ROOTSEARCH::save_test_bitboard
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_Bitboard * p_test_bitboard// ������������� �����
)
{

	p_test_bitboard->all_piece = p_bitboard->all_piece;// ��� ������
	p_test_bitboard->all_white_piece = p_bitboard->all_white_piece;// ��� ����� ������
	p_test_bitboard->all_black_piece = p_bitboard->all_black_piece;// ��� ������ ������
	p_test_bitboard->colour_move = p_bitboard->colour_move;
	p_test_bitboard->taking_passage_yes = p_bitboard->taking_passage_yes;
	p_test_bitboard->taking_passage_x = p_bitboard->taking_passage_x;
	p_test_bitboard->taking_passage_y = p_bitboard->taking_passage_y;
	p_test_bitboard->castling_Q = p_bitboard->castling_Q;
	p_test_bitboard->castling_K = p_bitboard->castling_K;
	p_test_bitboard->castling_q = p_bitboard->castling_q;
	p_test_bitboard->castling_k = p_bitboard->castling_k;
	p_test_bitboard->hash_key =  p_bitboard->hash_key;
	p_test_bitboard->value_white_material = p_bitboard->value_white_material;
	p_test_bitboard->value_black_material = p_bitboard->value_black_material;

	for (unsigned __int8 n =0; n < 7; n++ )
	{
		p_test_bitboard->white_piece[n] = p_bitboard->white_piece[n];
		p_test_bitboard->black_piece[n] = p_bitboard->black_piece[n];
	}

	for (unsigned __int8 k =0; k < 64; k++ )
	{
		p_test_bitboard->white_name_from_coord[k] = p_bitboard->white_name_from_coord[k];
		p_test_bitboard->black_name_from_coord[k] = p_bitboard->black_name_from_coord[k];
	}
}

//	===================================================================
// ��������� ��� �����. ���������� �����
inline void ROOTSEARCH::compare_test_bitboard
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_Bitboard * p_test_bitboard// ������������� �����
)
{
	// ��������� ������������� �� ����� ����� ����������� �������
	if (p_test_bitboard->all_piece != p_bitboard->all_piece)
	{
		std::cout << "white test_all_piece != bitboard->all_piece" << "\n";// ��� ������
	}

	if (p_test_bitboard->all_white_piece != p_bitboard->all_white_piece)
	{
		std::cout << "white test_all_white_piece != bitboard->all_white_piece" << "\n";// ��� ����� ������
	}

	if (p_test_bitboard->all_black_piece != p_bitboard->all_black_piece) 
	{
		std::cout << "white test_all_black_piece != bitboard->all_black_piece" << "\n";// ��� ������ ������
	}

	if (p_test_bitboard->colour_move != p_bitboard->colour_move)
	{
		std::cout << "white test_colour_move != bitboard->colour_move" << "\n";
	}

	if (p_test_bitboard->taking_passage_yes != p_bitboard->taking_passage_yes)
	{
		std::cout << "white test_taking_passage_yes != bitboard->taking_passage_yes" << "\n";
	}

	if (p_test_bitboard->taking_passage_x != p_bitboard->taking_passage_x)
	{
		std::cout << "white test_taking_passage_x != bitboard->taking_passage_x" << "\n";
	}

	if (p_test_bitboard->taking_passage_y != p_bitboard->taking_passage_y)
	{
		std::cout << "white test_taking_passage_y != bitboard->taking_passage_y" << "\n";
	}

	if (p_test_bitboard->castling_Q != p_bitboard->castling_Q)
	{
		std::cout << "white test_castling_Q != bitboard->castling_Q" << "\n";
	}

	if (p_test_bitboard->castling_K != p_bitboard->castling_K)
	{
		std::cout << "white test_castling_K != bitboard->castling_K" << "\n";
	}

	if (p_test_bitboard->castling_q != p_bitboard->castling_q)
	{
		std::cout << "white test_castling_q != bitboard->castling_q" << "\n";
	}

	if (p_test_bitboard->castling_k != p_bitboard->castling_k)
	{
		std::cout << "white test_castling_k != bitboard->castling_k" << "\n";
	}

	if (p_test_bitboard->hash_key !=  p_bitboard->hash_key)
	{
		std::cout << "white test_hash_key !=  bitboard->hash_key" << "\n";
	}

	if (p_test_bitboard->value_white_material != p_bitboard->value_white_material)
	{
		std::cout << "white test_value_white_material != bitboard->value_white_material" << "\n";
	}

	if (p_test_bitboard->value_black_material != p_bitboard->value_black_material)
	{
		std::cout << "white test_value_black_material != bitboard->value_black_material" << "\n";
	}



	for (unsigned __int8 n2 = 0; n2 < 7; n2++ )
	{
		if (p_test_bitboard->white_piece[n2] != p_bitboard->white_piece[n2])
		{
			std::cout << "white test_white_piece[n] = bitboard->white_piece[n]" << n2 << "\n";
		}

		if (p_test_bitboard->black_piece[n2] != p_bitboard->black_piece[n2])
		{
			std::cout << "white test_black_piece[n] = bitboard->black_piece[n]" << n2 << "\n";
		}
	}

	for (unsigned __int8 k2 = 0; k2 < 64; k2++ )
	{
		if (p_test_bitboard->white_name_from_coord[k2] != p_bitboard->white_name_from_coord[k2])
		{
			std::cout << "white test_white_name_from_coord[k] = bitboard->white_name_from_coord[k]" << k2 << "\n";
		}

		if (p_test_bitboard->black_name_from_coord[k2] != p_bitboard->black_name_from_coord[k2])
		{
			std::cout << "white test_black_name_from_coord[k] = bitboard->black_name_from_coord[k]" << k2 << "\n";
		}
	}

}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������������ �������, ������� ��������, �����,
#if TEST_L

void ROOTSEARCH::List_Print
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