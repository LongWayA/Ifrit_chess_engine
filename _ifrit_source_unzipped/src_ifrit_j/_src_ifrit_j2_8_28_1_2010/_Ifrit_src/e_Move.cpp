#include <iostream> // TEST
using namespace std;

#include "e_Move.h"
#include "h_Estimation.h"

//-----------------------------------------------------------------------------------
/*++
 * ������ ������� �� ���������� �����
 *
 * ����� ChessTerminator75
 * ��������� ��������� 23.02.2009
 *
 *  ������������
 * ���������������� ������ ���
 * ��� ��� ����� �������� ����� ��������� � ������ d_Loop
 *
*/
//---------------------------------------------------------------------------

extern class EVAL eval_o;// ����������� ������ �������(����� �� �� �������������)

/*
 ��� ������
 0 - ��� ������
 1 - ����� 
 2 - ���� 
 3 - ����
 4 - ����� 
 5 - ����� 
 6 - ������
*/

//	===================================================================
// ��������� ���� ������ ��� �����
void MOVES::Onward_moves_white(const unsigned __int8 & i,//����� ���� �� ������
                               struct T_undo undo[],//���������� ��� ������ ����
					           const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
						       struct T_Bitboard * const p_bitboard// ������������� �������
						       ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);// �������� ������� ������
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);// ��������� ������� ������
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);// ������ ������

// ���������� ���� ������ �� �������
undo->return_taking_passage = p_bitboard->taking_passage_yes;

// ������ ����������(����� ��� ���-�������)
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;

// ��������� ������ �� ������� 
p_bitboard->taking_passage_yes = 0;

// ����� ������ ������ ������ ��� ������
p_bitboard->colour_move = 0;

       // ������ ���������� ����� ��������� (����� ��� ���-�������)
      undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;

/*
 ��� ����
 0 - ��� ����
 1 � ������� ���
 2 � ������
 3 � ������� ���������
 4 � �������� ���������
 5 � ������ �� �������

 12 - ����������� � ���� 
 13 - ����������� � ����
 14 - ����������� � ����� 
 15 - ����������� � ����� 

 22 - ������ � ������������ � ���� 
 23 - ������ � ������������ � ����
 24 - ������ � ������������ � ����� 
 25 - ������ � ������������ � ����� 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  // ��� �� ������ ����----------------------------------------------------------------------
  case 1 :

    //������� ������ �� ������ �����---------------------------------------------- 
    //������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

    //������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

    //��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------------------------------------
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //---------------------------------------------------------------------------------
    // ����� �������� ������ �������� �� ��������� � ������ �� �������
    // �� ��� ������� ����� ������� ������ � ���� ��� ����� �� ��������� �� �����
    // �� �������� ������ �� ������� ���� ��� ������ �� �������� ��� ���������
    // � ���� ����� �� �������� ��������� � ��� 
    //---------------------------------------------------------------------------------
    // ����� - ���� ������ �� �������
    if(p_bitboard->white_name_from_coord[f_p] == 1){
	  if(f_p - i_p == 16){
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;
	      p_bitboard->taking_passage_yes = 1;
          p_bitboard->taking_passage_x =(i_p + 8) % 8;
          p_bitboard->taking_passage_y =(i_p + 8) / 8;
	  }//if(f_p - i_p == 16){

    // ������ - ���� ���������
    } else if(p_bitboard->white_name_from_coord[f_p] == 6){
        // ���������� ����� ���������
      undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      p_bitboard->castling_K = 0;
	  p_bitboard->castling_Q = 0;	   

    // ����� - ���� ���������  
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){
	  if(i_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(i_p == 0){
	  if(i_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(i_p == 7){
    }//if(p_bitboard->white_name_from_coord[f_p] == 1){
    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ������----------------------------------------------------------------------------------
  case 2 :

    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������ �� ����� ���� ������ �����
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ����� ���� ������� ������ ������ �� �������� ������ �����
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //---------------------------------------------------------------------------------
    // ������ ������
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //---------------------------------------------------------------------------------

    // �������� ������ ������
    p_bitboard->black_name_from_coord[f_p] = 0;
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    // ������ ������� ������ � ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������� ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������ - ���� ���������
    if(p_bitboard->white_name_from_coord[f_p] == 6){
      // ���������� ����� ���������
	  undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      p_bitboard->castling_K = 0;
	  p_bitboard->castling_Q = 0;		
  
    // ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){	
	  if(i_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q =0;
	  }//if(i_p == 0){
	  if(i_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K =0;
	  }//if(i_p == 7){
    }//if(p_bitboard->black_name_from_coord[f_p] == 4){	

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
  break;

  // ������ �� �������-----------------------------------------------------------------------
  case 5 :

    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;

    // ������� ������ ������ �� ����� ���� ������ �����
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
    // ����� ���� ������� ������ ������ �� �������� ������ �����
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

    //------------------------------------------------------------------------------
    // ������ ������
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // �������� ������ ������
    p_bitboard->black_name_from_coord[(f_p - 8)] = 0;

    //������ ������ �� ����� �����------------------
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ��������� 0-0-0-------------------------------------------------------------------------
  case 3 :

    //������� ������ �� ������ �����----------------- 
    // ��� �������
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;
    // ��� ������
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[0];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] ^ move_masks_m[0];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[3] = p_bitboard->white_name_from_coord[0];
	p_bitboard->white_name_from_coord[0] = 0;

    //������ ������ �� ����� �����------------------
    // ��� �������
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // ��� ������
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[3];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] ^ move_masks_m[3];

    // ������ - ���� ���������
    // ���������� ����� ���������
    undo->return_castling_Q = p_bitboard->castling_Q;
    undo->return_castling_K = p_bitboard->castling_K;
	p_bitboard->castling_Q = 0;
    p_bitboard->castling_K = 0;

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ��������� 0-0--------------------------------------------------------------------------
  case 4 :

    //������� ������ �� ������ �����----------------- 
    // ��� �������
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;
    // ��� ������
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[7];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] ^ move_masks_m[7];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[5] = p_bitboard->white_name_from_coord[7];
	p_bitboard->white_name_from_coord[7] = 0;

    //������ ������ �� ����� �����------------------
    // ��� �������
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // ��� ������
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[5];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] ^ move_masks_m[5];

    // ������ - ���� ���������
    // ���������� ����� ���������
    undo->return_castling_Q = p_bitboard->castling_Q;
    undo->return_castling_K = p_bitboard->castling_K;
	p_bitboard->castling_Q = 0;
    p_bitboard->castling_K = 0;

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ����������� � �����--------------------------------------------------------------------
  case 15 :

    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 5;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval + eval_o.QUEEN - eval_o.PAWN;
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

// ����������� � �����----------------------------------------------------------------------
  case 14 :

    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 4;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.ROOK - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ����������� � �����--------------------------------------------------------------------
  case 13 :

    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 3;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� +����� � -������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.BISHOP - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ����������� � ����---------------------------------------------------------------------
  case 12 :

    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 2;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.KNIGHT - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
    // ������ ������ � ����� ���� ����� �����
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
  break;

  // ������ � ������������ � �����----------------------------------------------------------
  case 25 :

    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������ �� ����� ���� ������ �����
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ����� ���� ������� ������ ������ �� �������� ������ �����
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ������ � ������������ ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
         p_bitboard->eval = p_bitboard->eval + eval_o.QUEEN - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // �������� ������ ������
    p_bitboard->black_name_from_coord[f_p] = 0;
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 5;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    // ������ ������� ������ � ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������� ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

// ������ � ������������ � �����------------------------------------------------------------
  case 24 :

    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������ �� ����� ���� ������ �����
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ����� ���� ������� ������ ������ �� �������� ������ �����
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ������ � ������������ ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
         p_bitboard->eval = p_bitboard->eval + eval_o.ROOK - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // �������� ������ ������
    p_bitboard->black_name_from_coord[f_p] = 0;
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 4;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    // ������ ������� ������ � ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������� ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ������ � ������������ �����--------------------------------------------------------------
  case 23 :

    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������ �� ����� ���� ������ �����
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ����� ���� ������� ������ ������ �� �������� ������ �����
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ������ � ������������ ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.BISHOP - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // �������� ������ ������
    p_bitboard->black_name_from_coord[f_p] = 0;
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 3;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    // ������ ������� ������ � ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������� ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

// ������ � ������������  � ����------------------------------------------------------------
  case 22 :

    // ������� ������ �� ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������ �� ����� ���� ������ �����
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ����� ���� ������� ������ ������ �� �������� ������ �����
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ������ � ������������ ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.KNIGHT - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // �������� ������ ������
    p_bitboard->black_name_from_coord[f_p] = 0;
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[f_p] = 2;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    // ������ ������� ������ � ����� ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������ ������� ������ � ������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  default:;//��� �� ���������
   
}// switch (name){

}//void MOVES::Onward_moves_white
//	*******************************************************************

//	===================================================================
// ��������� ���� ������ ��� ������
void MOVES::Onward_moves_black(const unsigned __int8 & i,//����� ���� �� ������
                               struct T_undo undo[],//���������� ��� ������ ����
					           const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
						       struct T_Bitboard * const p_bitboard// ������������� �������
						       ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


// ���������� ���� ������ �� �������
undo->return_taking_passage= p_bitboard->taking_passage_yes;

// ������ ����������(����� ��� ���-�������)
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;


// ��������� ������ �� ������� 
p_bitboard->taking_passage_yes = 0;

// ������ ������ ������ ������ ��� �����
p_bitboard->colour_move = 1;

       // ������ ���������� ����� ��������� (����� ��� ���-�������)
      undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;


/*
 ��� ����
 0 - ��� ����
 1 � ������� ���
 2 � ������
 3 � ������� ���������
 4 � �������� ���������
 5 � ������ �� �������

 12 - ����������� � ���� 
 13 - ����������� � ����
 14 - ����������� � ����� 
 15 - ����������� � ����� 

 22 - ������ � ������������ � ���� 
 23 - ������ � ������������ � ����
 24 - ������ � ������������ � ����� 
 25 - ������ � ������������ � ����� 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  //1 ��� �� ������ ����------------------------------------------------------------------
  case 1 :
    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ����� - ���� ������ �� ������� 
    if(p_bitboard->black_name_from_coord[f_p] ==1){
	  if(f_p - i_p == -16){
          //cout << "i_p= "<< i_p<<"\n";
          //cout << "f_p= "<< f_p<<"\n";
          //cout << "p_bitboard->black_name_from_coord[f_p]= "<< p_bitboard->black_name_from_coord[f_p]<<"\n";
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;
		  p_bitboard->taking_passage_yes =1;
          p_bitboard->taking_passage_x =(i_p - 8) % 8;
          p_bitboard->taking_passage_y =(i_p - 8) / 8;
	  }//if(f_p - i_p == -16){

    // ������ - ���� ���������
    } else if(p_bitboard->black_name_from_coord[f_p] == 6){
	  undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;
      p_bitboard->castling_k = 0;
	  p_bitboard->castling_q = 0;

    // ����� - ���� ���������  
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(i_p == 56){
	  if(i_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(i_p == 63){
     
    }//if(p_bitboard->black_name_from_coord[f_p] ==1){

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //2 ������----------------------------------------------------------------------------------
  case 2 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ������ �������
    // �� ������ �� �������� �������� ������ ������
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
      p_bitboard->eval = p_bitboard->eval - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������ - ���� ���������
    if(p_bitboard->black_name_from_coord[f_p] == 6){
	  undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;
      p_bitboard->castling_k = 0;
	  p_bitboard->castling_q = 0;

// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(i_p == 56){
	  if(i_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(i_p == 63){
     
    }//if(p_bitboard->black_name_from_coord[f_p] == 6){
    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // 3 ������ �� �������----------------------------------------------------------------------
  case 5 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];
  
    //------------------------------------------------------------------------------
    // ������ �������
    // �� ������ �� �������� �������� ������ ������
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------
	
	p_bitboard->white_name_from_coord[(f_p + 8)] = 0;


    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //��������� 0-0-0
  case 3 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[56];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] ^ move_masks_m[56];
	p_bitboard->black_name_from_coord[59] = p_bitboard->black_name_from_coord[56];
	p_bitboard->black_name_from_coord[56] = 0;

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[59];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] ^ move_masks_m[59];


    // ������ - ���� ���������
    //-------------------------------------------------------  
    undo->return_castling_q = p_bitboard->castling_q;
    undo->return_castling_k = p_bitboard->castling_k;
	p_bitboard->castling_q = 0;
    p_bitboard->castling_k = 0;
    //-------------------------------------------------------  

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //��������� 0-0
  case 4 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[63];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] ^ move_masks_m[63];
	p_bitboard->black_name_from_coord[61] = p_bitboard->black_name_from_coord[63];
	p_bitboard->black_name_from_coord[63] = 0;

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[61];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] ^ move_masks_m[61];


    // ������ - ���� ���������
    //-------------------------------------------------------  
    undo->return_castling_q = p_bitboard->castling_q;
    undo->return_castling_k = p_bitboard->castling_k;
	p_bitboard->castling_q = 0;
    p_bitboard->castling_k = 0;
    //-------------------------------------------------------  

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //����������� � �����-----------------------------------------------------------------------
  case 15 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 5;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.QUEEN + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //����������� � �����-----------------------------------------------------------------------
  case 14 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 4;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.ROOK + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //����������� � �����-----------------------------------------------------------------------
  case 13 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 3;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.BISHOP + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //����������� � ����-----------------------------------------------------------------------
  case 12 :

    //������� ������ �� ������ �����----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 2;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.KNIGHT + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ������������ ����� ���� �����
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ������ � ������������ � �����--------------------------------------------------------------
  case 25 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.QUEEN + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------


    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = 5;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f ==4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ������ � ������������ � �����--------------------------------------------------------------
  case 24 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.ROOK + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = 4;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f == 4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ������ � ������������ �����----------------------------------------------------------------
  case 23 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.BISHOP + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = 3;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f ==4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ������ � ������������  � ����--------------------------------------------------------------
  case 22 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ����������� �������
    // �� ������ �� �������� �������� ������ ����������� � ���������� �����
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.KNIGHT + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;
	p_bitboard->black_name_from_coord[f_p] = 2;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //������ ������ �� ����� �����------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if( t_f ==4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;


  default:;//��� �� ���������
   
}// switch (name){

}// void MOVES::Onward_moves_black
//	*******************************************************************




//	===================================================================
// ��� ����� ��� �����
void MOVES::Backs_moves_white(const unsigned __int8 & i,//����� ���� �� ������
					          const struct T_undo undo[],//���������� ��� ������ ����
					          const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
                              struct T_Bitboard * const p_bitboard// ������������� �������
					          ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


// ��������������� ���� ������ �� �������
p_bitboard->taking_passage_yes=undo->return_taking_passage;

// ����� ������� ���� ��� ������ ����� �� ��� ����
p_bitboard->colour_move = 1;

/*
 ��� ����
 0 - ��� ����
 1 � ������� ���
 2 � ������
 3 � ������� ���������
 4 � �������� ���������
 5 � ������ �� �������

 12 - ����������� � ���� 
 13 - ����������� � ����
 14 - ����������� � ����� 
 15 - ����������� � ����� 

 22 - ������ � ������������ � ���� 
 23 - ������ � ������������ � ����
 24 - ������ � ������������ � ����� 
 25 - ������ � ������������ � ����� 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  //1 ��� �� ������ ����-----------------------------------------------------------------------
  case 1 :
 
    // ����� - ���� ������ �� ������� 
    if(p_bitboard->white_name_from_coord[f_p] == 1){
	  if(f_p - i_p == 16){
		  p_bitboard->taking_passage_yes =undo->return_taking_passage;
          p_bitboard->taking_passage_x = undo->taking_passage_x;
          p_bitboard->taking_passage_y = undo->taking_passage_y;
	  }//if(f_p - i_p == 16){

    // ������ - ���� ���������
    } else if(p_bitboard->white_name_from_coord[f_p] == 6){
          // ���������� ����� ���������
		  p_bitboard->castling_K = undo->return_castling_K;
          p_bitboard->castling_Q = undo->return_castling_Q;   

    // ����� - ���� ���������  
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){
  
	  if(i_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(i_p == 0){
	  if(i_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	  }//if(i_p == 7){
    }//if(p_bitboard->black_name_from_coord[f_p] ==1){

    //������� ������ �� ������ �����----------------- 
    // ��� ����� ������
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    //������ ������ �� ����� �����------------------
    // ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //2 ������----------------------------------------------------------------------------------
  case 2 :

    // ������ - ���� ���������
    if(p_bitboard->white_name_from_coord[f_p] == 6){
      // ���������� ����� ���������
	  p_bitboard->castling_K = undo->return_castling_K;
      p_bitboard->castling_Q = undo->return_castling_Q;   

    // ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){	
	  if(i_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(i_p == 0){
	  if(i_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	  }//if(i_p == 7){
    }//if(p_bitboard->white_name_from_coord[f_p] == 6){

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(f_p == 56){
	  if(f_p == 63){
          p_bitboard->castling_k = undo->return_castling_k;
	  }//if(f_p == 63){
    }//t_f == 4){	

    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ������ ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // �������������� ��� ������ ������
	p_bitboard->black_name_from_coord[f_p] = t_f;

    //------------------------------------------------------------------------------
    // ������ ������
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
	// (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
    // � ����� ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // � ����� ��� ������ ������
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ������� ������ ������ ������
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //3 ������ �� �������-----------------------------------------------------------------------
  case 5 :

    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ������ ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // �������������� ��� ������ ������
	p_bitboard->black_name_from_coord[(f_p - 8)] = t_f;

    //------------------------------------------------------------------------------
    // ������ ������
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    // (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
    // � ����� ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // � ����� ��� ������ ������
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ������� ������ ������ ������
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
  break;

  // 0-0-0     -----------------------------------------------------------------------
  case 3 :

    // ������ - ���� ���������
    //-------------------------------------------------------  
    p_bitboard->castling_Q = undo->return_castling_Q;
    p_bitboard->castling_K = undo->return_castling_K;   
    //-------------------------------------------------------  

    //������� ������ �� ������ �����----------------- 
    // ������
    // ��� ����� ������
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // �����
    // ��� ����� ������
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[3];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] ^ move_masks_m[3];
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[0] = p_bitboard->white_name_from_coord[3];
	p_bitboard->white_name_from_coord[3] = 0;

    //������ ������ �� ����� �����------------------
    // ������
    // ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

    // �����
    // ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[0];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] ^ move_masks_m[0];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // 0-0     -----------------------------------------------------------------------
  case 4 :

    // ������ - ���� ���������
    //------------------------------------------------------- 
    p_bitboard->castling_Q = undo->return_castling_Q;
    p_bitboard->castling_K = undo->return_castling_K;   
    //-------------------------------------------------------  

    //������� ������ �� ������ �����----------------- 
    // ������
    // ��� ����� ������
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // �����
    // ��� ����� ������
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[5];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] ^ move_masks_m[5];
    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[7] = p_bitboard->white_name_from_coord[5];
	p_bitboard->white_name_from_coord[5] = 0;

    //������ ������ �� ����� �����------------------
    // ������
    // ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

    // �����
    // ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[7];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] ^ move_masks_m[7];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // ������ �����-----------------------------------------------------------------------
  case 15 :
  case 14 :
  case 13 :
  case 12 :

    //������� ������ �� ������ �����----------------- 
    // ��� ����� ������
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    // (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.kod[p_bitboard->white_name_from_coord[f_p]] + eval_o.PAWN;
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[i_p] = 1;
	p_bitboard->white_name_from_coord[f_p] = 0;

    //������ ������ �� ����� �����------------------
    // ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

//2 ������ � ������������---------------------------------------------------------------------
  case 25 :
  case 24 :
  case 23 :
  case 22 :

    //  ���������� ���� ������� ����� 
    if(t_f == 4){
	  if(f_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(f_p == 56){
	  if(f_p == 63){
          p_bitboard->castling_k = undo->return_castling_k;
	  }//if(f_p == 63){
    }//if(p_bitboard->black_name_from_coord[f_p] == 4){	

    //-------------------------------------------------------  
    //������� ������ �� ������ �����----------------- 
    // ������� ������ �� ������ ���� ����� �����
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // ������� ������ �� �������� ������ �����
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    // (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.kod[p_bitboard->white_name_from_coord[f_p]] + eval_o.PAWN - eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // ��������� ��� ������ � �������� �� ������ ����������
	p_bitboard->white_name_from_coord[i_p] = 1;
	p_bitboard->white_name_from_coord[f_p] = 0;

    // ��������������� ��� ������ ������
	p_bitboard->black_name_from_coord[f_p] = t_f;

    //������ ������ �� ����� �����------------------
    // � ����� ��� ����� ������
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // � ����� ��� ������ ������
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    // ������� ������ ������
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // ������� ������ ������ ������
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  default:;//��� �� ���������
   
}// switch (name){

}//void MOVES::Backs_moves_white
//	*******************************************************************


//	===================================================================
// ��� ����� ��� ������
void MOVES::Backs_moves_black(const unsigned __int8 & i,//����� ���� �� ������
					          const struct T_undo undo[],//���������� ��� ������ ����
					          const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
                              struct T_Bitboard * const p_bitboard// ������������� �������
					          ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


// �������������� ���� ������ �� �������
p_bitboard->taking_passage_yes=undo->return_taking_passage;

// ������ ������� ���� ��� ������ ����� �� ��� ����
p_bitboard->colour_move = 0;

/*
 ��� ����
 0 - ��� ����
 1 � ������� ���
 2 � ������
 3 � ������� ���������
 4 � �������� ���������
 5 � ������ �� �������

 12 - ����������� � ���� 
 13 - ����������� � ����
 14 - ����������� � ����� 
 15 - ����������� � ����� 

 22 - ������ � ������������ � ���� 
 23 - ������ � ������������ � ����
 24 - ������ � ������������ � ����� 
 25 - ������ � ������������ � ����� 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  //1 ��� �� ������ ����-----------------------------------------------------------------------
  case 1 :

    // ����� - ���� ������ �� ������� 
    if(p_bitboard->black_name_from_coord[f_p] ==1){
	  if(f_p - i_p == -16){
		  p_bitboard->taking_passage_yes =undo->return_taking_passage;
          p_bitboard->taking_passage_x = undo->taking_passage_x;
          p_bitboard->taking_passage_y = undo->taking_passage_y;
	  }//if(f_p - i_p == -16){

// ������ - ���� ���������
    } else if(p_bitboard->black_name_from_coord[f_p] == 6){
		  p_bitboard->castling_k = undo->return_castling_k;
          p_bitboard->castling_q = undo->return_castling_q;

    // ����� - ���� ���������  
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(i_p == 56){
	  if(i_p == 63){
          p_bitboard->castling_k =  undo->return_castling_k;
	  }//if(i_p == 63){
    }//if(p_bitboard->black_name_from_coord[f_p] ==1){


    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
    p_bitboard->black_name_from_coord[f_p] = 0;

//������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //2 ������----------------------------------------------------------------------------------
  case 2 :

    // ������ - ���� ���������
    if(p_bitboard->black_name_from_coord[f_p] == 6){
		  p_bitboard->castling_k = undo->return_castling_k;
          p_bitboard->castling_q = undo->return_castling_q;

    // ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(i_p == 56){
	  if(i_p == 63){
          p_bitboard->castling_k = undo->return_castling_k;
	  }//if(i_p == 63){
     
    }//if(p_bitboard->black_name_from_coord[f_p] == 6){

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(f_p == 0){
	  if(f_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	   }//if(f_p == 7){	
    }//if(t_f ==4 ){

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
	p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->white_name_from_coord[f_p] = t_f;

    //------------------------------------------------------------------------------
    // ������ ������
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    // (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //3 ������ �� �������-----------------------------------------------------------------------
  case 5 :
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
	p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->white_name_from_coord[(f_p + 8)] = t_f;

    //------------------------------------------------------------------------------
    // ������ ������
    // � ������ �� ���������� �������� ������ ������ 
    // ������ ��� � ������ ������ ������� + ����� � - ������
    // (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];

	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
  break;

  // 0-0-0 -----------------------------------------------------------------------
  case 3 :

    // ������ - ���� ���������
    //-------------------------------------------------------  
	p_bitboard->castling_q = undo->return_castling_q;
	p_bitboard->castling_k = undo->return_castling_k;
    //-------------------------------------------------------  

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
    p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[59];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] ^ move_masks_m[59];
    p_bitboard->black_name_from_coord[56] = p_bitboard->black_name_from_coord[59];
    p_bitboard->black_name_from_coord[59] = 0;

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[56];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] ^ move_masks_m[56];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // 0-0     -----------------------------------------------------------------------
  case 4 :
 
    // ������ - ���� ���������
    //-------------------------------------------------------  
    p_bitboard->castling_q = undo->return_castling_q;
    p_bitboard->castling_k = undo->return_castling_k;
    //-------------------------------------------------------  

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
    p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[61];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] ^ move_masks_m[61];
    p_bitboard->black_name_from_coord[63] = p_bitboard->black_name_from_coord[61];
    p_bitboard->black_name_from_coord[61] = 0;

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[63];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] ^ move_masks_m[63];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;


  case 15 :
  case 14 :
  case 13 :
  case 12 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
  
    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    // (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[p_bitboard->black_name_from_coord[f_p]] - eval_o.PAWN;
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------
	
	p_bitboard->black_name_from_coord[i_p] = 1;
    p_bitboard->black_name_from_coord[f_p] = 0;

    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

//2 ������ � ������������---------------------------------------------------------------------
  case 25 :
  case 24 :
  case 23 :
  case 22 :

    // ���� ������� ����� �� ��������� ��������� � �� ��������
    if(t_f == 4){
	  if(f_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(f_p == 0){
	  if(f_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	  }//if(f_p == 7){	
    }//if(t_f ==4){
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // ����������� ������
    // � ������ �� ���������� �������� ������ ����������� � �������� �����
    // ������ ��� � ������ ������ ������� + ����� � - ������
    // (�������� ���)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[p_bitboard->black_name_from_coord[f_p]] - eval_o.PAWN + eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------


	p_bitboard->black_name_from_coord[i_p] = 1;
	p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->white_name_from_coord[f_p] = t_f;
    //������ ������ �� ����� �����------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  default:;//��� �� ���������
   
}// switch (name){

}//void MOVES::Backs_moves_black
//	*******************************************************************

// � ������� ���� ����� ����� � ������� ������ �� �������� �������
const unsigned __int64 MOVES::move_masks_m[] = {
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
};//unsigned __int64 move_masks[]
//-----------------------------------------------------------------------------------


