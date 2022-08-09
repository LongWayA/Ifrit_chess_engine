#include <iostream>
#include <fstream>  // TEST

#include "c_Iterative_Deepening.h"
#include "k_Util.h"
#include "k_Timer.h"
#include "h_Evaluate.h"
#include "j_Hash.h"
#include "d_Bitboard_gen.h"

#include "j_ZObrist.h"
#include "g_Search_root.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 10.02.2008  11.11.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

������� � ����������� ����������� �������
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


namespace 
{
	__int64 nodes_y = 0; // ���������� ����� �� ������ �������
	__int64 nodes_previous = 0; // ���������� ������ ���������� �����
}

//	===================================================================
//
void GO::public_deepening
(
	struct T_Bitboard * p_bitboard,//������������� �����
	unsigned __int8 depth_max,//������� ��������
	double wtime,//���������� ����� �� ���� �� �����
	double btime,//���������� ����� �� ���� �� ������
	double winc,//���������� ������� �� ��� ��� �����
	double binc,//���������� ������� �� ��� ��� ������
	unsigned __int8 movestogo,//���������� ����� �� ������� �������� �����
	double movetime,//����� �� ���
	bool infinite//����������� ������
)
{

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
	//Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	// ����� ������� � ��������
	double time_terminate = -1.0;// ����� ���������� �� ���
	double time_current = -1.0;  // ������� ������� ������ ��� ������ ��������

	__int32 Alpha = -9999999;
	__int32 Beta  =  9999999;

	__int8 inv = 0;// ���� ����� ����
	__int32 value_root = 0;// ������ ������� �������� � ��������� ����. 
	__int32 d_m = 0; // ��� ����. ����� ��� ����������� ���� ������ � ������������
	__int32 test_eval = 0;// ���������� ������ ��������� ��� ��������
	bool flag_mate = 0;// ������ ��� 
	unsigned __int64 key_control = 0;   // ���������� ���-���� ��� ��������
	struct T_list_surplus_moves root_list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)

	__int64 nodes = 0;// ���������� ������������� �����(�������)
	unsigned __int16 cnt_move ;// ���������� ��� ��������� � ������ ���������

	unsigned __int16 limit_cnt_move ;// ������� ����� �������� ������� (�������������� �����)

	//-------------------------------------------------------------------------------
	//             > go btime 302000 wtime 302000 winc 2000 binc 2000 
	// 5 min * 60 = 300 sek
	// ����� � ��� �������� � �������� ���� �� �������� � ������������� �� ��
	// ��� ������ �������� �� 1000
	// �������� ����� �� ���(���� ������� ��� �����������). 
	// ������ ������������� ���������� � ����. � ������ ���� ����� � ����� ������
	// ��� ��� � ������� ��� ����� ������� ���������� �� ��� ������� ������ ���� ������ � ���
	//if (p_bitboard->colour_move == 1)// 1-��� ����� 0- ��� ������
	//}
	//	if (wtime != -1)
		//{
	//		time_terminate = wtime/100;
		//}
	//}
	//else
	//{
	//	if (btime != -1)
		//{
	//		time_terminate = btime/100;
		//}
	//}

	// ���������� ��������� ��������� � ������
	cnt_move = ZOBRIST::public_get_save_cnt_move();

	// �� ������������ ��� ��� ������ ���� �� ������������ ���������� ����� (80)
	// ��� ���������� ���������� �����
	limit_cnt_move = 80 - cnt_move/2 ;

	// ����� �� ������ 40 ����� �������� ����������
	// ����� 40 �� ���� ���� ���������� ������� �� ��� ��� �� �������� ��������� �� �������
	if (limit_cnt_move < 40)
	{
		limit_cnt_move = 40;
	}

	// �������� �� ����������� �������� �� ������ ������� �.�. 
	// ��������� ������ ����� ��������� ��������� �������
	limit_cnt_move = 3 * limit_cnt_move;


	if (p_bitboard->colour_move == 1)// 1-��� ����� 0- ��� ������
	{
		if (wtime != -1)
		{
			time_terminate = wtime/limit_cnt_move;
		}
	}
	else
	{
		if (btime != -1)
		{
			time_terminate = btime/limit_cnt_move;
		}
	}


	// TEST
	//std::cout << "wtime = " << wtime << "\n";
	//std::cout << "btime = " << btime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";
	//std::cout << "\n";

	// TEST
	//wtime = 60; // 5 min

	//while (wtime > 1)
	//{
	//	limit_cnt_move = 80 - cnt_move/2 ;
	//	if (limit_cnt_move < 40 )
	//		limit_cnt_move = 40;

	//	//limit_cnt_move = 3 * limit_cnt_move;
	//	time_terminate = wtime/limit_cnt_move;
	//
	//	std::cout << "cnt_move = " << cnt_move << "\n";
	//	std::cout << "wtime = " << wtime << "\n";
	//	std::cout << "time_terminate = " << time_terminate << "\n";
	//	//std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";
	//	std::cout << "   " << "\n";
	//
	//	wtime = wtime - time_terminate;
	//	cnt_move = cnt_move + 1;
	//}


	// TEST
	//std::cout << "cnt_move = " << cnt_move << "\n";
	//std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";


	//             > go movetime 300000 
	//������ ����� �� ��� (���� ������� ��� �����������).
	if (movetime != -1)
	{
		time_terminate = movetime;
	}
	//--------------------------

	// �� ������� �������� ������ ��� ������������ ���������� ������� �� ������
	ROOTSEARCH::public_ini_deep_time_terminate( (time_terminate * 50) );

	//���� ����� ��������� ���� ����������� ��
	//����������� �� ������� 50
	if (time_terminate != -1)
	{
		depth_max = 50;
	}
	//--------------------------

	//             > go infinite
	// ������ ��� ����������� �������. 50 ���� ����� ����� ����������
	if (infinite == true)
	{
		depth_max = 50;
	}
	//--------------------------

	// ���� ����� ��� ��������� �� ����������������  
	if (depth_max == 0)
	{
		depth_max = 1;
	}
	//-------------------------------------------------------------------------------

	// TEST
	//std::cout << "movetime = " << movetime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout << "depth_max = " << depth_max << "\n";

	// ������� ��������. � ���������� ������ ������������ ������ ��� �������.
	p_bitboard->value_white_material = EVALUATE::public_eval_white_material(p_bitboard);

	// ������� ��������. � ���������� ������ ������������ ������ ��� �������.
	p_bitboard->value_black_material = EVALUATE::public_eval_black_material(p_bitboard);

	// ��� �������� ��� ����� ����������� ������ ��������� � ��������������� ���������
    test_eval =  p_bitboard->value_white_material - p_bitboard->value_black_material;

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << "\n";

	// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
	ZOBRIST::public_start_position_random_key(p_bitboard);

	// ���������� ���� ��� �������� ��� ����������� � �������
	key_control = p_bitboard->hash_key;

	//�������������� ��� - ������ �������� ������������� ������� ��� �� ��������� ������ ������������
	ZOBRIST::public_ini_hash_three(); 

	// ���������� ���� ��� ������������ ������� �� ������ �������
	ZOBRIST::public_save_hash_three(p_bitboard->hash_key,0);

	// TEST
	// ����� ������� ���-�������
	// �������������� ��� ����� � ���-������� 0
	//   HASHM::Ini_hash_moves();

	// ��������� ��� ������������ �������������� ��������
	// TEST
	//depth_max = depth_max - 1;

	// TEST
	//std::cout << "1111111 " << "\n";

	// �������������� ���������� ���������� ������ loop
	ROOTSEARCH::public_ini_global_loop(depth_max);

	if (p_bitboard->colour_move == 1)
	{  
		// ����� 
		//��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
		BITBOARDGENERATOR::public_generator_captures_white(p_bitboard,&root_list_surplus_moves);// ���������� ������ � �����������
		BITBOARDGENERATOR::public_generator_move_white(p_bitboard,&root_list_surplus_moves);// ���������� ��������� ����
	}
	else
	{
		// ������
		//��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
		BITBOARDGENERATOR::public_generator_captures_black(p_bitboard,&root_list_surplus_moves);// ���������� ������ � �����������
		BITBOARDGENERATOR::public_generator_move_black(p_bitboard,&root_list_surplus_moves);// ���������� ��������� ����
	}

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

	// ���� ������� ������ ������������ � �� ����� ��� � ������ 50 � �� ���� ������� �� ��������� �� ���������� ����
	for (unsigned __int8 depth = 1;

	// ��� ������� �� ����� �����
	( (depth <= depth_max) && // ������� ������ ������������ �������� �� ���� �������
	(flag_mate == 0) && // ���� ����
	(depth < 51) && // ������� ������ ������������ 
	(ROOTSEARCH::public_get_stop_search() == 0)// ��� ������� �� ��������� ����� 
	);

	depth++
	)
	{

		// ���������� ������ ���������� ����� � ���������� �������� �.� �������
		nodes_previous = nodes;

		//���������� ������� �������
		ROOTSEARCH::public_set_nodes_in_0();

		// ��������� ������
		TIMER::public_time_start();

		if (p_bitboard->colour_move == 1)
		{
			value_root = ROOTSEARCH::public_white_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
		}
		else
		{
			value_root = ROOTSEARCH::public_black_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
		}

		// ������������� ������
		TIMER::public_time_stop();
		time_current = TIMER::public_time_delta();

		nodes = ROOTSEARCH::public_get_nodes();

		// ���������� ����� �� ���� ������� ��� ������ ���������� �����
		// ����� ������ ���������� ����� �� ���������� ��������
		nodes_y = nodes - nodes_previous ;

		// �������� ������ ���������

		if (test_eval != (p_bitboard->value_white_material - p_bitboard->value_black_material))
		{
			std::cout << "ERROR test_eval != bitboard->eval" << "\n";
		}
 
		//-----------------------------------------------
		// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� 
		// � ������� �� �������� � 
		// ��������� ������� �� ���� �� ���������� �� �������
		if (key_control != p_bitboard->hash_key)
		{
			std::cout <<"\n";
			std::cout << "ERROR HESH Test_Deepening " ;
			std::cout <<"\n";
		}
		//-----------------------------------------------

		//---------------------------------------------------------------
		// �������� ���������� � �����
		std::cout << "info " ;
		std::cout << " nps " << (__int32)(nodes / time_current);// �������� �� ������� �� ���� ����� �� ������ �������
		std::cout << " depth " << (__int32)depth;//�������

		// �������� ���������� � ������� �������������� ��������
		std::cout << " seldepth "<< (__int32)ROOTSEARCH::public_get_pv_best()->depth_max;

		p_bitboard->colour_move == 0 ? inv = -1 : inv = 1;

		// �������� ������. ��� ���� ����������� ������������� � ���� ���������
		if (inv * value_root > 900000)
		{ 
			flag_mate = 1;
			std::cout << " score mate "<< (__int32)ROOTSEARCH::public_get_pv_best()->depth_max;// ��������� �� ���� � ���������
		}
		else if (inv * value_root < -900000)
		{
			std::cout << " score mate "<< -(__int32)ROOTSEARCH::public_get_pv_best()->depth_max;// ��������� �� ���� � ���������
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

		for (unsigned __int8 n = 0; n < ROOTSEARCH::public_get_pv_best()->depth_max; n++)
		{
			std::cout << UTIL::public_x_to_notation(((ROOTSEARCH::public_get_pv_best()->move[n]>>12)& 63)%8);
			std::cout << (1 + ((ROOTSEARCH::public_get_pv_best()->move[n]>>12)& 63)/8);
			std::cout << UTIL::public_x_to_notation(((ROOTSEARCH::public_get_pv_best()->move[n]>>18)& 63)%8);
			std::cout << (1 + ((ROOTSEARCH::public_get_pv_best()->move[n]>>18)& 63)/8);

			// ��������� ��� ����
			d_m = ((ROOTSEARCH::public_get_pv_best()->move[n]>>6)& 63);

			// ���� d_m ������ 5 �� ������ ����� ����������� �����
			if (d_m > 5)
			{
				if (d_m > 15)
				{
					std::cout << UTIL::public_int_to_char_board((d_m - 20),0);// ����������� ����� � �������
				}
				else
				{
					std::cout << UTIL::public_int_to_char_board((d_m - 10),0);// ������ ����������� �����
				}
			}

			std::cout << " ";
		}

		std::cout <<"\n";

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
		if ((time_terminate != -1) && (time_terminate < time_current) && (depth >= 4))
		{
			break;
		}

	}

	// TEST
	//std::cout << "cnt_move = " << cnt_move << "\n";
	//std::cout << "wtime = " << wtime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout <<"time_current = "<< time_current <<"\n";
	//std::cout <<"   " <<"\n";

	//-----------------------------------------------
	// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
	// ��������� ������� �� ���� �� ���������� �� �������
	if (key_control != p_bitboard->hash_key)
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening " ;
		std::cout <<"\n";
	}
	//-----------------------------------------------


	//---------------------------------------------------------------------
	// ������ ��� ���� < bestmove e2e4 
	// ��� ��� ����������� � ����(� �������)  < bestmove d7d8N 
	std::cout << "bestmove ";
	std::cout << UTIL::public_x_to_notation( ROOTSEARCH::public_get_from() % 8);
	std::cout << (1 + ROOTSEARCH::public_get_from() / 8);
	std::cout << UTIL::public_x_to_notation( ROOTSEARCH::public_get_to() % 8);
	std::cout << (1 + ROOTSEARCH::public_get_to() / 8);

	// ��������� ��� ����
	d_m = ((ROOTSEARCH::public_get_pv_best()->move[0]>>6)& 63);

	// ���� d_m ������ 5 �� ������ ����� ����������� �����
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			std::cout << UTIL::public_int_to_char_board((d_m - 20),0);
		}
		else
		{
			std::cout << UTIL::public_int_to_char_board((d_m - 10),0);
		}
	}

	std::cout <<"\n";

	// �������� ������������ ������� ���� ������� ���� ����� ��������
	if (ROOTSEARCH::public_get_from() 
	!= ((ROOTSEARCH::public_get_pv_best()->move[0]>>12)& 63))
	{
		std::cout << "ERROR from(Deepening)" <<"\n";
	}

	if (ROOTSEARCH::public_get_to() 
	!= ((ROOTSEARCH::public_get_pv_best()->move[0]>>18)& 63))
	{
		std::cout << "ERROR to(Deepening)" <<"\n";
	}
//---------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}