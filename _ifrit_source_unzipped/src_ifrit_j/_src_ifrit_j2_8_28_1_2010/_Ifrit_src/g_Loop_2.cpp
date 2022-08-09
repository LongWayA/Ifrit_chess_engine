#include <iostream> // TEST
#include <fstream>  // TEST
#include <cstring>    // ���������� ��� ������ �� ������� (��������� �����)
#include <stdlib.h>   // ������������ ���������� ������ �������� exit(1)
using namespace std;


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
 * ��������� ��������� 23.02.2009 12.11.2009
 *
*/
//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
   ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

class UTIL util_o;

//	===================================================================
// ������� ������� ������� � ��������
void FULLSEARCH::Print_currmove(const struct T_list_surplus_moves * const p_list_surplus_moves,//������ �����
				              const unsigned __int8 nodes_0,// ���������� ����� �� ������� �������
				              const unsigned __int8 i,// ����� ���� �� ������
				              const unsigned __int8 Depth_Max// ������������ �������
				              ){

// �������������� � ������ ������ ���
 cout << "info currmove " ;
 cout <<util_o.x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8);
 cout <<(1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);
 cout <<util_o.x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8);
 cout <<(1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8);
// ���������� ����� ��������������� ����
 cout << " currmovenumber " ;
 cout << (__int32)nodes_0;
// �������� ���������� � ������� ���������
 cout << " depth " << (__int32)Depth_Max;
 cout << "\n";
 cout.flush();
}//void FULLSEARCH::Print_cout_v
//	*******************************************************************


//	===================================================================
// ������� ������� ������� � ��������
void FULLSEARCH::Print_pv_line(const bool colour_move,// ���� ���������� �������
				            const struct T_list_surplus_moves * const p_list_surplus_moves,// ������ �����
				            const struct T_PV * const p_PV_best_point,// ������ �������
				            const __int64 print_nodes,// ���������� �����
				            const unsigned __int8 i,// ����� ���� � ������
				            const unsigned __int8 Depth_Max,// ������������ �������
				            const __int32 value// ������ �������
				            //,const double time_nodes// ����� ������� ��������
				            ){

static __int32 save_value =-999999999; // ���������� ������
static unsigned __int8 save_Depth_Max = 0; // ���������� �������

__int32 inv = 0;// ��������� ���� ������� �������
__int32 d_m = 0;// ������������ ������ ����������� �����
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

if(save_value == value) cout <<"\n";

// ������� ������� ������ � ��� ������ ���� ������ ������ � ������ � ������
// ������� ���������� ��� �� �� ���� ������ ������ � ���� �� ��� ��� 
#if TEST_PRINT_ALL_LINE
   if(1){
#else
// ���� ������ ���������� ��� ���������� ������� �� ������� �������
   if(  (save_value != value) // ���������� ������
	    || (save_Depth_Max != Depth_Max) // ���������� �������
	  ){
      save_value = value;
      save_Depth_Max = Depth_Max;
#endif//#if TEST_PRINT_ALL_LINE

      cout << "info " ;
//      cout << " nps "<<(int)(nodes/time_nodes);
      cout << " depth "<< (__int32)Depth_Max;
//    �������� ���������� � ������� �������������� ��������
      cout << " seldepth "<< (__int32)p_PV_best_point->depth_max;

//    �������� ���������� � ������ �������(��� ������ �� ������ ������ ������ �������� � ������ ���������)
      colour_move == 0 ? inv = -1 : inv = 1;
      if(inv * value > 900000){
            cout << " score mate " << (__int32)p_PV_best_point->depth_max;// ��������� �� ���� � ���� �����
      } else if(inv * value < -900000){
            cout << " score mate " << -(__int32)p_PV_best_point->depth_max;// ��������� �� ���� � ���� �����
      }else{
	        cout << " score cp " << inv * value;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
      }//if(inv * value > 900000)

//    �������� ���������� � ���������� ������������� �����
      cout << " nodes "<<(__int32)print_nodes;

//    ��������� �� ������ ������ ��� �� �� ��������� �� ����� ������� �� ������
      if( (value * value) != (p_PV_best_point->score_move * p_PV_best_point->score_move) ) {
    //if(0) {
           cout << "\n";
	       cout << "ERROR LINE " ;
           cout << "\n";
           cout << " value = " << value;
           cout << "\n";
           cout << " p_PV_best_point.score_move = " << p_PV_best_point->score_move;
           cout << "\n";
           cout << "**********************"<<"\n";
           cout << "  " << "\n";
      }else{
//         ����� ����� ��������
           cout << " pv ";
           for (unsigned __int8 n = 0; n < p_PV_best_point->depth_max; n++){
                cout << util_o.x_to_notation(((p_PV_best_point->move[n]>>12)& 63)%8);
                cout << (1 + ((p_PV_best_point->move[n]>>12)& 63)/8);
                cout << util_o.x_to_notation(((p_PV_best_point->move[n]>>18)& 63)%8);
                cout << (1 + ((p_PV_best_point->move[n]>>18)& 63)/8);

                d_m = ((p_PV_best_point->move[n]>>6)& 63);
//              ���������� ������ ����������� �����
                if(d_m != 5){
                    if(d_m > 15){
                        cout <<util_o.int_to_char_board((d_m - 20),0);
                    }else{
                        cout <<util_o.int_to_char_board((d_m - 10),0);
                    }//if(d_m > 15)
                }//if(d_m != 5)
                cout <<" ";
           }//for (int n=0;n<p_PV_best_point.depth_max;n++)
      }//if( (value * value) != (p_PV_best_point.score_move * p_PV_best_point.score_move) )
      cout <<"\n";
  }// if(  (save_value != value)

cout.flush();// ��� ��� �� ����� �� ��� ����� ������� :)

}//void FULLSEARCH::Print_cout
//	*******************************************************************


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������������ �������, ������� ��������, �����,
#if TEST_L

void FULLSEARCH::List_Print(int depth,// ������� �������
                struct T_list_surplus_moves * p_list_surplus_moves// ������ �����
				){

Test_Loop.open ("_TEST_LOOP.txt",ios::app);	
//Test_Loop.open ("_TEST_LOOP.txt");	
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop << "\n";
Test_Loop << "\n";
Test_Loop << "\n";
Test_Loop << "------------------------------------------------- "<<"\n";
Test_Loop <<"depth= "<< depth << "\n";
Test_Loop << "\n";
Test_Loop << "������ ��������� ����� ������ "<<"\n";

for (int i = 0 ; i < p_list_surplus_moves->end_list ; i++){	
     Test_Loop << "����� ���� �� ������ i= " << i << "\n";  
     Test_Loop << "������� ������ " << util_o.int_to_char_board(((p_list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
     Test_Loop << "������ ���� " << p_list_surplus_moves->sorting_score[i] << "\n";

     // ����� ���� ���� e2-e4
     //------------------------------
	 Test_Loop << " " << (util_o.x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8));
     Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
 //  Test_Loop<< "\n";
     Test_Loop << " " << (util_o.x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8));
     Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
     //------------------------------
 //��� ���� 
 //0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
 //12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
 //22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
 //25 - ������ � ������������ � ����� 
     Test_Loop << "��� ���� " << ((p_list_surplus_moves->move[i]>>6)& 63)  << "\n";
     Test_Loop << "������ ������ " << util_o.int_to_char_board(( p_list_surplus_moves->move[i] & 63),1)<< "\n";
	 Test_Loop << "��������� ��������� " << ((p_list_surplus_moves->move[i]>>12)& 63)  << "\n";
	 Test_Loop << "�������� ��������� " << ((p_list_surplus_moves->move[i]>>18)& 63)  << "\n";
     

  
	 Test_Loop << "\n";
}//for (i=1,i<p_list_surplus_moves->amount_moves+1){

Test_Loop << "p_list_surplus_moves->end_captures= " <<p_list_surplus_moves->end_captures << "\n";
Test_Loop << "p_list_surplus_moves->end_list= " <<p_list_surplus_moves->end_list << "\n";
Test_Loop << "------------------------------------------------- " << "\n";
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop.close();
}//void FULLSEARCH::List_Print

#endif
//	*******************************************************************





