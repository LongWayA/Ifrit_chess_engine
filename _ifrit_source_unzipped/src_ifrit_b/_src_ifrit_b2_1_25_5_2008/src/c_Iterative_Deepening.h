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
void Deepening(struct T_Bitboard * bitboard,int depth,char three_position[],struct T_three_moves three_moves,int & nodes,double wtime,double btime, double winc,double binc,int movestogo,double movetime,bool infinite);

//---------------------------------------------------------------------------