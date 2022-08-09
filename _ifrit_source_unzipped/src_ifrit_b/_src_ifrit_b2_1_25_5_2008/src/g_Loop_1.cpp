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
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"


//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 22.4.2008
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
����� �������� � ����� ����� ���� � ������������ �������
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
// ���������� ����������
int audit=0;// �������� ������ ������� ����������� ������

// ����� �� �����. ����� ��������� ����� ��������
// ������� �� ������� ������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
int White_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // ������� ������ �������
int value_max=-9999999;   // ������ � ����  ������ �������  
int no_moves=0;           // ���� ������������� ��������� ����� (���� 1 �� ��������� ����� �� ����)
int nodes_0=1;            // ���������� ����� �� ������� ������
int key=0;                // ��� ����
int insert_hash=0;        // ��������� �� ��� �� ���� �� ������ �������
double  time_nodes=0;     // ����� �������� ����
struct T_PV pv_best_point;   // ������ ��� ������� ���� �������
int check=0;
int depth_max_z=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
	//-----------------------------------------------
	#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------
//cout << "Rook_amoun_white(bitboard)="<< Rook_amoun_white(bitboard)<<"\n";
//cout << "Rook_amoun_black(bitboard)="<< Rook_amoun_black(bitboard)<<"\n";
//cout << "Bishop_amoun_white(bitboard)="<< Bishop_amoun_white(bitboard)<<"\n";
//cout << "Bishop_amoun_black(bitboard)="<< Bishop_amoun_black(bitboard)<<"\n";

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
 //
//��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
Generator_captures_white(bitboard,&list_surplus_moves);// ���������� ������ � �����������
Generator_move_white(bitboard,&list_surplus_moves);// ���������� ��������� ����
//Bitboard_print(bitboard);// �������� ������ �������� �����
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// �������� ������ ������ � ����� �� ������� ������
	#if TEST_L
		List_816_Print(depth,list_surplus_moves);// �������� ���� ����������
	//Bitboard_print(bitboard);
	#endif 
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if HASH
	// �������� � �����
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���� ������ ������ � ������ �� �� ����� ��� ��� ��� �� ����
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	// ����� ������� �������
	#if KILLER
	// ���� ������� � ����� ������ ����� �� ������ ������� ����� �� ������
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for ( int i=0;i<list_surplus_moves.end_list;i++){

// ��������� ��� �� ������ ��� ������� ������� ���������� �������
 Onward_moves_white(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

// ���� ������ �� ����� ��
 if (King_white_check(bitboard)!=0){
// �� �������� ���� ��� ��� ������������
   Backs_moves_white(i,undo,&list_surplus_moves,bitboard);
 }else{
// ������ ���� ��� ���� ��������� ���� � ����������� �������
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// �������� ���� � ��������������� ���� � ������� ��������
	Print_cout_v(&list_surplus_moves,nodes_0,i,depth_max);
	// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);
	// ��������� ��� ����
	Modification_up_random_key(1,i,&list_surplus_moves,bitboard);
	// ���������� ���� ��� �������� ��� ����������� � �������
	key=bitboard->hash_key;

	Timer_start();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
	Extension_for_chek_white(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------
// ����������� ����� �������
value=Black(check,bitboard,alpha,beta,depth_max,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,0);
 

	//-----------------------------------------------
	#if SAVE_LINE
	Timer_stop();
	time_nodes= Timer_delta();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	// �������� ��� ����� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
	// ��������� ������� �� ���� �� ���������� �� �������
	#if HASH
	if(key!=bitboard->hash_key){
		cout <<"\n";
		cout << "ERROR HESH " ;
		cout <<"\n";
	}//if(key!=board_list_816->hash_key){
	#endif 
	//-----------------------------------------------

// ��������������� �������
 Backs_moves_white(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	// ��������������� ��� ����
	Modification_down_random_key(1,i,&list_surplus_moves,bitboard);
	//Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max);
    //-----------------------------------------------


 // ���� ������ ������� �� ����� ���� ��������
 if (value > value_max){
   value_max = value;
   from=((list_surplus_moves.move[i]>>12)& 63);
   to=((list_surplus_moves.move[i]>>18)& 63);
// ����� �� �������������� ������ ������� ��������������� ������ ������
   Update_PV_best_point(&pv_best_point,pv_best);
 #if HASH
// �������� ������ �� ������ ������ ���
   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
 #endif
   //Test_Loop << "bitboard->hash_key="<<bitboard->hash_key << "\n";
  // Bitboard_print(bitboard);
 }//if (value < value_best){ 

// ���� ALPHA BETA ����������� 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// ����� �������� � ������ � �������� �� ��������
 #if ALPHA_BETA_ON
 if (value > alpha){
   alpha = value;
 }//if (value < alpha){ 
// ���� ����� ��� ������ ���� �� ����� �� ���������� 
// ���� ������ ��� ����� ������ ����� � �� ����� ��� ��� ��� ������ ����� ��������
// ��� ��� ���������� ���� ������ �� �����
//   if(alpha >= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

	//--------------------------------------------------------------------
	// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� �.�. � ����� �����������
	if (br!=1){
	  nodes_0 = nodes_0+1;
	  #if TEST_PRINT
	  // ��� ����� ���������
	  Print_cout(1,bitboard,&list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value,time_nodes);
	  #else
	  // ��� �������� ������
	  Print_cout(1,bitboard,&list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max,value_max,time_nodes);
	  #endif
    }//if (br!=1){
    //-----------------------------------------------
    #endif //#if !COMPILING_1
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves.amount_moves;i++){
//3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333

//����444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
// ��������� ������ ����� �� ���� ��������� �����

// ���� �� ������ ���� �������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (no_moves == 0){
  value=No_moves_white(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if SAVE_LINE // ����� ������ �������
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 Update_PV_best(pv_best,pv_best_point);
//Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max);
// cout <<"alpha= "<<alpha;
// cout <<"\n";
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
//Print_name_from_coord(bitboard);
return value_max;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

}//int White_root(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

// ������� �� ������� ������ �� ������ ���� �������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
int Black_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // ������� ������ �������
int value_min=9999999;   // ������ � ����  ������ �������  
int no_moves=0;           // ���� ������������� ��������� ����� (���� 1 �� ��������� ����� �� ����)
int nodes_0=1;            // ���������� ����� �� ������� ������
int key=0;                // ��� ����
int insert_hash=0;        // ��������� �� ��� �� ���� �� ������ �������
double  time_nodes=0;     // ����� �������� ����
struct T_PV pv_best_point;   // ������ ��� ������� ���� �������
int check=0;
int depth_max_z=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
	//-----------------------------------------------
	#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
 //
//��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
Generator_captures_black(bitboard,&list_surplus_moves);// ���������� ������ � �����������
Generator_move_black(bitboard,&list_surplus_moves);// ���������� ��������� ����
//Bitboard_print(bitboard);// �������� ������ �������� �����
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// �������� ������ ������ � ����� �� ������� ������
	#if TEST_L
		List_816_Print(depth,list_surplus_moves);// �������� ���� ����������
	//Bitboard_print(bitboard);
	#endif 
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if HASH
	// �������� � �����
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���� ������ ������ � ������ �� �� ����� ��� ��� ��� �� ����
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	// ����� ������� �������
	#if KILLER
	// ���� ������� � ����� ������ ����� �� ������ ������� ����� �� ������
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for ( int i=0;i<list_surplus_moves.end_list;i++){

// ��������� ��� �� ������ ��� ������� ������� ���������� �������
 Onward_moves_black(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

// ���� ������ �� ����� ��
 if (King_black_check(bitboard)!=0){
// �� �������� ���� ��� ��� ������������
   Backs_moves_black(i,undo,&list_surplus_moves,bitboard);
 }else{
// ������ ���� ��� ���� ��������� ���� � ����������� �������
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// �������� ���� � ��������������� ���� � ������� ��������
	Print_cout_v(&list_surplus_moves,nodes_0,i,depth_max);
	// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);
	// ��������� ��� ����
	Modification_up_random_key(0,i,&list_surplus_moves,bitboard);
	// ���������� ���� ��� �������� ��� ����������� � �������
	key=bitboard->hash_key;

	Timer_start();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
	Extension_for_chek_black(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------

// ����������� ����� �������
value=White(check,bitboard,alpha,beta,depth_max,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,0);
 

	//-----------------------------------------------
	#if SAVE_LINE
	Timer_stop();
	time_nodes= Timer_delta();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	// �������� ��� ����� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
	// ��������� ������� �� ���� �� ���������� �� �������
	#if HASH
	if(key!=bitboard->hash_key){
		cout <<"\n";
		cout << "ERROR HESH " ;
		cout <<"\n";
	}//if(key!=board_list_816->hash_key){
	#endif 
	//-----------------------------------------------

// ��������������� �������
 Backs_moves_black(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	// ��������������� ��� ����
	Modification_down_random_key(0,i,&list_surplus_moves,bitboard);
	//Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max);
    //-----------------------------------------------


 // ���� ������ ������� �� ������ ���� ������
 if (value < value_min){
   value_min = value;
   from=((list_surplus_moves.move[i]>>12)& 63);
   to=((list_surplus_moves.move[i]>>18)& 63);
// ����� �� �������������� ������ ������� ��������������� ������ ������
   Update_PV_best_point(&pv_best_point,pv_best);
 #if HASH
// �������� ������ �� ������ ������ ���
   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
 #endif
   //Test_Loop << "bitboard->hash_key="<<bitboard->hash_key << "\n";
  // Bitboard_print(bitboard);
 }//if (value < value_best){ 

// ���� ALPHA BETA ����������� 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
// ������ �������� �������� ����
 if (value < beta){
   beta = value;
 }//if (value < alpha){ 
// ���� ���� ������ ����� �� ������ ��� ����� ������ ����������� ��� ���
// ���� ����� ����������
//   if(alpha >= beta) return beta; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

	//--------------------------------------------------------------------
	// ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� �.�. � ����� �����������
	if (br!=1){
	  nodes_0 = nodes_0+1;
	  #if TEST_PRINT
	  // ��� ����� ���������
	  Print_cout(0,bitboard,&list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value,time_nodes);
	  #else
	  // ��� �������� ������
	  Print_cout(0,bitboard,&list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max,value_min,time_nodes);
	  #endif
    }//if (br!=1){
    //-----------------------------------------------
    #endif //#if !COMPILING_1
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves.amount_moves;i++){
//3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333

//����444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
// ��������� ������ ����� �� ���� ��������� �����

// ���� �� ������ ���� �������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (no_moves == 0){
  value=No_moves_black(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if SAVE_LINE // ����� ������ �������
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 Update_PV_best(pv_best,pv_best_point);
//Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max);
// cout <<"alpha= "<<alpha;
// cout <<"\n";
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

return value_min;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

}//int Black_root(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************


//	===================================================================
// ���� �������� � ����. ������� �� ����� 
int White(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // ������� ������ �������
int value_max=-9999999;   // ������ ������� ������ � ���� 
int no_moves=0;           // ���� ������������� ��������� �����
int depth_max_z=0;        // ���������� �� �����
int key=0;                // ��� ����
int insert_hash=0;        // ��������� �� ��� �� ���� �� ������ �������
struct T_PV pv_best_point;   // ������ ��� ������� ���� �������
int taking_passage_mem=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)

	//-----------------------------------------------
	#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------

//���������������������������������������������������������������������������������������������
// �� ���������� ������� ���������� ����������� ������ �������
if (depth == depth_max){
  #if SAVE_LINE
    value=Max_depth_white(bitboard,alpha,beta,depth_max,depth,nodes,pv_current,pv_best,three_moves,description_move,br);
//  value=0;
  #endif 
 return value ;
}//	if (Depth == Depth_Max){
//���������������������������������������������������������������������������������������������

	#if NULL_MOVE
// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
// ������� ���� ��� ����� ����� ���������
// >3 ������ ��� ����� �������� ������� �� �������� ��� ������� ����
// ������� ��� �������� �������� ��� ������������ ������� 6 ������� � ������� 2 (6-2=4>3)
if ((null_m==0)&&((depth_max-depth) > 3 )&&(description_move != 2)&&(check == 0)){
//if ((null_m==0)&&((depth_max-depth) > 3 )&&(check == 0)){
   taking_passage_mem = bitboard->taking_passage_yes;
   bitboard->taking_passage_yes = 0;
   null_m=1;// ��� ���� ����� � ����� ����� �� ��������
   depth_max_z= depth_max;// �������� ������� � ��� 3
   value=Black(check,bitboard,alpha,beta,depth_max_z,depth+4,nodes,pv_current,pv_best,((list_surplus_moves.move[1]>>6)& 63),three_moves,br,null_m); 
//   value=Black(bitboard,list_surplus_moves,(alpha),(alpha+1),depth_max_z,depth,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m); 
//   value=Black(bitboard,list_surplus_moves,(beta-1),(beta),depth_max_z,depth,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m); 
   bitboard->taking_passage_yes = taking_passage_mem;
//��� ��� ������ ��� ������� ��� �������� ����� ����������� ������ �.�. �������� �����
//������ ����� ��� ������ � ���� �� �� ������ �������� ����� �.�. ���� ������ ������
//��� ����� ����� ������ ����� ������ �.�. ����� ��� �� ���������
       if(value>=beta) return beta ;
   null_m=0;
}//if ((null_m==0)&&((depth_max-depth) > 2 )){
    #endif



// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
//����������� ������ ���������� �����
// ��������� ���� ������� � ������� ������ ��������
Generator_captures_white(bitboard,&list_surplus_moves);
Generator_move_white(bitboard,&list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
	//-----------------------------------------------
	#if HASH
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif

	#if KILLER
	// ���� ������� � ����� ������ ����� �� ������ ������� ����� �� ������
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for (int i=0;i<list_surplus_moves.end_list;i++){

// ��������� ��� �� ������ ������� ������� ���������� �������
 Onward_moves_white(i,undo,&list_surplus_moves,bitboard);
// Bitboard_ini_t(bitboard,board_list_816);

// ���� ������ �� ����� ��
 if (King_white_check(bitboard)!=0){
// if (0){
// �� �������� ���� ��� ��� ������������
   Backs_moves_white(i,undo,&list_surplus_moves,bitboard);
 }else{
// ������ ���� ��� ���� ��������� ���� � ����������� �������
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);

	Modification_up_random_key(1,i,&list_surplus_moves,bitboard);
	key=bitboard->hash_key;
	#endif 
	//-----------------------------------------------

	depth_max_z= depth_max;

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
	Extension_for_chek_white(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CAPTURES
       Extension_for_captures(i,0,((list_surplus_moves.move[i]>>6)& 63),depth_max,depth_max_z,depth);
    #endif
	//-----------------------------------------------

#if !NEGA_SCOUT
// ����������� ����� �������
 value=Black(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
#endif

	//------------------------------------------------------
	//
	#if NEGA_SCOUT
	if (i==0){
	// ����������� ����� �������
	value=Black(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}else{
	// ����������� ����� �������
	value=Black(check,bitboard,(alpha),(alpha+1),depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	if ((value > alpha)&&(value < beta)){
//	if (value > alpha){
	// ����������� ����� �������
		value=Black(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}//if ((value < alpha)&&(value < alpha)){
	}//if (i==list_surplus_moves->start_list[depth]){
	#endif
	//------------------------------------------------------

// ��������������� �������
 Backs_moves_white(i,undo,&list_surplus_moves,bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	Modification_down_random_key(1,i,&list_surplus_moves,bitboard);
    //-----------------------------------------------

 if (value > value_max){
   value_max = value;

//TEST----------------------------------------------------
// ���������� ��� ����� ������� � ���� � ������ ��������
//#if KILLER
//// ����� ������� �������
//  Save_killer(i,&list_surplus_moves,depth);
//#endif
// #if HASH
//// �������� ������ �� ������ ������ ���
//   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
// #endif
//----------------------------------------------------

// ����� �� �������������� ������ ������� ��������������� ������ ������
   Update_PV_best_point(&pv_best_point,pv_best);
 }//if (value < value_best){

// ���� ALPHA BETA ����������� 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value > alpha){
   alpha = value;
//-----------------------------------------------
	#if KILLER
	// ����� ������� �������
	Save_killer(i,&list_surplus_moves,depth);
	#endif
	#if HASH
	// �������� ������ �� ������ ������ ���
	Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
	#endif
//-----------------------------------------------
 }//if (value < alpha){ 

   if(alpha >= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
#endif //#if !COMPILING_1

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){

//����������������������������������������������������������������������������������������������
// ��������� ������ ����� �� ���� ��������� �����
// ���� �� ������ ���� �������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (no_moves == 0){
  value=No_moves_white(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//����������������������������������������������������������������������������������������������

//-----------------------------------------------
#if SAVE_LINE // ����� ������ �������
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 Update_PV_best(pv_best,pv_best_point);
#endif 
//-----------------------------------------------

return value_max;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
}//int White(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************


//	===================================================================
// ���� ������ � ����. ������� �� ������
int Black(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // ������� ������ �������
int value_min=9999999;   // ������ ������� ������ � ���� 
int no_moves=0;           // ���� ������������� ��������� �����
int depth_max_z=0;        // ���������� �� �����
int key=0;                // ��� ����
int insert_hash=0;        // ��������� �� ��� �� ���� �� ������ �������
struct T_PV pv_best_point;   // ������ ��� ������� ���� �������
int taking_passage_mem=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
	//-----------------------------------------------
	#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------

//���������������������������������������������������������������������������������������������
// �� ���������� ������� ���������� ����������� ������ �������
if (depth == depth_max){
  #if SAVE_LINE
    value=Max_depth_black(bitboard,alpha,beta,depth_max,depth,nodes,pv_current,pv_best,three_moves,description_move,br);
//  value=0;
  #endif 
 return value ;
}//	if (Depth == Depth_Max){
//���������������������������������������������������������������������������������������������


	#if NULL_MOVE
// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
// ������� ���� ��� ����� ����� ���������
if ((null_m==0)&&( (depth_max-depth)>3)&&(description_move != 2)&&(check == 0)){
//if ((null_m==0)&&((depth_max-depth) > 3 )&&(check == 0)){
   taking_passage_mem = bitboard->taking_passage_yes;
   bitboard->taking_passage_yes = 0;
   null_m=1;// ��� ���� ����� � ����� ����� �� ��������
   depth_max_z= depth_max; //
   value=White(check,bitboard,alpha,beta,depth_max_z,depth+4,nodes,pv_current,pv_best,((list_surplus_moves.move[1]>>6)& 63),three_moves,br,null_m);
//   value=White(bitboard,list_surplus_moves,(beta-1),(beta),depth_max_z,depth+3,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m);
//   value=White(bitboard,list_surplus_moves,(alpha),(alpha+1),depth_max_z,depth+3,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m);
   
   bitboard->taking_passage_yes = taking_passage_mem;
//��� ��� ����� ��� ������� ��� �������� ����� ������������ ������ �.�. �������� �����
//������ ����� ��� ����� � ���� �� �� ������ �������� ����� �.�. ���� ������ ������
//��� ����� ����� ������ ����� ������ �.�. ����� ��� �� ���������
        if(value<=alpha) return alpha ;
   null_m=0;
}//

    #endif

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
//����������� ������ ���������� �����
// ��������� ���� ������� � ������� ������ ��������
Generator_captures_black(bitboard,&list_surplus_moves);
Generator_move_black(bitboard,&list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
	//-----------------------------------------------
	#if HASH
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif

	#if KILLER
	// ���� ������� � ����� ������ ����� �� ������ ������� ����� �� ������
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ���� ������� �������� 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for (int i=0;i<list_surplus_moves.end_list;i++){

// ��������� ��� �� ������ ������� ������� ���������� �������
 Onward_moves_black(i,undo,&list_surplus_moves,bitboard);
// Bitboard_ini_t(bitboard,board_list_816);

// ���� ������ �� ����� ��
 if (King_black_check(bitboard)!=0){
// if (0){
// �� �������� ���� ��� ��� ������������
   Backs_moves_black(i,undo,&list_surplus_moves,bitboard);
 }else{
// ������ ���� ��� ���� ��������� ���� � ����������� �������
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);

	Modification_up_random_key(0,i,&list_surplus_moves,bitboard);
	key=bitboard->hash_key;
	#endif 
	//-----------------------------------------------

	depth_max_z= depth_max;

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
	Extension_for_chek_black(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CAPTURES
       Extension_for_captures(i,0,((list_surplus_moves.move[i]>>6)& 63),depth_max,depth_max_z,depth);
    #endif
	//-----------------------------------------------


#if !NEGA_SCOUT
// ����������� ����� �������
 value=White(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
#endif

	//------------------------------------------------------
	//
	#if NEGA_SCOUT
	if (i==0){
	// ����������� ����� �������
	value=White(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}else{
	// ����������� ����� �������
	value=White(check,bitboard,(beta-1),(beta),depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
    if ((value > alpha)&&(value < beta)){
//	if (value < beta){
	// ����������� ����� �������
		value=White(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}//if ((value < alpha)&&(value < alpha)){
	}//if (i==list_surplus_moves->start_list[depth]){
	#endif
	//------------------------------------------------------

// ��������������� �������
 Backs_moves_black(i,undo,&list_surplus_moves,bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	Modification_down_random_key(0,i,&list_surplus_moves,bitboard);
    //-----------------------------------------------

 if (value < value_min){
   value_min = value;

//TEST----------------------------------------------------
// ���������� ��� ����� ������� � ���� � ������ ��������
//#if KILLER
//// ����� ������� �������
//  Save_killer(i,&list_surplus_moves,depth);
//#endif
// #if HASH
//// �������� ������ �� ������ ������ ���
//   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
// #endif
//----------------------------------------------------

// ����� �� �������������� ������ ������� ��������������� ������ ������
   Update_PV_best_point(&pv_best_point,pv_best);
 }//if (value < value_best){

// ���� ALPHA BETA ����������� 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value < beta){
   beta = value;
//-----------------------------------------------
	#if KILLER
	// ����� ������� �������
	Save_killer(i,&list_surplus_moves,depth);
	#endif
	#if HASH
	// �������� ������ �� ������ ������ ���
	Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
	#endif
//-----------------------------------------------
 }//if (value < alpha){ 

   if(alpha >= beta) return beta; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
#endif //#if !COMPILING_1

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){

//����������������������������������������������������������������������������������������������
// ��������� ������ ����� �� ���� ��������� �����
// ���� �� ������ ���� �������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (no_moves == 0){
  value=No_moves_black(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//����������������������������������������������������������������������������������������������

//-----------------------------------------------
#if SAVE_LINE // ����� ������ �������
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 Update_PV_best(pv_best,pv_best_point);
#endif 
//-----------------------------------------------

return value_min;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
}//int Black(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// ���� ��� ��������� ����� �� ��� ��� ��� ���. �� �����
inline int No_moves_white(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current){
int score_m=0; 
if(King_white_check(bitboard)!=0){
  score_m = -(1000000-1000*depth);
}//if(Shah(n_king,*board_list_816 )==1){

// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
Update_PV_best_max_depth(score_m,depth,pv_best,pv_current);
 //cout <<"����� ��� ����� ";
 //cout <<"\n";
return score_m ;
}//int No_moves(int Depth,T_board_list_816 * board_list_816,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// ���� ��� ��������� ����� �� ��� ��� ��� ���. �� ������
inline int No_moves_black(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current){
int score_m=0; 
if(King_black_check(bitboard)!=0){
  score_m = (1000000-1000*depth);
}//if(Shah(n_king,*board_list_816 )==1){

// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
Update_PV_best_max_depth(score_m,depth,pv_best,pv_current);
 //cout <<"����� ��� ����� ";
 //cout <<"\n";
return score_m ;
}//int No_moves(int Depth,T_board_list_816 * board_list_816,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// ���������� ������� �� ������������ �������. �� �����
inline int Max_depth_white(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_three_moves * three_moves,int description_move,int & br){
int score=0;
int score_f=0;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// ��������� ������ ����������� ���������� ����� 
if (audit>50000){
//if (0){
//���� ��������� ������� �� ��� ��� quit ��� stop
 if (Bioskey()){
// if (0){
char string_in2[65536];
cin.getline(string_in2,65536,'\n');
 if (strcmp(string_in2,"quit")== 0) exit(1);
 if ((strcmp(string_in2,"stop")== 0)){
// ������ ���
  br=1;
  }//if ((strcmp(string_in2,"stop")== 0)){
 }//if (Bioskey()){
 audit=0;
}//if (audit>100000){
audit=audit+1;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//------------------------------------------------------
// ���������� ���������� �����
// ������� ��� ������������� ������� �� ����� ���� ������
// (0)b1c3 (1)... (2)c3b1 (3)...(4)b1c3 (5)..
// ������������ ���������� ��������� ���� ����� � ��� ��������� ����� �.�. ����� ������� ��� ������ � ������                                                                                                                                                                                        ������������ ���������� ���� �������
//	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(pv_current->initial_position[0]==three_moves->final_position [2])&&(pv_current->final_position[0]==three_moves->initial_position[2])){
	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(((pv_current->move[0]>>12)& 63)==three_moves->final_position [2])&&(((pv_current->move[0]>>18)& 63)==three_moves->initial_position[2])){
//		if(0){
        score = 0;
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
        Update_PV_best_max_depth(score,depth,pv_best,pv_current); 
        return score ;
	}else{
// ����������
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// ���������� ������������� �������
// �� ��� ����������� �� ������� ��� ��� ����� ���� � ������� �������� �� ��
// �������� ������ ��� ���� ����� ������
 #if FORCED // �������� ����� ����������

// ����������� ������ �� ������� � ����� ��������� ��������
    score = Eval_State(bitboard);
//    score = 0;
// �������� �� ������� � ����� �������������� ��������
// ������������ (alpha-score),(beta-score) ��� �� ��������� ������������ ��������� ���� ������ ������� ������ ��������
// Bitboard_print(bitboard);
	score_f=Forced_variant_white(bitboard,(alpha-score),(beta-score),depth,pv_current,pv_best);
// ����� ������� �� �������� ��������������� ������ � ������� ����������� ������� �����������
// ����� � �������� ������ ����� ������������� ��������
    score = score + score_f;
    pv_best->score_move= score;
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
// ���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
//   Update_PV_best_max_depth(score,depth,pv_best,pv_current);
    return score ;
 #endif 

 #if !FORCED // �������� ������ ��� ����������
// ����������� ������ �� ������� � ����� ��������� ��������
 //   bitboard->colour_move == 1 ? inv=-1 : inv=1;
 //score = Eval_forced(bitboard);
   score = Eval_State(bitboard)+Eval_forced(bitboard);
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
// ���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
   Update_PV_best_max_depth(score,depth,pv_best,pv_current);

// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
// ������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
// �����  ������ ���� � ����
//   Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
   return score ;
 #endif 
// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
 }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

//// 0 ��������� ������ �������
//// ��� ����������� ������� ������ ���������� ������ �������. ��� ������ ��� ���
//// ������������ ����� ������ ������������ � ��� ������������ ������ ������ ������������
//// ���� ��� ����� �� ������ ����������� ������ ��� �� ��������� ��� ��������� �������.
//// ��� ��� �������� ������ �������� ������ ��� ����� �����
//// � ������ �������� ��� ����� ������ ��� �� ���������� ��������
//      board_list_816->colour_move == 1 ? inv=-1 : inv=1;
//      score = inv*Eval_State_816(*board_list_816);
//// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
//      Update_PV_best_max_depth(score,Depth,PV_best,PV_current); 
//      return score ;
//    }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

}//int Max_depth(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_current,T_PV * PV_best,T_PV * three_moves){
//	*******************************************************************

//	===================================================================
// ���������� ������� �� ������������ �������. �� ������
inline int Max_depth_black(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_three_moves * three_moves,int description_move,int & br){
//int score_m=0;
int score=0;
int score_f=0;
int inv=0;          // �������������� ������ �������
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// ��������� ������ ����������� ���������� ����� 
if (audit>50000){
//if (0){
//���� ��������� ������� �� ��� ��� quit ��� stop
 if (Bioskey()){
// if (0){
char string_in2[65536];
cin.getline(string_in2,65536,'\n');
 if (strcmp(string_in2,"quit")== 0) exit(1);
 if ((strcmp(string_in2,"stop")== 0)){
// ������ ���
  br=1;
  }//if ((strcmp(string_in2,"stop")== 0)){
 }//if (Bioskey()){
 audit=0;
}//if (audit>100000){
audit=audit+1;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//------------------------------------------------------
// ���������� ���������� �����
// ������� ��� ������������� ������� �� ����� ���� ������
// (0)b1c3 (1)... (2)c3b1 (3)...(4)b1c3 (5)..
// ������������ ���������� ��������� ���� ����� � ��� ��������� ����� �.�. ����� ������� ��� ������ � ������                                                                                                                                                                                        ������������ ���������� ���� �������
//	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(pv_current->initial_position[0]==three_moves->final_position [2])&&(pv_current->final_position[0]==three_moves->initial_position[2])){
	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(((pv_current->move[0]>>12)& 63)==three_moves->final_position [2])&&(((pv_current->move[0]>>18)& 63)==three_moves->initial_position[2])){
//  if(0){
        score = 0;
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
        Update_PV_best_max_depth(score,depth,pv_best,pv_current); 
        return score ;
	}else{
// ����������
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// ���������� ������������� �������
// �� ��� ����������� �� ������� ��� ��� ����� ���� � ������� �������� �� ��
// �������� ������ ��� ���� ����� ������
 #if FORCED // �������� ����� ����������
//   score=Forced_variant_816(list_surplus_moves,alpha,beta,depth,board_list_816,nodes,pv_current,pv_best);
//   score=Forced_variant_816(list_surplus_moves,9999999,-9999999,depth,board_list_816,nodes,pv_current,pv_best);

// ����������� ������ �� ������� � ����� ��������� ��������
    score = Eval_State(bitboard);
//    score = 0;
// �������� �� ������� � ����� �������������� ��������
// ������������ (alpha-score),(beta-score) ��� �� ��������� ������������ ��������� ���� ������ ������� ������ ��������
// Bitboard_print(bitboard);
	score_f=Forced_variant_black(bitboard,(alpha-score),(beta-score),depth,pv_current,pv_best);
// ����� ������� �� �������� ��������������� ������ � ������� ����������� ������� �����������
// ����� � �������� ������ ����� ������������� ��������
    score = score + score_f;
    pv_best->score_move= score;
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
// ���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
//   Update_PV_best_max_depth(score,depth,pv_best,pv_current);
    return score ;
 #endif 

 #if !FORCED // �������� ������ ��� ����������
// ����������� ������ �� ������� � ����� ��������� ��������
 //   bitboard->colour_move == 1 ? inv=-1 : inv=1;
 //score = Eval_forced(bitboard);
   score = Eval_State(bitboard)+Eval_forced(bitboard);
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
// ���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
   Update_PV_best_max_depth(score,depth,pv_best,pv_current);

// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
// ������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
// �����  ������ ���� � ����
//   Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
   return score ;
 #endif 
// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
 }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

//// 0 ��������� ������ �������
//// ��� ����������� ������� ������ ���������� ������ �������. ��� ������ ��� ���
//// ������������ ����� ������ ������������ � ��� ������������ ������ ������ ������������
//// ���� ��� ����� �� ������ ����������� ������ ��� �� ��������� ��� ��������� �������.
//// ��� ��� �������� ������ �������� ������ ��� ����� �����
//// � ������ �������� ��� ����� ������ ��� �� ���������� ��������
//      board_list_816->colour_move == 1 ? inv=-1 : inv=1;
//      score = inv*Eval_State_816(*board_list_816);
//// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
//      Update_PV_best_max_depth(score,Depth,PV_best,PV_current); 
//      return score ;
//    }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

}//int Max_depth(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_current,T_PV * PV_best,T_PV * three_moves){
//	*******************************************************************

//	===================================================================
// ��������� ��������� �� �����
inline void Extension_for_chek_white(struct T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check){

// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
// �� �� ����� 20 ���������
check=0;
 if(King_black_check(bitboard)!=0){
 check=1;
     if(depth_max<21){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(King_check(bitboard)==1){
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************

//	===================================================================
// ��������� ��������� �� �����
inline void Extension_for_chek_black(struct T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check){

// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
// �� �� ����� 20 ���������
check=0;
 if(King_white_check(bitboard)!=0){
     check=1;
     if(depth_max<21){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(King_check(bitboard)==1){
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************

//	===================================================================
// ��������� ��������� �� �������
inline void Extension_for_captures(int i,int start,int description_move,int depth_max,int & depth_max_new,int depth){

// ���� ���� ������ �� ����������� ������������ ������� �������� ��� ���� �����
// �� �� ����� 10 ���������
 if(description_move == 2){
// if(i == start){
     if(depth < 4){
 //    if((depth<5)&&(depth_max<15)){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(description_move == 2){
}//void Extension_for_captures(int description_move,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z){
//	*******************************************************************



//	===================================================================
void Print_name_from_coord(struct T_Bitboard * bitboard){

  for (int i=0;i<8;i++){
       for (int j=0;j<8;j++){
           cout << bitboard->white_name_from_coord[(7-i)*8 + j];
	   }//
       cout << "\n";
  }//
cout << "\n";
  for (int i=0;i<8;i++){
       for (int j=0;j<8;j++){
           cout << bitboard->black_name_from_coord[(7-i)*8 + j];
	   }//
       cout << "\n";
  }//

  //for (int i=0;i<64;i++){
  //   cout << bitboard->white_name_from_coord[i];
  //   if (i%8 ==7) cout << "\n";
  //}

}//void Print_name_from_coord(struct T_Bitboard * bitboard){


//	*******************************************************************



