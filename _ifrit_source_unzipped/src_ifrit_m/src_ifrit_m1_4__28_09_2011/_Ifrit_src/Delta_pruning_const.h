
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 17.04.2010 17.08.2011 28.09.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%

#include "Evaluate_const.h"

/// const 

namespace DELTA_PRUNING_CONSTANTS
{
	const unsigned __int16 QUEEN_MARGIN = (QUEEN + 2 * PAWN); /// отсчка взятий ладьей
	const unsigned __int16 ROOK_MARGIN = (ROOK + 2 * PAWN); /// отсчка взятий ладьей
	const unsigned __int16 MAX_BISHOP_KNIGHT_MARGIN = (BISHOP > KNIGHT) ? (BISHOP + (2 * PAWN)) : (KNIGHT + (2 * PAWN)) ; /// отсчка взятий  слонов и коней
	const unsigned __int16 PAWN_MARGIN = PAWN + (2 * PAWN); /// отсчка взятий  пешек

}

using namespace DELTA_PRUNING_CONSTANTS;