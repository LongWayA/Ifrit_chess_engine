
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 18.08.2011
/// ������� ����� ������� 90%


///------------------------------------------------------------------------------

/// NULL MOVE
namespace D_SEARCH_CONSTANTS_NULL_MOVE
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

using namespace D_SEARCH_CONSTANTS_NULL_MOVE;

/// LMR
namespace D_SEARCH_CONSTANTS_LMR_MOVE
{
	const unsigned __int8 LMR_DEPTH = 3; /// ��������� �� ��� ������ ������� LMR �� ����������
	const unsigned __int8 LMR_REDUCTION_D = 1; /// ���������� ������� �������� ��� LMR
	const unsigned __int8 FULL_SEARCH = 2;  /// ���� 2
											/// ����� ����� � ������ ������� ��� ���������� 
											/// ��� �� ���-�������, ������ ������,
											/// ��� �������, ��� ������ �� ������������
											/// ���-��� ��� ������� ��� 3 � ����� ���� ������ ��� � ���� 
											/// ���� ��� ������� ��� ������� �������� i > lmr_reduction
}

using namespace D_SEARCH_CONSTANTS_LMR_MOVE;


/// FUTILITY
namespace D_SEARCH_CONSTANTS_FUTILITY_MOVE
{
	const __int32 FUTILITY_DEPTH = 4; //5
	const __int32 FUTILITY_MARGIN[] = {150, 150, 200, 300, 400, 1000};

	/// RAZORING
	const __int32 RAZORING_MARGIN[] = {150, 150, 200, 300, 400, 1000};
}

using namespace D_SEARCH_CONSTANTS_FUTILITY_MOVE;

/// ���������
namespace D_SEARCH_CONSTANTS_CONSTANTS
{

	const unsigned __int8 CHECK_YES = 1; /// ���� ���
	const unsigned __int8 CHECK_NO = 0; /// ��� ����


	const unsigned __int8 MOVE_IS_CAPTURE = 2; /// ��� ��� ������
	const unsigned __int8 CAPTURE_NO = 0; /// ��� ������

	const unsigned __int8 PV_YES = 1; /// �������� �������
	const unsigned __int8 PV_NO = 0; /// 

	/// 0 - ������ 1200 - ����� 600 - ����� 400 - ����  400 - ���� 100 - �����

	const  __int32 VALUE_MATE = 1000000; /// ������ ����
	const  __int16 DELTA_MATE = 1000; /// ������ ��� ���������� ���������� ����
}

using namespace D_SEARCH_CONSTANTS_CONSTANTS;





