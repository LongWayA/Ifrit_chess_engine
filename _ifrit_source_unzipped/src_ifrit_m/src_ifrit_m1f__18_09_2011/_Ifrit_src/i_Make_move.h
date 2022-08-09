
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010
/// ������� ����� ������� 99%



///!
#include "a_Header_constants.h"
#include "a_Position.h"
#include "a_List.h"
#include "a_Undo.h"

/// ��������-���������(1-0) ����� ������� ���������
#define MATERIAL 1 


/// ������ ������� �� ���������� �����

/// ������� ������ �� ���������� ����� �� ������ List_surplus_moves
/// � ����� ������ ����������� ����� Chess_bitboard

class i_Make_move
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
private:

	///���������� ��� ������ ����
	struct Undo d_undo[MAX_DEPTH];

/// ������ � ����� ������
//////////////////////////////////////////////////////////////////////////////////////
public:

	/// 
	inline struct Undo get_d_undo(const unsigned __int8 depth)
	{
		return  d_undo[depth];
	};


//////////////////////////////////////////////////////////////////////////////////////
public:

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