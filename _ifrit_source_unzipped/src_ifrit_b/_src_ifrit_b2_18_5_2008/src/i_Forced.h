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
int Forced_variant_white(T_Bitboard * bitboard,int alpha,int beta,int depth,T_PV * pv_current,T_PV * pv_best);

//---------------------------------------------------------------------------
//
int Forced_variant_black(T_Bitboard * bitboard,int alpha,int beta,int depth,T_PV * pv_current,T_PV * pv_best);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������,������
#if TEST_F
void List_816_Print_F(int Depth,const T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------

