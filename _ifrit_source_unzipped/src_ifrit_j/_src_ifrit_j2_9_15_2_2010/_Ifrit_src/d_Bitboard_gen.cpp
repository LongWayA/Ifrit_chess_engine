#include <fstream>  // TEST
#include <iostream> // TEST
#include <stdlib.h>
using namespace std;

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
   from = first_one(from_mask);
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
        to = first_one(move_conversion_mask);
        move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

//      отмечаем превращение пешки
//      превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,превращение в коня 12
        list_record_os(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0,p_list_surplus_moves);
        list_record_os(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0,p_list_surplus_moves);
        list_record_os(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0,p_list_surplus_moves);
        list_record_os(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0,p_list_surplus_moves);
 
        #if KONTROL_B
          KONTROL_move(&from,&to,&p);
        #endif//KONTROL_B
     }//while (move_conversion_mask != 0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
     // находим координаты взятий с превращением пешки  
     while (captures_conversion_mask != 0){
        to = first_one(captures_conversion_mask);
        captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

//      отмечаем превращение пешки с взятием
//      превращение в ферзь 25,превращение в ладью 24,превращение в слона 23,превращение в коня 22
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);

        #if KONTROL_B
          KONTROL_captures_W(p_bitboard,&from,&to,&p,120);
        #endif//KONTROL_B
     }//while (captures_conversion_mask != 0){

 }else {
     // считаем взятия пешкой
     captures_mask = pawn_capture_white[from] &  p_bitboard->all_black_piece;
     while (captures_mask != 0){
        to = first_one(captures_mask);
        list_capture_white(N_PAWN,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
        captures_mask = captures_mask & (captures_mask - 1);
     }//while (captures_mask != 0){
 }//if ( from / 8 == 6 ){


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// 4 находим взятия на проходе
   if(p_bitboard->taking_passage_yes == 1){
     if( from / 8 == 4 ){
       to = 8 * p_bitboard->taking_passage_y + p_bitboard->taking_passage_x;

       if( ((from - to) == -9) && (from % 8 != 7) ){
          list_record_os(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->black_name_from_coord[to - 8],p_list_surplus_moves);

          #if KONTROL_B
            to = to - 8;
            KONTROL_captures_W(p_bitboard,&from,&to,&p,130);
          #endif//KONTROL_B
       }//if ( ((from - to) == -9) && (from % 8 != 7) ){

       if( ((from - to) == -7) && (from % 8 != 0) ){
          list_record_os(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->black_name_from_coord[to - 8],p_list_surplus_moves);

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
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);
   captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[from]);
   while (captures_mask != 0){
      to = first_one(captures_mask);
      list_capture_white(N_KNIGHT,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
      captures_mask = captures_mask & (captures_mask - 1);
   }//while (captures_mask != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------


// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->white_piece[N_BISHOP];
while(from_mask != 0){
  from = first_one(from_mask);
  from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх - вправо--------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы ладьи---------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[N_ROOK];
while(from_mask != 0){
  from = first_one(from_mask);
  from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------


// ходы ферзя---------------------------------------------------------------------------------
// маска расположения белого ферзя
from_mask = p_bitboard->white_piece[N_QUEEN];
while(from_mask != 0){
  from = first_one(from_mask);
//надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
  from_mask = from_mask & (from_mask - 1);

//смотрим в четырех различных направлениях (по типу ладьи)
//луч вверх------------------------------------------------------------------
//1) находим блокированные биты.
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

  if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate =  first_one(blocking_square_mask);
//   3) находим маску взятий
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
//   4) заполняем список ходов
     if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ферзя--------------------------------------------------------------------------------------


// ходы короля--------------------------------------------------------------------------------
// маска расположения белого короля
from_mask = p_bitboard->white_piece[N_KING];
if(from_mask != 0){
  // находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
  from = first_one(from_mask);
  // находим маску взятий короля
  captures_mask = (p_bitboard->all_black_piece & king_moves_masks[from]);

  while (captures_mask != 0){
     // Заполняем список ходов
     to = first_one(captures_mask);
     list_capture_white(N_KING,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
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
   from = first_one(from_mask);
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
        to = first_one(move_conversion_mask);
        move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

//      отмечаем превращение пешки
//      превращение в ферзь 15,превращение в ладью 14,превращение в слона 13,превращение в коня 12
        list_record_os(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0,p_list_surplus_moves);
        list_record_os(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0,p_list_surplus_moves);
        list_record_os(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0,p_list_surplus_moves);
        list_record_os(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0,p_list_surplus_moves);
 
        #if KONTROL_B
          KONTROL_move(&from,&to,&p);
        #endif//KONTROL_B
     }//while (move_conversion_mask != 0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//   находим координаты взятий с превращением пешки  
//   находим координаты взятий с превращением пешки  
     while (captures_conversion_mask != 0){
        to = first_one(captures_conversion_mask);
        captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

//      отмечаем превращение пешки с взятием
//      превращение в ферзь 25,превращение в ладью 24,превращение в слона 23,превращение в коня 22
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
        list_record_os(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);

        #if KONTROL_B
           KONTROL_captures_B(p_bitboard,&from,&to,&p,120);
        #endif//KONTROL_B
     }//while (captures_conversion_mask != 0){

 }else {
     // считаем взятия пешкой
     captures_mask =  pawn_capture_black[from] &  p_bitboard->all_white_piece;
     while (captures_mask != 0){
        to = first_one(captures_mask);
        list_capture_black(N_PAWN,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
        captures_mask = captures_mask & (captures_mask - 1);
      }//while (captures_mask != 0){
 }// if ( from / 8 == 1 ){


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// 4 находим взятия на проходе
 if(p_bitboard->taking_passage_yes == 1){
   if( from / 8 == 3 ){

     to = 8 * p_bitboard->taking_passage_y + p_bitboard->taking_passage_x;

     if ( ((from - to) == 9) && (from % 8 != 0) ){
       list_record_os(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->white_name_from_coord[to + 8],p_list_surplus_moves);

       #if KONTROL_B
         to = to + 8;
         KONTROL_captures_B(p_bitboard,&from,&to,&p,130);
       #endif//KONTROL_B
     }//if ((from - to) == 9) && (from % 8 != 0)){

     if( ((from - to) == 7) && (from % 8 != 7) ){
       list_record_os(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->white_name_from_coord[to + 8],p_list_surplus_moves);

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
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask - 1);
    captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[from]);

    while(captures_mask != 0){
       to = first_one(captures_mask);
       list_capture_black(N_KNIGHT,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
       captures_mask = captures_mask & (captures_mask - 1);
    }//while (captures != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------


// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->black_piece[N_BISHOP];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
   if(captures_mask != 0) list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы ладьи-----------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_ROOK];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  last_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0) list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------


// ходы ферзя----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_QUEEN];
while(from_mask != 0){
  from = first_one(from_mask);
  from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях
// луч вверх-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях
// луч вверх - вправо---------------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
  }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------


// ходы короля---------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_KING];
if(from_mask != 0){
  from = first_one(from_mask);
  captures_mask = (p_bitboard->all_white_piece & king_moves_masks[from]);

  while (captures_mask != 0){
     to = first_one(captures_mask);
     list_capture_black(N_KING,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
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
  from = first_one(from_mask);
  // находим маску ходов короля(без взятий)
  move_mask = king_moves_masks[from] & (~p_bitboard->all_piece);

  while (move_mask != 0){
     to = first_one(move_mask);
     move_mask = move_mask & (move_mask - 1);
     list_record_move(N_KING,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
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
        if((White_check_square(2,p_bitboard) == 0) && (White_check_square(3,p_bitboard) == 0) && (White_check_square(4,p_bitboard) == 0)){
           list_record_os(N_KING,LONG_CASTLING,p,from,2,0,p_list_surplus_moves);
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
	    if((White_check_square(4,p_bitboard)==0)&&(White_check_square(5,p_bitboard)==0)&&(White_check_square(6,p_bitboard)==0)){
           list_record_os(N_KING,SHORT_CASTLING,p,from,6,0,p_list_surplus_moves);
	    }//if((White_check_square(4,p_bitboard)==0)&&(White_check_square(5,p_bitboard)==0)&&(White_check_square(6,p_bitboard)==0)){
      }//if(( (p_bitboard->all_piece) & 96)){ 
  }//if(p_bitboard->castling_K){ 
}//if(from_mask != 0){
// ходы короля--------------------------------------------------------------------------------


// ходы ферзя----------------------------------------------------------------------------------
// маска расположения белого ферзя
from_mask = p_bitboard->white_piece[N_QUEEN];
while(from_mask != 0){
   from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate =  first_one(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

}//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------



// ходы ладьи----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[N_ROOK];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate =  first_one(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------



// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->white_piece[N_BISHOP];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
from_mask = p_bitboard->white_piece[N_KNIGHT];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);
   move_mask = knigt_moves_masks[from] & (~p_bitboard->all_piece);

  while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[N_PAWN];
while(from_mask != 0){
   from = first_one(from_mask);
 //выделяем одну пешку 
   one_pawn_mask = from_mask & move_masks[from];
   from_mask = from_mask & (from_mask - 1);

// 1 находим простые ходы белых пешек на одно поле вперед
   move_mask = (one_pawn_mask << 8) & (~(p_bitboard->all_piece));
// 2 находим ходы пешек на 2 поля вперед 
// в начале оставляем только пешки стоящие на 2 горизонтали
   move_mask_d = one_pawn_mask & LINE_1;
// отметаем пешки если они чем то заблокированны
   move_mask_d = (move_mask_d <<8) & (~(p_bitboard->all_piece));
// оставшимися пешками делаем ход
   move_mask_d = (move_mask_d << 8) & (~(p_bitboard->all_piece));

// здесь все ходы пешки как на одно так на два поля вперед
   move_mask = (move_mask | move_mask_d) ;

// ходы без превращений
   move_mask = move_mask & ~LINE_7;

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
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
  from = first_one(from_mask);
  move_mask = king_moves_masks[from] & (~(p_bitboard->all_piece));

  while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_KING,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
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
	     if((Black_check_square(58,p_bitboard)==0)&&(Black_check_square(59,p_bitboard)==0)&&(Black_check_square(60,p_bitboard)==0)){
            list_record_os(N_KING,LONG_CASTLING,p,from,58,0,p_list_surplus_moves);			 
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
     if((~(p_bitboard->all_piece) & CASTLING_k )==CASTLING_k){
//      короткая рокировка черных проверяем 0-0 60 61 62
	    if((Black_check_square(60,p_bitboard)==0)&&(Black_check_square(61,p_bitboard)==0)&&(Black_check_square(62,p_bitboard)==0)){
           list_record_os(N_KING,SHORT_CASTLING,p,from,62,0,p_list_surplus_moves);
		}//if((Black_check_square(60,p_bitboard)==0)&&(Black_check_square(61,p_bitboard)==0)&&(Black_check_square(62,p_bitboard)==0)){
	  }//if((~(p_bitboard->all_piece) & CASTLING_k )==CASTLING_k){
	}//if(p_bitboard->castling_k){
}//if(from_mask != 0){
// короля---------------------------------------------------------------------------------


// ходы ферзя----------------------------------------------------------------------------------
// маска расположения ферзя
from_mask = p_bitboard->black_piece[N_QUEEN];
while(from_mask != 0){
   from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate =  first_one(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

}//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------


// ходы ладьи----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_ROOK];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх-----------------------------------------------------------------
// 1) находим блокированные биты.
   blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
//   2) находим координату первого блокированного бита
     blocking_coordinate =  first_one(blocking_square_mask);
	 move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_up_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
	 move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
	 move_mask = rook_left_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------  
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_down_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = rook_right_masks[from];
   }//if(blocking_square_mask != 0){

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
}//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------



// ходы слона---------------------------------------------------------------------------------- 
from_mask = p_bitboard->black_piece[N_BISHOP];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо--------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  first_one(blocking_square_mask);
     move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_up_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
   }else{
     move_mask = bishop_down_right_masks[from];
   }//if(blocking_square_mask != 0){ 

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
//---------------------------------------------------------------------------
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 


// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
from_mask = p_bitboard->black_piece[N_KNIGHT];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask - 1);
   move_mask = knigt_moves_masks[from] & (~p_bitboard->all_piece);

  while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
   }//while (move_mask != 0){
}// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[N_PAWN];
while(from_mask != 0){
   from = first_one(from_mask);
   one_pawn_mask = from_mask & move_masks[from];
   from_mask = from_mask & (from_mask - 1);

// 1 находим простые ходы черных пешек на одно поле вперед
   move_mask = (one_pawn_mask >> 8) & (~(p_bitboard->all_piece));

// 2 находим ходы пешек на 2 поля вперед 
// в начале оставляем только пешки стоящие на 7 горизонтали
   move_mask_d = one_pawn_mask & LINE_6;

// отметаем пешки если они чем то заблокированны
   move_mask_d = (move_mask_d >>8) & (~(p_bitboard->all_piece));
// оставшимися пешками делаем ход
   move_mask_d = (move_mask_d >> 8) & (~(p_bitboard->all_piece));

// здесь все ходы пешки как на одно так на два поля вперед
   move_mask = (move_mask | move_mask_d) ;

// ходы без превращений
   move_mask = move_mask & ~LINE_0;

   while (move_mask != 0){
      to = first_one(move_mask);
      move_mask = move_mask & (move_mask - 1);
      list_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
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
  from = first_one(from_mask);

// луч вверх------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
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
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
        if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// по типу коня--------------------------------------------------------------
 captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[from]);
 while(captures_mask != 0 ){
   if( p_bitboard->black_name_from_coord[first_one(captures_mask)] == N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask ==0){
//---------------------------------------------------------------------------


// короля--------------------------------------------------------------------
  captures_mask = (p_bitboard->all_black_piece & king_moves_masks[from]);
 while(captures_mask != 0){
   if( p_bitboard->black_name_from_coord[first_one(captures_mask)] == N_KING) return N_KING;
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
  from = first_one(from_mask);
 
// смотрим в четырех различных направлениях
// луч вверх------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
       if(captures_mask != 0){
          i = first_one(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
       }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  last_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = first_one(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0){
          i = first_one(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
     }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = first_one(captures_mask);
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
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
         i = first_one(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
         i = first_one(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
   if(captures_mask != 0){
         i = first_one(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
   }//if(captures_mask != 0){
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0){
         i = first_one(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0){
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// По типу коня--------------------------------------------------------------
 captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[from]);
 while(captures_mask != 0){
   if( p_bitboard->white_name_from_coord[first_one(captures_mask)] ==  N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask != 0){
//---------------------------------------------------------------------------

// по типу короля------------------------------------------------------------
 captures_mask = (p_bitboard->all_white_piece & king_moves_masks[from]);
 while(captures_mask != 0){
   if( p_bitboard->white_name_from_coord[first_one(captures_mask)] == N_KING) return N_KING;
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
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч влево-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вправо-----------------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_ROOK) return N_ROOK;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вверх - вправо--------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0) {
        i = first_one(captures_mask);
        if( p_bitboard->black_name_from_coord[i] == N_BISHOP) return N_BISHOP;
        if( p_bitboard->black_name_from_coord[i] == N_QUEEN) return N_QUEEN;
    }//if(captures_mask != 0) {
  }//if(blocking_square_mask != 0){ 
//---------------------------------------------------------------------------

// луч вверх - влево---------------------------------------------------------
  blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[sq];
  if(blocking_square_mask != 0){ 
    blocking_coordinate =  first_one(blocking_square_mask);
    captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0) {
        i = first_one(captures_mask);
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
   if( p_bitboard->black_name_from_coord[first_one(captures_mask)] ==  N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask ==0){
//---------------------------------------------------------------------------

// по типу короля
//---------------------------------------------------------------------------
 captures_mask = (p_bitboard->all_black_piece & king_moves_masks[sq]);
 while(captures_mask != 0){
   if( p_bitboard->black_name_from_coord[first_one(captures_mask)] == N_KING) return N_KING;
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
      blocking_coordinate =  last_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = first_one(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз------------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[sq];
   if(blocking_square_mask != 0){ 
     blocking_coordinate =  last_one(blocking_square_mask);
     captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
     if(captures_mask != 0){
          i = first_one(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
     }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вправо----------------------------------------------------------------
   blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[sq];
   if(blocking_square_mask != 0){ 
      blocking_coordinate =  first_one(blocking_square_mask);
      captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
      if(captures_mask != 0){
          i = first_one(captures_mask);
          if( p_bitboard->white_name_from_coord[i] == N_ROOK) return N_ROOK;
          if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
      }//if(captures_mask != 0){
   }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вниз - влево----------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[sq];
 if(blocking_square_mask != 0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
   if(captures_mask != 0){
         i = first_one(captures_mask);
         if( p_bitboard->white_name_from_coord[i] == N_BISHOP) return N_BISHOP;
         if( p_bitboard->white_name_from_coord[i] == N_QUEEN) return N_QUEEN;
   }//if(captures_mask != 0){
 }//if(blocking_square_mask != 0){
//---------------------------------------------------------------------------

// луч вниз - вправо---------------------------------------------------------
 blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[sq];
 if(blocking_square_mask != 0){ 
    blocking_coordinate =  last_one(blocking_square_mask);
    captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];
    if(captures_mask != 0){
         i = first_one(captures_mask);
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
   if( p_bitboard->white_name_from_coord[first_one(captures_mask)] == N_KNIGHT) return N_KNIGHT;
   captures_mask = captures_mask & (captures_mask - 1);
 }//if(captures_mask != 0){
//---------------------------------------------------------------------------

// по типу короля
//---------------------------------------------------------------------------
 captures_mask = (p_bitboard->all_white_piece & king_moves_masks[sq]);
 while(captures_mask != 0){
   if( p_bitboard->white_name_from_coord[first_one(captures_mask)] == N_KING) return N_KING;
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
inline void BITBOARDGENERATOR::list_capture_white(unsigned __int8 name,// имя ходящей фигуры
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
 
}//static inline void list_capture_white(int name,int type_move,int & from,int & to,int & count,const struct T_p_bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// заполняем список ходов - взятия за черных
// две функции только из за разного контроля списков(строки взятия)
inline void BITBOARDGENERATOR::list_capture_black(unsigned __int8 name,// имя ходящей фигуры
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
}//static inline void list_capture_black(int name,int type_move,int & from,int & to,int & count,const struct T_p_bitboard * p_bitboard,struct T_list_surplus_moves * p_list_surplus_moves){
//	*******************************************************************

//	===================================================================
// заполняем список ходов
inline void BITBOARDGENERATOR::list_record_move(unsigned __int8 name,// имя ходящей фигуры
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
inline void BITBOARDGENERATOR::list_record_os(unsigned __int8 name,// имя ходящей фигуры
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
}//static inline void list_record_os(int name,int d_m,int & p,int & from,int to,int capture,struct T_list_surplus_moves * p_list_surplus_moves){
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
if ((*p_from < 0)||(*p_from > 63)) cout << "ERROR Generator_captures from= "<< *p_from << "\n";
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
if ((*p_from < 0)||(*p_from > 63)) cout << "ERROR Generator_captures from= "<< *p_from << "\n";
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
if ((*p_from < 0)||(*p_from > 63)) cout << "ERROR Generator_move from= "<< *p_from << "\n";
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
__inline __int32 BITBOARDGENERATOR::first_one(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
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
}//static __inline int first_one(unsigned __int64 analyzed_number){ 
//	*******************************************************************

//	===================================================================
// ищем последний установленный бит(т.е. бежим слева)
__inline __int32 BITBOARDGENERATOR::last_one(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
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
}//static __inline int last_one(unsigned __int64 analyzed_number){ 
//	*******************************************************************


// const 

const unsigned __int64 BITBOARDGENERATOR::move_masks[] = {
// в примечании пишем какой бит установлен в 1
1,           // 0 bit т.е. ....0001
2,           // 1 bit т.е. ....0010
4,           // 2 bit т.е. ....0100 
8,           // 3 bit и т.д.
16,          // 4 bit
32,          // 5 bit
64,          // 6 bit
128,         // 7 bit
256,         // 8 bit
512,         // 9 bit
1024,        // 10 bit
2048,        // 11 bit
4096,        // 12 bit
8192,        // 13 bit
16384,       // 14 bit
32768,       // 15 bit
65536,       // 16 bit
131072,      // 17 bit
262144,      // 18 bit
524288,      // 19 bit
1048576,     // 20 bit
2097152,     // 21 bit
4194304,     // 22 bit
8388608,     // 23 bit
16777216,    // 24 bit
33554432,    // 25 bit
67108864,    // 26 bit
134217728,   // 27 bit
268435456,   // 28 bit
536870912,   // 29 bit
1073741824,  // 30 bit
2147483648,  // 31 bit
4294967296,         // 32 bit
8589934592,         // 33 bit
17179869184,        // 34 bit
34359738368,        // 35 bit
68719476736,        // 36 bit
137438953472,       // 37 bit
274877906944,       // 38 bit
549755813888,       // 39 bit
1099511627776,      // 40 bit
2199023255552,      // 41 bit
4398046511104,      // 42 bit
8796093022208,      // 43 bit
17592186044416,     // 44 bit
35184372088832,     // 45 bit
70368744177664,     // 46 bit
140737488355328,    // 47 bit
281474976710656,    // 48 bit
562949953421312,    // 49 bit
1125899906842624,   // 50 bit
2251799813685248,   // 51 bit
4503599627370496,   // 52 bit
9007199254740992,   // 53 bit
18014398509481984,  // 54 bit
36028797018963968,  // 55 bit
72057594037927936,  // 56 bit
144115188075855872, // 57 bit
288230376151711744, // 58 bit
576460752303423488, // 59 bit
1152921504606846976,// 60 bit
2305843009213693952,// 61 bit
4611686018427387904,// 62 bit
9223372036854775808 // 63 bit
};//unsigned __int64 move_masks[]
//-----------------------------------------------------------------------------------

// маски ходов короля
const unsigned __int64 BITBOARDGENERATOR::king_moves_masks[] = {
// в примечании пишем где находится король например from0 - король в A1(0)
//======================
770, // from0
//======================
1797, // from1
//======================
3594, // from2
//======================
7188, // from3
//======================
14376, // from4
//======================
28752, // from5
//======================
57504, // from6
//======================
49216, // from7
//======================
197123, // from8
//======================
460039, // from9
//======================
920078, // from10
//======================
1840156, // from11
//======================
3680312, // from12
//======================
7360624, // from13
//======================
14721248, // from14
//======================
12599488, // from15
//======================
50463488, // from16
//======================
117769984, // from17
//======================
235539968, // from18
//======================
471079936, // from19
//======================
942159872, // from20
//======================
1884319744, // from21
//======================
3768639488, // from22
//======================
3225468928, // from23
//======================
12918652928, // from24
//======================
30149115904, // from25
//======================
60298231808, // from26
//======================
120596463616, // from27
//======================
241192927232, // from28
//======================
482385854464, // from29
//======================
964771708928, // from30
//======================
825720045568, // from31
//======================
3307175149568, // from32
//======================
7718173671424, // from33
//======================
15436347342848, // from34
//======================
30872694685696, // from35
//======================
61745389371392, // from36
//======================
123490778742784, // from37
//======================
246981557485568, // from38
//======================
211384331665408, // from39
//======================
846636838289408, // from40
//======================
1975852459884544, // from41
//======================
3951704919769088, // from42
//======================
7903409839538176, // from43
//======================
15806819679076352, // from44
//======================
31613639358152704, // from45
//======================
63227278716305408, // from46
//======================
54114388906344448, // from47
//======================
216739030602088448, // from48
//======================
505818229730443264, // from49
//======================
1011636459460886528, // from50
//======================
2023272918921773056, // from51
//======================
4046545837843546112, // from52
//======================
8093091675687092224, // from53
//======================
//6962811314519408640, // from54
16186183351374184448, // from54
//======================
//4629911523169402880, // from55
13853283560024178688, // from55
//======================
144959613005987840, // from56
//======================
362258295026614272, // from57
//======================
724516590053228544, // from58
//======================
1449033180106457088, // from59
//======================
2898066360212914176, // from60
//======================
5796132720425828352, // from61
//======================
//2368893403996880896, // from62
11592265440851656704, // from62
//======================
4665729213955833856, // from63
};//unsigned __int64 king_moves_masks[]
//-----------------------------------------------------------------------------------

// маски ходов коня
const unsigned __int64 BITBOARDGENERATOR::knigt_moves_masks[] = {
//======================
132096, // from0
//======================
329728, // from1
//======================
659712, // from2
//======================
1319424, // from3
//======================
2638848, // from4
//======================
5277696, // from5
//======================
10489856, // from6
//======================
4202496, // from7
//======================
33816580, // from8
//======================
84410376, // from9
//======================
168886289, // from10
//======================
337772578, // from11
//======================
675545156, // from12
//======================
1351090312, // from13
//======================
2685403152, // from14
//======================
1075839008, // from15
//======================
8657044482, // from16
//======================
21609056261, // from17
//======================
43234889994, // from18
//======================
86469779988, // from19
//======================
172939559976, // from20
//======================
345879119952, // from21
//======================
687463207072, // from22
//======================
275414786112, // from23
//======================
2216203387392, // from24
//======================
5531918402816, // from25
//======================
11068131838464, // from26
//======================
22136263676928, // from27
//======================
44272527353856, // from28
//======================
88545054707712, // from29
//======================
175990581010432, // from30
//======================
70506185244672, // from31
//======================
567348067172352, // from32
//======================
1416171111120896, // from33
//======================
2833441750646784, // from34
//======================
5666883501293568, // from35
//======================
11333767002587136, // from36
//======================
22667534005174272, // from37
//======================
45053588738670592, // from38
//======================
18049583422636032, // from39
//======================
145241105196122112, // from40
//======================
362539804446949376, // from41
//======================
725361088165576704, // from42
//======================
1450722176331153408, // from43
//======================
2901444352662306816, // from44
//======================
5802888705324613632, // from45
//======================
11533718717099671552, // from46
//======================
4620693356194824192, // from47
//======================
288234782788157440, // from48
//======================
576469569871282176, // from49
//======================
1224997833292120064, // from50
//======================
2449995666584240128, // from51
//======================
4899991333168480256, // from52
//======================
9799982666336960512, // from53
//======================
1152939783987658752, // from54
//======================
2305878468463689728, // from55
//======================
1128098930098176, // from56
//======================
2257297371824128, // from57
//======================
4796069720358912, // from58
//======================
9592139440717824, // from59
//======================
19184278881435648, // from60
//======================
38368557762871296, // from61
//======================
4679521487814656, // from62
//======================
9077567998918656 // from63
};//unsigned __int64 knigt_moves_masks[]
//-----------------------------------------------------------------------------------

// маска ладья ходы вверх
const unsigned __int64 BITBOARDGENERATOR::rook_up_masks[] = {
//======================
72340172838076672, // from0
//======================
144680345676153344, // from1
//======================
289360691352306688, // from2
//======================
578721382704613376, // from3
//======================
1157442765409226752, // from4
//======================
2314885530818453504, // from5
//======================
4629771061636907008, // from6
//======================
9259542123273814016, // from7
//======================
72340172838076416, // from8
//======================
144680345676152832, // from9
//======================
289360691352305664, // from10
//======================
578721382704611328, // from11
//======================
1157442765409222656, // from12
//======================
2314885530818445312, // from13
//======================
4629771061636890624, // from14
//======================
9259542123273781248, // from15
//======================
72340172838010880, // from16
//======================
144680345676021760, // from17
//======================
289360691352043520, // from18
//======================
578721382704087040, // from19
//======================
1157442765408174080, // from20
//======================
2314885530816348160, // from21
//======================
4629771061632696320, // from22
//======================
9259542123265392640, // from23
//======================
72340172821233664, // from24
//======================
144680345642467328, // from25
//======================
289360691284934656, // from26
//======================
578721382569869312, // from27
//======================
1157442765139738624, // from28
//======================
2314885530279477248, // from29
//======================
4629771060558954496, // from30
//======================
9259542121117908992, // from31
//======================
72340168526266368, // from32
//======================
144680337052532736, // from33
//======================
289360674105065472, // from34
//======================
578721348210130944, // from35
//======================
1157442696420261888, // from36
//======================
2314885392840523776, // from37
//======================
4629770785681047552, // from38
//======================
9259541571362095104, // from39
//======================
72339069014638592, // from40
//======================
144678138029277184, // from41
//======================
289356276058554368, // from42
//======================
578712552117108736, // from43
//======================
1157425104234217472, // from44
//======================
2314850208468434944, // from45
//======================
4629700416936869888, // from46
//======================
9259400833873739776, // from47
//======================
72057594037927936, // from48
//======================
144115188075855872, // from49
//======================
288230376151711744, // from50
//======================
576460752303423488, // from51
//======================
1152921504606846976, // from52
//======================
2305843009213693952, // from53
//======================
4611686018427387904, // from54
//======================
9223372036854775808, // from55
//======================
0, // from56
//======================
0, // from57
//======================
0, // from58
//======================
0, // from59
//======================
0, // from60
//======================
0, // from61
//======================
0, // from62
//======================
0, // from63
};//unsigned __int64 rook_up_masks[]
//-----------------------------------------------------------------------------------

// маска ладья ходы вниз
const unsigned __int64 BITBOARDGENERATOR::rook_down_masks[] = {
//======================
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
//======================
1, // from8
//======================
2, // from9
//======================
4, // from10
//======================
8, // from11
//======================
16, // from12
//======================
32, // from13
//======================
64, // from14
//======================
128, // from15
//======================
257, // from16
//======================
514, // from17
//======================
1028, // from18
//======================
2056, // from19
//======================
4112, // from20
//======================
8224, // from21
//======================
16448, // from22
//======================
32896, // from23
//======================
65793, // from24
//======================
131586, // from25
//======================
263172, // from26
//======================
526344, // from27
//======================
1052688, // from28
//======================
2105376, // from29
//======================
4210752, // from30
//======================
8421504, // from31
//======================
16843009, // from32
//======================
33686018, // from33
//======================
67372036, // from34
//======================
134744072, // from35
//======================
269488144, // from36
//======================
538976288, // from37
//======================
1077952576, // from38
//======================
2155905152, // from39
//======================
4311810305, // from40
//======================
8623620610, // from41
//======================
17247241220, // from42
//======================
34494482440, // from43
//======================
68988964880, // from44
//======================
137977929760, // from45
//======================
275955859520, // from46
//======================
551911719040, // from47
//======================
1103823438081, // from48
//======================
2207646876162, // from49
//======================
4415293752324, // from50
//======================
8830587504648, // from51
//======================
17661175009296, // from52
//======================
35322350018592, // from53
//======================
70644700037184, // from54
//======================
141289400074368, // from55
//======================
282578800148737, // from56
//======================
565157600297474, // from57
//======================
1130315200594948, // from58
//======================
2260630401189896, // from59
//======================
4521260802379792, // from60
//======================
9042521604759584, // from61
//======================
18085043209519168, // from62
//======================
36170086419038336, // from63
};//unsigned __int64 rook_down_masks[]
//-----------------------------------------------------------------------------------

// маска ладья ходы влево
const unsigned __int64 BITBOARDGENERATOR::rook_left_masks[] = {
//======================
0, // from0
//======================
1, // from1
//======================
3, // from2
//======================
7, // from3
//======================
15, // from4
//======================
31, // from5
//======================
63, // from6
//======================
127, // from7
//======================
0, // from8
//======================
256, // from9
//======================
768, // from10
//======================
1792, // from11
//======================
3840, // from12
//======================
7936, // from13
//======================
16128, // from14
//======================
32512, // from15
//======================
0, // from16
//======================
65536, // from17
//======================
196608, // from18
//======================
458752, // from19
//======================
983040, // from20
//======================
2031616, // from21
//======================
4128768, // from22
//======================
8323072, // from23
//======================
0, // from24
//======================
16777216, // from25
//======================
50331648, // from26
//======================
117440512, // from27
//======================
251658240, // from28
//======================
520093696, // from29
//======================
1056964608, // from30
//======================
2130706432, // from31
//======================
0, // from32
//======================
4294967296, // from33
//======================
12884901888, // from34
//======================
30064771072, // from35
//======================
64424509440, // from36
//======================
133143986176, // from37
//======================
270582939648, // from38
//======================
545460846592, // from39
//======================
0, // from40
//======================
1099511627776, // from41
//======================
3298534883328, // from42
//======================
7696581394432, // from43
//======================
16492674416640, // from44
//======================
34084860461056, // from45
//======================
69269232549888, // from46
//======================
139637976727552, // from47
//======================
0, // from48
//======================
281474976710656, // from49
//======================
844424930131968, // from50
//======================
1970324836974592, // from51
//======================
4222124650659840, // from52
//======================
8725724278030336, // from53
//======================
17732923532771328, // from54
//======================
35747322042253312, // from55
//======================
0, // from56
//======================
72057594037927936, // from57
//======================
216172782113783808, // from58
//======================
504403158265495552, // from59
//======================
1080863910568919040, // from60
//======================
2233785415175766016, // from61
//======================
4539628424389459968, // from62
//======================
9151314442816847872, // from63
};//unsigned __int64 rook_left_masks[]
//-----------------------------------------------------------------------------------

// маска ладья ходы вправо
const unsigned __int64 BITBOARDGENERATOR::rook_right_masks[] = {
//======================
254, // from0
//======================
252, // from1
//======================
248, // from2
//======================
240, // from3
//======================
224, // from4
//======================
192, // from5
//======================
128, // from6
//======================
0, // from7
//======================
65024, // from8
//======================
64512, // from9
//======================
63488, // from10
//======================
61440, // from11
//======================
57344, // from12
//======================
49152, // from13
//======================
32768, // from14
//======================
0, // from15
//======================
16646144, // from16
//======================
16515072, // from17
//======================
16252928, // from18
//======================
15728640, // from19
//======================
14680064, // from20
//======================
12582912, // from21
//======================
8388608, // from22
//======================
0, // from23
//======================
4261412864, // from24
//======================
4227858432, // from25
//======================
4160749568, // from26
//======================
4026531840, // from27
//======================
3758096384, // from28
//======================
3221225472, // from29
//======================
2147483648, // from30
//======================
0, // from31
//======================
1090921693184, // from32
//======================
1082331758592, // from33
//======================
1065151889408, // from34
//======================
1030792151040, // from35
//======================
962072674304, // from36
//======================
824633720832, // from37
//======================
549755813888, // from38
//======================
0, // from39
//======================
279275953455104, // from40
//======================
277076930199552, // from41
//======================
272678883688448, // from42
//======================
263882790666240, // from43
//======================
246290604621824, // from44
//======================
211106232532992, // from45
//======================
140737488355328, // from46
//======================
0, // from47
//======================
71494644084506624, // from48
//======================
70931694131085312, // from49
//======================
69805794224242688, // from50
//======================
67553994410557440, // from51
//======================
63050394783186944, // from52
//======================
54043195528445952, // from53
//======================
36028797018963968, // from54
//======================
0, // from55
//======================
//9079256848778919936, // from56
18302628885633695744, // from56
//======================
//8935141660703064064, // from57
18158513697557839872, // from57
//======================
//8646911284551352320, // from58
17870283321406128128, // from58
//======================
//8070450532247928832, // from59
17293822569102704640, // from59
//======================
//6917529027641081856, // from60
16140901064495857664, // from60
//======================
//4611686018427387904, // from61
13835058055282163712, // from61
//======================
//0, // from62
9223372036854775808, // from62
//======================
0 // from63
};//unsigned __int64 rook_right_masks[]
//-----------------------------------------------------------------------------------

// маска слон ходы вверх-вправо
const unsigned __int64 BITBOARDGENERATOR::bishop_up_right_masks[] = {
//======================
//18049651735527936, // from0
9241421688590303744, // from0
//======================
36099303471055872, // from1
//======================
141012904183808, // from2
//======================
550831656960, // from3
//======================
2151686144, // from4
//======================
8404992, // from5
//======================
32768, // from6
//======================
0, // from7
//======================
4620710844295151616, // from8
//======================
//18049651735527424, // from9
9241421688590303232, // from9
//======================
36099303471054848, // from10
//======================
141012904181760, // from11
//======================
550831652864, // from12
//======================
2151677952, // from13
//======================
8388608, // from14
//======================
0, // from15
//======================
2310355422147510272, // from16
//======================
4620710844295020544, // from17
//======================
//18049651735265280, // from18
9241421688590041088, // from18
//======================
36099303470530560, // from19
//======================
141012903133184, // from20
//======================
550829555712, // from21
//======================
2147483648, // from22
//======================
0, // from23
//======================
1155177711056977920, // from24
//======================
2310355422113955840, // from25
//======================
4620710844227911680, // from26
//======================
//18049651601047552, // from27
9241421688455823360, // from27
//======================
36099303202095104, // from28
//======================
141012366262272, // from29
//======================
549755813888, // from30
//======================
0, // from31
//======================
577588851233521664, // from32
//======================
1155177702467043328, // from33
//======================
2310355404934086656, // from34
//======================
4620710809868173312, // from35
//======================
//18049582881570816, // from36
9241421619736346624, // from36
//======================
36099165763141632, // from37
//======================
140737488355328, // from38
//======================
0, // from39
//======================
288793326105133056, // from40
//======================
577586652210266112, // from41
//======================
1155173304420532224, // from42
//======================
2310346608841064448, // from43
//======================
4620693217682128896, // from44
//======================
//18014398509481984, // from45
9241386435364257792, // from45
//======================
36028797018963968, // from46
//======================
0, // from47
//======================
144115188075855872, // from48
//======================
288230376151711744, // from49
//======================
576460752303423488, // from50
//======================
1152921504606846976, // from51
//======================
2305843009213693952, // from52
//======================
4611686018427387904, // from53
//======================
9223372036854775808, // from54
//======================
0, // from55
//======================
0, // from56
//======================
0, // from57
//======================
0, // from58
//======================
0, // from59
//======================
0, // from60
//======================
0, // from61
//======================
0, // from62
//======================
0 // from63
};//unsigned __int64 bishop_up_right_masks[]
//-----------------------------------------------------------------------------------

// маска слон ходы вверх-влево
const unsigned __int64 BITBOARDGENERATOR::bishop_up_left_masks[] = {
//======================
0, // from0
//======================
256, // from1
//======================
66048, // from2
//======================
16909312, // from3
//======================
4328785920, // from4
//======================
1108169199616, // from5
//======================
283691315109888, // from6
//======================
72624976668147712, // from7
//======================
0, // from8
//======================
65536, // from9
//======================
16908288, // from10
//======================
4328783872, // from11
//======================
1108169195520, // from12
//======================
283691315101696, // from13
//======================
72624976668131328, // from14
//======================
145249953336262656, // from15
//======================
0, // from16
//======================
16777216, // from17
//======================
4328521728, // from18
//======================
1108168671232, // from19
//======================
283691314053120, // from20
//======================
72624976666034176, // from21
//======================
145249953332068352, // from22
//======================
290499906664136704, // from23
//======================
0, // from24
//======================
4294967296, // from25
//======================
1108101562368, // from26
//======================
283691179835392, // from27
//======================
72624976397598720, // from28
//======================
145249952795197440, // from29
//======================
290499905590394880, // from30
//======================
580999811180789760, // from31
//======================
0, // from32
//======================
1099511627776, // from33
//======================
283673999966208, // from34
//======================
72624942037860352, // from35
//======================
145249884075720704, // from36
//======================
290499768151441408, // from37
//======================
580999536302882816, // from38
//======================
1161999072605765632, // from39
//======================
0, // from40
//======================
281474976710656, // from41
//======================
72620543991349248, // from42
//======================
145241087982698496, // from43
//======================
290482175965396992, // from44
//======================
580964351930793984, // from45
//======================
1161928703861587968, // from46
//======================
2323857407723175936, // from47
//======================
0, // from48
//======================
72057594037927936, // from49
//======================
144115188075855872, // from50
//======================
288230376151711744, // from51
//======================
576460752303423488, // from52
//======================
1152921504606846976, // from53
//======================
2305843009213693952, // from54
//======================
4611686018427387904, // from55
//======================
0, // from56
//======================
0, // from57
//======================
0, // from58
//======================
0, // from59
//======================
0, // from60
//======================
0, // from61
//======================
0, // from62
//======================
0, // from63
};//unsigned __int64 bishop_up_left_masks[]
//-----------------------------------------------------------------------------------

// маска слон ходы вниз-вправо
const unsigned __int64 BITBOARDGENERATOR::bishop_down_right_masks[] = {
//======================
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
//======================
2, // from8
//======================
4, // from9
//======================
8, // from10
//======================
16, // from11
//======================
32, // from12
//======================
64, // from13
//======================
128, // from14
//======================
0, // from15
//======================
516, // from16
//======================
1032, // from17
//======================
2064, // from18
//======================
4128, // from19
//======================
8256, // from20
//======================
16512, // from21
//======================
32768, // from22
//======================
0, // from23
//======================
132104, // from24
//======================
264208, // from25
//======================
528416, // from26
//======================
1056832, // from27
//======================
2113664, // from28
//======================
4227072, // from29
//======================
8388608, // from30
//======================
0, // from31
//======================
33818640, // from32
//======================
67637280, // from33
//======================
135274560, // from34
//======================
270549120, // from35
//======================
541097984, // from36
//======================
1082130432, // from37
//======================
2147483648, // from38
//======================
0, // from39
//======================
8657571872, // from40
//======================
17315143744, // from41
//======================
34630287488, // from42
//======================
69260574720, // from43
//======================
138521083904, // from44
//======================
277025390592, // from45
//======================
549755813888, // from46
//======================
0, // from47
//======================
2216338399296, // from48
//======================
4432676798592, // from49
//======================
8865353596928, // from50
//======================
17730707128320, // from51
//======================
35461397479424, // from52
//======================
70918499991552, // from53
//======================
140737488355328, // from54
//======================
0, // from55
//======================
567382630219904, // from56
//======================
1134765260439552, // from57
//======================
2269530520813568, // from58
//======================
4539061024849920, // from59
//======================
9078117754732544, // from60
//======================
18155135997837312, // from61
//======================
36028797018963968, // from62
//======================
0, // from63
};//unsigned __int64 bishop_down_right_masks[]
//-----------------------------------------------------------------------------------

// маска слон ходы вниз-влево
const unsigned __int64 BITBOARDGENERATOR::bishop_down_left_masks[] = {
//======================
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
//======================
0, // from8
//======================
1, // from9
//======================
2, // from10
//======================
4, // from11
//======================
8, // from12
//======================
16, // from13
//======================
32, // from14
//======================
64, // from15
//======================
0, // from16
//======================
256, // from17
//======================
513, // from18
//======================
1026, // from19
//======================
2052, // from20
//======================
4104, // from21
//======================
8208, // from22
//======================
16416, // from23
//======================
0, // from24
//======================
65536, // from25
//======================
131328, // from26
//======================
262657, // from27
//======================
525314, // from28
//======================
1050628, // from29
//======================
2101256, // from30
//======================
4202512, // from31
//======================
0, // from32
//======================
16777216, // from33
//======================
33619968, // from34
//======================
67240192, // from35
//======================
134480385, // from36
//======================
268960770, // from37
//======================
537921540, // from38
//======================
1075843080, // from39
//======================
0, // from40
//======================
4294967296, // from41
//======================
8606711808, // from42
//======================
17213489152, // from43
//======================
34426978560, // from44
//======================
68853957121, // from45
//======================
137707914242, // from46
//======================
275415828484, // from47
//======================
0, // from48
//======================
1099511627776, // from49
//======================
2203318222848, // from50
//======================
4406653222912, // from51
//======================
8813306511360, // from52
//======================
17626613022976, // from53
//======================
35253226045953, // from54
//======================
70506452091906, // from55
//======================
0, // from56
//======================
281474976710656, // from57
//======================
564049465049088, // from58
//======================
1128103225065472, // from59
//======================
2256206466908160, // from60
//======================
4512412933881856, // from61
//======================
9024825867763968, // from62
//======================
18049651735527937, // from63
};//unsigned __int64 bishop_down_left_masks[]
//-----------------------------------------------------------------------------------

// маска взятий белой пешки
const unsigned __int64 BITBOARDGENERATOR::pawn_capture_white[] = {
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
// вышестоящие нули для обеспечения порядка обращения к данным массива
131072,//from=  8
327680,// from=  9
655360,// from=  10
1310720,// from=  11
2621440,// from=  12
5242880,// from=  13
10485760,// from=  14
4194304,// from=  15
33554432,// from=  16
83886080,// from=  17
167772160,// from=  18
335544320,// from=  19
671088640,// from=  20
1342177280,// from=  21
2684354560,// from=  22
1073741824,// from=  23
8589934592,// from=  24
21474836480,// from=  25
42949672960,// from=  26
85899345920,// from=  27
171798691840,// from=  28
343597383680,// from=  29
687194767360,// from=  30
274877906944,// from=  31
2199023255552,// from=  32
5497558138880,// from=  33
10995116277760,// from=  34
21990232555520,// from=  35
43980465111040,// from=  36
87960930222080,// from=  37
175921860444160,// from=  38
70368744177664,// from=  39
562949953421312,// from=  40
1407374883553280,// from=  41
2814749767106560,// from=  42
5629499534213120,// from=  43
11258999068426240,// from=  44
22517998136852480,// from=  45
45035996273704960,// from=  46
18014398509481984,// from=  47
144115188075855872,// from=  48
360287970189639680,// from=  49
720575940379279360,// from=  50
1441151880758558720,// from=  51
2882303761517117440,// from=  52
5764607523034234880,// from=  53
11529215046068469760,// from=  54
4611686018427387904// from=  55
};//const unsigned __int64 pawn_capture_white[]
//-----------------------------------------------------------------------------------

// маска взятий черной пешки
const unsigned __int64 BITBOARDGENERATOR::pawn_capture_black[] = {
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
// вышестоящие нули для обеспечения порядка обращения к данным массива
2,// from=  8
5,// from=  9
10,// from=  10
20,// from=  11
40,// from=  12
80,// from=  13
160,// from=  14
64,// from=  15
512,// from=  16
1280,// from=  17
2560,// from=  18
5120,// from=  19
10240,// from=  20
20480,// from=  21
40960,// from=  22
16384,// from=  23
131072,// from=  24
327680,// from=  25
655360,// from=  26
1310720,// from=  27
2621440,// from=  28
5242880,// from=  29
10485760,// from=  30
4194304,// from=  31
33554432,// from=  32
83886080,// from=  33
167772160,// from=  34
335544320,// from=  35
671088640,// from=  36
1342177280,// from=  37
2684354560,// from=  38
1073741824,// from=  39
8589934592,// from=  40
21474836480,// from=  41
42949672960,// from=  42
85899345920,// from=  43
171798691840,// from=  44
343597383680,// from=  45
687194767360,// from=  46
274877906944,// from=  47
2199023255552,// from=  48
5497558138880,// from=  49
10995116277760,// from=  50
21990232555520,// from=  51
43980465111040,// from=  52
87960930222080,// from=  53
175921860444160,// from=  54
70368744177664,// from=  55

};//const unsigned __int64 pawn_capture_black[]
//-----------------------------------------------------------------------------------
