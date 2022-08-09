#include <fstream.h>  // TEST
#include <stdlib.h>   // обеспечиваем div

#include "m_Line.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "k_Util.h"
#include "l_Sorting.h"
//-----------------------------------------------------------------------------------
/* ? нужно внимательно все посмотреть
 * запись лучшего варианта
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.1.2008
 *
 *  ТЕСТИРОВАНИЕ
 * тестов пока нет тут надо очень много смотреть
*/
//---------------------------------------------------------------------------


//	===================================================================
// обновляем линию лучшего варианта в узле
void Update_PV_best_point(T_PV * PV_best_point,T_PV * PV_best){
//------------------------------------------------------
// лучший вариант который функция перебора выдает наверх содержится в структуре PV_best
// мы не только присваиваем оценку но и вариант соответствующий ей
// присвоение идет структуре уникальной для каждого узла PV_best_point
// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
    for (int n=0;n<PV_best->depth_max;n++){
      PV_best_point->initial_position [n]=PV_best->initial_position [n] ;
      PV_best_point->final_position [n]=  PV_best->final_position [n] ;
	  PV_best_point->final_pawn [n]=      PV_best->final_pawn[n];
    }//for (int n=0;n<(Depth_Max);n++){
    PV_best_point->score_move =PV_best->score_move;
   // 
   PV_best_point->depth_max =PV_best->depth_max;
//------------------------------------------------------
}//void Update_PV_best_point(T_PV * PV_best_point,T_PV * PV_best){
//	*******************************************************************

//	===================================================================
// обновляем линию лучшего варианта
void Update_PV_best(T_PV * PV_best,T_PV PV_best_point){
//--------------------------------------------
// мы пробежали весь список и теперь в структуре PV_best_point содержится лучшая линия
// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
 for (int n=0;n<PV_best_point.depth_max;n++){
      PV_best->initial_position [n]=PV_best_point.initial_position [n] ;
      PV_best->final_position [n]=PV_best_point.final_position [n] ;
	  PV_best->final_pawn[n]=PV_best_point.final_pawn [n] ;
    }//for (int n=0;n<(Depth_Max);n++){
    PV_best->score_move = PV_best_point.score_move;
 // 
   PV_best->depth_max =PV_best_point.depth_max;
//--------------------------------------------
}//void Update_PV_current(int i,int Depth,T_PV * PV_current,T_list_surplus_moves list_surplus_moves){
//	*******************************************************************

//	===================================================================
// обновляем линию лучшего варианта на максимальной глубине
void Update_PV_best_max_depth(int score,int depth,T_PV * pv_best,T_PV * pv_current){
//--------------------------------------------
// мы достигли предельной глубины и статически оценили получившуюся позицию
// погружаясь на глубину мы на каждом полуходе заносили ходы варианта в структуру PV_current
// теперь мы перепишем его в структуру PV_best и прицепим оценку позиции
// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
// ну и еще конечно длина варианта
// внимание тут цикл идет от 0 до Depth-1
// именно так ведь у нас начальная глубина 0
   for (int n=0;n<depth;n++){
      pv_best->initial_position [n]=pv_current->initial_position [n] ;
      pv_best->final_position [n]=  pv_current->final_position [n] ;
	  pv_best->final_pawn [n]=      pv_current->final_pawn[n];
    }// for (int n=0;n<Depth;n++){
    pv_best->score_move = score;
   // запоминаем глубину варианта
   pv_best->depth_max =depth;
//--------------------------------------------
}//void Update_PV_best_max_depth(int score,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************
//	===================================================================
// записываем ход в текущий вариант
void Update_PV_current(int i,int depth,T_PV * pv_current,const T_list_surplus_moves * list_surplus_moves){
// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
// мы видим что текущий вариант прописывается до рекурсивного вызова функции Alpha_beta_816 т.е. 
// мы разматываем нить погружаясь в глубину 
// запись в структуре PV_current верна сверху и до текущей глубины 
// то что расположенно ниже данного уровня не правильно так как относится к предыдущему узлу.
pv_current->initial_position [depth]=list_surplus_moves->initial_position[i] ;
pv_current->final_position [depth]=list_surplus_moves->final_position[i] ;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// записываем фигуру превращения пешки
if(list_surplus_moves->description_move[i]>5 ){
 if(list_surplus_moves->description_move[i]>15 ){
   pv_current->final_pawn [depth]=list_surplus_moves->description_move[i]-20;
 }else{
   pv_current->final_pawn [depth]=list_surplus_moves->description_move[i]-10;
 }//if(list_surplus_moves->description_move[i]>7 ){
}else{
   pv_current->final_pawn [depth]=0;
}//
}//void Update_PV_current(int i,int Depth,T_PV * PV_current,T_list_surplus_moves list_surplus_moves){
//	*******************************************************************











