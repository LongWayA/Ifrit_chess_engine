
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 2.11.2009 20.12.2009

#include <fstream>
#include <iostream>

#include "d_Search_root.h"
#include "e_Search.h"
#include "h_Killer.h"
#include "i_ZObrist.h"
#include "j_Hash.h"
#include "k_PV_save.h"
#include "l_Chess_bitboard.h"
#include "m_List_surplus_moves.h"

class Search search; 

extern class Killer killer;
extern class ZObrist zobrist;
extern class Hash hash;
extern class PV_save pv_save;
extern class Chess_bitboard bitboard;
extern class List_surplus_moves root_list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)



//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
	std::ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// ����� �� �����. ����� ��������� ����� ��������
// ������� �� ������� ������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 Search_root::white_root
(
	__int32 alpha,//
	const __int32 beta,// 
	const unsigned __int8 depth_max// ������������ �������
)
{

	__int32 value;// ������� ������ �������
	__int32 value_max = -9999999;//������ � ����  ������ �������  
	unsigned __int8 depth_max_new;//����� ������� ��������
	bool flag_save_best = false;// ����� �� ������ �������
	bool flag_is_legal_moves = false;// ���� ������������� ��������� ����� (���� 0 �� ��������� ����� �� ����)
	bool flag_check = false;//���� ����
	const unsigned __int8 depth = 0;// �������
	const unsigned __int8 new_depth = 1; // ��������� �������

	// ���������� ��������� � ������ NegaScout
	__int32 beta_scout;//
	bool flag_pv_f_scout;// �������� �� ��� ������� (pv)

#if SAVE_LINE 
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	class Chess_bitboard test_bitboard;// ������������� �����. ��������� ��� �����
	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // ���������� ���-���� �������� ����
	unsigned __int8 i_hash_move = 0; // ����� ����������� ����
#endif//#if SAVE_LINE 


	//
	search.set_depth_max_rem(depth_max);

#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (search.get_stop_search() == true)
	{
		return 0;
	}

#endif//#if SAVE_LINE 

	root_list_surplus_moves.sorting_moves();
	//Sorting_white_moves_root(&root_list_surplus_moves);// ��������� ����

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_L && SAVE_LINE)
	list_print(depth_max,root_list_surplus_moves);
	//if(depth_max == 4) List_Print(depth,&root_list_surplus_moves);
	//Bitboard_print(bitboard);
#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------
#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	hash.look_hash_move(root_list_surplus_moves,bitboard.get_hash_key()
						,depth,flag_insert_hash_move);

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
	//List_Print(depth,&root_list_surplus_moves);
	////if(depth_max == 4) List_Print(depth,&root_list_surplus_moves);
	////Bitboard_print(bitboard);
//#endif//#if TEST_L
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


#if SAVE_LINE

	// TEST
	// ���������� ����� ��� ���������� �������� �� ������������
	// ����� �������� � ������� 
	test_bitboard.save_test_bitboard(bitboard);

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
	//if( (bitboard.white_piece[5] == 0) && // ���� � ����� ��� ������
	//(bitboard.white_piece[4] == 0) && // ��� �����
	//(bitboard.white_piece[3] == 0))  // ��� ������
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

	search.ini_flag_stop_null_move(1);

#endif//#if SAVE_LINE 


	//����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < root_list_surplus_moves.get_end_list(); i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		bitboard.do_moves_white(i,depth,root_list_surplus_moves);
		//Bitboard_print(bitboard);

		// ���� ������ ��� ����� �� (�������� ����� ���������� ��� �������� ������)
		if (root_list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			bitboard.undo_moves_white(i,depth,root_list_surplus_moves);

			// ���������� ������ � �������� ������ �����
			root_list_surplus_moves.set_sorting_score(i,-1000000);

			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes_root = nodes_root + 1;

#if SAVE_LINE
		// �������� ���������� � ��������������� ���� � ������� ��������
		pv_save.print_currmove(root_list_surplus_moves,nodes_0,i,depth_max);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� 
		// �� ������� �������
		pv_save.update_PV_current(i,depth,root_list_surplus_moves);

		// ��������� ��� ����
		zobrist.modification_random_key(1,i,root_list_surplus_moves
										,bitboard.get_d_undo(depth));

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth);

#endif//#if SAVE_LINE 

		depth_max_new = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
		search.extension_for_chek_white(depth_max,depth_max_new,flag_check
										,root_list_surplus_moves);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

		// ��� ����: 0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
		// 4 � �������� ��������� 5 � ������ �� ������� � �.�.
		const __int32 description_move = ( root_list_surplus_moves.get_move(i) >> 6 ) & 63;

//_scout_scout white
		// ����� � ��� ������������ ���������(NegaScout)
		// ������ ����� ������� ������� � ������� � ������ ����� ��������� ��������
		// ������ ������� � ������� ����� � ���� ����� ������ ���������� �� �������������
		// � ������ �����
#if (ALPHA_BETA_ON && SAVE_LINE)
		if (i == 0)
#else
		// � ������ ������� �������� ������� ��� � ��� ����� ������� ��������
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			// ������ ����� ������� ������� � ������� � ������ �����
			beta_scout = beta;
			flag_pv_f_scout = PV_YES;
		}
		else
		{
			// �������� �������  � ������� �����. ��� ��� �� ������� �������
			beta_scout = alpha + 1;
			flag_pv_f_scout = PV_NO;
		}

		//����������� ����� �������
		value = search.black(alpha,beta_scout,depth_max_new,new_depth,description_move,
							 flag_pv_f_scout,flag_check,NULL_NO);

#if (ALPHA_BETA_ON && SAVE_LINE)
		if( (value > alpha) && (i != 0) )
		{
			// ���� ����� alpha ������� �� ������� �� ����� ��� ������� �������
			beta_scout = beta;
			flag_pv_f_scout = PV_YES;

			//����������� ����� �������
			value = search.black(alpha,beta_scout,depth_max_new,new_depth,description_move,
								 flag_pv_f_scout,flag_check,NULL_NO);
		}

#endif//#if ALPHA_BETA_ON



		// ��������������� �������
		bitboard.undo_moves_white(i,depth,root_list_surplus_moves);

		// ���������� ������ � �������� ������ �����
		root_list_surplus_moves.set_sorting_score(i, value);


#if SAVE_LINE

		// ��������������� ���-���� �����
		bitboard.set_hash_key(key_undo);

		// ���� ������ �������. �� ����� ���� ��������
		if (value > value_max)
		{
			value_max = value;
			i_hash_move = i;

			// TEST
			from = ((root_list_surplus_moves.get_move(i)>>12)& 63);
			to = ((root_list_surplus_moves.get_move(i)>>18)& 63);

			// TEST		 
			//-----------------------------------------------
			//std::cout << "�������� ��������. ��� ���������� � ��� " << "\n";
			//std::cout << "depth= " << depth << "\n";
			//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
			//std::cout << "((root_list_surplus_moves.move[i]>>6)& 63)= " 
			//	<< ((root_list_surplus_moves.move[i]>>6)& 63) << "\n";
			//std::cout << "������� ������ " << util_o.public_int_to_char_board(((root_list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
           
			//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>12)& 63)%8);
			//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>12)& 63)/8);
			//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>18)& 63)%8);
			//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
			//std::cout << "  " << "\n"; 
			//-----------------------------------------------



//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// ����� �������� � ������ � �������� �� ��������
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			if (value > alpha)
			{ 
				alpha = value;       
				flag_save_best = true;

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				pv_save.update_PV_best_point(pv_best_point);

				// TEST
				// ��������� ���������� �������� � ������
				if (value != pv_save.get_pv_best().score_move)
				{
					std::cout << "ERROR white value " ;
					std::cout << "\n";
				}

				// TEST
				//-----------------------------------------------
				//std::cout << "�������� �����. ��� ���������� � ��� " << "\n";
				//std::cout << "depth= " << depth << "\n";
				//std::cout << "depth_max_rem= " << depth_max_rem << "\n";
				//std::cout << "((root_list_surplus_moves.move[i]>>6)& 63)= " 
				//	<< ((root_list_surplus_moves.move[i]>>6)& 63) << "\n";
				//std::cout << "������� ������ " << util_o.public_int_to_char_board(((root_list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
               
				//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>12)& 63)%8);
				//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>12)& 63)/8);
				//std::cout <<util_o.public_x_to_notation(((root_list_surplus_moves.move[i]>>18)& 63)%8);
				//std::cout <<(1 + ((root_list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
				//std::cout << "  " << "\n";     
				//-----------------------------------------------

			}
#else///#if ALPHA_BETA_ON

			flag_save_best = true;

			// ����� �� �������������� ������ ������� ��������������� ������ ������
			pv_save.update_PV_best_point(pv_best_point);

			// TEST
			// ��������� ���������� �������� � ������
			if (value != pv_save.get_pv_best().score_move)
			{
				std::cout << "ERROR white value " ;
				std::cout << "\n";
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� 
		// �.�. � ����� �����������
		if (search.get_stop_search() != true)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// ��� ����� ���������
			pv_save.print_pv_line(1,root_list_surplus_moves,pv_save.get_pv_best()
								  ,nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			pv_save.print_pv_line(1,root_list_surplus_moves,pv_best_point
								  ,nodes_root,i,depth_max,value_max,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 

	}


#if SAVE_LINE
	// TEST
	// ��������� �������������� �� ����� ����� ����������� �������
	test_bitboard.compare_test_bitboard(bitboard);
#endif//#if SAVE_LINE 

	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == false)
	{
		value = 0;
		if (root_list_surplus_moves.king_white_check(bitboard) != CHECK_NO)
		{
			value = -(VALUE_MATE - DELTA_MATE * depth);//
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		pv_save.update_PV_best_max_depth(value,depth);

		return value;
	}

#if SAVE_LINE
	if (flag_save_best == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� 
		// ������� �������
		pv_save.update_PV_best(pv_best_point);
	}

#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	// �������� ������ ���
	hash.save_best_hash_score_or_move(i_hash_move, bitboard.get_hash_key()
									  ,root_list_surplus_moves, 
									  value_max, UPPER, depth, depth_max);

#endif//#if HASH

	return value_max;

}

//	===================================================================
// ������� �� ������� ������ �� ������ ���� �������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 Search_root::black_root
(
	const __int32 alpha,//
	__int32 beta,// 
	const unsigned __int8 depth_max// ������������ �������
)
{
	__int32 value;              // ������� ������ �������

	__int32 value_min = 9999999;// ������ � ����  ������ �������  
	unsigned __int8 depth_max_new;//����� ������� ��������
	bool flag_save_best = false;// ����� �� ������ �������
	bool flag_is_legal_moves = false;// ���� ������������� ��������� ����� (���� 0 �� ��������� ����� �� ����)
	bool flag_check = false;//���� ����

	const unsigned __int8 depth = 0;// �������
	const unsigned __int8 new_depth = 1; // ��������� �������

	// ���������� ��������� � ������ NegaScout
	__int32 alpha_scout;//
	bool flag_pv_f_scout;// �������� �� ��� ������� (pv)

#if SAVE_LINE
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	class Chess_bitboard test_bitboard;// ������������� �����. ��������� ��� �����
	unsigned __int8  flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = bitboard.get_hash_key();   // ���������� ���-���� �������� ����
	unsigned __int8 i_hash_move = 0; // ����� ����������� ����
#endif//#if SAVE_LINE 


	search.set_depth_max_rem(depth_max);

#if SAVE_LINE

	// ��� ������� ��� ���������� ������
	if (search.get_stop_search() == true)
	{
		return 0;
	}

#endif//#if SAVE_LINE 


	root_list_surplus_moves.sorting_moves();
	//Sorting_black_moves_root(&root_list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_L && SAVE_LINE)
	list_print(depth_max,root_list_surplus_moves);
	//if (depth_max == 3) 
		//List_Print(depth,&root_list_surplus_moves);

	//	Bitboard_print(bitboard);
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	hash.look_hash_move(root_list_surplus_moves,bitboard.get_hash_key()
						,depth,flag_insert_hash_move);

#if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

	if (flag_insert_hash_move == 0)
	{
		std::cout << "���� �� ����� ������ ������+++++++++++++++++++++++++++++++++" << "\n";   
	}

	if (flag_insert_hash_move == 1) 
	{
		std::cout << "�������� ���� ������ ������+++++++++++++++++++++++++++++++++" << "\n";
		exit(1); 
	}

#endif//#if HASH_TEST
#endif//#if HASH


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//�������� ������ ������ � ����� �� ������� ������
//#if (TEST_L && SAVE_LINE)
	//if (depth_max == 3)
	//{
		//List_Print(depth,&root_list_surplus_moves);
	//}

	////Bitboard_print(bitboard);
//#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



#if SAVE_LINE

	// TEST
	//���������� ����� ��� ���������� �������� �� ������������
	test_bitboard.save_test_bitboard(bitboard);

#endif//#if SAVE_LINE 


	// ��������� ���������� � ��� ����� ���� ������ ��� ����� �� ��������� ������� ���
	//if ((bitboard.black_piece[5] == 0) && // ���� � ����� ��� ������
		//(bitboard.black_piece[4] == 0) && // ��� �����
		//(bitboard.black_piece[3] == 0))  // ��� ������
	//{ 
		//std::cout <<"������� ��� ������ ��������� ";
		////std::cout <<"\n";
		//flag_stop_null_move = 0; // �� ��������� ������� ���
	//}
	//else
	//{
		//flag_stop_null_move = 1; // ����� �� �������
	//}

	search.ini_flag_stop_null_move(1); // ����� �� �������


	// ����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < root_list_surplus_moves.get_end_list(); i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		bitboard.do_moves_black(i,depth,root_list_surplus_moves);
		//Bitboard_print(bitboard);

		// ���� ������ ��� ����� ��
		if (root_list_surplus_moves.king_black_check(bitboard) != CHECK_NO)
		{
			// �� �������� ���� ��� ��� ������������
			bitboard.undo_moves_black(i,depth,root_list_surplus_moves);

			// ���������� ������ � �������� ������ �����
			root_list_surplus_moves.set_sorting_score(i, 1000000);

			continue;
		}

		// ������ ���� ��� ���� ��������� ���� � ����������� �������
		flag_is_legal_moves = true;
		nodes_root = nodes_root + 1;

#if SAVE_LINE
		// �������� ���������� � ��������������� ���� � ������� ��������
		pv_save.print_currmove(root_list_surplus_moves,nodes_0,i,depth_max);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
		pv_save.update_PV_current(i,depth,root_list_surplus_moves);

		// ��������� ��� ����
		zobrist.modification_random_key(0,i,root_list_surplus_moves
										,bitboard.get_d_undo(depth));

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		zobrist.save_hash_three(bitboard.get_hash_key(),new_depth);

#endif//#if SAVE_LINE 

		depth_max_new = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
		search.extension_for_chek_black(depth_max,depth_max_new,flag_check
										,root_list_surplus_moves);
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

		const __int32 description_move = ( root_list_surplus_moves.get_move(i) >> 6 ) & 63;

//_scout_scout black
#if (ALPHA_BETA_ON && SAVE_LINE)
		if (i == 0)
#else
		if (1)
#endif//#if ALPHA_BETA_ON

		{
			alpha_scout = alpha;
			flag_pv_f_scout = PV_YES;
		}
		else
		{
			alpha_scout = beta - 1;
			flag_pv_f_scout = PV_NO;
		}

		value = search.white(alpha_scout,beta,depth_max_new,new_depth,description_move,
							 flag_pv_f_scout,flag_check,NULL_NO);

#if (ALPHA_BETA_ON && SAVE_LINE)
		if ( (value < beta) && (i != 0) )
		{
			alpha_scout = alpha;
			flag_pv_f_scout = PV_YES;

			value = search.white(alpha_scout,beta,depth_max_new,new_depth,description_move,
								 flag_pv_f_scout,flag_check,NULL_NO);
		}
#endif//#if ALPHA_BETA_ON

		//��������������� �������
		bitboard.undo_moves_black(i,depth,root_list_surplus_moves);

		// ���������� ������ � �������� ������ �����
		root_list_surplus_moves.set_sorting_score(i, value);


#if SAVE_LINE

		// ��������������� ���-���� �����
		bitboard.set_hash_key(key_undo);

		//���� ������ �������. �� ������ ���� �������
		if (value < value_min)
		{
			value_min = value;
			i_hash_move = i; // ����� ����������� ����

			// TEST
			from = ((root_list_surplus_moves.get_move(i) >> 12) & 63);
			to = ((root_list_surplus_moves.get_move(i) >> 18) & 63);
  
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
			// ������ �������� �������� ����
			if (value < beta)
			{
				beta = value;
				flag_save_best = true;

				// ����� �� �������������� ������ ������� ��������������� ������ ������
				pv_save.update_PV_best_point(pv_best_point);

				// TEST
				// ��������� ���������� �������� � ������
				if (value != pv_save.get_pv_best().score_move)
				{
					std::cout << "ERROR black value " ;
					std::cout << "\n";
				}
			}
#else//#if ALPHA_BETA_ON

			flag_save_best = 1;

			// ����� �� �������������� ������ ������� ��������������� ������ ������
			pv_save.update_PV_best_point(pv_best_point);

			// TEST
			// ��������� ���������� �������� � ������
			if (value != pv_save.get_pv_best().score_move)
			{
				std::cout << "ERROR black value " ;
				std::cout << "\n";
			}

#endif//#if ALPHA_BETA_ON
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

		}

		//--------------------------------------------------------------------
		// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� 
		// �.�. � ����� �����������
		if (search.get_stop_search() != 1)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// ��� ����� ���������
			pv_save.print_pv_line(0,root_list_surplus_moves,pv_save.get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			pv_save.print_pv_line(0,root_list_surplus_moves,pv_best_point,nodes_root
								  ,i,depth_max,value_min,0);
#endif//#if TEST_PRINT_ALL_LINE
		}
#endif//#if SAVE_LINE 

	}

#if SAVE_LINE
	// TEST
	// ��������� �������������� �� ����� ����� ����������� �������
	test_bitboard.compare_test_bitboard(bitboard);
#endif//#if SAVE_LINE 

	// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
	// ��� ������ ���� ��� ����
	if (flag_is_legal_moves == false)
	{
		value = 0;
		if (root_list_surplus_moves.king_black_check(bitboard) != CHECK_NO)
		{
			value = (VALUE_MATE - DELTA_MATE * depth);// ����� ��� ������� ������� �����?
		}

		// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
		pv_save.update_PV_best_max_depth(value,depth);

		return value;
	}


#if SAVE_LINE
	if (flag_save_best == true)
	{
		// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� 
		// ����� ������� �������
		pv_save.update_PV_best(pv_best_point);
	}
#endif//#if SAVE_LINE 


#if (HASH && SAVE_LINE)
	//�������� ������ ���
	hash.save_best_hash_score_or_move(i_hash_move, bitboard.get_hash_key()
									  ,root_list_surplus_moves, 
									   value_min, UPPER, depth, depth_max);
#endif//#if HASH


	return value_min;
}

//	===================================================================
//
void Search_root::ini_global_loop
(
	unsigned __int8 depth_max//
)
{
	// �������������� ���������� ���������� ������ loop
	search.ini_global_loop();
}

//	===================================================================
//
void Search_root::ini_deep_time_terminate
(
	double deep_time_terminate_v//
)
{
	search.ini_deep_time_terminate(deep_time_terminate_v);
}

//	===================================================================
// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
// �� ��������� �� �������� �� ������
bool Search_root::get_stop_search()
{
	return search.get_stop_search();
}

//	===================================================================
// ���������� ������ �������
struct PV_line & Search_root::get_pv_best()
{
	return pv_save.get_pv_best();
}

//	===================================================================
// ���������� ���������� ������������� �����
__int64 Search_root::get_nodes()
{
	return (nodes_root + search.get_nodes());// ���������� �����;
}

//	===================================================================
// �������� ���������� ������������� �����
void Search_root::set_nodes_in_0()
{
	nodes_root = 0;// ���������� �����;
	search.set_nodes_in_0();
}

//	===================================================================
// ������ �������� ���
unsigned __int8 Search_root::get_from()
{
	return from;
}

//	===================================================================
// ���� �������� ���
unsigned __int8 Search_root::get_to()
{
	return to;
}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������������ �������, ������� ��������, �����,
#if TEST_L

void Search_root::list_print
(
	int depth,// ������� �������
	class List_surplus_moves & list_surplus_moves// ������ ��������� �����
)
{
	Test_Loop.open ("_TEST_LOOP.txt",std::ios::app);	
	//Test_Loop.open ("_TEST_LOOP.txt");	
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "\n";
	Test_Loop << "------------------------------------------------- "<<"\n";
	Test_Loop <<"depth= "<< depth << "\n";
	Test_Loop << "\n";
	Test_Loop << "������ ��������� ����� ������ "<<"\n";

	for (int i = 0 ; i < list_surplus_moves.get_end_list() ; i++)
	{	
		Test_Loop << "����� ���� �� ������ i= " << i << "\n";  
		Test_Loop << "������� ������ " << pv_save.int_to_char_board(((list_surplus_moves.get_move(i)>>24)& 7),1)<< "\n";
		Test_Loop << "������ ���� " << list_surplus_moves.get_sorting_score(i) << "\n";

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Loop << " " << (pv_save.x_to_notation(((list_surplus_moves.get_move(i)>>12)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.get_move(i)>>12)& 63)/8);//<< "\n";
		//Test_Loop<< "\n";
		Test_Loop << " " << (pv_save.x_to_notation(((list_surplus_moves.get_move(i)>>18)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.get_move(i)>>18)& 63)/8)<< "\n";   
		//------------------------------
		//��� ���� 
		//0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
		//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
		//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
		//25 - ������ � ������������ � ����� 
		Test_Loop << "��� ���� " << ((list_surplus_moves.get_move(i)>>6)& 63)  << "\n";
		Test_Loop << "������ ������ " << pv_save.int_to_char_board(( list_surplus_moves.get_move(i) & 63),1)<< "\n";
		Test_Loop << "��������� ��������� " << ((list_surplus_moves.get_move(i)>>12)& 63)  << "\n";
		Test_Loop << "�������� ��������� " << ((list_surplus_moves.get_move(i)>>18)& 63)  << "\n";
     
		Test_Loop << "\n";
	}

	Test_Loop << "list_surplus_moves.end_captures= " <<list_surplus_moves.get_end_captures() << "\n";
	Test_Loop << "list_surplus_moves.end_list= " <<list_surplus_moves.get_end_list() << "\n";
	Test_Loop << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop.close();
}
#endif