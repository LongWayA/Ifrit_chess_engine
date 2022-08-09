#include "Chess_bitboard.h"

//---------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 24.02.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

��������� �������

������� 
�������� (���� ����� ������� � ���� ������ ��������)
����������� ������ PST
����������� �����
����� �� ����������� ������
���������

-------------------------------------------------------------------
���� ���� ��� ������ ���� ������.
���� ����� ������ ��������� ��������� (������� �� WildCat(����� ��������)
1. ���������                -195 ��
2. �������� �����           -173 ��
4. �����������              - 78 ��
3. �������� ���             - 69 ��
5. ������� ������           - 29 ?
6. �������� ���������       - 25 ?
7. ����������� ����         - 18 ?
8. ����� �� ������          -  9 ��
9. ��������                 -  0 ?

�������� ���������� ����:

���: �������� ��
10. ���������               - 19 ?
-------------------------------------------------------------------
*/

class Evaluate 
{

//////////////////////////////////////////////////////////////////////////////////////////
private:

// ������ ��������� ����� (������������ ���� �� ������� ����)
class List_surplus_moves list_surplus_moves;


// ���������� � �����
// Evaluate.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	//  ������� �������� ������� � ��������� ������� ���������. ����� - ������
	__int32 eval_difference_material
	(
		const Chess_bitboard & bitboard// ������������� �����
	);

	//---------------------------------------------------------------------------
	// ������� �������� ������� � ��������� �������� ��� �����.
	__int32 eval_white_material
	(
		const Chess_bitboard & bitboard// ������������� �����
	);

	//---------------------------------------------------------------------------
	// ������� �������� ������� � ��������� �������� ��� ������.
	__int32 eval_black_material
	(
		const Chess_bitboard & bitboard// ������������� �����
	);

	//---------------------------------------------------------------------------
	// ������� �������� ������� � ��������� �� ����������
	__int32 eval_state
	(
		const Chess_bitboard & bitboard// ������������� �����
	);


// ���������� � �����
// Evaluate.cpp
//////////////////////////////////////////////////////////////////////////////////////////
private:

	//---------------------------------------------------------------------------
	// ������� ���������� ����
	//inline __int32 popcnt(unsigned __int64 analyzed_number);

	//---------------------------------------------------------------------------
	//
	inline void king_pst
	(
		const Chess_bitboard & bitboard,// ������������� �����
		unsigned __int8 & from_white_king,// ������������ ������ ������
		unsigned __int8 & from_black_king,// ������������ ������� ������
		const unsigned __int16 stadia_w, // �������� ��� ����� � �����
		const unsigned __int16 stadia_b, // �������� ��� ����� � ������
		__int32 & eval //����������� ������
	);

	//---------------------------------------------------------------------------
	//
	inline void pawn_pst
	(
		const Chess_bitboard & bitboard,// ������������� �����
		const unsigned __int8 from_white_king,// ������������ ������ ������
		const unsigned __int8 from_black_king,// ������������ ������� ������
		const unsigned __int16 stadia_w, // �������� ��� ����� � �����
		const unsigned __int16 stadia_b, // �������� ��� ����� � ������
		__int32 & eval //����������� ������
	);

	//---------------------------------------------------------------------------
	//
	inline void PST
	(
		const Chess_bitboard & bitboard,// ������������� �����
		const unsigned __int8 from_white_king,// ������������ ������ ������
		const unsigned __int8 from_black_king,// ������������ ������� ������
		unsigned __int8 & at_white_king, // ���������� ���������� ����� ������ ������ ������
		unsigned __int8 & at_black_king, // ���������� ���������� ����� ������ ������� ������
		__int32 & eval //����������� ������
	);

	//---------------------------------------------------------------------------
	// ����� �� ���������� ���� ����� �������
	// ���� ��� ��������� ������ �� ����������� ����� 
	// (������ ���� ��������� ����� �������, �� ��� ����� ����� :))
	inline __int32 king_shield
	(
		unsigned __int8 n,// ��������� ������ (from)
		unsigned __int8 z,// ���� ������
		const Chess_bitboard & bitboard,// ������������� �����
		const unsigned __int8 SHIELD// ����� �� ��������� ������
	);

	//---------------------------------------------------------------------------
	// ������������ ��� ��������� ���� ����� ���������� ���������� �����
	// � ���������� ����������� ����� ������ ������
	inline unsigned __int8 rook_amoun
	(
		unsigned __int64 all_piece,// ��� ������
		unsigned __int64 piece,// ���� ������
		unsigned __int8 from,// ������������ �����
		unsigned __int8 from_king,// ������������ ������
		unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
	);

	//---------------------------------------------------------------------------
	// ������������ ��� ��������� ���� ����� ���������� ���������� �����
	// � ���������� ����������� ����� ������ ������
	inline unsigned __int8 bishop_amoun
	(
		unsigned __int64 all_piece,// ��� ������
		unsigned __int64 piece,// ���� ������
		unsigned __int8 from,// ������������ �����
		unsigned __int8 from_king,// ������������ ������
		unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
	);

	//---------------------------------------------------------------------------
	// ������������ ��� ��������� ���� �����. ���������� ���������� �����
	// � ���������� ����������� ����� ������ ������
	inline unsigned __int8 queen_amoun
	(
		unsigned __int64 all_piece,// ��� ������
		unsigned __int64 piece,// ���� ������
		unsigned __int8 from,// ������������ �����
		unsigned __int8 from_king,// ������������ ������
		unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
	);

	//---------------------------------------------------------------------------
	// ������������ ��� ��������� ���� ����. ���������� ���������� �����
	// � ���������� ����������� ����� ������ ������
	inline unsigned __int8 knigt_amoun
	(
		unsigned __int64 all_piece,// ��� ������
		unsigned __int64 piece,// ���� ������
		unsigned __int8 from,// ������������ ����
		unsigned __int8 from_king,// ������������ ������
		unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
	);

};