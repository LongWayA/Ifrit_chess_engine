#include "a_Structure.h"
#include "a_List_surplus_moves.h"

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


class Killer 
{

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////////
private:

	__int32 killer_moves_1[MAX_DEPTH];
	__int32 killer_moves_2[MAX_DEPTH];


//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// �������������� ������ ��������
	void ini_killer();

	//---------------------------------------------------------------------------
	// ����� �������
	void save_killer
	(
		const unsigned __int8 i,// ����� ���� � ������
		class List_surplus_moves * p_list_surplus_moves,// ������ �����
		const unsigned __int8 depth// ������� �������
	);

	//---------------------------------------------------------------------------
	// ��������� ������� � ������ ������(������� start) 
	void insert_killer
	(
		class List_surplus_moves * p_list_surplus_moves,// ������ �����
		const unsigned __int8 depth,// ������� �������
		unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
	);

};
