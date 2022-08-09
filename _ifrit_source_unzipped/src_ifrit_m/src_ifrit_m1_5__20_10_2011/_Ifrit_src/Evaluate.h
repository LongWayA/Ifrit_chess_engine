
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 24.02.2009 09.10.2011
/// ������� ����� ������� 70%



#include "Position.h"


/// ��������� �������

/// ������� 
/// �������� (���� ����� ������� � ���� ������ ��������)
/// ����������� ������ PST
/// ����������� ����� (����������)
/// ����� �� ����������� ������ (����������)
/// ���������


namespace Evaluate 
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	///  ������� �������� ������� � ��������� ������� ���������. ����� - ������
	__int32 eval_difference_material
	(
		struct Position & position/// ������������� �����
	);

	///---------------------------------------------------------------------------
	/// ������� �������� ������� � ��������� �������� ��� �����.
	__int32 eval_white_material
	(
		struct Position & position/// ������������� �����
	);

	///---------------------------------------------------------------------------
	/// ������� �������� ������� � ��������� �������� ��� ������.
	__int32 eval_black_material
	(
		struct Position & position/// ������������� �����
	);

	///---------------------------------------------------------------------------
	/// ������� �������� ������� � ��������� �� ����������
	__int32 eval_state
	(
		struct Position & position/// ������������� �����
	);


//////////////////////////////////////////////////////////////////////////////////////
//private:

	///---------------------------------------------------------------------------
	/// ������� ���������� ����
	//inline __int32 popcnt(unsigned __int64 analyzed_number);

	///---------------------------------------------------------------------------
	///
	inline void king_pst_opening
	(
		struct Position & position,/// ������������� �����
		unsigned __int8 & from_white_king,/// ������������ ������ ������
		unsigned __int8 & from_black_king,/// ������������ ������� ������
		const unsigned __int16 stadia_w, /// �������� ��� ����� � �����
		const unsigned __int16 stadia_b, /// �������� ��� ����� � ������
		__int32 & eval ///����������� ������
	);

	///---------------------------------------------------------------------------
	///
	inline void king_pst_endgame
	(
		struct Position & position,/// ������������� �����
		unsigned __int8 & from_white_king,/// ������������ ������ ������
		unsigned __int8 & from_black_king,/// ������������ ������� ������
		const unsigned __int16 stadia_w, /// �������� ��� ����� � �����
		const unsigned __int16 stadia_b, /// �������� ��� ����� � ������
		__int32 & eval ///����������� ������
	);

	///---------------------------------------------------------------------------
	///
	inline void pawn_pst_opening
	(
		struct Position & position,/// ������������� �����
		const unsigned __int8 from_white_king,/// ������������ ������ ������
		const unsigned __int8 from_black_king,/// ������������ ������� ������
		const unsigned __int16 stadia_w, /// �������� ��� ����� � �����
		const unsigned __int16 stadia_b, /// �������� ��� ����� � ������
		__int32 & eval ///����������� ������
	);

	///---------------------------------------------------------------------------
	///
	inline void pawn_pst_endgame
	(
		struct Position & position,/// ������������� �����
		const unsigned __int8 from_white_king,/// ������������ ������ ������
		const unsigned __int8 from_black_king,/// ������������ ������� ������
		const unsigned __int16 stadia_w, /// �������� ��� ����� � �����
		const unsigned __int16 stadia_b, /// �������� ��� ����� � ������
		__int32 & eval ///����������� ������
	);

	///---------------------------------------------------------------------------
	///
	inline void PST_opening
	(
		unsigned __int64 mask_mobility_from_coord[MAX_COORDINATE],
		struct Position & position,/// ������������� �����
		const unsigned __int8 from_white_king,/// ������������ ������ ������
		const unsigned __int8 from_black_king,/// ������������ ������� ������
		unsigned __int8 & at_white_king, /// ���������� ���������� ����� ������ ������ ������
		unsigned __int8 & at_black_king, /// ���������� ���������� ����� ������ ������� ������
		__int32 & eval ///����������� ������
	);

	///---------------------------------------------------------------------------
	///
	inline void PST_endgame
	(
		unsigned __int64 mask_mobility_from_coord[MAX_COORDINATE],
		struct Position & position,/// ������������� �����
		const unsigned __int8 from_white_king,/// ������������ ������ ������
		const unsigned __int8 from_black_king,/// ������������ ������� ������
		unsigned __int8 & at_white_king, /// ���������� ���������� ����� ������ ������ ������
		unsigned __int8 & at_black_king, /// ���������� ���������� ����� ������ ������� ������
		__int32 & eval ///����������� ������
	);

	///---------------------------------------------------------------------------
	/// ����� �� ���������� ���� ����� �������
	/// ���� ��� ��������� ������ �� ����������� ����� 
	/// (������ ���� ��������� ����� �������, �� ��� ����� ����� :))
	inline __int32 king_shield
	(
		struct Position & position,/// ������������� �����
		unsigned __int8 n,/// ��������� ������ (from)
		unsigned __int8 z,/// ���� ������
		const unsigned __int8 SHIELD/// ����� �� ��������� ������
	);

	///---------------------------------------------------------------------------
	///
	inline __int32 popcnt(unsigned __int64 analyzed_number);

};



/*

Other factors that affect material evaluation might be:

Bonus for the bishop pair (bishops complement each other, controlling squares of different color)
Penalty for the rook pair (Larry Kaufman called it "principle of redundancy")
Penalty for the knight pair (as two knights are less successful against the rook than any 
other pair of minor pieces)
decreasing the value of the rook pawns and increasing the value of the central pawns (though this 
can be done in the piece-square tables as well)
Trade down bonus that encourages the winning side to trade pieces but no pawns
Penalty for having no pawns, as it makes it more difficult to win the endgame
Bad trade penalty as proposed by Robert Hyatt, that is penalizing the material imbalances that 
are disadvantageous like having three pawns for a piece or a rook for two minors.


*/







