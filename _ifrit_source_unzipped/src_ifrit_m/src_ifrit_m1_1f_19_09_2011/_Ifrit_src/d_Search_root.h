
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 2.11.2009 20.12.2009
/// ������� ����� ������� 50%


///!
#include "a_List.h"


///
#define TEST_LIST 0

/// ������� �������
/// �������� � ���� ������ � ������  �������� � ����� �.�. �� ������� �������

 class d_Search_root
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
private:

	__int64 nodes_root;/// ���������� �����

	///TEST
	/// ��������� ������ ��� �������� ���������� ���� � ��� ��� �������� � ����� ����
	unsigned __int8 from;/// ������ ����� ������
	unsigned __int8 to;/// ���� ����� ������


//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	/// ����� �� ������� ������ �� �����
	__int32 white_root
	(
		struct Position & position,/// ������������� �����
		__int32 alpha,///
		const __int32 beta,/// 
		const unsigned __int8 depth_max/// ������������ �������
	);

	///---------------------------------------------------------------------------
	/// ����� �� ������� ������ �� ������
	__int32 black_root
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		__int32 beta,/// 
		const unsigned __int8 depth_max/// ������������ �������
	);

	///---------------------------------------------------------------------------
	/// ���������� ������ �������
	struct PV_line & get_pv_best();

	///---------------------------------------------------------------------------
	/// ���������� ���������� ������������� �����
	__int64  get_nodes();

	///---------------------------------------------------------------------------
	///�������� ���������� ������������� �����
	void set_nodes_in_0();

	///---------------------------------------------------------------------------
	/// ���������� ������ �������� ���
	unsigned __int8 get_from();

	///---------------------------------------------------------------------------
	/// ���������� ���� �������� ���
	unsigned __int8 get_to();

//////////////////////////////////////////////////////////////////////////////////////////
public:

#if TEST_LIST

/// �������� ������
/// ������� ��������, ������
	void list_print
	(
		int Depth,// ������� �������
		struct List & list_surplus_moves// ������ ��������� �����
	);

#endif

};