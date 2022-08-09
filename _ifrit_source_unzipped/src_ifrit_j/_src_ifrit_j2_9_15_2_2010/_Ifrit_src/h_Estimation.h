#include "a_structure.h"
//-----------------------------------------------------------------------------------
/* +
 * ������ �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 24.02.2009
*/
//---------------------------------------------------------------------------

/*******************************************
����� EVAL
��������� �������

������� 
�������� (���� ����� ������� � ���� ������ ��������)
����������� ������ PST
����������� �����
����� �� ����������� ������
���������

-------------------------------------------------------------------
���� ���� ��� ������ ���� ������.
���� ����� ������ ��������� ��������� (������� �� WildCat(����� ��������)
1. ���������                -195 ��
2. �������� �����           -173 ��
4. �����������              - 78 ��
3. �������� ���             - 69 ��
5. ������� ������           - 29 ?
6. �������� ���������       - 25 ?
7. ����������� ����         - 18 ?
8. ����� �� ������          -  9 ��
9. ��������                 -  0 ?

�������� ���������� ����:

���: �������� ��
10. ���������               - 19 ?
-------------------------------------------------------------------


********************************************/


class EVAL {

// � ������� ���� ����� ����� � ������� ������ �� �������� �������
static const unsigned __int64 e_move_masks[];

// ����� ����� ���� �����
static const unsigned __int64 e_rook_up_masks[];

// ����� ����� ���� ����
static const unsigned __int64 e_rook_down_masks[];

// ����� ����� ���� �����
static const unsigned __int64 e_rook_left_masks[];

// ����� ����� ���� ������
static const unsigned __int64 e_rook_right_masks[];


// ����� ���� ���� �����-������
static const unsigned __int64 e_bishop_up_right_masks[];

// ����� ���� ���� �����-�����
static const unsigned __int64 e_bishop_up_left_masks[];

// ����� ���� ���� ����-������
static const unsigned __int64 e_bishop_down_right_masks[];

// ����� ���� ���� ����-�����
static const unsigned __int64 e_bishop_down_left_masks[];


// ����� ����� ������
static const unsigned __int64 e_king_moves_masks[];


// ����� ����� ����
static const unsigned __int64 e_knigt_moves_masks[];

//////////////////////////////////////////////////////////////////////////////////////////
public:

//---------------------------------------------------------------------------
// ��������� ��������� ������ �� �������� �����
bool Read_parameters_eval(
			              );

//---------------------------------------------------------------------------
// ������ ��������� ������ ������ ����
void Default_parameters_eval(
			                 );

//---------------------------------------------------------------------------
// ������������� ��������� ������
void Ini_parameters_eval(
			             );

//---------------------------------------------------------------------------
// ������� �������� ������� � ��������� �������� � ������
__int32 Eval_forced(const T_Bitboard * p_bitboard// ������������� �����
				);

//---------------------------------------------------------------------------
// ������� �������� ������� � ��������� �� ����������
__int32 Eval_State(const T_Bitboard * p_bitboard// ������������� �����
			   );

//////////////////////////////////////////////////////////////////////////////////////////
private:

//---------------------------------------------------------------------------
//
inline void EVAL::King_pst(const T_Bitboard * p_bitboard,// ������������� �����
                           unsigned __int8 & from_white_king,// ������������ ������ ������
                           unsigned __int8 & from_black_king,// ������������ ������� ������
                           unsigned __int16 stadia_w, // �������� ��� ����� � �����
                           unsigned __int16 stadia_b, // �������� ��� ����� � ������
                           __int32 & eval //����������� ������
                           );

//---------------------------------------------------------------------------
//
inline void EVAL::Pawn_pst(const T_Bitboard * p_bitboard,// ������������� �����
                           unsigned __int8 & from_white_king,// ������������ ������ ������
                           unsigned __int8 & from_black_king,// ������������ ������� ������
                           unsigned __int16 stadia_w, // �������� ��� ����� � �����
                           unsigned __int16 stadia_b, // �������� ��� ����� � ������
                           __int32 & eval //����������� ������
                           );

//---------------------------------------------------------------------------
//
inline void EVAL::PST(const T_Bitboard * p_bitboard,// ������������� �����
                      unsigned __int8 from_white_king,// ������������ ������ ������
                      unsigned __int8 from_black_king,// ������������ ������� ������
                      unsigned __int8 & at_white_king, // ���������� ���������� ����� ������ ������ ������
                      unsigned __int8 & at_black_king, // ���������� ���������� ����� ������ ������� ������
                      __int32 & eval //����������� ������
                      );

//---------------------------------------------------------------------------
__inline __int32 first_one(unsigned __int64 bm);

//---------------------------------------------------------------------------
__inline __int32 last_one(unsigned __int64 bm);

//---------------------------------------------------------------------------
// ����� �� ���������� ���� ����� �������
inline __int32 King_shield(unsigned __int8 n,// ��������� ������ (from)
					   unsigned __int8 z,// ���� ������
					   const T_Bitboard * p_bitboard,// ������������� �����
					   const unsigned __int8 SHIELD// ����� �� ��������� ������
					   );

//---------------------------------------------------------------------------
// ������������ ��� ��������� ���� ����� ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 Rook_amoun(unsigned __int64  all_piece,// ��� ������
                      unsigned __int64  piece,// ���� ������
					  unsigned __int8 from,// ������������ �����
					  unsigned __int8 from_king,// ������������ ������
					  unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					  );

//---------------------------------------------------------------------------
// ������������ ��� ��������� ���� ����� ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 Bishop_amoun(unsigned __int64  all_piece,// ��� ������
                        unsigned __int64  piece,// ���� ������
					    unsigned __int8 from,// ������������ �����
					    unsigned __int8 from_king,// ������������ ������
					    unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
						);

//---------------------------------------------------------------------------
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 Queen_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					   );

//---------------------------------------------------------------------------
// ������������ ��� ��������� ���� ����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 Knigt_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ ����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					   );

////////////////////////////////////////////////////////////////////////////////////////////
public:

// �������� ��� ������� ���������
unsigned __int32 KING; // ������
unsigned __int32 QUEEN;// �����
unsigned __int32 ROOK;// ����� 
unsigned __int32 BISHOP;// ���� 
unsigned __int32 KNIGHT;// ���� 
unsigned __int32 PAWN;// ����� 

// ������������ ������
unsigned __int8 BONUS_CASTLING_O_O;// ����� �� �������� ���������
unsigned __int8 BONUS_CASTLING_O_O_O;// ����� �� ������� ���������
unsigned __int8 BONUS_SHIELD;// ����� �� ��������� ������ ������� ������
unsigned __int8 BONUS_PAWNN;// ����� �� ��������� �����
unsigned __int8 PERSPECTIVE_PAWNN;// ��������� ����� ����� �� ������ ��������� (���� 3)
unsigned __int8 CONTROL_SQUARE ;// ����������� ������� ������������ ��������
unsigned __int8 ATAK_KING;// ����������� ����� �� ������

// ��� ������ ����� ����� ������� ������ � �������� ����� ����� ������
unsigned __int16 rubikon;//����� �������� � ��������(��� ����� � ���� � �����)
            // ��� ������ ��������� � ����� ������ �� ��������  


// ��������� ������� pst(piece square tables)
__int8 test[64];

// ���� ������ ���������� � ������
__int8 Pozicion_n[64];

// ����� ������ ���������� �������������� ������� ���������
__int8 Pozicion_b[64];

//����� ������ �� �����������
//������ ����������� �������� �������
//�������� �.�. hgfedcba
__int8 Pawn[64];

//����� ������ �� ����������� ���������� ��� ��������
// ����� ������� ������
__int8 Pawn_e[64];


//����� ��������� ������ �� ����������� ���������� ��� ��������
// ����� ������� ������
__int16 Perspective_pawn_e[64];

// ������ ������ ���������
__int8 Pozicion_k[64];

// � �������� ������ ������ ���������� � ������
// ��� ������� ������� �� ������������� ������
__int8 Pozicion_k_e[64];

// ������ ����� �����: 0 - 0, 1 - �����, 2 - ����, 3- ����, 4 - �����, 5 - �����, 6 - ������;
unsigned __int32 kod[7];

// ��� ������ 0 - ��� ������ 1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 6 - ������
// ������ ������������� ��������� (�����(1) � ������(6) �� �����������)
//                0 1 2      3      4    5     6
unsigned __int32 rubikon_kod[7];

// ��� ���� ��� �� �������� ��� ���� ����������
static const __int8 c_test[];

// ���� ������ ���������� � ������
static const __int8 c_Pozicion_n[];

// ����� ������ ���������� �������������� ������� ���������
static const __int8 c_Pozicion_b[];

//����� ������ �� �����������
//������ ����������� �������� �������
//�������� �.�. hgfedcba
static const __int8 c_Pawn[];

//����� ������ �� ����������� ���������� ��� ��������
// ����� ������� ������
static const __int8 c_Pawn_e[];

//����� ��������� ������ �� ����������� ���������� ��� ��������
// ����� ������� ������
static const __int16 c_Perspective_pawn_e[];

// ������ ������ ���������
static const __int8 c_Pozicion_k[];

// � �������� ������ ������ ���������� � ������
// ��� ������� ������� �� ������������� ������
static const __int8 c_Pozicion_k_e[];


};//class EVAL
