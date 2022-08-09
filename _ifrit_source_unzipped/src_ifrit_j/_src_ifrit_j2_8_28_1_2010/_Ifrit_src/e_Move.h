#include "a_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������ ������� �� ���������� �����
 *
 * ����� ChessTerminator75
 * ��������� ��������� 23.02.2009
 * 
*/
//---------------------------------------------------------------------------

#define MATERIAL 1 // ��������-���������(1-0) ����� ������� ���������

/*******************************************
����� MOVES

 � ������ ������� ������ �� ���������� ����� �� ������ T_list_surplus_moves
 ������� ������ ��� ������������ �������� ������ BITBOARDGENERATOR
 � ����� ������ ������ ����������� ����� T_Bitboard

********************************************/
class MOVES{

// � ������� ���� ����� ����� � ������� ������ �� �������� �������
static const unsigned __int64 move_masks_m[];

//////////////////////////////////////////////////////////////////////////////////////////
public:

//---------------------------------------------------------------------------
// ��������� ���� ������ ��� �����
void Onward_moves_white(const unsigned __int8 & i,//����� ���� �� ������
                        struct T_undo undo[],//���������� ��� ������ ����
					    const struct T_list_surplus_moves * const p_list_surplus_moves,//������ ��������� �����
						struct T_Bitboard * const p_bitboard// ������������� �������
						);

//---------------------------------------------------------------------------
// ��������� ���� ������ ��� ������
void Onward_moves_black(const unsigned __int8 & i,//����� ���� �� ������
                        struct T_undo undo[],//���������� ��� ������ ����
					    const struct T_list_surplus_moves * const p_list_surplus_moves,//������ ��������� �����
						struct T_Bitboard * const p_bitboard// ������������� �������
						);

//---------------------------------------------------------------------------
// ��������� ���� ����� ��� �����
void Backs_moves_white(const unsigned __int8 & i,//����� ���� �� ������
					   const struct T_undo undo[],//���������� ��� ������ ����
					   const struct T_list_surplus_moves * const p_list_surplus_moves,//������ ��������� �����
                       struct T_Bitboard * const p_bitboard// ������������� �������
					   );

//---------------------------------------------------------------------------
// ��������� ���� ����� ��� ������
void Backs_moves_black(const unsigned __int8 & i,//����� ���� �� ������
					   const struct T_undo undo[],//���������� ��� ������ ����
					   const struct T_list_surplus_moves * const p_list_surplus_moves,//������ ��������� �����
                       struct T_Bitboard * const p_bitboard// ������������� �������
					   );


};//class MOVES
