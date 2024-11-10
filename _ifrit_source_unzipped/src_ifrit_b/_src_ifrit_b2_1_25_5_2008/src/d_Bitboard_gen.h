#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ��������� �������� ���������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 10.05.2008
 *
*/
//---------------------------------------------------------------------------

#define TEST_B 0   // ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define KONTROL_B 0 // ��������-���������(1-0)
//---------------------------------------------------------------------------
// ���� ������ ������������� ���(�.�. ����� ������)
//static __inline int first_one(unsigned __int64 bm);

//---------------------------------------------------------------------------
// ���� ��������� ������������� ���(�.�. ����� �����)
//static __inline int last_one(unsigned __int64 bm);

//---------------------------------------------------------------------------
// ���������� ������ ������ � ����������� �����
void Generator_captures_white(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves);

//---------------------------------------------------------------------------
// ���������� ������ ������ � ����������� �����
void Generator_captures_black(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves);


//---------------------------------------------------------------------------
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ � ����������� �����)
void Generator_move_white(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves);

//---------------------------------------------------------------------------
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ � ����������� �����)
void Generator_move_black(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves);

//---------------------------------------------------------------------------
// ��� ���� �� ������ ����� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
int King_white_check(const struct T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��� ���� �� ������ ������ ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
int King_black_check(const struct T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��� ���� �� ����� ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
static inline int White_check_square(int sq,const struct T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��� ���� �� ������ ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
static inline int Black_check_square(int sq, const struct T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// �������� �������� �����
#if TEST_B
 void Bitboard_print(const struct T_Bitboard * bitboard);
#endif//TEST_B
//---------------------------------------------------------------------------
// ��������� ������������� �����
void Bitboard_ini_t(struct T_Bitboard * bitboard);

