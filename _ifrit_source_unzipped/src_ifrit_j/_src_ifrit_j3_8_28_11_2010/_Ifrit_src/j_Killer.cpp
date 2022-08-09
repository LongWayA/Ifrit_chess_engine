#include <fstream>  // TEST

#include "j_Killer.h"
#include "k_Util.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ���������  25.02.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 80%

 ��������� �������
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 std::ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

namespace 
{
	__int32 killer_moves_1[MAX_DEPTH];
	__int32 killer_moves_2[MAX_DEPTH];
}

//	===================================================================
// �������������� �������
void SKILLER::public_ini_killer()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH; i++ )
	{
		killer_moves_2[i] = 0;
		killer_moves_1[i] = 0;
	}
}

//	===================================================================
// ����� �������
void SKILLER::public_save_killer
(
	const unsigned __int8 i,//����� ���� � ������
	class List_surplus_moves * p_list_surplus_moves,// ������ �����
	const unsigned __int8 depth// ������� �������
)
{
	if (killer_moves_1[depth] != p_list_surplus_moves->get_move(i))
	{
		killer_moves_2[depth] = killer_moves_1[depth];
		killer_moves_1[depth] = p_list_surplus_moves->get_move(i);
	}
}

//	===================================================================
// ��������� ������� � ������ ������(������� start) 
void List_surplus_moves::public_insert_killer
(
	const unsigned __int8 depth,// ������� �������
	unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
)
{
	__int32 move_s = 0;
	__int32 score_s;// ���������� ��� ����������� ������ ����
	unsigned __int8 j;

	// 1) ���� ������ 1 ��� � ������
	for ( j = start
	// ������ �� ��� ��� ���� �� ����� ������ ������ ��� ��� �� �������� ������
	; ( (move[j] != killer_moves_1[depth]) 
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
	; ( (move[j] != killer_moves_2[depth]) 
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
