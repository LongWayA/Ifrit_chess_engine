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
#include "j_Killer.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"


//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 2.11.2009 20.12.2009
 *
 *  ������������
 * �������� ���������� ������� � ���� _TEST_LOOP.txt
 *
 * ��������� ��������� ������
 * ����������� � ��������� ��� ����� �����. ���� ����� ��� ��������� ���������
 * �� ���� ��� ������ ��������� � ������ ��� �������� ��� �������
 * ����� ��������������:
 * 1 ��������� ������� - ��������� �� ������������ ������������ ������� �� ��������� �� ���
 * 2 ����� �������� - ��������� �� ��� ���������� ��� �� ��� ���������
 * ��������� �� ���������� ������ � ������� �������� � ����� ���������� �������
 * 3 �����-���� ��������� - ��������� �� ��� �������� ��� �� ��� �������������
 *   ��� ���������� ��������� 
*/
//---------------------------------------------------------------------------


//	===================================================================
/*
----------------------------------------------------------------------------
 ��� �������� ��������� �������:
 1 � ������ ������������ ������ ���������� �����. ���������� ������ ��� ��� �������� �� ���
   � � ��� ���� ����� ���� ��� ��� ������ ��� ���
 2 ������ �� ��������� ���� ������ ������ ������ �� ���� � ��� �� ���-������� ���� �� ���� � ������
 3 ������ �� ��������� ��� � ��������� ������� �� ��� ���� 
   ��� ���� ��� ������ �� ��� ���������� ����� ���� ������
 4 ������������ ����������
 5 ����� ��������� � ������� ���������� ���
 6 ���� ����� �� ���� �� ��� ������ ���� ��� ����

*/

// NULL MOVE
#define NULL_YES 1 // �� ������ �������� ���� 
#define NULL_NO  0 // �� ������� �������� ���� 
#define NULL_DEPTH 2 // ��������� �� ��� ������ ������� NULL �� ����������
#define SWITCH_NULL_REDUCTION_1_2 5 // ������� �� ������� �������� �������� �������� ���� 
                                    // � NULL_REDUCTION_ONE �� NULL_REDUCTION_TWO
#define NULL_REDUCTION 4 // 4 ���������� ������� �������� ��� NULL
#define NULL_REDUCTION_ONE 4 // 4 ���������� ������� �������� ��� ������������� NULL
#define NULL_REDUCTION_TWO 3 // 3 ���������� ������� �������� ��� ������������� NULL
#define VERIFICATOIN_SEARCH_REDUCTION 5 // ���������� ������� �������� 
                                        //��� ������ �� ����������� ���������
// LMR
#define LMR_DEPTH 3 // ��������� �� ��� ������ ������� LMR �� ����������
#define LMR_REDUCTION_D 1 // ���������� ������� �������� ��� LMR
#define FULL_SEARCH 2 // ���� 2
                      // ����� ����� � ������ ������� ��� ���������� 
                      // ��� �� ���-�������, ������ ������,
                      // ��� �������, ��� ������ �� ������������
                      // ���-��� ��� ������� ��� 3 � ����� ���� ������ ��� � ���� 
                      // ���� ��� ������� ��� ������� �������� i > lmr_reduction



// ���������
#define CHECK_YES 1 // ���� ���
#define CHECK_NO  0 // ��� ����


#define MOVE_IS_CAPTURE 2 // ��� ��� ������
#define CAPTURE_NO  0 // ��� ������

#define PV_YES 1 // �������� �������
#define PV_NO  0 // 

#define VALUE_MATE  1000000 // ������ ����
#define DELTA_MATE  1000 // ������ ��� ���������� ���������� ����

// ����������� � ������ j_Hash.cpp
 #define EXACT 1 // ������ ���
 #define LOWER 2 // ������� �� ����(�����)
 #define UPPER 3 // �����(����) �������� �� �������

// ������ ���� �������� ���� �� ����������.-----------------------------

// FUTILITY PRUNING
# define FUTILITY_MARGIN 120 // ���� ������� ��� futility pruning
# define FUTILITY_DEPTH 1 // ������� ������������ ��� futility pruning

//-------------------------------------------------------------------------



// global
extern class TIMER timer;// ������. �������� ��������� �������
class BITBOARDGENERATOR bitboard_generator_o;// ��������� �����
class MOVES moves_o;// �������� �����
class QUIESCENCESEARCH quiescence_search_o;// ������������� �����
class SKILLER killer_o;// �������
class HASHM hash_o;// �����������
class PVSAVE pv_save_o;// ������ ��������� ��������
class EVAL eval_o;// ����������� ������ �������

extern class UTIL util_o;

//	===================================================================
// ����� �� �����. ����� ��������� ����� ��������
// ������� �� ������� ������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 FULLSEARCH::White_root(struct T_Bitboard * const p_bitboard,// ������������� �����
                           struct T_list_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
			               __int32 alpha,//
			               const __int32 beta,// 
			               const unsigned __int8 depth_max// ������������ �������
			               ){

__int32 value;// ������� ������ �������
struct T_undo undo[1];//���������� ������������ ��� �������� ����
__int32 value_max = -9999999;// ������ � ����  ������ �������  
unsigned __int8 depth_max_new = 0;//����� ������� ��������
bool flag_save_best = 0;// ����� �� ������ �������
bool flag_is_legal_moves = 0;// ���� ������������� ��������� ����� (���� 0 �� ��������� ����� �� ����)
bool flag_check = 0;//���� ����
const unsigned __int8 depth = 0;// �������
const unsigned __int8 new_depth = 1; // ��������� �������

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	struct T_PV pv_best_point;// ������ ��� ������� ���� �������
    struct T_Bitboard test_bitboard;// ������������� �����. ��������� ��� �����
    unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
    const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
    unsigned __int8 i_hash_move = 0; // ����� ����������� ����
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

depth_max_rem = depth_max;
audit = 0;// �������� ������ ������� ����������� ������
lmr_reduction = 0;//
flag_stop_search = 0;//

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
		// ��� ������� ��� ���������� ������
		if (flag_stop_search == 1) return 0;

// �������������� ������ ��������
killer_o.Ini_killer(); 

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

Sorting_moves(&list_surplus_moves);
//Sorting_white_moves_root(&list_surplus_moves);// ��������� ����

	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// �������� ������ ������ � ����� �� ������� ������
	#if (TEST_L && SAVE_LINE)
         List_Print(depth_max,&list_surplus_moves);
        //if(depth_max == 4) List_Print(depth,&list_surplus_moves);
		//Bitboard_print(p_bitboard);
	#endif//#if TEST_L
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if (HASH && SAVE_LINE)
		// �������� � ���-��������
		// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
		// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
		hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

	// ��� ������� ����� ����� ��������� ������� �� ������
    #if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

//cout << "���� ������ ����� flag_insert_hash_move " << flag_insert_hash_move << "\n";

       if (flag_insert_hash_move == 0) {
          cout << "���� �� ����� ������ �����++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
       }//if (flag_insert_hash_move == 0) {

       if (flag_insert_hash_move == 1) {
           cout << "�������� ���� ������ �����++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 1) {
    #endif//#if HASH_TEST

	#endif//#if HASH
	//-----------------------------------------------

	////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	//// �������� ������ ������ � ����� �� ������� ������
	//#if (TEST_L && SAVE_LINE)
 //       List_Print(depth,&list_surplus_moves);
	//	//if(depth_max == 4) List_Print(depth,&list_surplus_moves);
	//	//Bitboard_print(p_bitboard);
	//#endif//#if TEST_L
	////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	//------------------------------------------------------------
    //���������� ����� ��� ���������� �������� �� ������������
    //����� �������� � ������� 
    Save_test_bitboard(p_bitboard,&test_bitboard);
	//------------------------------------------------------------

/*
 ��� ������
 0 - ��� ������
 1 - ����� 
 2 - ���� 
 3 - ����
 4 - ����� 
 5 - ����� 
 6 - ������
*/
 //   // ��������� ���������� � ��� ����� ���� ������ ��� ����� �� ��������� ������� ���
	//if( (p_bitboard->white_piece[5] == 0) && // ���� � ����� ��� ������
 //       (p_bitboard->white_piece[4] == 0) && // ��� �����
 //       (p_bitboard->white_piece[3] == 0)  // ��� ������
	//   ){ 
 //       //   cout <<"������� ��� ����� ��������� ";
	//       //cout <<"\n";
	//       flag_stop_null_move = 0; // �� ��������� ������� ���
	//}else {
	//       flag_stop_null_move = 1; // ����� �� �������
	//}// if( p_bitboard->white_piece[5] == 0 &&

flag_stop_null_move = 1;

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

  // ��������� ��� �� ������ ��� ������� ������� ���������� �������
  moves_o.Onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);
 // Bitboard_print(p_bitboard);

  // ���� ������ ��� ����� �� (�������� ����� ���������� ��� �������� ������)
  if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
     // �� �������� ���� ��� ��� ������������
     moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);
// ���������� ������ � �������� ������ �����
list_surplus_moves.sorting_score[i] = -1000000;

  }else{

  // ������ ���� ��� ���� ��������� ���� � ����������� �������
     flag_is_legal_moves = 1;
     nodes = nodes + 1;
     depth_max_new = depth_max;

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	    // �������� ���������� � ��������������� ���� � ������� ��������
	    Print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

	    // �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������
	    pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	    // ��������� ��� ����
	    hash_o.Modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

// TEST-----------
// ��� ���� ��������� ��������� �� �����
// ���������� ���-�������
// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
//hash_o.Start_position_random_key(p_bitboard);
//----------------

		// ���������� ���� ��� ������������ ������� �� ������ �������
        hash_o.Save_hash_three(p_bitboard->hash_key,new_depth);

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
    
	//-----------------------------------------------
	#if (EXTENSION_CHEK && SAVE_LINE)
	// ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
	    Extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
	#endif//#if EXTENSION_CHEK
	//-----------------------------------------------


#if (ALPHA_BETA_ON && SAVE_LINE)
	// ����� � ��� ������������ ���������(NegaScout)
	// ������ ����� ������� ������� � ������� � ������ ����� ��������� ��������
    // ������ ������� � ������� ����� � ���� ����� ������ ���������� �� �������������
	// � ������ �����
	if (i == 0){
#else
    // � ������ ������� �������� ������� ��� � ��� ����� ������� ��������
    if (1){
#endif//#if ALPHA_BETA_ON

            //����������� ����� �������
            value = Black(p_bitboard,alpha,beta,depth_max_new,
			              new_depth,((list_surplus_moves.move[i]>>6) & 63),
					      PV_YES,flag_check,NULL_NO);

    }else{

             // �������� �������  � ������� �����. ��� ��� �� ������� �������
	         value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
			               new_depth,((list_surplus_moves.move[i]>>6) & 63),
						   PV_NO,flag_check,NULL_NO);

	         if(value > alpha){

        	      // ���� ����� alpha ������� �� ������� �� ����� ��� ������� �������
		          value = Black(p_bitboard,alpha,beta,depth_max_new,
					            new_depth,((list_surplus_moves.move[i]>>6) & 63),
								PV_YES,flag_check,NULL_NO);

	         }//if (value > alpha)
    }//if (i == 0)

 // ��������������� �������
    moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

// ���������� ������ � �������� ������ �����
list_surplus_moves.sorting_score[i] = value;

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE

// ��������������� ���-���� �����
p_bitboard->hash_key = key_undo;

   // ���� ������ �������. �� ����� ���� ��������
      if (value > value_max){
         value_max = value;
         i_hash_move = i;
         from = ((list_surplus_moves.move[i]>>12)& 63);
         to = ((list_surplus_moves.move[i]>>18)& 63);

        // TEST
      
			 
			 //-----------------------------------------------
        //       cout << "�������� ��������. ��� ���������� � ��� " << "\n";
        //       cout << "depth= " << depth << "\n";
        //       cout << "depth_max_rem= " << depth_max_rem << "\n";
        //       cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				    //<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
        //       cout << "������� ������ " << util_o.int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
        //       
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
        //       cout << "  " << "\n"; 
               //-----------------------------------------------



         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
         // ����� �������� � ������ � �������� �� ��������
         #if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
	         if (value > alpha){ 
		        alpha = value;       
		        flag_save_best = 1;
                // ����� �� �������������� ������ ������� ��������������� ������ ������
                pv_save_o.Update_PV_best_point(&pv_best_point);
	            // ��������� ���������� �������� � ������
                if(value != pv_save_o.get_pv_best()->score_move){
		           cout << "ERROR white value " ;
		           cout << "\n";
	            }//if(value != pv_best.score_move){

        // TEST
        //-----------------------------------------------
        //       cout << "�������� �����. ��� ���������� � ��� " << "\n";
        //       cout << "depth= " << depth << "\n";
        //       cout << "depth_max_rem= " << depth_max_rem << "\n";
        //       cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				    //<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
        //       cout << "������� ������ " << util_o.int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
        //       
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
        //       cout << "  " << "\n";     
               //-----------------------------------------------

	         }//if (value > alpha){ 
         #else///#if ALPHA_BETA_ON
            flag_save_best = 1;
            // ����� �� �������������� ������ ������� ��������������� ������ ������
            pv_save_o.Update_PV_best_point(&pv_best_point);

            // ��������� ���������� �������� � ������
            if (value != pv_save_o.get_pv_best()->score_move){
		        cout << "ERROR white value " ;
		        cout << "\n";
	        }//if(value != pv_best.score_move){

         #endif//#if ALPHA_BETA_ON
         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

    }//if (value > value_max){

	  //--------------------------------------------------------------------
	  // ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� �.�. � ����� �����������
	  if (flag_stop_search != 1){
	     nodes_0 = nodes_0 + 1;
	     #if TEST_PRINT_ALL_LINE
	        // ��� ����� ���������
	        Print_pv_line(1,&list_surplus_moves,pv_save_o.get_pv_best(),nodes,i,depth_max,value,time_nodes);
	     #else
	        // ��� �������� ������
	        //Print_pv_line(1,&list_surplus_moves,&pv_best_point,nodes,i,depth_max,value_max);
	     #endif//#if TEST_PRINT_ALL_LINE
       }//if (flag_stop_search != 1){
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

  }//if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

}//for (int i = 0; i < list_surplus_moves.end_list; i++){

//------------------------------------------------------------
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
         // ��������� �������������� �� ����� ����� ����������� �������
         Compare_test_bitboard(p_bitboard,&test_bitboard);
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
//------------------------------------------------------------

// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (flag_is_legal_moves == 0){
    value = 0;
    if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
      value = -(VALUE_MATE - DELTA_MATE * depth);//
     }//if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

    // � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
    pv_save_o.Update_PV_best_max_depth(value,depth);

    return value;
}//	if (flag_is_legal_moves == 0){

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      if(flag_save_best == 1){
          // ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
          pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(flag_save_best == 1){
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    #if HASH
       // �������� ������ ���
       hash_o.Save_best_hash(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
				             value_max, UPPER, depth, depth_max);
    #endif//#if HASH

return value_max;

}//int FULLSEARCH::White_root
//	*******************************************************************

//	===================================================================
// ������� �� ������� ������ �� ������ ���� �������
// ������� �� �� ������ ���������� � ��������. ��������� ������ ��� ������ �� ������� ������
__int32 FULLSEARCH::Black_root(struct T_Bitboard * const p_bitboard,// ������������� �����
                           struct T_list_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
			               const __int32 alpha,//
			               __int32 beta,// 
			               const unsigned __int8 depth_max// ������������ �������
			               ){

__int32 value;              // ������� ������ �������
struct T_undo undo[1];//���������� ������������ ��� �������� ����

__int32 value_min = 9999999;// ������ � ����  ������ �������  
unsigned __int8 depth_max_new = 0;//����� ������� ��������
bool flag_save_best = 0;// ����� �� ������ �������
bool flag_is_legal_moves = 0;// ���� ������������� ��������� ����� (���� 0 �� ��������� ����� �� ����)
bool flag_check = 0;//���� ����

const unsigned __int8 depth = 0;// �������
const unsigned __int8 new_depth = 1; // ��������� �������

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	  unsigned __int8 nodes_0 = 1;            // ���������� ����� �� ������� ������
	  struct T_PV pv_best_point;// ������ ��� ������� ���� �������
      struct T_Bitboard test_bitboard;// ������������� �����. ��������� ��� �����
	  unsigned __int8  flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
      const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
      unsigned __int8 i_hash_move = 0; // ����� ����������� ����
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

depth_max_rem = depth_max;
audit = 0;// �������� ������ ������� ����������� ������
lmr_reduction = 0;//
flag_stop_search = 0;//

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
   // ��� ������� ��� ���������� ������
        if (flag_stop_search == 1) return 0;

// �������������� ������ ��������
killer_o.Ini_killer(); 

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

Sorting_moves(&list_surplus_moves);
//Sorting_black_moves_root(&list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// �������� ������ ������ � ����� �� ������� ������
	#if (TEST_L && SAVE_LINE)
     List_Print(depth_max,&list_surplus_moves);
	 //if(depth_max == 3) List_Print(depth,&list_surplus_moves);
	//	Bitboard_print(p_bitboard);
	#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if (HASH && SAVE_LINE)
	// �������� � ���-��������
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// �.�. �� � ������ ��� �� ���-������� ������ ������ � ��������� ���� ��������
	hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

    #if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

      if (flag_insert_hash_move == 0) {
          cout << "���� �� ����� ������ ������++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
       }//if (flag_insert_hash_move == 0) {

       if (flag_insert_hash_move == 1) {
           cout << "�������� ���� ������ ������++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 0) {

    #endif//#if HASH_TEST
	#endif//#if HASH



	//-----------------------------------------------

////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//	// �������� ������ ������ � ����� �� ������� ������
//	#if (TEST_L && SAVE_LINE)
//	 if(depth_max == 3) List_Print(depth,&list_surplus_moves);
//	//	Bitboard_print(p_bitboard);
//	#endif//#if TEST_L 
////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	//------------------------------------------------------------
    //���������� ����� ��� ���������� �������� �� ������������
    Save_test_bitboard(p_bitboard,&test_bitboard);
	//------------------------------------------------------------
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

 //   // ��������� ���������� � ��� ����� ���� ������ ��� ����� �� ��������� ������� ���
	//if( (p_bitboard->black_piece[5] == 0) && // ���� � ����� ��� ������
 //       (p_bitboard->black_piece[4] == 0) && // ��� �����
 //       (p_bitboard->black_piece[3] == 0)  // ��� ������
	//   ){ 
 //       //   cout <<"������� ��� ������ ��������� ";
	//       //cout <<"\n";
	//       flag_stop_null_move = 0; // �� ��������� ������� ���
	//}else {
	//       flag_stop_null_move = 1; // ����� �� �������
	//}// if( p_bitboard->white_piece[5] == 0 &&

flag_stop_null_move = 1; // ����� �� �������


//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for(unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

  // ��������� ��� �� ������ ��� ������� ������� ���������� �������
   moves_o.Onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);
// Bitboard_print(p_bitboard);

// ���� ������ ��� ����� ��
   if (bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
    // �� �������� ���� ��� ��� ������������
       moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);
// ���������� ������ � �������� ������ �����
list_surplus_moves.sorting_score[i] = 1000000;
   }else{
    // ������ ���� ��� ���� ��������� ���� � ����������� �������
       flag_is_legal_moves = 1;
       nodes = nodes + 1;
       depth_max_new = depth_max;

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
   	//    �������� ���������� � ��������������� ���� � ������� ��������
	      Print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

	//    �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	      pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	//    ��������� ��� ����
	      hash_o.Modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);

// TEST-----------
// ��� ���� ��������� ��������� �� �����
// ���������� ���-�������
// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
//hash_o.Start_position_random_key(p_bitboard);
//----------------

		  // ���������� ���� ��� ������������ ������� �� ������ �������
          hash_o.Save_hash_three(p_bitboard->hash_key,new_depth);

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

	   //-----------------------------------------------
	   #if (EXTENSION_CHEK && SAVE_LINE)
	   // ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
	      Extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);
	   #endif//#if EXTENSION_CHEK
	   //-----------------------------------------------

    #if (ALPHA_BETA_ON && SAVE_LINE)
	  if (i == 0) {
    #else
      if (1) {
    #endif//#if ALPHA_BETA_ON

               value = White(p_bitboard,alpha,beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_YES,flag_check,NULL_NO);

      }else{

	           //����������� ����� �������
               value = White(p_bitboard,(beta - 1),beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
						     PV_NO,flag_check,NULL_NO);

               if(value < beta){

	               //����������� ����� �������
		           value = White(p_bitboard,alpha,beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 PV_YES,flag_check,NULL_NO);

	           }//if(value < beta)

      }//if (i == 0)

//     ��������������� �������
       moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

// ���������� ������ � �������� ������ �����
list_surplus_moves.sorting_score[i] = value;


	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE

// ��������������� ���-���� �����
p_bitboard->hash_key = key_undo;

        //���� ������ ������� �� ������ ���� �������
        if(value < value_min){
           value_min = value;
           i_hash_move = i; // ����� ����������� ����
           from = ((list_surplus_moves.move[i] >> 12) & 63);
           to = ((list_surplus_moves.move[i] >> 18) & 63);
  
           //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
           #if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
              // ������ �������� �������� ����
              if (value < beta){
                 beta = value;
                 flag_save_best = 1;
                 // ����� �� �������������� ������ ������� ��������������� ������ ������
                 pv_save_o.Update_PV_best_point(&pv_best_point);
	             // ��������� ���������� �������� � ������
                 if (value != pv_save_o.get_pv_best()->score_move){
		            cout << "ERROR black value " ;
		            cout << "\n";
	             }//if(value != pv_best.score_move){

              }//if (value < beta){
           #else//#if ALPHA_BETA_ON
             flag_save_best = 1;
             // ����� �� �������������� ������ ������� ��������������� ������ ������
             pv_save_o.Update_PV_best_point(&pv_best_point);

	         // ��������� ���������� �������� � ������
             if (value != pv_save_o.get_pv_best()->score_move){
		         cout << "ERROR black value " ;
		         cout << "\n";
	         }//if(value != pv_best.score_move){

           #endif//#if ALPHA_BETA_ON
           //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        }//if (value < value_min){ 

	   //--------------------------------------------------------------------
	   // ������� ��� ������� ���������� ������ ��� ������ �� ������� ������� �.�. � ����� �����������
	   if (flag_stop_search != 1){
	       nodes_0 = nodes_0 + 1;
	       #if TEST_PRINT_ALL_LINE
	        // ��� ����� ���������
	          Print_pv_line(0,&list_surplus_moves,pv_save_o.get_pv_best(),nodes,i,depth_max,value,time_nodes);
	       #else
	        // ��� �������� ������
	          //Print_pv_line(0,&list_surplus_moves,&pv_best_point,nodes,i,depth_max,value_min);
	       #endif//#if TEST_PRINT_ALL_LINE
        }//if (flag_stop_search != 1){

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

  }//if (bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){

}//for(int i = 0; i < list_surplus_moves.end_list; i++){

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
         // ��������� �������������� �� ����� ����� ����������� �������
         Compare_test_bitboard(p_bitboard,&test_bitboard);
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (flag_is_legal_moves == 0){
  value = 0;
  if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
    value = (VALUE_MATE - DELTA_MATE * depth);// ����� ��� ������� ������� �����?
  }//if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){

  // � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
  pv_save_o.Update_PV_best_max_depth(value,depth);

  return value;
}//	if (flag_is_legal_moves == 0){

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      if(flag_save_best == 1){
          // ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
          pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(flag_save_best == 1){
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    #if HASH
      //�������� ������ ���
      hash_o.Save_best_hash(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
				            value_min, UPPER, depth, depth_max);
    #endif//#if HASH


return value_min;
}//int FULLSEARCH::Black_root
//	*******************************************************************


//	===================================================================
// ���� �������� � ����. ������� �� ����� 
__int32 FULLSEARCH::White(struct T_Bitboard * const p_bitboard,// ������������� �����
					  __int32 alpha,//
		              const __int32 beta,//
		              const unsigned __int8 depth_max,// ������������ �������
		              const unsigned __int8 depth,// �������
		              const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		              const bool flag_pv_f,// �������� �� ��� ������� (pv)
		              const bool old_flag_check,//���� ����
		              const bool flag_null_m// ���� �������� ����
		              ){

__int32 value;              // ������� ������ �������
__int32 value_max = -9999999;   // ������ ������� ������ � ���� 
struct T_undo undo[1]; // ��������� ��� �������� ����
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
bool flag_is_legal_moves = 0;           // ���� ������������� ��������� �����
bool flag_save_best = 0;           // ����� �� ������ �������
const unsigned __int8 new_depth = depth + 1; // ��������� �������

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      unsigned __int8 depth_max_new = 0;//����� ������� ��������
      bool flag_check = 0;//���� ����
	  struct T_PV pv_best_point;// ������ ��� ������� ���� �������

	  bool taking_passage_yes_mem = 0;// ��������� ��� ����������� ����� ������� �����
	      unsigned __int8 taking_passage_y_mem = 0;
	      unsigned __int8 taking_passage_x_mem = 0;
	  
      // ��� ��� �����������
      const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
      unsigned __int8  flag_insert_hash_move = 0; // ��������� �� ��� �� ���-������� �� ������ �������
      __int32 hash_score = -1000000000;// ���-������ �������
      unsigned __int8 i_hash_moves_1;// ��� ������� �������� ����� (1- �������� �����) ����� ��� ����������� 
      unsigned __int8 i_hash_moves_3;// ��� ������ ��������� �������� (3-������ ��������� �������� )����� ��� �����������
      unsigned __int8 hash_moves_test13;// ��� �����
      unsigned __int8 test_flag_hash_moves;// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
      unsigned __int8 hash_depth;// ������������ �������
      unsigned __int8 hash_depth_max;// ������������ �������
//      unsigned __int64 test_key_control;
//      int dynamic_null_move_reduction;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss


	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (flag_stop_search == 1) return 0;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss


//--------------------------------------------
// �� ���������� ������� ���������� ����������� ������ �������
if (depth >= depth_max){
 	   //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	   #if SAVE_LINE
         value = Max_depth_white(p_bitboard,alpha,beta,flag_pv_f,depth);
	   #endif//#if SAVE_LINE 
	   //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
       return value ;
}//	if (depth >= depth_max){
//--------------------------------------------

//-----------------------------------------------
    #if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
      // ���������� ���������� �������
	  if ( hash_o.Analysis_hash_string_moves(depth) ){
        // � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
        if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(0 // ������ �����
			                                             ,depth);
		return 0;
	  }//if ( hash_o.Analysis_hash_string_moves(depth) )
    #endif//#if ALPHA_BETA_ON
//-----------------------------------------------

	//--------------------------------------------
	#if (FUTILITY_PRUNING && SAVE_LINE)
     //
	 if((depth_max - depth) == FUTILITY_DEPTH){
		if( (description_move != MOVE_IS_CAPTURE)
			&& (flag_check == CHECK_NO) 
			&& (flag_pv_f == PV_NO)){

			if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) - FUTILITY_MARGIN) 
				>= beta ){
				return beta;
			}//	if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) - FUTILITY_MARGIN) >= beta )

		}//	if((flag_null_m == NULL_NO) && (description_move != MOVE_IS_CAPTURE)

	  }//if((depth_max - depth) == FUTILITY_DEPTH)
	#endif//#if FUTILITY_PRUNING
	//--------------------------------------------



	//--------------------------------------------
#if (HASH_SCORE)
	#if (HASH && SAVE_LINE)
// ������� �� ��� ������ �� ���������� ��� �������� ��������
// ������� ������� ��� ������������� ������ � ����� �������� � ��� ������� �������
// ������ ���������� ������� ������� ������������.
      if(!flag_pv_f){ 
	     // ������� �� ����������� �� ��� ����� ������� ���� ����������� �� 
		 // �������� ��������� ����������� ������
	     hash_score = hash_o.Look_hash_score_white(
			                          p_bitboard->hash_key,depth,depth_max,
									  alpha,beta,test_flag_hash_moves,hash_depth,
									  hash_depth_max);

		  if (hash_score != -1000000000 ) {
//return_hash_score = return_hash_score + 1 ;
           #if (!HASH_TEST_SCORE)
			  return hash_score;
           #endif//#if (!HASH_TEST_SCORE)

		  }//if (hash_score != -1000000000 ) {
       }//if(!flag_pv_f){
     #endif//#if HASH
 #endif//#if (HASH_SCORE)
	//--------------------------------------------

	//--------------------------------------------
	//if(!flag_pv_f){
 //     // ���� �������� �� ������ :)
 //     // ���� � ��� ��� ������� � ������� 8 �������� ���������� ���� ������ �� ���������
 //    if ( depth > 8 )
 //        if((p_bitboard->eval - 700) >= beta ) return beta;

	//}//if(!flag_pv_f){
	//--------------------------------------------


	//--------------------------------------------
	#if (NULL_MOVE && SAVE_LINE)
	// ����� ����������� ��������� ��������� ��� ������ �������� ����
    // � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
    // ���� ��� � ��� ���
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������

    if ((flag_stop_null_move == NULL_YES) // ���� �������� ��� �� ������
	 && (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
	 && (depth_max > NULL_REDUCTION ) // ����� ������ � ����� � � ��� ������� ������ ������������
	 && ((depth_max - depth) >= NULL_DEPTH ) // ���� �� ������������ ������� ���������� �����������
	 && (flag_pv_f == PV_NO) // ���� �� �� � ������� ��������
	 && (description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
	 && (old_flag_check == CHECK_NO) // ���� ���� ���
//	 && (p_bitboard->eval >= beta) // ����� ������ �������������. ������ �� �������
                                   // �������� ���� � ��� ��� � �� ��������� �����������
								   // ������������ ������ � ������ ��������
		){ // ����� �������� ������� ���

// ���������� ����� ������ �� ������� � �������� ����������.
		taking_passage_yes_mem = p_bitboard->taking_passage_yes;
		taking_passage_x_mem = p_bitboard->taking_passage_x;
		taking_passage_y_mem = p_bitboard->taking_passage_y;

// ������ ���-���� ��� ������� ��� -------------------------
// ��������� ������ �� �������
if( p_bitboard->taking_passage_yes ){
	p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//if( p_bitboard->taking_passage_yes )

// ��������� ���� ����
// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
// ��������� ������� ���������� ������ ������ ����
 p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);
//---------------------------------------------

		p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

        p_bitboard->colour_move = !p_bitboard->colour_move;

        const __int64 nodes_null_rem = nodes;

// TEST ���������� ��������� ���-�����
// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
//hash_o.Start_position_random_key(p_bitboard);

// �� ������������ ��������
value = Black(p_bitboard,alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
			         new_depth,description_move, 
					 PV_NO,old_flag_check,NULL_YES); 

//--------------------------------------------------------
// ������������ �������� �������� ���� ��� ��������� �� ������������ �������
//������ ������ 5 ���������� 4 ����� 3
//dynamic_null_move_reduction = 
// ((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
// ? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
//
//value = Black(p_bitboard,alpha,(alpha + 1),(depth_max - dynamic_null_move_reduction),
//			         new_depth,description_move, 
//					 PV_NO,flag_check,NULL_YES); 
//--------------------------------------------------------

// ���� ����������� � ������� ������ �� ���������
        nodes = nodes_null_rem;

// TEST
//cout << "p_bitboard->eval "<< p_bitboard->eval << "\n";
//cout << "i_move= "<< i_move << "\n";
//cout << "description_move= "<< description_move << "\n";
//cout << "check= "<< check << "\n";
//cout << "       "<< "\n";
//
//if(value < beta){
//cout << "�������� "<< "\n";
//cout << "depth_max= "<< depth_max <<"\n";
//cout << "depth= "<< depth <<"\n";
//cout << "value= "<< value <<"\n";
//cout << "beta= "<< beta <<"\n";
//cout << "       "<< "\n";
//}

		p_bitboard->taking_passage_yes = taking_passage_yes_mem;
		p_bitboard->taking_passage_x = taking_passage_x_mem ;
		p_bitboard->taking_passage_y = taking_passage_y_mem ;

        p_bitboard->colour_move = !p_bitboard->colour_move;

// ��������������� ���-���� �����
p_bitboard->hash_key = key_undo;

		//��� ��� ������ ��� ������� ��� �������� ����� ����������� ������ �.�. �������� ����
		//����� ���������� ���� ��� ��� ����� ������� ������ ����
		//������ ����� ��� ������ � ���� �� �� ������ �������� ���� �.�. ���� ������ ������
		//��� ����� ���� ������ ����� ������ �.�. ����� ��� �� ���������
		//������ ��� ���� ���� � ����� ����� �� ������ �������� �� ��� ���� � ������� �� ������ 
		if(value >= beta){

			  //TEST
              //cout << "������� "<< "\n";
              //cout << "depth_max= "<< depth_max <<"\n";
              //cout << "depth= "<< depth <<"\n";
              //cout << "value= "<< value <<"\n";
              //cout << "beta= "<< beta <<"\n";
              //cout << "       "<< "\n";

//         return beta;

//------------------------------
if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1) ) return beta;

       // ������� ������ ��������
       value = White(p_bitboard,(beta - 1),beta,(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
			          depth,description_move, 
					  PV_NO,old_flag_check,NULL_YES);

       if(value >= beta)  return beta;
//------------------------------

		}//if(value >= beta)

	}//    if ((flag_null_m == NULL_NO) && ((depth_max - depth) >= NULL_DEPTH ) && 

	#endif//#if NULL_MOVE
	//--------------------------------------------


//����������� ������ ���������� �����
// ��������� ���� ������� � ������� ������ ��������
bitboard_generator_o.Generator_captures_white(p_bitboard,&list_surplus_moves);
bitboard_generator_o.Generator_move_white(p_bitboard,&list_surplus_moves);
Sorting_moves(&list_surplus_moves);

 
	//-----------------------------------------------
	#if (KILLER && SAVE_LINE)
	// ���� ��� ������ ��� ������ ������
    //�� ������ ��������� �� ������ ������� ����� �� ������
    if((list_surplus_moves.end_captures == CAPTURE_NO) || 
		(list_surplus_moves.sorting_score[0] < 0)){

		killer_o.Insert_killer(&list_surplus_moves,depth,0);

	}else{

		killer_o.Insert_killer(&list_surplus_moves,depth,1);

	}//   if((list_surplus_moves.end_captures == CAPTURE_NO) || 

	#endif//#if KILLER
	//-----------------------------------------------

	//--------------------------------------------
	#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	// ������ �� ��������� ��� �� ��� ������� �� ������ ����� ������� ��������� ����
	hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

	// ��� ������� ����� ����� ��������� ������� �� ������
    #if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

       if (flag_insert_hash_move == 1) {
           cout << "�������� ���� �����" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 0) {
    #endif//#if HASH_TEST

	#endif//#if HASH
	//--------------------------------------------

#if (LMR && SAVE_LINE)
   // ��� �� ���-�������, ������ ������, ��� �������, ��� ������ �� ������������
   lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
   //lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR

//int r;

//����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

//--------------------------------------------
// � ����� ��������� ������� ��� �� ������� �� � �� ��� ���������
// � ��� ������ ����� ���� ������ ������ �������� �� �����
	//if(!flag_pv_f){
 //      // ���� �������� ����� ������� ��� ����� ���(lmr) :)
 //     // ���� � ��� ��� ������� � ������� 6 �������� ���������� ��������� ����
	//  // �� ������ ���� �������� � ����� ������
	// r = 5 + list_surplus_moves.end_list - depth;
	// if ( r < 0) r = 0;
 //    if ( i >= 4 + r ) continue;
	//}//if(!flag_pv_f){
//--------------------------------------------

  // ��������� ��� �� ������ ������� ������� ���������� �������
  moves_o.Onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);

  // ���� ������ ��� ����� ��
  if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

     // �� �������� ���� ��� ��� ������������
     moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

  }else{

   // ������ ���� ��� ���� ��������� ���� � ����������� �������
   flag_is_legal_moves = 1;
   nodes = nodes + 1;

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	 // �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	 if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	 hash_o.Modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY

  unsigned __int64 test_key_control = p_bitboard->hash_key;

 //��� ���� ��������� ��������� �� �����
 //���������� ���-�������
 //�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
  hash_o.Start_position_random_key(p_bitboard);

  if(test_key_control != p_bitboard->hash_key){
       cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
       cout << "collision white depth = " << depth << "\n";
       exit(1);
  }//if(key_control != p_bitboard->hash_key){

#endif//#if HASH_TEST_KEY
//----------------------------

	 // ���������� ���� ��� ������������ ������� �� ������ �������
     hash_o.Save_hash_three(p_bitboard->hash_key,new_depth); 

	 // ������ ��� ���������� ����� ������������ ������� ��� �� ���������� ����� 
	 // � �� ���� ����
     depth_max_new = depth_max;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

   //-----------------------------------------------
   #if (EXTENSION_CHEK && SAVE_LINE)
	// ���� ������ ��� ����� �� ����������� �� 1 ������������ ������� ��������
	    Extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
   #endif//#if EXTENSION_CHEK
   //-----------------------------------------------

	//-----------------------------------------------
	#if (EXTENSION_CAPTURES && SAVE_LINE)
      if(flag_pv_f == PV_YES){//

//         if (description_move == 2){

             // ���� ��� ��� ������
             if (((list_surplus_moves.move[i]>>6) & 63) == 2){
                 // ���� ������ ������ ����� �������
                 if ( ((list_surplus_moves.move[i]>>24) & 7) // �����
                        <= (list_surplus_moves.move[i] & 63) ){ // ������

                           depth_max_new = depth_max + 1;
                          //Extension_for_captures(((list_surplus_moves.move[i]>>6) & 63)
		                  //                  ,description_move,depth_max,depth_max_new);

				 }//if ( ((list_surplus_moves.move[i]>>24) & 7)

			 }//if (((list_surplus_moves.move[i]>>6) & 63) == 2){

//		 }//if (description_move == 2){

	  }//if(flag_pv_f == PV_YES)
    #endif//#if EXTENSION_CAPTURES
	//-----------------------------------------------

// ������� ������� ��������� �� ������������ ������ 
// � ������������ ������� � ���� ������������ ��������
// ���� � ��� ��� � ���� ��� ��������� �� ���� � ������������ ��� ����
// �� ����� ���������������� �������
  //   if(flag_pv_f == PV_YES){//

  //      if ((flag_check) // ���� ��� ���
		//	&&(list_surplus_moves.end_list < 10)  // � ����� ������ 10
		//	&&(depth_max_new < 51) ){			
  //     //   cnt_single = 0; 
		//		depth_max_new = depth_max + 1;
		//}//if ((flag_check) // ���� ��� ���

	 // }//if(flag_pv_f == PV_YES)




//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
 #if !SAVE_LINE
    // ����������� ����� �������
    value = Black(p_bitboard,alpha,beta,depth_max,new_depth,
		          ((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
				  flag_check,flag_null_m);
 #endif//#if !SAVE_LINE


//------------------------------------------------------
    #if (!LMR && SAVE_LINE)

       #if (ALPHA_BETA_ON && SAVE_LINE)
	      if (i == 0) {
       #else
          if (1) {
       #endif//#if ALPHA_BETA_ON

             value = Black(p_bitboard,alpha,beta,depth_max_new,new_depth,
			              ((list_surplus_moves.move[i]>>6) & 63),
					        flag_pv_f,flag_check,flag_null_m);

          }else{

             // ����������� ����� �������
	         value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
// TEST
//value = Black(p_bitboard,alpha,beta,depth_max_new,
				           new_depth,((list_surplus_moves.move[i]>>6) & 63),
						   PV_NO,flag_check,flag_null_m);

// ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
// ����� ������� ������� ���
// � ����������� ����� ����� ���� ��� ���� ������� �� ������������� �� �������
	         if((value > alpha) && (value < beta)){
 
            
//cout << "white depth = " << depth << "\n";
//cout << "value = " << value << "\n";
//cout << "alpha = " << alpha << "\n";
//cout << "beta = " << beta << "\n";

		          value = Black(p_bitboard,alpha,beta,depth_max_new,
					           new_depth,((list_surplus_moves.move[i]>>6) & 63),
							   flag_pv_f,flag_check,flag_null_m);

//cout << "value mod = " << value << "\n";
//cout << "   " <<"\n";

	         }//if ((value > alpha) && (value < beta))
          }//if (i==0)
    #endif//#if !LMR
    //------------------------------------------------------

	//------------------------------------------------------
	#if  (LMR && SAVE_LINE)

	  if(flag_pv_f){
          // ���� �� � ������� �������� �� ������� �� �������� � ������� �����
		  // �.�. �� alpha �� beta = alpha + 1

	      if (i == 0) {
            // ������ ��� ������� �� ������ ������� � � ������ �����
		    // �� ������������ ��� ���� �� ������ �� ������ � ������� ������� ���������
             value = Black(p_bitboard,alpha,beta,depth_max_new,
				           new_depth,((list_surplus_moves.move[i]>>6) & 63),
					       PV_YES,flag_check,flag_null_m);

          }else{

             // �������� �������� ��������� � ������� �����
	         value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				           new_depth,((list_surplus_moves.move[i]>>6) & 63),
						   PV_NO,flag_check,flag_null_m);

              // ���� ������ � ������� ����� � ��� ��� �� �� ���� ��� ������� ������� 
              // ����� ������� ������� ��� � ����������� ����� �.�. ������� �����
              //  ���� ��� ���� ������� �� ������������� c ������ �����
	         if(value > alpha){
                  //
		          value = Black(p_bitboard,alpha,beta,depth_max_new,
					           new_depth,((list_surplus_moves.move[i]>>6) & 63),
							   PV_YES,flag_check,flag_null_m);

	         }//if (value > alpha)
		  }//if (i == 0) 

	  }else{//if(flag_pv_f)
          // ���� �� �� � ������� �������� �� ������� �� �������� LMR

          // ����� ���������� ������ ��� �� ��� ������� ������ ��� 
		  // ������ ������ ������ ��������� ��� ������� ������ ���� 
		  // ������ ������ � ������ ��� ��������� ����
          // ��� ��� ����������� ��� ������� lmr_reduction
          // �� ���������� ������ ���������(�����) ���� ���� ��� �� ��� 
		  // �� ���� � ���������� �������
          if((i > lmr_reduction) 
			 && (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
             && ((depth_max_new - depth) >= LMR_DEPTH )
			 ){
               // ���� � ��������� ������� � � ������� �����
               value = Black(p_bitboard,alpha,(alpha + 1),(depth_max_new - LMR_REDUCTION_D),
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

         }else{
               // ��� ������ ���������� ����� ��� �� ����������� ������� value > alpha
			   // ��� ������ ����� ��� �� ���������� �� ���(lmr)
               value = alpha + 1; 
         }//if((i > lmr_reduction)

// ������� �������� ����������� ��������
// � ������ ���� if((value > alpha) && (value < beta)) ��� ��������� ������ ������ ���
// ������� �� �������������� � ������ value >= beta � ���������� ������������� ���������
// ����� ���� value >= beta �� ��� ������ ������ ��������� ���� �������� ���������
// ������ � ��� value > alpha ��� ����� ��������.

	         if(value > alpha){
                  //
		          value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
					           new_depth,((list_surplus_moves.move[i]>>6) & 63),
							   PV_NO,flag_check,flag_null_m);

	         }//if (value > alpha)

      }//if(flag_pv_f)

	#endif//#if  LMR
	//------------------------------------------------------



    // ��������������� �������
    moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
 
// ��������������� ���-���� �����
p_bitboard->hash_key = key_undo;

    //--------------------------------------------
    if (value > value_max){

    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    #if ALPHA_BETA_ON
       if (value > alpha){

         //-----------------------------------------------
	     if(value >= beta) {        
	         #if KILLER
	            // ����� �������
			 if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE){
				 killer_o.Save_killer(i,&list_surplus_moves,depth);
			 }//if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
	         #endif//#if KILLER

             #if HASH
               // �������� ������ �� ������ ������ ���
               hash_o.Save_best_hash(i, p_bitboard->hash_key, &list_surplus_moves, 
				                     value, LOWER, depth, depth_max);
             #endif

// TEST
//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 ))
//cout << "��������� �� ��� ������� ����� " << "\n";

		     return value; 
	     }//if(value >= beta) 
         //-----------------------------------------------

// TEST------------
#if HASH_TEST_SCORE
// ������� ��� �� ���� ���� ������� ������ ���� ���������� �� ��� ������ :)
// ���� �� ������ ���������� �� ��� �� ����� ��� �� ������ ������� �� ����� �� ����
// ��������� �� �����
// ���� � ��� ��� �� ����� ������ �� ���-������� � ������� ��� ��� ������ ���� �.�. �������
// ���������. ������ �� ������������ �� � ���� ������ �������������� �� �� ����� �����
// ���� �� ������ ��� ��� ��������� ������
		 if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 )){
			 cout << "������ ��������� �� ��� ������� ����� " << "\n";
             cout << "depth= " << depth << "\n";
             cout << "depth_max_rem= " << depth_max_rem << "\n";
             cout << "hash_depth= " << (__int32)hash_depth << "\n";
             cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
             cout << "hash_score= " << hash_score  << "\n";
             cout << "value= " << value << "\n";
             cout << "alpha= " << alpha << "\n";
             cout << "beta= " << beta << "\n";
             cout << "  " << "\n";

             exit(1);
		 }//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 )){

#endif//#if HASH_TEST_SCORE
//-----------------


         alpha = value;
         flag_save_best = 1;
         i_hash_moves_1 = i;// ��� ������� �������� ����� (1- �������� �����) ����� ��� ����������� 

         // ����� �� �������������� ������ ������� ��������������� ������ ������
        if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
 
      }//if (value > alpha)

    #else // #if ALPHA_BETA_ON
        // ����� �� �������������� ������ ������� ��������������� ������ ������
        if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
        flag_save_best = 1;
        hash_moves_1 = i;// ��� ������������ ������� ��������
    #endif// #if ALPHA_BETA_ON
    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

       value_max = value;
       i_hash_moves_3 = i;// ��� ������ ��������� �������� (3-������ ��������� ��������)����� ��� �����������

   }//if (value > value_max)

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

 }//if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO)

}//for (int i = 0; i<list_surplus_moves.end_list; i++)

//--------------------------------------------
// ��������� ������ ����� �� ���� ��������� �����
// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (flag_is_legal_moves == 0){
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
       value = 0;
       if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
           value = -(VALUE_MATE - DELTA_MATE * depth);//
       }//if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

       // � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
     if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value,depth);
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    return value;
}//if (flag_is_legal_moves == 0){
//--------------------------------------------

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      if(flag_save_best == 1){
          // ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
       if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(flag_save_best == 1){
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

	#if (HASH && SAVE_LINE)
	  if( flag_save_best == 1 ){
	    // �������� ��� ���������� ����� �.�. ���� 1
	    hash_o.Save_best_hash(i_hash_moves_1, p_bitboard->hash_key, &list_surplus_moves, 
			                  alpha, EXACT, depth, depth_max);
	  }else{
	    // �������� ��� ������ ��������� �������� �.�. ���� 3
	    hash_o.Save_best_hash(i_hash_moves_3, p_bitboard->hash_key, &list_surplus_moves, 
			                  value_max, UPPER, depth, depth_max);
	  }//if(flag_save_best == 1){
	#endif//#if HASH

// ��� ���� ������� ��� ���� ������� ��� ���������� ���� � ������
// � ���� ���� � � ��������� � ����� ������ �� �� � ��� �������� ����
//��� ����� ���� ��� � ������ ����� ���� �������
// ��� ��� ��� ������ ������ ������ �����
//
// TEST------------
#if HASH_TEST_SCORE

// ������� ��� �� ���� ���� ������� ������ ���� ���������� �� ��� ������ :)
/*
������ �� ���-������� ������ ������ �����
���� � �������� ������ ��� ����� ����� ������ ����� �� �� �� ���� �����
*/
		 if ( hash_score != -1000000000 ){
         //if ( 0 ){

            hash_moves_test13 = hash_moves_3;
            if(flag_save_best == 1) hash_moves_test13 = hash_moves_1;

			//if ( hash_moves_test13 != 0 ) {
            //if ( hash_score != value_max ) {
               if ( value_max > alpha ) {
				cout << "������������ � ��������� ���� ������ �������� ������ ����� " << "\n";
               cout << "p_bitboard->hash_key= " << p_bitboard->hash_key << "\n";
               cout << "depth= " << depth << "\n";
               cout << "depth_max_rem= " << depth_max_rem << "\n";
               cout << "hash_depth= " << (__int32)hash_depth << "\n";
               cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
               cout << "test_flag_hash_moves= " << test_flag_hash_moves  << "\n";
               cout << "hash_score= " << hash_score  << "\n";
               cout << "value_max= " << value_max << "\n";
               cout << "alpha= " << alpha << "\n";
               cout << "beta= " << beta << "\n";

  cout << "��� �� ��� ������� " << "\n";
               cout << "((list_surplus_moves.move[0]>>6)& 63)= " 
				    << ((list_surplus_moves.move[0]>>6)& 63) << "\n";
               cout << "������� ������ " << util_o.int_to_char_board(((list_surplus_moves.move[0]>>24)& 7),1)<< "\n";
               
               cout <<util_o.x_to_notation(((list_surplus_moves.move[0]>>12)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[0]>>12)& 63)/8);
               cout <<util_o.x_to_notation(((list_surplus_moves.move[0]>>18)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[0]>>18)& 63)/8) << "\n";

cout << "��� ��������� � ������ " << "\n";
cout << "hash_moves_test12= " << hash_moves_test13  << "\n";
               cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				    << ((list_surplus_moves.move[hash_moves_test13]>>6)& 63) << "\n";
               cout << "������� ������ " << util_o.int_to_char_board(((list_surplus_moves.move[hash_moves_test13]>>24)& 7),1)<< "\n";
               
               cout <<util_o.x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>12)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>12)& 63)/8);
               cout <<util_o.x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>18)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>18)& 63)/8) << "\n";
               cout << "  " << "\n";     
  exit(1);
			}//if ( hash_moves_test13 != 0 ) {

		 }//if ( hash_score != -1000000000 ){

#endif //#if HASH_TEST_SCORE
//-----------------

return value_max;

}//int FULLSEARCH::White
//	*******************************************************************

//	===================================================================
// ���� ������� � ����. ������� �� ������
__int32 FULLSEARCH::Black(struct T_Bitboard * const p_bitboard,// ������������� �����
					  const __int32 alpha,//
		              __int32 beta,//
		              const unsigned __int8 depth_max,// ������������ �������
		              const unsigned __int8 depth,// �������
		              const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		              const bool flag_pv_f,// �������� �� ��� ������� (pv)
		              const bool old_flag_check,//���� ����
		              const bool flag_null_m// ���� �������� ����
		              ){

__int32 value;              // ������� ������ �������
__int32 value_min = 9999999;   // ������ ������� ������ � ���� 
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
bool flag_is_legal_moves = 0;           // ���� ������������� ��������� �����
bool flag_save_best = 0;           // ����� �� ������ �������
const unsigned __int8 new_depth = depth + 1; // ��������� �������

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
    unsigned __int8 depth_max_new = 0;//����� ������� ��������
    bool flag_check = 0;//���� ����
	struct T_PV pv_best_point;// ������ ��� ������� ���� �������
	bool taking_passage_yes_mem = 0;
	   unsigned __int8 taking_passage_y_mem = 0;
	   unsigned __int8 taking_passage_x_mem = 0;
	unsigned __int8 flag_insert_hash_move = 0;        // ��������� �� ��� �� ���-������� �� ������ �������
    const unsigned __int64 key_undo = p_bitboard->hash_key;   // ���������� ���-���� �������� ����
    __int32 hash_score = -1000000000;// ���-������ �������
    unsigned __int8 i_hash_moves_1;// ��� ������� �������� ���� (1- �������� ����) ����� ��� ����������� 
    unsigned __int8 i_hash_moves_3;// ��� ������ ��������� ������� (3-������ ��������� �������)����� ��� �����������
    unsigned __int8 test_flag_hash_moves;// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
    unsigned __int8 hash_depth;// ������������ �������
    unsigned __int8 hash_depth_max;// ������������ �������
//    unsigned __int64 test_key_control;// ���������� ���-���� �������� ����
//    int dynamic_null_move_reduction;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss



 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	// ��� ������� ��� ���������� ������
	if (flag_stop_search == 1) return 0;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

//--------------------------------------------
// �� ���������� ������� ���������� ����������� ������ �������
if (depth >= depth_max){
  	      //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	      #if SAVE_LINE
             value = Max_depth_black(p_bitboard,alpha,beta,flag_pv_f,depth);
	      #endif//#if SAVE_LINE 
	      //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
          return value ;
}//if (depth >= depth_max){
//-----------------------------------------------

//-----------------------------------------------
       #if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
// ���������� ���������� �������
	if ( hash_o.Analysis_hash_string_moves(depth) ){
        // � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
        if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(0 // ������ �����
			                                             ,depth);
		return 0;
	}//if ( hash_o.Analysis_hash_string_moves(depth) )
      #endif//#if ALPHA_BETA_ON
//-----------------------------------------------

	//--------------------------------------------
	#if(FUTILITY_PRUNING && SAVE_LINE)
	//
	if((depth_max - depth) == FUTILITY_DEPTH){

		if((description_move != MOVE_IS_CAPTURE) 
			 && (flag_check == CHECK_NO) 
			 && (flag_pv_f == PV_NO)){

			if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) + FUTILITY_MARGIN) 
				<= alpha ){ 
				return alpha;
			}//if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) + FUTILITY_MARGIN) <= alpha ){ 

		}//if((flag_null_m == NULL_NO) && (description_move != MOVE_IS_CAPTURE) 
		 //	    && (flag_check == CHECK_NO) && (flag_pv_f == PV_NO)){

	}//if((depth_max-depth) == FUTILITY_DEPTH){
	#endif//#if FUTILITY_PRUNING
	//--------------------------------------------


	//--------------------------------------------
#if (HASH_SCORE)
	#if (HASH && SAVE_LINE)
// ������� �� ��� ������ �� ���������� ��� �������� ��������
// ������� ������� ��� ������������� ������ � ����� �������� � ��� ������� �������
// ������ ���������� ������� ������� ������������.
      if(!flag_pv_f){ 
	     // ������� �� ����������� �� ��� ����� ������� ���� ����������� �� 
		 // �������� ��������� ����������� ������
	     hash_score = hash_o.Look_hash_score_black(
			                          p_bitboard->hash_key,depth,depth_max,
									  alpha,beta,test_flag_hash_moves,hash_depth,
									  hash_depth_max);

		  if (hash_score != -1000000000 ) {
//return_hash_score = return_hash_score + 1 ;
		   #if (!HASH_TEST_SCORE)
			  return hash_score;
           #endif//#if (!HASH_TEST_SCORE)
		  }//if (hash_score != -1000000000 ) {
	  }//if(!flag_pv_f){ 
     #endif//#if HASH
 #endif//#if (HASH_SCORE)
	//--------------------------------------------


	//--------------------------------------------
	//if(!flag_pv_f){
 //     // ���� �������� �� ������ :)
 //     // ���� � ��� ��� ������� � ������� 8 �������� ���������� ���� ������ �� ���������
 //    if ( depth > 8 )
 //        if((p_bitboard->eval + 700) <= alpha ) return alpha;

	//}//if(!flag_pv_f){
	//--------------------------------------------




//--------------------------------------------
	#if (NULL_MOVE && SAVE_LINE)
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
    if ((flag_stop_null_move == NULL_YES) // ���� �������� ��� �� ������
	 && (flag_null_m == NULL_NO) // ���� �� �� � ����� �������� ����
	 && (depth_max > NULL_REDUCTION ) // ����� ������ � ����� � � ��� ������� ������ ������������
	 && ((depth_max - depth) >= NULL_DEPTH ) // ���� �� ������������ ������� ���������� �����������
	 && (flag_pv_f == PV_NO) // ���� �� �� � ������� ��������
	 && (description_move != MOVE_IS_CAPTURE) // ���� ��� �� ������
	 && (old_flag_check == CHECK_NO) // ���� ���� ���
//	 && (p_bitboard->eval <= alpha) //
		){ // ����� �������� ������� ���

        taking_passage_yes_mem = p_bitboard->taking_passage_yes;
        taking_passage_x_mem = p_bitboard->taking_passage_x;
        taking_passage_y_mem = p_bitboard->taking_passage_y;

// ������ ���-���� ��� ������� ��� -------------------------
// ��������� ������ �� �������
if( p_bitboard->taking_passage_yes ){
	p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//
// ��������� ���� ����
// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
// ��������� ������� ���������� ������ ������ ����
 p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);

//---------------------------------------------


        p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

        p_bitboard->colour_move = !p_bitboard->colour_move;
	
        const __int64 nodes_null_rem = nodes;

// TEST ���������� ��������� ���-�����
// �������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
//hash_o.Start_position_random_key(p_bitboard);


         value = White(p_bitboard,(beta - 1),beta,(depth_max - NULL_REDUCTION),
			          new_depth,description_move, 
					  PV_NO,old_flag_check,NULL_YES);

//--------------------------------------------------------
//dynamic_null_move_reduction = 
// ((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
// ? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
//
//         value = White(p_bitboard,(beta - 1),beta,(depth_max - dynamic_null_move_reduction),
//			          new_depth,description_move, 
//					  PV_NO,flag_check,NULL_YES);
//--------------------------------------------------------

        nodes = nodes_null_rem;

        p_bitboard->taking_passage_yes = taking_passage_yes_mem;
        p_bitboard->taking_passage_x = taking_passage_x_mem ;
        p_bitboard->taking_passage_y = taking_passage_y_mem ;

        p_bitboard->colour_move = !p_bitboard->colour_move;

// ��������������� ���-���� �����
p_bitboard->hash_key = key_undo;


        //��� ��� ����� ��� ������� ��� �������� ����� ������������ ������ �.�. �������� �����
		//������ ���������� ���
        //������ ����� ��� ����� � ���� �� �� ������ �������� ����� �.�. ���� ������ ������
        //��� ����� ����� ������ ����� ������ �.�. ����� ��� �� ���������
        if(value <= alpha){ 
              //cout << "������� "<< "\n";
              //cout << "depth_max= "<< depth_max <<"\n";
              //cout << "depth= "<< depth <<"\n";
              //cout << "value= "<< value <<"\n";
              //cout << "alpha= "<< alpha <<"\n";
              //cout << "       "<< "\n";

//         return alpha;

//------------------------------
if( (depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1) )  return alpha;

        // ������� ������ ��������
	    value = Black(p_bitboard,alpha,(alpha + 1),(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
			         depth,description_move, 
					 PV_NO,old_flag_check,NULL_YES);

     if(value <= alpha) return alpha;
//------------------------------

	    }//if(value <= alpha)

	}//   if((flag_null_m == NULL_NO) && ((depth_max - depth) >= NULL_DEPTH ) && 

	#endif//#if NULL_MOVE


// ����������� ������ ���������� �����
// ��������� ���� ������� � ������� ������ ��������
bitboard_generator_o.Generator_captures_black(p_bitboard,&list_surplus_moves);
bitboard_generator_o.Generator_move_black(p_bitboard,&list_surplus_moves);
Sorting_moves(&list_surplus_moves);

	//-----------------------------------------------
	#if (KILLER && SAVE_LINE)
	// ���� ��� ������ ��� ������ �������������
    // �� ������ ��������� �� ������ ������� ����� �� ������
    if((list_surplus_moves.end_captures == CAPTURE_NO) || 
	   (list_surplus_moves.sorting_score[0] < 0)){

		 killer_o.Insert_killer(&list_surplus_moves,depth,0);

	}else{

		 killer_o.Insert_killer(&list_surplus_moves,depth,1);

	}//   if((list_surplus_moves.end_captures == CAPTURE_NO) || 

	#endif//#if KILLER

	#if (HASH && SAVE_LINE)
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

	// ��� ������� ����� ����� ��������� ������� �� ������
    #if HASH_TEST
	// ���� 0 ������ ��� ������� �� ��� ����� �� �����
	// ���� 1 ������ ��� ������� ����� �� � ������� ������ ��� ���� ����������� � ���
	// ���� 2 ������ ��� �� ����� ������� ����� � ����� ��� ���������� � ������ �����

       if (flag_insert_hash_move == 1) {
           cout << "�������� ���� ������" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 0) {
    #endif//#if HASH_TEST


	#endif//#if HASH
	//-----------------------------------------------

#if (LMR && SAVE_LINE)
   // ��� �� ���-�������, ������ ������, ��� �������, ��� ������ �� ������������
     lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
   //lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR

//int r;

// ����� �� ������ ���������� �����
// ����� � ����� �����; ������ ���� ���������� � ����������� ������
for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

//--------------------------------------------
// � ����� ��������� ������� ��� �� ������� �� � �� ��� ���������
// � ��� ������ ����� ���� ������ ������ �������� �� �����
	//if(!flag_pv_f){
 //      // ���� �������� ����� ������� ��� ����� ���(lmr) :)
 //     // ���� � ��� ��� ������� � ������� 6 �������� ���������� ��������� ����
	//  // �� ������ ���� �������� � ����� ������
	// r = 5 + list_surplus_moves.end_list - depth;
	// if ( r < 0) r = 0;
 //    if ( i >= 4 + r ) continue;
	//}//if(!flag_pv_f){
//--------------------------------------------

  // ��������� ��� �� ������ ������� ������� ���������� �������
  moves_o.Onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);

  // ���� ������ �� ����� ��
  if (bitboard_generator_o.King_black_check(p_bitboard)!= CHECK_NO){

// �� �������� ���� ��� ��� ������������
     moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

  }else{
 // ������ ���� ��� ���� ��������� ���� � ����������� �������
    flag_is_legal_moves = 1;
    nodes = nodes + 1;

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	//-----------------------------------------------
	  // �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	 if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	  hash_o.Modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);


//TEST-----------
#if HASH_TEST_KEY

  unsigned __int64 test_key_control = p_bitboard->hash_key;

 //��� ���� ��������� ��������� �� �����
 //���������� ���-�������
 //�������������� ���� ��������� �������(�� ����� ��� � ��������� �����)
  hash_o.Start_position_random_key(p_bitboard);

  if(test_key_control != p_bitboard->hash_key){
       cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
       cout << "collision black depth = " << depth << "\n";
       exit(1);
  }//if(key_control != p_bitboard->hash_key){

#endif//#if HASH_TEST_KEY 
//----------------------------

	  // ���������� ���� ��� ������������ ������� �� ������ �������
      hash_o.Save_hash_three(p_bitboard->hash_key,new_depth); 
	//-----------------------------------------------
    depth_max_new = depth_max;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

	//-----------------------------------------------
	#if (EXTENSION_CHEK && SAVE_LINE)

	   // ���� ������ ��� ����� �� ����������� ������������ ������� �������� 
	      Extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);

	#endif//#if EXTENSION_CHEK
	//-----------------------------------------------

	//-----------------------------------------------
	#if (EXTENSION_CAPTURES && SAVE_LINE)
      if(flag_pv_f == PV_YES){//

//            if (description_move == 2){

             // ���� ��� ��� ������
             if (((list_surplus_moves.move[i]>>6) & 63) == 2){

                 // ���� ������ ������ ����� �������
                 if ( ((list_surplus_moves.move[i]>>24) & 7) // �����
                        <= (list_surplus_moves.move[i] & 63) ){ // �����

                           depth_max_new = depth_max + 1;
                          //Extension_for_captures(((list_surplus_moves.move[i]>>6) & 63)
		                  //                  ,description_move,depth_max,depth_max_new);

				 }//if ( ((list_surplus_moves.move[i]>>24) & 7)

			 }//if (((list_surplus_moves.move[i]>>6) & 63) == 2){

//		 }//if (description_move == 2){

	  }//if(flag_pv_f == PV_YES)
    #endif//#if EXTENSION_CAPTURES
	//-----------------------------------------------

// ������� ������� ��������� �� ������������ ������     
  //   if(flag_pv_f == PV_YES){//

  //      if ((flag_check) // ���� ��� ���
		//	&&(list_surplus_moves.end_list < 10)  // � ����� ������ 10
		//	&&(depth_max_new < 51) ){			
  //     //   cnt_single = 0; 
		//		depth_max_new = depth_max + 1;
		//}//if ((flag_check) // ���� ��� ���

	 // }//if(flag_pv_f == PV_YES)

    #if !SAVE_LINE
   // ����������� ����� �������
      value = White(p_bitboard,alpha,beta,depth_max,new_depth,
		            ((list_surplus_moves.move[i]>>6) & 63),PV_YES,flag_check,
					flag_null_m);
    #endif//#if !SAVE_LINE


    #if (!LMR && SAVE_LINE)
      // ����������� ����� �������
      //depth_max_z
    #if ALPHA_BETA_ON
	  if (i == 0) {
    #else
      if (1) {
    #endif//#if ALPHA_BETA_ON

              value = White(p_bitboard,alpha,beta,depth_max_new,new_depth,
			               ((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
				        	 flag_check,flag_null_m);

       }else{

               value = White(p_bitboard,(beta - 1),beta,depth_max_new,
// TEST
//value = White(p_bitboard,alpha,beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

               if((value > alpha) && (value < beta)){

		           value = White(p_bitboard,alpha,beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 flag_pv_f,flag_check,flag_null_m);

	           }//if((value > alpha)&&(value < beta))
        }//if (i == 0)
    #endif//#if !NEGA_SCOUT

	//------------------------------------------------------
	#if  (LMR && SAVE_LINE)

	  if(flag_pv_f){
          // ���� �� � ������� �������� �� ������� �� �������� � ������� �����

	      if (i == 0) {
              //
              value = White(p_bitboard,alpha,beta,depth_max_new,
				            new_depth,((list_surplus_moves.move[i]>>6) & 63),
							PV_YES,flag_check,flag_null_m);
          }else{

              value = White(p_bitboard,(beta - 1),beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

              if(value < beta){

		           value = White(p_bitboard,alpha,beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 PV_YES,flag_check,flag_null_m);

	          }//if(value < beta)

        }//if (i == 0)

      }else{
          // ���� �� �� � ������� �������� �� ������� �� �������� LMR

        if((i > lmr_reduction)
			&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
			&& ((depth_max_new - depth) >= LMR_DEPTH ) 
			){

               value = White(p_bitboard,(beta - 1),beta,(depth_max_new - LMR_REDUCTION_D),
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

        }else{

               value = beta - 1;

        }// if((i > lmr_reduction)

             if(value < beta){

		           value = White(p_bitboard,(beta - 1),beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 PV_NO,flag_check,flag_null_m);

	          }//if(value < beta)

	  }//if(flag_pv_f){

	#endif//#if  LMR
	//------------------------------------------------------

// ��������������� �������
   moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE

// ��������������� ���-���� �����
p_bitboard->hash_key = key_undo;

   //-----------------------------------------------

   if (value < value_min){

     //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      #if ALPHA_BETA_ON
        if (value < beta){

           //-----------------------------------------------
		   if(value <= alpha) {
	          #if KILLER
			 if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE){
				 killer_o.Save_killer(i,&list_surplus_moves,depth);
			 }//if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE){
	          #endif//#if KILLER

              #if HASH
	             // �������� ������ �� ������ ������ ���
	             hash_o.Save_best_hash(i, p_bitboard->hash_key, &list_surplus_moves, 
					                   value, LOWER, depth, depth_max);	
	          #endif//#if HASH

			  return value; 
		   }//if(value <= alpha) {
         //-----------------------------------------------

// TEST------------
#if HASH_TEST_SCORE
// ������� ��� �� ���� ���� ������� ������ ���� ���������� �� ��� ������ :)
// ���� �� ������ ���������� �� ��� �� ����� ��� �� ������ ������� �� ����� �� ����
// ��������� �� ������
// ���� � ��� ��� �� ����� ������ �� ���-������� � ������� ��� ��� ������ ����� �.�. �������
// ���������. ������ �� ������������ �� � ���� ������ �������������� �� �� ����� �����
// ���� �� ������ ��� ��� ��������� ������
		 if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 )){
			 cout << "������ ��������� �� ��� ������� ������" << "\n";
             cout << "depth= " << depth << "\n";
             cout << "depth_max_rem= " << depth_max_rem << "\n";
             cout << "hash_depth= " << (__int32)hash_depth << "\n";
             cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
             cout << "hash_score= " << hash_score  << "\n";
             cout << "value= " << value << "\n";
             cout << "alpha= " << alpha << "\n";
             cout << "beta= " << beta << "\n";
             cout << "  " << "\n";

             exit(1);
		 }//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 )){

#endif//#if HASH_TEST_SCORE
//-------------------

           beta = value;
           flag_save_best = 1;
           i_hash_moves_1 = i;// ��� ������� �������� ���� (1- �������� ����) ����� ��� ����������� 

           // ����� �� �������������� ������ ������� ��������������� ������ ������
           if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);

        }//if (value < beta){

      #else // #if ALPHA_BETA_ON

        // ����� �� �������������� ������ ������� ��������������� ������ ������
        if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
        flag_save_best = 1;
        hash_moves_1 = i;// ��� ������������ ������� ��������
      #endif//#if ALPHA_BETA_ON
    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

       value_min = value;
       i_hash_moves_3 = i;// ��� ������ ��������� ������� (3-������ ��������� �������)����� ��� �����������

   }//if (value < value_min){

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

 }//  if (bitboard_generator_o.King_black_check(p_bitboard)!= CHECK_NO){

}//for (int i=0;i<list_surplus_moves.end_list;i++){

//-----------------------------------------------
// ��������� ������ ����� �� ���� ��������� �����
// ���� �� ������ ���� ������� �� ���� �� ���������� ����������� ������ �������
// ��� ������ ���� ��� ����
if (flag_is_legal_moves == 0){
 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
     value = 0;
     if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
         value = (VALUE_MATE - DELTA_MATE * depth);//
     }//if(King_white_check(p_bitboard) != 0){

      // � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
      if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value,depth);
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
  return value;
}//	if (no_moves == 0){
//-----------------------------------------------

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	     if(flag_save_best == 1){
          // ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
          if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(save_best == 1){
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    #if (HASH && SAVE_LINE)
	  if( flag_save_best == 1 ) {
	    // �������� ��� ���������� ���� �.�. ���� 1
	    hash_o.Save_best_hash(i_hash_moves_1, p_bitboard->hash_key, &list_surplus_moves, 
			                  beta, EXACT, depth, depth_max);
	  }else{
	    // �������� ��� ������ ��������� ������� �.�. ���� 3
	    hash_o.Save_best_hash(i_hash_moves_3, p_bitboard->hash_key, &list_surplus_moves, 
			                  value_min, UPPER, depth, depth_max);
	  }//if(flag_save_best == 1){
	#endif//#if HASH

return value_min;
}//int FULLSEARCH::Black
//	*******************************************************************

//	===================================================================
// ���������� ������� �� ������������ �������. �� �����
inline __int32 FULLSEARCH::Max_depth_white(struct T_Bitboard * const p_bitboard,// ������������� �����
									   const __int32 alpha,//
						               const __int32 beta,//
                                       const bool flag_pv_f,// �������� �� ��� ������� (pv)
						               const unsigned __int8 depth// �������
						               ){

__int32 score = 0;
__int32 score_f = 0;
double deep_time_current;

//-----------------------------------------------
// ��������� ������ ����������� ���������� ����� 
if (audit > 50000){
//if (0){
  //���� ��������� ������� �� ��� ��� quit ��� stop
  if (Bioskey()){
// if (0){
//     char string_in2[65536];
     cin.getline(string_in2,65536, '\n');
     if (strcmp(string_in2, "quit") == 0) {
        hash_o.Hash_size_free();
	    exit(1);
     }//if (strcmp(string_in2, "quit")== 0) {
     if ((strcmp(string_in2, "stop") == 0)){
     // ������ ���
        flag_stop_search = 1;
     }//if ((strcmp(string_in2, "stop")== 0)){
   }//if (Bioskey()){

// ���������� ����� � �������. ���������� ����� �� �����
  timer.Timer_stop();
  deep_time_current = timer.Timer_delta();
// ������� �� �������
   if((deep_time_terminate > 0) && (deep_time_terminate < deep_time_current)) flag_stop_search = 1;
//------------------------

   audit = 0;

}//if (audit>50000){

audit = audit + 1;
//-----------------------------------------------

//------------------------------------------------------
   // ����������
   //FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
   // ���������� ������������� �������
   // �� ��� ����������� �� ���� ��� ��� ��� ������� �������� �� ��
   #if FORCED // �������� ����� �������� ������ ������
  	   score = quiescence_search_o.Forced_variant_white(p_bitboard,alpha,beta,depth,flag_pv_f,(depth + 5));
       return score ;
   #endif//#if FORCED

//----------------------------------------------
// ������������ � �������������� � ���������� �����
   //#if FORCED // �������� ����� �������� ������ ������
   //    score = eval_o.Eval_State(p_bitboard);
  	//   score_f = quiescence_search_o.Forced_variant_white(p_bitboard,(alpha - score),(beta + score),depth,(depth + 5));
   //    score = score + score_f;
   //    pv_save_o.pv_best.score_move = score;
   //    return score ;
   //#endif//#if FORCED
//----------------------------------------------


// �������� ������ ��� ����������(�������� ������ ������)
   #if !FORCED 
      score = eval_o.Eval_State(p_bitboard) + p_bitboard->eval;
      //score = p_bitboard->eval;

////--------------------------------------------------------------------------
//// TEST
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "WHITE  Eval_forced(p_bitboard) != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) 
////--------------------------------------------------------------------------

//    � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
//    ���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
      if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(score,depth);

//    TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
//    ������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
//    �����  ������ ���� � ����
//    Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
//    TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
      return score ;
 #endif//#if !FORCED 

}//inline int FULLSEARCH::Max_depth_white
//	*******************************************************************

//	===================================================================
// ���������� ������� �� ������������ �������. �� ������
inline __int32 FULLSEARCH::Max_depth_black(struct T_Bitboard * const p_bitboard,// ������������� �����
									   const __int32 alpha,//
						               const __int32 beta,//
                                       const bool flag_pv_f,// �������� �� ��� ������� (pv)
						               const unsigned __int8 depth// �������
						               ){

__int32 score = 0;
__int32 score_f = 0;
double deep_time_current;

//-----------------------------------------------
// ��������� ������ ����������� ���������� ����� 
if (audit > 50000){
//if (0){
//���� ��������� ������� �� ��� ��� quit ��� stop
  if (Bioskey()){
// if (0){
//    char string_in2[65536];
    cin.getline(string_in2,65536, '\n');
    if (strcmp(string_in2, "quit") == 0) {
       hash_o.Hash_size_free();
	   exit(1);
    }//if (strcmp(string_in2, "quit") == 0) 
    if ((strcmp(string_in2, "stop") == 0)){
    // ������ ���
       flag_stop_search = 1;
    }//if ((strcmp(string_in2, "stop")== 0))
  }//if (Bioskey())

// ���������� ����� � �������. ���������� ����� �� �����
  timer.Timer_stop();
  deep_time_current = timer.Timer_delta();
// ������� �� �������
   if((deep_time_terminate  > 0) && (deep_time_terminate < deep_time_current)) flag_stop_search = 1;
//------------------------

  audit = 0;

}//if (audit>50000)

audit = audit + 1;
//-----------------------------------------------

//------------------------------------------------------
//  ����������
//  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
//  ���������� ������������� �������
//  �� ��� ����������� �� ���� ��� ��� ��� ������� �������� �� ��
    #if FORCED // �������� ����� ����������(�������� ������ ������)
       score = quiescence_search_o.Forced_variant_black(p_bitboard,alpha,beta,depth,flag_pv_f,(depth + 5));
       return score ;
    #endif//#if FORCED 

//----------------------------------------------
// ������������ � �������������� � ����������  ����� 
   //#if FORCED // �������� ����� �������� ������ ������
   //    score = eval_o.Eval_State(p_bitboard);
  	//   score_f = quiescence_search_o.Forced_variant_black(p_bitboard,(alpha - score),(beta + score),depth,(depth + 5));
   //    score = score + score_f;
   //    pv_save_o.pv_best.score_move = score;
   //    return score ;
   //#endif//#if FORCED
//----------------------------------------------


// �������� ������ ��� ����������(�������� ������ ������)
    #if !FORCED
       score = eval_o.Eval_State(p_bitboard) + p_bitboard->eval;
       //score = p_bitboard->eval;

////--------------------------------------------------------------------------
//// TEST
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "BLACK  Eval_forced(p_bitboard) != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) 
////--------------------------------------------------------------------------

//     � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
//     ���� �������� ��� � pv_current �� ������ �� ������� �� �������� �� �� ����� ����� � pv_best
       if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(score,depth);

//     TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
//     ������ ����� ����� ��������� ��������� �� �������� ������� �� ���������� �������
//     �����  ������ ���� � ����
//     Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
//     TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
       return score ;
    #endif//#if !FORCED 

}//inline int FULLSEARCH::Max_depth_black
//	*******************************************************************

//	===================================================================
// ��������� ��������� �� ����� -
inline void FULLSEARCH::Extension_for_chek_white(const struct T_Bitboard * const p_bitboard,// ������������� �����
	                                             const unsigned __int8 depth_max,// ������������ �������
									             unsigned __int8 & depth_max_new,// ����� ������������ �������
									             bool & flag_check// ���� ����
									             ){

// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
      flag_check = CHECK_YES;
      depth_max_new = depth_max + 1;
}else{
      flag_check = CHECK_NO;
}//if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){

}//inline void FULLSEARCH::Extension_for_chek_white
//	*******************************************************************

//	===================================================================
// ��������� ��������� �� ����� -
inline void FULLSEARCH::Extension_for_chek_black(const struct T_Bitboard * const p_bitboard,// ������������� �����
	                                             const unsigned __int8 depth_max,// ������������ �������
									             unsigned __int8 & depth_max_new,// ����� ������������ �������
									             bool & flag_check// ���� ����
									             ){

// ���� ���� ��� �� ����������� ������������ ������� �������� ��� ���� �����
if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
      flag_check = CHECK_YES;
      depth_max_new = depth_max + 1;
}else{
      flag_check = CHECK_NO;
}//if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

}//inline void FULLSEARCH::Extension_for_chek_black
//	*******************************************************************

//	===================================================================
// ��������� ��������� �� ������� 
inline void FULLSEARCH::Extension_for_captures(const __int32 description_move,// ��� ����
								               const __int32 description_move_up,// ��� ���� �� ���������� �������
								               const unsigned __int8 depth_max,// ������������ �������
								               unsigned __int8 & depth_max_new// ����� ������������ �������
								               ){

// ���� ���� ������ �� ����������� ������������ ������� �������� ��� ���� �����
// �������� ��� ������� ������� �� ������ ������ � ������������
// (����� ������ ������ ������ �������)
     if((description_move == 2) && (description_move_up == 2)){
	         depth_max_new = depth_max + 1;
     }//if((description_move == 2) && (description_move_up == 2)){
}//inline void FULLSEARCH::Extension_for_captures
//	*******************************************************************

//	===================================================================
// ������ ����������
inline void FULLSEARCH::Sorting_moves(struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						              ){
 // 
__int32 move_s;// ���������� ��� ����������� ����
__int32 score_s;// ���������� ��� ����������� ������ ����
unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//���������� ������
unsigned __int8 end_list = p_list_surplus_moves->end_list;//���������� �����


/*
������ � ��� ���������� � 0 � ����� ���
0 ������
1 ������
2 ������
3 ���          p_list_surplus_moves->end_captures
�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
4 ���
5 ���
6 ���
7              p_list_surplus_moves->end_list
���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����

*/

// 1 ��������� ������----------------------------------
/*
���� � ��� ���� ������ ����� �� �� ��������� �� ������ ����������� ��� � ���������� ������
���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
����� ������� ����� �� ������� �� ����� ��� ������ � ��� ������������� �� ���������.
*/
   for(unsigned __int8 k = 0; k < end_captures; k++){	
       for(unsigned __int8 i = k + 1; i < end_captures; i++){
           if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
//            ��������� ������   
	          move_s = p_list_surplus_moves->move[k];
              score_s = p_list_surplus_moves->sorting_score[k];

//            �������� �� i � k
	          p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
              p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

//            ��������������� ������
	          p_list_surplus_moves->move[i] = move_s;
              p_list_surplus_moves->sorting_score[i] = score_s;
	       }//if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
       }//for(int i = k + 1; i < captures; i++){
   }//for(int k = 0; k < captures; k++){	

}//inline void FULLSEARCH::Sorting_moves
//	*******************************************************************

//	===================================================================
// ������ ���������� ��������� ������
inline void FULLSEARCH::Sorting_white_moves_root(struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						              ){
 // 
__int32 move_s;// ���������� ��� ����������� ����
__int32 score_s;// ���������� ��� ����������� ������ ����
unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//���������� ������
unsigned __int8 end_list = p_list_surplus_moves->end_list;//���������� �����


/*
������ � ��� ���������� � 0 � ����� ���
0 ������
1 ������
2 ������
3 ���          p_list_surplus_moves->end_captures
�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
4 ���
5 ���
6 ���
7              p_list_surplus_moves->end_list
���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����

*/

// 1 ��������� �������� ����----------------------------------
/*
�� �� ��������� �� ������ ����������� �� ���������� ��������
���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
����� ������� ����� �� ������� �� ����� ��� ���� � ��� ������������� �� ���������.
*/
   for(unsigned __int8 k = 0; k < end_list; k++){	
       for(unsigned __int8 i = k + 1; i < end_list; i++){
           if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
//            ��������� ������   
	          move_s = p_list_surplus_moves->move[k];
              score_s = p_list_surplus_moves->sorting_score[k];

//            �������� �� i � k
	          p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
              p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

//            ��������������� ������
	          p_list_surplus_moves->move[i] = move_s;
              p_list_surplus_moves->sorting_score[i] = score_s;
	       }//if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
       }//for(int i = k + 1; i < captures; i++){
   }//for(int k = 0; k < captures; k++){	

}//inline void FULLSEARCH::Sorting_moves
//	*******************************************************************

//	===================================================================
// ������ ���������� ��������� ������
inline void FULLSEARCH::Sorting_black_moves_root(struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						              ){
 // 
__int32 move_s;// ���������� ��� ����������� ����
__int32 score_s;// ���������� ��� ����������� ������ ����
unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//���������� ������
unsigned __int8 end_list = p_list_surplus_moves->end_list;//���������� �����


/*
������ � ��� ���������� � 0 � ����� ���
0 ������
1 ������
2 ������
3 ���          p_list_surplus_moves->end_captures
�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
4 ���
5 ���
6 ���
7              p_list_surplus_moves->end_list
���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����

*/

// 1 ��������� �������� ����----------------------------------
/*
�� �� ��������� �� ������ ����������� �� ���������� ��������
���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
����� ������� ����� �� ������� �� ����� ��� ���� � ��� ������������� �� ���������.
*/
   for(unsigned __int8 k = 0; k < end_list; k++){	
       for(unsigned __int8 i = k + 1; i < end_list; i++){
           if(p_list_surplus_moves->sorting_score[k] > p_list_surplus_moves->sorting_score[i]){ 
//            ��������� ������   
	          move_s = p_list_surplus_moves->move[k];
              score_s = p_list_surplus_moves->sorting_score[k];

//            �������� �� i � k
	          p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
              p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

//            ��������������� ������
	          p_list_surplus_moves->move[i] = move_s;
              p_list_surplus_moves->sorting_score[i] = score_s;
	       }//if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
       }//for(int i = k + 1; i < captures; i++){
   }//for(int k = 0; k < captures; k++){	

}//inline void FULLSEARCH::Sorting_moves
//	*******************************************************************

//	===================================================================
// ��������� ��� �����. ������ ��� �� ����� ��� ���� �� ��� ��� �����
void FULLSEARCH::Print_name_from_coord(const struct T_Bitboard * const p_bitboard// ������������� �����
									   ){

for(unsigned __int8 i = 0; i < 8; i++){
   for(unsigned __int8 j = 0; j < 8; j++){
       cout << p_bitboard->white_name_from_coord[(7 - i) * 8 + j];
   }//for(int j = 0; j < 8; j++){
   cout << "\n";
}//for(int i = 0; i < 8; i++){
cout << "\n";
for(unsigned __int8 i = 0; i < 8; i++){
   for(unsigned __int8 j = 0; j < 8; j++){
       cout << p_bitboard->black_name_from_coord[(7 - i) * 8 + j];
   }//for(int j = 0; j < 8; j++){
   cout << "\n";
}//for(int i = 0; i < 8; i++){

  //for(int i= 0; i < 64; i++){
  //   cout << bitboard->white_name_from_coord[i];
  //   if(i%8 == 7) cout << "\n";
  //}//for(int i= 0; i < 64; i++){
}//void FULLSEARCH::Print_name_from_coord
//	*******************************************************************

//	===================================================================
// ��������� ��� �����. ���������� �����
inline void FULLSEARCH::Save_test_bitboard(const struct T_Bitboard * const p_bitboard,// ������������� �����
                                           struct T_Bitboard * p_test_bitboard// ������������� �����
						                  ){

	p_test_bitboard->all_piece = p_bitboard->all_piece;// ��� ������
	p_test_bitboard->all_white_piece = p_bitboard->all_white_piece;// ��� ����� ������
	p_test_bitboard->all_black_piece = p_bitboard->all_black_piece;// ��� ������ ������
	p_test_bitboard->colour_move = p_bitboard->colour_move;
	p_test_bitboard->taking_passage_yes = p_bitboard->taking_passage_yes;
	p_test_bitboard->taking_passage_x = p_bitboard->taking_passage_x;
	p_test_bitboard->taking_passage_y = p_bitboard->taking_passage_y;
	p_test_bitboard->castling_Q = p_bitboard->castling_Q;
	p_test_bitboard->castling_K = p_bitboard->castling_K;
	p_test_bitboard->castling_q = p_bitboard->castling_q;
	p_test_bitboard->castling_k = p_bitboard->castling_k;
	p_test_bitboard->hash_key =  p_bitboard->hash_key;
	p_test_bitboard->eval = p_bitboard->eval;

	for(unsigned __int8 n =0; n < 7; n++ ){
       p_test_bitboard->white_piece[n] = p_bitboard->white_piece[n];
       p_test_bitboard->black_piece[n] = p_bitboard->black_piece[n];
	}//for(int n =0; n < 7; n++ ){

    for(unsigned __int8 k =0; k < 64; k++ ){
        p_test_bitboard->white_name_from_coord[k] = p_bitboard->white_name_from_coord[k];
        p_test_bitboard->black_name_from_coord[k] = p_bitboard->black_name_from_coord[k];
	}//for(int k =0; k < 64; k++ ){

}//inline void FULLSEARCH::Save_test_bitboard
//	*******************************************************************

//	===================================================================
// ��������� ��� �����. ���������� �����
inline void FULLSEARCH::Compare_test_bitboard(const struct T_Bitboard * const p_bitboard,// ������������� �����
                                              struct T_Bitboard * p_test_bitboard// ������������� �����
						                     ){
// ��������� ������������� �� ����� ����� ����������� �������
if(p_test_bitboard->all_piece != p_bitboard->all_piece) cout << "white test_all_piece != bitboard->all_piece" << "\n";// ��� ������
if(p_test_bitboard->all_white_piece != p_bitboard->all_white_piece) cout << "white test_all_white_piece != bitboard->all_white_piece" << "\n";// ��� ����� ������
if(p_test_bitboard->all_black_piece != p_bitboard->all_black_piece) cout << "white test_all_black_piece != bitboard->all_black_piece" << "\n";// ��� ������ ������
if(p_test_bitboard->colour_move != p_bitboard->colour_move) cout << "white test_colour_move != bitboard->colour_move" << "\n";
if(p_test_bitboard->taking_passage_yes != p_bitboard->taking_passage_yes) cout << "white test_taking_passage_yes != bitboard->taking_passage_yes" << "\n";
if(p_test_bitboard->taking_passage_x != p_bitboard->taking_passage_x) cout << "white test_taking_passage_x != bitboard->taking_passage_x" << "\n";
if(p_test_bitboard->taking_passage_y != p_bitboard->taking_passage_y) cout << "white test_taking_passage_y != bitboard->taking_passage_y" << "\n";
if(p_test_bitboard->castling_Q != p_bitboard->castling_Q) cout << "white test_castling_Q != bitboard->castling_Q" << "\n";
if(p_test_bitboard->castling_K != p_bitboard->castling_K) cout << "white test_castling_K != bitboard->castling_K" << "\n";
if(p_test_bitboard->castling_q != p_bitboard->castling_q) cout << "white test_castling_q != bitboard->castling_q" << "\n";
if(p_test_bitboard->castling_k != p_bitboard->castling_k) cout << "white test_castling_k != bitboard->castling_k" << "\n";
if(p_test_bitboard->hash_key !=  p_bitboard->hash_key) cout << "white test_hash_key !=  bitboard->hash_key" << "\n";
if(p_test_bitboard->eval != p_bitboard->eval) cout << "white test_eval != bitboard->eval" << "\n";

	for(unsigned __int8 n2 = 0; n2 < 7; n2++ ){
       if(p_test_bitboard->white_piece[n2] != p_bitboard->white_piece[n2])  cout << "white test_white_piece[n] = bitboard->white_piece[n]" << n2 << "\n";
       if(p_test_bitboard->black_piece[n2] != p_bitboard->black_piece[n2])  cout << "white test_black_piece[n] = bitboard->black_piece[n]" << n2 << "\n";
	}//for(int n =0; n < 7; n++ ){

    for(unsigned __int8 k2 = 0; k2 < 64; k2++ ){
       if(p_test_bitboard->white_name_from_coord[k2] != p_bitboard->white_name_from_coord[k2])  cout << "white test_white_name_from_coord[k] = bitboard->white_name_from_coord[k]" << k2 << "\n";;
       if(p_test_bitboard->black_name_from_coord[k2] != p_bitboard->black_name_from_coord[k2])  cout << "white test_black_name_from_coord[k] = bitboard->black_name_from_coord[k]" << k2 << "\n";;
	}//for(int k2 =0; k2 < 64; k2++ ){

}//inline void FULLSEARCH::Compare_test_bitboard
//	*******************************************************************

//	===================================================================
//
void FULLSEARCH::ini_global_loop(unsigned __int8 depth_max//
								 ){
        audit = 0;// �������� ������ ������� ����������� ������
        lmr_reduction = 0;//
        flag_stop_search = 0;//
}//void FULLSEARCH::ini_global_loop
//	*******************************************************************

//	===================================================================
// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
// �� ��������� �� �������� �� ������
bool FULLSEARCH::get_stop_search(){
	return flag_stop_search;
}//bool FULLSEARCH::get_stop_search
//	*******************************************************************

//	===================================================================
// ���������� ������ �������
struct T_PV * FULLSEARCH::get_pv_best(){
	return pv_save_o.get_pv_best();
}//struct T_PV FULLSEARCH::get_pv_best
//	*******************************************************************

//	===================================================================
// ���������� ���������� ������������� �����
__int64  FULLSEARCH::get_nodes(){
	return nodes;// ���������� �����;
}//__int64  FULLSEARCH::get_nodes
//	*******************************************************************

//	===================================================================
// �������� ���������� ������������� �����
 void FULLSEARCH::set_nodes_in_0(){
	nodes = 0;// ���������� �����;
}//__int64  FULLSEARCH::get_nodes
//	*******************************************************************

//	===================================================================
// ������ �������� ���
unsigned __int8 FULLSEARCH::get_from(){
	return from;
}//int FULLSEARCH::get_from
//	*******************************************************************

//	===================================================================
// ���� �������� ���
unsigned __int8 FULLSEARCH::get_to(){
	return to;
}//int FULLSEARCH::get_to
//	*******************************************************************
