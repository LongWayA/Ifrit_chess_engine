#include <iostream.h> // TEST
#include <fstream.h>  // TEST
#include <cstring>    // используем дл€ работы со строкой (сравнение строк)
#include <stdlib.h>   // обеспечиваем завершение потока функцией exit(1)


#include "k_Util.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "i_Forced.h"
#include "k_Win_32.h"
#include "g_Loop.h"
#include "j_Hash.h"
#include "j_Killer.h"
#include "l_Sorting.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"


//-----------------------------------------------------------------------------------
/*
 * перебор позиций
 *
 * ј¬“ќ– ChessTerminator75
 * ѕќ—Ћ≈ƒЌ≈≈ »«ћ≈Ќ≈Ќ»≈ 10.02.2008
 *
 *  “≈—“»–ќ¬јЌ»≈
 * тестовую информащию выводим в файл _TEST_LOOP.txt
 * печатаем
 * тестируем модули 
 * целью теста €вл€етс€
 *
 * ¬ќ«ћќ∆Ќџ≈ »Ќƒ” “ќ–џ ќЎ»Ѕ »
 * тестировать и описывать тут очень много. надо будет все тщательно продумать
 * по сути это сердце программы и именно тут сход€тс€ все ниточки
 * нужно контролировать:
 * 1 генератор позиций - правильно ли генерируютс€ всевозможные позиции не пропущенно ли что
 * 2 лини€ варианта - правильно ли она печатаетс€ нет ли тут искажений
 * правильно ли печатаетс€ оценка и глубина варианта а также количество позиций
 * 3 альфа-бета отсечение - правильно ли оно работает нет ли где недостаточных
 *   или избыточных отсечений 
*/
//---------------------------------------------------------------------------


//	===================================================================
/*
€ решил вернутс€ к схеме альфа бета с амортизацией отказов
int AlphaBeta (pos, depth, alpha, beta) {
	if (depth == 0) return Evaluate(pos);
	best = -INFINITY;
	succ = Successors(pos);
	while (!Empty(succ)) {
		pos = RemoveOne(succ);
		value = -AlphaBeta(pos, depth - 1, -beta, -alpha);
		if (value > best) best = value;
	    if (value > alpha) alpha = value;
        if (alpha >= beta) return alpha;
	}
	return best;
}
----------------------------------------------------------------------------
так работает генератор позиций:
1 в начале генерируютс€ список избыточных ходов. избыточных потому что нет проверки на шах
  и в них есть такие ходы как рокировка под шахом или ход корол€ под шах
2 дальше мы сортируем этот список вывод€ вз€ти€ на верх и ход из хеша если он есть
3 дальше мы реализуем ход и провер€ем позицию на шах если пол€ рокировки под боем или 
 под боем наш король то ход откатываем иначе идем дальше
4 итерационное углубление
5 когда вернулись с глубины откатываем ход
6 если ходов не было то это случай мата или пата

*/
// перебор на нулевом уровне
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
int Alpha_beta_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,int description_move,T_PV * three_moves,int & br,int for_chek,int null_m){
int value=0;              // текуща€ оценка позиции
int value_best=9999999;   // лучша€ в узле  оценка позиции  
int inv=0;                // инвертирование оценки позиции
int no_moves=0;           // флаг существовани€ легальных ходов (если 1 то легальных ходов не было)
int nodes_0=1;            // количество ходов на нулевом уровне
int number_taking=0;      // конечна€ позици€ по вз€ти€м в списке возможных ходов
int key=0;                // хеш ключ
int insert_hash=0;        // поместили ли ход из кеша на первую позицию
double  time_nodes=0;     // 
T_PV pv_best_point={0};   // лучший дл€ данного узла вариант

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// это отсечка при экстренном выходе
if (br == 1) return 0;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
list_surplus_moves->current_depth =depth;//
//«аполн€ем список избыточных ходов. »менно эти списки и составл€ют дерево перебора.
Generator_captures(bitboard,list_surplus_moves);
Generator_move(bitboard,list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// тестова€ печать списка и доски на нулевом уровне
  #if TEST_L
    List_816_Print(depth,list_surplus_moves);
  //Bitboard_print(bitboard);
  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if HASH
// работаем с хешем
// смотрим не встречалась ли нам така€ позици€ если встречалась то ее ход смотрим первым
// т.е. мы в списке ход из хеша ставим первым а первый на то место где был ход из хеша
  Look_hash(list_surplus_moves,bitboard->hash_key,depth,insert_hash);
 #endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
// пишем лучшего киллера
#if KILLER
// если неудача с хешем киллер пишем на первую позицию иначе на вторую
  if(insert_hash==0){
    Insert_killer(list_surplus_moves,depth,list_surplus_moves->start_list[depth]);
  }else{
    Insert_killer(list_surplus_moves,depth,(list_surplus_moves->start_list[depth]+1));
  }//if(insert_hash==0){
#endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// тестова€ печать списка и доски на нулевом уровне
  #if TEST_L
 //   List_816_Print(depth,list_surplus_moves);
 //   Board_816_Print( depth,board_list_816);
  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for ( int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){

// реализуем ход из списка или другими словами генерируем позицию
 Onward_moves(i,list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

// если король по шахом то
 if (King_check(bitboard)!=0){
// мы отмен€ем этот ход как некорректный
   Backs_moves(i,list_surplus_moves,bitboard);
 }else{
// ставим флаг что есть легальные ходы и засчитываем позицию
 no_moves = 1;
 nodes = nodes+1;

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// печатаем инфу о рассматриваемом ходе и глубине перебора
Print_cout_v(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value);
// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
 Update_PV_current(i,depth,pv_current,list_surplus_moves);
// обновл€ем хеш ключ
Modification_up_random_key(i,list_surplus_moves,bitboard);
// записываем ключ дл€ контрол€ при выныривании с глубины
key=bitboard->hash_key;

Timer_start();
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// рекурсивный вызов функции
value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,0,0);
list_surplus_moves->current_depth =depth;
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
 Timer_stop();
 time_nodes= Timer_delta();
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
// проверка хеш ключа на корректность т.е. что после возвращени€ с глубины он вернулс€ к 
// состо€нию которое он имел до погружени€ на глубину
 #if HASH
   if(key!=bitboard->hash_key){
     cout <<"\n";
     cout << "ERROR HESH " ;
     cout <<"\n";
   }//if(key!=board_list_816->hash_key){
 #endif 

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// восстанавливаем позицию
 Backs_moves(i,list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// восстанавливаем хеш ключ
 Modification_down_random_key(i,list_surplus_moves,bitboard);
 //Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max);

 // ищем лучшую позицию
 if (value < value_best){
   value_best = value;
// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
   Update_PV_best_point(&pv_best_point,pv_best);
 #if HASH
// хешируем лучший на данный момент ход
   Save_best_hash_key(i,list_surplus_moves,bitboard->hash_key,depth);
 #endif
   //Test_Loop << "bitboard->hash_key="<<bitboard->hash_key << "\n";
  // Bitboard_print(bitboard);
 }//if (value < value_best){ 

// ЅЋќ  ALPHA BETA ќѕ“»ћ»«ј÷»» 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value < alpha){
   alpha = value;
 }//if (value < alpha){ 

   if(alpha <= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

//--------------------------------------------------------------------
// выводим ход который обдумываем делаем это только на нулевой глубине т.е. у самой поверхности
 if (br!=1){
  nodes_0 = nodes_0+1;
 #if TEST_PRINT
   // дл€ теста вариантов
  Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value,time_nodes);
#else
  // дл€ рабочего режима
  Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max,value_best,time_nodes);
#endif


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  #if TEST_L
 //   List_816_Print(depth,list_surplus_moves);
 //   Board_816_Print( depth,board_list_816);
  #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

 }//if (br!=1){
//--------------------------------------------------------------------
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){
//3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333

//ЅЋќ 444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
// обработка случа€ когда не было легальных ходов

// если не одного хода сделанно не было то производим статическую оценку позиции
// это случай пата или мата
if (no_moves == 0){
  value=No_moves(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1 // голый полный перебор
// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
 Update_PV_best(pv_best,pv_best_point);
//Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max);
// cout <<"alpha= "<<alpha;
// cout <<"\n";
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

return value_best;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

}//int Alpha_beta_root(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************



//	===================================================================
// 
int Alpha_beta_816(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int for_chek, int null_m){
int value=0;              // текуща€ оценка позиции
int value_best=9999999;   // оценка позиции лучша€ в узле 
int inv=0;                // инвертирование оценки позиции
int no_moves=0;           // флаг существовани€ легальных ходов
int depth_max_z=0;        // форсировка на шахах
int for_chek_l=0;         // форсировка на шахах
int number_taking=0;      // конечна€ позици€ по вз€ти€м в списке возможных ходов
int key=0;                // хеш ключ
int insert_hash=0;        // поместили ли ход из кеша на первую позицию
T_PV pv_best_point={0};   // лучший дл€ данного узла вариант

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// это отсечка при экстренном выходе
if (br == 1) return 0;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

//ѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕ
// на предельной глубине производим статическую оценку позиции
if (depth == depth_max){
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
  value=Max_depth(bitboard,list_surplus_moves,alpha,beta,depth_max,depth,nodes,pv_current,pv_best,three_moves,description_move,br,for_chek);
//  value=0;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 return value ;
}//	if (Depth == Depth_Max){
//ѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕ

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if NULL_MOVE
// нулевой ход
if (Null_move(bitboard,depth,depth_max,null_m,description_move,for_chek,list_surplus_moves,alpha,beta,nodes,pv_current,pv_best,three_moves,br)==1){
	return beta;
}//if (Null_move(list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),board_list_816,nodes,pv_current,pv_best,description_move,three_moves,br,for_chek_l,null_m)){
  #endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK


// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
list_surplus_moves->current_depth =depth;
//насчитываем список избыточных ходов
// множество этих списков и создают дерево перебора
Generator_captures(bitboard,list_surplus_moves);
Generator_move(bitboard,list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

//
////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//// тестова€ печать списка и доски на нулевом уровне
//  #if TEST_L
//    List_816_Print(depth,list_surplus_moves);
// //   Board_816_Print( depth,board_list_816);
//  #endif 
////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if HASH
// смотрим не встречалась ли нам така€ позици€ если встречалась то ее ход смотрим первым
  Look_hash(list_surplus_moves,bitboard->hash_key,depth,insert_hash);
 #endif

#if KILLER
// если неудача с хешем киллер пишем на первую позицию иначе на вторую
  if(insert_hash==0){
    Insert_killer(list_surplus_moves,depth,list_surplus_moves->start_list[depth]);
  }else{
    Insert_killer(list_surplus_moves,depth,list_surplus_moves->start_list[depth]+1);
  }//if(insert_hash==0){
#endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for (int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){

// реализуем ход из списка другими словами генерируем позицию
 Onward_moves(i,list_surplus_moves,bitboard);
// Bitboard_ini_t(bitboard,board_list_816);

// если король по шахом то
 if (King_check(bitboard)!=0){
// мы отмен€ем этот ход как некорректный
   Backs_moves(i,list_surplus_moves,bitboard);
 }else{
// ставим флаг что есть легальные ходы и засчитываем позицию
 no_moves = 1;
 nodes = nodes+1;

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
 Update_PV_current(i,depth,pv_current,list_surplus_moves);

Modification_up_random_key(i,list_surplus_moves,bitboard);
key=bitboard->hash_key;
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

 depth_max_z= depth_max;

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 #if EXTENSION_CHEK
// если король под шахом то увеличиваем максимальную глубину перебора 
   Extension_for_chek(bitboard,for_chek,depth_max,for_chek_l,depth_max_z);
 #endif
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

#if !NEGA_SCOUT
// рекурсивный вызов функции
 value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
 list_surplus_moves->current_depth =depth;
#endif

 //------------------------------------------------------
 //
#if NEGA_SCOUT
 if (i==list_surplus_moves->start_list[depth]){
// рекурсивный вызов функции
   value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
   list_surplus_moves->current_depth =depth;
 }else{
// рекурсивный вызов функции
   value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-(alpha-1),-(alpha),depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
// value=-1*Alpha_beta_816(list_surplus_moves,-(alpha),-(alpha+1),depth_max_z,(depth+1),board_list_816,nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
  list_surplus_moves->current_depth =depth;
   if ((value < alpha)&&(value > beta)){
 // рекурсивный вызов функции
     value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,list_surplus_moves->description_move[i],three_moves,br,for_chek_l,null_m);
     list_surplus_moves->current_depth =depth;
   }//if ((value < alpha)&&(value < alpha)){
 }//if (i==list_surplus_moves->start_list[depth]){
#endif
//------------------------------------------------------

// восстанавливаем позицию
 Backs_moves(i,list_surplus_moves,bitboard);

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1
 Modification_down_random_key(i,list_surplus_moves,bitboard);

 if (value < value_best){
   value_best = value;
//----------------------------------------------------
//// используем дл€ теста киллера и хеша в полном переборе
//#if KILLER
//// пишем лучшего киллера
//  Save_killer(i,list_surplus_moves,depth);
//#endif
// #if HASH
//// хешируем лучший на данный момент ход
//   Save_best_hash_key(i,list_surplus_moves,bitboard->hash_key,depth);
// #endif
//----------------------------------------------------
// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
   Update_PV_best_point(&pv_best_point,pv_best);
 }//if (value < value_best){

// ЅЋќ  ALPHA BETA ќѕ“»ћ»«ј÷»» 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value < alpha){
   alpha = value;
#if KILLER
// пишем лучшего киллера
  Save_killer(i,list_surplus_moves,depth);
#endif
 #if HASH
// хешируем лучший на данный момент ход
   Save_best_hash_key(i,list_surplus_moves,bitboard->hash_key,depth);
 #endif
 }//if (value < alpha){ 

   if(alpha <= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
//--------------------------------------------------------------------
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){

//ЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌ
// обработка случа€ когда не было легальных ходов

// если не одного хода сделанно не было то производим статическую оценку позиции
// это случай пата или мата
if (no_moves == 0){
  value=No_moves(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//ЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌ

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if !COMPILING_1 // голый полный перебор
// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
 Update_PV_best(pv_best,pv_best_point);
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

return value_best;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
}//int Alpha_beta_816(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// реализуем нулевой ход
//(bitboard,depth,depth_max,null_m,description_move,for_chek,list_surplus_moves,alpha,beta,nodes,pv_current,pv_best,three_moves,br)==1)
int Null_move(T_Bitboard * bitboard,int depth,int depth_max,int null_m,int description_move,int for_chek,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int & br){
int depth_max_z=0;
int  value=0;
int  taking_passage = 0;
int  taking_passage_x = 0;
int  taking_passage_y = 0;

if ( (depth>1)&&((depth_max-depth)>3)&&(null_m==0)&&(list_surplus_moves->end_list[depth-1]>10) ){
// используем эвристику дл€ обычного хода и если не было шахов т.е. глубина не увеличивалась
  if ((description_move == 1)&&(for_chek==0)){
     //cout << "NULL_MOVE " ;
     //cout <<"\n";
// null move
// иде€ нулевого хода в том что если мы пропускаем свой ход т.е. противник делает
// два хода подр€д и не смог сделать ничего ужасного т.е. отсечка по бете проходит
// то значит если мы сделаем ход то тем более противник ничего не сможет сделать
// эта эвристика не работает в позици€х цуцванга
    depth_max_z= depth_max-3;

    // мен€ем цвет хода
   bitboard->colour_move = !bitboard->colour_move;
 if (King_check(bitboard) ==0){
   taking_passage = bitboard->taking_passage_yes;
   taking_passage_x = bitboard->taking_passage_x;
   taking_passage_y = bitboard->taking_passage_y;
   bitboard->taking_passage_yes = 0;

// рекурсивный вызов функции
    value=-1*Alpha_beta_816(bitboard,list_surplus_moves,-beta,-alpha,depth_max_z,(depth+1),nodes,pv_current,pv_best,0,three_moves,br,0,1);  

	// восстанавливаем позицию(цвет хода)
    bitboard->taking_passage_yes = taking_passage;
    bitboard->taking_passage_x = taking_passage_x;
    bitboard->taking_passage_y = taking_passage_y;
 }//if (King_check(bitboard) ==0){
    bitboard->colour_move = !bitboard->colour_move;

	if (value <= beta) return 1; 
  }// if ((description_move==0)&&(for_chek==0)){
 }//if ((depth==4)&&(depth_max>6)){
    return 0; 
}//int Null_move(){
//	*******************************************************************

//	===================================================================
// реализуем продление на шахах
void Extension_for_chek(T_Bitboard * bitboard,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z){
// это флаг что бы увеличивать глубину не дл€ куста а дл€ одной ветки
 if(for_chek==0){
	 for_chek_l =0;
 }else{
     for_chek_l =1;
 }//if(for_chek==0){
// если есть шах то увеличиваем максимальную глубину перебора дл€ этой ветки
// но не более 20 полуходов
bitboard->colour_move = !bitboard->colour_move;
 if(King_check(bitboard)==1){
     if(depth_max<21){
	    depth_max_z=depth_max+1;
        for_chek_l=1;
     }//if(depth_max<21){
 }//if(Check(1,*board_list_816)==1) {
bitboard->colour_move = !bitboard->colour_move;
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************




