#include <iostream>
#include <fstream>  // TEST
using namespace std;

#include "g_Search.h"
#include "c_Iterative_Deepening.h"
#include "k_Util.h"
#include "k_Timer.h"
#include "h_Evaluate.h"
#include "j_Hash.h"
#include "d_Bitboard_gen.h"

//-----------------------------------------------------------------------------------
/*
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

#define TEST_WRITE_READ_WN 0 // ��������-���������(1-0) ����� �������� ����������
                             // ���������� � ��������� �������
                             // ���� ��� �������� ���� TEST_WRITE_READ � j_Hash.h

//global

//	===================================================================
//
void Deepening(struct T_Bitboard * p_bitboard,//������������� �����
			   unsigned __int8 depth_max,//������� ��������
			   double wtime,//���������� ����� �� ���� �� �����
			   double btime,//���������� ����� �� ���� �� ������
			   double winc,//���������� ������� �� ��� ��� �����
			   double binc,//���������� ������� �� ��� ��� ������
			   unsigned __int8 movestogo,//���������� ����� �� ������� �������� �����
			   double movetime,//����� �� ���
			   bool infinite//����������� ������
			   ){

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
 //   Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

// ����� ������� � ��������
double time_terminate = -1.0;// ����� ���������� �� ���
double time_current = -1.0;  // ������� ������� ������ ��� ������ ��������

__int32 Alpha = -9999999;
__int32 Beta  =  9999999;

__int8 inv = 0;// ���� ����� ����
__int32 value_root = 0;// ������ ������� �������� � ��������� ����. 
__int32 d_m = 0; // ��� ����. ����� ��� ����������� ���� ������ � ������������
__int64 static nodes_y = 0; // ���������� ����� �� ������ �������
__int64 static nodes_previous = 0; // ���������� ������ ���������� �����
__int32 test_eval = 0;// ���������� ������ ��������� ��� ��������
bool flag_mate = 0;// ������ ��� 
unsigned __int64 key_control = 0;   // ���������� ���-���� ��� ��������
struct T_list_surplus_moves root_list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)

__int64 nodes = 0;// ���������� ������������� �����(�������)
unsigned __int16 cnt_move ;// ���������� ��� ��������� � ������ ���������

unsigned __int16 limit_cnt_move ;// ������� ����� �������� ������� (�������������� �����)
//-------------------------------------------------------------------------------
//             > go btime 302000 wtime 302000 winc 2000 binc 2000 
// 5 min * 60 = 300 sek
// ����� � ��� �������� � �������� ���� �� �������� � ������������� �� ��
// ��� ������ �������� �� 1000
// �������� ����� �� ���(���� ������� ��� �����������). 
// ������ ������������� ���������� � ����. � ������ ���� ����� � ����� ������
// ��� ��� � ������� ��� ����� ������� ���������� �� ��� ������� ������ ���� ������ � ���
//if(p_bitboard->colour_move == 1){// 1-��� ����� 0- ��� ������
//    if(wtime != -1) time_terminate = wtime/100;
//}else{
//    if(btime != -1) time_terminate = btime/100;
//}//if (board_list_816->colour_move == 1){


// ���������� ��������� ��������� � ������
cnt_move = HASHM::get_save_cnt_move();

// �� ������������ ��� ��� ������ ���� �� ������������ ���������� ����� (80)
// ��� ���������� ���������� �����
limit_cnt_move = 80 - cnt_move/2 ;
// ����� �� ������ 40 ����� �������� ����������
// ����� 40 �� ���� ���� ���������� ������� �� ��� ��� �� �������� ��������� �� �������
if(limit_cnt_move < 40 ) limit_cnt_move = 40;

// �������� �� ����������� �������� �� ������ ������� �.�. 
// ��������� ������ ����� ��������� ��������� �������
limit_cnt_move = 3 * limit_cnt_move;


if(p_bitboard->colour_move == 1){// 1-��� ����� 0- ��� ������
    if(wtime != -1) time_terminate = wtime/limit_cnt_move;
}else{
    if(btime != -1) time_terminate = btime/limit_cnt_move;
}//if (board_list_816->colour_move == 1){


// TEST
//wtime = 60; // 5 min
//while (wtime > 1){
// limit_cnt_move = 80 - cnt_move/2 ;
// if(limit_cnt_move < 40 ) limit_cnt_move = 40;
// //limit_cnt_move = 3 * limit_cnt_move;
// time_terminate = wtime/limit_cnt_move;
//
// cout << "cnt_move = " << cnt_move << "\n";
// cout << "wtime = " << wtime << "\n";
// cout << "time_terminate = " << time_terminate << "\n";
// //cout << "limit_cnt_move = " << limit_cnt_move << "\n";
// cout << "   " << "\n";
//
// wtime = wtime - time_terminate;
// cnt_move = cnt_move + 1;
//}//while (wtime > 0)


// TEST
//cout << "cnt_move = " << cnt_move << "\n";
//cout << "limit_cnt_move = " << limit_cnt_move << "\n";
//cout << "time_terminate = " << time_terminate << "\n";


//             > go movetime 300000 
//������ ����� �� ��� (���� ������� ��� �����������).
if (movetime != -1) time_terminate = movetime;
//--------------------------

// �� ������� �������� ������ ��� ������������ ���������� ������� �� ������
FULLSEARCH::ini_deep_time_terminate( (time_terminate * 50) );

//���� ����� ��������� ���� ����������� ��
//����������� �� ������� 50
if(time_terminate != -1) depth_max = 50;
//--------------------------

//             > go infinite
// ������ ��� ����������� �������. 50 ���� ����� ����� ����������
if(infinite == true) depth_max = 50;
//--------------------------

// ���� ����� ��� ��������� �� ����������������  
if(depth_max == 0) depth_max = 1;
//-------------------------------------------------------------------------------

// TEST
//cout << "movetime = " << movetime << "\n";
//cout << "time_terminate = " << time_terminate << "\n";
//cout << "depth_max = " << depth_max << "\n";

// �������������� ���������� ���������� ������ loop
FULLSEARCH::ini_global_loop(depth_max);

// ������� ��������. � ���������� ������ ������������ ������ ��� �������.
p_bitboard->eval = EVALUATE::Eval_forced(p_bitboard);

// ��� �������� ��� ����� ����������� ������ ��������� � ��������������� ���������
test_eval =  p_bitboard->eval;

// TEST
//cout << " N bitboard->eval = " << bitboard->eval << "\n";

// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
HASHM::Start_position_random_key(p_bitboard);

// ���������� ���� ��� �������� ��� ����������� � �������
key_control = p_bitboard->hash_key;

//�������������� ��� - ������ �������� ������������� ������� ��� �� ��������� ������ ������������
HASHM::Ini_hash_three(); 

// ���������� ���� ��� ������������ ������� �� ������ �������
HASHM::Save_hash_three(p_bitboard->hash_key,0);

// TEST
// ����� ������� ���-�������
// �������������� ��� ����� � ���-������� 0
//   HASHM::Ini_hash_moves();

// ��������� ��� ������������ �������������� ��������
// TEST
//depth_max = depth_max - 1;

// TEST
//cout << "1111111 " << "\n";

  if(p_bitboard->colour_move == 1){  
	   // ����� 
       //��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
       BITBOARDGENERATOR::Generator_captures_white(p_bitboard,&root_list_surplus_moves);// ���������� ������ � �����������
       BITBOARDGENERATOR::Generator_move_white(p_bitboard,&root_list_surplus_moves);// ���������� ��������� ����
  }else{
	   // ������
       //��������� ������ ���������� �����. ������ ��� ������ � ���������� ������ ��������.
       BITBOARDGENERATOR::Generator_captures_black(p_bitboard,&root_list_surplus_moves);// ���������� ������ � �����������
       BITBOARDGENERATOR::Generator_move_black(p_bitboard,&root_list_surplus_moves);// ���������� ��������� ����
  }//if(bitboard->colour_move ==1){

// TEST
//cout << "22222222222 " << "\n";
//cout << " depth_max " << depth_max << "\n";//�������

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
  // TEST
  // �������� ��������
  HASHM::Test_hash_ini();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

// ���� ������� ������ ������������ � �� ����� ��� � ������ 50 � �� ���� ������� �� ��������� �� ���������� ����
for(unsigned __int8 depth = 1; 
    ( (depth <= depth_max) && // ������� ������ ������������ �������� �� ���� �������
	  (flag_mate == 0) && // ���� ����
	  (depth < 51) && // ������� ������ ������������ 
	  (FULLSEARCH::get_stop_search() == 0)// ��� ������� �� ��������� ����� 
	); // ��� ������� �� ����� �����
    depth++
   ){
 
// ���������� ������ ���������� ����� � ���������� �������� �.� �������
  nodes_previous = nodes;

//���������� ������� �������
  FULLSEARCH::set_nodes_in_0();

// ��������� ������
  TIMER::Timer_start();

  if(p_bitboard->colour_move == 1){                                               
       value_root = FULLSEARCH::White_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
  }else{
       value_root = FULLSEARCH::Black_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
  }//if(bitboard->colour_move ==1){

// ������������� ������
  TIMER::Timer_stop();
  time_current = TIMER::Timer_delta();

nodes = FULLSEARCH::get_nodes();

//���������� ����� �� ���� ������� ��� ������ ���������� �����
//����� ������ ���������� ����� �� ���������� ��������
  nodes_y = nodes - nodes_previous ;

// �������� ������ ���������
if(test_eval != p_bitboard->eval) cout << "ERROR test_eval != bitboard->eval" << "\n";
 
//-----------------------------------------------
// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
// ��������� ������� �� ���� �� ���������� �� �������
    if(key_control != p_bitboard->hash_key){
	   	 cout <<"\n";
		 cout << "ERROR HESH Test_Deepening " ;
		 cout <<"\n";
	}//if(key != p_bitboard->hash_key){
//-----------------------------------------------

//---------------------------------------------------------------
// �������� ���������� � �����
  cout << "info " ;
  cout << " nps " << (__int32)(nodes / time_current);// �������� �� ������� �� ���� ����� �� ������ �������
  cout << " depth " << (__int32)depth;//�������

// �������� ���������� � ������� �������������� ��������
   cout << " seldepth "<< (__int32)FULLSEARCH::get_pv_best()->depth_max;

  p_bitboard->colour_move == 0 ? inv = -1 : inv = 1;

// �������� ������. ��� ���� ����������� ������������� � ���� ���������
  if(inv * value_root > 900000){ 
     flag_mate = 1;
     cout << " score mate "<< (__int32)FULLSEARCH::get_pv_best()->depth_max;// ��������� �� ���� � ���������
  }else if(inv * value_root < -900000){
     cout << " score mate "<< -(__int32)FULLSEARCH::get_pv_best()->depth_max;// ��������� �� ���� � ���������
     flag_mate = 1;
  }else{
     cout << " score cp "<< inv * value_root;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
     flag_mate = 0;
  }//if(inv * value_root > 900000){ 
 
// cout << " score cp "<< inv*value_root;// ��� ����� ������� ������

// �������� ����� ��������
 cout << " nodes " << (__int32)nodes; // ������ ���������� �����
//  cout << " nodes "<< nodes_y; // ���������� ����� �� ����
  cout <<" pv ";

  for( unsigned __int8 n = 0; n < FULLSEARCH::get_pv_best()->depth_max; n++){
      cout << UTIL::x_to_notation(((FULLSEARCH::get_pv_best()->move[n]>>12)& 63)%8);
      cout << (1 + ((FULLSEARCH::get_pv_best()->move[n]>>12)& 63)/8);
      cout << UTIL::x_to_notation(((FULLSEARCH::get_pv_best()->move[n]>>18)& 63)%8);
      cout << (1 + ((FULLSEARCH::get_pv_best()->move[n]>>18)& 63)/8);

//    ��������� ��� ����
      d_m = ((FULLSEARCH::get_pv_best()->move[n]>>6)& 63);

//    ���� d_m ������ 5 �� ������ ����� ����������� �����
      if(d_m > 5){
          if(d_m > 15){
               cout << UTIL::int_to_char_board((d_m - 20),0);// ����������� ����� � �������
          }else{
               cout << UTIL::int_to_char_board((d_m - 10),0);// ������ ����������� �����
          }//if(d_m > 15){
      }//if(d_m > 5){

      cout << " ";
   }//for( int n = 0; n < FULLSEARCH::get_pv_best()->depth_max; n++){

   cout <<"\n";

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
// TEST

// ������� ��������� � ������� � ������� ������������� �.�. �� ������� �� ��������
HASHM::TEST_Hash_index_print();

// �������� ���� �� ������������� �����������
HASHM::Test_hash_print();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

// ������� �� �������
   if((time_terminate != -1) && (time_terminate < time_current) && (depth >= 2)) break;

}//for(int depth = 1; ((depth < (depth_max + 1)) && (depth < 51) && (FULLSEARCH::get_stop_search() == 0)); depth++){

// TEST
//cout << "cnt_move = " << cnt_move << "\n";
//cout << "wtime = " << wtime << "\n";
//cout << "time_terminate = " << time_terminate << "\n";
//cout <<"time_current = "<< time_current <<"\n";
//cout <<"   " <<"\n";

//-----------------------------------------------
// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
// ��������� ������� �� ���� �� ���������� �� �������
    if(key_control != p_bitboard->hash_key){
	   	 cout <<"\n";
		 cout << "ERROR HESH Test_Deepening " ;
		 cout <<"\n";
	}//if(key != p_bitboard->hash_key){
//-----------------------------------------------


//---------------------------------------------------------------------
// ������ ��� ���� < bestmove e2e4 
// ��� ��� ����������� � ����(� �������)  < bestmove d7d8N 
cout << "bestmove ";
cout << UTIL::x_to_notation( FULLSEARCH::get_from() % 8);
cout << (1 + FULLSEARCH::get_from() / 8);
cout << UTIL::x_to_notation( FULLSEARCH::get_to() % 8);
cout << (1 + FULLSEARCH::get_to() / 8);

// ��������� ��� ����
d_m = ((FULLSEARCH::get_pv_best()->move[0]>>6)& 63);

// ���� d_m ������ 5 �� ������ ����� ����������� �����
if(d_m > 5){
   if(d_m > 15){
      cout << UTIL::int_to_char_board((d_m - 20),0);
   }else{
      cout << UTIL::int_to_char_board((d_m - 10),0);
   }//if(d_m > 15){
}//if(d_m != 5){
cout <<"\n";

// �������� ������������ ������� ���� ������� ���� ����� ��������
if( FULLSEARCH::get_from() != ((FULLSEARCH::get_pv_best()->move[0]>>12)& 63)) cout << "ERROR from(Deepening)" <<"\n";
if( FULLSEARCH::get_to() != ((FULLSEARCH::get_pv_best()->move[0]>>18)& 63))     cout << "ERROR to(Deepening)" <<"\n";
//---------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//void Deepening
//	*******************************************************************
