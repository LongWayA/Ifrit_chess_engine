#include <iostream> // TEST
using namespace std;

#include "e_Move.h"
#include "h_Estimation.h"

//-----------------------------------------------------------------------------------
/*++
 * задаем функции по реализации ходов
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.02.2009
 *
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
 * все что здесь проходит можно проверить в модуле d_Loop
 *
*/
//---------------------------------------------------------------------------

extern class EVAL eval_o;// статическая оценка позиции(здесь мы ее иницализируем)

/*
 имя фигуры
 0 - нет фигуры
 1 - пешка 
 2 - конь 
 3 - слон
 4 - ладья 
 5 - ферзь 
 6 - король
*/

//	===================================================================
// реализуем ходы вперед для белых
void MOVES::Onward_moves_white(const unsigned __int8 & i,//номер хода по списку
                               struct T_undo undo[],//информация для отката хода
					           const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
						       struct T_Bitboard * const p_bitboard// представление позиции
						       ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);// исходная позиция фигуры
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);// финальная позиция фигуры
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);// взятая фигура

// запоминаем флаг взятия на проходе
undo->return_taking_passage = p_bitboard->taking_passage_yes;

// всегда запоминаем(нужно для хеш-таблицы)
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;

// запрещаем взятие на проходе 
p_bitboard->taking_passage_yes = 0;

// белые сходят значит станет ход черных
p_bitboard->colour_move = 0;

       // всегда запоминаем флаги рокировок (нужно для хеш-таблицы)
      undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;

/*
 тип хода
 0 - нет хода
 1 – простой ход
 2 – взятие
 3 – длинная рокировка
 4 – короткая рокировка
 5 – взятие на проходе

 12 - превращение в конь 
 13 - превращение в слон
 14 - превращение в ладья 
 15 - превращение в ферзь 

 22 - взятие с превращением в конь 
 23 - взятие с превращением в слон
 24 - взятие с превращением в ладья 
 25 - взятие с превращением в ферзь 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  // ход на пустое поле----------------------------------------------------------------------
  case 1 :

    //стираем фигуру на старом месте---------------------------------------------- 
    //стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

    //стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

    //переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------------------------------------
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //---------------------------------------------------------------------------------
    // особо отмечаем случаи влияющие на рокировку и взятие на проходе
    // мы тут смотрим какой фигурой ходили и если это пешка то проверяем не нужно
    // ли отметить взятие на проходе если это король то отменяем его рокировки
    // а если ладья то отменяем рокировку с ней 
    //---------------------------------------------------------------------------------
    // пешка - флаг взятия на проходе
    if(p_bitboard->white_name_from_coord[f_p] == 1){
	  if(f_p - i_p == 16){
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;
	      p_bitboard->taking_passage_yes = 1;
          p_bitboard->taking_passage_x =(i_p + 8) % 8;
          p_bitboard->taking_passage_y =(i_p + 8) / 8;
	  }//if(f_p - i_p == 16){

    // король - флаг рокировки
    } else if(p_bitboard->white_name_from_coord[f_p] == 6){
        // запоминаем флаги рокировок
      undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      p_bitboard->castling_K = 0;
	  p_bitboard->castling_Q = 0;	   

    // ладья - флаг рокировки  
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){
	  if(i_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(i_p == 0){
	  if(i_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(i_p == 7){
    }//if(p_bitboard->white_name_from_coord[f_p] == 1){
    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // взятие----------------------------------------------------------------------------------
  case 2 :

    // убираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // убираем взятую фигуру из маски всех черных фигур
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // убираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // кроме того убираем взятую фигуру из битового списка фигур
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //---------------------------------------------------------------------------------
    // взятие белыми
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //---------------------------------------------------------------------------------

    // обнуляем взятую фигуру
    p_bitboard->black_name_from_coord[f_p] = 0;
    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    // рисуем берущую фигуру в маске всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем берущую фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // король - флаг рокировки
    if(p_bitboard->white_name_from_coord[f_p] == 6){
      // запоминаем флаги рокировок
	  undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      p_bitboard->castling_K = 0;
	  p_bitboard->castling_Q = 0;		
  
    // смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){	
	  if(i_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q =0;
	  }//if(i_p == 0){
	  if(i_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K =0;
	  }//if(i_p == 7){
    }//if(p_bitboard->black_name_from_coord[f_p] == 4){	

    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
  break;

  // взятие на проходе-----------------------------------------------------------------------
  case 5 :

    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;

    // убираем взятую фигуру из маски всех черных фигур
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
    // кроме того убираем взятую фигуру из битового списка фигур
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

    //------------------------------------------------------------------------------
    // взятие белыми
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // обнуляем взятую фигуру
    p_bitboard->black_name_from_coord[(f_p - 8)] = 0;

    //рисуем фигуру на новом месте------------------
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // рокировка 0-0-0-------------------------------------------------------------------------
  case 3 :

    //стираем фигуру на старом месте----------------- 
    // ход королем
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;
    // ход ладьей
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[0];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] ^ move_masks_m[0];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[3] = p_bitboard->white_name_from_coord[0];
	p_bitboard->white_name_from_coord[0] = 0;

    //рисуем фигуру на новом месте------------------
    // ход королем
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // ход ладьей
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[3];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] ^ move_masks_m[3];

    // король - флаг рокировки
    // запоминаем флаги рокировок
    undo->return_castling_Q = p_bitboard->castling_Q;
    undo->return_castling_K = p_bitboard->castling_K;
	p_bitboard->castling_Q = 0;
    p_bitboard->castling_K = 0;

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // рокировка 0-0--------------------------------------------------------------------------
  case 4 :

    //стираем фигуру на старом месте----------------- 
    // ход королем
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
	p_bitboard->white_name_from_coord[i_p] = 0;
    // ход ладьей
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[7];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] ^ move_masks_m[7];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[5] = p_bitboard->white_name_from_coord[7];
	p_bitboard->white_name_from_coord[7] = 0;

    //рисуем фигуру на новом месте------------------
    // ход королем
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // ход ладьей
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[5];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] ^ move_masks_m[5];

    // король - флаг рокировки
    // запоминаем флаги рокировок
    undo->return_castling_Q = p_bitboard->castling_Q;
    undo->return_castling_K = p_bitboard->castling_K;
	p_bitboard->castling_Q = 0;
    p_bitboard->castling_K = 0;

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // превращение в ферзя--------------------------------------------------------------------
  case 15 :

    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = 5;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval + eval_o.QUEEN - eval_o.PAWN;
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

// превращение в ладью----------------------------------------------------------------------
  case 14 :

    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = 4;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.ROOK - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // превращение в слона--------------------------------------------------------------------
  case 13 :

    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = 3;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен +белые и -черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.BISHOP - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // превращение в коня---------------------------------------------------------------------
  case 12 :

    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // переносим имя фигуры в новую координату и обнуляем его по старой координате
	p_bitboard->white_name_from_coord[f_p] = 2;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.KNIGHT - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
    // рисуем фигуру в маске всех белых фигур
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
  break;

  // взятие с превращением в ферзя----------------------------------------------------------
  case 25 :

    // убираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // убираем взятую фигуру из маски всех черных фигур
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // убираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // кроме того убираем взятую фигуру из битового списка фигур
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // взятие с превращением белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
         p_bitboard->eval = p_bitboard->eval + eval_o.QUEEN - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // обнуляем взятую фигуру
    p_bitboard->black_name_from_coord[f_p] = 0;
    // переносим имя фигуры и зануляем по старой координате
	p_bitboard->white_name_from_coord[f_p] = 5;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    // рисуем берущую фигуру в маске всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем берущую фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

// взятие с превращением в ладью------------------------------------------------------------
  case 24 :

    // убираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // убираем взятую фигуру из маски всех черных фигур
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // убираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // кроме того убираем взятую фигуру из битового списка фигур
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // взятие с превращением белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
         p_bitboard->eval = p_bitboard->eval + eval_o.ROOK - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // зануляем взятую фигуру
    p_bitboard->black_name_from_coord[f_p] = 0;
    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[f_p] = 4;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    // рисуем берущую фигуру в маске всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем берущую фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // взятие с превращением слона--------------------------------------------------------------
  case 23 :

    // убираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // убираем взятую фигуру из маски всех черных фигур
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // убираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // кроме того убираем взятую фигуру из битового списка фигур
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // взятие с превращением белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.BISHOP - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // зануляем взятую фигуру
    p_bitboard->black_name_from_coord[f_p] = 0;
    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[f_p] = 3;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    // рисуем берущую фигуру в маске всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем берущую фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

// взятие с превращением  в коня------------------------------------------------------------
  case 22 :

    // убираем фигуру из маски всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // убираем взятую фигуру из маски всех черных фигур
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

    // убираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // кроме того убираем взятую фигуру из битового списка фигур
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // взятие с превращением белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.KNIGHT - eval_o.PAWN + eval_o.kod[p_bitboard->black_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // обнуляем взятую фигуру
    p_bitboard->black_name_from_coord[f_p] = 0;
    // переносим имя фигуры и зануляем по старой координате
	p_bitboard->white_name_from_coord[f_p] = 2;
	p_bitboard->white_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    // рисуем берущую фигуру в маске всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // рисуем берущую фигуру в битовом списке фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    //-------------------------------------------------------  

    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f  == 4){
	  if(f_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(f_p == 56){
	  if(f_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(f_p == 63){
    }//if(t_f  == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  default:;//ход не распознан
   
}// switch (name){

}//void MOVES::Onward_moves_white
//	*******************************************************************

//	===================================================================
// реализуем ходы вперед для черных
void MOVES::Onward_moves_black(const unsigned __int8 & i,//номер хода по списку
                               struct T_undo undo[],//информация для отката хода
					           const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
						       struct T_Bitboard * const p_bitboard// представление позиции
						       ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


// запоминаем флаг взятия на проходе
undo->return_taking_passage= p_bitboard->taking_passage_yes;

// всегда запоминаем(нужно для хеш-таблицы)
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;


// запрещаем взятие на проходе 
p_bitboard->taking_passage_yes = 0;

// черные сходят значит станет ход белых
p_bitboard->colour_move = 1;

       // всегда запоминаем флаги рокировок (нужно для хеш-таблицы)
      undo->return_castling_K = p_bitboard->castling_K;
      undo->return_castling_Q = p_bitboard->castling_Q;
      undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;


/*
 тип хода
 0 - нет хода
 1 – простой ход
 2 – взятие
 3 – длинная рокировка
 4 – короткая рокировка
 5 – взятие на проходе

 12 - превращение в конь 
 13 - превращение в слон
 14 - превращение в ладья 
 15 - превращение в ферзь 

 22 - взятие с превращением в конь 
 23 - взятие с превращением в слон
 24 - взятие с превращением в ладья 
 25 - взятие с превращением в ферзь 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  //1 ход на пустое поле------------------------------------------------------------------
  case 1 :
    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // пешка - флаг взятия на проходе 
    if(p_bitboard->black_name_from_coord[f_p] ==1){
	  if(f_p - i_p == -16){
          //cout << "i_p= "<< i_p<<"\n";
          //cout << "f_p= "<< f_p<<"\n";
          //cout << "p_bitboard->black_name_from_coord[f_p]= "<< p_bitboard->black_name_from_coord[f_p]<<"\n";
          undo->taking_passage_x = p_bitboard->taking_passage_x;
          undo->taking_passage_y = p_bitboard->taking_passage_y;
		  p_bitboard->taking_passage_yes =1;
          p_bitboard->taking_passage_x =(i_p - 8) % 8;
          p_bitboard->taking_passage_y =(i_p - 8) / 8;
	  }//if(f_p - i_p == -16){

    // король - флаг рокировки
    } else if(p_bitboard->black_name_from_coord[f_p] == 6){
	  undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;
      p_bitboard->castling_k = 0;
	  p_bitboard->castling_q = 0;

    // ладья - флаг рокировки  
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(i_p == 56){
	  if(i_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(i_p == 63){
     
    }//if(p_bitboard->black_name_from_coord[f_p] ==1){

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //2 взятие----------------------------------------------------------------------------------
  case 2 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // взятие черными
    // от оценки мы отнимаем значение взятой фигуры
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
      p_bitboard->eval = p_bitboard->eval - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // король - флаг рокировки
    if(p_bitboard->black_name_from_coord[f_p] == 6){
	  undo->return_castling_k = p_bitboard->castling_k;
      undo->return_castling_q = p_bitboard->castling_q;
      p_bitboard->castling_k = 0;
	  p_bitboard->castling_q = 0;

// смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          undo->return_castling_q = p_bitboard->castling_q;
		  p_bitboard->castling_q = 0;
	  }//if(i_p == 56){
	  if(i_p == 63){
          undo->return_castling_k = p_bitboard->castling_k;
		  p_bitboard->castling_k = 0;
	  }//if(i_p == 63){
     
    }//if(p_bitboard->black_name_from_coord[f_p] == 6){
    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f == 4){

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // 3 взятие на проходе----------------------------------------------------------------------
  case 5 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];
  
    //------------------------------------------------------------------------------
    // взятие черными
    // от оценки мы отнимаем значение взятой фигуры
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------
	
	p_bitboard->white_name_from_coord[(f_p + 8)] = 0;


    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //рокировка 0-0-0
  case 3 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[56];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] ^ move_masks_m[56];
	p_bitboard->black_name_from_coord[59] = p_bitboard->black_name_from_coord[56];
	p_bitboard->black_name_from_coord[56] = 0;

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[59];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] ^ move_masks_m[59];


    // король - флаг рокировки
    //-------------------------------------------------------  
    undo->return_castling_q = p_bitboard->castling_q;
    undo->return_castling_k = p_bitboard->castling_k;
	p_bitboard->castling_q = 0;
    p_bitboard->castling_k = 0;
    //-------------------------------------------------------  

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //рокировка 0-0
  case 4 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
	p_bitboard->black_name_from_coord[i_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[63];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] ^ move_masks_m[63];
	p_bitboard->black_name_from_coord[61] = p_bitboard->black_name_from_coord[63];
	p_bitboard->black_name_from_coord[63] = 0;

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[61];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] ^ move_masks_m[61];


    // король - флаг рокировки
    //-------------------------------------------------------  
    undo->return_castling_q = p_bitboard->castling_q;
    undo->return_castling_k = p_bitboard->castling_k;
	p_bitboard->castling_q = 0;
    p_bitboard->castling_k = 0;
    //-------------------------------------------------------  

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //превращение в ферзя-----------------------------------------------------------------------
  case 15 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 5;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.QUEEN + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //превращение в ладью-----------------------------------------------------------------------
  case 14 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 4;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.ROOK + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //превращение в слона-----------------------------------------------------------------------
  case 13 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 3;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.BISHOP + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //превращение в конь-----------------------------------------------------------------------
  case 12 :

    //стираем фигуру на старом месте----------------- 
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	p_bitboard->black_name_from_coord[f_p] = 2;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.KNIGHT + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // корректируем маску всех фигур
    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // взятие с превращением в ферзя--------------------------------------------------------------
  case 25 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
       p_bitboard->eval = p_bitboard->eval - eval_o.QUEEN + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------


    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = 5;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f ==4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // взятие с превращением в ладью--------------------------------------------------------------
  case 24 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.ROOK + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = 4;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // если сьедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f == 4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // взятие с превращением слона----------------------------------------------------------------
  case 23 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.BISHOP + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;

	p_bitboard->black_name_from_coord[f_p] = 3;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // если сьедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if(t_f ==4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // взятие с превращением  в коня--------------------------------------------------------------
  case 22 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // превращение черными
    // от оценки мы отнимаем значение фигуры превращения и прибавляем пешку
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.KNIGHT + eval_o.PAWN - eval_o.kod[p_bitboard->white_name_from_coord[f_p]];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    p_bitboard->white_name_from_coord[f_p] = 0;
	p_bitboard->black_name_from_coord[f_p] = 2;
	p_bitboard->black_name_from_coord[i_p] = 0;

    //рисуем фигуру на новом месте------------------
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //-------------------------------------------------------  
    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 0){
          undo->return_castling_Q = p_bitboard->castling_Q;
		  p_bitboard->castling_Q = 0;
	  }//if(f_p == 0){
	  if(f_p == 7){
          undo->return_castling_K = p_bitboard->castling_K;
		  p_bitboard->castling_K = 0;
	  }//if(f_p == 7){	
    }//if( t_f ==4){
    //-------------------------------------------------------  

    p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;


  default:;//ход не распознан
   
}// switch (name){

}// void MOVES::Onward_moves_black
//	*******************************************************************




//	===================================================================
// ход назад для белых
void MOVES::Backs_moves_white(const unsigned __int8 & i,//номер хода по списку
					          const struct T_undo undo[],//информация для отката хода
					          const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
                              struct T_Bitboard * const p_bitboard// представление позиции
					          ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


// восстанавливаем флаг взятия на проходе
p_bitboard->taking_passage_yes=undo->return_taking_passage;

// белые забрали свой ход значит опять их ход стал
p_bitboard->colour_move = 1;

/*
 тип хода
 0 - нет хода
 1 – простой ход
 2 – взятие
 3 – длинная рокировка
 4 – короткая рокировка
 5 – взятие на проходе

 12 - превращение в конь 
 13 - превращение в слон
 14 - превращение в ладья 
 15 - превращение в ферзь 

 22 - взятие с превращением в конь 
 23 - взятие с превращением в слон
 24 - взятие с превращением в ладья 
 25 - взятие с превращением в ферзь 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  //1 ход на пустое поле-----------------------------------------------------------------------
  case 1 :
 
    // пешка - флаг взятия на проходе 
    if(p_bitboard->white_name_from_coord[f_p] == 1){
	  if(f_p - i_p == 16){
		  p_bitboard->taking_passage_yes =undo->return_taking_passage;
          p_bitboard->taking_passage_x = undo->taking_passage_x;
          p_bitboard->taking_passage_y = undo->taking_passage_y;
	  }//if(f_p - i_p == 16){

    // король - флаг рокировки
    } else if(p_bitboard->white_name_from_coord[f_p] == 6){
          // вспоминаем флаги рокировок
		  p_bitboard->castling_K = undo->return_castling_K;
          p_bitboard->castling_Q = undo->return_castling_Q;   

    // ладья - флаг рокировки  
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){
  
	  if(i_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(i_p == 0){
	  if(i_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	  }//if(i_p == 7){
    }//if(p_bitboard->black_name_from_coord[f_p] ==1){

    //стираем фигуру на старом месте----------------- 
    // все белые фигуры
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    //рисуем фигуру на новом месте------------------
    // все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //2 взятие----------------------------------------------------------------------------------
  case 2 :

    // король - флаг рокировки
    if(p_bitboard->white_name_from_coord[f_p] == 6){
      // вспоминаем флаги рокировок
	  p_bitboard->castling_K = undo->return_castling_K;
      p_bitboard->castling_Q = undo->return_castling_Q;   

    // смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
    } else if(p_bitboard->white_name_from_coord[f_p] == 4){	
	  if(i_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(i_p == 0){
	  if(i_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	  }//if(i_p == 7){
    }//if(p_bitboard->white_name_from_coord[f_p] == 6){

    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(f_p == 56){
	  if(f_p == 63){
          p_bitboard->castling_k = undo->return_castling_k;
	  }//if(f_p == 63){
    }//t_f == 4){	

    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из списка всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // востанавливаем имя взятой фигуры
	p_bitboard->black_name_from_coord[f_p] = t_f;

    //------------------------------------------------------------------------------
    // взятие белыми
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
	// (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
    // в маске все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // в маске все черные фигуры
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // битовый список взятой фигуры
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //3 взятие на проходе-----------------------------------------------------------------------
  case 5 :

    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из списка всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // востанавливаем имя взятой фигуры
	p_bitboard->black_name_from_coord[(f_p - 8)] = t_f;

    //------------------------------------------------------------------------------
    // взятие белыми
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    // (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
    // в маске все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // в маске все черные фигуры
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // битовый список взятой фигуры
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
  break;

  // 0-0-0     -----------------------------------------------------------------------
  case 3 :

    // король - флаг рокировки
    //-------------------------------------------------------  
    p_bitboard->castling_Q = undo->return_castling_Q;
    p_bitboard->castling_K = undo->return_castling_K;   
    //-------------------------------------------------------  

    //стираем фигуру на старом месте----------------- 
    // король
    // все белые фигуры
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // ладья
    // все белые фигуры
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[3];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] ^ move_masks_m[3];
    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[0] = p_bitboard->white_name_from_coord[3];
	p_bitboard->white_name_from_coord[3] = 0;

    //рисуем фигуру на новом месте------------------
    // король
    // все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

    // ладья
    // все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[0];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] ^ move_masks_m[0];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // 0-0     -----------------------------------------------------------------------
  case 4 :

    // король - флаг рокировки
    //------------------------------------------------------- 
    p_bitboard->castling_Q = undo->return_castling_Q;
    p_bitboard->castling_K = undo->return_castling_K;   
    //-------------------------------------------------------  

    //стираем фигуру на старом месте----------------- 
    // король
    // все белые фигуры
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
    // переносим имя фигуры и зануляем по старой координате
	p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
	p_bitboard->white_name_from_coord[f_p] = 0;

    // ладья
    // все белые фигуры
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[5];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] ^ move_masks_m[5];
    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[7] = p_bitboard->white_name_from_coord[5];
	p_bitboard->white_name_from_coord[5] = 0;

    //рисуем фигуру на новом месте------------------
    // король
    // все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

    // ладья
    // все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[7];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] ^ move_masks_m[7];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // вернем ферзь-----------------------------------------------------------------------
  case 15 :
  case 14 :
  case 13 :
  case 12 :

    //стираем фигуру на старом месте----------------- 
    // все белые фигуры
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен + белые и - черные
    // (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.kod[p_bitboard->white_name_from_coord[f_p]] + eval_o.PAWN;
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // переносим имя фигуры и зануляем по старой координате
	p_bitboard->white_name_from_coord[i_p] = 1;
	p_bitboard->white_name_from_coord[f_p] = 0;

    //рисуем фигуру на новом месте------------------
    // все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

//2 взятие с превращением---------------------------------------------------------------------
  case 25 :
  case 24 :
  case 23 :
  case 22 :

    //  вспоминаем если съедена ладья 
    if(t_f == 4){
	  if(f_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(f_p == 56){
	  if(f_p == 63){
          p_bitboard->castling_k = undo->return_castling_k;
	  }//if(f_p == 63){
    }//if(p_bitboard->black_name_from_coord[f_p] == 4){	

    //-------------------------------------------------------  
    //стираем фигуру на старом месте----------------- 
    // стираем фигуру из списка всех белых фигур
    p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
    // стираем фигуру из битового списка фигур
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен + белые и - черные
    // (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval - eval_o.kod[p_bitboard->white_name_from_coord[f_p]] + eval_o.PAWN - eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    // переносим имя фигуры и обнуляем по старой координате
	p_bitboard->white_name_from_coord[i_p] = 1;
	p_bitboard->white_name_from_coord[f_p] = 0;

    // восстанавливаем имя взятой фигуры
	p_bitboard->black_name_from_coord[f_p] = t_f;

    //рисуем фигуру на новом месте------------------
    // в маске все белые фигуры
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];
    // в маске все черные фигуры
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    // битовый список фигуры
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
    // битовый список взятой фигуры
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  default:;//ход не распознан
   
}// switch (name){

}//void MOVES::Backs_moves_white
//	*******************************************************************


//	===================================================================
// ход назад для черных
void MOVES::Backs_moves_black(const unsigned __int8 & i,//номер хода по списку
					          const struct T_undo undo[],//информация для отката хода
					          const struct T_list_surplus_moves * const list_surplus_moves,//список возможных ходов
                              struct T_Bitboard * const p_bitboard// представление позиции
					          ){

unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


// востанавливаем флаг взятия на проходе
p_bitboard->taking_passage_yes=undo->return_taking_passage;

// черные забрали свой ход значит опять их ход стал
p_bitboard->colour_move = 0;

/*
 тип хода
 0 - нет хода
 1 – простой ход
 2 – взятие
 3 – длинная рокировка
 4 – короткая рокировка
 5 – взятие на проходе

 12 - превращение в конь 
 13 - превращение в слон
 14 - превращение в ладья 
 15 - превращение в ферзь 

 22 - взятие с превращением в конь 
 23 - взятие с превращением в слон
 24 - взятие с превращением в ладья 
 25 - взятие с превращением в ферзь 
 */
switch (((list_surplus_moves->move[i]>>6)& 63)){

  //1 ход на пустое поле-----------------------------------------------------------------------
  case 1 :

    // пешка - флаг взятия на проходе 
    if(p_bitboard->black_name_from_coord[f_p] ==1){
	  if(f_p - i_p == -16){
		  p_bitboard->taking_passage_yes =undo->return_taking_passage;
          p_bitboard->taking_passage_x = undo->taking_passage_x;
          p_bitboard->taking_passage_y = undo->taking_passage_y;
	  }//if(f_p - i_p == -16){

// король - флаг рокировки
    } else if(p_bitboard->black_name_from_coord[f_p] == 6){
		  p_bitboard->castling_k = undo->return_castling_k;
          p_bitboard->castling_q = undo->return_castling_q;

    // ладья - флаг рокировки  
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(i_p == 56){
	  if(i_p == 63){
          p_bitboard->castling_k =  undo->return_castling_k;
	  }//if(i_p == 63){
    }//if(p_bitboard->black_name_from_coord[f_p] ==1){


    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
    p_bitboard->black_name_from_coord[f_p] = 0;

//рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //2 взятие----------------------------------------------------------------------------------
  case 2 :

    // король - флаг рокировки
    if(p_bitboard->black_name_from_coord[f_p] == 6){
		  p_bitboard->castling_k = undo->return_castling_k;
          p_bitboard->castling_q = undo->return_castling_q;

    // смотрим а не ходили ли мы ладьей и если да то рокировки с ней отменяем
    } else if(p_bitboard->black_name_from_coord[f_p] == 4){	
	  if(i_p == 56){
          p_bitboard->castling_q = undo->return_castling_q;
	  }//if(i_p == 56){
	  if(i_p == 63){
          p_bitboard->castling_k = undo->return_castling_k;
	  }//if(i_p == 63){
     
    }//if(p_bitboard->black_name_from_coord[f_p] == 6){

    // если съедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(f_p == 0){
	  if(f_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	   }//if(f_p == 7){	
    }//if(t_f ==4 ){

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
	p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->white_name_from_coord[f_p] = t_f;

    //------------------------------------------------------------------------------
    // взятие белыми
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    // (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  //3 взятие на проходе-----------------------------------------------------------------------
  case 5 :
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

	p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
	p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->white_name_from_coord[(f_p + 8)] = t_f;

    //------------------------------------------------------------------------------
    // взятие белыми
    // к оценке мы прибавляем значение взятой фигуры 
    // потому что в начале баланс получен + белые и - черные
    // (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.PAWN;	
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];

	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
  break;

  // 0-0-0 -----------------------------------------------------------------------
  case 3 :

    // король - флаг рокировки
    //-------------------------------------------------------  
	p_bitboard->castling_q = undo->return_castling_q;
	p_bitboard->castling_k = undo->return_castling_k;
    //-------------------------------------------------------  

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
    p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[59];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] ^ move_masks_m[59];
    p_bitboard->black_name_from_coord[56] = p_bitboard->black_name_from_coord[59];
    p_bitboard->black_name_from_coord[59] = 0;

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[56];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] ^ move_masks_m[56];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  // 0-0     -----------------------------------------------------------------------
  case 4 :
 
    // король - флаг рокировки
    //-------------------------------------------------------  
    p_bitboard->castling_q = undo->return_castling_q;
    p_bitboard->castling_k = undo->return_castling_k;
    //-------------------------------------------------------  

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
    p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
    p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[61];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] ^ move_masks_m[61];
    p_bitboard->black_name_from_coord[63] = p_bitboard->black_name_from_coord[61];
    p_bitboard->black_name_from_coord[61] = 0;

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[63];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] ^ move_masks_m[63];

	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;


  case 15 :
  case 14 :
  case 13 :
  case 12 :

    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
  
    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен + белые и - черные
    // (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[p_bitboard->black_name_from_coord[f_p]] - eval_o.PAWN;
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------
	
	p_bitboard->black_name_from_coord[i_p] = 1;
    p_bitboard->black_name_from_coord[f_p] = 0;

    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

//2 взятие с превращением---------------------------------------------------------------------
  case 25 :
  case 24 :
  case 23 :
  case 22 :

    // если сьедена ладья то запрещаем рокировки с ее участием
    if(t_f == 4){
	  if(f_p == 0){
          p_bitboard->castling_Q = undo->return_castling_Q;
	  }//if(f_p == 0){
	  if(f_p == 7){
          p_bitboard->castling_K = undo->return_castling_K;
	  }//if(f_p == 7){	
    }//if(t_f ==4){
    p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
    p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

    //------------------------------------------------------------------------------
    // превращение белыми
    // к оценке мы прибавляем значение фигуры превращения и отнимаем пешку
    // потому что в начале баланс получен + белые и - черные
    // (обратный ход)
    #if MATERIAL
        p_bitboard->eval = p_bitboard->eval + eval_o.kod[p_bitboard->black_name_from_coord[f_p]] - eval_o.PAWN + eval_o.kod[t_f];
    #endif//#if MATERIAL
    //------------------------------------------------------------------------------


	p_bitboard->black_name_from_coord[i_p] = 1;
	p_bitboard->black_name_from_coord[f_p] = 0;

    p_bitboard->white_name_from_coord[f_p] = t_f;
    //рисуем фигуру на новом месте------------------
	p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
	p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

    p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
    p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
	
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

  break;

  default:;//ход не распознан
   
}// switch (name){

}//void MOVES::Backs_moves_black
//	*******************************************************************

// с помощью этой маски пишем и стираем фигуры из битового массива
const unsigned __int64 MOVES::move_masks_m[] = {
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


