
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 18.08.2011
/// ������� ����� ������� 90%

#include "b_evaluate_const.h"

///------------------------------------------------------------------------------

/// NULL MOVE
namespace SEARCH_CONSTANTS_NULL_MOVE
{
	const unsigned __int8 NULL_YES = 1; // �� ������ �������� ���� 
	const unsigned __int8 NULL_NO = 0; // �� ������� �������� ���� 
	const unsigned __int8 NULL_REDUCTION = 3; // 4 ���������� ������� �������� ��� NULL
	const unsigned __int8 VERIFICATOIN_SEARCH_REDUCTION = 5; // ���������� ������� �������� 
											//��� ������ �� ����������� ���������

	const unsigned __int8 SWITCH_NULL_REDUCTION_1_2 = 5; // ������� �� ������� �������� �������� �������� ���� 
										// � NULL_REDUCTION_ONE �� NULL_REDUCTION_TWO
	const unsigned __int8 NULL_REDUCTION_ONE = 4; // 4 ���������� ������� �������� ��� ������������� NULL
	const unsigned __int8 NULL_REDUCTION_TWO = 3; // 3 ���������� ������� �������� ��� ������������� NULL
}

using namespace SEARCH_CONSTANTS_NULL_MOVE;

// LMR
namespace SEARCH_CONSTANTS_LMR_MOVE
{
	const unsigned __int8 LMR_DEPTH = 3; // ��������� �� ��� ������ ������� LMR �� ����������
	const unsigned __int8 LMR_REDUCTION_D = 1; // ���������� ������� �������� ��� LMR
}

using namespace SEARCH_CONSTANTS_LMR_MOVE;


/// FUTILITY
namespace SEARCH_CONSTANTS_FUTILITY_MOVE
{
	const __int32 FUTILITY_DEPTH = 4; //5
	//const __int32 FUTILITY_MARGIN[] = {PAWN, // 0 - �� ���������
	//								   PAWN, // 1
	//								   KNIGHT, // 2
	//								   (KNIGHT + PAWN), // 3
	//								   ROOK, // 4
	//								   (ROOK + PAWN), // 5- �� ���������
	//								   QUEEN // 6 - �� ���������
	//								  };

	//const __int32 FUTILITY_MARGIN[] = {PAWN, // 0 - �� ���������
	//								   PAWN, // 1
	//								   2*PAWN, // 2
	//								   3*PAWN, // 3
	//								   4*PAWN, // 4
	//								   5*PAWN, // 5- �� ���������
	//								   6*PAWN // 6 - �� ���������
	//								  };

	const __int32 FUTILITY_MARGIN[] = {2*PAWN, // 0 
									   2*PAWN, // 1
									   3*PAWN, // 2
									   4*PAWN, // 3
									   5*PAWN, // 4
									   6*PAWN, // 5- �� ���������
									   7*PAWN // 6 - �� ���������
									  };


	//const __int32 futility_move_count[] = { 5, // 0  ���� 3
	//									   10, // 1  ���� 3
	//									   15, // 2  ���� 4
	//									   25, // 3  ���� 5
	//									   30, // 4  ���� 7
	//									   35, // 5  ���� 11
	//									   40, // 6  ���� 19
	//									   50, // 7  ���� 35
	//									   67, // 8  ���� 67
	//									  131, // 9  ���� 131
	//									  259, // 10 ���� 259
	//									  515, // 11 ���� 515
	//									  1000 // 12 - �� ���������
	//									   }; 
	const __int32 futility_move_count[] = { 3, // 0  ���� 3
										    3, // 1  ���� 3
										    4, // 2  ���� 4
										    5, // 3  ���� 5
										    7, // 4  ���� 7
										   11, // 5  ���� 11
										   19, // 6  ���� 19
										   35, // 7  ���� 35
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

	const __int32 RAZORING_MARGIN[] = {2*PAWN, // 0
									   2*PAWN, // 1
									   3*PAWN, // 2
									   4*PAWN, // 3
									   5*PAWN, // 4
									   6*PAWN, // 5- �� ���������
									   7*PAWN // 6 - �� ���������
									  };

	//const __int32 RAZORING_MARGIN[] = {KNIGHT, // 0 - �� ���������
	//								   KNIGHT, // 1
	//						  (KNIGHT + PAWN), // 2
	//						   		     ROOK, // 3
	//								     ROOK, // 4 - �� ���������
	//								     ROOK, // 5 - �� ���������
	//								    QUEEN  // 6 - �� ���������
	//								  }; 
}

using namespace SEARCH_CONSTANTS_RAZORING_MOVE;



/// ���������
namespace SEARCH_CONSTANTS_CONSTANTS
{

	const unsigned __int8 CHECK_YES = 1; // ���� ���
	const unsigned __int8 CHECK_NO = 0; // ��� ����

	const unsigned __int8 MOVE_IS_SIMPLE = 1; // ��� ��� �� ������
	const unsigned __int8 MOVE_IS_CAPTURE = 2; // ��� ��� ������
	const unsigned __int8 CAPTURE_NO = 0; // ��� ������

	const unsigned __int8 PV_YES = 1; // �������� �������
	const unsigned __int8 PV_NO = 0; // 

	/// 0 - ������ 1200 - ����� 600 - ����� 400 - ����  400 - ���� 100 - �����

	const  __int32 VALUE_MATE = 1000000; // ������ ����
	const  __int16 DELTA_MATE = 1000; // ������ ��� ���������� ���������� ����
	const  __int32 INFINITE_SCORE = 9999999; //
	const  __int32 DRAWN_SCORE = 0; //

}

using namespace SEARCH_CONSTANTS_CONSTANTS;





