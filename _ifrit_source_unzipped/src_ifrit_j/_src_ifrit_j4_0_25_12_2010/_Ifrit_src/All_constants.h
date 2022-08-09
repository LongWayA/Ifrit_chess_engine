#ifndef A_ALL_CONSTANTS_H
#define A_ALL_CONSTANTS_H

//--------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 27.11.2010
*/
//--------------------------------------------------------------------------

/*
 ������� ����� ������� 90%

  ��������� ����� ���� ����������� �������� ������������ � ���������

  ����� ��� ��� ������ � ����� a_List_surplus_moves_const.h,  h_Evaluate_const.h


*/


//------------------------------------------------------------------------------
// ���������� � g_Search.cpp g_Search_root.cpp

	// NULL MOVE
	const unsigned __int8 NULL_YES = 1; // �� ������ �������� ���� 
	const unsigned __int8 NULL_NO = 0; // �� ������� �������� ���� 
	const unsigned __int8 NULL_DEPTH = 2; // ��������� �� ��� ������ ������� NULL �� ����������
	const unsigned __int8 SWITCH_NULL_REDUCTION_1_2 = 5; // ������� �� ������� �������� �������� �������� ���� 
										// � NULL_REDUCTION_ONE �� NULL_REDUCTION_TWO
	const unsigned __int8 NULL_REDUCTION = 4; // 4 ���������� ������� �������� ��� NULL
	const unsigned __int8 NULL_REDUCTION_ONE = 4; // 4 ���������� ������� �������� ��� ������������� NULL
	const unsigned __int8 NULL_REDUCTION_TWO = 3; // 3 ���������� ������� �������� ��� ������������� NULL
	const unsigned __int8 VERIFICATOIN_SEARCH_REDUCTION = 5; // ���������� ������� �������� 
											//��� ������ �� ����������� ���������
	// LMR
	const unsigned __int8 LMR_DEPTH = 3; // ��������� �� ��� ������ ������� LMR �� ����������
	const unsigned __int8 LMR_REDUCTION_D = 1; // ���������� ������� �������� ��� LMR
	const unsigned __int8 FULL_SEARCH = 2;  // ���� 2
											// ����� ����� � ������ ������� ��� ���������� 
											// ��� �� ���-�������, ������ ������,
											// ��� �������, ��� ������ �� ������������
											// ���-��� ��� ������� ��� 3 � ����� ���� ������ ��� � ���� 
											// ���� ��� ������� ��� ������� �������� i > lmr_reduction

	// FUTILITY
	const __int32 FUTILITY_DEPTH = 5; //4
	const __int32 futility_margin[] = {130, 130, 300, 300, 300, 300, 300, 300};

	// ���������
	const unsigned __int8 CHECK_YES = 1; // ���� ���
	const unsigned __int8 CHECK_NO = 0; // ��� ����


	const unsigned __int8 MOVE_IS_CAPTURE = 2; // ��� ��� ������
	const unsigned __int8 CAPTURE_NO = 0; // ��� ������

	const unsigned __int8 PV_YES = 1; // �������� �������
	const unsigned __int8 PV_NO = 0; // 


//------------------------------------------------------------------------------
// ���������� � g_Search_quiescence.cpp g_Search.cpp g_Search_root.cpp
	// 0 - ������ 1200 - ����� 600 - ����� 400 - ����  400 - ���� 100 - �����

	const  __int32 VALUE_MATE = 1000000; // ������ ����
	const  __int16 DELTA_MATE = 1000; // ������ ��� ���������� ���������� ����


//------------------------------------------------------------------------------
// ���������� � a_Chess_bitboard_delta_pruning.cpp

	// 0 - ������ 1200 - ����� 600 - ����� 400 - ����  400 - ���� 100 - �����
	const unsigned __int16 ROOK_MARGIN = 700; // ������ ������ ������
	const unsigned __int16 BISHOP_KNIGHT_MARGIN = 500; // ������ ������  ������ � �����
	const unsigned __int8 PAWN_MARGIN = 160; // ������ ������  �����


//------------------------------------------------------------------------------
// ���������� � j_Hash.h
	// ����� �������� ������������� �������
	const unsigned __int8 EXACT = 1; // ������ ���
	const unsigned __int8 LOWER = 2; // ������� �� ����(�����)
	const unsigned __int8 UPPER = 3; // �����(����) �������� �� �������


//------------------------------------------------------------------------------
// ���������� � a_Structure.h

// unsigned __int8 : 0 to 255
//          __int8 : -128 to 127

// unsigned __int16 :  0 to 65535
//          __int16 : -32768 to 32767

// unsigned __int32 : 0 to 4294967295
//          __int32 : -2147483648 to 2147483647

// unsigned __int64 : 0 to 
//          __int64 : - to 

const unsigned __int16 LENGTH_STRING = 65534; // ������ ����� ������ ������

// + 1 - ������ ��� ������� ������ ������� ���� ���������� 
// � ���������� ��������: 
//���������� �������� = ����������� ��������� ������ + 1

// �������� �� ��� ����� 6 + 1
const unsigned __int8 MAX_NAME = 7;

// �������� ���������� ������ 63 + 1
const unsigned __int8 MAX_COORDINATE = 64;

// ����������� �������������� ����� �������� � ��� �� ������� ������ 100 + 1
const unsigned __int8 MAX_DEPTH = 101;

// ����������� �������������� ���������� ����� �� �������
const unsigned __int16 MAX_MOVE = 256;

// �����
const unsigned __int8 A8=56, B8=57, C8=58, D8=59, E8=60, F8=61, G8=62, H8=63;
const unsigned __int8 A7=48, B7=49, C7=50, D7=51, E7=52, F7=53, G7=54, H7=55;
const unsigned __int8 A6=40, B6=41, C6=42, D6=43, E6=44, F6=45, G6=46, H6=47;
const unsigned __int8 A5=32, B5=33, C5=34, D5=35, E5=36, F5=37, G5=38, H5=39;
const unsigned __int8 A4=24, B4=25, C4=26, D4=27, E4=28, F4=29, G4=30, H4=31;
const unsigned __int8 A3=16, B3=17, C3=18, D3=19, E3=20, F3=21, G3=22, H3=23;
const unsigned __int8 A2=8,  B2=9,  C2=10, D2=11, E2=12, F2=13, G2=14, H2=15;
const unsigned __int8 A1=0,  B1=1,  C1=2,  D1=3,  E1=4,  F1=5,  G1=6,  H1=7;


#endif // !defined A_ALL_CONSTANTS_H






