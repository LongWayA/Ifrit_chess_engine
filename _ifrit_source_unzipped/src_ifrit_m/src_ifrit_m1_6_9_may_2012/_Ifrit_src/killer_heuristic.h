
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ���������  25.02.2009
/// ������� ����� ������� 80%



///!
#include "constants_header.h"
#include "move_list_struct.h"


/// ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_K 0 

/*

Killer Heuristic
http://chessprogramming.wikispaces.com/Killer+Heuristic

 The Killer Heuristic is a dynamic, path-dependent move ordering 
 technique. It considers moves that caused a beta-cutoff in a 
 sibling node as killer moves and orders them high on the list. 
 When a node fails high, a non-capturing move that caused a 
 cutoff is stored in a table indexed by depth, typically 
 containing two or three moves per ply. The replacement 
 scheme ought to ensure that all the available slots 
 contain different moves.

 In move ordering, killer moves usually come right after after 
 the hash move and (good) captures. The logic behind this 
 heuristic is as follows. In many positions there is only a 
 small set of moves creating a threat or defending against it, 
 and those that cannot do it might be refuted ("killed") by the 
 same move by the opponent. Apart from the killer moves from the 
 same depth, some programs use killers from two plies ago. Also 
 the mate killers are often separated and treated differently.

 As far as relative position of captures and killer moves 
 is concerned, there are different schemes. Sometimes killer 
 moves are sorted below all the captures, sometimes - between 
 equal and losing captures. One rare idea was to place them 
 even before winning captures of a pawn.


 ��������� �������

 ������ ��� ��� ������ ��� ��������� �� ����� �� ������ �������
 �� �� ������ ���� ������� �.�. � �������� �������� ������

 ��� ������ ������? 
 ��� ������  
 ��� �� ������� ����� � ������ ������� �� ����
 ������� ��� ��� ������ �� �����
 ��� ������ ������� ���� � ������ ������� �� �����

*/

namespace Killer_heuristic
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:


//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	///
	__int32 get_killer_moves_1(const unsigned __int16 n);

	///---------------------------------------------------------------------------
	/// 
	__int32 get_killer_moves_2(const unsigned __int16 n);

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// �������������� ������ ��������
	void ini_killer();

	///---------------------------------------------------------------------------
	/// ����� �������
	void save_killer
	(
		const unsigned __int8 i,/// ����� ���� � ������
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 depth/// ������� �������
	);

	///---------------------------------------------------------------------------
	/// ��������� ������� � ������ ������(������� start) 
	void insert_killer
	(
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 depth,/// ������� �������
		unsigned __int8 start///������� � ������� ���� � �� ������� ��������� ������
	);

	///---------------------------------------------------------------------------
	/// ��������� ������� � ������ ������(������� start) 
	void insert_killer_in_list
	(
		struct List & list,/// ������������� ������ �����
		const __int32 killer_moves_1,/// 
		const __int32 killer_moves_2,///
		const unsigned __int8 start///������� � ������� ���� � �� ������� ��������� ������
	);


};
