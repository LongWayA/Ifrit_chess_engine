
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 25.02.2009 11.12.2010

#include <fstream>

#include "k_Killer.h"


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 std::ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// �������������� �������
void k_Killer::ini_killer()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH; i++ )
	{
		killer_moves_2[i] = 0;
		killer_moves_1[i] = 0;
	}
}

//	===================================================================
// ����� �������
void k_Killer::save_killer
(
	const unsigned __int8 i,//����� ���� � ������
	struct List & list,// ������������� ������ �����
	const unsigned __int8 depth// ������� �������
)
{
	if (killer_moves_1[depth] != list.move[i])
	{
		killer_moves_2[depth] = killer_moves_1[depth];
		killer_moves_1[depth] = list.move[i];
	}
}


//	===================================================================
// ��������� ������� � ������ ������(������� start) 
void k_Killer::insert_killer
(
	struct List & list,// ������������� ������ �����
	const unsigned __int8 depth,// ������� �������
	unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
)
{
	insert_killer_in_list(list,killer_moves_1[depth],
							killer_moves_2[depth],
							start);
}


//	===================================================================
// ��������� ������� � ������ ������(������� start) 
void k_Killer::insert_killer_in_list
(
	struct List & list,// ������������� ������ �����
	const __int32 killer_moves_1,// 
	const __int32 killer_moves_2,//
	const unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
)
{
	__int32 move_s = 0;
	__int32 score_s;// ���������� ��� ����������� ������ ����
	unsigned __int8 j;

	// 1) ���� ������ 1 ��� � ������
	if( killer_moves_1 != 0 )
	{
		for ( j = start
		// ������ �� ��� ��� ���� �� ����� ������ ������ ��� ��� �� �������� ������
		; ( (list.move[j] != killer_moves_1) 
		&& (j < list.end_list) )
		; j++  );

		// ���� ����� ��� ������� �� ���� ������
		if (j < list.end_list)
		{	
			//2) ���������� ������ ��� 
			move_s  = list.move[j];
			score_s = list.sorting_score[j];

			//3) �������� ��� ������� ����
			//�� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
			for (unsigned __int8 i = j; i > start; i--)
			{	
				list.move[i] = list.move[i-1];
				list.sorting_score[i] = list.sorting_score[i-1];
			}

			//4) �� ��������� ������� ���������� �������
			list.move[start] = move_s;
			list.sorting_score[start] = score_s;
		}
	}

	if( killer_moves_2 != 0 )
	{
		const unsigned __int8 start_2 = start + 1;

		// 1 ���� ������ 2 ��� � ������
		for ( j = start_2
		// ������ �� ��� ��� ���� �� ����� ������ ������ ��� ��� �� �������� ������
		; ( (list.move[j] != killer_moves_2) 
		&& (j < list.end_list) )
		;j++);

		// ���� ����� ��� ������� �� ���� ������
		if (j < list.end_list)
		{	
			//2) ���������� ������ ��� 
			move_s  = list.move[j];
			score_s = list.sorting_score[j];

			//3) �������� ��� ������� ����
			//�� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
			for (unsigned __int8 i = j; i > start_2; i--)
			{	
				list.move[i] = list.move[i-1];
				list.sorting_score[i] = list.sorting_score[i-1];
			}

			//4) �� ��������� ������� ���������� �������
			list.move[start_2] = move_s;
			list.sorting_score[start_2] = score_s;


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
}