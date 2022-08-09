
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 17.04.2010 17.08.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%



/// const 

namespace DELTA_PRUNING_CONSTANTS
{

	/// 0 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка
	const unsigned __int16 ROOK_MARGIN = 700; /// отсчка взятий ладьей
	const unsigned __int16 BISHOP_KNIGHT_MARGIN = 500; /// отсчка взятий  слонов и коней
	const unsigned __int8 PAWN_MARGIN = 160; /// отсчка взятий  пешек

}

using namespace DELTA_PRUNING_CONSTANTS;