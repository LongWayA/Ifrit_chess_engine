#include <iostream> // TEST

using namespace std;


#include "m_Line.h"
#include "k_Util.h"

//-----------------------------------------------------------------------------------
/*
 * ������ ����� ��������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 27.02.2009
 *
 *  ������������
*/
//---------------------------------------------------------------------------

// global
  struct T_PV pv_current;// ������� �������
  struct T_PV pv_best;// ������ �������

//	===================================================================
// ��������� ����� ������� �������� � ����
void PVSAVE::Update_PV_best_point(struct T_PV * const p_PV_best_point// ������ ������� ��������� � ����
						          ){

// ������ ������� ������� ������� �������� ������ ������ ���������� � ��������� PV_best
// �� �� ������ ����������� ������ �� � ������� ��������������� ��
// ���������� ���� ��������� ���������� ��� ������� ���� PV_best_point
// ����� �� �������������� ������ ������� ��������������� ������ ������
 for(unsigned __int8 n = 0; n < pv_best.depth_max; n++){
    p_PV_best_point->move[n] = pv_best.move[n];
 }//for (int n = 0; n < PV_best.depth_max; n++){

 p_PV_best_point->score_move = pv_best.score_move;
 p_PV_best_point->depth_max = pv_best.depth_max;

}//void PVSAVE::Update_PV_best_point
//	*******************************************************************

//	===================================================================
// ��������� ����� ������� ��������
void PVSAVE::Update_PV_best(const struct T_PV * const p_PV_best_point// ������ ������� ��������� � ����
					        ){

// �� ��������� ���� ������ � ������ � ��������� PV_best_point ���������� ������ �����
// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
  for(unsigned __int8 n = 0; n < p_PV_best_point->depth_max; n++){

     pv_best.move[n] = p_PV_best_point->move[n];

  }//for(int n = 0; n < p_PV_best_point->depth_max; n++){

  pv_best.score_move = p_PV_best_point->score_move; 
  pv_best.depth_max = p_PV_best_point->depth_max;

}//void PVSAVE::Update_PV_best
//	*******************************************************************

//	===================================================================
// ��������� ����� ������� �������� �� ������������ �������
void PVSAVE::Update_PV_best_max_depth(const __int32 & score,// ������ ������� � ����� ��������
							          const unsigned __int8 & depth// ������� �������
							          ){

// �� �������� ���������� ������� � ���������� ������� ������������ �������
// ���������� �� ������� �� �� ������ ���� ���� �������� ���� �������� � ��������� PV_current
// ������ �� ��������� ��� � ��������� PV_best � �������� ������ �������
// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
// �� � ��� ������� ����� ��������
// �������� ��� ���� ���� �� 0 �� Depth - 1
// ������ ��� ���� � ��� ��������� ������� 0
 for(unsigned __int8 n = 0; n < depth; n++){

    pv_best.move[n] = pv_current.move[n];

 }//for(int n = 0; n < depth; n++){

 pv_best.score_move = score;

 // ���������� ������� ��������
 pv_best.depth_max = depth;

}//void PVSAVE::Update_PV_best_max_depth
//	*******************************************************************

//	===================================================================
// ���������� ��� � ������� �������
void PVSAVE::Update_PV_current(const unsigned __int8 & i,// ����� ���� � ������ �����
					           const unsigned __int8 & depth,// ������� �������
					           const struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
					           ){

// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
// �� ����� ��� ������� ������� ������������� �� ������������ ������ ������� Alpha_beta �.�. 
// �� ����������� ���� ���������� � ������� 
// ������ � ��������� PV_current ����� ������ � �� ������� ������� 
// �� ��� ����������� ���� ������� ������ �� ��������� ��� ��� ��������� � ����������� ����.
pv_current.move[depth] = p_list_surplus_moves->move[i];

}//void PVSAVE::Update_PV_current
//	*******************************************************************

//	===================================================================
// ���������� ������ �������
struct T_PV * PVSAVE::get_pv_best(){

 return & pv_best;

}//
//	*******************************************************************

//	===================================================================
// ���������� ������� �������
struct T_PV * PVSAVE::get_pv_current(){

return & pv_current;

}//
//	*******************************************************************



//	===================================================================
// ������� ������� ������� � ��������
void PVSAVE::Print_currmove(const struct T_list_surplus_moves * const p_list_surplus_moves,//������ �����
				              const unsigned __int8 nodes_0,// ���������� ����� �� ������� �������
				              const unsigned __int8 i,// ����� ���� �� ������
				              const unsigned __int8 Depth_Max// ������������ �������
				              ){

// �������������� � ������ ������ ���
 cout << "info currmove " ;
 cout <<UTIL::x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8);
 cout <<(1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);
 cout <<UTIL::x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8);
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
void PVSAVE::Print_pv_line(const bool colour_move,// ���� ���������� �������
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
                cout << UTIL::x_to_notation(((p_PV_best_point->move[n]>>12)& 63)%8);
                cout << (1 + ((p_PV_best_point->move[n]>>12)& 63)/8);
                cout << UTIL::x_to_notation(((p_PV_best_point->move[n]>>18)& 63)%8);
                cout << (1 + ((p_PV_best_point->move[n]>>18)& 63)/8);

                d_m = ((p_PV_best_point->move[n]>>6)& 63);
//              ���������� ������ ����������� �����
                if(d_m != 5){
                    if(d_m > 15){
                        cout <<UTIL::int_to_char_board((d_m - 20),0);
                    }else{
                        cout <<UTIL::int_to_char_board((d_m - 10),0);
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






