#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ��������� �������� ���������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 20.02.2008
 *
*/
//---------------------------------------------------------------------------


#define TEST_B 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������

//---------------------------------------------------------------------------
static __inline int first_one(unsigned __int64 bm);

//---------------------------------------------------------------------------
static __inline int last_one(unsigned __int64 bm);

//---------------------------------------------------------------------------
void Generator_captures(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves);

//---------------------------------------------------------------------------
void Generator_move(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves);

//---------------------------------------------------------------------------
// ��� ���� �� ������
int King_check(T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ���������� �� ���������
int King_castling(int sq,T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��� ���� �� ����� ����
int White_check_square(int sq, T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��� ���� �� ������ ����
int Black_check_square(int sq, T_Bitboard * bitboard);

//---------------------------------------------------------------------------
#if TEST_B
 void Bitboard_print(T_Bitboard * bitboard);
#endif//TEST_B
//---------------------------------------------------------------------------
//
void Bitboard_ini_t(T_Bitboard * bitboard);


