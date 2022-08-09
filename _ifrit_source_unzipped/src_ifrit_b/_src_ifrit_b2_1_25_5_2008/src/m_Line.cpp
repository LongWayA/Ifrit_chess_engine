#include <fstream.h>  // TEST
#include <stdlib.h>   // ������������ div

#include "m_Line.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "k_Util.h"
//-----------------------------------------------------------------------------------
/*
 * ������ ������� ��������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 11.5.2008
 *
 *  ������������
 * ������ ���� ��� ��� ���� ����� ����� ��������
*/
//---------------------------------------------------------------------------


//	===================================================================
// ��������� ����� ������� �������� � ����
void Update_PV_best_point(T_PV * PV_best_point,T_PV * PV_best){
//------------------------------------------------------
// ������ ������� ������� ������� �������� ������ ������ ���������� � ��������� PV_best
// �� �� ������ ����������� ������ �� � ������� ��������������� ��
// ���������� ���� ��������� ���������� ��� ������� ���� PV_best_point
// ����� �� �������������� ������ ������� ��������������� ������ ������
    for (int n=0;n<PV_best->depth_max;n++){
      PV_best_point->move[n]=PV_best->move[n];
    }//for (int n=0;n<(Depth_Max);n++){
    PV_best_point->score_move =PV_best->score_move;
   // 
   PV_best_point->depth_max =PV_best->depth_max;
//------------------------------------------------------
}//void Update_PV_best_point(T_PV * PV_best_point,T_PV * PV_best){
//	*******************************************************************

//	===================================================================
// ��������� ����� ������� ��������
void Update_PV_best(T_PV * PV_best,T_PV PV_best_point){
//--------------------------------------------
// �� ��������� ���� ������ � ������ � ��������� PV_best_point ���������� ������ �����
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 for (int n=0;n<PV_best_point.depth_max;n++){
      PV_best->move[n]=PV_best_point.move[n];
    }//for (int n=0;n<(Depth_Max);n++){
    PV_best->score_move = PV_best_point.score_move;
 // 
   PV_best->depth_max =PV_best_point.depth_max;
//--------------------------------------------
}//void Update_PV_current(int i,int Depth,T_PV * PV_current,T_list_surplus_moves list_surplus_moves){
//	*******************************************************************

//	===================================================================
// ��������� ����� ������� �������� �� ������������ �������
void Update_PV_best_max_depth(int score,int depth,T_PV * pv_best,T_PV * pv_current){
//--------------------------------------------
// �� �������� ���������� ������� � ���������� ������� ������������ �������
// ���������� �� ������� �� �� ������ �������� �������� ���� �������� � ��������� PV_current
// ������ �� ��������� ��� � ��������� PV_best � �������� ������ �������
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
// �� � ��� ������� ����� ��������
// �������� ��� ���� ���� �� 0 �� Depth-1
// ������ ��� ���� � ��� ��������� ������� 0
   for (int n=0;n<depth;n++){
      pv_best->move[n]=pv_current->move[n] ;
    }// for (int n=0;n<Depth;n++){
    pv_best->score_move = score;
   // ���������� ������� ��������
   pv_best->depth_max =depth;
//--------------------------------------------
}//void Update_PV_best_max_depth(int score,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************
//	===================================================================
// ���������� ��� � ������� �������
void Update_PV_current(int i,int depth,T_PV * pv_current,const T_list_surplus_moves * list_surplus_moves){
//int d_m = ((list_surplus_moves->move[i]>>6)& 63);
// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
// �� ����� ��� ������� ������� ������������� �� ������������ ������ ������� Alpha_beta_816 �.�. 
// �� ����������� ���� ���������� � ������� 
// ������ � ��������� PV_current ����� ������ � �� ������� ������� 
// �� ��� ������������ ���� ������� ������ �� ��������� ��� ��� ��������� � ����������� ����.
//pv_current->initial_position [depth]=list_surplus_moves->initial_position[i] ;
//pv_current->final_position [depth]=list_surplus_moves->final_position[i] ;
//pv_current->initial_position [depth]=((list_surplus_moves->move[i]>>12)& 63);
//pv_current->final_position [depth]=((list_surplus_moves->move[i]>>18)& 63) ;
pv_current->move[depth] = list_surplus_moves->move[i];
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//// ���������� ������ ����������� �����
//if(d_m > 5){
// if(d_m > 15){
//   pv_current->final_pawn [depth]=d_m - 20;
// }else{
//   pv_current->final_pawn [depth]=d_m - 10;
// }//if(list_surplus_moves->description_move[i]>7 ){
//}else{
//   pv_current->final_pawn [depth]=0;
//}//
}//void Update_PV_current(int i,int Depth,T_PV * PV_current,T_list_surplus_moves list_surplus_moves){
//	*******************************************************************











