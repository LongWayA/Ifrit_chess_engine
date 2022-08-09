#include "h_Estimation.h"
#include "k_Util.h"
//#include "d_Bitboard_gen.h"
//-----------------------------------------------------------------------------------
/* +
 * ������ �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 12.04.2008
 *  ������������
 * ���������������� ������ ���
 * ��� ��� ����� �������� ����� ��������� � ������ d_Loop
*/
//---------------------------------------------------------------------------

// ������ ������ ���������� � ������
int Pozicion[] ={
				 100,100,100,100,100,100,100,100,
 		         100,105,105,100,100,105,105,100,
 		         100,105,115,115,115,115,105,100,
 		         100,105,115,110,110,115,105,100,
 		         100,105,115,110,110,115,105,100,
 		         100,105,115,115,115,115,105,100,
 		         100,105,105,100,100,105,105,100,
 		         100,100,100,100,100,100,100,100,
 		         };
// �������� ����� � ����� �� ������������� ������ 
// ���� �� ��������� �������������� �������� ���������
int Pozicion_r[] ={
				 100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         };
 
//����� ������ �� �����������
int Pawn[] ={
	         0,0,0,0,0,0,0,0,
		     50,50,50,50,50,50,50,50,
		     40,40,40,40,40,40,40,40,
		     10,10,10,15,15,10,10,10,
		     5,5,30,50,50,5,5,5,
		     0,0,0,30,30,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0
		     };

 // ��������� ���� ������� 
 // ��������� �������� ���������
 int Castling[]={
	         0,0,30,0,0,0,50,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,30,0,0,0,50,0
		     };

//	===================================================================
// ���� ������ ������������� ���(�.�. ����� ������)
static __inline int first_one(unsigned __int64 analyzed_number){ 
   _asm { 
/*
������� mov �������� ������ ������� (��������) ������ (����������). 
��� ���� �������� �������� ������� �������� ��������. 
������ ������� �� ����������. � ����������� �� �������� ���������, 
������������ ����� ��� ����. ���� �������� ������� ��-������� ��� 
����� ��������� �� ��������� ���������� ���������� ������ ��������, 
��� ��������� ������� ������������ ������ � ������� ������� �������� ���� 
�� ���������� ���������� byte ptr ��� word ptr. 
������� �� ������������ �� ����� ����������. 
����� ��������� ������������� ��������� �������� �������� - 
� ���� ������ ����������, ��� � �����. ��� ����� ���� ������������ �������� word ptr:
*/
           mov  eax, dword ptr analyzed_number[0]
/*
������� test ��������� �������� ����������� ��������� 
� ��� ����� ���������� �, � ����������� �� ����������, 
������������� ����� SF, ZF � PF. ����� OF � CF ������������, 
a AF ����� �������������� ��������. ��������� ������ ����� ����� 
���������������� ��������� �������� ���������. ������� test �� �������� 
�� ���� �� ���������. � �������� ������� �������� ������� test ����� 
��������� ������� (����� �����������) ��� ������ ������, � �������� 
������� - ������� (����� �����������), ������ ������ ��� ���������������� 
��������, ������ �� ����������� ���������� ��� �������� ������������ ��� 
������ ������. �������� ����� ���� ������� ��� ������� � ������������ ����� 
�� ������ ��� ��� �����. 

������� ���������� ���������:

������ �������-��� 0101
������ �������-��� 0011
��� ���������� 0001

���� SF ��������������� � 1, ���� � ���������� ���������� ������� 
������������ ����� � ������������� �������� �����.
���� ZF ��������������� � 1, ���� � ���������� ���������� ������� 
������������ �����, ��������� �� ����� �������� �����.
���� PF ��������������� � 1, ���� � ���������� ���������� ������� 
������������ ����� � ������ ����������� �������� ������ � ��� �����.
*/
           test eax, eax
/*
������� �������, ���� ������� ��������
jz ���� ZF= 1
*/
           jz   grown_up
/*
������� bsf ��������� ����� ��� ������� ����� � ������� ����, ������� 1. 
������������ ����������� �� �������� ���� (0) � ��������. ���� � ����� 
�� ������� ������������� �����, �� ��������������� ���� ZF. 
���� ������������� ���� ����, �� ����� ������� �������������� ���� 
��������� � ��������� � ������� �������. ������� ���� ��������� ��� 
������� � �����, ������������� �� ���� 0. � �������� ������� �������� ������� bsf 
������� ��������� �������, ���� ����� ������� ��������� ������������, � 
�������� ������� - ������� ��� ������ ������ �� ����������� ������. 
� ������� bsf ��������� ������������� ��� 16-�������, ��� � 32-������� ��������� 
(�� � ������, � ������ �������� ������ ���� ������ ����).
*/
           bsf  eax, eax
/*
������� jmp �������� ���������� � ��������� ����� ���� �� ��� ������� ������������ 
��������. ����� �������� �� �����������. ������� �� ������������ �� ����� ����������.
*/
           jmp jump_end
           grown_up:    bsf  eax, dword ptr analyzed_number[4]
/*
��������
*/
           add  eax, 20h
           jump_end:
       }//_asm {
}//static __inline int first_one(unsigned __int64 analyzed_number){ 
//	*******************************************************************

//	===================================================================
// ���� ��������� ������������� ���(�.�. ����� �����)
static __inline int last_one(unsigned __int64 analyzed_number){ 
  _asm { 
           mov  eax, dword ptr analyzed_number[4]
           test eax, eax
           jz   younger
/*
������� bsr ��������� ���� ��� ������� ����� � ������� ����, ������� 1. 
������������ ����������� �� �������� ���� (15 ��� 31) � ��������. 
���� � ����� �� ������� ������������� �����, �� ��������������� ���� ZF. 
���� ������������� ���� ����, �� ����� ������� �������������� ���� ��������� 
� ��������� � ������� �������. ������� ���� ��������� ��� ������� � �����, 
������������� �� ���� 0. � �������� ������� �������� ������� bsr ������� 
��������� �������, ���� ����� ������� ��������� ������������, � �������� 
������� - ������� ��� ������ ������ �� ����������� ������. 
� ������� bsr ��������� ������������� ��� 16-�������, ��� � 32-������� ���������, 
�� � ������, � ������ �������� ������ ���� ������ ���� (�� ����������� ������, 
����� ������ ������� - ���������).
*/
           bsr  eax, eax
           add  eax, 20h
           jmp  jump_end
           younger:    bsr  eax, dword ptr analyzed_number[0]
           jump_end:
       }//_asm { 
}//static __inline int last_one(unsigned __int64 analyzed_number){ 
//	*******************************************************************


//	===================================================================
// ������� �������� ������� � ��������� �� ���������� ������� �������� � ������
int Eval_forced(const T_Bitboard * bitboard ){

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
static  int ocenka_f=0;
ocenka_f=0;

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������ ������
  from_mask = bitboard->white_piece[6];
  if(from_mask != 0) ocenka_f= ocenka_f + 900000;

// ���� �����-----------------------------------------------------------------
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 1000;
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 500;
  }//while(from_mask != 0){

// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 300;
 }//while(from_mask != 0){
// �����---------------------------------------------------------------------------------- 

// ���� ���� ----------------------------------------------------------------------------------
// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 300;
 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f=ocenka_f + 100;
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

///////////////////
///////////////////////////////////
/////////////////


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������
  from_mask = bitboard->black_piece[6];
  if(from_mask != 0) ocenka_f= ocenka_f - 900000;

// ���� �����-----------------------------------------------------------------
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 1000;
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 500;
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 300;
 }//while(from_mask != 0){
// �����---------------------------------------------------------------------------------- 

// ���� ���� ----------------------------------------------------------------------------------
// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 300;
 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f=ocenka_f - 100;
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------
   return ocenka_f;
}//int Eval_forced_816(T_board_list_816 * board_list_816){
//	*******************************************************************



//	===================================================================
// ������� �������� ������� � ��������� �� ���������� ������� ����������� �������
int Eval_State(const T_Bitboard * bitboard){

 // ����� ��� ����������� ��������� ����� 
        int Structure_pawn_w[]={
		     0,0,0,0,0,0,0,0
            };
        int Structure_pawn_b[]={
	         0,0,0,0,0,0,0,0
            };

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
static  int ocenka_p=0;
int from = 0; // ���������� ���� ������ ������ �����
ocenka_p=0;

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������ ������
  from_mask = bitboard->white_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
	  ocenka_p= ocenka_p + Castling[from] - King_shield(from,1,bitboard);
//	  ocenka_p= ocenka_p + Castling[from];
  }

// ���� �����-----------------------------------------------------------------
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pozicion_r[from];
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pozicion_r[from] + 2*Rook_amoun_white(bitboard);
//    ocenka_p= ocenka_p + Pozicion_r[from];
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pozicion[from] + 2*Bishop_amoun_white(bitboard);
//    ocenka_p= ocenka_p + Pozicion[from];
 }//while(from_mask != 0){
// �����---------------------------------------------------------------------------------- 

// ���� ���� ----------------------------------------------------------------------------------
// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pozicion[from];
 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pawn[63 - from];
// ���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_w[from % 8]=Structure_pawn_w[from % 8] + 1;
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

///////////////////
///////////////////////////////////
/////////////////


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������
  from_mask = bitboard->black_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
	  ocenka_p= ocenka_p - Castling[from] - King_shield(from,0,bitboard);
//	  ocenka_p= ocenka_p - Castling[from];
  }

// ���� �����-----------------------------------------------------------------
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pozicion_r[from];
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pozicion_r[from] - 2*Rook_amoun_black(bitboard);
//    ocenka_p= ocenka_p - Pozicion_r[from];
  }//while(from_mask != 0){

// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pozicion[from] - 2*Bishop_amoun_black(bitboard);
//    ocenka_p= ocenka_p - Pozicion[from];
 }//while(from_mask != 0){
// �����---------------------------------------------------------------------------------- 

// ���� ���� ----------------------------------------------------------------------------------
// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pozicion[from];
 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pawn[from];
// ���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_b[from % 8]=Structure_pawn_b[from % 8] + 1;
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------
// �������� �� ��������� �����
  for(int ii=0;ii<8;ii++){
    if(Structure_pawn_w[ii]>1)ocenka_p=ocenka_p-20*Structure_pawn_w[ii];
    if(Structure_pawn_b[ii]>1)ocenka_p=ocenka_p+20*Structure_pawn_b[ii];
  }//for(int ii=1;ii<9;ii++){

   return ocenka_p;
}//int Eval_State(T_Bitboard * bitboard){

//	*******************************************************************

//	===================================================================
// ����� �� ���������� ���� ����� �������
inline int King_shield(int n,int z,const T_Bitboard * bitboard){
 int st=0;
st=0;
	if(z==1){
// ���� �� ������ �����
       if(bitboard->white_name_from_coord[n + 7]==0) st=st+30;
// ���� �� ������ ����� �����
       if(bitboard->white_name_from_coord[n + 8]==0) st=st+30;
// ���� �� ������ ������ �����
       if(bitboard->white_name_from_coord[n + 9]==0) st=st+30;
	}else{
// ���� �� ������ ����
       if(bitboard->black_name_from_coord[n - 7]==0) st=st-30;
// ���� �� ������  ����� ����
       if(bitboard->black_name_from_coord[n - 8]==0) st=st-30;
// ���� �� ������ ������ ����
       if(bitboard->black_name_from_coord[n - 9]==0) st=st-30;
	}//if(z==1){

return st;

}//int King_shield(int n,int z,T_board_list_816 * board_list_816){
//	*******************************************************************

//	===================================================================
// ������������ ��� ��������� ���� ����� ���������� ���������� �����
inline int Rook_amoun_white(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
//unsigned __int64 captures_mask = 0; // ����� ������


int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int blocking_coordinate = 0; // ���������� �������������� ���� 
int number = 0; // ���������� ���� ������ ������ �����
// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & e_rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
 blocking_square_mask = (bitboard->all_piece) & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & e_rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------
  return number;
}//int Rook_amoun_white(T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// ������������ ��� ��������� ���� ����� ���������� ���������� �����
inline int Rook_amoun_black(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
//unsigned __int64 captures_mask = 0; // ����� ������


int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int blocking_coordinate = 0; // ���������� �������������� ���� 
int number = 0; // ���������� ���� ������ ������ �����
// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & e_rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
   blocking_square_mask = (bitboard->all_piece) & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & e_rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------
  return number;
}//int Rook_amoun_black(T_Bitboard * bitboard){
//	*******************************************************************


//	===================================================================
// ������������ ��� ��������� ���� ����� ���������� ���������� �����
inline int Bishop_amoun_white(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
//unsigned __int64 captures_mask = 0; // ����� ������


int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int blocking_coordinate = 0; // ���������� �������������� ���� 
int number = 0; // ���������� ���� ������ ������ �����
// ���� �����---------------------------------------------------------------------------------- 
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - ������
 blocking_square_mask = (bitboard->all_piece) & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
   blocking_square_mask = (bitboard->all_piece) & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
// �����---------------------------------------------------------------------------------- 
  return number;
}//int Bishop_amoun_white(T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// ������������ ��� ��������� ���� ����� ���������� ���������� �����
inline int Bishop_amoun_black(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
//unsigned __int64 captures_mask = 0; // ����� ������


int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int blocking_coordinate = 0; // ���������� �������������� ���� 
int number = 0; // ���������� ���� ������ ������ �����
// ���� �����---------------------------------------------------------------------------------- 
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ��� ����� - ������
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
 blocking_square_mask = (bitboard->all_piece) & e_bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
// �����---------------------------------------------------------------------------------- 
  return number;
}//int Bishop_amoun_black(T_Bitboard * bitboard){
//	*******************************************************************