#include "a_Structure.h"

#define TEST_F 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������

#define DELTA_PRUNING 1 // ��������-���������(1-0) �����
// ����� ������������ �����������

//---------------------------------------------------------------------------
/*+
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 5.12.2008 19.11.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 60%

�������� � ���� ������ � ������ �������������� ��������:
������� ������ ������ 
���������� delta pruning
*/

namespace  QUIESCENCESEARCH
{
	//---------------------------------------------------------------------------
	// ������������ ������ ������ ��� �����
	__int32 public_forced_variant_white
	(
		struct T_Bitboard * const p_bitboard,// ������������� �����
		__int32 alpha,//
		__int32 beta,//
		unsigned __int8 depth,// �������
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const unsigned __int8 & depth_max// ������������ �������
	);

	//---------------------------------------------------------------------------
	// ������������ ������ ������ ��� ������
	__int32 public_forced_variant_black
	(
		struct T_Bitboard * const p_bitboard,// ������������� �����
		__int32 alpha,//
		__int32 beta,//
		unsigned __int8 depth,// �������
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const unsigned __int8 & depth_max// ������������ �������
	);

	//////////////////////////////////////////////////////////////////////////////////////////
	//private:

	//---------------------------------------------------------------------------
	// ��������� ������. ���������� ���������� ������
	inline unsigned __int8 sorting_captures_moves
	(
		struct T_list_surplus_moves * const list_surplus_moves// ������ �����
	);

	//---------------------------------------------------------------------------
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// �������� ������
	// ������� ��������, ������
#if TEST_F
	void list_Print_F
	(
		int depth,// ������� �������
		const struct T_list_surplus_moves * list_surplus_moves//������ �����
	);
#endif

}