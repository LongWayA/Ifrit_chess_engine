
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 18.08.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%


///------------------------------------------------------------------------------

/// NULL MOVE
namespace D_SEARCH_CONSTANTS_NULL_MOVE
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

using namespace D_SEARCH_CONSTANTS_NULL_MOVE;

/// LMR
namespace D_SEARCH_CONSTANTS_LMR_MOVE
{
	const unsigned __int8 LMR_DEPTH = 3; /// дистанция до дна меньше которой LMR не вызывается
	const unsigned __int8 LMR_REDUCTION_D = 1; /// сокращение глубины перебора для LMR
	const unsigned __int8 FULL_SEARCH = 2;  /// было 2
											/// число веток с полным поиском без сокращений 
											/// ход из хеш-таблицы, лучшее взятие,
											/// два киллера, все взятия не редуцируются
											/// хеш-ход два киллера это 3 и минус один потому что с нуля 
											/// надо еще помнить что условие редукции i > lmr_reduction
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

/// константы
namespace D_SEARCH_CONSTANTS_CONSTANTS
{

	const unsigned __int8 CHECK_YES = 1; /// есть шах
	const unsigned __int8 CHECK_NO = 0; /// нет шаха


	const unsigned __int8 MOVE_IS_CAPTURE = 2; /// ход это взятие
	const unsigned __int8 CAPTURE_NO = 0; /// нет взятий

	const unsigned __int8 PV_YES = 1; /// основной вариант
	const unsigned __int8 PV_NO = 0; /// 

	/// 0 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка

	const  __int32 VALUE_MATE = 1000000; /// оценка мата
	const  __int16 DELTA_MATE = 1000; /// дельта для нахождения скорейшего мата
}

using namespace D_SEARCH_CONSTANTS_CONSTANTS;





