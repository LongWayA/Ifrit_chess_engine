
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 18.08.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%

#include "Evaluate_const.h"

///------------------------------------------------------------------------------

/// NULL MOVE
namespace SEARCH_CONSTANTS_NULL_MOVE
{
	const unsigned __int8 NULL_YES = 1; /// мы внутри нулевого хода 
	const unsigned __int8 NULL_NO = 0; /// мы снаружи нулевого хода 
	const unsigned __int8 NULL_DEPTH = 2; /// дистанция до дна меньше которой NULL не вызывается
	const unsigned __int8 SWITCH_NULL_REDUCTION_1_2 = 5; /// глубина на которой меняется редукция нулевого хода 
										/// с NULL_REDUCTION_ONE на NULL_REDUCTION_TWO
	const unsigned __int8 NULL_REDUCTION = 3; /// 4 сокращение глубины перебора для NULL
	const unsigned __int8 NULL_REDUCTION_ONE = 4; /// 4 сокращение глубины перебора для динамического NULL
	const unsigned __int8 NULL_REDUCTION_TWO = 3; /// 3 сокращение глубины перебора для динамического NULL
	const unsigned __int8 VERIFICATOIN_SEARCH_REDUCTION = 5; // сокращение глубины перебора 
											///для поиска по обнаружению цугцванга
}

using namespace SEARCH_CONSTANTS_NULL_MOVE;

/// LMR
/// lmr_reduction = FULL_SEARCH 
/// + list_surplus_moves.end_captures; - на одну больше 
/// да плюс еще условие отсечки > да еще считаем с 0
/// значит хеш+ 2 киллера + тихий ход + взятия - 2 если взятия вообще есть
namespace SEARCH_CONSTANTS_LMR_MOVE
{
	const unsigned __int8 LMR_DEPTH = 3; /// дистанция до дна меньше которой LMR не вызывается
	const unsigned __int8 LMR_REDUCTION_D = 1; /// сокращение глубины перебора для LMR
	const unsigned __int8 FULL_SEARCH = 0;  /// было 2
											/// число веток с полным поиском без сокращений 
											/// ход из хеш-таблицы, лучшее взятие,
											/// два киллера, все взятия не редуцируются
											/// хеш-ход два киллера это 3 и минус один потому что с нуля 
											/// надо еще помнить что условие редукции i > lmr_reduction
}

using namespace SEARCH_CONSTANTS_LMR_MOVE;


/// FUTILITY
namespace SEARCH_CONSTANTS_FUTILITY_MOVE
{
	const __int32 FUTILITY_DEPTH = 5; //5
	//const __int32 FUTILITY_MARGIN[] = {PAWN, // 0 - не использую
	//								   PAWN, // 1
	//								   KNIGHT, // 2
	//								   (KNIGHT + PAWN), // 3
	//								   ROOK, // 4
	//								   (ROOK + PAWN), // 5- не использую
	//								   QUEEN // 6 - не использую
	//								  };

	//const __int32 FUTILITY_MARGIN[] = {PAWN, // 0 - не использую
	//								   PAWN, // 1
	//								   2*PAWN, // 2
	//								   3*PAWN, // 3
	//								   4*PAWN, // 4
	//								   5*PAWN, // 5- не использую
	//								   6*PAWN // 6 - не использую
	//								  };

	const __int32 FUTILITY_MARGIN[] = {PAWN, // 0 - не использую
									   2*PAWN, // 1
									   3*PAWN, // 2
									   4*PAWN, // 3
									   5*PAWN, // 4
									   6*PAWN, // 5- не использую
									   7*PAWN // 6 - не использую
									  };


	//const __int32 futility_move_count[] = { 5, // 0  было 3
	//									   10, // 1  было 3
	//									   15, // 2  было 4
	//									   25, // 3  было 5
	//									   30, // 4  было 7
	//									   35, // 5  было 11
	//									   40, // 6  было 19
	//									   50, // 7  было 35
	//									   67, // 8  было 67
	//									  131, // 9  было 131
	//									  259, // 10 было 259
	//									  515, // 11 было 515
	//									  1000 // 12 - не использую
	//									   }; 
	const __int32 futility_move_count[] = { 3, // 0  было 3
										    3, // 1  было 3
										    4, // 2  было 4
										    5, // 3  было 5
										    7, // 4  было 7
										   11, // 5  было 11
										   19, // 6  было 19
										   35, // 7  было 35
										   67, // 8  было 67
										  131, // 9  было 131
										  259, // 10 было 259
										  515, // 11 было 515
										  1000 // 12 - не использую
										   }; 

}

using namespace SEARCH_CONSTANTS_FUTILITY_MOVE;


/// RAZORING
namespace SEARCH_CONSTANTS_RAZORING_MOVE
{

	const __int32 RAZORING_MARGIN[] = {PAWN, // 0 - не использую
									   2*PAWN, // 1
									   3*PAWN, // 2
									   4*PAWN, // 3
									   5*PAWN, // 4
									   6*PAWN, // 5- не использую
									   7*PAWN // 6 - не использую
									  };

	//const __int32 RAZORING_MARGIN[] = {KNIGHT, // 0 - не использую
	//								   KNIGHT, // 1
	//						  (KNIGHT + PAWN), // 2
	//						   		     ROOK, // 3
	//								     ROOK, // 4 - не использую
	//								     ROOK, // 5 - не использую
	//								    QUEEN  // 6 - не использую
	//								  }; 
}

using namespace SEARCH_CONSTANTS_RAZORING_MOVE;



/// константы
namespace SEARCH_CONSTANTS_CONSTANTS
{

	const unsigned __int8 CHECK_YES = 1; /// есть шах
	const unsigned __int8 CHECK_NO = 0; /// нет шаха

	const unsigned __int8 MOVE_IS_SIMPLE = 1; /// ход это взятие
	const unsigned __int8 MOVE_IS_CAPTURE = 2; /// ход это взятие
	const unsigned __int8 CAPTURE_NO = 0; /// нет взятий

	const unsigned __int8 PV_YES = 1; /// основной вариант
	const unsigned __int8 PV_NO = 0; /// 

	/// 0 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка

	const  __int32 VALUE_MATE = 1000000; /// оценка мата
	const  __int16 DELTA_MATE = 1000; /// дельта для нахождения скорейшего мата
}

using namespace SEARCH_CONSTANTS_CONSTANTS;





