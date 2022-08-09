
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 17.04.2010 17.08.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%

#include "Evaluate_const.h"

/// const 

namespace DELTA_PRUNING_CONSTANTS
{
	const unsigned __int16 ROOK_MARGIN = ROOK; /// отсчка взятий ладьей
	const unsigned __int16 MAX_BISHOP_KNIGHT_MARGIN = (BISHOP > KNIGHT) ? BISHOP : KNIGHT ; /// отсчка взятий  слонов и коней
	const unsigned __int8 PAWN_MARGIN = 160; /// отсчка взятий  пешек

}

using namespace DELTA_PRUNING_CONSTANTS;