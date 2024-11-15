#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������������ ������� �� �����
 *
 * ����� ChessTerminator75
 * ��������� ��������� 8.1.2007
*/
//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// ���� �� � ������ ������� ���������
// ��������� :������, ������
static bool string_equal(const char s1[], const char s2[]);

//-----------------------------------------------------------------------------------
// ���� �� � ������ ������ ������� ���������
// ��������� :������, ������
static bool string_start_with(const char s1[], const char s2[]);

//-----------------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ����������� ������ � ������
// ���������: ������ �����������������(���������),��������� �������������� ����(��� ���������� �����)
void transformation_moves(char three_position[],T_PV * three_moves);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// �������� ������ � ������
// ���������: ������,������
void parse_three_position(const char string_in[],char three_position[]);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ������������ ������� �� ����� � ��������� �������
// ��������� :�������� ������,������, �����(138)
// ������ ����� ��� ��������� ������� �������� 
void parse_go(T_Bitboard * bitboard,const char string_in[],char three_position[],int & nodes);

//---------------------------------------------------------------------------