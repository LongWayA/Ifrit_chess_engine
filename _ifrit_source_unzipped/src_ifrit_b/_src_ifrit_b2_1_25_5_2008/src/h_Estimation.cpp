#include <fstream.h>  // TEST
#include "h_Estimation.h"
#include "k_Util.h"
//#include "d_Bitboard_gen.h"
//-----------------------------------------------------------------------------------
/* +
 * ������ �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 26.04.2008
 *  ������������
 * ���������������� ������ ���
 * ��� ��� ����� �������� ����� ��������� � ������ d_Loop
*/
//---------------------------------------------------------------------------
//
// ���� ������ ���������� � ������
// 100 ��� ����� �� ������������� ����� �������� ��� �� ���� �� ������� �� 3 �����
static int Pozicion_n[] ={
				 90,90 ,90 ,90 ,90 ,90 ,90 ,90,
 		         90,100,100,100,100,100,100,90,
 		         90,100,115,115,115,115,100,90,
 		         90,100,115,110,110,115,100,90,
 		         90,100,115,110,110,115,100,90,
 		         90,100,115,115,115,115,100,90,
 		         90,100,100,100,100,100,100,90,
 		         90,90 ,90 ,90 ,90 ,90 ,90 ,90,
 		         };
//
// ����� ������ ���������� �������������� ������� ���������
// 100 ��� ����� �� ������������� ������ �������� ��� �� ���� �� ������� �� 3 �����
static int Pozicion_b[] ={
				 100,100,90 ,90 ,90 ,90 ,100,100,
 		         100,115,110,100,100,110,115,100,
 		         90 ,110,115,110,110,115,110,90,
 		         90 ,100,110,115,115,110,100,90,
 		         90 ,100,110,115,115,110,100,90,
 		         90 ,110,115,110,110,115,110,90,
 		         100,115,110,100,100,110,115,100,
 		         100,100 ,90 ,90 ,90 ,90 ,100,100,
 		         };


// 100 ��� ����� �� ������������� ������ � �����
int Pozicion_q[] ={
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
	         0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
		     50,50,50,50,50,50,50,50,
		     10,10,10,30,30,10,10,10,
		     5 ,5 ,5 ,30,30 ,5 ,5 ,5 ,
		     0 ,0 ,10,30,30,0 ,0 ,0 ,
		     0 ,0 ,0 ,5 ,5 ,0 ,0 ,0 ,
		     0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
		     0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
		     };

 // ��������� ���� ������� 
 int Castling[]={
	         0,0,50,0,0,0,50,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,50,0,0,0,50,0
		     };
// ���� ������ ���������� � ������
// 100 ��� ����� �� ������������� ����� �������� ��� �� ���� �� ������� �� 3 �����
static int Pozicion_k_e[] ={
				 100,100,100,100,100,100,100,100,
 		         100,105,105,105,105,105,105,100,
 		         100,105,110,110,110,110,105,100,
 		         100,105,110,115,115,110,105,100,
 		         100,105,110,115,115,110,105,100,
 		         100,105,110,110,110,110,105,100,
 		         100,105,105,105,105,105,105,100,
 		         100,100,100,100,100,100,100,100,
 		         };

//����� ������ �� �����������
int Pawn_e[] ={
	         0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		     100,100,100,100,100,100,100,100,
		     50 ,50 ,50 ,50 ,50 ,50 ,50 ,50 ,
		     20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,
		     10 ,10 ,10 ,10 ,10 ,10 ,10 ,10 ,
		     5  ,5  ,5  ,5  ,5  ,5  ,5  ,5  ,
		     0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		     0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
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
  if(from_mask != 0) ocenka_f= ocenka_f + 90000;

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
  if(from_mask != 0) ocenka_f= ocenka_f - 90000;

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

//const  int kod[]={0,100,300,300,500,1000,90000};
//
//int ocenka_p=0;
//
//for(int i=0;i<64;i++) {
//	ocenka_p = ocenka_p + kod[bitboard->white_name_from_coord[i]];
//    ocenka_p = ocenka_p - kod[bitboard->black_name_from_coord[i]];
//}//for(int i=0 ;i<64;i++) {
//
//if (ocenka_p != ocenka_f) cout << "KARAUUUUUUUUUUUUUUL"<< "\n";


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
static  int from = 0; // ���������� ���� ������ ������ �����
static  int from_white_king = 0;
static  int from_black_king = 0;
static  int dx;
static  int dy;
static  int distanse;
static  int stadia_w=0;
static  int stadia_b=0;

const  int kd = 7;// ������. ���������
const  int rubikon=1500;//����� �������� � ��������
// ������ ������������� ���������
const  int kod[]={0,0,300,300,500,1000,0};

stadia_w=0;
stadia_b=0;
ocenka_p=0;

for(int i=0;i<64;i++) {
	stadia_w = stadia_w + kod[bitboard->white_name_from_coord[i]];
    stadia_b = stadia_b + kod[bitboard->black_name_from_coord[i]];
}//for(int i=0 ;i<64;i++) {

/*
 ��� ������
 0 - ��� ������ 1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 6 - ������

� ��������� ������� �� ����� 600(2*300) + 620(2*310) + 1000(2*500) + 1000 = 3220

*/



//cout << "������ " << "\n";
//cout << "bitboard->white_piece[6]= "<< first_one(bitboard->white_piece[6]) << "\n";
//cout << "bitboard->black_piece[6]= "<< first_one(bitboard->black_piece[6]) << "\n";
//cout << "����� " << "\n";
//cout << "�����  32 8" << "\n";
//cout << "bitboard->white_piece[4]= 32  "<< first_one(32) << "\n";
//cout << "bitboard->white_piece[4]= 8   "<< first_one(8) << "\n";
//
//cout << "bitboard->white_piece[4]= "<< first_one(bitboard->white_piece[4]) << "\n";
//
//cout << "������  32 8" << "\n";
//cout << "bitboard->black_piece[4]= 2305843009213693952  "<< first_one(2305843009213693952) << "\n";
//cout << "bitboard->black_piece[4]= 576460752303423488   "<< first_one(576460752303423488) << "\n";
//
//cout << "bitboard->black_piece[4]= "<< first_one(bitboard->black_piece[4]) << "\n";

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������ ������
  from_mask = bitboard->white_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
      from_white_king = from;
//	  ocenka_p= ocenka_p + Castling[from]- King_shield(from,1,bitboard);
  if(stadia_b > rubikon){
// ������ �� ���� ��������� 0-0
	  if (from==6){
// ����� �� ���� ���������
         if ((bitboard->white_piece[4]& 32) == 32){
	        ocenka_p= ocenka_p + 50 ;//- King_shield(from,1,bitboard);
         }else{
            ocenka_p= ocenka_p ;//- King_shield(from,1,bitboard);
		 }//if (bitboard->white_piece[4]==32){
// ������ �� ���� ��������� 0-0-0
	  }else if (from==2){//if (from==6){
// ����� �� ���� ���������
         if ((bitboard->white_piece[4]& 8) == 8){
	        ocenka_p= ocenka_p + 50 ;//- King_shield(from,1,bitboard);
         }else{
            ocenka_p= ocenka_p ;//- King_shield(from,1,bitboard);
		 }//if (bitboard->white_piece[6]==8){
       }else{
         ocenka_p= ocenka_p ;//- King_shield(from,1,bitboard);
	  }//}else if (from==2){//if (from==6){
  }else{
        ocenka_p= ocenka_p + Pozicion_k_e[from];
  }// if(stadia_b > rubikon){


  }//if(from_mask != 0) {



// ����� ������������ ������� ������
  from_mask = bitboard->black_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
      from_black_king = from;
//	  ocenka_p= ocenka_p - Castling[from] + King_shield(from,0,bitboard);
//	  ocenka_p= ocenka_p - Castling[from] + King_shield(from,0,bitboard);
  //	  ocenka_p= ocenka_p + Castling[from];
 if(stadia_w > rubikon){
// ������ �� ���� ��������� 0-0
	  if (from==62){
// ����� �� ���� ���������
         if ((bitboard->black_piece[4]& 2305843009213693952) == 2305843009213693952){
	        ocenka_p= ocenka_p - 50 + King_shield(from,0,bitboard);
         }else{
            ocenka_p= ocenka_p + King_shield(from,0,bitboard);
		 }//if (bitboard->white_piece[4]==32){
// ������ �� ���� ��������� 0-0-0
	  }else if (from==58){//if (from==6){
// ����� �� ���� ���������
         if ((bitboard->black_piece[4] & 576460752303423488)==576460752303423488){
	        ocenka_p= ocenka_p - 50 + King_shield(from,0,bitboard);
         }else{
            ocenka_p= ocenka_p + King_shield(from,0,bitboard);
		 }//if (bitboard->white_piece[6]==8){
       }else{
         ocenka_p= ocenka_p + King_shield(from,0,bitboard);
	  }//}else if (from==2){//if (from==6){
}else{
        ocenka_p= ocenka_p - Pozicion_k_e[from];
 }// if(stadia_b > rubikon){

  }//if(from_mask != 0) {

// ���� �����-----------------------------------------------------------------
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);
dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
	ocenka_p= ocenka_p + Pozicion_q[from] + distanse;
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p + Pozicion_q[from];
dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p + Pozicion_q[from] + Rook_amoun_white(bitboard) + distanse;
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p + Pozicion_b[from];
    ocenka_p= ocenka_p + Pozicion_b[from] + Bishop_amoun_white(bitboard);
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
dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p + Pozicion_n[from] + distanse;
 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
// ocenka_p= ocenka_p + Pawn[63 - from];
 if(stadia_b > rubikon){
    ocenka_p= ocenka_p + Pawn[63 - from];
 }else{
    ocenka_p= ocenka_p + Pawn_e[63 - from];;
  }// if(stadia_b > rubikon){
// ���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_w[from % 8]=Structure_pawn_w[from % 8] + 1;
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

///////////////////
///////////////////////////////////
/////////////////


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���� �����-----------------------------------------------------------------
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);
dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p - Pozicion_q[from] - distanse;
  }//while(from_mask != 0){
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p - Pozicion_q[from];
dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p - Pozicion_q[from] - Rook_amoun_black(bitboard) - distanse;
  }//while(from_mask != 0){

// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p - Pozicion_b[from];
   ocenka_p= ocenka_p - Pozicion_b[from] - Bishop_amoun_black(bitboard);
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
dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p - Pozicion_n[from] - distanse;
 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//  ocenka_p= ocenka_p - Pawn[from];
  if(stadia_w > rubikon){
    ocenka_p= ocenka_p - Pawn[from];
  }else{
    ocenka_p= ocenka_p - Pawn_e[from];
  }//if(stadia_w > rubikon){

// ���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_b[from % 8]=Structure_pawn_b[from % 8] + 1;
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------
// �������� �� ��������� �����
  for(int ii=0;ii<8;ii++){
    if(Structure_pawn_w[ii]>1)ocenka_p=ocenka_p - 20;
    if(Structure_pawn_b[ii]>1)ocenka_p=ocenka_p + 20;
  }//for(int ii=1;ii<9;ii++){

   return ocenka_p;
}//int Eval_State(T_Bitboard * bitboard){

//	*******************************************************************


//	===================================================================
// ����� �� ���������� ���� ����� �������
inline int King_shield(int n,int z,const T_Bitboard * bitboard){
static int st=0;
st=0;
	if(z==1){
// ���� �� ������ �����
       if(bitboard->white_name_from_coord[n + 7]==0) st=st+20;
// ���� �� ������ ����� �����
       if(bitboard->white_name_from_coord[n + 8]==0) st=st+20;
// ���� �� ������ ������ �����
       if(bitboard->white_name_from_coord[n + 9]==0) st=st+20;
	}else{
// ���� �� ������ ����
       if(bitboard->black_name_from_coord[n - 7]==0) st=st+20;
// ���� �� ������  ����� ����
       if(bitboard->black_name_from_coord[n - 8]==0) st=st+20;
// ���� �� ������ ������ ����
       if(bitboard->black_name_from_coord[n - 9]==0) st=st+20;
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