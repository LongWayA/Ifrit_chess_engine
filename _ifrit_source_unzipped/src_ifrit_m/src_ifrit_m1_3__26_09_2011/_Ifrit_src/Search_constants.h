
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 18.08.2011
/// ������� ����� ������� 90%

#include "Evaluate_const.h"

///------------------------------------------------------------------------------

/// NULL MOVE
namespace SEARCH_CONSTANTS_NULL_MOVE
{
	const unsigned __int8 NULL_YES = 1; /// �� ������ �������� ���� 
	const unsigned __int8 NULL_NO = 0; /// �� ������� �������� ���� 
	const unsigned __int8 NULL_DEPTH = 2; /// ��������� �� ��� ������ ������� NULL �� ����������
	const unsigned __int8 SWITCH_NULL_REDUCTION_1_2 = 5; /// ������� �� ������� �������� �������� �������� ���� 
										/// � NULL_REDUCTION_ONE �� NULL_REDUCTION_TWO
	const unsigned __int8 NULL_REDUCTION = 3; /// 4 ���������� ������� �������� ��� NULL
	const unsigned __int8 NULL_REDUCTION_ONE = 4; /// 4 ���������� ������� �������� ��� ������������� NULL
	const unsigned __int8 NULL_REDUCTION_TWO = 3; /// 3 ���������� ������� �������� ��� ������������� NULL
	const unsigned __int8 VERIFICATOIN_SEARCH_REDUCTION = 5; // ���������� ������� �������� 
											///��� ������ �� ����������� ���������
}

using namespace SEARCH_CONSTANTS_NULL_MOVE;

/// LMR
/// lmr_reduction = FULL_SEARCH 
/// + list_surplus_moves.end_captures; - �� ���� ������ 
/// �� ���� ��� ������� ������� > �� ��� ������� � 0
/// ������ ���+ 2 ������� + ����� ��� + ������ - 2 ���� ������ ������ ����
namespace SEARCH_CONSTANTS_LMR_MOVE
{
	const unsigned __int8 LMR_DEPTH = 3; /// ��������� �� ��� ������ ������� LMR �� ����������
	const unsigned __int8 LMR_REDUCTION_D = 1; /// ���������� ������� �������� ��� LMR
	const unsigned __int8 FULL_SEARCH = 0;  /// ���� 2
											/// ����� ����� � ������ ������� ��� ���������� 
											/// ��� �� ���-�������, ������ ������,
											/// ��� �������, ��� ������ �� ������������
											/// ���-��� ��� ������� ��� 3 � ����� ���� ������ ��� � ���� 
											/// ���� ��� ������� ��� ������� �������� i > lmr_reduction
}

using namespace SEARCH_CONSTANTS_LMR_MOVE;


/// FUTILITY
namespace SEARCH_CONSTANTS_FUTILITY_MOVE
{
	const __int32 FUTILITY_DEPTH = 4; //5
	const __int32 FUTILITY_MARGIN[] = {2*PAWN, // 0 - �� ���������
									   2*PAWN, // 1
									   KNIGHT, // 2
									   KNIGHT, // 3
									   KNIGHT, // 4
									   1000 // 5 - �� ���������
									  };


	const __int32 futility_move_count[] = { 5, // 0  ���� 3
										   10, // 1  ���� 3
										   15, // 2  ���� 4
										   25, // 3  ���� 5
										   30, // 4  ���� 7
										   35, // 5  ���� 11
										   40, // 6  ���� 19
										   50, // 7  ���� 35
										   67, // 8  ���� 67
										  131, // 9  ���� 131
										  259, // 10 ���� 259
										  515, // 11 ���� 515
										  1000 // 12 - �� ���������
										   }; 

}

using namespace SEARCH_CONSTANTS_FUTILITY_MOVE;


/// RAZORING
namespace SEARCH_CONSTANTS_RAZORING_MOVE
{
	const __int32 RAZORING_MARGIN[] = {2*PAWN, // 0 - �� ���������
									   2*PAWN, // 1
									   KNIGHT, // 2
									   KNIGHT, // 3
									   KNIGHT, // 4 - �� ���������
									   QUEEN  // 5 - �� ���������
									  }; 

}

using namespace SEARCH_CONSTANTS_RAZORING_MOVE;



/// ���������
namespace SEARCH_CONSTANTS_CONSTANTS
{

	const unsigned __int8 CHECK_YES = 1; /// ���� ���
	const unsigned __int8 CHECK_NO = 0; /// ��� ����

	const unsigned __int8 MOVE_IS_SIMPLE = 1; /// ��� ��� ������
	const unsigned __int8 MOVE_IS_CAPTURE = 2; /// ��� ��� ������
	const unsigned __int8 CAPTURE_NO = 0; /// ��� ������

	const unsigned __int8 PV_YES = 1; /// �������� �������
	const unsigned __int8 PV_NO = 0; /// 

	/// 0 - ������ 1200 - ����� 600 - ����� 400 - ����  400 - ���� 100 - �����

	const  __int32 VALUE_MATE = 1000000; /// ������ ����
	const  __int16 DELTA_MATE = 1000; /// ������ ��� ���������� ���������� ����
}

using namespace SEARCH_CONSTANTS_CONSTANTS;





