
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 21.09.2011
/// ������� ����� ������� 99%

#ifndef PV_LINE_H
#define PV_LINE_H

//!
#include "Header_constants.h"

struct PV_line
{
	/// �������� ���� �������� � ������ List
	__int32 move[MAX_DEPTH]; 

	/// ������ ��������
	__int32 score_move;

	/// ����� ��������
	unsigned __int8  depth_max;

};


#endif // !PV_LINE_H






