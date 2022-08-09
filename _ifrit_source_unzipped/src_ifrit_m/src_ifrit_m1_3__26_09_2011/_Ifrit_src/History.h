
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ���������  8.09.2011
/// ������� ����� ������� 0%



///!
#include "Header_constants.h"
#include "List.h"


/// ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_HISTORY 0 



/// ��������� �������


/// ���� ���  ��� �� ������� ����� � ������ ������� �� ����
/// ������ ��� ������� � ��� ��������� �������� 
/// ������� ��� ��� ������ �� �����
/// ��� ������ ������� ���� � ������ ������� �� �����

namespace History
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///
	__int32 get_white_history
	(
		const unsigned __int8 n,///
		const unsigned __int8 c ///
	);

	///
	__int32 get_black_history
	(
		const unsigned __int8 n,///
		const unsigned __int8 c ///
	);

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// �������������� ������ �������
	void ini_history();

	///---------------------------------------------------------------------------
	/// ����� �������
	void white_history_good_save
	(
		const unsigned __int8 i,/// ����� ���� � ������
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 depth,// ������� ������ ����
		const unsigned __int8 depth_max// ������������ ������� �������
	);

	///---------------------------------------------------------------------------
	/// ����� �������
	void black_history_good_save
	(
		const unsigned __int8 i,/// ����� ���� � ������
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 depth,// ������� ������ ����
		const unsigned __int8 depth_max// ������������ ������� �������
	);

	///---------------------------------------------------------------------------
	/// ����� �������
	inline void white_history_bad_save
	(
		const unsigned __int8 e,/// ����� ���� � ������
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 depth,// ������� ������ ����
		const unsigned __int8 depth_max// ������������ ������� �������
	);

	///---------------------------------------------------------------------------
	/// ����� �������
	inline void black_history_bad_save
	(
		const unsigned __int8 e,/// ����� ���� � ������
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 depth,// ������� ������ ����
		const unsigned __int8 depth_max// ������������ ������� �������
	);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// ��������
// ������������ �������
#if TEST_HISTORY

	void History::test_print
	(	
		const unsigned __int8 depth_max// ������������ ������� �������
	);

#endif

};
