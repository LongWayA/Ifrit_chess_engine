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
#include "m_Line.h"
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
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
 ofstream Test_Loop;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// ���������� ����������
int audit=0;// �������� ������ ������� ����������� ������

//	===================================================================
// ���� ��� ��������� ����� �� ��� ��� ��� ���
int No_moves(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current){
bitboard->colour_move = !bitboard->colour_move;
int score_m=0; 
if(King_check(bitboard)!=0){
// ������ ��� ������ ���� ���� ��� ���� �����������
// �� ��� ��� �������� � �� ������� �� ��������
  score_m = 1000000-1000*depth;
}//if(Shah(n_king,*board_list_816 )==1){

// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
Update_PV_best_max_depth(score_m,depth,pv_best,pv_current);
 //cout <<"����� ��� ����� ";
 //cout <<"\n";
bitboard->colour_move = !bitboard->colour_move;
return score_m ;
}//int No_moves(int Depth,T_board_list_816 * board_list_816,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// ���������� ������� �� ������������ �������
int Max_depth(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int description_move,int & br,int for_chek){
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
	if((pv_current->initial_position[0]==three_moves->final_position [2])&&(pv_current->final_position[0]==three_moves->initial_position[2])     &&(three_moves->initial_position[3]==three_moves->final_position[1])&&(three_moves->final_position [3]==three_moves->initial_position [1])){
//    if(0){
      score = 0;
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
      Update_PV_best_max_depth(score,depth,pv_best,pv_current); 
      return score ;
	}else{
// ����������
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// ���������� ������������� �������
// �� ��� ����������� �� ������� ��� ��� ����� ���� � ������� �������� �� ��
// �������� ������ ��� ���� ����� ������
 #if FORCED // �������� ����� ����������
//   score=Forced_variant_816(list_surplus_moves,alpha,beta,depth,board_list_816,nodes,pv_current,pv_best);
//   score=Forced_variant_816(list_surplus_moves,9999999,-9999999,depth,board_list_816,nodes,pv_current,pv_best);

// ����������� ������ �� ������� � ����� ��������� ��������
    bitboard->colour_move == 1 ? inv=-1 : inv=1;
    score = inv*Eval_State(bitboard);
// �������� �� ������� � ����� �������������� ��������
// ������������ (alpha-score),(beta-score) ��� �� ��������� ������������ ��������� ���� ������ ������� ������ ��������
// Bitboard_print(bitboard);
	score_f=Forced_variant_816(bitboard,list_surplus_moves,(alpha-score),(beta-score),depth,pv_current,pv_best);
// ����� ������� �� �������� ��������������� ������ � ������� ����������� ������� �����������
// ����� � �������� ������ ����� ������������� ��������
    score = score + score_f;
    pv_best->score_move=-score;
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
// ���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
//   Update_PV_best_max_depth(score,depth,pv_best,pv_current);
    return score ;
 #endif 
 #if !FORCED // �������� ������ ��� ����������
   bitboard->colour_move == 1 ? inv=-1 : inv=1;
   score = inv*Eval_State(bitboard)+inv*Eval_forced(bitboard);
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
// ������� ������� ������� � ��������
void Print_cout_v(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value){
// ������������� � ������ ������ ���
 cout << "info currmove " ;
 cout <<x_to_notation(list_surplus_moves->initial_position[i]%8);
 cout <<(1 + list_surplus_moves->initial_position[i]/8);
 cout <<x_to_notation(list_surplus_moves->final_position[i]%8);
 cout <<(1 + list_surplus_moves->final_position[i]/8);
// ���������� ����� �������������� ����
 cout << " currmovenumber " ;
 cout << Nodes_0;
 //cout <<"\n";
// �������� ���������� � ������� ���������
 cout << " depth "<<Depth_Max;
 //if(Nodes_0==1){
 //   cout << " seldepth "<<Depth_Max;
 //}else{
 //   cout << " seldepth "<<PV_best_point.depth_max;
 //}//if(Nodes_0==1){
 cout <<"\n";
 cout.flush();
}//Print_cout_v(c

//	===================================================================
// ������� ������� ������� � ��������
void Print_cout(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int nodes_0, int nodes,int i,int Depth_Max,int value,double  time_nodes){
static int mem =-999999993;
static int mem_depth = 0; 
static int z=-1;
/*
// Here are examples of the possible info output: 
< info currmove e8e7 (current calculated move) +
< info currmovenumber 29 +
< info nps 291506 (nodes per sec, Fritz calculates the nps itself)
< info tbhits 830
< info depth 9
< info score cp12 (in centipawns)
< info score mate 5 (or mate -5)
< info nodes 288973
< info time 11192 (milliseconds)
< info pv e2e4 e7e5 g1f3 b8c6 f1d3 f8c5 e1g1 g8e7 b1c3 
*/



// ������������� � ������ ������ ���

// cout << "info currmove " ;
// cout <<x_to_notation(list_surplus_moves->initial_position[i]%16);
// cout <<(8-list_surplus_moves->initial_position[i]/16);
// cout <<x_to_notation(list_surplus_moves->final_position[i]%16);
// cout <<(8-list_surplus_moves->final_position[i]/16);
//// ���������� ����� �������������� ����
// cout << " currmovenumber " ;
// cout << Nodes_0;
// //cout <<"\n";
//// �������� ���������� � ������� ���������
// cout << " depth "<<Depth_Max;

// 
// ������ ���� � ����������� �� �������(������ ��� ��� ���)
z=1;
//if (div(PV_best_point.depth_max,2).rem == 0) z=-1;
//if (div(Depth_Max,2).rem == 0) z=1;
if ( value >= 0) z=-1;

if (mem_depth != Depth_Max) mem =-999999993;
//   if(mem==PV_best_point.score_move) cout <<"\n";
   if(mem==value) cout <<"\n";

// ������� ������� ������ � ��� ������ ���� ������ ������ � ������ � ������
// ������� ���������� ��� �� �� ���� ������ ������ � ���� �� ��� ��� 
#if TEST_PRINT
   if(1){
#else

//   if(mem!=PV_best_point.score_move){
   if(mem!=value){
//   mem =PV_best_point.score_move;
   mem =value;
   mem_depth =Depth_Max;
#endif

 cout << "info " ;
 cout << " nps "<<nodes/time_nodes;
 cout << " depth "<<Depth_Max;
// �������� ���������� � ������� ����������
 cout << " seldepth "<<PV_best_point.depth_max;

// �������� ���������� � ������ �������(��� ������ �� ������ ������ ������ �������� � ������ ���������)
 if(PV_best_point.score_move>900000){
    cout << " score mate "<< z*(PV_best_point.depth_max-1);// ��������� �� ���� � ���������
 }else{
 //   cout << " score cp "<<z*(PV_best_point.score_move);// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
    cout << " score cp "<< -value;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
 }//if(PV_best_point.score_move>900000){
// �������� ���������� � ���������� ������������� �����
 cout << " nodes "<<nodes;

// ���� ������ � ���������� �������� �� ��������� � ������� ���������� �� ��������
// �� ������� �����
//if( value != (-z*PV_best_point.score_move) ) {
//if( value !=(-z*PV_best_point.score_move) ) {
// ��������� �� ������ ������ ��� �� �� ��������� �� ����� ������� �� ������
// �������� �� ��������� ������� ������������ �������
if( (value*value) !=(PV_best_point.score_move*PV_best_point.score_move) ) {
//if(0) {
     cout <<"\n";
	 cout << "ERROR LINE " ;
     cout <<"\n";
     cout << " value = "<<value;
     cout <<"\n";
     cout << " -z*PV_best_point.score_move = "<<-z*PV_best_point.score_move;
     cout << " PV_best_point.score_move = "<<PV_best_point.score_move;
     cout <<"\n";
     cout <<"**********************"<<"\n";
     cout <<"  "<<"\n";
}else{//if( value != (-z*PV_best_point.score_move) ) {


// ����� ����� ��������
 cout <<" pv ";
 for (int n=0;n<PV_best_point.depth_max;n++){
 cout <<x_to_notation(PV_best_point.initial_position[n]%8);
 cout <<(1 + PV_best_point.initial_position[n]/8);
 cout <<x_to_notation(PV_best_point.final_position[n]%8);
 cout <<(1 + PV_best_point.final_position[n]/8); 
 if (PV_best_point.final_pawn[n]!=0) cout <<int_to_char_board(PV_best_point.final_pawn[n],0);
 cout <<" ";
}//for (int n=1;n<(Depth_Max);n++){ 
 }//if(mem!=PV_best_point.score_move){
 cout <<"\n";
}//if( value != (-z*PV_best_point.score_move) ) {
 cout.flush();
}//Print_cout(T_list_surplus_moves list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i){
//	*******************************************************************


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������������ �������,������� ��������,�����,
#if TEST_L
void List_816_Print(int depth,const T_list_surplus_moves * list_surplus_moves){

Test_Loop.open ("_TEST_LOOP.txt",ios::app);	
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop << "\n";
Test_Loop << "------------------------------------------------- "<<"\n";
Test_Loop <<"depth= "<< depth << "\n";
Test_Loop << "\n";
Test_Loop << "������ ��������� ����� ������ "<<"\n";
for (int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){	
     Test_Loop <<"i= "<<i<< "\n";  
     Test_Loop <<"��� ���� "<<list_surplus_moves->description_move[i]<< "\n";
	 Test_Loop <<"��������� ��������� "<<list_surplus_moves->initial_position[i]<< "\n";
	 Test_Loop <<"�������� ��������� "<<list_surplus_moves->final_position[i]<< "\n";
     Test_Loop <<"������ ������ "<<int_to_char_board(list_surplus_moves->name_taking_figure[i],1)<< "\n";
	Test_Loop <<" "<<(x_to_notation(list_surplus_moves->initial_position[i]%8));
Test_Loop <<""<<(1 + list_surplus_moves->initial_position[i]/8);//<< "\n";
 //    Test_Loop<< "\n";
Test_Loop <<" "<<(x_to_notation(list_surplus_moves->final_position[i]%8));
Test_Loop <<""<<(1 + list_surplus_moves->final_position[i]/8)<< "\n";     
	 Test_Loop << "\n";
}//for (i=1,i<list_surplus_moves->amount_moves+1){

Test_Loop <<"list_surplus_moves->start_list= "<<list_surplus_moves->start_list[depth]<< "\n";
Test_Loop <<"list_surplus_moves->end_list= "<<list_surplus_moves->end_list[depth]<< "\n";
Test_Loop << "------------------------------------------------- "<<"\n";
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop.close();
}//void List_816_Print(int Depth,T_list_surplus_moves list_surplus_moves;){
#endif
//	*******************************************************************





