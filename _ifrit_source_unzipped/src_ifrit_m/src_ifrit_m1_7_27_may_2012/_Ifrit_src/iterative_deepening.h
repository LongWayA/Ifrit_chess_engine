
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 22.02.2009 25.12.2010 23.5.2012
/// ������� ����� ������� 50%

#include "a_position_struct.h"

/*
Iterative deepening (ID)
http://chessprogramming.wikispaces.com/Iterative+Deepening

Iterative deepening (ID) has been invented as the basic time 
management strategy in depth-first searches, but proved 
surprisingly beneficial as far as the move ordering is concerned 
in alpha-beta and its enhancements. 
It has been noticed, that even if one is about to search to a 
given depth, the iterative deepening is faster than searching 
for the given depth immediately. It happens due to dynamic move 
ordering techniques, such as using PV-, hash- and refutation 
moves determined in previous iteration(s), as well the history 
heuristic.

How it Works
It works as follows: program starts with a one ply search, 
then increments the search depth and does another search. 
This process is repeated until the time allocated for the 
search is exhausted. In case of an unfinished search, program 
has always an option to fall back to the move from the less 
deep search. Yet if we make sure that this move is searched 
first in the next iteration, then overwriting the new move with 
the old one becomes unnecessary. This way, also the results from 
the partial search can be accepted - though in case of a severe 
drop of the score it is wise to allocate some more time, as the 
first alternative is often a bad capture, delaying the loss instead 
of preventing it.
Iterative deepening, using a TT, embed the depth-first algorithms 
like alpha-beta into a framework with best-first characteristics.

end link(����� ������)---------------------
*/

namespace Iterative_deepening
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

//////////////////////////////////////////////////////////////////////////////////////
//private:

/*
	������� � ����������� ����������� �������

	���������� ���������� ���������� ������� �� ��� ��� ���� �� ��������� �������.
	�.�. � ������ �� ������� 1 ����� 2 � �.�.
	��� ����� �� ������ ��� ���������� ���-�������
	�� ������ ����� ����������� ����� ��������. �������� ���� �� �� ���������� �������
	������� ������� �� �� ��������� � ������� � �������� ��� ������.

*/
	void loop
	(
		struct Position & position, /// ������������� �����
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

//////////////////////////////////////////////////////////////////////////////////////
//private:

/*
�������� ������ ������� �����, �������� ������ ���������
��������� ���-���� �� ������������, 
�������� ������������ ������� ���� ������� ���� ����� ��������
*/
	void test_loop
	(
		struct Position & position,// ������������� �����
		__int32 value_root,
 		const __int32 test_eval,// ���������� ������ ��������� ��� ��������
		const unsigned __int64 key_control// ���������� ���-���� ��� ��������
	);

};
