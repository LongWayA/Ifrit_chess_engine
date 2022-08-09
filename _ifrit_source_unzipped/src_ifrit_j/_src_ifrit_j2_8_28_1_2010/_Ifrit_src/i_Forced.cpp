#include <iostream> // TEST
#include <fstream>  // TEST
using namespace std;

#include "i_Forced.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "k_Util.h"
#include "m_Line.h"
#include "d_Bitboard_gen.h"

//-----------------------------------------------------------------------------------
/*+
 * ������������� ������� ������� 
 *
 * ����� ChessTerminator75
 * ��������� ��������� 5.12.2008 19.11.2009
 *
 *  ������������
 *
*/
//---------------------------------------------------------------------------
#define DELTA_PRUNING 1 // ��������-���������(1-0) �����
// ����� ������������ �����������

// ������ ��� �����(����������� ������ ���� ������������ � ������� b_FEN.cpp � d_Bitboard_gen.cpp)
#define N_KING    6 // ������
#define N_QUEEN   5 // �����
#define N_ROOK    4 // �����
#define N_BISHOP  3 // ����
#define N_KNIGHT  2 // ����
#define N_PAWN    1 // �����

// 90000 - ������ 1200 - ����� 600 - ����� 400 - ����  400 - ���� 100 - �����
#define ROOK_MARGIN    700 // ������ ������ ������
#define BISHOP_KNIGHT_MARGIN  500 // ������ ������  ������ � �����
#define PAWN_MARGIN 160 // ������ ������  �����

// global
extern class PVSAVE pv_save_o;// ���������� �������
extern class EVAL  eval_o;// ������ �������
extern class BITBOARDGENERATOR bitboard_generator_o;// ��������� �����
extern class MOVES moves_o;// ���������� �����

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
 ofstream Test_Loop_f;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*
 ����� �������� ������� ������ ��� �����. ���� ��� ��� ������ ����������.

 ������� ������� ������� ��������� �� ������� �����-���� ����������� ��� �������������
 ����������� ������ ��� ��������� ����� � ��������� �� ����. ���� ��� �������� ��� 
 ���������� �� ������ ������.

 ��� �� ������ ������ �������� ����� ��������� ��� ����� �����-���� �����������
 � ����� ����������� ��������� ������ �� ����������� �������.

 ����� ���� �������� ����� ��������� ������� ����� ������. ����� ������� ������ �������
 ����� ��� ����� � ���������� ������ � ���� �����. ����� ������� ����� ��������� ��������� 
 �����. ������ ���� �������. � ����� ��������� ������ ������� ��� ��� ����� � ���� ����.
 ����� ���������� ������ ������� ������ ����� � ��� ��������� ��� �����������. �������� 
 ����� ������� � ������ ��� ������ � ��������� ������ ������ �����. ���� ��� ��������� 
 ������ ���� ������� � ���������� ������ ����� ������ �����������. �� ������ ������ 
 ���������� ��� ��� ��� ���� ��� ����� ��� ������ ��������.
   
 � ������������� ������ �� ������� ������ ������ ��� ��� ��������� �� ����� ��� �����
 �� ����������� � ����� ������ ����������. ��� ���� ��� �� ��������� �� ������ 
 ������������� ������ �� ����������� ����� ����������� �������. � ����� ���� ���������
 ������ ������ ������� �� ��� ������� ��� � ���� ��� �� ��������� �� ���������� ������.

 ����� �� ���������� ����������� ������� �� ���� �� ������� �� ������������� ��� ���� 
 ������� ��� � ���� ������ � ��� ��� ���� ���� ������ �� �� ���� ������� ������ ���� 
 ������� ��. ��� ��� �� ������� ������ ������ � �� ����� ����� ����� ������������� ��
 ��������� �������� �������������� ��������.

1) ������� ����������� ������ �������.

  1.1 ���� ����������� ������ ������ ����� �� ����������� �� �����. ����� ������ 
   �������� ����� ����������� �������.

  1.2 ���� ����� �������� �� �������� ����� ������ �� ����. 

  1.3 �������� ���� �� ������ 1:

	if(value_stat > alpha){ 
	    if(value_stat >= beta) return value_stat;
		alpha = value_stat;
        save_best = 1;
	}

---------------------------------------
2) ����������� � ��������� �� mvv/lva ������ ��������� ������. ���������� �����
   �������������� �������� ��� ��������� ������.

  2.1  ������ ������� �����-����. 

  2.2 �������� ���� �� ������ 2:
  value_max_din = value_stat;
  if(value_din > value_max_din){
         value_max_din = value_din;
		 if(value_din > alpha) {
             if(value_din >= beta) return value_din;
			 alpha = value_din; 
             //����� �� �������������� ������ ������� ��������������� ������ ������
             pv_save_o.Update_PV_best_point(&pv_best_point);
             save_best = 1;
		 }
     }

  �������������� ������ ����������� � ��� ��� 
  �� ����� ������ �� ������� ������ ��� ����������� ������ ������� �������� �����
if(save_best == 1){
    if(value_max_din == value_stat){
         // ����������� ������ �������� �� �������

         // ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
         // � ������ ������� ����� �������� ������
         pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
	}else{
         // ������ �� ������� ������ ������� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
         pv_save_o.Update_PV_best(&pv_best_point);
	}//if(value_max_din == value_stat){
}//if(save_best == 1){
---------------------------------------

*/

//	===================================================================
// ������������ ������ ������ ��� �����
 __int32 QUIESCENCESEARCH::Forced_variant_white(struct T_Bitboard * const p_bitboard,// ������������� �����
						                    __int32 alpha,//
						                    __int32 beta,//
						                    unsigned __int8 depth,// �������
		                                    const bool flag_pv_f,// �������� �� ��� ������� (pv)
						                    const unsigned __int8 & depth_max// ������������ �������
						                    ){

bool save_best = 0;// ����� �� ������ �������
__int32 value_din; // ������������ ������
__int32 value_stat; // ����������� ������
__int32 value_max_din; // �������� � ���� (���� = -9999999)


struct T_undo undo[1];// ���������� ��� �������� ����
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
struct T_PV pv_best_point;// ������ ��� ������� ���� �������

#if DELTA_PRUNING

unsigned __int64 all_black_piece;

#endif //#if DELTA_PRUNING

    // ������� ����������� ������ �������
    value_stat = p_bitboard->eval + eval_o.Eval_State(p_bitboard);

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    value_stat = p_bitboard->eval;

 //------------------------------------------------
// TEST
// ��������� �� ������������ ��������� ������ ���������
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "WHITE  value_stat != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) {
//------------------------------------------------

	// �������� ����� ��� �� �� ������ ������������� ������.
	if(value_stat > alpha){ 
	    // �������� ����� ������ �� ���� 
        // ������ ������ �� ����?
        // ���� � ��� ��� � ������������� ������ �� ���������� ������� ����� ����� ������ � ��� ���� ����� ��������
        // ������ ����
        // �� ������� �� ���� ��� ���� � ��� ������� ��� �� ��������� ������
        // �� ������ ��� �� ������ �� �������
	    if(value_stat >= beta) return value_stat;
		alpha = value_stat;
        save_best = 1;
	}//if(value_stat > alpha){ 
    //
    value_max_din = value_stat;


#if DELTA_PRUNING

    // ���������� ������ �� �����
    all_black_piece = p_bitboard->all_black_piece;

	if(value_stat < alpha - PAWN_MARGIN ){
       // ������ ����� ���������
       p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_PAWN];

       if(value_stat < alpha - BISHOP_KNIGHT_MARGIN ){
          // ������ ������ � ����� ���������
          p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_KNIGHT];
          p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_BISHOP];

          if(value_stat < alpha - ROOK_MARGIN ){
              // ������ ����� ���������
              p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_ROOK];

	      }//if(value_stat < alpha - 700 ){

	   }//if(value_stat < alpha - 500 ){

	}//if(value_stat < alpha - 300 ){


#endif //#if DELTA_PRUNING


//����������� ������ ��������� ������ � ��������� ���
bitboard_generator_o.Generator_captures_white(p_bitboard,&list_surplus_moves);
//if (depth == 2) List_Print_F(depth, &list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);
//if (depth == 2) List_Print_F(depth, &list_surplus_moves);

#if DELTA_PRUNING
     // ���������� �� ����� ������
	 p_bitboard->all_black_piece = all_black_piece;
#endif //#if DELTA_PRUNING

//����� �� ������ ��������� �����
// ����� � �����-���� ����������
for(unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++){
              //TEST
              //cout << "white depth= " << depth<<"\n";
              //cout << "i = " << i <<"\n";
              //cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

// ��������� ���
   moves_o.Onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);

// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
   if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);


// ���� ������ �� �����
   if(( list_surplus_moves.move[i] & 63) != 6){

//   ����������� ����� �������
     value_din = Forced_variant_black(p_bitboard,alpha,beta,(depth+1),flag_pv_f,depth_max);

//   ��������������� �������
     moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

     if(value_din > value_max_din){
         value_max_din = value_din;
         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
		 if(value_din > alpha) {
             if(value_din >= beta) return value_din;
			 alpha = value_din; 
             //   ����� �� �������������� ������ ������� ��������������� ������ ������
             if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
             save_best = 1;
		 }//if(value_din > alpha) {
         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
     }//if(value_din > value_max_din){

  //-------------------------------------------------------------------
// ������������ ������ ������� ������
  }else{//if (list_surplus_moves->name_taking_figure[i] !=6){

//  ������ ������� !
//------------------------------------------------
// ��� ��� ������� + ��� - ??
// ��� ������� ����� ��� ��� ����� ������ ������� ������ ������ ������ ���� +
// ������ ����� ����������� ���
    value_stat = (1000000 - depth * 1000);
    moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);
    // ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
    // � ������ ������� ����� �������� ������
    if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
    return value_stat;

  }//if (list_surplus_moves->name_taking_figure[i] !=6){
  //--------------------------------------------------------------------

}//for(int i = 0; i < list_surplus_moves.end_captures; i++){

/*
�������������� ������ ����������� � ��� ��� �� ����� ������ �� ������� ������
��� ����������� ������ ������� �������� �����
*/
if(save_best == 1){
    if(value_max_din == value_stat){
         // ����������� ������ �������� �� �������

         // ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
         // � ������ ������� ����� �������� ������
         if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
	}else{
         // ������ �� ������� ������ ������� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
         if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	}//if(value_max_din == value_stat){
}//if(save_best == 1){


return value_max_din;
}//int QUIESCENCESEARCH::Forced_variant_white
//	*******************************************************************

//	===================================================================
// ������������ ������ ������ ��� ������
__int32 QUIESCENCESEARCH::Forced_variant_black(struct T_Bitboard * const p_bitboard,// ������������� �����
						                   __int32 alpha,//
						                   __int32 beta,//
						                   unsigned __int8 depth,// �������
		                                   const bool flag_pv_f,// �������� �� ��� ������� (pv)
						                   const unsigned __int8 & depth_max// ������������ �������
						                   ){

bool save_best = 0;// ����� �� ������ �������
__int32 value_din; // ������������ ������
__int32 value_stat; // ����������� ������
__int32 value_min_din; //������� � ���� (���� = 9999999)

struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)
struct T_PV pv_best_point;// ������ ��� ������� ���� �������

#if DELTA_PRUNING

unsigned __int64 all_white_piece;

#endif //#if DELTA_PRUNING

    value_stat = p_bitboard->eval + eval_o.Eval_State(p_bitboard);
 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    value_stat = p_bitboard->eval;

//------------------------------------------------
// TEST
// ��������� �� ������������ ��������� ������ ���������
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "BLACK value_stat != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) {
//------------------------------------------------

	if(value_stat < beta) {
	    if(value_stat <= alpha) return value_stat;
		beta = value_stat;
        save_best = 1;
	}//if(value_stat < beta)
	//
    value_min_din = value_stat;

#if DELTA_PRUNING

    // ���������� ������ �� �����
    all_white_piece = p_bitboard->all_white_piece;

	if(value_stat > beta + PAWN_MARGIN){
       // ������ ����� ���������
       p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_PAWN];

       if(value_stat > beta + BISHOP_KNIGHT_MARGIN){
          // ������ ������ � ����� ���������
          p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_KNIGHT];
          p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_BISHOP];      

          if(value_stat > beta + ROOK_MARGIN){
              // ������ ����� ���������
              p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_ROOK];   

	      }//if(value_stat > beta + 700){

	   }//if(value_stat > beta + 500){

	}//if(value_stat > beta + 300){


#endif //#if DELTA_PRUNING


//����������� ������ ��������� ������
bitboard_generator_o.Generator_captures_black(p_bitboard,&list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);

#if DELTA_PRUNING
   // ���������� �� ����� ������
   p_bitboard->all_white_piece = all_white_piece;
#endif //#if DELTA_PRUNING


//���� �� ��� ��� ������� �� ���� ������ ����������� ������ �� �����

//����� �� ������ ��������� �����
// ����� � ����� ���� ����������
for(unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++){
                       //TEST
                       //cout << "black depth= " << depth<<"\n";
                       //cout << "i = " << i <<"\n";
                       //cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

//  ��������� ���
    moves_o.Onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);

//  �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
    if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

// ���� ������ �� �����
    if(( list_surplus_moves.move[i] & 63) != 6){

//      ����������� ����� �������
        value_din = Forced_variant_white(p_bitboard,alpha,beta,(depth+1),flag_pv_f,depth_max);
//      ��������������� �������
        moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

        if(value_din < value_min_din){
             value_min_din = value_din;
             //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			 if(value_din < beta) {
                 if(value_din <= alpha) return value_din;
				 beta = value_din; 
                 // ����� �� �������������� ������ ������� ��������������� ������ ������
                 if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
                 save_best = 1;
			 }//if(value_din < beta) {
             //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        }//if(value_din < value_min_din){

// ������������ ������ ������� ������
   }else{//if (list_surplus_moves->name_taking_figure[i] !=6){

//------------------------------------------------
//      ������ ������� !

        value_stat = -(1000000 - depth * 1000);
        moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);
        // ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
        // � ������ ������� ����� �������� ������
        if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
        return value_stat;

   }//if (list_surplus_moves->name_taking_figure[i] !=6){

}//for(int i = 0; i < list_surplus_moves.end_list; i++){

if(save_best == 1){
    if(value_min_din == value_stat){
         // ����������� ������ �������� �� �������

         // ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
         // � ������ ������� ����� �������� ������
         if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
	}else{
         // ������ �� ������� ������ ������� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
         if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	}//if(value_min_din == value_stat)
}//if(save_best == 1){

return value_min_din;
}//int QUIESCENCESEARCH::Forced_variant_black
//	*******************************************************************

//	===================================================================
// ��������� ������ ���������� ���������� ������
inline unsigned __int8 QUIESCENCESEARCH::Sorting_captures_moves(struct T_list_surplus_moves * const list_surplus_moves// ������ �����
								                    ){
 // 
__int32 move_s;// ���������� ��� ����������� ����
__int32 score_s;// ���������� ��� ����������� ������ ����
unsigned __int8 end_captures = list_surplus_moves->end_captures;//���������� ������
unsigned __int8 n;

/*
���� � ��� ���� ������ ����� �� �� ��������� �� ������ ����������� ��� � ���������� ������
���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
����� ������� ����� �� ������� �� ����� ��� ������ � ��� ������������� �� ���������.
*/
for(n = 0; n < end_captures; n++){	
    for(unsigned __int8 i = n + 1; i < end_captures; i++){
 
        if(list_surplus_moves->sorting_score[n] < list_surplus_moves->sorting_score[i]){ 
//            ��������� ������ �� n     
	          move_s = list_surplus_moves->move[n];
              score_s = list_surplus_moves->sorting_score[n];

//            �������� �� i � n
	          list_surplus_moves->move[n] = list_surplus_moves->move[i];
              list_surplus_moves->sorting_score[n] = list_surplus_moves->sorting_score[i];

//            ��������������� ������
	          list_surplus_moves->move[i] = move_s;
              list_surplus_moves->sorting_score[i] = score_s;

	    }// if(list_surplus_moves->sorting_score[n] < list_surplus_moves->sorting_score[i]){ 
    }//for(int i = n + 1; i < captures; i++)
}//for(n = 0 ; n < captures ; n++){		
return n;
}//inline int QUIESCENCESEARCH::Sorting_captures_moves
//	*******************************************************************


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
#if TEST_F
void QUIESCENCESEARCH::List_Print_F(int depth,// ������� �������
				  const struct T_list_surplus_moves * list_surplus_moves//������ �����
				  ){

Test_Loop_f.open ("_TEST_LOOP_FORCED.txt",ios::app);	
//Test_Loop_f.open ("_TEST_LOOP_FORCED.txt");	
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop_f << "\n";
Test_Loop_f << "------------------------------------------------- "<<"\n";
Test_Loop_f <<"depth= "<< depth << "\n";
Test_Loop_f << "\n";
Test_Loop_f << "������ ��������� ����� ������ "<<"\n";

for (int i = 0 ; i < list_surplus_moves->end_list ; i++){	
     Test_Loop_f << "����� ���� �� ������ i= " << i << "\n";  
     Test_Loop_f << "������� ������ " << int_to_char_board(((list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
     Test_Loop_f << "������ ���� " << list_surplus_moves->sorting_score[i] << "\n";

     // ����� ���� ���� e2-e4
     //------------------------------
	 Test_Loop_f << " " << (x_to_notation(((list_surplus_moves->move[i]>>12)& 63)%8));
     Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
 //  Test_Loop_f << "\n";
     Test_Loop_f << " " << (x_to_notation(((list_surplus_moves->move[i]>>18)& 63)%8));
     Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
     //------------------------------

     Test_Loop_f << "��� ���� " << ((list_surplus_moves->move[i]>>6)& 63)  << "\n";
     Test_Loop_f << "������ ������ " << int_to_char_board(( list_surplus_moves->move[i] & 63),1)<< "\n";
	 Test_Loop_f << "��������� ��������� " << ((list_surplus_moves->move[i]>>12)& 63)  << "\n";
	 Test_Loop_f << "�������� ��������� " << ((list_surplus_moves->move[i]>>18)& 63)  << "\n";
      
	 Test_Loop_f << "\n";
}//for (i=1,i<list_surplus_moves->amount_moves+1){

Test_Loop_f << "list_surplus_moves->end_captures= " <<list_surplus_moves->end_captures << "\n";
Test_Loop_f << "list_surplus_moves->end_list= " <<list_surplus_moves->end_list << "\n";
Test_Loop_f << "------------------------------------------------- " << "\n";
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop_f.close();
}//void QUIESCENCESEARCH::List_Print_F
#endif
//	*******************************************************************
