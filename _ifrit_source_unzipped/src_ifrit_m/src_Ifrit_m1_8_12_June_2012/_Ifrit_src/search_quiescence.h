
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 5.12.2008 19.11.2009
/// ������� ����� ������� 80%

#include "a_position_struct.h"

/// ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_F 0;

///!
/// �������� �������������� ������� �������� �� �� ������������� ������� 
/// ��� ������������� #if #endif
/// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 

/*

Quiescence search
http://chessprogramming.wikispaces.com/Quiescence+Search

Virtually all chess programs, at the end of the main search perform 
a more limited quiescence search, containing fewer moves. 
The purpose of this search is to only evaluate "quiet" positions, 
or positions where there are no winning tactical moves to be made. 
This search is needed to avoid the horizon effect. Simply stopping 
your search when you reach the desired depth and then evaluate, 
is very dangerous. Consider the situation where the last move 
you consider is QxP. If you stop there and evaluate, you might 
think that you have won a pawn. But what if you were to search one 
move deeper and find that the next move is PxQ? You didn't win a pawn, 
you actually lost a queen. Hence the need to make sure that you are 
evaluating only quiescent (quiet) positions.

Pseudo Code
int Quiesce( int alpha, int beta ) {
    int stand_pat = Evaluate();
    if( stand_pat >= beta )
        return beta;
    if( alpha < stand_pat )
        alpha = stand_pat;
 
    until( every_capture_has_been_examined )  {
        MakeCapture();
        score = -Quiesce( -beta, -alpha );
        TakeBackMove();
 
        if( score >= beta )
            return beta;
        if( score > alpha )
           alpha = score;
    }
    return alpha;
}

end link(����� ������)---------------------
*/

/*
 ������������� ������� ������� 
 �������� � ���� ������ � ������ �������������� ��������:
 ������� ������ ������ 
 ���������� delta pruning
*/

namespace Search_quiescence
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// ������������ ������ ������ ��� �����
	__int32 forced_variant_white
	(
		struct Position & position,/// ������������� �����
		__int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth,/// �������
		const bool flag_pv_f/// �������� �� ��� ������� (pv)
	);

	///---------------------------------------------------------------------------
	/// ������������ ������ ������ ��� ������
	__int32 forced_variant_black
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		__int32 beta,///
		const unsigned __int8 depth,/// �������
		const bool flag_pv_f/// �������� �� ��� ������� (pv)
	);

	///---------------------------------------------------------------------------
	///TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	/// �������� ������
	/// ������� ��������, ������
#if TEST_F

	void list_Print_F
	(
		int depth,/// ������� �������
		const namespace List_surplus_moves & list_surplus_moves///������ �����
	);
#endif

};


namespace DELTA_PRUNING_CONSTANTS
{
	const unsigned __int16 QUEEN_MARGIN = (QUEEN + 2 * PAWN); /// ������ ������ ������
	const unsigned __int16 ROOK_MARGIN = (ROOK + 2 * PAWN); /// ������ ������ ������
	const unsigned __int16 MAX_BISHOP_KNIGHT_MARGIN = (BISHOP > KNIGHT) ? (BISHOP + (2 * PAWN)) : (KNIGHT + (2 * PAWN)) ; /// ������ ������  ������ � �����
	const unsigned __int16 PAWN_MARGIN = PAWN + (2 * PAWN); /// ������ ������  �����

}

using namespace DELTA_PRUNING_CONSTANTS;