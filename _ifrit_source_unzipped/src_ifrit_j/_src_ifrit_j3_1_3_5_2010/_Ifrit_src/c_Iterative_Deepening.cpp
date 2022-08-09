#include <iostream>
#include <fstream>  // TEST

#include "g_Search.h"
#include "c_Iterative_Deepening.h"
#include "k_Util.h"
#include "k_Timer.h"
#include "h_Evaluate.h"
#include "j_Hash.h"
#include "d_Bitboard_gen.h"
#include "j_ZObrist.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 10.02.2008  11.11.2009
*/
//---------------------------------------------------------------------------

/*
������� � ����������� ����������� �������
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


namespace {
 __int64 nodes_y = 0; // ���������� ����� �� ������ �������
 __int64 nodes_previous = 0; // ���������� ������ ���������� �����
}//namespace {

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
cnt_move = ZOBRIST::get_save_cnt_move();

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
// std::cout << "cnt_move = " << cnt_move << "\n";
// std::cout << "wtime = " << wtime << "\n";
// std::cout << "time_terminate = " << time_terminate << "\n";
// //std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";
// std::cout << "   " << "\n";
//
// wtime = wtime - time_terminate;
// cnt_move = cnt_move + 1;
//}//while (wtime > 0)


// TEST
//std::cout << "cnt_move = " << cnt_move << "\n";
//std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";
//std::cout << "time_terminate = " << time_terminate << "\n";


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
//std::cout << "movetime = " << movetime << "\n";
//std::cout << "time_terminate = " << time_terminate << "\n";
//std::cout << "depth_max = " << depth_max << "\n";

// �������������� ���������� ���������� ������ loop
FULLSEARCH::ini_global_loop(depth_max);

// ������� ��������. � ���������� ������ ������������ ������ ��� �������.
p_bitboard->eval = EVALUATE::Eval_forced(p_bitboard);

// ��� �������� ��� ����� ����������� ������ ��������� � ��������������� ���������
test_eval =  p_bitboard->eval;

// TEST
//std::cout << " N bitboard->eval = " << bitboard->eval << "\n";

// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
ZOBRIST::Start_position_random_key(p_bitboard);

// ���������� ���� ��� �������� ��� ����������� � �������
key_control = p_bitboard->hash_key;

//�������������� ��� - ������ �������� ������������� ������� ��� �� ��������� ������ ������������
ZOBRIST::Ini_hash_three(); 

// ���������� ���� ��� ������������ ������� �� ������ �������
ZOBRIST::Save_hash_three(p_bitboard->hash_key,0);

// TEST
// ����� ������� ���-�������
// �������������� ��� ����� � ���-������� 0
//   HASHM::Ini_hash_moves();

// ��������� ��� ������������ �������������� ��������
// TEST
//depth_max = depth_max - 1;

// TEST
//std::cout << "1111111 " << "\n";

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
//std::cout << "22222222222 " << "\n";
//std::cout << " depth_max " << depth_max << "\n";//�������

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
  TIMER::Time_start();

  if(p_bitboard->colour_move == 1){                                               
       value_root = FULLSEARCH::White_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
  }else{
       value_root = FULLSEARCH::Black_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
  }//if(bitboard->colour_move ==1){

// ������������� ������
  TIMER::Time_stop();
  time_current = TIMER::Time_delta();

nodes = FULLSEARCH::get_nodes();

//���������� ����� �� ���� ������� ��� ������ ���������� �����
//����� ������ ���������� ����� �� ���������� ��������
  nodes_y = nodes - nodes_previous ;

// �������� ������ ���������
if(test_eval != p_bitboard->eval) std::cout << "ERROR test_eval != bitboard->eval" << "\n";
 
//-----------------------------------------------
// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
// ��������� ������� �� ���� �� ���������� �� �������
    if(key_control != p_bitboard->hash_key){
	   	 std::cout <<"\n";
		 std::cout << "ERROR HESH Test_Deepening " ;
		 std::cout <<"\n";
	}//if(key != p_bitboard->hash_key){
//-----------------------------------------------

//---------------------------------------------------------------
// �������� ���������� � �����
  std::cout << "info " ;
  std::cout << " nps " << (__int32)(nodes / time_current);// �������� �� ������� �� ���� ����� �� ������ �������
  std::cout << " depth " << (__int32)depth;//�������

// �������� ���������� � ������� �������������� ��������
   std::cout << " seldepth "<< (__int32)FULLSEARCH::get_pv_best()->depth_max;

  p_bitboard->colour_move == 0 ? inv = -1 : inv = 1;

// �������� ������. ��� ���� ����������� ������������� � ���� ���������
  if(inv * value_root > 900000){ 
     flag_mate = 1;
     std::cout << " score mate "<< (__int32)FULLSEARCH::get_pv_best()->depth_max;// ��������� �� ���� � ���������
  }else if(inv * value_root < -900000){
     std::cout << " score mate "<< -(__int32)FULLSEARCH::get_pv_best()->depth_max;// ��������� �� ���� � ���������
     flag_mate = 1;
  }else{
     std::cout << " score cp "<< inv * value_root;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
     flag_mate = 0;
  }//if(inv * value_root > 900000){ 
 
// std::cout << " score cp "<< inv*value_root;// ��� ����� ������� ������

// �������� ����� ��������
 std::cout << " nodes " << (__int32)nodes; // ������ ���������� �����
//  std::cout << " nodes "<< nodes_y; // ���������� ����� �� ����
  std::cout <<" pv ";

  for( unsigned __int8 n = 0; n < FULLSEARCH::get_pv_best()->depth_max; n++){
      std::cout << UTIL::x_to_notation(((FULLSEARCH::get_pv_best()->move[n]>>12)& 63)%8);
      std::cout << (1 + ((FULLSEARCH::get_pv_best()->move[n]>>12)& 63)/8);
      std::cout << UTIL::x_to_notation(((FULLSEARCH::get_pv_best()->move[n]>>18)& 63)%8);
      std::cout << (1 + ((FULLSEARCH::get_pv_best()->move[n]>>18)& 63)/8);

//    ��������� ��� ����
      d_m = ((FULLSEARCH::get_pv_best()->move[n]>>6)& 63);

//    ���� d_m ������ 5 �� ������ ����� ����������� �����
      if(d_m > 5){
          if(d_m > 15){
               std::cout << UTIL::int_to_char_board((d_m - 20),0);// ����������� ����� � �������
          }else{
               std::cout << UTIL::int_to_char_board((d_m - 10),0);// ������ ����������� �����
          }//if(d_m > 15){
      }//if(d_m > 5){

      std::cout << " ";
   }//for( int n = 0; n < FULLSEARCH::get_pv_best()->depth_max; n++){

   std::cout <<"\n";

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
//std::cout << "cnt_move = " << cnt_move << "\n";
//std::cout << "wtime = " << wtime << "\n";
//std::cout << "time_terminate = " << time_terminate << "\n";
//std::cout <<"time_current = "<< time_current <<"\n";
//std::cout <<"   " <<"\n";

//-----------------------------------------------
// ��������� ���-���� �� ������������ �.�. ��� ����� ����������� � ������� �� �������� � 
// ��������� ������� �� ���� �� ���������� �� �������
    if(key_control != p_bitboard->hash_key){
	   	 std::cout <<"\n";
		 std::cout << "ERROR HESH Test_Deepening " ;
		 std::cout <<"\n";
	}//if(key != p_bitboard->hash_key){
//-----------------------------------------------


//---------------------------------------------------------------------
// ������ ��� ���� < bestmove e2e4 
// ��� ��� ����������� � ����(� �������)  < bestmove d7d8N 
std::cout << "bestmove ";
std::cout << UTIL::x_to_notation( FULLSEARCH::get_from() % 8);
std::cout << (1 + FULLSEARCH::get_from() / 8);
std::cout << UTIL::x_to_notation( FULLSEARCH::get_to() % 8);
std::cout << (1 + FULLSEARCH::get_to() / 8);

// ��������� ��� ����
d_m = ((FULLSEARCH::get_pv_best()->move[0]>>6)& 63);

// ���� d_m ������ 5 �� ������ ����� ����������� �����
if(d_m > 5){
   if(d_m > 15){
      std::cout << UTIL::int_to_char_board((d_m - 20),0);
   }else{
      std::cout << UTIL::int_to_char_board((d_m - 10),0);
   }//if(d_m > 15){
}//if(d_m != 5){
std::cout <<"\n";

// �������� ������������ ������� ���� ������� ���� ����� ��������
if( FULLSEARCH::get_from() != ((FULLSEARCH::get_pv_best()->move[0]>>12)& 63)) std::cout << "ERROR from(Deepening)" <<"\n";
if( FULLSEARCH::get_to() != ((FULLSEARCH::get_pv_best()->move[0]>>18)& 63))     std::cout << "ERROR to(Deepening)" <<"\n";
//---------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//void Deepening
//	*******************************************************************
