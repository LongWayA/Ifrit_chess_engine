#include <fstream.h>  // TEST
#include <stdlib.h>   // ������������ div

#include "i_Forced.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "k_Util.h"
#include "l_Sorting.h"
#include "m_Line.h"
#include "d_Bitboard_gen.h"

//-----------------------------------------------------------------------------------
/* ? ����� ����������� ��� ����������
 * ������������� ������� ������� 
 *
 * ����� ChessTerminator75
 * ��������� ��������� 9.12.2007
 *
 *  ������������
 * ������ ���� ��� ��� ���� ����� ����� ��������
*/
//---------------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
 ofstream Test_Loop_f;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// ������������ �������,������� ��������,�����,
int Forced_variant_816(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth,T_PV * pv_current,T_PV * pv_best){
int value=0; //
int value_best=9999999; //
int inv=0;   //
int z=1;

// ������ ��� ������� ���� �������
T_PV pv_best_point={0};

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
    Test_Loop_f.open ("_TEST_LOOP_FORCED.txt",ios::app);
 //   Test_Loop_f.open ("_TEST_LOOP_FORCED.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


bitboard->colour_move == 1 ? inv=-1 : inv=1;
value_best = inv*Eval_forced(bitboard);
// ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
// � ������ ������� ����� �������� ������
Update_PV_best_max_depth(value_best,depth,pv_best,pv_current); 

// ���������� � ����� ������ ��� ��� � ���������� ����� �� ����� �����
// ����� �� �������������� ������ ������� ��������������� ������ ������
  Update_PV_best_point(&pv_best_point,pv_best);

// 1
//-----------------------------------------------------------------------
// ���� ����������� ������ ���������� ������ �� ������ ������ ������ �� �����
// ������� �� ��� ����� ������� ������ ��� ��� ��������� �����
// ���������� ������ ���������
 if(value_best <= beta){
	 return value_best;
 }//if(value_best <= beta){

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
 if(alpha <= beta){
	 return alpha;
 }//if(value_best <= beta){
//-----------------------------------------------------------------------

//����������� ������ ��������� ������
list_surplus_moves->current_depth =depth;
Generator_captures(bitboard,list_surplus_moves);
Sorting_captures_moves(list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  #if TEST_F
//Test_Loop_f <<"============================================ "<< "\n";
//Test_Loop_f <<"���� 1 ������������� ������ ��� ����: "<< "\n";
//Test_Loop_f <<" "<<(x_to_notation(pv_current->initial_position[depth-1]%16));
//Test_Loop_f <<""<<(8-pv_current->initial_position[depth-1]/16);//<< "\n";
//Test_Loop_f <<" "<<(x_to_notation(pv_current->final_position[depth-1]%16));
//Test_Loop_f <<""<<(8-pv_current->final_position[depth-1]/16);//<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f <<"�� �������: "<<depth<< "\n";
//Test_Loop_f <<"��� ���� ���������� ������ � ����� ����: "<< "\n";
//Test_Loop_f <<"value_otsehki = "<<value_best;//<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f <<"alpha = "<<alpha;//<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f <<"beta = "<<beta;//<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f <<"����� ��������� ������ ������: ";
//    List_816_Print_F(depth,list_surplus_moves);
// //   Board_816_Print( depth,board_list_816);
//Test_Loop_f << "\n";
//  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// 2
//-----------------------------------------------------------------------
// ���� ��� ������ �� ������ ���������� ������ ���������
// ���� �������� ����� ������ �� ������������ � �������(�.�. ������ ������ ������ � ����� ���������)
if (list_surplus_moves->start_list[depth]==list_surplus_moves->end_list[depth]){ 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
  //Test_Loop_f <<"return return======= "<< "\n";
  Test_Loop_f << "\n";
  Test_Loop_f.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  return value_best;
 }//if (list_surplus_moves->start_list[depth]==list_surplus_moves->end_list[depth]){ 
//-----------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//#if TEST_F
//if (value < alpha) Test_Loop_f <<"��������  ����� : "<< "\n";
//    Test_Loop_f.close();
//#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//���� �� ��� ��� ������� �� ���� ������ ��������������� ������ � ����� �� �����
//

//����� �� ������ ��������� �����
// ������� ������������� ������� �� ������� 3
//if (depth > 3) list_surplus_moves->end_list[depth]=0;
//list_surplus_moves->end_list[depth]=0;
//SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
// ����� � ����� ���� ����������
for (int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){
// nodes = nodes+1;
// ��������� ���
 Onward_moves(i,list_surplus_moves,bitboard);
// Bitboard_ini_t(bitboard,board_list_816);


//-------------------------------------------------------------------
// ������������ ������ ������� ������
if (list_surplus_moves->name_taking_figure[i]!=6){
//list_surplus_moves->current_depth =depth;
// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
 Update_PV_current(i,depth,pv_current,list_surplus_moves);
// ����������� ����� �������
value=-1*Forced_variant_816(bitboard,list_surplus_moves,-beta,-alpha,(depth+1),pv_current,pv_best);
list_surplus_moves->current_depth =depth;
// ��������������� �������
Backs_moves(i,list_surplus_moves,bitboard);

 if (value < value_best){
  value_best = value;
// ����� �� �������������� ������ ������� ��������������� ������ ������
  Update_PV_best_point(&pv_best_point,pv_best);
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//#if TEST_F
//    Test_Loop_f.open ("_TEST_LOOP_FORCED.txt",ios::app);
//#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  #if TEST_F
//Test_Loop_f <<"�� �������: "<<depth<< "\n";
//Test_Loop_f <<"��������  ����� ������ ����� � ����������� : "<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f <<"i = "<<i<< "\n";
//
//Test_Loop_f <<"value = "<<value;//<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f <<"alpha = "<<alpha;//<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f <<"beta = "<<beta;//<< "\n";
//Test_Loop_f << "\n";
//Test_Loop_f << "\n";
//  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//#if TEST_F
//    Test_Loop_f.close();
//#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 }//if (value < alpha){ 

//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 if (value < alpha){  
  alpha = value;                      
 }//if (value < alpha){ 

 // ���� ��� ������� �� ���� ������� ������� ������ ��� ������ ��� ���� �� ������ ��������
 // ���� ���� ���� �� ����� ��������
 if(alpha <= beta){
   Update_PV_best(pv_best,pv_best_point);
   return alpha;
 }//if(alpha <= beta){
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//-------------------------------------------------------------------
}else{

// ������ ������� !
//  nodes = nodes-1;

// ��� ���� ��� ����� ������ ������ ��� � ����
// ��� ���� �� ������� ����� �������
   value=-(1000000-depth*1000);
//z=1;
//if (div(depth,2).rem == 0) z=-1;
//  value=z*(1000000-depth*1000);

   //board_list_816->colour_move == 1 ? inv=1 : inv=-1;
   //value = z*inv*Eval_forced_816(board_list_816);
   Backs_moves(i,list_surplus_moves,bitboard);

// ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
// � ������ ������� ����� �������� ������
Update_PV_best_max_depth(value,depth,pv_best,pv_current);
return value;

}//if (list_surplus_moves->name_taking_figure[i] !=6){

//--------------------------------------------------------------------
}//for (int i=1;i<list_surplus_moves->amount_moves;i++){
//SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS

// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
 Update_PV_best(pv_best,pv_best_point);
return value_best;
}//int Forced_variant_816(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������������ �������,������� ��������,�����,
#if TEST_F
void List_816_Print_F(int depth,const T_list_surplus_moves * list_surplus_moves){
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop_f << "\n";
Test_Loop_f << "------------------------------------------------- "<<"\n";
Test_Loop_f <<"depth= "<< depth << "\n";
Test_Loop_f << "\n";
Test_Loop_f << "������ ��������� ����� ������ "<<"\n";
for (int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){	
     Test_Loop_f <<"i= "<<i<< "\n";  
     Test_Loop_f <<"��� ���� "<<list_surplus_moves->description_move[i]<< "\n";
//	 Test_Loop_f <<"��������� ��������� "<<list_surplus_moves->initial_position[i]<< "\n";
     Test_Loop_f <<"������ "<<int_to_char_board(list_surplus_moves->name_figure[i],1)<< "\n";
     Test_Loop_f <<"������ ������ "<<int_to_char_board(list_surplus_moves->name_taking_figure[i],1)<< "\n";
Test_Loop_f <<" "<<(x_to_notation(list_surplus_moves->initial_position[i]%16));
Test_Loop_f <<""<<(8-list_surplus_moves->initial_position[i]/16);//<< "\n";
 //    Test_Loop_f<< "\n";
//	 Test_Loop_f <<"�������� ��������� "<<list_surplus_moves->final_position[i]<< "\n";
Test_Loop_f <<" "<<(x_to_notation(list_surplus_moves->final_position[i]%16));
Test_Loop_f <<""<<(8-list_surplus_moves->final_position[i]/16)<< "\n";     
	 Test_Loop_f << "\n";
}//for (i=1,i<list_surplus_moves->amount_moves+1){

Test_Loop_f <<"list_surplus_moves->start_list= "<<list_surplus_moves->start_list[depth]<< "\n";
Test_Loop_f <<"list_surplus_moves->end_list= "<<list_surplus_moves->end_list[depth]<< "\n";
Test_Loop_f << "------------------------------------------------- "<<"\n";
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
}//void List_816_Print(int Depth,T_list_surplus_moves list_surplus_moves;){
#endif
//	*******************************************************************











