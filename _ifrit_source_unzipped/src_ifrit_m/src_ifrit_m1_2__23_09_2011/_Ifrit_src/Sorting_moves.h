
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010
/// ������� ����� ������� 70%


///!
#include "Header_constants.h"
#include "List.h"

/// ��������-���������(1-0) �������� ������ �����
#define KONTROL_B 0 

/// 
#define TEST_H 0 





/// ���������� ������ �����
/// ��������� �� ��������������� ������ �����

namespace Sorting_moves 
{


//////////////////////////////////////////////////////////////////////////////////////
//public:

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