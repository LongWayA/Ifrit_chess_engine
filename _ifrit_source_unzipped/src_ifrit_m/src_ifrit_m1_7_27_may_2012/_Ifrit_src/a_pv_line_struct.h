
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 21.09.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 99%

#ifndef PV_LINE_H
#define PV_LINE_H

//!
#include "b_constants_header.h"

struct PV_line
{
	/// описание хода смотреть в классе List
	__int32 move[MAX_DEPTH]; 

	/// оценка варианта
	__int32 score_move;

	/// длина варианта
	unsigned __int8  depth_max;

};


#endif // !PV_LINE_H






