
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 2.11.2009 20.12.2009 27.09.2011

#include <fstream>
#include <iostream>

#include "Search_switch.h"
#include "Search_constants.h"
#include "Search_root.h"
#include "Search.h"
#include "Generator_moves.h"
#include "Make_move.h"
#include "Test_chess_bitboard.h"
#include "Sorting_moves.h"
#include "Killer.h"
#include "History.h"
#include "ZObrist.h"
#include "Hash.h"
#include "Hash_constants.h"
#include "PV_save.h"

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


//	===================================================================
// ����� �� �����. ����� ��������� ����� ��������
// ������� �� ������� ������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 Search_root::white_root
(
	struct Position & position,// ������������� �����
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

	struct List list_surplus_moves; // ������ ��������� ����� (������������ ���� �� ������� ����)


#if SAVE_LINE 
	struct Position position_save;// ������������� �����
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct PV_line pv_best_point;// ������ ��� ������� ���� �������
	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
	const unsigned __int64 key_undo = position.hash_key;   // ���������� ���-���� �������� ����
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
	Generator_moves::generator_captures_white(list_surplus_moves,position);
	Generator_moves::generator_move_white(list_surplus_moves,position);
	Sorting_moves::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Sorting_moves::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY

	//Sorting_white_moves_root(&root_list_surplus_moves);// ��������� ����

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_LIST && SAVE_LINE)
	list_print(depth_max,list_surplus_moves);
	History::test_print(depth_max);
	//if(depth_max == 4) List_Print(depth,list_surplus_moves);
	//Bitboard_print(bitboard);
#endif//#if TEST_LIST
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------
#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	Hash::look_hash_move(list_surplus_moves,position.hash_key
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

	Search::ini_flag_stop_null_move(1);

#endif//#if SAVE_LINE 


	//����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		Make_move::do_moves_white(position,i,depth,list_surplus_moves);
		//Bitboard_print(bitboard);

		// ���� ������ ��� ����� �� (�������� ����� ���������� ��� �������� ������)
		if (Generator_moves::king_white_check(position) != CHECK_NO)
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

		depth_max_new = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
		if (Generator_moves::king_black_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES; 
			depth_max_new = depth_max + 1;
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

		// �������� ���������� � ��������������� ���� � ������� ��������
		PV_save::print_currmove(list_surplus_moves,nodes_0,i,depth_max);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� 
		// �� ������� �������
		PV_save::update_PV_current(i,depth,list_surplus_moves);

		// ��������� ��� ����
		ZObrist::modification_random_key(position,1,i,list_surplus_moves
										,Make_move::get_d_undo(depth));

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		ZObrist::save_hash_three(position.hash_key,new_depth);

#endif//#if SAVE_LINE 



//_scout_scout white
/*
 ����� � ��� ������������ ���������(NegaScout)
 ������ ����� ������� ������� � ������� � ������ ����� ��������� ��������
 ������ ������� � ������� ����� � ���� ����� ������ ���������� �� �������������
 � ������ �����
*/
//#if (ALPHA_BETA_ON && SAVE_LINE)
//		if (i == 0)
//#else
//		// � ������ ������� �������� ������� ��� � ��� ����� ������� ��������
//		if (1)
//#endif//#if ALPHA_BETA_ON
//		{
//			// ������ ����� ������� ������� � ������� � ������ �����
//			beta_scout = beta;
//			flag_pv_f_scout = PV_YES;
//		}
//		else
//		{
//			// �������� �������  � ������� �����. ��� ��� �� ������� �������
//			beta_scout = alpha + 1;
//			flag_pv_f_scout = PV_NO;
//		}
//
//		//����������� ����� �������
//		value = Search::black(position,alpha,beta_scout,depth_max_new,new_depth,description_move,
//							 flag_pv_f_scout,flag_check,NULL_NO);
//
//#if (ALPHA_BETA_ON && SAVE_LINE)
//		if( (value > alpha) && (flag_pv_f_scout == PV_NO) )
//		{
//			// ���� ����� alpha ������� �� ������� �� ����� ��� ������� �������
//			beta_scout = beta;
//			flag_pv_f_scout = PV_YES;
//
//			//����������� ����� �������
//			value = Search::black(position,alpha,beta_scout,depth_max_new,new_depth,description_move,
//								 flag_pv_f_scout,flag_check,NULL_NO);
//		}
//
//#endif//#if ALPHA_BETA_ON

unsigned __int8 depth_max_new_scout = depth_max_new;// ������������ �������
bool search_scout;

//_scout_scout white
#if (ALPHA_BETA_ON && SAVE_LINE)
		if ( i == 0 ) 
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

			value = Search::black(position,alpha,beta_scout,depth_max_new_scout,new_depth
							,description_move,
							flag_pv_f_scout,flag_check,NULL_NO);
		}
		else
		{
			//
			search_scout = true;

#if (LMR && SAVE_LINE)

			// ������� �� �������� LMR
			
			if (
			   (description_move == MOVE_IS_SIMPLE)// ������� ���
			&& (flag_check == CHECK_NO)  // ��� �� ���. ���������� ������ ����!
			&& ( depth_max_new >= LMR_DEPTH ))// ����������� �������
			{
				// ���� � ��������� ������� � � ������� �����
				beta_scout = alpha + 1;
				flag_pv_f_scout = PV_NO;
				depth_max_new_scout = depth_max_new - LMR_REDUCTION_D;

				//
				value = Search::black(position,alpha,beta_scout,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,NULL_NO);
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
				value = Search::black(position,alpha,beta_scout,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,NULL_NO);

				// ��� �������� �������� ������������� � ������ ����� � ��������
				// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
				// ����� ������� ������� ��� � ����������� ����� �.�. ������� �����
				// ���� ��� ���� ������� �� ������������� c ������ �����
				// ����� �� ����� ��������� ������ �������� ��������
				if ( value > alpha )
				{
					beta_scout = beta;
					flag_pv_f_scout = PV_YES;

					//
					value = Search::black(position,alpha,beta_scout,depth_max_new_scout,new_depth
									,description_move,
									flag_pv_f_scout,flag_check,NULL_NO);
				}
			}
		}

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
				PV_save::update_PV_best_point(pv_best_point);

				// TEST
				// ��������� ���������� �������� � ������
				if (value != PV_save::get_pv_best().score_move)
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
			PV_save::update_PV_best_point(pv_best_point);

			// TEST
			// ��������� ���������� �������� � ������
			if (value != PV_save::get_pv_best().score_move)
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
		if (Search::get_stop_search() != true)
		{
			nodes_0 = nodes_0 + 1;
#if TEST_PRINT_ALL_LINE
			// ��� ����� ���������
			PV_save::print_pv_line(1,list_surplus_moves,PV_save::get_pv_best()
								  ,nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			PV_save::print_pv_line(1,list_surplus_moves,pv_best_point
								  ,nodes_root,i,depth_max,value_max,0);
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
		if (Generator_moves::king_white_check(position) != CHECK_NO)
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
	// �������� ������ ���
	Hash::save_best_hash_score_or_move(i_hash_move, position.hash_key
									  ,list_surplus_moves, 
									  value_max, UPPER, depth, depth_max);

#endif//#if HASH


	return value_max;

}

//	===================================================================
// ������� �� ������� ������ �� ������ ���� �������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 Search_root::black_root
(
	struct Position & position,// ������������� �����
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

	//Generator_moves::mobility_evaluation(position);

	// ����������� ������ ���������� �����
	// ��������� ���� ������� � ������� ������ ��������
	Generator_moves::generator_captures_black(list_surplus_moves,position);
	Generator_moves::generator_move_black(list_surplus_moves,position);
	Sorting_moves::sorting_moves_captures(list_surplus_moves);

#if SW_HISTORY
	Sorting_moves::sorting_moves_history(list_surplus_moves);
#endif//#if SW_HISTORY

	//Sorting_black_moves_root(&root_list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
#if (TEST_LIST && SAVE_LINE)
	list_print(depth_max,list_surplus_moves);
	History::test_print(depth_max);
	//if (depth_max == 3) 
		//List_Print(depth,list_surplus_moves);

	//	Bitboard_print(bitboard);
#endif//#if TEST_LIST
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	Hash::look_hash_move(list_surplus_moves,position.hash_key
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


	Search::ini_flag_stop_null_move(1); // ����� �� �������


	// ����� �� ������ ���������� �����
	// ����� � ����� �����; ������ ���� ���������� � ����������� ������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{
		// ��������� ��� �� ������ ��� ������� ������� ���������� �������
		Make_move::do_moves_black(position,i,depth,list_surplus_moves);
		//Bitboard_print(bitboard);

		// ���� ������ ��� ����� ��
		if (Generator_moves::king_black_check(position) != CHECK_NO)
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

		depth_max_new = depth_max;
//-----------------------------------------------
#if (EXTENSION_CHEK && SAVE_LINE)
		// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
		if(Generator_moves::king_white_check(position) != CHECK_NO)
		{
			flag_check = CHECK_YES;
			depth_max_new = depth_max + 1;
		}
		else
		{
			flag_check = CHECK_NO;
		}
#endif//#if EXTENSION_CHEK
//-----------------------------------------------

		const __int32 description_move = ( list_surplus_moves.move[i] >> 6 ) & 63;

#if SAVE_LINE

		// �������� ���������� � ��������������� ���� � ������� ��������
		PV_save::print_currmove(list_surplus_moves,nodes_0,i,depth_max);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
		PV_save::update_PV_current(i,depth,list_surplus_moves);

		// ��������� ��� ����
		ZObrist::modification_random_key(position,0,i,list_surplus_moves
										,Make_move::get_d_undo(depth));

		// TEST-----------
		// ��� ���� ��������� ��������� �� �����
		// ���������� ���-�������
		// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
		//HASHM::public_start_position_random_key(bitboard);
		//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
		ZObrist::save_hash_three(position.hash_key,new_depth);

#endif//#if SAVE_LINE 


////_scout_scout black
//#if (ALPHA_BETA_ON && SAVE_LINE)
//		if (i == 0)
//#else
//		if (1)
//#endif//#if ALPHA_BETA_ON
//
//		{
//			alpha_scout = alpha;
//			flag_pv_f_scout = PV_YES;
//		}
//		else
//		{
//			alpha_scout = beta - 1;
//			flag_pv_f_scout = PV_NO;
//		}
//
//		value = Search::white(position,alpha_scout,beta,depth_max_new,new_depth,description_move,
//							 flag_pv_f_scout,flag_check,NULL_NO);
//
//#if (ALPHA_BETA_ON && SAVE_LINE)
//		if ( (value < beta) && (flag_pv_f_scout == PV_NO) )
//		{
//			alpha_scout = alpha;
//			flag_pv_f_scout = PV_YES;
//
//			value = Search::white(position,alpha_scout,beta,depth_max_new,new_depth,description_move,
//								 flag_pv_f_scout,flag_check,NULL_NO);
//		}
//#endif//#if ALPHA_BETA_ON


unsigned __int8 depth_max_new_scout = depth_max_new;// ������������ �������
bool search_scout;

//_scout_scout black
#if (ALPHA_BETA_ON && SAVE_LINE)
		if (i == 0) 
#else
		if (1)
#endif//#if ALPHA_BETA_ON
		{
			alpha_scout = alpha;//
			flag_pv_f_scout = PV_YES;// �������� �� ��� ������� (pv)
	
			value = Search::white(position,alpha_scout,beta,depth_max_new_scout,new_depth,description_move,
							flag_pv_f_scout,flag_check,NULL_NO);
		}
		else
		{
			//
			search_scout = true;

#if (LMR && SAVE_LINE)
			// ������� �� �������� LMR

			if (
			   (description_move == MOVE_IS_SIMPLE)
			&& (flag_check == CHECK_NO)
			&& ( depth_max_new >= LMR_DEPTH ))
			{
				alpha_scout = beta - 1;//
				flag_pv_f_scout = PV_NO;//
				depth_max_new_scout = depth_max_new - LMR_REDUCTION_D;// ������������ �������

				value = Search::white(position,alpha_scout,beta,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,NULL_NO);
				//
				search_scout = (value < beta);

				depth_max_new_scout = depth_max_new;

			}
#endif//#if  LMR

			if (search_scout)
			{
				alpha_scout = beta - 1;//
				flag_pv_f_scout = PV_NO;//

				value = Search::white(position,alpha_scout,beta,depth_max_new_scout,new_depth
								,description_move,
								flag_pv_f_scout,flag_check,NULL_NO);

				if ( value < beta )
				{
					alpha_scout = alpha;//
					flag_pv_f_scout = PV_YES;//

					value = Search::white(position,alpha_scout,beta,depth_max_new_scout,new_depth
									,description_move,
									flag_pv_f_scout,flag_check,NULL_NO);
				}
			}
		}

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
					std::cout << "ERROR black value " ;
					std::cout << "\n";
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
				std::cout << "ERROR black value " ;
				std::cout << "\n";
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
			PV_save::print_pv_line(0,list_surplus_moves,PV_save::get_pv_best(),nodes_root,i,depth_max,value,1);
#else
			// ��� �������� ������
			PV_save::print_pv_line(0,list_surplus_moves,pv_best_point,nodes_root
								  ,i,depth_max,value_min,0);
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
		if (Generator_moves::king_black_check(position) != CHECK_NO)
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
	//�������� ������ ���
	Hash::save_best_hash_score_or_move(i_hash_move,position.hash_key
									  ,list_surplus_moves, 
									   value_min, UPPER, depth, depth_max);
#endif//#if HASH

	return value_min;
}

//	===================================================================
// ���������� ������ �������
struct PV_line & Search_root::get_pv_best()
{
	return PV_save::get_pv_best();
}

//	===================================================================
// ���������� ���������� ������������� �����
__int64 Search_root::get_nodes()
{
	return (nodes_root + Search::get_nodes());// ���������� �����;
}

//	===================================================================
// �������� ���������� ������������� �����
void Search_root::set_nodes_in_0()
{
	nodes_root = 0;// ���������� �����;
	Search::set_nodes_in_0();
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
#if TEST_LIST

void Search_root::list_print
(
	int depth,// ������� �������
	struct List & list_surplus_moves// ������ ��������� �����
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

	for (int i = 0 ; i < list_surplus_moves.end_list ; i++)
	{	
		Test_Loop << "����� ���� �� ������ i= " << i << "\n";  
		Test_Loop << "������� ������ " << PV_save::int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
		Test_Loop << "������ ���� " << list_surplus_moves.sorting_score[i] << "\n";

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Loop << " " << (PV_save::x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.move[i]>>12)& 63)/8);//<< "\n";
		//Test_Loop<< "\n";
		Test_Loop << " " << (PV_save::x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8));
		Test_Loop << "" << (1 + ((list_surplus_moves.move[i]>>18)& 63)/8)<< "\n";   
		//------------------------------
		//��� ���� 
		//0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
		//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
		//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
		//25 - ������ � ������������ � ����� 
		Test_Loop << "��� ���� " << ((list_surplus_moves.move[i]>>6)& 63)  << "\n";
		Test_Loop << "������ ������ " << PV_save::int_to_char_board(( list_surplus_moves.move[i] & 63),1)<< "\n";
		Test_Loop << "��������� ��������� " << ((list_surplus_moves.move[i]>>12)& 63)  << "\n";
		Test_Loop << "�������� ��������� " << ((list_surplus_moves.move[i]>>18)& 63)  << "\n";
     
		Test_Loop << "\n";
	}

	Test_Loop << "list_surplus_moves.end_captures= " <<int(list_surplus_moves.end_captures) << "\n";
	Test_Loop << "list_surplus_moves.end_list= " <<int(list_surplus_moves.end_list) << "\n";
	Test_Loop << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop.close();
}
#endif