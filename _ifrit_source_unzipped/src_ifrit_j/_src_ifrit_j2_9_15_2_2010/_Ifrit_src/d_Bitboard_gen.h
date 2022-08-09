#include "a_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ��������� ��������(bit board) ���������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 21.02.2009
 *
*/
//---------------------------------------------------------------------------

#define TEST_B 0   // ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define KONTROL_B 0 // ��������-���������(1-0) �������� ������ �����

/*******************************************
 ����� BITBOARDGENERATOR

 ��������� ��������(bit board) ���������
 ��� ������� ��������� ��� �������. � ��� ���������
 ��� ������� ������:
 "Hyatt, Robert. Rotated bitmaps, a new twist on an old idea"

 �� ������� �������� � ��������� T_Bitboard ���������� ������ ����� T_list_surplus_moves
 ������ ����� ��������� ������ ��� �������� ���� ��� ��� � ����������� ���
 + ���������� �������� �����

 �������� �� ��� � ���� ������ �� ������� ������ ��� ��� ������� �������� �����
 ������� �� ��� ������� �� ��������� � ������ ����� ����� ���������� ����

********************************************/
class BITBOARDGENERATOR{

// const 

// � ������� ���� ����� ����� � ������� ������ �� �������� �������
static const unsigned __int64 move_masks[];

// ����� ����� ������
static const unsigned __int64 king_moves_masks[];

// ����� ����� ����
static const unsigned __int64 knigt_moves_masks[];

// ����� ���� �����
// ����� ����� ���� �����
static const unsigned __int64 rook_up_masks[];
// ����� ����� ���� ����
static const unsigned __int64 rook_down_masks[];
// ����� ����� ���� �����
static const unsigned __int64 rook_left_masks[];
// ����� ����� ���� ������
static const unsigned __int64 rook_right_masks[];

// ����� ���� �����
// ����� ���� ���� ����� - ������
static const unsigned __int64 bishop_up_right_masks[];
// ����� ���� ���� ����� - �����
static const unsigned __int64 bishop_up_left_masks[];
// ����� ���� ���� ���� - ������
static const unsigned __int64 bishop_down_right_masks[];
// ����� ���� ���� ���� - �����
static const unsigned __int64 bishop_down_left_masks[];
// ����� ������ ����� �����
static const unsigned __int64 pawn_capture_white[];
// ����� ������ ������ �����
static const unsigned __int64 pawn_capture_black[];

// ��������� ��� ����� �������
// � ������� LINE_1 ������� ����� ������� �� ������ �����������
// ��� ����� ������ �� ��� ���� ������
// � ������� LINE_7 ������� ����� �������� �� ����������� �.�. ��������� ������� �����������
static const unsigned __int64 LINE_1 = 255<<8; // 2 ����������� �����
static const unsigned __int64 LINE_7 = 18374686479671623680; // 8 ����������� �����

static const unsigned __int64 CASTLING_Q = 14; // ����  B1 C1 D1 ������
static const unsigned __int64 CASTLING_K = 96; // ���� F1 G1 ������

// ��������� ��� ������ �������
static const unsigned __int64 LINE_0 = 255; // 1 ����������� ����� (������� �����������)
static const unsigned __int64 LINE_6 = 71776119061217280; // 7 ����������� �����

static const unsigned __int64 CASTLING_q = 1008806316530991104; // ����  B8 C8 D8 ������
static const unsigned __int64 CASTLING_k = 6917529027641081856; // ���� F8 G8 ������


//////////////////////////////////////////////////////////////////////////////////////////
public:

//---------------------------------------------------------------------------
// ���������� ������ ������ + ����������� ����� (��� �����)
void Generator_captures_white(const struct T_Bitboard * const p_bitboard,// ������������� �����
                              struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����                
							  );

//---------------------------------------------------------------------------
// ���������� ������ ������  + ����������� ����� (��� ������)
void Generator_captures_black(const struct T_Bitboard * const p_bitboard,// ������������� �����
                              struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
							  );


//---------------------------------------------------------------------------
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ + ����������� �����)(��� �����)
void Generator_move_white(const struct T_Bitboard * const p_bitboard,// ������������� �����
                          struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						  );

//---------------------------------------------------------------------------
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ + ����������� �����)(��� ������)
void Generator_move_black(const struct T_Bitboard * const p_bitboard,// ������������� �����
                          struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						  );

//---------------------------------------------------------------------------
// ��� ���� �� ������ ����� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
unsigned __int8 King_white_check(const struct T_Bitboard * const p_bitboard// ������������� �����
					 );

//---------------------------------------------------------------------------
// ��� ���� �� ������ ������ ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
unsigned __int8 King_black_check(const struct T_Bitboard * const p_bitboard// ������������� �����
					 );


//---------------------------------------------------------------------------
// �������� ��������(bit board) �����
#if TEST_B
 void Bitboard_print(const struct T_Bitboard * p_bitboard// ������������� �����
	                 );
#endif//TEST_B

//////////////////////////////////////////////////////////////////////////////////////////
private:

//---------------------------------------------------------------------------
// ��� ���� �� ����� ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
inline unsigned __int8 White_check_square(const unsigned __int8 & sq,//������ ������������ ����(0-63) 
							  const struct T_Bitboard * const p_bitboard// ������������� �����
							  );

//---------------------------------------------------------------------------
// ��� ���� �� ������ ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
inline unsigned __int8 Black_check_square(const unsigned __int8 & sq,//������ ������������ ����(0-63) 
							  const struct T_Bitboard * const p_bitboard// ������������� �����
							  );

//---------------------------------------------------------------------------
// ��������� ������������� �����
 inline void Bitboard_ini_t(struct T_Bitboard * p_bitboard// ������������� �����
					        );

//---------------------------------------------------------------------------
// ��������� ������ ����� - ������ �� �����
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
inline void list_capture_white(unsigned __int8 name,// ��� ������� ������
							   unsigned __int8 type_move,// ��� ����
							   const unsigned __int8 & from,// ������ �����
							   const unsigned __int8 & to,// ���� �����
							   unsigned __int8 & count,// ����� ���� � ������
							   const struct T_Bitboard * const p_bitboard,// ������������� �����
                               struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
							   );

//---------------------------------------------------------------------------
// ��������� ������ ����� - ������ �� ������
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
inline void list_capture_black(unsigned __int8 name,// ��� ������� ������
							   unsigned __int8 type_move,// ��� ����
							   const unsigned __int8 & from,// ������ �����
							   const unsigned __int8 & to,// ���� �����
							   unsigned __int8 & count,// ����� ���� � ������
							   const struct T_Bitboard * const p_bitboard,// ������������� �����
                               struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						       );

//---------------------------------------------------------------------------
// ��������� ������ �����
inline void list_record_move(unsigned __int8 name,// ��� ������� ������
							 unsigned __int8 type_move,// ��� ����
							 const unsigned __int8 & from,// ������ �����
							 const unsigned __int8 & to,// ���� �����
							 unsigned __int8 & count,// ����� ���� � ������
							 const struct T_Bitboard * const p_bitboard,// ������������� �����
                             struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
	                         );

//---------------------------------------------------------------------------
// ��������� ������ ����� ��� ������ ������� ���� ��������� ������ �� ������� � ����������� �����
inline void list_record_os(unsigned __int8 name,// ��� ������� ������
						   unsigned __int8 d_m,// ��� ����
						   unsigned __int8 & p,// ����� ���� � ������
						   const unsigned __int8 & from,// ������ �����
						   unsigned __int8 to,// ���� �����
						   unsigned __int8 capture,// ��� ������ ������
                           struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						   );

//---------------------------------------------------------------------------
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ����� ����� ���������� ������
inline void KONTROL_captures_W(const struct T_Bitboard * p_bitboard,// ������������� �����
							   const unsigned __int8 * p_from,// ������ ����� ������
							   const unsigned __int8 * p_to,// ���� ����� ������
							   const unsigned __int8 * p_p,// ����� ���� � ������
							   unsigned __int8 name_figure// ��� ������� ������
							   );

//---------------------------------------------------------------------------
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ������ ����� ���������� ������
inline void KONTROL_captures_B(const struct T_Bitboard * p_bitboard,// ������������� �����
							   const unsigned __int8 * p_from,// ������ ����� ������
							   const unsigned __int8 * p_to,// ���� ����� ������
							   const unsigned __int8 * p_p,// ����� ���� � ������
							   unsigned __int8 name_figure// ��� ������� ������
							   );

//---------------------------------------------------------------------------
// ������������ ����� �� ������� �����, �� ������� ������ 
// ��� ���������� ����� 
inline void BITBOARDGENERATOR::KONTROL_move(const unsigned __int8 * p_from,// ������ ����� ������
						                    const unsigned __int8 * p_to,// ���� ����� ������
						                    const unsigned __int8 * p_p// ����� ���� � ������
						                    );

//---------------------------------------------------------------------------
// ���� ������ ������������� ���(�.�. ����� ������)
__inline __int32 first_one(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
					   );

//---------------------------------------------------------------------------
// ���� ��������� ������������� ���(�.�. ����� �����)
__inline __int32 last_one(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
					  );

};//class BITBOARDGENERATOR
