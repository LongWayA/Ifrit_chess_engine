
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ���������  8.09.2011
/// ������� ����� ������� 0%



///!
#include "a_Header_constants.h"
#include "a_List.h"


/// ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_HISTORY 0 



/// ��������� �������


/// ���� ���  ��� �� ������� ����� � ������ ������� �� ����
/// ������ ��� ������� � ��� ��������� �������� 
/// ������� ��� ��� ������ �� �����
/// ��� ������ ������� ���� � ������ ������� �� �����



/// 
const __int32 History_Maximum = 100000;

class k_History
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
private:

	///
	__int32 white_history[MAX_NAME][MAX_COORDINATE];  // [������][���� ���� ������ �����]
	__int32 black_history[MAX_NAME][MAX_COORDINATE];

//////////////////////////////////////////////////////////////////////////////////////
public:

	///
	inline __int32 get_white_history
	(
		const unsigned __int8 n,///
		const unsigned __int8 c ///
	)
	{
		return white_history[n][c];
	};

	///
	inline __int32 get_black_history
	(
		const unsigned __int8 n,///
		const unsigned __int8 c ///
	)
	{
		return black_history[n][c];
	};

//////////////////////////////////////////////////////////////////////////////////////
public:

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

	void k_History::test_print
	(	
		const unsigned __int8 depth_max// ������������ ������� �������
	);

#endif

};
