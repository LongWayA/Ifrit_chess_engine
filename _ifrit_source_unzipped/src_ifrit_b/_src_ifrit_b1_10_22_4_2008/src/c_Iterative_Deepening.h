#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������� � ����������� ����������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 10.02.2008
*/


//---------------------------------------------------------------------------
#define TEST_D 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������

//---------------------------------------------------------------------------
// ��������� : �����(138),������� �������,������ ���,������,���������� ������������� �����,�������
void Deepening(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int depth,char three_position[],T_PV three_moves,int & nodes,double wtime,double btime, double winc,double binc,int movestogo,double movetime,bool infinite);

//---------------------------------------------------------------------------