#include <fstream>  // TEST
#include <iostream> // TEST
//#include <stdlib.h>

#include "a_List_surplus_moves.h"

#define TEST_H 0 

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ���������  28.11.2010 12.12.2010
*/
//---------------------------------------------------------------------------

/*
   ������� ����� ������� 

*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 std::ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// ��������� ��� ��� �� ������ ������� � ������
// ���� ��� ������� �� flag_insert_hash_move 2 ����� ���� 1
void List_surplus_moves::insert_hash_moves
(
	__int32 hash_moves,//
	unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
)
{
	__int32 move_s = 0;
	__int32 score_s;// ���������� ��� ����������� ������ ����
	unsigned __int8 j;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    
	static int kolizii = 0; 
	static int enter = 0; 
	int pozition = 0;

	int move_print;

	enter = enter + 1;

#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	flag_insert_hash_move = 1;// � ������� �����

	// 1 ���� ���-��� � ������
	for (j = 0; 
	( (move[j] != hash_moves) 
	&& (j < end_list) ); 
	j++);


// ���� ������ � ������������. ����� ����� ����������
#if TEST_H

	//--------------------
	if (j == end_list)
	//if (1)
	{	
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
		Test_Hash.open ("_TEST_HASH.txt",std::ios::app);
		//Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

		for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++)
		{
			if(p_hash_moves[ihm].key_lock_32_move != -1) pozition = pozition + 1;
		}

		Test_Hash << "����� ������� "<< max_index_hash <<"\n";
		Test_Hash << "��� ������� ������������ ������� "<< pozition <<"\n";

		move_print = p_hash_moves[ind].move;

		kolizii = kolizii + 1;

		Test_Hash << "��������� "<< enter <<"\n";
		Test_Hash << "�������� "<< kolizii <<"\n";
		Test_Hash << "depth "<< depth <<"\n";
		Test_Hash << "p_hash_moves[].move_delta_depth " << p_hash_moves[ind].move_delta_depth << "\n";
		//Test_Hash << "key "<< key <<"\n";
		Test_Hash << "p_hash_moves[].key " << p_hash_moves[ind].key_lock_32_move << "\n";
		Test_Hash << "p_hash_moves[].move "<< move_print <<"\n";

		//------------------------------------------
		Test_Hash << "������� ������ " << UTIL::public_int_to_char_board(((move_print>>24)& 7),1)<< "\n";
		Test_Hash << "p_hash_moves[].score_move " << p_hash_moves[ind].score << "\n";

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Hash << " " << (UTIL::public_x_to_notation(((move_print>>12)& 63)%8));
		Test_Hash << "" << (1 + ((move_print>>12)& 63)/8);//<< "\n";
		//Test_Hash<< "\n";
		Test_Hash << " " << (UTIL::public_x_to_notation(((move_print>>18)& 63)%8));
		Test_Hash << "" << (1 + ((move_print>>18)& 63)/8)<< "\n";   
		//------------------------------
		//��� ���� 
		//0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
		//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
		//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
		//25 - ������ � ������������ � ����� 
		Test_Hash << "��� ���� " << ((move_print>>6)& 63)  << "\n";
		Test_Hash << "������ ������ " << UTIL::public_int_to_char_board(( move_print & 63),1)<< "\n";
		//Test_Hash << "��������� ��������� " << ((move_print>>12)& 63)  << "\n";
		//Test_Hash << "�������� ��������� " << ((move_print>>18)& 63)  << "\n";
     
		Test_Hash << "\n";
		//------------------------------------------

		Test_Hash << "end_list= "<< end_list <<"\n";
		Test_Hash << "j= "<< j <<"\n";
		Test_Hash << "-----------------------------------"<<"\n";

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
		Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}	
#endif 


	// ���� ����� ��� ������� �� ���� ������
	if (j < end_list)
	{	
		//------------------------------------------------------------------------
		// 2 ���������� ���-��� 
		move_s  = move[j];
		score_s = sorting_score[j];
		//------------------------------------------------------------------------

		// 3 �������� ��� ������� ����
		// �� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
		for (unsigned __int8 i = j; i > 0; i--)
		{	
			move[i] = move[i-1];
			sorting_score[i] = sorting_score[i-1];
		}	

		//------------------------------------------------------------------------
		// 4 �� ��������� ������� ���������� ���-���
		move[0] = move_s;
		sorting_score[0] = score_s;
		//------------------------------------------------------------------------

		flag_insert_hash_move = 2;// ��� �������
	}
}


//	===================================================================
// ��������� ������� � ������ ������(������� start) 
void List_surplus_moves::insert_killer_in_list
(
	__int32 killer_moves_1,// 
	__int32 killer_moves_2,//
	unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
)
{
	__int32 move_s = 0;
	__int32 score_s;// ���������� ��� ����������� ������ ����
	unsigned __int8 j;

	// 1) ���� ������ 1 ��� � ������
	for ( j = start
	// ������ �� ��� ��� ���� �� ����� ������ ������ ��� ��� �� �������� ������
	; ( (move[j] != killer_moves_1) 
	&& (j < end_list) )
	; j++  );

	// ���� ����� ��� ������� �� ���� ������
	if (j < end_list)
	{	
		//2) ���������� ������ ��� 
		move_s  = move[j];
		score_s = sorting_score[j];

		//3) �������� ��� ������� ����
		//�� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
		for (unsigned __int8 i = j; i > start; i--)
		{	
			move[i] = move[i-1];
			sorting_score[i] = sorting_score[i-1];
		}

		//4) �� ��������� ������� ���������� �������
		move[start] = move_s;
		sorting_score[start] = score_s;
	}

	start = start + 1;

	// 1 ���� ������ 2 ��� � ������
	for ( j = start
	// ������ �� ��� ��� ���� �� ����� ������ ������ ��� ��� �� �������� ������
	; ( (move[j] != killer_moves_2) 
	&& (j < end_list) )
	;j++);

	// ���� ����� ��� ������� �� ���� ������
	if (j < end_list)
	{	
		//2) ���������� ������ ��� 
		move_s  = move[j];
		score_s = sorting_score[j];

		//3) �������� ��� ������� ����
		//�� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
		for (unsigned __int8 i = j; i > start; i--)
		{	
			move[i] = move[i-1];
			sorting_score[i] = sorting_score[i-1];
		}

		//4) �� ��������� ������� ���������� �������
		move[start] = move_s;
		sorting_score[start] = score_s;


		// TEST
		// � ��������� ��������� ������� ��� ����. 
		// �� ��� �� ������ ����� ������ ��� ����� ��� ����� ������!
		// ���-��� ��� ��������.
		// ������� �� ������ �� �������� �����������. 
		// ���� ����� ��������� ��� � �������

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
		Test_Killer.open ("_TEST_KILLER.txt",std::ios::app);
		//Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if TEST_K
		Test_Killer << "���������� �������"<< "\n";
		Test_Killer << "start= "<< start << "\n";
		//Test_Killer << "j= "<< j << "\n";
		//Test_Killer << "i= "<< i << "\n";
		//Test_Killer << "i_end= "<< i_end << "\n";
		Test_Killer << "depth= "<< depth << "\n";
		Test_Killer << "end_list[depth]= "<< end_list << "\n";
		Test_Killer << "killer_moves[depth].initial_position= "<< killer_moves_1[depth] << "\n";
		Test_Killer << "killer_moves[depth].final_position= "<< killer_moves_2[depth] << "\n";
		Test_Killer << "                "<< "\n";
		Test_Killer <<" "<<(UTIL::public_x_to_notation(killer_moves_1[depth]%16));
		Test_Killer <<""<<(8-killer_moves_2[depth]/16);
		Test_Killer <<" "<<(UTIL::public_x_to_notation(killer_moves_2[depth]%16));
		Test_Killer <<""<<(8-killer_moves_2[depth]/16) << "\n";
#endif 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
		Test_Killer.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}
}
