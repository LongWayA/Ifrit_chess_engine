//---------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 21.07.2007
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 90%

 ������ ������� �� ������ � �������� ��������� �����
 ���� ������ ������� ����� ������� ������� ����!
 ������� ��� �� ���� ����� ������ ���������  And_chess ���������� �� �����(java) 
 � ������ 9.01.06
*/

class Util
{

//////////////////////////////////////////////////////////////////////////////////////////
public:

	//-----------------------------------------------------------------
	// ������� ����������� � ���� ��� ���� �� 9.01.06 
	// ������� ��� ������������ �������� :)) 
	// ������������ ����� ������ � �����
	// 0-��� ������, 1-�����, 2-����, 3-����, 4-�����, 5-�����, 6-������	
	__int8 int_to_char_board
	(
		const int i,// ��� ������
		const int z// ���� ������
	);

	//-----------------------------------------------------------------
	// ����� �� ���������-------------------------------------------------------
	// ������������ ����� ������ � ����� 
	// 0-��� ������, 1-�����, 2-�����, 3-����, 4-����, 5-�����, 6-������	
	int char_to_int_board
	(
		const __int8 f// ������ ������
	);

	//---------------------------------------------------------------------------
	//	�� ���������� � ����� ������ ����� ������� �������� 0 - �, 1 - b, ..., 7 - h	
	__int8 x_to_notation
	(
		const int x// �������������� ���������� ����� (0-7)
	);

};
