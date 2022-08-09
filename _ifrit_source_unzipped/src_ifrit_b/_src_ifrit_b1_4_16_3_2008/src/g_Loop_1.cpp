#include <iostream.h> // TEST
#include <fstream.h>  // TEST
#include <cstring>    // ���������� ��� ������ �� ������� (��������� �����)
#include <stdlib.h>   // ������������ ���������� ������ �������� exit(1)


#include "k_Util.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "i_Forced.h"
#include "k_Win_32.h"
#include "g_Loop.h"
#include "j_Hash.h"
#include "j_Killer.h"
#include "l_Sorting.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"


//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 10.02.2008
 *
 *  ������������
 * �������� ���������� ������� � ���� _TEST_LOOP.txt
 * ��������
 * ��������� ������ 
 * ����� ����� ��������
 *
 * ��������� ��������� ������
 * ����������� � ��������� ��� ����� �����. ���� ����� ��� ��������� ���������
 * �� ���� ��� ������ ��������� � ������ ��� �������� ��� �������
 * ����� ��������������:
 * 1 ��������� ������� - ��������� �� ������������ ������������ ������� �� ���������� �� ���
 * 2 ����� �������� - ��������� �� ��� ���������� ��� �� ��� ���������
 * ��������� �� ���������� ������ � ������� �������� � ����� ���������� �������
 * 3 �����-���� ��������� - ��������� �� ��� �������� ��� �� ��� �������������
 *   ��� ���������� ��������� 
*/
//---------------------------------------------------------------------------


//	===================================================================
/*
� ����� �������� � ����� ����� ���� � ������������ �������
int AlphaBeta (pos, depth, alpha, beta) {
	if (depth == 0) return Evaluate(pos);
	best = -INFINITY;
	succ = Successors(pos);
	while (!Empty(succ)) {
		pos = RemoveOne(succ);
		value = -AlphaBeta(pos, depth - 1, -beta, -alpha);
		if (value > best) best = value;
	    if (value > alpha) alpha = value;
        if (alpha >= beta) return alpha;
	}
	return best;
}
----------------------------------------------------------------------------
��� �������� ��������� �������:
1 � ������ ������������ ������ ���������� �����. ���������� ������ ��� ��� �������� �� ���
  � � ��� ���� ����� ���� ��� ��������� ��� ����� ��� ��� ������ ��� ���
2 ������ �� ��������� ���� ������ ������ ������ �� ���� � ��� �� ���� ���� �� ����
3 ������ �� ��������� ��� � ��������� ������� �� ��� ���� ���� ��������� ��� ���� ��� 
 ��� ���� ��� ������ �� ��� ���������� ����� ���� ������
4 ������������ ����������
5 ����� ��������� � ������� ���������� ���
6 ���� ����� �� ���� �� ��� ������ ���� ��� ����

*/
// ������� �� ������� ������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
int Alpha_beta_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,int description_move,T_PV * three_moves,int & br,int for_chek,int null_m){
int value=0;              // ������� ������ �������
int value_best=9999999;   // ������ � ����  ������ �������  
int inv=0;                // �������������� ������ �������
int no_moves=0;           // ���� ������������� ��������� ����� (���� 1 �� ��������� ����� �� ����)
int nodes_0=1;            // ���������� ����� �� ������� ������
int number_taking=0;      // �������� ������� �� ������� � ������ ��������� �����
int key=0;                // ��� ����
int insert_hash=0;        // ��������� �� ��� �� ���� �� ������ �������
double  time_nodes=0;     // 
T_PV pv_best_point={0};   // ������ ��� ������� ���� �������

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// ��� ������� ��� ���������� ������
if (br == 1) return 0;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
list_surplus_moves->current_depth =depth;//
//��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
Generator_captures(bitboard,list_surplus_moves);
Generator_move(bitboard,list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
  #if TEST_L
    List_816_Print(depth,list_surplus_moves);
  //Bitboard_print(bitboard);
  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if HASH
// �������� � �����
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
// �.�. �� � ������ ��� �� ���� ������ ������ � ������ �� �� ����� ��� ��� ��� �� ����
  Look_hash(list_surplus_moves,bitboard->hash_key,depth,insert_hash);
 #endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
// ����� ������� �������
#if KILLER
// ���� ������� � ����� ������ ����� �� ������ ������� ����� �� ������
  if(insert_hash==0){
    Insert_killer(list_surplus_moves,depth,list_surplus_moves->start_list[depth]);
  }else{
    Insert_killer(list_surplus_moves,depth,(list_surplus_moves->start_list[depth]+1));
  }//if(insert_hash==0){
#endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������ ������ � ����� �� ������� ������
  #if TEST_L
 //   List_816_Print(depth,list_surplus_moves);
 //   Board_816_Print( depth,board_list_816);
  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for ( int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){

// ��������� ��� �� ������ ��� ������� ������� ���������� �������
 Onward_moves(i,list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

// ���� ������ �� ����� ��
 if (King_check(bitboard)!=0){
// �� �������� ���� ��� ��� ������������
   Backs_moves(i,list_surplus_moves,bitboard);
 }else{
// ������ ���� ��� ���� ��������� ���� � ����������� �������
 no_moves = 1;
 nodes = nodes+1;

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// �������� ���� � ��������������� ���� � ������� ��������
Print_cout_v(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value);
// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
 Update_PV_current(i,depth,pv_current,list_surplus_moves);
// ��������� ��� ����
Modification_up_random_key(i,list_surplus_moves,bitboard);
// ���������� ���� ��� �������� ��� ����������� � �������
key=bitboard->hash_key;

Timer_start();
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// ����������� ����� �������
value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,0,0);
list_surplus_moves->current_depth =depth;
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
 Timer_stop();
 time_nodes= Timer_delta();
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
// �������� ��� ����� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
// ��������� ������� �� ���� �� ���������� �� �������
 #if HASH
   if(key!=bitboard->hash_key){
     cout <<"\n";
     cout << "ERROR HESH " ;
     cout <<"\n";
   }//if(key!=board_list_816->hash_key){
 #endif 

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// ��������������� �������
 Backs_moves(i,list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// ��������������� ��� ����
 Modification_down_random_key(i,list_surplus_moves,bitboard);
 //Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max);

 // ���� ������ �������
 if (value < value_best){
   value_best = value;
// ����� �� �������������� ������ ������� ��������������� ������ ������
   Update_PV_best_point(&pv_best_point,pv_best);
 #if HASH
// �������� ������ �� ������ ������ ���
   Save_best_hash_key(i,list_surplus_moves,bitboard->hash_key,depth);
 #endif
   //Test_Loop << "bitboard->hash_key="<<bitboard->hash_key << "\n";
  // Bitboard_print(bitboard);
 }//if (value < value_best){ 

// ���� ALPHA BETA ����������� 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value < alpha){
   alpha = value;
 }//if (value < alpha){ 

   if(alpha <= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

//--------------------------------------------------------------------
// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� �.�. � ����� �����������
 if (br!=1){
  nodes_0 = nodes_0+1;
 #if TEST_PRINT
   // ��� ����� ���������
  Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value,time_nodes);
#else
  // ��� �������� ������
  Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max,value_best,time_nodes);
#endif


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  #if TEST_L
 //   List_816_Print(depth,list_surplus_moves);
 //   Board_816_Print( depth,board_list_816);
  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

 }//if (br!=1){
//--------------------------------------------------------------------
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){
//3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333

//����444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
// ��������� ������ ����� �� ���� ��������� �����

// ���� �� ������ ���� �������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (no_moves == 0){
  value=No_moves(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1 // ����� ������ �������
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 Update_PV_best(pv_best,pv_best_point);
//Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max);
// cout <<"alpha= "<<alpha;
// cout <<"\n";
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

return value_best;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

}//int Alpha_beta_root(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************



//	===================================================================
// 
int Alpha_beta_816(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int for_chek, int null_m){
int value=0;              // ������� ������ �������
int value_best=9999999;   // ������ ������� ������ � ���� 
int inv=0;                // �������������� ������ �������
int no_moves=0;           // ���� ������������� ��������� �����
int depth_max_z=0;        // ���������� �� �����
int for_chek_l=0;         // ���������� �� �����
int number_taking=0;      // �������� ������� �� ������� � ������ ��������� �����
int key=0;                // ��� ����
int insert_hash=0;        // ��������� �� ��� �� ���� �� ������ �������
T_PV pv_best_point={0};   // ������ ��� ������� ���� �������

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// ��� ������� ��� ���������� ������
if (br == 1) return 0;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//���������������������������������������������������������������������������������������������
// �� ���������� ������� ���������� ����������� ������ �������
if (depth == depth_max){
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
  value=Max_depth(bitboard,list_surplus_moves,alpha,beta,depth_max,depth,nodes,pv_current,pv_best,three_moves,description_move,br,for_chek);
//  value=0;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 return value ;
}//	if (Depth == Depth_Max){
//���������������������������������������������������������������������������������������������

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if NULL_MOVE
// ������� ���
if (Null_move(bitboard,depth,depth_max,null_m,description_move,for_chek,list_surplus_moves,alpha,beta,nodes,pv_current,pv_best,three_moves,br)==1){
	return beta;
}//if (Null_move(list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),board_list_816,nodes,pv_current,pv_best,description_move,three_moves,br,for_chek_l,null_m)){
  #endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK


// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
list_surplus_moves->current_depth =depth;
//����������� ������ ���������� �����
// ��������� ���� ������� � ������� ������ ��������
Generator_captures(bitboard,list_surplus_moves);
Generator_move(bitboard,list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

//
////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//// �������� ������ ������ � ����� �� ������� ������
//  #if TEST_L
//    List_816_Print(depth,list_surplus_moves);
// //   Board_816_Print( depth,board_list_816);
//  #endif 
////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if HASH
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
  Look_hash(list_surplus_moves,bitboard->hash_key,depth,insert_hash);
 #endif

#if KILLER
// ���� ������� � ����� ������ ����� �� ������ ������� ����� �� ������
  if(insert_hash==0){
    Insert_killer(list_surplus_moves,depth,list_surplus_moves->start_list[depth]);
  }else{
    Insert_killer(list_surplus_moves,depth,list_surplus_moves->start_list[depth]+1);
  }//if(insert_hash==0){
#endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for (int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){

// ��������� ��� �� ������ ������� ������� ���������� �������
 Onward_moves(i,list_surplus_moves,bitboard);
// Bitboard_ini_t(bitboard,board_list_816);

// ���� ������ �� ����� ��
 if (King_check(bitboard)!=0){
// �� �������� ���� ��� ��� ������������
   Backs_moves(i,list_surplus_moves,bitboard);
 }else{
// ������ ���� ��� ���� ��������� ���� � ����������� �������
 no_moves = 1;
 nodes = nodes+1;

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
 Update_PV_current(i,depth,pv_current,list_surplus_moves);

Modification_up_random_key(i,list_surplus_moves,bitboard);
key=bitboard->hash_key;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

 depth_max_z= depth_max;

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if EXTENSION_CHEK
// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
   Extension_for_chek(bitboard,for_chek,depth_max,for_chek_l,depth_max_z);
 #endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

#if !NEGA_SCOUT
// ����������� ����� �������
 value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
 list_surplus_moves->current_depth =depth;
#endif

 //------------------------------------------------------
 //
#if NEGA_SCOUT
 if (i==list_surplus_moves->start_list[depth]){
// ����������� ����� �������
   value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
   list_surplus_moves->current_depth =depth;
 }else{
// ����������� ����� �������
   value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-(alpha-1),-(alpha),depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
// value=-1*Alpha_beta_816(list_surplus_moves,-(alpha),-(alpha+1),depth_max_z,(depth+1),board_list_816,nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
  list_surplus_moves->current_depth =depth;
   if ((value < alpha)&&(value > beta)){
 // ����������� ����� �������
     value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
     list_surplus_moves->current_depth =depth;
   }//if ((value < alpha)&&(value < alpha)){
 }//if (i==list_surplus_moves->start_list[depth]){
#endif
//------------------------------------------------------

// ��������������� �������
 Backs_moves(i,list_surplus_moves,bitboard);

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
 Modification_down_random_key(i,list_surplus_moves,bitboard);

 if (value < value_best){
   value_best = value;
//----------------------------------------------------
//// ���������� ��� ����� ������� � ���� � ������ ��������
//#if KILLER
//// ����� ������� �������
//  Save_killer(i,list_surplus_moves,depth);
//#endif
// #if HASH
//// �������� ������ �� ������ ������ ���
//   Save_best_hash_key(i,list_surplus_moves,bitboard->hash_key,depth);
// #endif
//----------------------------------------------------
// ����� �� �������������� ������ ������� ��������������� ������ ������
   Update_PV_best_point(&pv_best_point,pv_best);
 }//if (value < value_best){

// ���� ALPHA BETA ����������� 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value < alpha){
   alpha = value;
#if KILLER
// ����� ������� �������
  Save_killer(i,list_surplus_moves,depth);
#endif
 #if HASH
// �������� ������ �� ������ ������ ���
   Save_best_hash_key(i,list_surplus_moves,bitboard->hash_key,depth);
 #endif
 }//if (value < alpha){ 

   if(alpha <= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
//--------------------------------------------------------------------
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){

//����������������������������������������������������������������������������������������������
// ��������� ������ ����� �� ���� ��������� �����

// ���� �� ������ ���� �������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (no_moves == 0){
  value=No_moves(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//����������������������������������������������������������������������������������������������

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1 // ����� ������ �������
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 Update_PV_best(pv_best,pv_best_point);
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

return value_best;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
}//int Alpha_beta_816(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// ��������� ������� ���
//(bitboard,depth,depth_max,null_m,description_move,for_chek,list_surplus_moves,alpha,beta,nodes,pv_current,pv_best,three_moves,br)==1)
int Null_move(T_Bitboard * bitboard,int depth,int depth_max,int null_m,int description_move,int for_chek,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int & br){
int depth_max_z=0;
int  value=0;
int  taking_passage = 0;
int  taking_passage_x = 0;
int  taking_passage_y = 0;

if ( (depth>1)&&((depth_max-depth)>3)&&(null_m==0)&&(list_surplus_moves->end_list[depth-1]>10) ){
// ���������� ��������� ��� �������� ���� � ���� �� ���� ����� �.�. ������� �� �������������
  if ((description_move == 1)&&(for_chek==0)){
     //cout << "NULL_MOVE " ;
     //cout <<"\n";
// null move
// ���� �������� ���� � ��� ��� ���� �� ���������� ���� ��� �.�. ��������� ������
// ��� ���� ������ � �� ���� ������� ������ �������� �.�. ������� �� ���� ��������
// �� ������ ���� �� ������� ��� �� ��� ����� ��������� ������ �� ������ �������
// ��� ��������� �� �������� � �������� ��������
    depth_max_z= depth_max-3;

    // ������ ���� ����
   bitboard->colour_move = !bitboard->colour_move;
 if (King_check(bitboard) ==0){
   taking_passage = bitboard->taking_passage_yes;
   taking_passage_x = bitboard->taking_passage_x;
   taking_passage_y = bitboard->taking_passage_y;
   bitboard->taking_passage_yes = 0;

// ����������� ����� �������
    value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,0,three_moves,br,0,1);  

	// ��������������� �������(���� ����)
    bitboard->taking_passage_yes = taking_passage;
    bitboard->taking_passage_x = taking_passage_x;
    bitboard->taking_passage_y = taking_passage_y;
 }//if (King_check(bitboard) ==0){
    bitboard->colour_move = !bitboard->colour_move;

	if (value <= beta) return 1; 
  }// if ((description_move==0)&&(for_chek==0)){
 }//if ((depth==4)&&(depth_max>6)){
    return 0; 
}//int Null_move(){
//	*******************************************************************

//	===================================================================
// ��������� ��������� �� �����
void Extension_for_chek(T_Bitboard * bitboard,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z){
// ��� ���� ��� �� ����������� ������� �� ��� ����� � ��� ����� �����
 if(for_chek==0){
	 for_chek_l =0;
 }else{
     for_chek_l =1;
 }//if(for_chek==0){
// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
// �� �� ����� 20 ���������
bitboard->colour_move = !bitboard->colour_move;
 if(King_check(bitboard)==1){
     if(depth_max<21){
	    depth_max_z=depth_max+1;
        for_chek_l=1;
     }//if(depth_max<21){
 }//if(Check(1,*board_list_816)==1) {
bitboard->colour_move = !bitboard->colour_move;
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************




