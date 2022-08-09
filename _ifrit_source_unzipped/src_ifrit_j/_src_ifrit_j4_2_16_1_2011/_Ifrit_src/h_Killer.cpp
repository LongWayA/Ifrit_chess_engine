
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 25.02.2009 11.12.2010

#include <fstream>

#include "h_Killer.h"
#include "m_List_surplus_moves.h"

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 std::ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// �������������� �������
void Killer::ini_killer()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH; i++ )
	{
		killer_moves_2[i] = 0;
		killer_moves_1[i] = 0;
	}
}

//	===================================================================
// ����� �������
void Killer::save_killer
(
	const unsigned __int8 i,//����� ���� � ������
	class List_surplus_moves & list_surplus_moves,// ������ �����
	const unsigned __int8 depth// ������� �������
)
{
	if (killer_moves_1[depth] != list_surplus_moves.get_move(i))
	{
		killer_moves_2[depth] = killer_moves_1[depth];
		killer_moves_1[depth] = list_surplus_moves.get_move(i);
	}
}


//	===================================================================
// ��������� ������� � ������ ������(������� start) 
void Killer::insert_killer
(
	class List_surplus_moves & list_surplus_moves,// ������ �����
	const unsigned __int8 depth,// ������� �������
	unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
)
{
	list_surplus_moves.insert_killer_in_list(killer_moves_1[depth],
											 killer_moves_2[depth],
											 start);
}
