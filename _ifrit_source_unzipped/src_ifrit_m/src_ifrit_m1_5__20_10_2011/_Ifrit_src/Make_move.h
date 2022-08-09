
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010
/// ������� ����� ������� 99%



///!
#include "Header_constants.h"
#include "Position.h"
#include "List.h"
#include "Undo.h"

/// ��������-���������(1-0) ����� ������� ���������
#define MATERIAL 1 


/// ������ ������� �� ���������� �����

/// ������� ������ �� ���������� ����� �� ������ List_surplus_moves
/// � ����� ������ ����������� ����� Chess_bitboard

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