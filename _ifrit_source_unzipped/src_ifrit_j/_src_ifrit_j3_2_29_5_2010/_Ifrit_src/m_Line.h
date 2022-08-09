#include "a_Structure.h"

//-----------------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 27.02.2009
*/
//-----------------------------------------------------------------------------------


/*
 ������� ����� ������� 70%

 ������ � ������ �������� ��������

 � ����� ������ �����.
 � ��� ���� ��� ���������� ����� �������� - ���� ����������� ��� ���������� �� �������, 
 � ������ ��� ������� �  �������. ����� ���� � ������ ���� ���� ����� ������� �������� 
 ��� ����.
 �� ���������� ����, ������� ������, ���������� �� ������� � ����� �������� ��������. 
 �� ������� �� �������������� �� � ����� ������� ��������. ���������� �� � ������ ���� 
 ���������� ������ ��� ������� ���� �������. ����� �� ����, �� ������ � ���� ������� 
 �������������� � ����� ������� ��������.

 ���������:

 � ��� ���� ����� �������� ����� ��� ����� ������ pv_current. 
 �� �� ���������, ����� ���������� �� ������� public_update_PV_current.  

 ���� ��� ������ ����� �������� pv_best. 
 �� �� ��������� ����� ��� ��� ����� �� ���� public_update_PV_best.
 ������ ������ �� ���������� �� ������������ ������� public_update_PV_best_max_depth.

 ��� �� � ������ ���� ���� ������ ����� �������� ��� ������� ���� p_PV_best_point.
 �� �� ��������� ������ ��� ����� � ���� ����� ������ ��� public_update_PV_best_point.

*/

namespace PVSAVE
{

	//---------------------------------------------------------------------------
	// ���������� ������ �������
	struct T_PV * public_get_pv_best();

	//---------------------------------------------------------------------------
	// ���������� ������� �������
	struct T_PV * public_get_pv_current();

	//---------------------------------------------------------------------------
	// ��������� ����� ������� �������� � ����
	void public_update_PV_best_point
	(
		struct T_PV * const p_PV_best_point// ������ ������� ��������� � ����
	);

	//---------------------------------------------------------------------------
	// ��������� ����� ������� ��������
	void public_update_PV_best
	(
		const struct T_PV * const p_PV_best_point// ������ ������� ��������� � ����
	);

	//---------------------------------------------------------------------------
	// ��������� ����� ������� �������� �� ������������ �������
	void public_update_PV_best_max_depth
	(
		const __int32 & score,// ������ ������� � ����� ��������
		const unsigned __int8 & depth// ������� �������
	);

	//---------------------------------------------------------------------------
	// ���������� ��� � ������� �������
	void public_update_PV_current
	(
		const unsigned __int8 & i,// ����� ���� � ������ �����
		const unsigned __int8 & depth,// ������� �������
		const struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
	);

	//---------------------------------------------------------------------------
	// ������� ��� ������� ����������
	void public_print_currmove
	(
		const struct T_list_surplus_moves * const p_list_surplus_moves,//������ �����
		const unsigned __int8 nodes_0,// ���������� ����� �� ������� �������
		const unsigned __int8 i,// ����� ���� �� ������
		const unsigned __int8 Depth_Max// ������������ �������
	);

	//---------------------------------------------------------------------------
	// ������� ������� ������� � ��������
	void public_print_pv_line
	(
		const bool colour_move,// ���� ���������� �������
		const struct T_list_surplus_moves * const p_list_surplus_moves,// ������ �����
		const struct T_PV * const p_PV_best_point,// ������ �������
		const __int64 print_nodes,// ���������� �����
		const unsigned __int8 i,// ����� ���� � ������
		const unsigned __int8 Depth_Max,// ������������ �������
		const __int32 value// ������ �������
		//,const double time_nodes// ����� ������� ��������
	);

}
