#include <iostream> // TEST
#include <fstream>  // TEST
#include <string>


#include "h_Evaluate.h"


//---------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 24.02.2009
*/
//---------------------------------------------------------------------------


/*
��������� �������

������� 
�������� (���� ����� ������� � ���� ������ ��������)
����������� ������ PST
����������� �����
����� �� ����������� ������
���������


 ���� ������� ��� ��� ����� ������ �� ����� �� �������
 ��� �� ��������� ������������ ������������ ����������� �������������� �����

 � ������������� ����� � ��� �������� ���
  A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
  A7(48) B7(49) C7(50) D7(51) E7(52) F7(53) G7(54) H7(55)
  A6(40) B6(41) C6(42) D6(43) E6(44) F6(45) G6(46) H6(47)
  A5(32) B5(33) C5(34) D5(35) E5(36) F5(37) G5(38) H5(39)
  A4(24) B4(25) C4(26) D4(27) E4(28) F4(29) G4(30) H4(31)
  A3(16) B3(17) C3(18) D3(19) E3(20) F3(21) G3(22) H3(23)
  A2(8)  B2(9)  C2(10) D2(11) E2(12) F2(13) G2(14) H2(15)
  A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

  � ������ �������� ����� ������� � ������ ����

  ������� ������� ���������� ������������ �������������� ����� � ����� � ������ ������
  ��������� ���� test[i]
*/


//	===================================================================
// ������� �������� ������� � ��������� �� ���������� ������� ����������� �������
__int32 EVALUATE::Eval_State(const T_Bitboard * p_bitboard// ������������� �����
			         ){
__int32 eval = 0;//����������� ������
unsigned __int8 from_white_king = 0;// ������������ ������ ������
unsigned __int8 from_black_king = 0;// ������������ ������� ������
unsigned __int8 at_white_king = 0; // ���������� ����������� ����� ������ ������ ������
unsigned __int8 at_black_king = 0;// ���������� ����������� ����� ������ ������� ������

// ����� ��� ���������� ������ ����
unsigned __int16 stadia_w = 0; // �������� ��� ����� � �����
unsigned __int16 stadia_b = 0; // �������� ��� ����� � ������

//� ��������� ������� �� ����� 400 + 400 + 600 + 1200 = 2600 (��� ����� �������)
//                             2*400 + 2*400 + 2*600 + 1200 
//                             800   + 800   + 1200  + 1200 = 4000 (����� ��� ���� ������) 
for(unsigned __int8 i = 0; i < 64; i++) {
	stadia_w = stadia_w + rubikon_kod[p_bitboard->white_name_from_coord[i]];
    stadia_b = stadia_b + rubikon_kod[p_bitboard->black_name_from_coord[i]];
}//for(int i=0 ;i<64;i++) {

//stadia_w = 2600; // �������� ��� ����� � �����
//stadia_b = 2600; // �������� ��� ����� � ������

// ����������� ������
EVALUATE_private::King_pst( p_bitboard,// ������������� �����
          from_white_king,// ������������ ������ ������
          from_black_king,// ������������ ������� ������
          stadia_w, // �������� ��� ����� � �����
          stadia_b, // �������� ��� ����� � ������
          eval //����������� ������
         );

// ����������� �����
EVALUATE_private::Pawn_pst( p_bitboard,// ������������� �����
          from_white_king,// ������������ ������ ������
          from_black_king,// ������������ ������� ������
          stadia_w, // �������� ��� ����� � �����
          stadia_b, // �������� ��� ����� � ������
          eval //����������� ������
         );

// ����������� �����, �����, ����, ����
// ����� ������������ 
EVALUATE_private::PST( p_bitboard,// ������������� �����
     from_white_king,// ������������ ������ ������
     from_black_king,// ������������ ������� ������
     at_white_king, // ���������� ����������� ����� ������ ������ ������
     at_black_king, // ���������� ����������� ����� ������ ������� ������
     eval //����������� ������
    );


return eval;

}//int EVALUATE::Eval_State
//	*******************************************************************

//	===================================================================
//
 inline void EVALUATE_private::King_pst(const T_Bitboard * p_bitboard,// ������������� �����
                           unsigned __int8 & from_white_king,// ������������ ������ ������
                           unsigned __int8 & from_black_king,// ������������ ������� ������
                           unsigned __int16 stadia_w, // �������� ��� ����� � �����
                           unsigned __int16 stadia_b, // �������� ��� ����� � ������
                           __int32 & eval //����������� ������
                           ){

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

unsigned __int8 from; // ���������� ���� ������ ������ �����

/*
����� ����������� ��������� � ��������� ������ �������� 
�� ���� ���� ���������� �� ������������ ������

������� ��� ������� �������� � ��������.
���� ��� ������ ������ - �� �������� � �����. �� �������� ��������� ������ �� ���������
�������� � ��������� � ����� �������� ��� ���.

��������� ��������(�� ����������� �������) ��� ��� ������ ����� �������. �� ������� � ���� ���� �����

*/
// ����� ������
from_mask = p_bitboard->white_piece[6];

if(from_mask != 0) {
   from = Bit_scan_forward_win32(from_mask);
   from_white_king = from;
//cout << "������ " << "\n";
//cout << "from= "<< from << "\n";
//cout << "Pozicion_k_e[from]= "<< Pozicion_k_e[from] << "\n";

   if(stadia_b > rubikon){

//-----------------------------------------------------------
 // �������� ����� � ��� �������� ���
 // .........
 // A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
// � ��������: 64 - 6 bit  128 - 7 bit 64 + 128 = 192 // �������� 6 � 7 ���
//	  if ( 
//          ( (p_bitboard->white_piece[4] & 192) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 6) || (from == 5) ) //  ����� ������ � ���� ��������� 0-0
//		  ) eval = eval - BONUS_CASTLING_O_O; // ����� �������� 
//
//// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
//// � ��������: 1 - 0 bit   2 - 1 bit
//// 1 + 2 = 3 // �������� 0,1 ����
//	  if ( 
//          ( (p_bitboard->white_piece[4] & 3) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 1) || (from == 2) || (from == 3) ) //  ����� ������ � ���� ��������� 0-0-0
//		  ) eval = eval - BONUS_CASTLING_O_O_O; // ����� �������� 
//-----------------------------------------------------------

         eval = eval + Pozicion_k[from] - King_shield(from,1,p_bitboard,BONUS_SHIELD);

     }else{// if(stadia_b > rubikon){

         // ����� �������� ������ ������ ���������� � ������
         eval = eval + Pozicion_k_e[from];

     }// if(stadia_b > rubikon){
}//if(from_mask != 0) {


//������ ������
from_mask = p_bitboard->black_piece[6];

//from_mask = 0;
if(from_mask != 0) {
   from = Bit_scan_forward_win32(from_mask);
   from_black_king = from;

   if(stadia_w > rubikon){

//-----------------------------------------------------------
 // �������� ����� � ��� �������� ���
// A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
 // .........

// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
// � ��������: 4611686018427387904 - 62 bit  9223372036854775808 - 63 bit 
// 4611686018427387904 + 9223372036854775808 = 13835058055282163712  �������� 62 � 63 ���
//	  if ( 
//          ( (p_bitboard->black_piece[4] & 13835058055282163712) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 61) || (from == 62) ) //  ������ ������ � ���� ��������� 0-0
//		  ) eval = eval + BONUS_CASTLING_O_O; // ����� �������� 
//
//// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
//// � ��������: 72057594037927936 - 56 bit   144115188075855872 - 57 bit
//// 72057594037927936 + 144115188075855872 = 216172782113783808  �������� 56,57 ����
//	  if ( 
//          ( (p_bitboard->black_piece[4] & 216172782113783808) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 57) || (from == 58) || (from == 59) ) //  ������ ������ � ���� ��������� 0-0-0
//		  ) eval = eval + BONUS_CASTLING_O_O_O; // ����� �������� 
//-----------------------------------------------------------

         eval = eval - Pozicion_k[from] + King_shield(from,0,p_bitboard,BONUS_SHIELD);

    }else{

       // ����� �������� ������ ������ ���������� � ������
         eval = eval - Pozicion_k_e[from];

    }// if(stadia_w > rubikon){

}//if(from_mask != 0) {

}//inline void EVALUATE::King
//	*******************************************************************

//	===================================================================
// ������ ������ �� ������������ 
 inline void EVALUATE_private::Pawn_pst(const T_Bitboard * p_bitboard,// ������������� �����
                           unsigned __int8 & from_white_king,// ������������ ������ ������
                           unsigned __int8 & from_black_king,// ������������ ������� ������
                           unsigned __int16 stadia_w, // �������� ��� ����� � �����
                           unsigned __int16 stadia_b, // �������� ��� ����� � ������
                           __int32 & eval //����������� ������
                           ){


 // ����� ��� ����������� ��������� ����� 
  unsigned __int8 Structure_pawn_w[]={
		     0,0,0,0,0,0,0,0
            };
  unsigned __int8 Structure_pawn_b[]={
	         0,0,0,0,0,0,0,0
            };

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

unsigned __int8 from; // ���������� ���� ������ ������ �����

bool end; // �������� ����� �������� ��� ���������

// ���������� �������� ����� ����� ������ ������� ������ ���� ���� ������
// �.�. �� ���� ����� ����� ��� - ����� � ������ �� �����
unsigned __int8 cnt_line_pawn_white = 0;
unsigned __int8 cnt_line_pawn_black = 0;

/*

������� ��� ������� �������� � ��������.
���� ��� ������ ������ - �� �������� � �����.

�� ��������� ����� �����

*/

// �����

//�����----------------------------------------------------------------------------------
end = 0;
from_mask = p_bitboard->white_piece[1];
while(from_mask != 0){

   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask-1);

   if(stadia_b > rubikon){
      eval = eval + Pawn[from];
      //eval = eval + test[from];
//cout << "from= "<< from << "\n";
//cout << "test[from]= "<< test[from] << "\n";
   }else{
      end = 1;
      eval = eval + Pawn_e[from];

//cout << "from= "<< from << "\n";
//cout << "from % 8= "<< from % 8 << "\n";

   }// if(stadia_b > rubikon){

	// ���������� ���������
    // � ������ ���� ����� ������ ��� ��������� �����
	if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){ 

		eval = eval + 2 * PERSPECTIVE_PAWNN;// 10
        cnt_line_pawn_white = 0;

		// ��� ������ ������� ���������
        if( ((from % 8) == 7) || ((from % 8) == 0) ) cnt_line_pawn_white = 1;

        // � ������ ���� ������ ����� ������ ��� ��������� �����
		if((from % 8) != 7) if(!(e_rook_up_masks[from + 1] & p_bitboard->black_piece[1])){ 
			eval = eval + PERSPECTIVE_PAWNN;
			cnt_line_pawn_white = cnt_line_pawn_white + 1;
		}//if((from % 8) != 7) if(!(e_rook_up_masks[from + 1] & p_bitboard->black_piece[1])){

        // � ������ ���� ����� ����� ������ ��� ��������� �����
		if((from % 8) != 0) if(!(e_rook_up_masks[from - 1] & p_bitboard->black_piece[1])){
			eval = eval + PERSPECTIVE_PAWNN;
            cnt_line_pawn_white = cnt_line_pawn_white + 1;
		}//if((from % 8) != 0) if(!(e_rook_up_masks[from - 1] & p_bitboard->black_piece[1])){

        // ��� ��� ����� ������� ��� ������ ���������.
        if(cnt_line_pawn_white == 2) eval = eval + end * Perspective_pawn_e[from]; 
		                                        // + 26 * PERSPECTIVE_PAWNN;

	}//if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){ 


 //���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_w[from % 8] = Structure_pawn_w[from % 8] + 1;

}//while(from_mask != 0){


// ������

//�����----------------------------------------------------------------------------------
end = 0;
from_mask = p_bitboard->black_piece[1];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask-1);

   if(stadia_w > rubikon){
     eval = eval - Pawn[63 - from];//
     //eval = eval - test[from];//
   }else{
     end = 1;
     eval = eval - Pawn_e[63 - from];

   }//if(stadia_w > rubikon){

	// ���������� ���������
    // � ������ ���� ����� ������ ��� ��������� �����
	if(!(e_rook_down_masks[from] & p_bitboard->white_piece[1])){ 

		eval = eval - 2 * PERSPECTIVE_PAWNN;
        cnt_line_pawn_black = 0;

		// ��� ������ ������� ���������
        if( ((from % 8) == 7) || ((from % 8) == 0) ) cnt_line_pawn_black = 1;

        // � ������ ���� ������ ����� ������ ��� ��������� �����
		if((from % 8) != 7) if(!(e_rook_down_masks[from + 1] & p_bitboard->white_piece[1])){
			eval = eval - PERSPECTIVE_PAWNN;
            cnt_line_pawn_black = cnt_line_pawn_black + 1;
		}//if((from % 8) != 7) if(!(e_rook_down_masks[from + 1] & p_bitboard->white_piece[1])){

        // � ������ ���� ����� ����� ������ ��� ��������� �����
		if((from % 8) != 0) if(!(e_rook_down_masks[from - 1] & p_bitboard->white_piece[1])){
			eval = eval - PERSPECTIVE_PAWNN;
            cnt_line_pawn_black = cnt_line_pawn_black + 1;
		}//if((from % 8) != 0) if(!(e_rook_down_masks[from - 1] & p_bitboard->white_piece[1])){

        // ��� ��� ����� ������� ��� ������ ���������.
        if(cnt_line_pawn_black == 2) eval = eval - end * Perspective_pawn_e[63 - from]; 
		                                         //- 26 * PERSPECTIVE_PAWNN;

	}//if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){


// ���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_b[from % 8] = Structure_pawn_b[from % 8] + 1;
}//while(from_mask != 0){

// �������� �� ��������� �����
  for(int ii = 0; ii < 8; ii++){
    if(Structure_pawn_w[ii] > 1) eval = eval - BONUS_PAWNN; // ������ ����� 20
    if(Structure_pawn_b[ii] > 1) eval = eval + BONUS_PAWNN;
    //cout << "ii = "<< ii << "\n";
    //cout << "Structure_pawn_w[ii]= "<< Structure_pawn_w[ii] << "\n";
  }//for(int ii = 0; ii < 8; ii++){

}//inline void EVALUATE::Pawn
//	*******************************************************************



//	===================================================================
// ������ ������ �� ������������ ������, �����, ������, �����
// ������ ��������������� ������������ ����� ��������
// ������ ����� ����� ������ ������� ����� ��������
 inline void EVALUATE_private::PST(const T_Bitboard * p_bitboard,// ������������� �����
                      unsigned __int8 from_white_king,// ������������ ������ ������
                      unsigned __int8 from_black_king,// ������������ ������� ������
                      unsigned __int8 & at_white_king, // ���������� ���������� ����� ������ ������ ������
                      unsigned __int8 & at_black_king, // ���������� ���������� ����� ������ ������� ������
                      __int32 & eval //����������� ������
                      ){

unsigned __int8 from; // ���������� ���� ������ ������ �����
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

// ��� ���������� ���������� ����� ��������
//int dx;// ��������� �� �����������
//int dy;// ��������� �� ���������
//int distanse;// ������ ����������
//const int kd = 7;// ����������� ���������

/*
������ � ������ ����� �� ������ ������������ ������ ����� ���������
��������(���� ������� ��������� :)) �� �������� � ������� ����

���� ��������� � ������
*/

// �����-----------------------------------------------------------------
//from_mask = p_bitboard->white_piece[5];
//while(from_mask != 0){
//    from = Bit_scan_forward_win32(from_mask);
//    from_mask = from_mask & (from_mask - 1);
//
////dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
////dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
////distanse = kd/(dx + dy);
////distanse = 0;
//
//	eval = eval + CONTROL_SQUARE
//	* Queen_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
//	              from, from_black_king, at_black_king);
//
//  }//while(from_mask != 0)

//�����----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[4];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask - 1);

    eval = eval + CONTROL_SQUARE 
	* Rook_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	             from, from_black_king, at_black_king);

//    cout << "Rook_amoun_white(p_bitboard)= "<< Rook_amoun(p_bitboard, from) << "\n";
}//while(from_mask != 0)

//����---------------------------------------------------------------------------------- 
from_mask = p_bitboard->white_piece[3];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval + Pozicion_b[from] 
	//+ CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//	                            from, from_black_king, at_black_king)
	;

//cout << "Bishop_amoun(p_bitboard, from)= "<< Bishop_amoun(p_bitboard, from) << "\n";
}//while(from_mask != 0)

//���� ----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[2];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask - 1);

    eval = eval + Pozicion_n[from] 
	//+ CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//	                           from, from_black_king, at_black_king)
    ;

 
}// while(from_mask != 0)


/////////////////////////////////////


//�����-----------------------------------------------------------------
//from_mask = p_bitboard->black_piece[5];
//while(from_mask != 0){
//    from = Bit_scan_forward_win32(from_mask);
//    from_mask = from_mask & (from_mask-1);
//
////dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
////dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
////distanse = kd/(dx + dy);
////distanse = 0;
//
//    eval = eval - CONTROL_SQUARE 
//	* Queen_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
//	              from_white_king, at_white_king);
//
//}//while(from_mask != 0)

//�����----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[4];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - CONTROL_SQUARE 
	* Rook_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
	             from_white_king, at_white_king);

}//while(from_mask != 0){

//����---------------------------------------------------------------------------------- 
from_mask = p_bitboard->black_piece[3];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - Pozicion_b[from] 
	//- CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece,p_bitboard->all_black_piece, 
	//	                            from, from_white_king, at_white_king)
	;


}//while(from_mask != 0){

//���� ----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[2];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - Pozicion_n[from] 
	//- CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, 
	//	                           from, from_white_king, at_white_king)
	;


 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ����� �� ����� ������
//eval = eval + ATAK_KING * (at_black_king - at_white_king);


}// inline int EVALUATE::PST(const T_Bitboard * p_bitboard// ������������� �����
//	*******************************************************************


//	*******************************************************************


//	===================================================================
// ����� �� ���������� ���� ����� �������. ���� ��� ��������� �� ����������� ����� 
 inline __int32 EVALUATE_private::King_shield(unsigned __int8 n,// ��������� ������ (from)
					         unsigned __int8 z,// ���� ������
					         const T_Bitboard * p_bitboard,// ������������� �����
					         const unsigned __int8 SHIELD// ����� �� ��������� ������
					         ){

unsigned __int8 st = 0;

if(z == 1){
// ������� �� ������ ����� �� ������
   if(p_bitboard->white_name_from_coord[n + 7] == 0) st = st + SHIELD;
// ������� �� ������ ����� �����
   if(p_bitboard->white_name_from_coord[n + 8] == 0) st = st + SHIELD;
// ������� �� ������ ������ �����
   if(p_bitboard->white_name_from_coord[n + 9] == 0) st = st + SHIELD;
}else{
// ������� �� ������ ���� �� ������
   if(p_bitboard->black_name_from_coord[n - 7] == 0) st = st + SHIELD;
// ������� �� ������  ����� ����
   if(p_bitboard->black_name_from_coord[n - 8] == 0) st = st + SHIELD;
// ������� �� ������ ������ ����
   if(p_bitboard->black_name_from_coord[n - 9] == 0) st = st + SHIELD;
}//if(z==1){
return st;
}//inline int EVALUATE::King_shield
//	*******************************************************************

//	===================================================================
// ������� �������� ������� � ��������� �� ���������� ������� �������� � ������
__int32 EVALUATE::Eval_forced(const T_Bitboard * p_bitboard// ������������� �����
				      ){

unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
__int32 ocenka_f = 0;

// ����� ���������

// ������----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[6];
  if(from_mask != 0) ocenka_f = ocenka_f + KING;

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[5];
  while(from_mask != 0){
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + QUEEN;
  }//while(from_mask != 0){

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + ROOK;
  }//while(from_mask != 0){

// ����------------------------------------------------------------------- 
  from_mask = p_bitboard->white_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + BISHOP;
 }//while(from_mask != 0){

// ���� -------------------------------------------------------------------
  from_mask = p_bitboard->white_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + KNIGHT;
 }// while(from_mask != 0){

// �����--------------------------------------------------------------------
  from_mask = p_bitboard->white_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + PAWN;
}//while(from_mask != 0){

///////////////////////////////////

// ������ ����������(����������)

// ������----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[6];
  if(from_mask != 0) ocenka_f = ocenka_f - KING;

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[5];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - QUEEN;
  }//while(from_mask != 0){

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - ROOK;
  }//while(from_mask != 0){

// ����------------------------------------------------------------------- 
  from_mask = p_bitboard->black_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - BISHOP;
 }//while(from_mask != 0){

// ���� -------------------------------------------------------------------
  from_mask = p_bitboard->black_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - KNIGHT;
 }// while(from_mask != 0){

// �����--------------------------------------------------------------------
  from_mask = p_bitboard->black_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - PAWN;
}//while(from_mask != 0){

//--------------------------------------------------------------------------
// ��� ������ ������� ������� ������. �� ����� �� ��� �� ���������
//int eval = 0;
//
//for(int i = 0; i < 64; i++){
//	 eval = eval + kod[p_bitboard->white_name_from_coord[i]];
//   eval = eval - kod[p_bitboard->black_name_from_coord[i]];
//}//for(int i = 0; i < 64; i++) {
//
//if (eval != ocenka_f) cout << "KARAUUUUUUUUUUUUUUL"<< "\n";
//--------------------------------------------------------------------------

return ocenka_f;
}//int EVALUATE::Eval_forced
//	*******************************************************************


//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
 inline unsigned __int8 EVALUATE_private::Rook_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					        ){

unsigned __int64 blocking_square_mask = 0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����
// ���� �����----------------------------------------------------------------------------------

// ������� � ������� ��������� ������������

// ��� �����----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� �����----------------------------------------------------
// blocking_square_mask = all_piece & e_rook_left_masks[from];
// if(blocking_square_mask !=0){ 
//   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
//   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
// }else{
//   move_mask = e_rook_left_masks[from];
// }//if(blocking_square_mask !=0){
//move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����-----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------


// ��� ������---------------------------------------------------
// blocking_square_mask = all_piece & e_rook_right_masks[from];
// if(blocking_square_mask !=0){ 
//   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
//   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
// }else{
//   move_mask = e_rook_right_masks[from];
// }//if(blocking_square_mask !=0){ 
//move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// ������� ������ ����� �����
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// ��������� ������ �����
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

  return number_n;
}//inline int EVALUATE::Rook_amoun
//	*******************************************************************

//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
 inline unsigned __int8 EVALUATE_private::Bishop_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
						      ){

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

// ���� �����---------------------------------------------------------------------------------- 

// ������� � ������� ��������� ������������

// ��� ����� - ������
 blocking_square_mask = all_piece & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����� - �����
   blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - �����
 blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - ������
 blocking_square_mask = all_piece & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
  }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// ������� ������ ����� �����
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// ��������� ������ �����
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

  return number_n;
}//inline int EVALUATE::Bishop_amoun
//	*******************************************************************


//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
 inline unsigned __int8 EVALUATE_private::Queen_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					         ){

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

// ���� �����----------------------------------------------------------------------------------

// ������� � ������� ��������� ������������

// ��� �����----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� �����----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_left_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����-----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------


// ��� ������---------------------------------------------------
 blocking_square_mask = all_piece & e_rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 

// ������� � ������� ��������� ������������

// ��� ����� - ������
 blocking_square_mask = all_piece & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����� - �����
   blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - �����
 blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - ������
 blocking_square_mask = all_piece & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
  }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// ������� ������ ����� �����
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// ��������� ������ �����
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

//if (number_n != number) cout << "������!!!!!!!!!! " << "\n";

  return number_n;
}//inline int EVALUATE::Queen_amoun
//	*******************************************************************


//	===================================================================
// ������������ ��� ��������� ���� ����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
 inline unsigned __int8 EVALUATE_private::Knigt_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					         ){

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

// ������� ���� �� ���� ������
// � ����� ������� �� �� ���� �����
move_mask = e_knigt_moves_masks[from] ^ (e_knigt_moves_masks[from] & piece) ;

if ((e_king_moves_masks[from_king] & move_mask) != 0 ) at_cnt = at_cnt + 1 ;

// ��������� ������ �����---------------------------------------
   while (move_mask !=0){
     move_mask = move_mask & (move_mask-1);
     number = number + 1;
   }//while (move !=0){
//--------------------------------------------------------------

  return number;
}//inline int EVALUATE::Knigt_amoun
//	*******************************************************************


//	===================================================================
// ���� ������ ������������� ���(�.�. ����� ������)
 __inline __int32 EVALUATE_private::Bit_scan_forward_win32(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
							 ){ 

   _asm { 
/*
�� �������� �� ����������(This document saved from http://mind.telenet.ru/samouch/7/31/index.html):
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
}//__inline int EVALUATE::Bit_scan_forward_win32
//	*******************************************************************

//	===================================================================
// ���� ��������� ������������� ���(�.�. ����� �����)
 __inline __int32 EVALUATE_private::Bit_scan_reverse_win32(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
							){ 

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
}//__inline int EVALUATE::Bit_scan_reverse_win32
//	*******************************************************************




