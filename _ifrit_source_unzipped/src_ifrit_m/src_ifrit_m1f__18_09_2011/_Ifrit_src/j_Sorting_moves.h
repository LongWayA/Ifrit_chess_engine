
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010
/// ������� ����� ������� 70%


///!
#include "a_Header_constants.h"
#include "a_List.h"

/// ��������-���������(1-0) �������� ������ �����
#define KONTROL_B 0 

/// 
#define TEST_H 0 





/// ���������� ������ �����
/// ��������� �� ��������������� ������ �����

class j_Sorting_moves 
{


//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	/// ������ ����������
	void sorting_moves_captures
	(
		struct List & list/// ������������� ������ �����
	);

	///---------------------------------------------------------------------------
	/// ������ ���������� ��������� ������
	void sorting_moves_history
	(
		struct List & list/// ������������� ������ �����
	);

	///---------------------------------------------------------------------------
	/// ������ ���������� ��������� ������
	void sorting_white_moves_root
	(
		struct List & list/// ������������� ������ �����
	);

	///---------------------------------------------------------------------------
	/// ������ ���������� ��������� ������
	void sorting_black_moves_root
	(
		struct List & list/// ������������� ������ �����
	);

};