
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010
/// ������� ����� ������� 99%



///!
#include "constants_header.h"
#include "position_struct.h"
#include "move_list_struct.h"
#include "undo_struct.h"

/// ��������-���������(1-0) ����� ������� ���������
#define MATERIAL 1 

/*

Make Move
http://chessprogramming.wikispaces.com/Make+Move

 Make Move is a function inside a chess program to update the 
 internal chess position and its Board representation as well 
 as associated or dependent state variables and data by a move 
 made on the internal board, such as zobrist keys to index the 
 transposition table. That usually happens inside the Search 
 algorithm, where the move acts like an edge connecting two 
 nodes of a search tree, a parent and a child node. Dependent 
 on the design of the data structures and the used search 
 algorithms there are different approaches with respect to 
 randomly accessing aspects of nodes and restoring the position 
 while unmaking the move.

 The general arithmetical and/or bitwise-logical operations to 
 convert a node to its child are either reversible 
 (even for irreversible moves), by inverse 
 (e.g. addition and subtraction) or own inverse (exclusive or) 
 operations, or are irreversible, for instance a sequence of 
 addition and conjunction (add, and) of move aspects, to update 
 ep state, castling rights and the halfmove clock.

 Aspects of a position altered by reversible operations may 
 be kept in global structures, shared by all nodes of the tree, 
 for instance square centric board arrays like mailbox and 0x88, 
 as well the classical bitboard board-definition, and require 
 incremental updates by make and unmake. Irreversible aspects 
 of the position, which can not generally restored from a 
 child-position by unmaking a move, need to be restored by 
 either re-playing the whole game record from the root position 
 (too slow), or to be memorized inside an array indexed by ply 
 or on a stack (LIFO), most notably ep state, castling rights 
 and the halfmove clock enforcing the fifty-move rule.

 All those aspects of a position, no matter if reversible by 
 unmake or not, which may need random access in a branch of 
 moves from the root (or initial position of the game) to the 
 current position inside a certain implementation of a search 
 algorithm, require an array of those aspects, indexed by ply, 
 and updated by a Copy-Make approach.

 ������ ������� �� ���������� �����

 ������� ������ �� ���������� ����� �� ������ List_surplus_moves
 � ����� ������ ����������� ����� Chess_bitboard

*/

namespace Make_move
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:

/// ������ � ����� ������
//////////////////////////////////////////////////////////////////////////////////////
//public:

	/// 
	struct Undo get_d_undo(const unsigned __int8 depth);


//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// ��������� ���� ������ ��� �����
	void do_moves_white
	(
		struct Position & position, ///
		const unsigned __int8 & i,///����� ���� �� ������
		const unsigned __int8 & depth,///������� ������
		const struct List & list_surplus_moves///������ ��������� �����
	);

	///---------------------------------------------------------------------------
	/// ��������� ���� ������ ��� ������
	void do_moves_black
	(
		struct Position & position, ///
		const unsigned __int8 & i,///����� ���� �� ������
		const unsigned __int8 & depth,///������� ������
		const struct List & list_surplus_moves///������ ��������� �����
	);

	///---------------------------------------------------------------------------
	/// ��������� ���� ����� ��� �����
	void undo_moves_white
	(
		struct Position & position, ///
		const unsigned __int8 & i,///����� ���� �� ������
		const unsigned __int8 & depth,///������� ������
		const struct List & list_surplus_moves///������ ��������� �����
	);

	///---------------------------------------------------------------------------
	/// ��������� ���� ����� ��� ������
	void undo_moves_black
	(
		struct Position & position, ///
		const unsigned __int8 & i,///����� ���� �� ������
		const unsigned __int8 & depth,///������� ������
		const struct List & list_surplus_moves///������ ��������� �����
	);

};