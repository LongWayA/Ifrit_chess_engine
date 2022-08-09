#include <iostream.h> // TEST

#include "e_Move.h"
//-----------------------------------------------------------------------------------
/*++
 * задаем функции по реализации ходов
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 19.04.2008
 *
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
 * все что здесь проходит можно проверить в модуле d_Loop
 *
*/
//---------------------------------------------------------------------------

//	===================================================================
// реализуем ходы вперед для белых
// int i номер хода по списку,T_list_surplus_moves список возможных ходов,T_board_list_816 позиция,T_list_realizable_moves список реальных ходов 
void Onward_moves_white( int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);

// запоминаем флаг взятия на проходе
list_surplus_moves->return_taking_passage[depth]= bitboard->taking_passage_yes;
// запрещаем взятие на проходе 
bitboard->taking_passage_yes =0;

// является ли ход ходом 0, взятием 1, взятием на проходе 2, рокировкой 3,
// превращение в ладью 4,превращение в коня 5,превращение в слона 6,превращение в ферзь 7,
// превращение с взятием в ладью 8,превращение с взятием в коня 9,
// превращение с взятием в слона 10,превращение с взятием в ферзь 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){
   case 1 :
//1 ход на пустое поле-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

//--------------------------------------------------------------------------------
// особо отмечаем случаи влияющие на рокировку и взятие на проходе
// мы тут смотрим какой фигурой ходили и если это пешка то проверяем не нужно
// ли отметить взятие на проходе если это король то отменяем его рокировки
// а если ладья то отменяем рокировку с ней
// пешка - флаг взятия на проходе 
//-------------------------------------------------------
  if(bitboard->white_name_from_coord[f_p] == 1){
	 if(f_p - i_p == 16){
		  bitboard->taking_passage_yes =1;
          bitboard->taking_passage_x =(i_p + 8) % 8;
          bitboard->taking_passage_y =(i_p + 8) / 8;
	 }//if(f_p - i_p == 16){
//-------------------------------------------------------
// король - флаг рокировки
//-------------------------------------------------------  
  } else if(bitboard->white_name_from_coord[f_p] == 6){
// запоминаем флаги рокировок
		 list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         bitboard->castling_K =0;
	     bitboard->castling_Q =0;	   
//-------------------------------------------------------  
// ладья - флаг рокировки  
 } else if(bitboard->white_name_from_coord[f_p] == 4){
  
	if(i_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(i_p == 0){
	if(i_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(i_p == 7){
 }//if(bitboard->black_name_from_coord[f_p] ==1){
// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 взятие----------------------------------------------------------------------------------
{
// убираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// убираем взятую фигуру из маски всех черных фигур
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// убираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// кроме того убираем взятую фигуру из битового списка фигур
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// зануляем взятую фигуру
    bitboard->black_name_from_coord[f_p] = 0;
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем берущую фигуру в маске всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем берущую фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// король - флаг рокировки
//-------------------------------------------------------  
if(bitboard->white_name_from_coord[f_p] == 6){
// запоминаем флаги рокировок
		 list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         bitboard->castling_K =0;
	     bitboard->castling_Q =0;		
//-------------------------------------------------------  
// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
  } else if(bitboard->white_name_from_coord[f_p] == 4){	
	if(i_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(i_p == 0){
	if(i_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(i_p == 7){
   }//if(bitboard->black_name_from_coord[f_p] == 4){	

// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
// 3 взятие на проходе-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;

// убираем взятую фигуру из маски всех черных фигур
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
// кроме того убираем взятую фигуру из битового списка фигур
    bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] = bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];
// зануляем взятую фигуру
    bitboard->black_name_from_coord[(f_p - 8)] = 0;

//рисуем фигуру на новом месте------------------
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
//рокировка 0-0-0
{
//стираем фигуру на старом месте----------------- 
 // ход королем
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;
// ход ладьей
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[0];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[0]] = bitboard->white_piece[bitboard->white_name_from_coord[0]] ^ move_masks_m[0];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[3] = bitboard->white_name_from_coord[0];
	bitboard->white_name_from_coord[0] = 0;

//рисуем фигуру на новом месте------------------
// ход королем
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ход ладьей
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[3];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[3]] = bitboard->white_piece[bitboard->white_name_from_coord[3]] ^ move_masks_m[3];

// король - флаг рокировки
//-------------------------------------------------------  
// запоминаем флаги рокировок
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
	     bitboard->castling_Q =0;
         bitboard->castling_K =0;
//-------------------------------------------------------  
// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;

   case 4 :
//рокировка 0-0
{
//стираем фигуру на старом месте----------------- 
 // ход королем
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = bitboard->white_name_from_coord[i_p];
	bitboard->white_name_from_coord[i_p] = 0;
// ход ладьей
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[7];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[7]] = bitboard->white_piece[bitboard->white_name_from_coord[7]] ^ move_masks_m[7];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[5] = bitboard->white_name_from_coord[7];
	bitboard->white_name_from_coord[7] = 0;

//рисуем фигуру на новом месте------------------
// ход королем
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// ход ладьей
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[5];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[5]] = bitboard->white_piece[bitboard->white_name_from_coord[5]] ^ move_masks_m[5];

// король - флаг рокировки
//-------------------------------------------------------  
// запоминаем флаги рокировок
         list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
         list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
	     bitboard->castling_Q =0;
         bitboard->castling_K =0;
//-------------------------------------------------------  
// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
//превращение в ферзя-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = 5;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 14 :
//превращение в ладью-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = 4;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 13 :
//превращение в слона-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = 3;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 12 :
//превращение в слона-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
// стираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// переносим имя фигуры в новую координату и зануляем его по старой координате
	bitboard->white_name_from_coord[f_p] = 2;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем фигуру в маске всех белых фигур
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
// взятие с превращением в ферзя----------------------------------------------------------------------------------
{

// убираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// убираем взятую фигуру из маски всех черных фигур
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// убираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// кроме того убираем взятую фигуру из битового списка фигур
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// зануляем взятую фигуру
    bitboard->black_name_from_coord[f_p] = 0;
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[f_p] = 5;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем берущую фигуру в маске всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем берущую фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 24 :
// взятие с превращением в ладью----------------------------------------------------------------------------------
{

// убираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// убираем взятую фигуру из маски всех черных фигур
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// убираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// кроме того убираем взятую фигуру из битового списка фигур
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// зануляем взятую фигуру
    bitboard->black_name_from_coord[f_p] = 0;
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[f_p] = 4;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем берущую фигуру в маске всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем берущую фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 23 :
// взятие с превращением слона----------------------------------------------------------------------------------
{
// убираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// убираем взятую фигуру из маски всех черных фигур
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// убираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// кроме того убираем взятую фигуру из битового списка фигур
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// зануляем взятую фигуру
    bitboard->black_name_from_coord[f_p] = 0;
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[f_p] = 3;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем берущую фигуру в маске всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем берущую фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 22 :
// взятие с превращением  в коня----------------------------------------------------------------------------------
{
// убираем фигуру из маски всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// убираем взятую фигуру из маски всех черных фигур
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];

// убираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// кроме того убираем взятую фигуру из битового списка фигур
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// зануляем взятую фигуру
    bitboard->black_name_from_coord[f_p] = 0;
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[f_p] = 2;
	bitboard->white_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
// рисуем берущую фигуру в маске всех белых фигур
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// рисуем берущую фигуру в битовом списке фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
//-------------------------------------------------------  

// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f  == 4){
	if(f_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(f_p == 56){
	if(f_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


   default:;//ход не распознан
   
 }// switch (name){
}//void Onward_moves_816( int i,T_list_surplus_moves * list_surplus_moves,T_board_list_816 * board_list_816){ 
//	*******************************************************************

//	===================================================================
// реализуем ходы вперед для черных
// int i номер хода по списку,T_list_surplus_moves список возможных ходов,T_board_list_816 позиция,T_list_realizable_moves список реальных ходов 
void Onward_moves_black( int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);


// запоминаем флаг взятия на проходе
list_surplus_moves->return_taking_passage[depth]= bitboard->taking_passage_yes;
// запрещаем взятие на проходе 
bitboard->taking_passage_yes =0;

// является ли ход ходом 0, взятием 1, взятием на проходе 2, рокировкой 3,
// превращение в ладью 4,превращение в коня 5,превращение в слона 6,превращение в ферзь 7,
// превращение с взятием в ладью 8,превращение с взятием в коня 9,
// превращение с взятием в слона 10,превращение с взятием в ферзь 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){
   case 1 :
//1 ход на пустое поле-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// пешка - флаг взятия на проходе 
//-------------------------------------------------------
  if(bitboard->black_name_from_coord[f_p] ==1){
	 if(f_p - i_p == -16){
		  bitboard->taking_passage_yes =1;
          bitboard->taking_passage_x =(i_p - 8) % 8;
          bitboard->taking_passage_y =(i_p - 8) / 8;
	 }//if(f_p - i_p == -16){
//-------------------------------------------------------
// король - флаг рокировки
//-------------------------------------------------------  
  } else if(bitboard->black_name_from_coord[f_p] == 6){
		 list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         bitboard->castling_k =0;
	     bitboard->castling_q =0;
//-------------------------------------------------------  
// ладья - флаг рокировки  
 } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(i_p == 56){
	if(i_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(i_p == 63){
     
 }//if(bitboard->black_name_from_coord[f_p] ==1){

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 взятие----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// король - флаг рокировки
//-------------------------------------------------------  
  if(bitboard->black_name_from_coord[f_p] == 6){
		 list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         bitboard->castling_k =0;
	     bitboard->castling_q =0;
//-------------------------------------------------------  
// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
   } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
		bitboard->castling_q =0;
	}//if(i_p == 56){
	if(i_p == 63){
        list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
		bitboard->castling_k =0;
	}//if(i_p == 63){
     
 }//if(bitboard->black_name_from_coord[f_p] == 4){	
// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f == 4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
// 3 взятие на проходе-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];
    bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] = bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];
    bitboard->white_name_from_coord[(f_p + 8)] = 0;


//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
//рокировка 0-0-0
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[56];
    bitboard->black_piece[bitboard->black_name_from_coord[56]] = bitboard->black_piece[bitboard->black_name_from_coord[56]] ^ move_masks_m[56];
	bitboard->black_name_from_coord[59] = bitboard->black_name_from_coord[56];
	bitboard->black_name_from_coord[56] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[59];
    bitboard->black_piece[bitboard->black_name_from_coord[59]] = bitboard->black_piece[bitboard->black_name_from_coord[59]] ^ move_masks_m[59];


// король - флаг рокировки
//-------------------------------------------------------  
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
	     bitboard->castling_q =0;
         bitboard->castling_k =0;
//-------------------------------------------------------  

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;

   case 4 :
//рокировка 0-0
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = bitboard->black_name_from_coord[i_p];
	bitboard->black_name_from_coord[i_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[63];
    bitboard->black_piece[bitboard->black_name_from_coord[63]] = bitboard->black_piece[bitboard->black_name_from_coord[63]] ^ move_masks_m[63];
	bitboard->black_name_from_coord[61] = bitboard->black_name_from_coord[63];
	bitboard->black_name_from_coord[63] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[61];
    bitboard->black_piece[bitboard->black_name_from_coord[61]] = bitboard->black_piece[bitboard->black_name_from_coord[61]] ^ move_masks_m[61];


// король - флаг рокировки
//-------------------------------------------------------  
         list_surplus_moves->return_castling_q[depth]= bitboard->castling_q;
         list_surplus_moves->return_castling_k[depth]= bitboard->castling_k;
	     bitboard->castling_q =0;
         bitboard->castling_k =0;
//-------------------------------------------------------  

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
 break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
//превращение в ферзя-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 5;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 14 :
//превращение в ладью-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 4;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 13 :
//превращение в слона-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 3;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 12 :
//превращение в слона-----------------------------------------------------------------------
{
//стираем фигуру на старом месте----------------- 
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	bitboard->black_name_from_coord[f_p] = 2;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

// корректируем маску всех фигур
    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
// взятие с превращением в ферзя----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 5;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f ==4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 24 :
// взятие с превращением в ладью----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 4;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f == 4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 23 :
// взятие с превращением слона----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 3;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if(t_f ==4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 22 :
// взятие с превращением  в коня----------------------------------------------------------------------------------
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    bitboard->white_name_from_coord[f_p] = 0;

	bitboard->black_name_from_coord[f_p] = 2;
	bitboard->black_name_from_coord[i_p] = 0;

//рисуем фигуру на новом месте------------------
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

//-------------------------------------------------------  
// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 0){
        list_surplus_moves->return_castling_Q[depth]= bitboard->castling_Q;
		bitboard->castling_Q =0;
	}//if(f_p == 0){
	if(f_p == 7){
        list_surplus_moves->return_castling_K[depth]= bitboard->castling_K;
		bitboard->castling_K =0;
	}//if(f_p == 7){	
  }//if( t_f ==4){
//-------------------------------------------------------  

    bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


   default:;//ход не распознан
   
 }// switch (name){
}//void Onward_moves_816( int i,T_list_surplus_moves * list_surplus_moves,T_board_list_816 * board_list_816){ 
//	*******************************************************************




//	===================================================================
// ход назад для белых
void Backs_moves_white( int i,const T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);


// востанавливаем флаг взятия на проходе
bitboard->taking_passage_yes=list_surplus_moves->return_taking_passage[depth];


// является ли ход ходом 0, взятием 1, взятием на проходе 2, рокировкой 3,
// превращение в ладью 4,превращение в коня 5,превращение в слона 6,превращение в ферзь 7,
// превращение с взятием в ладью 8,превращение с взятием в коня 9,
// превращение с взятием в слона 10,превращение с взятием в ферзь 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){

   case 1 :
{
  //1 ход на пустое поле-----------------------------------------------------------------------

// пешка - флаг взятия на проходе 
//-------------------------------------------------------
  if(bitboard->white_name_from_coord[f_p] == 1){
	 if(f_p - i_p == 16){
		  bitboard->taking_passage_yes =0;
          bitboard->taking_passage_x =  0;
          bitboard->taking_passage_y =  0;
	 }//if(f_p - i_p == 16){
//-------------------------------------------------------
// король - флаг рокировки
//-------------------------------------------------------  
  } else if(bitboard->white_name_from_coord[f_p] == 6){
// запоминаем флаги рокировок
		 bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
         bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];   
//-------------------------------------------------------  
// ладья - флаг рокировки  
 } else if(bitboard->white_name_from_coord[f_p] == 4){
  
	if(i_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(i_p == 0){
	if(i_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(i_p == 7){
 }//if(bitboard->black_name_from_coord[f_p] ==1){

//стираем фигуру на старом месте----------------- 
// все белые фигуры
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

//рисуем фигуру на новом месте------------------
// все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 взятие----------------------------------------------------------------------------------
{
//
// король - флаг рокировки
//-------------------------------------------------------  
   if(bitboard->white_name_from_coord[f_p] == 6){
// запоминаем флаги рокировок
		 bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
         bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];   

// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
   } else if(bitboard->white_name_from_coord[f_p] == 4){	
	if(i_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(i_p == 0){
	if(i_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(i_p == 7){
   }//if(bitboard->black_name_from_coord[f_p] == 4){	

// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(f_p == 56){
	if(f_p == 63){
        bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
	}//if(f_p == 63){
  }//t_f == 4){	
//-------------------------------------------------------  


//стираем фигуру на старом месте----------------- 
// стираем фигуру из списка всех белых фигур
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// востанавливаем имя взятой фигуры
	bitboard->black_name_from_coord[f_p] = t_f;

//рисуем фигуру на новом месте------------------
// в маске все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// в маске все черные фигуры
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// битовый список взятой фигуры
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
//3 взятие на проходе-----------------------------------------------------------------------
{

//стираем фигуру на старом месте----------------- 
// стираем фигуру из списка всех белых фигур
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// востанавливаем имя взятой фигуры
	bitboard->black_name_from_coord[(f_p - 8)] = t_f;

//рисуем фигуру на новом месте------------------
// в маске все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// в маске все черные фигуры
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// битовый список взятой фигуры
    bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] = bitboard->black_piece[bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
   
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
 // 0-0-0     -----------------------------------------------------------------------
{

// король - флаг рокировки
//-------------------------------------------------------  
         bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
         bitboard->castling_K = list_surplus_moves->return_castling_K[depth];   
//-------------------------------------------------------  

//стираем фигуру на старом месте----------------- 
// король
// все белые фигуры
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// ладья
// все белые фигуры
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[3];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[3]] = bitboard->white_piece[bitboard->white_name_from_coord[3]] ^ move_masks_m[3];
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[0] = bitboard->white_name_from_coord[3];
	bitboard->white_name_from_coord[3] = 0;

//рисуем фигуру на новом месте------------------
// король
// все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

// ладья
// все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[0];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[0]] = bitboard->white_piece[bitboard->white_name_from_coord[0]] ^ move_masks_m[0];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   case 4 :
 // 0-0     -----------------------------------------------------------------------
{
 //-----------------------------------------------------------------------

// король - флаг рокировки
//------------------------------------------------------- 
          bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
          bitboard->castling_K = list_surplus_moves->return_castling_K[depth];   
//-------------------------------------------------------  

//стираем фигуру на старом месте----------------- 
// король
// все белые фигуры
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[i_p] = bitboard->white_name_from_coord[f_p];
	bitboard->white_name_from_coord[f_p] = 0;

// ладья
// все белые фигуры
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[5];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[5]] = bitboard->white_piece[bitboard->white_name_from_coord[5]] ^ move_masks_m[5];
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[7] = bitboard->white_name_from_coord[5];
	bitboard->white_name_from_coord[5] = 0;

//рисуем фигуру на новом месте------------------
// король
// все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

// ладья
// все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[7];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[7]] = bitboard->white_piece[bitboard->white_name_from_coord[7]] ^ move_masks_m[7];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
   case 14 :
   case 13 :
   case 12 :
{
 // вернем ферзь-----------------------------------------------------------------------
  //-----------------------------------------------------------------------

//стираем фигуру на старом месте----------------- 
// все белые фигуры
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[i_p] = 1;
	bitboard->white_name_from_coord[f_p] = 0;

//рисуем фигуру на новом месте------------------
// все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
   case 24 :
   case 23 :
   case 22 :
//2 взятие с превращением---------------------------------------------------------------------------------
{

// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(f_p == 56){
	if(f_p == 63){
        bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
	}//if(f_p == 63){
  }//if(bitboard->black_name_from_coord[f_p] == 4){	
//-------------------------------------------------------  

//-------------------------------------------------------  
//стираем фигуру на старом месте----------------- 
// стираем фигуру из списка всех белых фигур
   bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];
// стираем фигуру из битового списка фигур
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

// переносим имя фигуры и зануляем по старой координате
	bitboard->white_name_from_coord[i_p] = 1;
	bitboard->white_name_from_coord[f_p] = 0;

// востанавливаем имя взятой фигуры
	bitboard->black_name_from_coord[f_p] = t_f;

//рисуем фигуру на новом месте------------------
// в маске все белые фигуры
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[i_p];
// в маске все черные фигуры
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
// битовый список фигуры
    bitboard->white_piece[bitboard->white_name_from_coord[i_p]] = bitboard->white_piece[bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
// битовый список взятой фигуры
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   default:;//ход не распознан
   
 }// switch (name){

}
//	*******************************************************************


//	===================================================================
// ход назад для черных
void Backs_moves_black( int i,const T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
 int depth =list_surplus_moves->current_depth;
 int i_p = ((list_surplus_moves->move[i]>>12)& 63);
 int f_p = ((list_surplus_moves->move[i]>>18)& 63);
 int t_f = ( list_surplus_moves->move[i] & 63);


// востанавливаем флаг взятия на проходе
bitboard->taking_passage_yes=list_surplus_moves->return_taking_passage[depth];


// является ли ход ходом 0, взятием 1, взятием на проходе 2, рокировкой 3,
// превращение в ладью 4,превращение в коня 5,превращение в слона 6,превращение в ферзь 7,
// превращение с взятием в ладью 8,превращение с взятием в коня 9,
// превращение с взятием в слона 10,превращение с взятием в ферзь 11,
switch (((list_surplus_moves->move[i]>>6)& 63)){

   case 1 :
{
 //1 ход на пустое поле-----------------------------------------------------------------------

// пешка - флаг взятия на проходе 
//-------------------------------------------------------
  if(bitboard->black_name_from_coord[f_p] ==1){
	 if(f_p - i_p == -16){
		  bitboard->taking_passage_yes = 0;
          bitboard->taking_passage_x =   0;
          bitboard->taking_passage_y =   0;
	 }//if(f_p - i_p == -16){
//-------------------------------------------------------
// король - флаг рокировки
//-------------------------------------------------------  
  } else if(bitboard->black_name_from_coord[f_p] == 6){
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
         bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
//-------------------------------------------------------  
// ладья - флаг рокировки  
 } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(i_p == 56){
	if(i_p == 63){
        bitboard->castling_k =  list_surplus_moves->return_castling_k[depth];
	}//if(i_p == 63){
 }//if(bitboard->black_name_from_coord[f_p] ==1){


    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
    bitboard->black_name_from_coord[f_p] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 2 :
//2 взятие----------------------------------------------------------------------------------
{
// король - флаг рокировки
//-------------------------------------------------------  
   if(bitboard->black_name_from_coord[f_p] == 6){
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
         bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
//-------------------------------------------------------  
// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
    } else if(bitboard->black_name_from_coord[f_p] == 4){	
	if(i_p == 56){
        bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
	}//if(i_p == 56){
	if(i_p == 63){
        bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
	}//if(i_p == 63){
     
 }//if(bitboard->black_name_from_coord[f_p] == 4){	
// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(f_p == 0){
	if(f_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(f_p == 7){	
  }//if(t_f ==4 ){
//-------------------------------------------------------  

   bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
   bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
	bitboard->black_name_from_coord[f_p] = 0;

    bitboard->white_name_from_coord[f_p] = t_f;
//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 5 :
//3 взятие на проходе-----------------------------------------------------------------------
{
   bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
   bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
	bitboard->black_name_from_coord[f_p] = 0;

    bitboard->white_name_from_coord[(f_p + 8)] = t_f;
//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] = bitboard->white_piece[bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
   
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 3 :
 // 0-0-0     -----------------------------------------------------------------------
{
// король - флаг рокировки
//-------------------------------------------------------  
		 bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
//-------------------------------------------------------  

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
    bitboard->black_name_from_coord[f_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[59];
    bitboard->black_piece[bitboard->black_name_from_coord[59]] = bitboard->black_piece[bitboard->black_name_from_coord[59]] ^ move_masks_m[59];
    bitboard->black_name_from_coord[56] = bitboard->black_name_from_coord[59];
    bitboard->black_name_from_coord[59] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[56];
    bitboard->black_piece[bitboard->black_name_from_coord[56]] = bitboard->black_piece[bitboard->black_name_from_coord[56]] ^ move_masks_m[56];

	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   case 4 :
 // 0-0     -----------------------------------------------------------------------
{
// король - флаг рокировки
//-------------------------------------------------------  
		 bitboard->castling_q = list_surplus_moves->return_castling_q[depth];
		 bitboard->castling_k = list_surplus_moves->return_castling_k[depth];
//-------------------------------------------------------  

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = bitboard->black_name_from_coord[f_p];
    bitboard->black_name_from_coord[f_p] = 0;

    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[61];
    bitboard->black_piece[bitboard->black_name_from_coord[61]] = bitboard->black_piece[bitboard->black_name_from_coord[61]] ^ move_masks_m[61];
    bitboard->black_name_from_coord[63] = bitboard->black_name_from_coord[61];
    bitboard->black_name_from_coord[61] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[63];
    bitboard->black_piece[bitboard->black_name_from_coord[63]] = bitboard->black_piece[bitboard->black_name_from_coord[63]] ^ move_masks_m[63];

	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 15 :
   case 14 :
   case 13 :
   case 12 :
{
    bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
    bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    bitboard->black_name_from_coord[i_p] = 1;
    bitboard->black_name_from_coord[f_p] = 0;

//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   case 25 :
   case 24 :
   case 23 :
   case 22 :
//2 взятие с превращением---------------------------------------------------------------------------------
{
// если сьедена ладья то запрещаем рокировки с ее участием
 if(t_f == 4){
	if(f_p == 0){
        bitboard->castling_Q = list_surplus_moves->return_castling_Q[depth];
	}//if(f_p == 0){
	if(f_p == 7){
        bitboard->castling_K = list_surplus_moves->return_castling_K[depth];
	}//if(f_p == 7){	
  }//if(t_f ==4){
   bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[f_p];
   bitboard->black_piece[bitboard->black_name_from_coord[f_p]] = bitboard->black_piece[bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	bitboard->black_name_from_coord[i_p] = 1;
	bitboard->black_name_from_coord[f_p] = 0;

    bitboard->white_name_from_coord[f_p] = t_f;
//рисуем фигуру на новом месте------------------
	bitboard->all_black_piece = bitboard->all_black_piece ^ move_masks_m[i_p];
	bitboard->all_white_piece = bitboard->all_white_piece ^ move_masks_m[f_p];

    bitboard->black_piece[bitboard->black_name_from_coord[i_p]] = bitboard->black_piece[bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    bitboard->white_piece[bitboard->white_name_from_coord[f_p]] = bitboard->white_piece[bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
	
	bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;
}
   break;
//
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   default:;//ход не распознан
   
 }// switch (name){

}
//	*******************************************************************
