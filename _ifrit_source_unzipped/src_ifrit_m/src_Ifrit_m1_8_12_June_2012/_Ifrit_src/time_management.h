
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 25.02.2009 5.12.2009
/// ������� ����� ������� 70%


///!
#include <time.h>

/*

Time management
http://chessprogramming.wikispaces.com/Time+Management

Time management refers to algorithms and heuristics to 
allocate time for searching a move under time control 
requirements in a game of chess. The player to move consumes 
his time, and if he exceeds his time limit, the game is lost 
on demand of the opponent player, or in automatic computer 
chess play by an arbiter instance.

Iterative deepening in conjunction with its predictable 
effective branching factor allows a flexible time management 
either to terminate the current iteration and to fall back on 
best move and PV of the previous iteration, or to decide about 
termination prior to start a new iteration or to search a 
next root-move.

end link(����� ������)---------------------
*/

/*
 ������ ������� �� ������ � ��������
 ����������� ������� �����
*/

namespace Time_management
{

 
/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:


//////////////////////////////////////////////////////////////////////////////////////
//public:

/*

*/
	__int32 get_clocks_per_sec();

	///---------------------------------------------------------------------------
	/// ��������� ��������� �����
	void time_start();

	///-----------------------------------------------------------------
	/// ��������� ������� �����
	void time_stop();

	///-----------------------------------------------------------------
	/// ��������� ��������� ����� (��������� � ��������)
	double time_delta();

};
