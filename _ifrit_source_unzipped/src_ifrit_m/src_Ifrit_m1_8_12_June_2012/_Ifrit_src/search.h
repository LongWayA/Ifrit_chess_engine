
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 2.11.2009 20.12.2009
/// ������� ����� ������� 50%

#include "a_position_struct.h"
#include "a_pv_line_struct.h"

/*

Search
http://chessprogramming.wikispaces.com/Search

 Because having a perfect evaluation is impossible, chess programs 
 must rely on some type of Search in order to play reasonably.

 Searching involves looking ahead at different move sequences and 
 evaluating the positions after making the moves. Formally, searching a 
 two-player zero-sum board game with perfect information implies traversing 
 and min-maxing a tree-like data-structure by various search algorithms.

Search Algorithms
 The majority of chess-programs use some variation of the alpha-beta 
 algorithm to search the tree in depth-first manner, with big up to 
 square-root savings for the same search result as the pure minimax algorithm. 
 Alpha-beta may be further enhanced by PVS similar to NegaScout and MTD(f). 
 While move ordering in pure minimax search don't cares - since all of branches 
 and nodes are searched anyway, it becomes crucial for the performance of alpha beta 
 and enhancements. Hans Berliner's chess-program HiTech and Ulf Lorenz's 
 program P.ConNerS used best-first approaches quite successfully.

Alpha-Beta Enhancements

 Obligatory
  Transposition Table
  Iterative Deepening

 Selectivity
  Quiescence Search
  Selectivity

 Scout and Friends
  Principal Variation Search

end link(����� ������)---------------------
*/

/*
 ������� �������
 �������� � ���� ������ � ������ ��������� ��������

 ��� �������� ��������� �������:
 1 � ������ ������������ ������ ���������� �����. ���������� ������ ��� ��� �������� �� ���
   � � ��� ���� ����� ���� ��� ��� ������ ��� ���
 2 ������ �� ��������� ���� ������ ������ ������ �� ���� � ��� �� ���-������� ���� �� ���� � ������
 3 ������ �� ��������� ��� � ��������� ������� �� ��� ���� 
   ��� ���� ��� ������ �� ��� ���������� ����� ���� ������
 4 ������������ ����������
 5 ����� ��������� � ������� ���������� ���
 6 ���� ����� �� ���� �� ��� ������ ���� ��� ����

 ����� ���� � ������������ �������
 int AlphaBeta (pos, depth, alpha, beta) 
 {
	if (depth == 0) return Evaluate(pos);
	best = -INFINITY;
	succ = Successors(pos);
	while (!Empty(succ)) 
	{
		pos = RemoveOne(succ);
		value = -AlphaBeta(pos, depth - 1, -beta, -alpha);
		if (value > best) best = value;
	    if (value > alpha) alpha = value;
        if (alpha >= beta) return alpha;
	}
	return best;
 }
*/


namespace Search
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

/*
	����� �� �����
*/
	__int32 white_searching_max_value
	(
		struct Position & position,/// ������������� �����
		__int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth,/// �������
		const unsigned __int8 depth_max,/// ������������ �������
		const bool flag_pv_node,/// �������� �� ��� ������� (pv)

		const __int32 old_description_move,/// ��� ���� ������� ������ � ������ �������
		bool flag_old_check,///���� ����
		const bool flag_null_m/// ���� �������� ����
	);

/*
	����� �� ������ 
*/
	__int32 black_searching_min_value
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		__int32 beta,///
		const unsigned __int8 depth,/// �������
		const unsigned __int8 depth_max,/// ������������ �������
		const bool flag_pv_node,/// �������� �� ��� ������� (pv)

		const __int32 old_description_move,/// ��� ���� ������� ������ � ������ �������
		bool flag_old_check,///���� ����
		const bool flag_null_m/// ���� �������� ����
	);

/////////////////////////////////////////////////////////////////////////////////////
//private:

/*
	����� ����������� ��������� ��������� ��� ������ �������� ����
	� ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
	���� ��� � ��� ���
	�� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	������� ���� ��� ����� ����� ���������
	�� �����
*/
	inline __int32 white_null_move
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,/// ������������ �������
		const unsigned __int8 depth,/// �������
		const __int32 old_description_move,/// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_node,/// �������� �� ��� ������� (pv)
		const bool flag_old_check,///���� ����
		const bool flag_null_m,/// ���� �������� ����
		const unsigned __int8 new_depth, /// ��������� �������
		const unsigned __int64 key_undo
	);

/*
	�� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	������� ���� ��� ����� ����� ���������
	�� ������
*/
	inline __int32 black_null_move
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth_max,/// ������������ �������
		const unsigned __int8 depth,/// �������
		const __int32 old_description_move,/// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_node,/// �������� �� ��� ������� (pv)
		const bool flag_old_check,///���� ����
		const bool flag_null_m,/// ���� �������� ����
		const unsigned __int8 new_depth, /// ��������� �������
		const unsigned __int64 key_undo
	);


/*
	���������� ������� �� ������������ �������. ��� �����
*/
	inline __int32 max_depth_white
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		const __int32 beta,///
		const bool flag_pv_node,/// �������� �� ��� ������� (pv)
		const unsigned __int8 depth/// �������
	);

/*
	���������� ������� �� ������������ �������. ��� ������
*/
	inline __int32 max_depth_black
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		const __int32 beta,///
		const bool flag_pv_node,/// �������� �� ��� ������� (pv)
		const unsigned __int8 depth/// �������
	);


/*

*/
	inline void quit_or_stop();

/*

*/
	void set_depth_max_rem
	(
		unsigned __int8 _depth_max_rem//
	);

/*

*/
	void ini_deep_time_terminate
	(
		double _deep_time_terminate//
	);


/*

*/ 
	void ini_global_loop();


/*
	���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
	�� ��������� �� �������� �� ������
*/
	bool get_stop_search();

/*
	���������� ���������� ������������� �����
*/
	__int64  get_nodes();

/*
	�������� ���������� ������������� �����
*/
	void set_nodes_in_0();


};
