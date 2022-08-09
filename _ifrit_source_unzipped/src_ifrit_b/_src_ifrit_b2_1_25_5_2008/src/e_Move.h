#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������ ������� �� ���������� �����
 *
 * ����� ChessTerminator75
 * ��������� ��������� 17.05.2008
          ��������
*/
//---------------------------------------------------------------------------
extern const unsigned __int64 move_masks[];
//---------------------------------------------------------------------------
// ��������� ���� ������
// int i ����� ���� �� ������,T_list_surplus_moves ������ ��������� �����,T_board_list_816 �������,T_list_realizable_moves ������ �������� �����
void Onward_moves_white(int i,struct T_undo undo[],struct T_list_surplus_moves * list_surplus_moves,struct T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��������� ���� ������
// int i ����� ���� �� ������,T_list_surplus_moves ������ ��������� �����,T_board_list_816 �������,T_list_realizable_moves ������ �������� �����
void Onward_moves_black(int i,struct T_undo undo[],struct T_list_surplus_moves * list_surplus_moves,struct T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��������� ���� �����
//
void Backs_moves_white(int i,struct T_undo undo[],const struct T_list_surplus_moves * list_surplus_moves,struct T_Bitboard * bitboard);

//---------------------------------------------------------------------------
// ��������� ���� �����
//
void Backs_moves_black(int i,struct T_undo undo[],const struct T_list_surplus_moves * list_surplus_moves,struct T_Bitboard * bitboard);



// � ������� ���� ����� ����� � ������� ������ �� ������� �������
const unsigned __int64 move_masks_m[] ={
1,           // 1 bit �.�. ....0001
2,           // 2 bit �.�. ....0010
4,           // 3 bit �.�. ....0100 
8,           // 4 bit � �.�.
16,          // 5 bit
32,          // 6 bit
64,          // 7 bit
128,         // 8 bit
256,         // 9 bit
512,         // 10 bit
1024,        // 11 bit
2048,        // 12 bit
4096,        // 13 bit
8192,        // 14 bit
16384,       // 15 bit
32768,       // 16 bit
65536,       // 17 bit
131072,      // 18 bit
262144,      // 19 bit
524288,      // 20 bit
1048576,     // 21 bit
2097152,     // 22 bit
4194304,     // 23 bit
8388608,     // 24 bit
16777216,    // 25 bit
33554432,    // 26 bit
67108864,    // 27 bit
134217728,   // 28 bit
268435456,   // 29 bit
536870912,   // 30 bit
1073741824,  // 31 bit
2147483648,  // 32 bit
4294967296,         // 33 bit
8589934592,         // 34 bit
17179869184,        // 35 bit
34359738368,        // 36 bit
68719476736,        // 37 bit
137438953472,       // 38 bit
274877906944,       // 39 bit
549755813888,       // 40 bit
1099511627776,      // 41 bit
2199023255552,      // 42 bit
4398046511104,      // 43 bit
8796093022208,      // 44 bit
17592186044416,     // 45 bit
35184372088832,     // 46 bit
70368744177664,     // 47 bit
140737488355328,    // 48 bit
281474976710656,    // 49 bit
562949953421312,    // 50 bit
1125899906842624,   // 51 bit
2251799813685248,   // 52 bit
4503599627370496,   // 53 bit
9007199254740992,   // 54 bit
18014398509481984,  // 55 bit
36028797018963968,  // 56 bit
72057594037927936,  // 57 bit
144115188075855872, // 58 bit
288230376151711744, // 59 bit
576460752303423488, // 60 bit
1152921504606846976,// 61 bit
2305843009213693952,// 62 bit
4611686018427387904,// 63 bit
9223372036854775808 // 64 bit
};//unsigned __int64 move_masks[] ={
//-----------------------------------------------------------------------------------



