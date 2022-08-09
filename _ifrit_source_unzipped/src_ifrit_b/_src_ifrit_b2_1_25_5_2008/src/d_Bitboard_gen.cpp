#include <fstream.h>  // TEST
#include <stdlib.h>


#include "d_Bitboard_gen.h"
#include "k_Util.h"
#include "d_Bitboard_const.h"
//-----------------------------------------------------------------------------------
/*+
 * ��������� �������� ���������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 10.05.2008
 *
 *  ������������
 *  ��������� � ������ �������� �� ����������� �������� ��������
 *  �� ������ ������(14.03.08) ����� �� ���������
*/
//---------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_B
 ofstream Test_Bitboard;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*
������
             � &  0 0 1 ������ �������
                  0 1 1 ������ �������
                  -----
                  0 0 1 ���������

           ��� |  0 0 1
                  0 1 1
                  -----
                  0 1 1

   ����������� ^  0 0 1
   ���            0 1 1
                  -----
                  0 1 0

*/

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ����� ����� ���������� ������
inline void KONTROL_captures_W(const struct T_Bitboard * bitboard,const int * from,const int * to,const int * p,int name_figure){
#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p<0)||(*p>252595))   cout << "ERROR Generator_captures p= "   << *p    << "\n";
if ((*from<0)||(*from>63)) cout << "ERROR Generator_captures from= "<< *from << "\n";
if ((*to<0)||(*to>63))     cout << "ERROR Generator_captures to= "  << *to   << "\n";
if ((bitboard->black_name_from_coord[*to] <1)||(bitboard->black_name_from_coord[*to]>6)){
	cout << "ERROR Generator_captures bitboard->black_name_from_coord[*to]= "  << bitboard->black_name_from_coord[*to]   << "\n";
    cout << "ERROR Generator_captures from= "<< *from << "\n";
    cout << "ERROR Generator_captures to= "  << *to   << "\n";
    cout << "ERROR Generator_captures name_figure= "  << name_figure << "\n";
    Bitboard_print(bitboard);
}//if ((bitboard->black_name_from_coord[*to] <1)||(bitboard->black_name_from_coord[*to]>6)){
//if (bitboard->black_name_from_coord[*to] ==6) cout << "ERROR Generator_captures name_figure KING "  << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ������ ����� ���������� ������
inline void KONTROL_captures_B(const struct T_Bitboard * bitboard,const int * from,const int * to,const int * p,int name_figure){
#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p<0)||(*p>252595))   cout << "ERROR Generator_captures p= "   << *p    << "\n";
if ((*from<0)||(*from>63)) cout << "ERROR Generator_captures from= "<< *from << "\n";
if ((*to<0)||(*to>63))     cout << "ERROR Generator_captures to= "  << *to   << "\n";
if ((bitboard->white_name_from_coord[*to] <1)||(bitboard->white_name_from_coord[*to]>6)){
	cout << "ERROR Generator_captures bitboard->white_name_from_coord[*to]= "  << bitboard->white_name_from_coord[*to]   << "\n";
    cout << "ERROR Generator_captures from= "<< *from << "\n";
    cout << "ERROR Generator_captures to= "  << *to   << "\n";
    cout << "ERROR Generator_captures name_figure= "  << name_figure << "\n";
    Bitboard_print(bitboard);
}//if ((bitboard->white_name_from_coord[*to] <1)||(bitboard->white_name_from_coord[*to]>6)){
//if (bitboard->white_name_from_coord[*to] ==6) cout << "ERROR Generator_captures name_figure KING "  << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ 
// ��� ���������� ����� 
inline void KONTROL_move(const int * from,const int * to,const int * p){
#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p<0)||(*p>252595))   cout << "ERROR Generator_move p= "   << *p    << "\n";
if ((*from<0)||(*from>63)) cout << "ERROR Generator_move from= "<< *from << "\n";
if ((*to<0)||(*to>63))     cout << "ERROR Generator_move to= "  << *to   << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

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
// ��������� ������ ����� - ������ �� �����
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
static inline void list_record_w(int name,int d_m,int & p,int & from,unsigned __int64 & captures_mask,const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
int to; // ���������� ���� ���� ������ �����
// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  while (captures_mask !=0){
     to = first_one(captures_mask);
     captures_mask = captures_mask & (captures_mask-1);
#if KONTROL_B
KONTROL_captures_W(bitboard,&from,&to,&p,name);
#endif//KONTROL_B

// 6 ��� 
// �������� ��� ������ ������ (31 - ����� 32 - ���� 33 - ���� 34 - ����� 35 - ����� 36 - ������)
list_surplus_moves->move[p] = bitboard->black_name_from_coord[to]

// 6 ��� 
// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
//4 � �������� ��������� 5 � ������ �� �������
//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
//25 - ������ � ������������ � ����� )
 | (d_m << 6)

// 6 ��� 
// �������� ������ ����� (0 -63)
 | (from << 12)

// 6 ��� 
// �������� ���� ����� (0 -63)
 | (to << 18)

// 3 ��� 
// ������� ������ (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "bitboard->black_name_from_coord[to]= "<< bitboard->black_name_from_coord[to] << "\n";
//cout << "bitboard->black_name_from_coord[to]= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------
  //   list_surplus_moves->name_figure[p]=name;
	 //list_surplus_moves->initial_position[p] = from;
	 //list_surplus_moves->final_position[p] = to;
	 //list_surplus_moves->description_move[p] = d_m;
  //   list_surplus_moves->name_taking_figure[p] = bitboard->black_name_from_coord[to];
     p=p+1;
   }//while (captures !=0){
}//
//	*******************************************************************

//	===================================================================
// ��������� ������ ����� - ������ �� ������
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
static inline void list_record_b(int name,int d_m,int & p,int & from,unsigned __int64 & captures_mask,const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
int to; // ���������� ���� ���� ������ �����
// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  while (captures_mask !=0){
     to = first_one(captures_mask);
     captures_mask = captures_mask & (captures_mask-1);
#if KONTROL_B
KONTROL_captures_B(bitboard,&from,&to,&p,name);
#endif//KONTROL_B


// 6 ��� 
// �������� ��� ������ ������ (31 - ����� 32 - ���� 33 - ���� 34 - ����� 35 - ����� 36 - ������)
list_surplus_moves->move[p] = bitboard->white_name_from_coord[to]

// 6 ��� 
// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
//4 � �������� ��������� 5 � ������ �� �������
//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
//25 - ������ � ������������ � ����� )
 | (d_m << 6)

// 6 ��� 
// �������� ������ ����� (0 -63)
 | (from << 12)

// 6 ��� 
// �������� ���� ����� (0 -63)
 | (to << 18)

// 3 ��� 
// ������� ������ (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "bitboard->white_name_from_coord[to]= "<< bitboard->white_name_from_coord[to] << "\n";
//cout << "bitboard->white_name_from_coord[to]= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------


  //   list_surplus_moves->name_figure[p]=name;
	 //list_surplus_moves->initial_position[p] = from;
	 //list_surplus_moves->final_position[p] = to;
	 //list_surplus_moves->description_move[p] = d_m;
  //   list_surplus_moves->name_taking_figure[p] = bitboard->white_name_from_coord[to];
     p=p+1;
   }//while (captures !=0){
}//
//	*******************************************************************

//	===================================================================
// ��������� ������ �����
static inline void list_record_m(int name,int d_m,int & p,int & from,unsigned __int64 & move_mask,const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
int to; // ���������� ���� ���� ������ �����
// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask & (move_mask-1);
#if KONTROL_B
 KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// 6 ��� 
// ������ ������ 0
list_surplus_moves->move[p] = 0

// 6 ��� 
// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
//4 � �������� ��������� 5 � ������ �� �������
//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
//25 - ������ � ������������ � ����� )
 | (d_m << 6)

// 6 ��� 
// �������� ������ ����� (0 -63)
 | (from << 12)

// 6 ��� 
// �������� ���� ����� (0 -63)
 | (to << 18)

// 3 ��� 
// ������� ������ (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "0= "<< 0 << "\n";
//cout << "0= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------

     p=p+1;
   }//while (move !=0){
}//
//	*******************************************************************


//	===================================================================
// ��������� ������ ����� ��� ������ ������� ���� ��������� ������ �� ������� � ����������� �����
static inline void list_record_os(int name,int d_m,int & p,int & from,int to,int capture,struct T_list_surplus_moves * list_surplus_moves){
// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
// 6 ��� 
// ������ ������ 0
list_surplus_moves->move[p] = capture

// 6 ��� 
// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
//4 � �������� ��������� 5 � ������ �� �������
//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
//25 - ������ � ������������ � ����� )
 | (d_m << 6)

// 6 ��� 
// �������� ������ ����� (0 -63)
 | (from << 12)

// 6 ��� 
// �������� ���� ����� (0 -63)
 | (to << 18)

// 3 ��� 
// ������� ������ (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "0= "<< 0 << "\n";
//cout << "0= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------

  //   list_surplus_moves->name_figure[p]=name;
	 //list_surplus_moves->initial_position[p] = from;
	 //list_surplus_moves->final_position[p] = to;
	 //list_surplus_moves->description_move[p] =d_m; 
  //   list_surplus_moves->name_taking_figure[p] = capture;
     p=p+1;
}//
//	*******************************************************************




//	===================================================================
// ���������� ������ ������ � ����������� �����
void Generator_captures_white(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

unsigned __int64 move_mask_d = 0; // ����� ���� ����� �� ��� ����
unsigned __int64 one_pawn_mask = 0; // ����� ������������ ����� �����
unsigned __int64 move_conversion_mask = 0; // ����� ����� ����� � ������������
unsigned __int64 captures_conversion_mask = 0; // ����� ������ ����� � �������������

// ���������
const unsigned __int64 LINE_0 = 255; // 1 ����������� �����

const unsigned __int64 LINE_7 = 18374686479671623680; // 8 ����������� �����

const unsigned __int64 COLUMN_0 = 72340172838076673; // 1 ��������� �����
const unsigned __int64 COLUMN_7 = 9259542123273814144; // 8 ��������� �����


int blocking_coordinate = 0; // ���������� �������������� ���� 
int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int p =0; // ���������� ����� � ������ 

// ���������� ���� ����� �����
// ���� ������--------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������ ������
  from_mask = bitboard->white_piece[6];
 // ������� ���������� ������(����� ������(��� ������ ����������� ������ ) ������������� ���)
  from = first_one(from_mask);
// ������� ����� ������ ������
  captures_mask = (bitboard->all_black_piece & king_moves_masks[from]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(6,2,p,from,captures_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
}
// ���� ������--------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������ �����
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������ (�� ���� �����)
// ��� �����
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) ������� ������������� ����.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];

 if(blocking_square_mask !=0){ 
// 2) ������� ���������� ������� �������������� ����
   blocking_coordinate =  first_one(blocking_square_mask);
// 4) ������� ����� ������
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� ����  
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ������� � ������� ��������� ������������ (�� ���� �����)
// ��� ����� - ������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� ����� - �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){

}
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
   blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// �����---------------------------------------------------------------------------------- 

// ���� ���� ----------------------------------------------------------------------------------
{// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    captures_mask = (bitboard->all_black_piece & knigt_moves_masks[from]);

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(2,2,p,from,captures_mask,bitboard,list_surplus_moves);

 }// while(from_mask != 0){
}
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
 // �������� ���� ����� 
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 ������� ������� ���� ����� ����� �� ���� ���� ������
  move_mask = (one_pawn_mask << 8) & (~(bitboard->all_piece));

// ���� � ������������
  move_conversion_mask = move_mask & LINE_7;

// 3 ������� ������ �����
// ������ ( ����� ���� ������ �.�. ��������� � �1) �����  ( ������ ���� ������ �.�. ��������� � h1)
 captures_mask = ((one_pawn_mask << 9) & ~COLUMN_0) | ((one_pawn_mask << 7) & ~COLUMN_7);
 captures_mask = captures_mask &  bitboard->all_black_piece;

// ������ � ������������
captures_conversion_mask = captures_mask & LINE_7;

// ������ ������
captures_mask = captures_mask & ~LINE_7;

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(1,2,p,from,captures_mask,bitboard,list_surplus_moves);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// ������� ���������� ����� � ������������ �����
  while (move_conversion_mask !=0){
    to = first_one(move_conversion_mask);
    move_conversion_mask = move_conversion_mask & (move_conversion_mask-1);

// �������� ����������� �����
// ����������� � ����� 14,����������� � ���� 12,����������� � ����� 13,����������� � ����� 15,
list_record_os(1,15,p,from,to,0,list_surplus_moves);

list_record_os(1,14,p,from,to,0,list_surplus_moves);

list_record_os(1,13,p,from,to,0,list_surplus_moves);

list_record_os(1,12,p,from,to,0,list_surplus_moves);
 
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
   }//while (captures !=0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ������� ���������� ������ � ������������ �����  
  while (captures_conversion_mask !=0){
    to = first_one(captures_conversion_mask);
    captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask-1);

// �������� ����������� ����� � �������
// ����������� � ������� � �����
list_record_os(1,25,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);

// ����������� � ������� � �����
list_record_os(1,24,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);
 
// ����������� � ������� � �����
list_record_os(1,23,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);

// ����������� � ������� � ����
list_record_os(1,22,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);

#if KONTROL_B
 KONTROL_captures_W(bitboard,&from,&to,&p,120);
#endif//KONTROL_B
   }//while (captures !=0){

// 4 ������� ������ �� �������
 if(bitboard->taking_passage_yes == 1){
// ������ �� �������
   to = 8 * bitboard->taking_passage_y + bitboard->taking_passage_x;

// ������� ��� �������� ���� ������� ����� ���� ����� ���������
   if ( ((from - to) == -9) && (from % 8 != 7) ){
list_record_os(1,5,p,from,to,bitboard->black_name_from_coord[to - 8],list_surplus_moves);

#if KONTROL_B
to = to - 8;
KONTROL_captures_W(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ((from - to) == 9) && (from % 8 != 0)){

   if ( ((from - to) == -7) && (from % 8 != 0) ){
list_record_os(1,5,p,from,to,bitboard->black_name_from_coord[to - 8],list_surplus_moves);

#if KONTROL_B
to = to - 8;
KONTROL_captures_W(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ( ( (from - to) == 9) | ((from - to) == 7)){

  }//if(bitboard->taking_passage_yes == 1){

}//while(from_mask != 0){

}
// �����----------------------------------------------------------------------------------

list_surplus_moves->end_list = p;
list_surplus_moves->end_captures = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// ���������� ������ ������ � ����������� �����
void Generator_captures_black(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

unsigned __int64 move_mask_d = 0; // ����� ���� ����� �� ��� ����
unsigned __int64 one_pawn_mask = 0; // ����� ������������ ����� �����
unsigned __int64 move_conversion_mask = 0; // ����� ����� ����� � ������������
unsigned __int64 captures_conversion_mask = 0; // ����� ������ ����� � �������������

// ���������
const unsigned __int64 LINE_0 = 255; // 1 ����������� �����

const unsigned __int64 LINE_7 = 18374686479671623680; // 8 ����������� �����

const unsigned __int64 COLUMN_0 = 72340172838076673; // 1 ��������� �����
const unsigned __int64 COLUMN_7 = 9259542123273814144; // 8 ��������� �����


int blocking_coordinate = 0; // ���������� �������������� ���� 
int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int p =0; // ���������� ����� � ������ 

// ���������� ���� ������ �����
// ���� ������---------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[6];
  from = first_one(from_mask);
  captures_mask = (bitboard->all_white_piece & king_moves_masks[from]);

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(6,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}
// ������---------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
  from = first_one(from_mask);
  from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// �����----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
   blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// �����----------------------------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ��� ����� - ������
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// �����---------------------------------------------------------------------------------- 

// ���� ���� ----------------------------------------------------------------------------------
{
// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    captures_mask = (bitboard->all_white_piece & knigt_moves_masks[from]);

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(2,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }// while(from_mask != 0){
//-------------------------------------------------------------------------------------
}
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 ������� ������� ���� ������ ����� �� ���� ���� ������
  move_mask = (one_pawn_mask >> 8) & (~(bitboard->all_piece));

// ���� � ������������
  move_conversion_mask = move_mask & LINE_0;

// 3 ������� ������ �����
// ������ ( ����� ���� ������ �.�. ��������� � �1) �����  ( ������ ���� ������ �.�. ��������� � h1)
 captures_mask = ((one_pawn_mask >> 7) & ~COLUMN_0) | ((one_pawn_mask >> 9) & ~COLUMN_7 );
 captures_mask = captures_mask &  bitboard->all_white_piece;

// ������ � ������������
captures_conversion_mask = captures_mask & LINE_0;

// ������ ������
captures_mask = captures_mask & ~LINE_0;

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(1,2,p,from,captures_mask,bitboard,list_surplus_moves);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// ������� ���������� ����� � ������������ �����
  while (move_conversion_mask !=0){
    to = first_one(move_conversion_mask);
    move_conversion_mask = move_conversion_mask & (move_conversion_mask-1);

// �������� ����������� �����
// ����������� � ����� 14,����������� � ���� 12,����������� � ����� 13,����������� � ����� 15,
list_record_os(1,15,p,from,to,0,list_surplus_moves);

list_record_os(1,14,p,from,to,0,list_surplus_moves);

list_record_os(1,13,p,from,to,0,list_surplus_moves);
 
list_record_os(1,12,p,from,to,0,list_surplus_moves);

#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
   }//while (captures !=0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ������� ���������� ������ � ������������ �����  
  while (captures_conversion_mask !=0){
    to = first_one(captures_conversion_mask);
    captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask-1);

// �������� ����������� ����� � �������
// ����������� � ������� � ����
list_record_os(1,25,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

// ����������� � ������� � �����
list_record_os(1,24,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

// ����������� � ������� � �����
list_record_os(1,23,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

// ����������� � ������� � �����
list_record_os(1,22,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

#if KONTROL_B
KONTROL_captures_B(bitboard,&from,&to,&p,120);
#endif//KONTROL_B
   }//while (captures !=0){

// 4 ������� ������ �� �������
 if(bitboard->taking_passage_yes == 1){
// ������ �� �������
    to = 8 * bitboard->taking_passage_y + bitboard->taking_passage_x;

   if ( ((from - to) == 9) && (from % 8 != 0) ){
list_record_os(1,5,p,from,to,bitboard->white_name_from_coord[to + 8],list_surplus_moves);

#if KONTROL_B
 to = to + 8;
 KONTROL_captures_B(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ((from - to) == 9) && (from % 8 != 0)){

   if ( ((from - to) == 7) && (from % 8 != 7) ){
list_record_os(1,5,p,from,to,bitboard->white_name_from_coord[to + 8],list_surplus_moves);

#if KONTROL_B
to = to + 8;
 KONTROL_captures_B(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ( ( (from - to) == 9) | ((from - to) == 7)){

  }//if(bitboard->taking_passage_yes == 1){

}//while(from_mask != 0){

}
// �����----------------------------------------------------------------------------------

list_surplus_moves->end_list = p;
list_surplus_moves->end_captures = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ � ����������� �����)
void Generator_move_white(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

unsigned __int64 move_mask_d = 0; // ����� ���� ����� �� ��� ����
unsigned __int64 one_pawn_mask = 0; // ����� ������������ ����� �����
unsigned __int64 move_conversion_mask = 0; // ����� ����� ����� � ������������
unsigned __int64 captures_conversion_mask = 0; // ����� ������ ����� � �������������

// ���������
const unsigned __int64 LINE_0 = 255; // 1 ����������� �����
const unsigned __int64 LINE_1 = 255<<8; // 2 ����������� �����

const unsigned __int64 LINE_6 = 71776119061217280; // 7 ����������� �����
const unsigned __int64 LINE_7 = 18374686479671623680; // 8 ����������� �����

const unsigned __int64 CASTLING_q = 1008806316530991104; // ����  B8 C8 D8 ������
const unsigned __int64 CASTLING_k = 6917529027641081856; // ���� F8 G8 ������

const unsigned __int64 CASTLING_Q = 14; // ����  B1 C1 D1 ������
const unsigned __int64 CASTLING_K = 96; // ���� F1 G1 ������


int blocking_coordinate = 0; // ���������� �������������� ���� 
int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int p ; // ���������� ����� � ������ 

// ���������� ������ ������. ����� ������� � ��������� ����������� �������� �� �����
  
 p = list_surplus_moves->end_list;

// ���������� ���� ����� �����
// ���� ������--------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������ ������
  from_mask = bitboard->white_piece[6];
 // ������� ���������� ������(����� ������(��� ������ ����������� ������ ) ������������� ���)
    from = first_one(from_mask);
// ������� ����� ����� ������(��� ������)
   move_mask = king_moves_masks[from] & (~bitboard->all_piece);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(6,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��������� ������ ������
// ���� ������ �� ����� �� ����� ������ ������������� ����������� ���������
   if (from == 4){
// ���� ������� ��������� ���������
	   if(bitboard->castling_Q){
// � ����  1 2 3 ������  �� ��������� ���
to = 2;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
          if(((~bitboard->all_piece) & CASTLING_Q ) == CASTLING_Q ){
// ������� ��������� ����� ��������� 0-0-0 2 3 4
			if((White_check_square(2,bitboard)==0)&&(White_check_square(3,bitboard)==0)&&(White_check_square(4,bitboard)==0)){
list_record_os(6,3,p,from,2,0,list_surplus_moves);

			}//if((White_check_square(2,bitboard)==0)&&(White_check_square(3,bitboard)==0)&&(White_check_square(4,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 14)){ 
	   }//if(bitboard->castling_Q){

// ���� �������� ��������� ��������� �� ��������� ��� 
	   if(bitboard->castling_K){
to = 6;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// � ����  5 6 ������  �� ��������� ���
          if( ( (~bitboard->all_piece) & CASTLING_K ) == CASTLING_K ){

// �������� ��������� ����� ��������� 0-0 4 5 6
	       if((White_check_square(4,bitboard)==0)&&(White_check_square(5,bitboard)==0)&&(White_check_square(6,bitboard)==0)){
 list_record_os(6,4,p,from,6,0,list_surplus_moves);
			   
			}//if((White_check_square(4,bitboard)==0)&&(White_check_square(5,bitboard)==0)&&(White_check_square(6,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 96)){ 
	   }//if(bitboard->castling_K){
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
   }//if (from_mask == 4){
}
// ���� ������--------------------------------------------------------------------------------




// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ����� ������������ ������ �����
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������ (�� ���� �����)
// ��� �����
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) ������� ������������� ����.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];

 if(blocking_square_mask !=0){ 
// 2) ������� ���������� ������� �������������� ����
   blocking_coordinate =  first_one(blocking_square_mask);
// 3) ������� ����� ����������� ����� (���� ������ � ������� ����� �����)
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
// 4) ������� ����� ������
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
// 5) ������� ����� �����
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 


// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� ����  
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ������� � ������� ��������� ������������ (�� ���� �����)
// ��� ����� - ������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� ����� - �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  }//while(from_mask != 0){

}
// �����----------------------------------------------------------------------------------



// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// �����----------------------------------------------------------------------------------



// ���� �����---------------------------------------------------------------------------------- 
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
   blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// �����---------------------------------------------------------------------------------- 




// ���� ���� ----------------------------------------------------------------------------------
{// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    move_mask = knigt_moves_masks[from] & (~bitboard->all_piece);

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(2,1,p,from,move_mask,bitboard,list_surplus_moves);

 }// while(from_mask != 0){
}
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
 // �������� ���� ����� 
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 ������� ������� ���� ����� ����� �� ���� ���� ������
  move_mask = (one_pawn_mask << 8) & (~(bitboard->all_piece));
// 2 ������� ���� ����� �� 2 ���� ������ 
// � ������ ��������� ������ ����� ������� �� 2 �����������
  move_mask_d = one_pawn_mask & LINE_1;
// �������� ����� ���� ��� ��� �� ��������������
  move_mask_d = (move_mask_d <<8) & (~(bitboard->all_piece));
// ����������� ������� ������ ���
  move_mask_d = (move_mask_d << 8) & (~(bitboard->all_piece));

// ����� ��� ���� ����� ��� �� ���� ��� �� ��� ���� ������
  move_mask = (move_mask | move_mask_d) ;

// ���� ��� �����������
  move_mask = move_mask & ~LINE_7;

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(1,1,p,from,move_mask,bitboard,list_surplus_moves);
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------
}

list_surplus_moves->end_list = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ � ����������� �����)
void Generator_move_black(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

unsigned __int64 move_mask_d = 0; // ����� ���� ����� �� ��� ����
unsigned __int64 one_pawn_mask = 0; // ����� ������������ ����� �����
unsigned __int64 move_conversion_mask = 0; // ����� ����� ����� � ������������
unsigned __int64 captures_conversion_mask = 0; // ����� ������ ����� � �������������

// ���������
const unsigned __int64 LINE_0 = 255; // 1 ����������� �����
const unsigned __int64 LINE_1 = 255<<8; // 2 ����������� �����

const unsigned __int64 LINE_6 = 71776119061217280; // 7 ����������� �����
const unsigned __int64 LINE_7 = 18374686479671623680; // 8 ����������� �����

const unsigned __int64 CASTLING_q = 1008806316530991104; // ����  B8 C8 D8 ������
const unsigned __int64 CASTLING_k = 6917529027641081856; // ���� F8 G8 ������

const unsigned __int64 CASTLING_Q = 14; // ����  B1 C1 D1 ������
const unsigned __int64 CASTLING_K = 96; // ���� F1 G1 ������


int blocking_coordinate = 0; // ���������� �������������� ���� 
int from = 0; // ���������� ���� ������ ������ �����
int to = 0; // ���������� ���� ���� ������ �����
int p =0; // ���������� ����� � ������ 

p = list_surplus_moves->end_list;

// ���������� ���� ������ �����
// ���� ������---------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[6];
  from = first_one(from_mask);
  move_mask = king_moves_masks[from] & (~(bitboard->all_piece));

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(6,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��������� ������� ������
// ���� ������ �� ����� �� ����� ������ ������������� ����������� ���������
   if (from == 60){
// ���� ������� ��������� ���������
	   if(bitboard->castling_q){
to = 58;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// � ����  1 2 3 ������  �� ��������� ���
          if((~(bitboard->all_piece) & CASTLING_q )== CASTLING_q ){
// ������� ��������� ������ ��������� 0-0-0 58 59 60
			 if((Black_check_square(58,bitboard)==0)&&(Black_check_square(59,bitboard)==0)&&(Black_check_square(60,bitboard)==0)){
 
list_record_os(6,3,p,from,58,0,list_surplus_moves);
				 
			 }//if(Black_check_square(58,bitboard)==0)&&(Black_check_square(59,bitboard)==0)&&(Black_check_square(60,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 14)){ 
	   }//if(bitboard->castling_Q){
// ���� �������� ��������� ��������� �� ��������� ��� 
	   if(bitboard->castling_k){
to = 62;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// � ����  5 6 ������  �� ��������� ���
          if((~(bitboard->all_piece) & CASTLING_k )==CASTLING_k){
// �������� ��������� ������ ��������� 0-0 60 61 62
			if((Black_check_square(60,bitboard)==0)&&(Black_check_square(61,bitboard)==0)&&(Black_check_square(62,bitboard)==0)){
 list_record_os(6,4,p,from,62,0,list_surplus_moves);

			}//if((Black_check_square(60,bitboard)==0)&&(Black_check_square(61,bitboard)==0)&&(Black_check_square(62,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 96)){ 
	   }//if(bitboard->castling_K){
   
   }//if (from_mask == 60){

}
// ������---------------------------------------------------------------------------------




// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
  from = first_one(from_mask);
  from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// �����----------------------------------------------------------------------------------



// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����
   blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// �����----------------------------------------------------------------------------------



// ���� �����---------------------------------------------------------------------------------- 
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// ������� � ������� ��������� ������������
// ��� ����� - ������
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// �����---------------------------------------------------------------------------------- 




// ���� ���� ----------------------------------------------------------------------------------
{
// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
// ������������ ����� ������� ��� ��� ������� �� ��� ����������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    move_mask = knigt_moves_masks[from] & (~(bitboard->all_piece));

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(2,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }// while(from_mask != 0){
}
// ���� ----------------------------------------------------------------------------------

// ���� �����----------------------------------------------------------------------------------
{
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 ������� ������� ���� ������ ����� �� ���� ���� ������
  move_mask = (one_pawn_mask >> 8) & (~(bitboard->all_piece));

// 2 ������� ���� ����� �� 2 ���� ������ 
// � ������ ��������� ������ ����� ������� �� 7 �����������
  move_mask_d = one_pawn_mask & LINE_6;

// �������� ����� ���� ��� ��� �� ��������������
  move_mask_d = (move_mask_d >>8) & (~(bitboard->all_piece));
// ����������� ������� ������ ���
  move_mask_d = (move_mask_d >> 8) & (~(bitboard->all_piece));

// ����� ��� ���� ����� ��� �� ���� ��� �� ��� ���� ������
  move_mask = (move_mask | move_mask_d) ;

// ���� ��� �����������
  move_mask = move_mask & ~LINE_0;

// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(1,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}//while(from_mask != 0){
// �����----------------------------------------------------------------------------------
}

list_surplus_moves->end_list = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// ��� ���� �� ������ ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
int King_white_check(const struct T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

int blocking_coordinate = 0; // ���������� �������������� ���� 
int from = 0; // ���������� ���� ������ ������ �����
int i = 0; // ���������� 

// ����� ������������ ������ ������
  from_mask = bitboard->white_piece[6];
 // ������� ���������� ������(����� ������(��� ������ ����������� ������ ) ������������� ���)
  from = first_one(from_mask);

// ��� �����
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) ������� ������������� ����.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];

 if(blocking_square_mask !=0){ 
// 2) ������� ���������� ������� �������������� ����
   blocking_coordinate =  first_one(blocking_square_mask);
// 3) ������� ����� ����������� ����� (���� ������ � ������� ����� �����)
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
// 4) ������� ����� ������
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);

   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� ����  
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ��� ����� - ������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� ����� - �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



// ����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_black_piece & knigt_moves_masks[from]);

// ������� ����++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0 ){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ������
// ������� ����� ������ ������
  captures_mask = (bitboard->all_black_piece & king_moves_masks[from]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ������� ������++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

if ((from / 7) < 8){
// ����� ������� ��������
   if (from % 8 != 0){
     if( bitboard->black_name_from_coord[from + 7] ==1) return 1;
   }//if (from % 8 != 1){
   if (from % 8 != 7){
     if( bitboard->black_name_from_coord[from + 9] ==1) return 1;
   }//if (from % 8 != 7){
}//if ((from / 7) < 8){

 return 0; 
}//int White_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// ��� ���� �� ������ ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
int King_black_check(const struct T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

int blocking_coordinate = 0; // ���������� �������������� ���� 
int from = 0; // ���������� ���� ������ ������ �����
int i = 0; // ���������� 

 // ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[6];
  from = first_one(from_mask);
 
 // ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� �����
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
 
// ��� ����  
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ����� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// ����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_white_piece & knigt_moves_masks[from]);

// ������� ����++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ������
// ������� ����� ������ ������
  captures_mask = (bitboard->all_white_piece & king_moves_masks[from]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ������� ������++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

if ((from / 7) > 1 ){
// ����� ������� ��������
   if (from % 8 != 7){
     if( bitboard->white_name_from_coord[from - 7] ==1) return 1;
   }// if (from % 8 != 7){
   if (from % 8 != 0){
     if( bitboard->white_name_from_coord[from - 9] ==1) return 1;
   }// if (from % 8 != 7){
}//if ((from / 7) >0){

 return 0;
 
}//int White_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// ��� ���� �� ����� ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
static inline int White_check_square(int sq, const struct T_Bitboard * bitboard){

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

int blocking_coordinate = 0; // ���������� �������������� ���� 
int i = 0; // ���������� 

// ��� �����
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) ������� ������������� ����.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[sq];

 if(blocking_square_mask !=0){ 
// 2) ������� ���������� ������� �������������� ����
   blocking_coordinate =  first_one(blocking_square_mask);
// 3) ������� ����� ����������� ����� (���� ������ � ������� ����� �����)
   move_mask = rook_up_masks[sq] ^ rook_up_masks[blocking_coordinate];
// 4) ������� ����� ������
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[sq] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[sq] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ��� ����� - ������
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[sq] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� ����� - �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[sq] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// ����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_black_piece & knigt_moves_masks[sq]);

// ������� ����++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0 ){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ������
// ������� ����� ������ ������
  captures_mask = (bitboard->all_black_piece & king_moves_masks[sq]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ������� ������++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ����� ������� ��������
 if( bitboard->black_name_from_coord[sq + 7] ==1) return 1;
 if( bitboard->black_name_from_coord[sq + 9] ==1) return 1;

 return 0; 
}//int White_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// ��� ���� �� ������ ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
static inline int Black_check_square(int sq, const struct T_Bitboard * bitboard){

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
unsigned __int64 captures_mask = 0; // ����� ������
unsigned __int64 move_mask = 0; // ����� �����

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����

int blocking_coordinate = 0; // ���������� �������������� ���� 
int i = 0; // ���������� 

// ��� ����  
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[sq] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��� �����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[sq] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ������
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[sq] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// ��� ���� - ������
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[sq] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ��� ���� - �����
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[sq] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// ������� �� ���� ����� � �����++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// ����
// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_white_piece & knigt_moves_masks[sq]);

// ������� ����++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ������
// ������� ����� ������ ������
  captures_mask = (bitboard->all_white_piece & king_moves_masks[sq]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ������� ������++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ����� ������� ��������

 if( bitboard->white_name_from_coord[sq - 7] ==1) return 1;
 if( bitboard->white_name_from_coord[sq - 9] ==1) return 1;

 return 0; 
}//int Black_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// �������� �������� �����
#if TEST_B
void Bitboard_print(const struct T_Bitboard * bitboard){
unsigned __int64 z=0;
    Test_Bitboard.open ("_TEST_BITBOARD.txt",ios::app);

Test_Bitboard << "bitboard->colour_move = "<< bitboard->colour_move <<"\n";
Test_Bitboard << "\n";

 Test_Bitboard << "bitboard->taking_passage_yes= " << bitboard->taking_passage_yes << "\n";
 Test_Bitboard << "bitboard->taking_passage_x= " << bitboard->taking_passage_x << "\n";
 Test_Bitboard << "bitboard->taking_passage_y= " << bitboard->taking_passage_y << "\n";

Test_Bitboard << "\n";
 Test_Bitboard << "bitboard->castling_K= " << bitboard->castling_K << "\n";
 Test_Bitboard << "bitboard->castling_Q= " << bitboard->castling_Q << "\n";
 Test_Bitboard << "bitboard->castling_k= " << bitboard->castling_k << "\n";
 Test_Bitboard << "bitboard->castling_q= " << bitboard->castling_q << "\n";

Test_Bitboard << "------------------------"<<"\n";
Test_Bitboard << "\n";
// all_white_piece
//-------------------------------------------------------------------------------------
Test_Bitboard << "\n";
Test_Bitboard << "all_white_piece"<< "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z= bitboard->all_white_piece & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------

// all_black_piece
//-------------------------------------------------------------------------------------
Test_Bitboard << "\n";
Test_Bitboard << "all_black_piece"<< "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z= bitboard->all_black_piece & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
 Test_Bitboard << "\n";
 Test_Bitboard << "white_piece"<< "\n";
for(int p=1 ; p<7 ; p++){
Test_Bitboard << "\n";
Test_Bitboard << p << "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z=  bitboard->white_piece[p] & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
}//for(int i=0 ; i<8 ; i++){
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
 Test_Bitboard << "\n";
 Test_Bitboard << "black_piece"<< "\n";
for(int p=1 ; p<7 ; p++){
Test_Bitboard << "\n";
Test_Bitboard << p << "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z=  bitboard->black_piece[p] & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
}//for(int i=0 ; i<8 ; i++){
//-------------------------------------------------------------------------------------

Test_Bitboard <<"============================================"<< "\n";

    Test_Bitboard.close();
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
}//void Bitboard_print(T_Bitboard * bitboard){
#endif//TEST_B
//	*******************************************************************

//	===================================================================
// ��������� ������������� �����
void Bitboard_ini_t(struct T_Bitboard * bitboard){

bitboard->all_piece =0;
bitboard->all_white_piece =0;
bitboard->all_black_piece =0;

for(int i=0 ; i<7 ; i++){
  bitboard->white_piece[i]=0;
  bitboard->black_piece[i]=0;
}//for(int i=0 ; i<8 ; i++){

// ��������� �� ���������
for(int j=0 ; j<64 ; j++){
  bitboard->white_name_from_coord[j]=0;
  bitboard->black_name_from_coord[j]=0;
}//for(int j=0 ; j<64 ; j++){

}//void Bitboard_ini_t(T_Bitboard * bitboard){
//	*******************************************************************


