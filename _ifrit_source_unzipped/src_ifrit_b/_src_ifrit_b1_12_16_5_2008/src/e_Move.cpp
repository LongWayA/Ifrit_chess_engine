#include <iostream.h> // TEST

#include "e_Move.h"
//-----------------------------------------------------------------------------------
/*++
 * ������ ������� �� ���������� �����
 *
 * ����� ChessTerminator75
 * ��������� ��������� 19.04.2008
 *
 *  ������������
 * ���������������� ������ ���
 * ��� ��� ����� �������� ����� ��������� � ������ d_Loop
 *
*/
//---------------------------------------------------------------------------

//	===================================================================
// ��������� ���� ������ ��� �����
// int i ����� ���� �� ������,T_list_surplus_moves ������ ��������� �����,T_board_list_816 �������,T_list_realizable_moves ������ �������� ����� 
void Onward_moves_white( int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);

// ���������� ���� ������ �� �������
list_surplus_moves->return_taking_passage[depth]= bitboard->taking_passage_yes;
// ��������� ������ �� ������� 
bitboard->taking_passage_yes =0;

// �������� �� ��� ����� 0, ������� 1, ������� �� ������� 2, ���������� 3,
// ����������� � ����� 4,����������� � ���� 5,����������� � ����� 6,����������� � ����� 7,
// ����������� � ������� � ����� 8,����������� � ������� � ���� 9,
// ����������� � ������� � ����� 10,����������� � ������� � ����� 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){
   case 1 :
//1 ��� �� ������ ����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

//--------------------------------------------------------------------------------
// ����� �������� ������ �������� �� ��������� � ������ �� �������
// �� ��� ������� ����� ������� ������ � ���� ��� ����� �� ��������� �� �����
// �� �������� ������ �� ������� ���� ��� ������ �� �������� ��� ���������
// � ���� ����� �� �������� ��������� � ���
// ����� - ���� ������ �� ������� 
//-------------------------------------------------------
  if(bitboard->white_name_from_coord[f_p] == 1){
	 if(f_p - i_p == 16){
		  bitboard->taking_passage_yes =1;
          bitboard->taking_passage_x =(i_p + 8) % 8;
          bitboard->taking_passage_y =(i_p + 8) / 8;
	 }//if(f_p - i_p == 16){
//-------------------------------------------------------
// ������ - ���� ���������
//-------------------------------------------------------  
  } else if(bitboard->white_name_from_coord[f_p] == 6){
// ���������� ����� ���������
		 list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         bitboard->castling_K =0;
	     bitboard->castling_Q =0;	   
//-------------------------------------------------------  
// ����� - ���� ���������  
 } else if(bitboard->white_name_from_coord[f_p] == 4){
  
	if(i_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(i_p == 0){
	if(i_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(i_p == 7){
 }//if(bitboard->black_name_from_coord[f_p] ==1){
// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 ������----------------------------------------------------------------------------------
{
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������ �� ����� ���� ������ �����
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ����� ���� ������� ������ ������ �� �������� ������ �����
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// �������� ������ ������
    bitboard->black_name_from_coord[f_p] = 0;
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������� ������ � ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������� ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������ - ���� ���������
//-------------------------------------------------------  
if(bitboard->white_name_from_coord[f_p] == 6){
// ���������� ����� ���������
		 list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         bitboard->castling_K =0;
	     bitboard->castling_Q =0;		
//-------------------------------------------------------  
// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
  } else if(bitboard->white_name_from_coord[f_p] == 4){	
	if(i_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(i_p == 0){
	if(i_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(i_p == 7){
   }//if(bitboard->black_name_from_coord[f_p] == 4){	

// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
// 3 ������ �� �������-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;

// ������� ������ ������ �� ����� ���� ������ �����
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
// ����� ���� ������� ������ ������ �� �������� ������ �����
    bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] = bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];
// �������� ������ ������
    bitboard->black_name_from_coord[(f_p - 8)] = 0;

//������ ������ �� ����� �����------------------
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
//��������� 0-0-0
{
//������� ������ �� ������ �����----------------- 
 // ��� �������
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;
// ��� ������
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[0];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[0]] = bitboard->white_piece[bitboard->white_name_from_coord[0]] ^ move_masks_m[0];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[3] = bitboard->white_name_from_coord[0];
	bitboard->white_name_from_coord[0] = 0;

//������ ������ �� ����� �����------------------
// ��� �������
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ��� ������
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[3];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[3]] = bitboard->white_piece[bitboard->white_name_from_coord[3]] ^ move_masks_m[3];

// ������ - ���� ���������
//-------------------------------------------------------  
// ���������� ����� ���������
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
	     bitboard->castling_Q =0;
         bitboard->castling_K =0;
//-------------------------------------------------------  
// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;

   case 4 :
//��������� 0-0
{
//������� ������ �� ������ �����----------------- 
 // ��� �������
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;
// ��� ������
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[7];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[7]] = bitboard->white_piece[bitboard->white_name_from_coord[7]] ^ move_masks_m[7];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[5] = bitboard->white_name_from_coord[7];
	bitboard->white_name_from_coord[7] = 0;

//������ ������ �� ����� �����------------------
// ��� �������
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ��� ������
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[5];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[5]] = bitboard->white_piece[bitboard->white_name_from_coord[5]] ^ move_masks_m[5];

// ������ - ���� ���������
//-------------------------------------------------------  
// ���������� ����� ���������
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
	     bitboard->castling_Q =0;
         bitboard->castling_K =0;
//-------------------------------------------------------  
// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 5;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 14 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 4;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 13 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 3;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 12 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 2;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������ � ����� ���� ����� �����
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
// ������ � ������������ � �����----------------------------------------------------------------------------------
{

// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������ �� ����� ���� ������ �����
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ����� ���� ������� ������ ������ �� �������� ������ �����
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// �������� ������ ������
    bitboard->black_name_from_coord[f_p] = 0;
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 5;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������� ������ � ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������� ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 24 :
// ������ � ������������ � �����----------------------------------------------------------------------------------
{

// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������ �� ����� ���� ������ �����
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ����� ���� ������� ������ ������ �� �������� ������ �����
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// �������� ������ ������
    bitboard->black_name_from_coord[f_p] = 0;
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 4;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������� ������ � ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������� ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 23 :
// ������ � ������������ �����----------------------------------------------------------------------------------
{
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������ �� ����� ���� ������ �����
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ����� ���� ������� ������ ������ �� �������� ������ �����
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// �������� ������ ������
    bitboard->black_name_from_coord[f_p] = 0;
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 3;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������� ������ � ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������� ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 22 :
// ������ � ������������  � ����----------------------------------------------------------------------------------
{
// ������� ������ �� ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������ �� ����� ���� ������ �����
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ����� ���� ������� ������ ������ �� �������� ������ �����
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// �������� ������ ������
    bitboard->black_name_from_coord[f_p] = 0;
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[f_p] = 2;
	bitboard->white_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
// ������ ������� ������ � ����� ���� ����� �����
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������ ������� ������ � ������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


   default:;//��� �� ���������
   
 }// switch (name){
}//void Onward_moves_816( int i,T_list_surplus_moves * list_surplus_moves,T_board_list_816 * board_list_816){ 
//	*******************************************************************

//	===================================================================
// ��������� ���� ������ ��� ������
// int i ����� ���� �� ������,T_list_surplus_moves ������ ��������� �����,T_board_list_816 �������,T_list_realizable_moves ������ �������� ����� 
void Onward_moves_black( int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);


// ���������� ���� ������ �� �������
list_surplus_moves->return_taking_passage[depth]= bitboard->taking_passage_yes;
// ��������� ������ �� ������� 
bitboard->taking_passage_yes =0;

// �������� �� ��� ����� 0, ������� 1, ������� �� ������� 2, ���������� 3,
// ����������� � ����� 4,����������� � ���� 5,����������� � ����� 6,����������� � ����� 7,
// ����������� � ������� � ����� 8,����������� � ������� � ���� 9,
// ����������� � ������� � ����� 10,����������� � ������� � ����� 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){
   case 1 :
//1 ��� �� ������ ����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ����� - ���� ������ �� ������� 
//-------------------------------------------------------
  if(bitboard->black_name_from_coord[f_p] ==1){
	 if(f_p - i_p == -16){
		  bitboard->taking_passage_yes =1;
          bitboard->taking_passage_x =(i_p - 8) % 8;
          bitboard->taking_passage_y =(i_p - 8) / 8;
	 }//if(f_p - i_p == -16){
//-------------------------------------------------------
// ������ - ���� ���������
//-------------------------------------------------------  
  } else if(bitboard->black_name_from_coord[f_p] == 6){
		 list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         bitboard->castling_k =0;
	     bitboard->castling_q =0;
//-------------------------------------------------------  
// ����� - ���� ���������  
 } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(i_p == 56){
	if(i_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(i_p == 63){
     
 }//if(bitboard->black_name_from_coord[f_p] ==1){

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 ������----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������ - ���� ���������
//-------------------------------------------------------  
  if(bitboard->black_name_from_coord[f_p] == 6){
		 list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         bitboard->castling_k =0;
	     bitboard->castling_q =0;
//-------------------------------------------------------  
// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
   } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(i_p == 56){
	if(i_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(i_p == 63){
     
 }//if(bitboard->black_name_from_coord[f_p] == 4){	
// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f == 4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
// 3 ������ �� �������-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];
    bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] = bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];
    bitboard->white_name_from_coord[(f_p + 8)] = 0;


//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
//��������� 0-0-0
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[56];
    bitboard->black_piece[bitboard->black_name_from_coord[56]] = bitboard->black_piece[bitboard->black_name_from_coord[56]] ^ move_masks_m[56];
	bitboard->black_name_from_coord[59] = bitboard->black_name_from_coord[56];
	bitboard->black_name_from_coord[56] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[59];
    bitboard->black_piece[bitboard->black_name_from_coord[59]] = bitboard->black_piece[bitboard->black_name_from_coord[59]] ^ move_masks_m[59];


// ������ - ���� ���������
//-------------------------------------------------------  
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
	     bitboard->castling_q =0;
         bitboard->castling_k =0;
//-------------------------------------------------------  

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;

   case 4 :
//��������� 0-0
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[63];
    bitboard->black_piece[bitboard->black_name_from_coord[63]] = bitboard->black_piece[bitboard->black_name_from_coord[63]] ^ move_masks_m[63];
	bitboard->black_name_from_coord[61] = bitboard->black_name_from_coord[63];
	bitboard->black_name_from_coord[63] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[61];
    bitboard->black_piece[bitboard->black_name_from_coord[61]] = bitboard->black_piece[bitboard->black_name_from_coord[61]] ^ move_masks_m[61];


// ������ - ���� ���������
//-------------------------------------------------------  
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
	     bitboard->castling_q =0;
         bitboard->castling_k =0;
//-------------------------------------------------------  

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 5;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 14 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 4;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 13 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 3;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 12 :
//����������� � �����-----------------------------------------------------------------------
{
//������� ������ �� ������ �����----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 2;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ������������ ����� ���� �����
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
// ������ � ������������ � �����----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 5;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f ==4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 24 :
// ������ � ������������ � �����----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 4;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f == 4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 23 :
// ������ � ������������ �����----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 3;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f ==4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 22 :
// ������ � ������������  � ����----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 2;
	bitboard->black_name_from_coord[i_p] = 0;

//������ ������ �� ����� �����------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if( t_f ==4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


   default:;//��� �� ���������
   
 }// switch (name){
}//void Onward_moves_816( int i,T_list_surplus_moves * list_surplus_moves,T_board_list_816 * board_list_816){ 
//	*******************************************************************




//	===================================================================
// ��� ����� ��� �����
void Backs_moves_white( int i,const T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);


// �������������� ���� ������ �� �������
bitboard->taking_passage_yes=list_surplus_moves->return_taking_passage[depth];


// �������� �� ��� ����� 0, ������� 1, ������� �� ������� 2, ���������� 3,
// ����������� � ����� 4,����������� � ���� 5,����������� � ����� 6,����������� � ����� 7,
// ����������� � ������� � ����� 8,����������� � ������� � ���� 9,
// ����������� � ������� � ����� 10,����������� � ������� � ����� 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){

   case 1 :
{
  //1 ��� �� ������ ����-----------------------------------------------------------------------

// ����� - ���� ������ �� ������� 
//-------------------------------------------------------
  if(bitboard->white_name_from_coord[f_p] == 1){
	 if(f_p - i_p == 16){
		  bitboard->taking_passage_yes =0;
          bitboard->taking_passage_x =  0;
          bitboard->taking_passage_y =  0;
	 }//if(f_p - i_p == 16){
//-------------------------------------------------------
// ������ - ���� ���������
//-------------------------------------------------------  
  } else if(bitboard->white_name_from_coord[f_p] == 6){
// ���������� ����� ���������
		 bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
         bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];   
//-------------------------------------------------------  
// ����� - ���� ���������  
 } else if(bitboard->white_name_from_coord[f_p] == 4){
  
	if(i_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(i_p == 0){
	if(i_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(i_p == 7){
 }//if(bitboard->black_name_from_coord[f_p] ==1){

//������� ������ �� ������ �����----------------- 
// ��� ����� ������
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

//������ ������ �� ����� �����------------------
// ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 ������----------------------------------------------------------------------------------
{
//
// ������ - ���� ���������
//-------------------------------------------------------  
   if(bitboard->white_name_from_coord[f_p] == 6){
// ���������� ����� ���������
		 bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
         bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];   

// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
   } else if(bitboard->white_name_from_coord[f_p] == 4){	
	if(i_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(i_p == 0){
	if(i_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(i_p == 7){
   }//if(bitboard->black_name_from_coord[f_p] == 4){	

// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(f_p == 56){
	if(f_p == 63){
        bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
	}//if(f_p == 63){
  }//t_f == 4){	
//-------------------------------------------------------  


//������� ������ �� ������ �����----------------- 
// ������� ������ �� ������ ���� ����� �����
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// �������������� ��� ������ ������
	bitboard->black_name_from_coord[f_p] = t_f;

//������ ������ �� ����� �����------------------
// � ����� ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// � ����� ��� ������ ������
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ������� ������ ������ ������
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
//3 ������ �� �������-----------------------------------------------------------------------
{

//������� ������ �� ������ �����----------------- 
// ������� ������ �� ������ ���� ����� �����
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// �������������� ��� ������ ������
	bitboard->black_name_from_coord[(f_p - 8)] = t_f;

//������ ������ �� ����� �����------------------
// � ����� ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// � ����� ��� ������ ������
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ������� ������ ������ ������
    bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] = bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
   
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
 // 0-0-0     -----------------------------------------------------------------------
{

// ������ - ���� ���������
//-------------------------------------------------------  
         bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
         bitboard->castling_K = list_surplus_moves->return_castling_K[depth];   
//-------------------------------------------------------  

//������� ������ �� ������ �����----------------- 
// ������
// ��� ����� ������
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// �����
// ��� ����� ������
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[3];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[3]] = bitboard->white_piece[bitboard->white_name_from_coord[3]] ^ move_masks_m[3];
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[0] = bitboard->white_name_from_coord[3];
	bitboard->white_name_from_coord[3] = 0;

//������ ������ �� ����� �����------------------
// ������
// ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

// �����
// ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[0];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[0]] = bitboard->white_piece[bitboard->white_name_from_coord[0]] ^ move_masks_m[0];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   case 4 :
 // 0-0     -----------------------------------------------------------------------
{
 //-----------------------------------------------------------------------

// ������ - ���� ���������
//------------------------------------------------------- 
          bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
          bitboard->castling_K = list_surplus_moves->return_castling_K[depth];   
//-------------------------------------------------------  

//������� ������ �� ������ �����----------------- 
// ������
// ��� ����� ������
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// �����
// ��� ����� ������
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[5];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[5]] = bitboard->white_piece[bitboard->white_name_from_coord[5]] ^ move_masks_m[5];
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[7] = bitboard->white_name_from_coord[5];
	bitboard->white_name_from_coord[5] = 0;

//������ ������ �� ����� �����------------------
// ������
// ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

// �����
// ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[7];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[7]] = bitboard->white_piece[bitboard->white_name_from_coord[7]] ^ move_masks_m[7];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
   case 14 :
   case 13 :
   case 12 :
{
 // ������ �����-----------------------------------------------------------------------
  //-----------------------------------------------------------------------

//������� ������ �� ������ �����----------------- 
// ��� ����� ������
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[i_p] = 1;
	bitboard->white_name_from_coord[f_p] = 0;

//������ ������ �� ����� �����------------------
// ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
   case 24 :
   case 23 :
   case 22 :
//2 ������ � ������������---------------------------------------------------------------------------------
{

// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(f_p == 56){
	if(f_p == 63){
        bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

//-------------------------------------------------------  
//������� ������ �� ������ �����----------------- 
// ������� ������ �� ������ ���� ����� �����
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// ������� ������ �� �������� ������ �����
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// ��������� ��� ������ � �������� �� ������ ����������
	bitboard->white_name_from_coord[i_p] = 1;
	bitboard->white_name_from_coord[f_p] = 0;

// �������������� ��� ������ ������
	bitboard->black_name_from_coord[f_p] = t_f;

//������ ������ �� ����� �����------------------
// � ����� ��� ����� ������
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// � ����� ��� ������ ������
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
// ������� ������ ������
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// ������� ������ ������ ������
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   default:;//��� �� ���������
   
 }// switch (name){

}
//	*******************************************************************


//	===================================================================
// ��� ����� ��� ������
void Backs_moves_black( int i,const T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);


// �������������� ���� ������ �� �������
bitboard->taking_passage_yes=list_surplus_moves->return_taking_passage[depth];


// �������� �� ��� ����� 0, ������� 1, ������� �� ������� 2, ���������� 3,
// ����������� � ����� 4,����������� � ���� 5,����������� � ����� 6,����������� � ����� 7,
// ����������� � ������� � ����� 8,����������� � ������� � ���� 9,
// ����������� � ������� � ����� 10,����������� � ������� � ����� 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){

   case 1 :
{
 //1 ��� �� ������ ����-----------------------------------------------------------------------

// ����� - ���� ������ �� ������� 
//-------------------------------------------------------
  if(bitboard->black_name_from_coord[f_p] ==1){
	 if(f_p - i_p == -16){
		  bitboard->taking_passage_yes = 0;
          bitboard->taking_passage_x =   0;
          bitboard->taking_passage_y =   0;
	 }//if(f_p - i_p == -16){
//-------------------------------------------------------
// ������ - ���� ���������
//-------------------------------------------------------  
  } else if(bitboard->black_name_from_coord[f_p] == 6){
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
         bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
//-------------------------------------------------------  
// ����� - ���� ���������  
 } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(i_p == 56){
	if(i_p == 63){
        bitboard->castling_k =  list_surplus_moves->return_castling_k[depth];
	}//if(i_p == 63){
 }//if(bitboard->black_name_from_coord[f_p] ==1){


    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
    bitboard->black_name_from_coord[f_p] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 ������----------------------------------------------------------------------------------
{
// ������ - ���� ���������
//-------------------------------------------------------  
   if(bitboard->black_name_from_coord[f_p] == 6){
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
         bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
//-------------------------------------------------------  
// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� ��������
    } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(i_p == 56){
	if(i_p == 63){
        bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
	}//if(i_p == 63){
     
 }//if(bitboard->black_name_from_coord[f_p] == 4){	
// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(f_p == 0){
	if(f_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(f_p == 7){	
  }//if(t_f ==4 ){
//-------------------------------------------------------  

   bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
   bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
	bitboard->black_name_from_coord[f_p] = 0;

    bitboard->white_name_from_coord[f_p] = t_f;
//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
//3 ������ �� �������-----------------------------------------------------------------------
{
   bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
   bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
	bitboard->black_name_from_coord[f_p] = 0;

    bitboard->white_name_from_coord[(f_p + 8)] = t_f;
//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] = bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
   
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
 // 0-0-0     -----------------------------------------------------------------------
{
// ������ - ���� ���������
//-------------------------------------------------------  
		 bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
//-------------------------------------------------------  

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
    bitboard->black_name_from_coord[f_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[59];
    bitboard->black_piece[bitboard->black_name_from_coord[59]] = bitboard->black_piece[bitboard->black_name_from_coord[59]] ^ move_masks_m[59];
    bitboard->black_name_from_coord[56] = bitboard->black_name_from_coord[59];
    bitboard->black_name_from_coord[59] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[56];
    bitboard->black_piece[bitboard->black_name_from_coord[56]] = bitboard->black_piece[bitboard->black_name_from_coord[56]] ^ move_masks_m[56];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   case 4 :
 // 0-0     -----------------------------------------------------------------------
{
// ������ - ���� ���������
//-------------------------------------------------------  
		 bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
//-------------------------------------------------------  

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
    bitboard->black_name_from_coord[f_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[61];
    bitboard->black_piece[bitboard->black_name_from_coord[61]] = bitboard->black_piece[bitboard->black_name_from_coord[61]] ^ move_masks_m[61];
    bitboard->black_name_from_coord[63] = bitboard->black_name_from_coord[61];
    bitboard->black_name_from_coord[61] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[63];
    bitboard->black_piece[bitboard->black_name_from_coord[63]] = bitboard->black_piece[bitboard->black_name_from_coord[63]] ^ move_masks_m[63];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
   case 14 :
   case 13 :
   case 12 :
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = 1;
    bitboard->black_name_from_coord[f_p] = 0;

//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
   case 24 :
   case 23 :
   case 22 :
//2 ������ � ������������---------------------------------------------------------------------------------
{
// ���� ������� ����� �� ��������� ��������� � �� ��������
 if(t_f == 4){
	if(f_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(f_p == 0){
	if(f_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(f_p == 7){	
  }//if(t_f ==4){
   bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
   bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->black_name_from_coord[i_p] = 1;
	bitboard->black_name_from_coord[f_p] = 0;

    bitboard->white_name_from_coord[f_p] = t_f;
//������ ������ �� ����� �����------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   default:;//��� �� ���������
   
 }// switch (name){

}
//	*******************************************************************
