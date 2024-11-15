#ifndef A_STRUCTURE_H
#define A_STRUCTURE_H

#include "All_constants.h" 

//--------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 17.05.2008
*/
//--------------------------------------------------------------------------

/*
 ������� ����� ������� 90%

  ��������� ���������
  ��������� ���� �������� ������������ � ���������
*/

//--------------------------------------


// unsigned __int8 : 0 to 255
//          __int8 : -128 to 127

// unsigned __int16 :  0 to 65535
//          __int16 : -32768 to 32767

// unsigned __int32 : 0 to 4294967295
//          __int32 : -2147483648 to 2147483647

// unsigned __int64 : 0 to 
//          __int64 : - to 

//---------------------------------------------------------------------------
// ��� ��������� ����� �����������(��������) ����� ��� �������������� ������� ����� ����
struct Undo
{
	// ���� �������� ������ �� ������� 1/0
	bool return_taking_passage;

	// � ���������� ������ ����(������������ �� �����) 
	// ��� � - 0 , h - 7 ����� ������� �� � ����������� ������
	unsigned __int8  taking_passage_x;

	// � ���������� ������ ����
	// ��� '1' - 0 , '8' - 7 ����� ������� ����������� ���� ������ ������ �������� �� 1
	unsigned __int8  taking_passage_y;

	// ���� ��������� ����� � ������� �������
	bool return_castling_Q;

	// ���� ��������� ����� � �������� �������
	bool return_castling_K;

	// ���� ��������� ������ � ������� �������
	bool return_castling_q;

	// ���� ��������� ������ � �������� �������
	bool return_castling_k;

};

//---------------------------------------------------------------------------
// ����� ��������
struct PV_line
{
	// �������� ���� �������� � ������ List_surplus_moves
	__int32 move[MAX_DEPTH]; 

	// ������ ��������
	__int32 score_move;

	// ����� ��������
	unsigned __int8  depth_max;

};

//---------------------------------------------------------------------------

#endif // !defined D_STRUCTURE_H






