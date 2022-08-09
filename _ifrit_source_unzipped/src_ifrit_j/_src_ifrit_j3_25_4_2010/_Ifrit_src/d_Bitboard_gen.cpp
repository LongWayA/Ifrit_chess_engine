#include <fstream>  // TEST
#include <iostream> // TEST
#include <stdlib.h>
using namespace std;

#include "d_Bitboard_const.h"
#include "d_Bitboard_gen.h"
#include "k_Util.h"

//-----------------------------------------------------------------------------------
/*+
 * реализуем битбоард(bit board) генератор
   это простой генератор без ротации тем не менее в его написании
   мне помогла статья:
   "Hyatt, Robert. Rotated bitmaps, a new twist on an old idea"
   кроме нее никаких толковых источников я не обнаружил :)
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 21.02.2009
 *
 *  ТЕСТИРОВАНИЕ
 *  тестируем в полном переборе на специальных тестовых позициях
 *  на данный момент(21.02.09) багов(bugs) не замечено
*/
//---------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_B
 ofstream Test_Bitboard;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*
логика
             и &  0 0 1 первый операнд
                  0 1 1 второй операнд
                  -----
                  0 0 1 результат

           или |  0 0 1
                  0 1 1
                  -----
                  0 1 1

   исключающее ^  0 0 1
   или            0 1 1
                  -----
                  0 1 0

*/

// задаем имя фигур(обязательно должно быть согласованно с модулем b_FEN.cpp и i_Forced.cpp)
#define N_KING    6 // король
#define N_QUEEN   5 // ферзь
#define N_ROOK    4 // ладья
#define N_BISHOP  3 // слон
#define N_KNIGHT  2 // конь
#define N_PAWN    1 // пешка

 //           тип хода
 #define NO_MOVE 0 // нет хода
 
 #define SIMPLE_MOVE 1 //простой ход
 #define CAPTURE_MOVE 2 //взятие
 #define LONG_CASTLING 3 //длинная рокировка
 #define SHORT_CASTLING 4 //короткая рокировка
 #define TAKING_PASSAGE 5 //взятие на проходе

 #define TRANSFORMATION_IN_KNIGHT 12 //превращение в конь 
 #define TRANSFORMATION_IN_BISHOP 13 //превращение в слон
 #define TRANSFORMATION_IN_ROOK  14 //превращение в ладья 
 #define TRANSFORMATION_IN_QUEEN 15 //превращение в ферзь 

 #define CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT 22 //взятие с превращением в конь 
 #define CAPTURE_WITH_TRANSFORMATION_IN_BISHOP 23 //взятие с превращением в слон
 #define CAPTURE_WITH_TRANSFORMATION_IN_ROOK 24 //взятие с превращением в ладья 
 #define CAPTURE_WITH_TRANSFORMATION_IN_QUEEN 25 //взятие с превращением в ферзь 

//public

//	===================================================================
// генерируем список взятий + превращения пешки (для белых)
void BITBOARDGENERATOR::Generator_captures_white(const struct T_Bitboard * const p_bitboard,// представление доски
                                                 struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
							                     ){
unsigned __int64 from_mask; // маска расположения фигур одного типа
unsigned __int64 one_pawn_mask; // маска расположения одной пешки
unsigned __int64 move_conversion_mask; // маска ходов пешки с превращеинем
unsigned __int64 captures_conversion_mask; // маска взятий пешки с превращениями
unsigned __int64 captures_mask; // маска взятий
unsigned __int64 blocking_square_mask; // маска блокированных полей

unsigned __int8 p; // счетчик ходов в списке
unsigned __int8 from; // координата поля откуда фигура ходит
unsigned __int8 to; // координата поля куда фигура ходит
__int32 blocking_coordinate; // координата блокированного поля 

// генерируем ходы белых фигур
p = 0;

// ходы пешки----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[N_PAWN];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
 //выделяем одну пешку 
   one_pawn_mask = from_mask & move_masks[from];
   from_mask = from_mask & (from_mask - 1);


 if ( from / 8 == 6 ){
     // считаем ходы с превращением и взятия с превращением
     // 1 находим простые ходы белых пешек на одно поле вперед
     move_conversion_mask = (one_pawn_mask << 8) & (~(p_bitboard->all_piece));
     // взятия с превращением
     captures_conversion_mask = pawn_capture_white[from] &  p_bitboard->all_black_piece;

//   находим координаты ходов с превращением пешки
     while (move_conversion_mask != 0){
        to = Bit_scan_forward_win32(move_conversion_mask);
        move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

//      отмечаем превращение пешки
//      превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,превращение в коня 12
        List_record_sp(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0,p_list_surplus_moves);
        List_record_sp(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0,p_list_surplus_moves);
        List_record_sp(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0,p_list_surplus_moves);
        List_record_sp(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0,p_list_surplus_moves);
 
        #if KONTROL_B
          KONTROL_move(&from,&to,&p);
        #endif//KONTROL_B
     }//while (move_conversion_mask != 0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
     // находим координаты взятий с превращением пешки  
     while (captures_conversion_mask != 0){
        to = Bit_scan_forward_win32(captures_conversion_mask);
        captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

//      отмечаем превращение пешки с взятием
//      превращение в ферзь 25,превращение в ладью 24,превращение в слона 23,превращение в коня 22
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);

        #if KONTROL_B
          KONTROL_captures_W(p_bitboard,&from,&to,&p,120);
        #endif//KONTROL_B
     }//while (captures_conversion_mask != 0){

 }else {
     // считаем взятия пешкой
     captures_mask = pawn_capture_white[from] &  p_bitboard->all_black_piece;
     while (captures_mask != 0){
        to = Bit_scan_forward_win32(captures_mask);
        List_capture_white(N_PAWN,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
        captures_mask = captures_mask & (captures_mask - 1);
     }//while (captures_mask != 0){
 }//if ( from / 8 == 6 ){


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// 4 находим взятия на проходе
   if(p_bitboard->taking_passage_yes == 1){
     if( from / 8 == 4 ){
       to = 8 * p_bitboard->taking_passage_y + p_bitboard->taking_passage_x;

       if( ((from - to) == -9) && (from % 8 != 7) ){
          List_record_sp(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->black_name_from_coord[to - 8],p_list_surplus_moves);

          #if KONTROL_B
            to = to - 8;
            KONTROL_captures_W(p_bitboard,&from,&to,&p,130);
          #endif//KONTROL_B
       }//if ( ((from - to) == -9) && (from % 8 != 7) ){

       if( ((from - to) == -7) && (from % 8 != 0) ){
          List_record_sp(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->black_name_from_coord[to - 8],p_list_surplus_moves);

          #if KONTROL_B
             to = to - 8;
             KONTROL_captures_W(p_bitboard,&from,&to,&p,130);
          #endif//KONTROL_B
       }//if ( ((from - to) == -7) && (from % 8 != 0) ){
	 }//if ( from / 8 == 4 ){
   }//if(p_bitboard->taking_passage_yes == 1){

}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------


// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
from_mask = p_bitboard->white_piece[N_KNIGHT];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);
   captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[from]);
   while (captures_mask != 0){
      to = Bit_scan_forward_win32(captures_mask);
      List_capture_white(N_KNIGHT,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
      captures_mask = captures_mask & (captures_mask - 1);
   }//while (captures_mask != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------


// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->white_piece[N_BISHOP];
while(from_mask != 0){
  from = Bit_scan_forward_win32(from_mask);
  from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх - вправо--------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы ладьи---------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[N_ROOK];
while(from_mask != 0){
  from = Bit_scan_forward_win32(from_mask);
  from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------


// ходы ферзя---------------------------------------------------------------------------------
// маска расположения белого ферзя
from_mask = p_bitboard->white_piece[N_QUEEN];
while(from_mask != 0){
  from = Bit_scan_forward_win32(from_mask);
//надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
  from_mask = from_mask & (from_mask - 1);

//смотрим в четырех различных направлениях (по типу ладьи)
//луч вверх------------------------------------------------------------------
//1) находим блокированные биты.
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

  if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
//   3) находим маску взятий
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
//   4) заполняем список ходов
     if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ферзя--------------------------------------------------------------------------------------


// ходы короля--------------------------------------------------------------------------------
// маска расположения белого короля
from_mask = p_bitboard->white_piece[N_KING];
if(from_mask != 0){
  // находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
  from = Bit_scan_forward_win32(from_mask);
  // находим маску взятий короля
  captures_mask = (p_bitboard->all_black_piece & king_moves_masks[from]);

  while (captures_mask != 0){
     // Заполняем список ходов
     to = Bit_scan_forward_win32(captures_mask);
     List_capture_white(N_KING,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
     captures_mask = captures_mask & (captures_mask - 1);
  }//while (captures_mask != 0){
}//if(from_mask != 0){
// ходы короля--------------------------------------------------------------------------------

p_list_surplus_moves->end_list = p;
p_list_surplus_moves->end_captures = p;

}//void Generator_captures_white(const struct T_p_bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// генерируем список взятий  + превращения пешки (для черных)
void BITBOARDGENERATOR::Generator_captures_black(const struct T_Bitboard * const p_bitboard,// представление доски
                                                 struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
							                     ){

unsigned __int64 from_mask; // маска расположения фигур одного типа
unsigned __int64 one_pawn_mask; // маска расположения одной пешки
unsigned __int64 move_conversion_mask; // маска ходов пешки с превращеинем
unsigned __int64 captures_conversion_mask; // маска взятий пешки с превращениями
unsigned __int64 captures_mask; // маска взятий
unsigned __int64 blocking_square_mask; // маска блокированных полей

unsigned __int8 p; // счетчик ходов в списке
unsigned __int8 from; // координата поля откуда фигура ходит
unsigned __int8 to; // координата поля куда фигура ходит
__int32 blocking_coordinate; // координата блокированного поля 

// генерируем ходы черных фигур
p = 0;

// ходы пешки----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_PAWN];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
 //выделяем одну пешку 
   one_pawn_mask = from_mask & move_masks[from];
   from_mask = from_mask & (from_mask - 1);

   if ( from / 8 == 1 ){
     // считаем ходы с превращением и взятия с превращением
     // 1 находим простые ходы черных пешек на одно поле вперед
     move_conversion_mask = (one_pawn_mask >> 8) & (~(p_bitboard->all_piece));
     // взятия с превращением
     captures_conversion_mask = pawn_capture_black[from] &  p_bitboard->all_white_piece;

//   находим координаты ходов с превращением пешки
     while (move_conversion_mask != 0){
        to = Bit_scan_forward_win32(move_conversion_mask);
        move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

//      отмечаем превращение пешки
//      превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,превращение в коня 12
        List_record_sp(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0,p_list_surplus_moves);
        List_record_sp(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0,p_list_surplus_moves);
        List_record_sp(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0,p_list_surplus_moves);
        List_record_sp(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0,p_list_surplus_moves);
 
        #if KONTROL_B
          KONTROL_move(&from,&to,&p);
        #endif//KONTROL_B
     }//while (move_conversion_mask != 0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//   находим координаты взятий с превращением пешки  
//   находим координаты взятий с превращением пешки  
     while (captures_conversion_mask != 0){
        to = Bit_scan_forward_win32(captures_conversion_mask);
        captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

//      отмечаем превращение пешки с взятием
//      превращение в ферзь 25,превращение в ладью 24,превращение в слона 23,превращение в коня 22
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
        List_record_sp(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);

        #if KONTROL_B
           KONTROL_captures_B(p_bitboard,&from,&to,&p,120);
        #endif//KONTROL_B
     }//while (captures_conversion_mask != 0){

 }else {
     // считаем взятия пешкой
     captures_mask =  pawn_capture_black[from] &  p_bitboard->all_white_piece;
     while (captures_mask != 0){
        to = Bit_scan_forward_win32(captures_mask);
        List_capture_black(N_PAWN,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
        captures_mask = captures_mask & (captures_mask - 1);
      }//while (captures_mask != 0){
 }// if ( from / 8 == 1 ){


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// 4 находим взятия на проходе
 if(p_bitboard->taking_passage_yes == 1){
   if( from / 8 == 3 ){

     to = 8 * p_bitboard->taking_passage_y + p_bitboard->taking_passage_x;

     if ( ((from - to) == 9) && (from % 8 != 0) ){
       List_record_sp(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->white_name_from_coord[to + 8],p_list_surplus_moves);

       #if KONTROL_B
         to = to + 8;
         KONTROL_captures_B(p_bitboard,&from,&to,&p,130);
       #endif//KONTROL_B
     }//if ((from - to) == 9) && (from % 8 != 0)){

     if( ((from - to) == 7) && (from % 8 != 7) ){
       List_record_sp(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->white_name_from_coord[to + 8],p_list_surplus_moves);

       #if KONTROL_B
          to = to + 8;
          KONTROL_captures_B(p_bitboard,&from,&to,&p,130);
       #endif//KONTROL_B
     }//if ( ( (from - to) == 9) | ((from - to) == 7)){
   }//if( from / 8 == 3 ){
  }//if(p_bitboard->taking_passage_yes == 1){

}//while(from_mask != 0){

// пешки----------------------------------------------------------------------------------


// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
from_mask = p_bitboard->black_piece[N_KNIGHT];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask - 1);
    captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[from]);

    while(captures_mask != 0){
       to = Bit_scan_forward_win32(captures_mask);
       List_capture_black(N_KNIGHT,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
       captures_mask = captures_mask & (captures_mask - 1);
    }//while (captures != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------


// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->black_piece[N_BISHOP];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) List_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) List_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
   if(captures_mask != 0) List_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы ладьи-----------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_ROOK];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) List_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) List_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) List_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) List_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------


// ходы ферзя----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_QUEEN];
while(from_mask != 0){
  from = Bit_scan_forward_win32(from_mask);
  from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях
// луч вверх - вправо---------------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) List_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------


// ходы короля---------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_KING];
if(from_mask != 0){
  from = Bit_scan_forward_win32(from_mask);
  captures_mask = (p_bitboard->all_white_piece & king_moves_masks[from]);

  while (captures_mask != 0){
     to = Bit_scan_forward_win32(captures_mask);
     List_capture_black(N_KING,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
     captures_mask = captures_mask & (captures_mask - 1);
   }//while (captures != 0){
}//if(from_mask != 0){
// короля---------------------------------------------------------------------------------


p_list_surplus_moves->end_list = p;
p_list_surplus_moves->end_captures = p;

}//void Generator_captures_black(const struct T_Bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)(для белых)
void BITBOARDGENERATOR::Generator_move_white(const struct T_Bitboard * const p_bitboard,// представление доски
                                             struct T_list_surplus_moves * const p_list_surplus_moves// список ходов     
						                     ){

unsigned __int64 from_mask; // маска расположения фигур одного типа
unsigned __int64 move_mask; // маска ходов здесь только для ходов пешки с превращением
unsigned __int64 blocking_square_mask; // маска блокированных полей
unsigned __int64 one_pawn_mask; // маска расположения одной пешки
unsigned __int64 move_mask_d; // маска хода пешки на два поля

unsigned __int8 p; // счетчик ходов в списке
unsigned __int8 from; // координата поля откуда фигура ходит
unsigned __int8 to; // координата поля куда фигура ходит
__int32 blocking_coordinate; // координата блокированного поля 

 p = p_list_surplus_moves->end_list;

// генерируем ходы белых фигур
// ходы короля--------------------------------------------------------------------------------
// маска расположения белого короля
from_mask = p_bitboard->white_piece[N_KING];
if(from_mask != 0){
   // находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
  from = Bit_scan_forward_win32(from_mask);
  // находим маску ходов короля(без взятий)
  move_mask = king_moves_masks[from] & (~p_bitboard->all_piece);

  while (move_mask != 0){
     to = Bit_scan_forward_win32(move_mask);
     move_mask = move_mask & (move_mask - 1);
     List_record_move(N_KING,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
  }//while (move_mask != 0){
//---------------------------------------------------------------------------

  // рокировки белого короля

  // если длинная рокировка разрешена
  if(p_bitboard->castling_Q){
  // 
     to = 2;
     #if KONTROL_B
        KONTROL_move(&from,&to,&p);
     #endif//KONTROL_B
     // поля  1 2 3 пустые  то добавляем ход
     if(((~p_bitboard->all_piece) & CASTLING_Q ) == CASTLING_Q ){
  //    длинная рокировка белых  0-0-0 проверяем под боем ли поля 2 3 4 
        if((White_check_square(2,p_bitboard) == 0) 
			&& (White_check_square(3,p_bitboard) == 0) 
			&& (White_check_square(4,p_bitboard) == 0)){

                  List_record_sp(N_KING,LONG_CASTLING,p,from,2,0,p_list_surplus_moves);

	    }//if((White_check_square(2,p_bitboard) == 0) && (White_check_square(3,p_bitboard) == 0) && (White_check_square(4,p_bitboard) == 0)){
     }//if(((~p_bitboard->all_piece) & CASTLING_Q ) == CASTLING_Q ){
  }//if(p_bitboard->castling_Q){

  // если короткая рокировка разрешена то добавляем ход 
  if(p_bitboard->castling_K){
     to = 6;
     #if KONTROL_B
        KONTROL_move(&from,&to,&p);
     #endif//KONTROL_B
     //поля  5 6 пустые  то добавляем ход
     if( ( (~p_bitboard->all_piece) & CASTLING_K ) == CASTLING_K ){
        // короткая рокировка белых 0-0 проверяем под боем ли поля  4 5 6
	    if((White_check_square(4,p_bitboard) == 0)
			&& (White_check_square(5,p_bitboard) == 0)
			&& (White_check_square(6,p_bitboard) == 0)){

                List_record_sp(N_KING,SHORT_CASTLING,p,from,6,0,p_list_surplus_moves);

	    }//if((White_check_square(4,p_bitboard)==0)&&(White_check_square(5,p_bitboard)==0)&&(White_check_square(6,p_bitboard)==0)){
      }//if(( (p_bitboard->all_piece) & 96)){ 
  }//if(p_bitboard->castling_K){ 
}//if(from_mask != 0){
// ходы короля--------------------------------------------------------------------------------


// ходы ферзя----------------------------------------------------------------------------------
// маска расположения белого ферзя
from_mask = p_bitboard->white_piece[N_QUEEN];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

}//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------



// ходы ладьи----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[N_ROOK];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------



// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->white_piece[N_BISHOP];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
from_mask = p_bitboard->white_piece[N_KNIGHT];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);
   move_mask = knigt_moves_masks[from] & (~p_bitboard->all_piece);

  while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[N_PAWN];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
 //выделяем одну пешку 
   one_pawn_mask = from_mask & move_masks[from];
   from_mask = from_mask & (from_mask - 1);

// 1 находим простые ходы белых пешек на одно поле вперед
   move_mask = (one_pawn_mask << 8) & (~(p_bitboard->all_piece));
// 2 находим ходы пешек на 2 поля вперед 
// в начале оставляем только пешки стоящие на 2 горизонтали
   move_mask_d = one_pawn_mask & LINE_1;
// отметаем пешки если они чем то заблокированны
   move_mask_d = (move_mask_d << 8) & (~(p_bitboard->all_piece));
// оставшимися пешками делаем ход
   move_mask_d = (move_mask_d << 8) & (~(p_bitboard->all_piece));

// здесь все ходы пешки как на одно так на два поля вперед
   move_mask = (move_mask | move_mask_d) ;

// ходы без превращений
   move_mask = move_mask & ~LINE_7;

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------

p_list_surplus_moves->end_list = p;

}//void Generator_move_white(const struct T_Bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)(для черных)
void BITBOARDGENERATOR::Generator_move_black(const struct T_Bitboard * const p_bitboard,// представление доски
                                             struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						                     ){

unsigned __int64 from_mask; // маска расположения фигур одного типа
unsigned __int64 move_mask; // маска ходов здесь только для ходов пешки с превращением
unsigned __int64 blocking_square_mask; // маска блокированных полей
unsigned __int64 one_pawn_mask; // маска расположения одной пешки
unsigned __int64 move_mask_d; // маска хода пешки на два поля

unsigned __int8 p; // счетчик ходов в списке
unsigned __int8 from; // координата поля откуда фигура ходит
unsigned __int8 to; // координата поля куда фигура ходит
__int32 blocking_coordinate; // координата блокированного поля 

p = p_list_surplus_moves->end_list;

// генерируем ходы черных фигур
// ходы короля---------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_KING];
if(from_mask != 0){
  from = Bit_scan_forward_win32(from_mask);
  move_mask = king_moves_masks[from] & (~(p_bitboard->all_piece));

  while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_KING,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){

// рокировки черного короля

// если длинная рокировка разрешена
   if(p_bitboard->castling_q){
      to = 58;
      #if KONTROL_B
         KONTROL_move(&from,&to,&p);
      #endif//KONTROL_B
//    и поля  1 2 3 пустые  то добавляем ход
      if((~(p_bitboard->all_piece) & CASTLING_q )== CASTLING_q ){
//       длинная рокировка черных проверяем 0-0-0 58 59 60
	     if((Black_check_square(58,p_bitboard) == 0)
			 && (Black_check_square(59,p_bitboard) == 0)
			 && (Black_check_square(60,p_bitboard) == 0)){

                 List_record_sp(N_KING,LONG_CASTLING,p,from,58,0,p_list_surplus_moves);			 

		 }//if(Black_check_square(58,p_bitboard)==0)&&(Black_check_square(59,p_bitboard)==0)&&(Black_check_square(60,p_bitboard)==0)){
	   }//if((~(p_bitboard->all_piece) & CASTLING_q )== CASTLING_q ){
	}//if(p_bitboard->castling_q){

// если короткая рокировка разрешена то добавляем ход 
   if(p_bitboard->castling_k){
     to = 62;
     #if KONTROL_B
        KONTROL_move(&from,&to,&p);
     #endif//KONTROL_B
//   и поля  5 6 пустые  то добавляем ход
     if((~(p_bitboard->all_piece) & CASTLING_k )== CASTLING_k){
//      короткая рокировка черных проверяем 0-0 60 61 62
	    if((Black_check_square(60,p_bitboard) == 0)
			&& (Black_check_square(61,p_bitboard) == 0)
			&&(Black_check_square(62,p_bitboard) == 0)){

                  List_record_sp(N_KING,SHORT_CASTLING,p,from,62,0,p_list_surplus_moves);

		}//if((Black_check_square(60,p_bitboard)==0)&&(Black_check_square(61,p_bitboard)==0)&&(Black_check_square(62,p_bitboard)==0)){
	  }//if((~(p_bitboard->all_piece) & CASTLING_k )==CASTLING_k){
	}//if(p_bitboard->castling_k){
}//if(from_mask != 0){
// короля---------------------------------------------------------------------------------


// ходы ферзя----------------------------------------------------------------------------------
// маска расположения ферзя
from_mask = p_bitboard->black_piece[N_QUEEN];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

}//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------


// ходы ладьи----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_ROOK];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------



// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->black_piece[N_BISHOP];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
from_mask = p_bitboard->black_piece[N_KNIGHT];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask - 1);
   move_mask = knigt_moves_masks[from] & (~p_bitboard->all_piece);

  while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_PAWN];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   one_pawn_mask = from_mask & move_masks[from];
   from_mask = from_mask & (from_mask - 1);

// 1 находим простые ходы черных пешек на одно поле вперед
   move_mask = (one_pawn_mask >> 8) & (~(p_bitboard->all_piece));

// 2 находим ходы пешек на 2 поля вперед 
// в начале оставляем только пешки стоящие на 7 горизонтали
   move_mask_d = one_pawn_mask & LINE_6;

// отметаем пешки если они чем то заблокированны
   move_mask_d = (move_mask_d >> 8) & (~(p_bitboard->all_piece));
// оставшимися пешками делаем ход
   move_mask_d = (move_mask_d >> 8) & (~(p_bitboard->all_piece));

// здесь все ходы пешки как на одно так на два поля вперед
   move_mask = (move_mask | move_mask_d) ;

// ходы без превращений
   move_mask = move_mask & ~LINE_0;

   while (move_mask != 0){
      to = Bit_scan_forward_win32(move_mask);
      move_mask = move_mask & (move_mask - 1);
      List_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------

p_list_surplus_moves->end_list = p;

}//void Generator_move_black(const struct T_Bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 BITBOARDGENERATOR::King_white_check(const struct T_Bitboard * const p_bitboard// представление доски
					 ){

unsigned __int64 from_mask; // маска расположения фигур одного типа
unsigned __int64 blocking_square_mask; // маска блокированных полей
unsigned __int64 captures_mask; // маска взятий


unsigned __int8 from; // координата поля откуда фигура ходит
__int32 blocking_coordinate; // координата блокированного поля 
unsigned __int8 i; // координата 

// маска расположения белого короля
  from_mask = p_bitboard->white_piece[6];
 // находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
  from = Bit_scan_forward_win32(from_mask);

// луч вверх------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях
// луч вверх - вправо--------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
        if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// по типу коня--------------------------------------------------------------
 captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[from]);
 while(captures_mask != 0 ){
   if( p_bitboard->black_name_from_coord[Bit_scan_forward_win32(captures_mask)] == N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask ==0){
//---------------------------------------------------------------------------


// короля--------------------------------------------------------------------
  captures_mask = (p_bitboard->all_black_piece & king_moves_masks[from]);
 while(captures_mask != 0){
   if( p_bitboard->black_name_from_coord[Bit_scan_forward_win32(captures_mask)] == N_KING) return N_KING;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask ==0){
//---------------------------------------------------------------------------

// Пешки считаем напрямую----------------------------------------------------
if ((from / 7) < 8){
   if (from % 8 != 0){
     if( p_bitboard->black_name_from_coord[from + 7] == N_PAWN) return N_PAWN;
   }//if (from % 8 != 1){
   if (from % 8 != 7){
     if( p_bitboard->black_name_from_coord[from + 9] == N_PAWN) return N_PAWN;
   }//if (from % 8 != 7){
}//if ((from / 7) < 8){
//---------------------------------------------------------------------------

 return 0; 
}//int White_check_square(int sq, T_Bitboard * p_bitboard){
//	*******************************************************************

//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 BITBOARDGENERATOR::King_black_check(const struct T_Bitboard * const p_bitboard// представление доски
					 ){

unsigned __int64 from_mask; // маска расположения фигур одного типа
unsigned __int64 blocking_square_mask; // маска блокированных полей
unsigned __int64 captures_mask; // маска взятий


unsigned __int8 from; // координата поля откуда фигура ходит
__int32 blocking_coordinate; // координата блокированного поля 
unsigned __int8 i; // координата 

 // Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = p_bitboard->black_piece[6];
  from = Bit_scan_forward_win32(from_mask);
 
// смотрим в четырех различных направлениях
// луч вверх------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
       if(captures_mask != 0){
          i = Bit_scan_forward_win32(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
       }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = Bit_scan_forward_win32(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0){
          i = Bit_scan_forward_win32(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
     }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = Bit_scan_forward_win32(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
         i = Bit_scan_forward_win32(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
         i = Bit_scan_forward_win32(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
   if(captures_mask != 0){
         i = Bit_scan_forward_win32(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
   }//if(captures_mask != 0){
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0){
         i = Bit_scan_forward_win32(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0){
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// По типу коня--------------------------------------------------------------
 captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[from]);
 while(captures_mask != 0){
   if( p_bitboard->white_name_from_coord[Bit_scan_forward_win32(captures_mask)] ==  N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask != 0){
//---------------------------------------------------------------------------

// по типу короля------------------------------------------------------------
 captures_mask = (p_bitboard->all_white_piece & king_moves_masks[from]);
 while(captures_mask != 0){
   if( p_bitboard->white_name_from_coord[Bit_scan_forward_win32(captures_mask)] == N_KING) return N_KING;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask ==0){
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------- 
if ((from / 7) > 1 ){
// Пешки считаем напрямую
   if (from % 8 != 7){
     if( p_bitboard->white_name_from_coord[from - 7] == N_PAWN) return N_PAWN;
   }// if (from % 8 != 7){
   if (from % 8 != 0){
     if( p_bitboard->white_name_from_coord[from - 9] == N_PAWN) return N_PAWN;
   }// if (from % 8 != 7){
}//if ((from / 7) >0){
//---------------------------------------------------------------------------

 return 0;
 
}//int White_check_square(int sq, T_Bitboard * p_bitboard){
//	*******************************************************************



//private

//	===================================================================
// под боем ли белые поля если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
inline unsigned __int8 BITBOARDGENERATOR::White_check_square(const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
									                         const struct T_Bitboard * const p_bitboard// представление доски
									                         ){

unsigned __int64 blocking_square_mask; // маска блокированных полей
unsigned __int64 captures_mask; // маска взятий

__int32 blocking_coordinate; // координата блокированного поля 
unsigned __int8 i; // координата 

// луч вверх------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вверх - вправо--------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) {
        i = Bit_scan_forward_win32(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// по типу коня
//---------------------------------------------------------------------------
 captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[sq]);
 while(captures_mask != 0 ){
   if( p_bitboard->black_name_from_coord[Bit_scan_forward_win32(captures_mask)] ==  N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask ==0){
//---------------------------------------------------------------------------

// по типу короля
//---------------------------------------------------------------------------
 captures_mask = (p_bitboard->all_black_piece & king_moves_masks[sq]);
 while(captures_mask != 0){
   if( p_bitboard->black_name_from_coord[Bit_scan_forward_win32(captures_mask)] == N_KING) return N_KING;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask ==0){
//---------------------------------------------------------------------------


// пешки считаем напрямую
//---------------------------------------------------------------------------
 if( p_bitboard->black_name_from_coord[sq + 7] == N_PAWN) return N_PAWN;
 if( p_bitboard->black_name_from_coord[sq + 9] == N_PAWN) return N_PAWN;
//---------------------------------------------------------------------------

 return 0; 
}//int White_check_square(int sq, T_Bitboard * p_bitboard){
//	*******************************************************************

//	===================================================================
// под боем ли черные поля если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
inline unsigned __int8 BITBOARDGENERATOR::Black_check_square(const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
									                         const struct T_Bitboard * const p_bitboard// представление доски
									                         ){

unsigned __int64 blocking_square_mask; // маска блокированных полей
unsigned __int64 captures_mask; // маска взятий

__int32 blocking_coordinate; // координата блокированного поля 
unsigned __int8 i; // координата 

// луч влево-----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[sq];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = Bit_scan_forward_win32(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[sq];
   if(blocking_square_mask != 0){ 
     blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0){
          i = Bit_scan_forward_win32(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
     }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[sq];
   if(blocking_square_mask != 0){ 
      blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = Bit_scan_forward_win32(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[sq];
 if(blocking_square_mask != 0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
   if(captures_mask != 0){
         i = Bit_scan_forward_win32(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
   }//if(captures_mask != 0){
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[sq];
 if(blocking_square_mask != 0){ 
    blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0){
         i = Bit_scan_forward_win32(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0){
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// по типу коня
//---------------------------------------------------------------------------
 captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[sq]);
 while(captures_mask != 0){
   if( p_bitboard->white_name_from_coord[Bit_scan_forward_win32(captures_mask)] == N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask != 0){
//---------------------------------------------------------------------------

// по типу короля
//---------------------------------------------------------------------------
 captures_mask = (p_bitboard->all_white_piece & king_moves_masks[sq]);
 while(captures_mask != 0){
   if( p_bitboard->white_name_from_coord[Bit_scan_forward_win32(captures_mask)] == N_KING) return N_KING;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask != 0){
//---------------------------------------------------------------------------

// пешки считаем напрямую
//---------------------------------------------------------------------------
 if( p_bitboard->white_name_from_coord[sq - 7] == N_PAWN) return N_PAWN;
 if( p_bitboard->white_name_from_coord[sq - 9] == N_PAWN) return N_PAWN;
//---------------------------------------------------------------------------

 return 0; 
}//int Black_check_square(int sq, T_Bitboard * p_bitboard){
//	*******************************************************************

//	===================================================================
// начальная инициализация доски
inline void BITBOARDGENERATOR::Bitboard_ini_t(struct T_Bitboard * p_bitboard// представление доски
				    ){

p_bitboard->all_piece = 0;
p_bitboard->all_white_piece = 0;
p_bitboard->all_black_piece = 0;

for(unsigned __int8 i=0 ; i<7 ; i++){
  p_bitboard->white_piece[i]= 0;
  p_bitboard->black_piece[i]= 0;
}//for(int i=0 ; i<8 ; i++){

// медленней но очевидней
for(unsigned __int8 j=0 ; j<64 ; j++){
  p_bitboard->white_name_from_coord[j]= 0;
  p_bitboard->black_name_from_coord[j]= 0;
}//for(int j=0 ; j<64 ; j++){

}//void Bitboard_ini_t(T_Bitboard * p_bitboard){
//	*******************************************************************




//	===================================================================
// заполняем список ходов - взятия за белых
// две функции только из за разного контроля списков(строки взятия)
inline void BITBOARDGENERATOR::List_capture_white(unsigned __int8 name,// имя ходящей фигуры
									              unsigned __int8 type_move,// тип хода
									              const unsigned __int8 & from,// откуда ходим
									              const unsigned __int8 & to,// куда ходим
									              unsigned __int8 & count,// номер хода в списке
									              const struct T_Bitboard * const p_bitboard,// представление доски
                                                  struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
									              ){

unsigned __int8 capture;// взятая фигура


#if KONTROL_B
KONTROL_captures_W(p_bitboard,&from,&to,&count,name);
#endif//KONTROL_B

// взятая фигура
capture = p_bitboard->black_name_from_coord[to];

// записываем оценку взятия(mvv/lva в моем понимании) для последующей сортировки
p_list_surplus_moves->sorting_score[count] = 10 * capture - name; 

// 6 бит 
// записали имя взятой фигуры (1 - пешка 2 - конь 3 - слон 4 - ладья 5 - ферзь 6 - король)
p_list_surplus_moves->move[count] = capture

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (type_move << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "p_list_surplus_moves->move[count]= "<< p_list_surplus_moves->move[count] << "\n";
//
//cout << "p_bitboard->black_name_from_coord[to]= "<< p_bitboard->black_name_from_coord[to] << "\n";
//cout << "p_bitboard->black_name_from_coord[to]= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "type_move= "<< type_move << "\n";
//cout << "type_move= "<< ((p_list_surplus_moves->move[count]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((p_list_surplus_moves->move[count]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((p_list_surplus_moves->move[count]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((p_list_surplus_moves->move[count]>>24)& 7)  << "\n";
//-------------------------------------------------------------------------------

count = count + 1;
 
}//static inline void List_capture_white(int name,int type_move,int & from,int & to,int & count,const struct T_p_bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// заполняем список ходов - взятия за черных
// две функции только из за разного контроля списков(строки взятия)
inline void BITBOARDGENERATOR::List_capture_black(unsigned __int8 name,// имя ходящей фигуры
									              unsigned __int8 type_move,// тип хода
									              const unsigned __int8 & from,// откуда ходим
									              const unsigned __int8 & to,// куда ходим
									              unsigned __int8 & count,// номер хода в списке
									              const struct T_Bitboard * const p_bitboard,// представление доски
                                                  struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
									              ){


unsigned __int8 capture;// взятая фигура

#if KONTROL_B
KONTROL_captures_B(p_bitboard,&from,&to,&count,name);
#endif//KONTROL_B

// взятая фигура
capture = p_bitboard->white_name_from_coord[to];

// записываем оценку взятия(mvv/lva в моем понимании) для последующей сортировки
p_list_surplus_moves->sorting_score[count] = 10 * capture - name; 

// 6 бит 
// записали имя взятой фигуры (1 - пешка 2 - конь 3 - слон 4 - ладья 5 - ферзь 6 - король)
p_list_surplus_moves->move[count] = capture

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (type_move << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "p_list_surplus_moves->move[count]= "<< p_list_surplus_moves->move[count] << "\n";
//
//cout << "p_bitboard->white_name_from_coord[to]= "<< p_bitboard->white_name_from_coord[to] << "\n";
//cout << "p_bitboard->white_name_from_coord[to]= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "type_move= "<< type_move << "\n";
//cout << "type_move= "<< ((p_list_surplus_moves->move[count]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((p_list_surplus_moves->move[count]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((p_list_surplus_moves->move[count]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((p_list_surplus_moves->move[count]>>24)& 7)  << "\n";
//-------------------------------------------------------------------------------

count = count + 1;
}//static inline void List_capture_black(int name,int type_move,int & from,int & to,int & count,const struct T_p_bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// заполняем список ходов
inline void BITBOARDGENERATOR::List_record_move(unsigned __int8 name,// имя ходящей фигуры
									            unsigned __int8 type_move,// тип хода
									            const unsigned __int8 & from,// откуда ходим
									            const unsigned __int8 & to,// куда ходим
									            unsigned __int8 & count,// номер хода в списке
									            const struct T_Bitboard * const p_bitboard,// представление доски
                                                struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
	                                            ){

#if KONTROL_B
 KONTROL_move(&from,&to,&count);
#endif//KONTROL_B

// 6 бит 
// взятая фигура 0
p_list_surplus_moves->move[count] = 0

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (type_move << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "p_list_surplus_moves->move[p]= "<< p_list_surplus_moves->move[p] << "\n";
//
//cout << "0= "<< 0 << "\n";
//cout << "0= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((p_list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((p_list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((p_list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((p_list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------

count = count + 1;
}//
//	*******************************************************************

//	===================================================================
// заполняем список ходов для особых случаев типа рокировки взятия на проходе и превращений пешек
inline void BITBOARDGENERATOR::List_record_sp(unsigned __int8 name,// имя ходящей фигуры
								              unsigned __int8 d_m,// тип хода
								              unsigned __int8 & p,// номер хода в списке
								              const unsigned __int8 & from,// откуда ходим
								              unsigned __int8 to,// куда ходим
								              unsigned __int8 capture,// имя взятой фигуры
                                              struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
								              ){

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// тут рокировка превращение пешек взятие на проходе 
// таких ходов мало и лучше их посмотреть первыми среди простых
p_list_surplus_moves->sorting_score[p] = d_m;

// 6 бит 
// взятая фигура 0
p_list_surplus_moves->move[p] = capture

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (d_m << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "p_list_surplus_moves->move[p]= "<< p_list_surplus_moves->move[p] << "\n";
//
//cout << "0= "<< 0 << "\n";
//cout << "0= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((p_list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((p_list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((p_list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((p_list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------

p = p + 1;
}//static inline void List_record_sp(int name,int d_m,int & p,int & from,int to,int capture,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для белых фигур генератора взятий
inline void BITBOARDGENERATOR::KONTROL_captures_W(const struct T_Bitboard * p_bitboard,// представление доски
							                      const unsigned __int8 * p_from,// откуда ходит фигура
							                      const unsigned __int8 * p_to,// куда ходит фигура
							                      const unsigned __int8 * p_p,// номер хода в списке
							                      unsigned __int8 name_figure// имя ходящей фигуры
							                      ){

#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p_p < 0)||(*p_p > 256))   cout << "ERROR Generator_captures p= "   << *p_p    << "\n";
if ((*p_from < 0)||(*p_from > 63)) cout << "ERROR Generator_captures from= " << *p_from << "\n";
if ((*p_to < 0)||(*p_to > 63))     cout << "ERROR Generator_captures to= "  << *p_to   << "\n";
if ((p_bitboard->white_name_from_coord[*p_from] < 1)||(p_bitboard->white_name_from_coord[*p_from] > 6))     cout << "ERROR Generator_captures p_bitboard->white_name_from_coord[*from] = "  << p_bitboard->white_name_from_coord[*p_from]   << "\n";
if ((p_bitboard->black_name_from_coord[*p_to] < 1)||(p_bitboard->black_name_from_coord[*p_to] > 6)){
	cout << "ERROR Generator_captures p_bitboard->black_name_from_coord[*p_to]= "  << p_bitboard->black_name_from_coord[*p_to]   << "\n";
    cout << "ERROR Generator_captures from= "<< *p_from << "\n";
    cout << "ERROR Generator_captures to= "  << *p_to   << "\n";
    cout << "ERROR Generator_captures name_figure= "  << name_figure << "\n";
    Bitboard_print(p_bitboard);
}//if ((p_bitboard->black_name_from_coord[*to] <1)||(p_bitboard->black_name_from_coord[*to]>6)){
//if (p_bitboard->black_name_from_coord[*to] ==6) cout << "ERROR Generator_captures name_figure KING "  << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

//	===================================================================
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для черных фигур генератора взятий
inline void BITBOARDGENERATOR::KONTROL_captures_B(const struct T_Bitboard * p_bitboard,// представление доски
							                      const unsigned __int8 * p_from,// откуда ходит фигура
							                      const unsigned __int8 * p_to,// куда ходит фигура
							                      const unsigned __int8 * p_p,// номер хода в списке
							                      unsigned __int8 name_figure// имя ходящей фигуры
							                      ){

#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p_p < 0)||(*p_p > 256))   cout << "ERROR Generator_captures p= "   << *p_p    << "\n";
if ((*p_from < 0)||(*p_from > 63)) cout << "ERROR Generator_captures from= " << *p_from << "\n";
if ((*p_to < 0)||(*p_to > 63))     cout << "ERROR Generator_captures to= "  << *p_to   << "\n";
if ((p_bitboard->black_name_from_coord[*p_from] < 1)||(p_bitboard->black_name_from_coord[*p_from] > 6))     cout << "ERROR Generator_captures p_bitboard->black_name_from_coord[*p_from] = "  << p_bitboard->black_name_from_coord[*p_from]   << "\n";
if ((p_bitboard->white_name_from_coord[*p_to] < 1)||(p_bitboard->white_name_from_coord[*p_to] > 6)){
	cout << "ERROR Generator_captures p_bitboard->white_name_from_coord[*p_to]= "  << p_bitboard->white_name_from_coord[*p_to]   << "\n";
    cout << "ERROR Generator_captures from= "<< *p_from << "\n";
    cout << "ERROR Generator_captures to= "  << *p_to   << "\n";
    cout << "ERROR Generator_captures name_figure= "  << name_figure << "\n";
    Bitboard_print(p_bitboard);
}//if ((p_bitboard->white_name_from_coord[*to] <1)||(p_bitboard->white_name_from_coord[*to]>6)){
//if (p_bitboard->white_name_from_coord[*to] ==6) cout << "ERROR Generator_captures name_figure KING "  << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

//	===================================================================
// контролируем выход за пределы доски, за пределы списка 
// для генератора ходов 
inline void BITBOARDGENERATOR::KONTROL_move(const unsigned __int8 * p_from,// откуда ходит фигура
						                    const unsigned __int8 * p_to,// куда ходит фигура
						                    const unsigned __int8 * p_p// номер хода в списке
						                    ){

#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p_p < 0)||(*p_p > 256))   cout << "ERROR Generator_move p= "   << *p_p    << "\n";
if ((*p_from < 0)||(*p_from > 63)) cout << "ERROR Generator_move from= " << *p_from << "\n";
if ((*p_to < 0)||(*p_to > 63))     cout << "ERROR Generator_move to= "  << *p_to   << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

//	===================================================================
// печатаем битбоард доску
#if TEST_B
void BITBOARDGENERATOR::Bitboard_print(const struct T_Bitboard * p_bitboard// представление доски
					){
unsigned __int64 z = 0;
    Test_Bitboard.open ("_TEST_BITBOARD.txt",ios::app);

Test_Bitboard << "bitboard->colour_move = "<< p_bitboard->colour_move <<"\n";
Test_Bitboard << "\n";

 Test_Bitboard << "p_bitboard->taking_passage_yes= " << p_bitboard->taking_passage_yes << "\n";
 Test_Bitboard << "bitboard->taking_passage_x= " << p_bitboard->taking_passage_x << "\n";
 Test_Bitboard << "p_bitboard->taking_passage_y= " << p_bitboard->taking_passage_y << "\n";

Test_Bitboard << "\n";
 Test_Bitboard << "p_bitboard->castling_K= " << p_bitboard->castling_K << "\n";
 Test_Bitboard << "p_bitboard->castling_Q= " << p_bitboard->castling_Q << "\n";
 Test_Bitboard << "p_bitboard->castling_k= " << p_bitboard->castling_k << "\n";
 Test_Bitboard << "p_bitboard->castling_q= " << p_bitboard->castling_q << "\n";

Test_Bitboard << "------------------------"<<"\n";
Test_Bitboard << "\n";
// all_white_piece
//-------------------------------------------------------------------------------------
Test_Bitboard << "\n";
Test_Bitboard << "all_white_piece"<< "\n";
	for (int i = 63; i > -1 ; i = i - 8){
       for (int j = i - 7; j < i + 1 ; j++){
         z = p_bitboard->all_white_piece & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------

// all_black_piece
//-------------------------------------------------------------------------------------
Test_Bitboard << "\n";
Test_Bitboard << "all_black_piece"<< "\n";
	for (int i = 63; i > -1 ; i = i - 8){
       for (int j = i - 7; j < i + 1 ; j++){
         z = p_bitboard->all_black_piece & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
 Test_Bitboard << "\n";
 Test_Bitboard << "white_piece"<< "\n";
for(int p = 1 ; p < 7 ; p++){
Test_Bitboard << "\n";
Test_Bitboard << p << "\n";
	for (int i = 63; i > -1 ; i = i - 8){
       for (int j = i - 7; j < i + 1; j++){
         z =  p_bitboard->white_piece[p] & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
}//for(int i=0 ; i<8 ; i++){
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
 Test_Bitboard << "\n";
 Test_Bitboard << "black_piece"<< "\n";
for(int p = 1; p < 7; p++){
Test_Bitboard << "\n";
Test_Bitboard << p << "\n";
	for (int i = 63; i > -1; i = i - 8){
       for (int j = i - 7; j < i + 1 ; j++){
         z =  p_bitboard->black_piece[p] & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
}//for(int i=0 ; i<8 ; i++){
//-------------------------------------------------------------------------------------

Test_Bitboard << "============================================"<< "\n";

    Test_Bitboard.close();
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
}//void Bitboard_print(T_Bitboard * p_bitboard){
#endif//TEST_B
//	*******************************************************************

//	===================================================================
// ищем первый установленный бит(т.е. бежим справа)
__inline __int32 BITBOARDGENERATOR::Bit_scan_forward_win32(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
							                               ){ 
   _asm { 
/*
из учебника по ассемблеру(This document saved from http://mind.telenet.ru/samouch/7/31/index.html):
Команда mov замещает первый операнд (приемник) вторым (источником). 
При этом исходное значение первого операнда теряется. 
Второй операнд не изменяется. В зависимости от описания операндов, 
пересылается слово или байт. Если операнды описаны по-разному или 
режим адресации не позволяет однозначно определить размер операнда, 
для уточнения размера передаваемых данных в команду следует включить один 
из атрибутных операторов byte ptr или word ptr. 
Команда не воздействует на флаги процессора. 
Часто возникает необходимость выполнить обратную операцию - 
к паре байтов обратиться, как к слову. Для этого надо использовать оператор word ptr:
*/
           mov  eax, dword ptr analyzed_number[0]
/*
Команда test выполняет операцию логического умножения 
И над двумя операндами и, в зависимости от результата, 
устанавливает флаги SF, ZF и PF. Флаги OF и CF сбрасываются, 
a AF имеет неопределенное значение. Состояние флагов можно затем 
проанализировать командами условных переходов. Команда test не изменяет 
ни один из операндов. В качестве первого операнда команды test можно 
указывать регистр (кроме сегментного) или ячейку памяти, в качестве 
второго - регистр (кроме сегментного), ячейку памяти или непосредственное 
значение, однако не допускается определять оба операнда одновременно как 
ячейки памяти. Операнды могут быть байтами или словами и представлять числа 
со знаком или без знака. 

Правила побитового умножения:

Первый операнд-бит 0101
Второй операнд-бит 0011
Бит результата 0001

Флаг SF устанавливается в 1, если в результате выполнения команды 
образовалось число с установленным знаковым битом.
Флаг ZF устанавливается в 1, если в результате выполнения команды 
образовалось число, состоящее из одних двоичных нулей.
Флаг PF устанавливается в 1, если в результате выполнения команды 
образовалось число с четным количеством двоичных единиц в его битах.
*/
           test eax, eax
/*
Команда Перейти, если Условие перехода
jz нуль ZF= 1
*/
           jz   grown_up
/*
Команда bsf сканирует слово или двойное слово в поисках бита, равного 1. 
Сканирование выполняется от младшего бита (0) к старшему. Если в слове 
не найдено установленных битов, то устанавливается флаг ZF. 
Если установленные биты есть, то номер первого установленного бита 
заносится в указанный в команде регистр. Номером бита считается его 
позиция в слове, отсчитываемая от бита 0. В качестве первого операнда команды bsf 
следует указывать регистр, куда будет помещен результат сканирования, в 
качестве второго - регистр или ячейку памяти со сканируемым словом. 
В команде bsf допустимо использование как 16-битовых, так и 32-битовых операндов 
(но и первый, и второй операнды должны быть одного типа).
*/
           bsf  eax, eax
/*
Команда jmp передает управление в указанную точку того же или другого программного 
сегмента. Адрес возврата не сохраняется. Команда не воздействует на флаги процессора.
*/
           jmp jump_end
           grown_up:    bsf  eax, dword ptr analyzed_number[4]
/*
сложение
*/
           add  eax, 20h
           jump_end:
       }//_asm {
}//static __inline intBit_scan_forward_win32(unsigned __int64 analyzed_number){ 
//	*******************************************************************

//	===================================================================
// ищем последний установленный бит(т.е. бежим слева)
__inline __int32 BITBOARDGENERATOR::Bit_scan_reverse_win32(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
							                               ){ 

  _asm { 
           mov  eax, dword ptr analyzed_number[4]
           test eax, eax
           jz   younger
/*
Команда bsr сканирует слово или двойное слово в поисках бита, равного 1. 
Сканирование выполняется от старшего бита (15 или 31) к младшему. 
Если в слове не найдено установленных битов, то устанавливается флаг ZF. 
Если установленные биты есть, то номер первого установленного бита заносится 
в указанный в команде регистр. Номером бита считается его позиция в слове, 
отсчитываемая от бита 0. В качестве первого операнда команды bsr следует 
указывать регистр, куда будет помещен результат сканирования, в качестве 
второго - регистр или ячейку памяти со сканируемым словом. 
В команде bsr допустимо использование как 16-битовых, так и 32-битовых операндов, 
но и первый, и второй операнды должны быть одного типа (за исключением случая, 
когда второй операнд - константа).
*/
           bsr  eax, eax
           add  eax, 20h
           jmp  jump_end
           younger:    bsr  eax, dword ptr analyzed_number[0]
           jump_end:
       }//_asm { 
}//static __inline int Bit_scan_reverse_win32(unsigned __int64 analyzed_number){ 
//	*******************************************************************