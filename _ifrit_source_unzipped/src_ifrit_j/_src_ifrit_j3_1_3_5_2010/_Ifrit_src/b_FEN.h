#include "a_Structure.h"

//-----------------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009   11.11.2009
*/
//-----------------------------------------------------------------------------------

/*
������ ������� �� ������ � fen ����������

��������� ��� - ������ � �������������� ���������� ����� 

���������������� ������� ������ ����� ���� ���� ����� 
"position startpos" ������ ��������� �������
"position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� ������� � ������� 
"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� ����� ��� �������������

����� ����������� � ��������� T_Bitboard ������� ��������� � ����� a_Structure.h

� ���������� ������ ������ �� �������� ���������� ������� ��������������� �������
�������������� � ��� - �������������
*/

namespace FEN {

//////////////////////////////////////////////////////////////////////////////////////////
//public:

//---------------------------------------------------------------------------
// ��������� �������� ��� - �������
void parse_position(struct T_Bitboard * p_bitboard,// �����
					char string_in[]//�������� ������(65536)
                    );

//---------------------------------------------------------------------------
//	�� ����� ������ ������ ������	
// a-0 b-1 c-2 d-3 e-4 f-5 g-6 h-7 ����� 777 :)
 int Koordinata_To_Indeks(char ch);

//---------------------------------------------------------------------------
//	�� ������� ����� ������ �����
// ���������� ������ ������� � ���� ��� ����� �������� ��� ���� ������� :)
// '1'-0 '2'-1 '3'-2 '4'-3 '5'-4 '6'-5 '7'-6 '8'-7 ����� 777 :)
 int Ch_Koordinata_To_Indeks(int ch);


//////////////////////////////////////////////////////////////////////////////////////////
//private:

//---------------------------------------------------------------------------
// ��������� ������ ��� � �������������� �����
void fen_to_board(struct T_Bitboard * p_bitboard,// �����
				  const char fen[]//������ ���������� ��� �������(������ ����������)
                  );

//---------------------------------------------------------------------------
// �������� � ������ ������ �.�. ���� ���������� � ������� ������� �� ���������
void move_to_board(struct T_Bitboard * p_bitboard,// �����
				   char move_string[]// ������ ���������� ����(7)
                   );

//---------------------------------------------------------------------------
// � ������� ���� ����� ����� � ������� ������ �� �������� �������
const unsigned __int64 move_masks_f[] ={

// � ���������� ����� ����� ��� ���������� � 1
1,           // 0 bit �.�. ....0001
2,           // 1 bit �.�. ....0010
4,           // 2 bit �.�. ....0100 
8,           // 3 bit � �.�.
16,          // 4 bit
32,          // 5 bit
64,          // 6 bit
128,         // 7 bit
256,         // 8 bit
512,         // 9 bit
1024,        // 10 bit
2048,        // 11 bit
4096,        // 12 bit
8192,        // 13 bit
16384,       // 14 bit
32768,       // 15 bit
65536,       // 16 bit
131072,      // 17 bit
262144,      // 18 bit
524288,      // 19 bit
1048576,     // 20 bit
2097152,     // 21 bit
4194304,     // 22 bit
8388608,     // 23 bit
16777216,    // 24 bit
33554432,    // 25 bit
67108864,    // 26 bit
134217728,   // 27 bit
268435456,   // 28 bit
536870912,   // 29 bit
1073741824,  // 30 bit
2147483648,  // 31 bit
4294967296,         // 32 bit
8589934592,         // 33 bit
17179869184,        // 34 bit
34359738368,        // 35 bit
68719476736,        // 36 bit
137438953472,       // 37 bit
274877906944,       // 38 bit
549755813888,       // 39 bit
1099511627776,      // 40 bit
2199023255552,      // 41 bit
4398046511104,      // 42 bit
8796093022208,      // 43 bit
17592186044416,     // 44 bit
35184372088832,     // 45 bit
70368744177664,     // 46 bit
140737488355328,    // 47 bit
281474976710656,    // 48 bit
562949953421312,    // 49 bit
1125899906842624,   // 50 bit
2251799813685248,   // 51 bit
4503599627370496,   // 52 bit
9007199254740992,   // 53 bit
18014398509481984,  // 54 bit
36028797018963968,  // 55 bit
72057594037927936,  // 56 bit
144115188075855872, // 57 bit
288230376151711744, // 58 bit
576460752303423488, // 59 bit
1152921504606846976,// 60 bit
2305843009213693952,// 61 bit
4611686018427387904,// 62 bit
9223372036854775808 // 63 bit
};//unsigned __int64 move_masks[] ={
//-----------------------------------------------------------------------------------

};//namespace FEN

