
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 5.12.2008 19.11.2009
/// ������� ����� ������� 80%

#include "Position.h"

/// ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_F 0;

/// ��������-���������(1-0) �����
#define DELTA_PRUNING 1 

///!
/// �������� �������������� ������� �������� �� �� ������������� ������� 
/// ��� ������������� #if #endif
/// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 




/// ������������� ������� ������� 
/// �������� � ���� ������ � ������ �������������� ��������:
/// ������� ������ ������ 
/// ���������� delta pruning

namespace Search_quiescence
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// ������������ ������ ������ ��� �����
	__int32 forced_variant_white
	(
		struct Position & position,/// ������������� �����
		__int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth,/// �������
		const bool flag_pv_f/// �������� �� ��� ������� (pv)
	);

	///---------------------------------------------------------------------------
	/// ������������ ������ ������ ��� ������
	__int32 forced_variant_black
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		__int32 beta,///
		const unsigned __int8 depth,/// �������
		const bool flag_pv_f/// �������� �� ��� ������� (pv)
	);

	///---------------------------------------------------------------------------
	///TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	/// �������� ������
	/// ������� ��������, ������
#if TEST_F

	void list_Print_F
	(
		int depth,/// ������� �������
		const namespace List_surplus_moves & list_surplus_moves///������ �����
	);
#endif

};
