#include <fstream.h>  // TEST

#include "g_Loop.h"
#include "c_Iterative_Deepening.h"
#include "k_Util.h"
#include "k_Timer.h"
//-----------------------------------------------------------------------------------
/* ? ����� ����������� ��� ����������
 * ������� � ����������� ����������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 10.02.2008
 *
 *  ������������
 * ������ ���� ��� ��� ���� ����� ����� ��������
*/
//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// �� �������� ������ �����, ������������ �������,������ �������� �����
void Deepening(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,short int depth,char three_position[],T_PV three_moves,int & nodes,double wtime,double btime, double winc,double binc,int movestogo,double movetime,bool infinite){


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
 //   Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
T_PV PV_current;// ������� �����
T_PV PV_best;   // ������ �����

int Alpha=9999999;
int Beta=-9999999;
short int br =0;// ���������� ��� ������� �������� ��� ���������� ������

// ����� ������� � ��������
double time_terminate= -1.0;// ����� ���������� �� ���
double time_current= -1.0;  // ������� ������� ������ ��� ������ ��������
int value_root=0;           // ������ ������� �������� � ��������� ����. 
//> go btime 300000 wtime 300000 
// 5 min * 60 = 300 sek
// ����� � ��� �������� � �������� ���� �� �������� � ������������ �� ��
// ��� ������ �������� �� 1000


// �������� ����� �� ���(���� ������� ��� �����������). 
// ������ ������������� �� ����������. � ������ ���� ����� � ����� ������
// ������� �� 40 ���� ���� �� ���������� � ���� � ������ ���� ���������
// ��������� �� � ����� ��� ����� ������
if (bitboard->colour_move == 1){// 1-��� ����� 0- ��� ������
  if (wtime != -1) time_terminate= wtime/100;
}else{
  if (btime != -1) time_terminate= btime/100;
}//if (board_list_816->colour_move == 1){

// ������ ����� �� ��� (���� ������� ��� �����������).
 if (movetime != -1) time_terminate= movetime;

//  
if (depth == -1) depth=6;

// ���� ����� ��������� ���� ����������� ��
// ����������� �� ������� 20 �� ���� ��������� ��� ��������� :)
if (time_terminate!=-1) depth=20;

// ������ ��� ����������� ������� 100 ��������� ����� ���������� :)
if (infinite==true) depth=100;

//time_current=0;

//Print_hash_moves();
//Test_Deepening << "board_list_816->hash_key="<<board_list_816->hash_key << "\n";
//Test_Deepening << "========================================= "<< "\n";
//Test_Deepening << "wtime= "<<wtime << "\n";
//Test_Deepening << "btime= "<<btime << "\n";
//Test_Deepening << "movetime= "<<movetime << "\n";
//Test_Deepening << "time_terminate= "<<time_terminate << "\n";
nodes=0;
int static nodes_y = 0;
//-----------------------------------------------------
for (int d=1;((d<(depth+1))&&(br==0));d++){
  Timer_start_d();
  nodes_y = nodes;
  nodes=0;
  value_root=-Alpha_beta_root(bitboard,list_surplus_moves,-Beta,-Alpha,d,0,nodes,&PV_current,&PV_best,0,&three_moves,br,0,0);
 
  nodes_y = nodes - nodes_y ;
  Timer_stop_d();
  time_current= Timer_delta_d();
//---------------------------------------------------------------
 cout << "info " ;
 cout << " nps "<<nodes/time_current;
 cout << " depth "<<d;
 if(value_root>900000){
    cout << " score mate "<< (PV_best.depth_max - 1);// ��������� �� ���� � ���������
 }else if(value_root<-900000){
   cout << " score mate "<< -(PV_best.depth_max - 1);// ��������� �� ���� � ���������
 }else{
    cout << " score cp "<< value_root;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
 }//if(value_root>900000){
 
// cout << " nodes "<<nodes;
 cout << " nodes "<< nodes_y;
 cout <<" pv ";
 for (short int n=0;n<PV_best.depth_max;n++){
 cout <<x_to_notation(PV_best.initial_position[n]%8);
 cout <<(1 + PV_best.initial_position[n]/8);
 cout <<x_to_notation(PV_best.final_position[n]%8);
 cout <<(1 + PV_best.final_position[n]/8); 
 if (PV_best.final_pawn[n]!=0) cout <<int_to_char_board(PV_best.final_pawn[n],0);
 cout <<" ";
}//for (int n=1;n<(Depth_Max);n++){ 
 cout <<"\n";
//---------------------------------------------------------------
//Test_Deepening << "d= "<<d << "\n";
//Test_Deepening << "time_current= "<<time_current << "\n";
  if((time_terminate!=-1)&&(time_terminate<time_current)) break;
}//for (int d=1;d<(Depth_Max+1);d++){

//Print_hash_moves();
//Test_Deepening << "board_list_816->hash_key="<<board_list_816->hash_key << "\n";
//---------------------------------------------------------------------
// ������ ��� ���� < bestmove e2e4 
// ��� ��� ����������� � ����(� �������)  < bestmove d7d8N 
 cout <<"bestmove " <<x_to_notation(PV_best.initial_position[0]%8);
 cout <<(1 + PV_best.initial_position[0]/8);
 cout <<x_to_notation(PV_best.final_position[0]%8);
 cout <<(1 + PV_best.final_position[0]/8);
 // ���� ���� ������ �����������
 if (PV_best.final_pawn[0] !=0) cout <<int_to_char_board(PV_best.final_pawn[0],0);
 cout <<"\n";
//---------------------------------------------------------------------

//Test_Deepening << "\n";
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
}//void Loop(int chess_board[],int Depth_Max,int print_bestmove[],int Eval,int Nodes,int pv[]){
//	*******************************************************************