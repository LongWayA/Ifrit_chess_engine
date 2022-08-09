#include "a_Structure.h"
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
 BITBOARDGENERATOR

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
namespace BITBOARDGENERATOR{

//////////////////////////////////////////////////////////////////////////////////////////
//public:

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
//private:

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
inline void List_capture_white(unsigned __int8 name,// ��� ������� ������
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
inline void List_capture_black(unsigned __int8 name,// ��� ������� ������
							   unsigned __int8 type_move,// ��� ����
							   const unsigned __int8 & from,// ������ �����
							   const unsigned __int8 & to,// ���� �����
							   unsigned __int8 & count,// ����� ���� � ������
							   const struct T_Bitboard * const p_bitboard,// ������������� �����
                               struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						       );

//---------------------------------------------------------------------------
// ��������� ������ �����
inline void List_record_move(unsigned __int8 name,// ��� ������� ������
							 unsigned __int8 type_move,// ��� ����
							 const unsigned __int8 & from,// ������ �����
							 const unsigned __int8 & to,// ���� �����
							 unsigned __int8 & count,// ����� ���� � ������
							 const struct T_Bitboard * const p_bitboard,// ������������� �����
                             struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
	                         );

//---------------------------------------------------------------------------
// ��������� ������ ����� ��� ������ ������� ���� ��������� ������ �� ������� � ����������� �����
// sp - special
inline void List_record_sp(unsigned __int8 name,// ��� ������� ������
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
inline void KONTROL_move(const unsigned __int8 * p_from,// ������ ����� ������
						 const unsigned __int8 * p_to,// ���� ����� ������
						 const unsigned __int8 * p_p// ����� ���� � ������
						 );

//---------------------------------------------------------------------------
// ���� ������ ������������� ���(�.�. ����� ������)
__inline __int32 Bit_scan_forward_win32(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
					                    );

//---------------------------------------------------------------------------
// ���� ��������� ������������� ���(�.�. ����� �����)
__inline __int32 Bit_scan_reverse_win32(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
					                   );

};//namespace BITBOARDGENERATOR
