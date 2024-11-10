#include "a_Structure.h"

#define TEST_K 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ���������  25.02.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 80%

 ��������� �������

 ������ ��� ��� ������ ��� ��������� �� ����� �� ������ �������
 �� �� ������ ���� ������� �.�. � �������� �������� ������

 ��� ������ ������? 
 ��� ������  
 ��� �� ������� ����� � ������ ������� �� ����
 ������� ��� ��� ������ �� �����
 ��� ������ ������� ���� � ������ ������� �� �����

 ����� �� �������� ��� � ���� �� ��������(negamax) � ��������(minimax)

*/


namespace SKILLER 
{
	//---------------------------------------------------------------------------
	// �������������� ������ ��������
	void public_ini_killer();

	//---------------------------------------------------------------------------
	// ����� �������
	void public_save_killer
	(
		const unsigned __int8 i,// ����� ���� � ������
		const struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
		const unsigned __int8 depth// ������� �������
	);

	//---------------------------------------------------------------------------
	// ��������� ������� � ������ ������(������� start) 
	void public_insert_killer
	(
		struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
		const unsigned __int8 depth,// ������� �������
		unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
	);

}