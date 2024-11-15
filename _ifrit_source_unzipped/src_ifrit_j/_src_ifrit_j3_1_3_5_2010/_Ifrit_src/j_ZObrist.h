#include "a_Structure.h"

#define TEST_H 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ������ 2.05.10
 * ��������� ���������
*/
//---------------------------------------------------------------------------

/*
 ��������� ZObrist-�����
*/

// ���� ������ ���������� �� ��� �� ����������� � ��� ������� �����������??
// � ������� � ���� ����� ��� ������� ��������!
#define U64(u) (u##ui64)

namespace ZOBRIST{

//////////////////////////////////////////////////////////////////////////////////////////
//public:

// ������ � ��� - ������ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// ������ ���� �� ����� ����
void Modification_random_key(bool  colour_figure,// ���� ������� �������
							 unsigned __int8 i,// ����� ���� �� ������
							 struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
							 struct T_Bitboard * p_bitboard,// ������������� �����
                             struct T_undo undo[]//���������� ��� ������ ����
							 );

//---------------------------------------------------------------------------
// �������������� ���� ��������� �������
void Start_position_random_key(struct T_Bitboard * p_bitboard// ������������� �����
							   );

//---------------------------------------------------------------------------
// ������ ��������� ������� ������ ��������� ������ 
// ����������� ��������� �����
void Ini_random_piese_key();

//---------------------------------------------------------------------------
// ���������� ��������� 64 ������ ����
unsigned __int64 rand_64();


// ������� ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// ���������� ���������� ����� � ������ ���� ���������� �� ��������
__int32 get_save_cnt_move();

//---------------------------------------------------------------------------
// ���������� ���-���� �� ������� �������
// ����� ��� ��������� �������� � ����� ��������
void Save_hash_three(unsigned __int64 key,// ���-����
					 unsigned __int8 depth // �������
					 );

//---------------------------------------------------------------------------
// ���������� ���-���� ��� �������� ����
// ����� ��� ��������� �������� � ����� ������ ���������� ���������
void Save_hash_string_moves(unsigned __int64 key,// ���-����
					        unsigned __int16 cnt_move // ������� �����
					        );

//---------------------------------------------------------------------------
// ����������� ����� ������ ���������� �� ������ ������. ���� ����� ���������� �� ���������� 1 ����� 0
// ������� ���������� �� ������ ���� ����������� ���� ����� ��� ��� �����
bool Analysis_hash_string_moves(unsigned __int8 depth // �������
						        );

//---------------------------------------------------------------------------
// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
void Ini_hash_three();

//---------------------------------------------------------------------------
// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
void Ini_hash_string_moves();

//---------------------------------------------------------------------------

};//namespace ZOBRIST