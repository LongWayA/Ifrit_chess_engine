
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 17.04.2010 17.08.2011
/// ������� ����� ������� 90%

#include "Evaluate_const.h"

/// const 

namespace DELTA_PRUNING_CONSTANTS
{
	const unsigned __int16 ROOK_MARGIN = ROOK; /// ������ ������ ������
	const unsigned __int16 MAX_BISHOP_KNIGHT_MARGIN = (BISHOP > KNIGHT) ? BISHOP : KNIGHT ; /// ������ ������  ������ � �����
	const unsigned __int8 PAWN_MARGIN = 160; /// ������ ������  �����

}

using namespace DELTA_PRUNING_CONSTANTS;