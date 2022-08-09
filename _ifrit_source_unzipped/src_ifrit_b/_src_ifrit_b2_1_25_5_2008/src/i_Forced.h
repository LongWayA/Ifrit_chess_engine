#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 17.05.2007
*/


//---------------------------------------------------------------------------
#define TEST_F 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������

//---------------------------------------------------------------------------
//
int Forced_variant_white(struct T_Bitboard * bitboard,int alpha,int beta,int depth,struct T_PV * pv_current,struct T_PV * pv_best);

//---------------------------------------------------------------------------
//
int Forced_variant_black(struct T_Bitboard * bitboard,int alpha,int beta,int depth,struct T_PV * pv_current,struct T_PV * pv_best);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������,������
#if TEST_F
void List_816_Print_F(int Depth,const struct T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ��������� ������ ���������� ���������� ������
inline int Sorting_captures_moves(struct T_list_surplus_moves * list_surplus_moves);
