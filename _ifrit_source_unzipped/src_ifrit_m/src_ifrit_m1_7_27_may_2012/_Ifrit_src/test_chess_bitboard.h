
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010
/// ������� ����� ������� 50%


///!
#include "a_position_struct.h"


/*
 ��������� ��� �����
*/

namespace Test_chess_bitboard
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// �������� 64 ������ ����� ��� �����. ��������� ��� ����� 
	void print_bitboard_map
	(
		const unsigned __int64 mask //
	);

	///---------------------------------------------------------------------------
	/// ��������� ��� �����
	void print_name_from_coord
	(
		const struct Position & position/// ������������� �����
	);

	///---------------------------------------------------------------------------
	/// ��������� ��� �����. ���������� �����
	void save_test_bitboard
	(
		struct Position & position_save,/// ������������� �����
		const struct Position & position/// ������������� �����
	);

	///---------------------------------------------------------------------------
	/// ��������� ��� �����. ���������� �����
	void compare_test_bitboard
	(
		const struct Position & position_1,/// ������������� �����
		const struct Position & position_2/// ������������� �����
	);

	///---------------------------------------------------------------------------
	/// ��� �������� ������� ������ ��� �������� ������� ������
	/// �������� ��������(bit board) �����
	void bitboard_print
	(
		const struct Position & position/// ������������� �����
	);

};