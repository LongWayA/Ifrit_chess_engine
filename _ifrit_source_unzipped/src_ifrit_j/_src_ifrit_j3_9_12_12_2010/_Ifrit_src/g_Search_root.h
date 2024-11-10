#include "a_Structure.h"
#include "a_Chess_bitboard.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 2.11.2009 20.12.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

 ������� �������

 �������� � ���� ������ � ������  �������� � ����� �.�. �� ������� �������

*/


 class Search_root
{

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////////
private:

	//-------------------------------------------------------------------------
	__int64 nodes_root;// ���������� �����
	// ��������� ������ ��� �������� ���������� ���� � ��� ��� �������� � ����� ����
	unsigned __int8 from;// ������ ����� ������
	unsigned __int8 to;// ���� ����� ������


//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// ����� �� ������� ������ �� �����
	__int32 white_root
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		class List_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
		__int32 alpha,//
		const __int32 beta,// 
		const unsigned __int8 depth_max// ������������ �������
	);

	//---------------------------------------------------------------------------
	// ����� �� ������� ������ �� ������
	__int32 black_root
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		class List_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
		const __int32 alpha,//
		__int32 beta,// 
		const unsigned __int8 depth_max// ������������ �������
	);

	//---------------------------------------------------------------------------
	// 
	void ini_global_loop
	(
		unsigned __int8 depth_max//
	);

	//---------------------------------------------------------------------------
	// 
	void ini_deep_time_terminate
	(
		double deep_time_terminate_v//
	);

	//---------------------------------------------------------------------------
	// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
	// �� ��������� �� �������� �� ������
	bool get_stop_search();

	//---------------------------------------------------------------------------
	// ���������� ������ �������
	struct PV_line * get_pv_best();

	//---------------------------------------------------------------------------
	// ���������� ���������� ������������� �����
	__int64  get_nodes();

	//---------------------------------------------------------------------------
	//�������� ���������� ������������� �����
	void set_nodes_in_0();

	//---------------------------------------------------------------------------
	// ���������� ������ �������� ���
	unsigned __int8 get_from();

	//---------------------------------------------------------------------------
	// ���������� ���� �������� ���
	unsigned __int8 get_to();



//////////////////////////////////////////////////////////////////////////////////////////
private:

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������, ������
#if TEST_L
	void List_Print
	(
		int Depth,// ������� �������
		class List_surplus_moves * p_list_surplus_moves// ������ �����
	);
#endif
//---------------------------------------------------------------------------

};