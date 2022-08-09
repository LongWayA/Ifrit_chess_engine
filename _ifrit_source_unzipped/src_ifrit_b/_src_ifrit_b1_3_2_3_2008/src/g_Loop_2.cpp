#include <iostream.h> // TEST
#include <fstream.h>  // TEST
#include <cstring>    // используем для работы со строкой (сравнение строк)
#include <stdlib.h>   // обеспечиваем завершение потока функцией exit(1)


#include "k_Util.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "i_Forced.h"
#include "k_Win_32.h"
#include "g_Loop.h"
#include "j_Hash.h"
#include "m_Line.h"
#include "d_Bitboard_gen.h"
//-----------------------------------------------------------------------------------
/*
 * перебор позиций
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.02.2008
 *
 *  ТЕСТИРОВАНИЕ
 * тестовую информащию выводим в файл _TEST_LOOP.txt
 * печатаем
 * тестируем модули 
 * целью теста является
 *
 * ВОЗМОЖНЫЕ ИНДУКТОРЫ ОШИБКИ
 * тестировать и описывать тут очень много. надо будет все тщательно продумать
 * по сути это сердце программы и именно тут сходятся все ниточки
 * нужно контролировать:
 * 1 генератор позиций - правильно ли генерируются всевозможные позиции не пропущенно ли что
 * 2 линия варианта - правильно ли она печатается нет ли тут искажений
 * правильно ли печатается оценка и глубина варианта а также количество позиций
 * 3 альфа-бета отсечение - правильно ли оно работает нет ли где недостаточных
 *   или избыточных отсечений 
*/
//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
 ofstream Test_Loop;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// глобальные переменные
int audit=0;// задержка опроса команды экстренного выхода

//	===================================================================
// если нет легальных ходов то это мат или пат
int No_moves(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current){
bitboard->colour_move = !bitboard->colour_move;
int score_m=0; 
if(King_check(bitboard)!=0){
// почему тут всегда один знак для меня непостижимо
// но так это работает а по другому не работает
  score_m = 1000000-1000*depth;
}//if(Shah(n_king,*board_list_816 )==1){

// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
Update_PV_best_max_depth(score_m,depth,pv_best,pv_current);
 //cout <<"Вызов нет ходов ";
 //cout <<"\n";
bitboard->colour_move = !bitboard->colour_move;
return score_m ;
}//int No_moves(int Depth,T_board_list_816 * board_list_816,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// производим расчеты на максимальной глубине
int Max_depth(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int description_move,int & br,int for_chek){
//int score_m=0;
int score=0;
int score_f=0;
int inv=0;          // инвертирование оценки позиции
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// обработка команд экстренного завершения счета 
if (audit>50000){
//if (0){
//если поступила команда то это или quit или stop
 if (Bioskey()){
// if (0){
char string_in2[65536];
cin.getline(string_in2,65536,'\n');
 if (strcmp(string_in2,"quit")== 0) exit(1);
 if ((strcmp(string_in2,"stop")== 0)){
// делаем ход
  br=1;
  }//if ((strcmp(string_in2,"stop")== 0)){
 }//if (Bioskey()){
 audit=0;
}//if (audit>100000){
audit=audit+1;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//------------------------------------------------------
// распознаем повторение ходов
// конечно это недостаточное условие но думаю пока сойдет
// (0)b1c3 (1)... (2)c3b1 (3)...(4)b1c3 (5)..
	if((pv_current->initial_position[0]==three_moves->final_position [2])&&(pv_current->final_position[0]==three_moves->initial_position[2])     &&(three_moves->initial_position[3]==three_moves->final_position[1])&&(three_moves->final_position [3]==three_moves->initial_position [1])){
//    if(0){
      score = 0;
// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
      Update_PV_best_max_depth(score,depth,pv_best,pv_current); 
      return score ;
	}else{
// форсировка
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// подключаем форсированный вариант
// мы уже погрузились на полуход так что альфа бета и глубина остаются те же
// инверсия оценки тут тоже будет лишней
 #if FORCED // включаем режим форсировки
//   score=Forced_variant_816(list_surplus_moves,alpha,beta,depth,board_list_816,nodes,pv_current,pv_best);
//   score=Forced_variant_816(list_surplus_moves,9999999,-9999999,depth,board_list_816,nodes,pv_current,pv_best);

// позиционную оценку мы считаем в конце основного перебора
    bitboard->colour_move == 1 ? inv=-1 : inv=1;
    score = inv*Eval_State(bitboard);
// материал мы считаем в конце форсированного варианта
// корректируем (alpha-score),(beta-score) что бы корректно обрабатывать отсечения ведь дальше считаем только материал
// Bitboard_print(bitboard);
	score_f=Forced_variant_816(bitboard,list_surplus_moves,(alpha-score),(beta-score),depth,pv_current,pv_best);
// таким образом мы получаем комбинированную оценку в которой позиционные факторы учитываются
// сразу а материал только после форсированных разменов
    score = score + score_f;
    pv_best->score_move=-score;
// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
// надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
//   Update_PV_best_max_depth(score,depth,pv_best,pv_current);
    return score ;
 #endif 
 #if !FORCED // включаем оценку без форсировки
   bitboard->colour_move == 1 ? inv=-1 : inv=1;
   score = inv*Eval_State(bitboard)+inv*Eval_forced(bitboard);
// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
// надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
   Update_PV_best_max_depth(score,depth,pv_best,pv_current);

// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
// именно здесь можно проверить правильно ли рисуется вариант на предельной глубине
// кроме  случая шаха и пата
//   Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
   return score ;
 #endif 
// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
 }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

//// 0 оцениваем данную позицию
//// наш статический оценщик выдает абсолютную оценку позиции. это значит что при
//// преимуществе белых оценка положительна а при приемуществе черных оценка отрицательна
//// если ход белых то оценку инвентируем потому что мы оцениваем уже схоженную позицию.
//// это что касается почему инверсия именно при белом цвете
//// а вообще инверсия нам нужна потому что мы используем негамакс
//      board_list_816->colour_move == 1 ? inv=-1 : inv=1;
//      score = inv*Eval_State_816(*board_list_816);
//// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
//      Update_PV_best_max_depth(score,Depth,PV_best,PV_current); 
//      return score ;
//    }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

}//int Max_depth(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_current,T_PV * PV_best,T_PV * three_moves){
//	*******************************************************************

//	===================================================================
// выводим текущий вариант в оболочку
void Print_cout_v(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value){
// расчитываемый в момент печати ход
 cout << "info currmove " ;
 cout <<x_to_notation(list_surplus_moves->initial_position[i]%8);
 cout <<(1 + list_surplus_moves->initial_position[i]/8);
 cout <<x_to_notation(list_surplus_moves->final_position[i]%8);
 cout <<(1 + list_surplus_moves->final_position[i]/8);
// порядковый номер расчитываемого хода
 cout << " currmovenumber " ;
 cout << Nodes_0;
 //cout <<"\n";
// печатаем информацию о глубине просмотра
 cout << " depth "<<Depth_Max;
 //if(Nodes_0==1){
 //   cout << " seldepth "<<Depth_Max;
 //}else{
 //   cout << " seldepth "<<PV_best_point.depth_max;
 //}//if(Nodes_0==1){
 cout <<"\n";
 cout.flush();
}//Print_cout_v(c

//	===================================================================
// выводим текущий вариант в оболочку
void Print_cout(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int nodes_0, int nodes,int i,int Depth_Max,int value,double  time_nodes){
static int mem =-999999993;
static int mem_depth = 0; 
static int z=-1;
/*
// Here are examples of the possible info output: 
< info currmove e8e7 (current calculated move) +
< info currmovenumber 29 +
< info nps 291506 (nodes per sec, Fritz calculates the nps itself)
< info tbhits 830
< info depth 9
< info score cp12 (in centipawns)
< info score mate 5 (or mate -5)
< info nodes 288973
< info time 11192 (milliseconds)
< info pv e2e4 e7e5 g1f3 b8c6 f1d3 f8c5 e1g1 g8e7 b1c3 
*/



// расчитываемый в момент печати ход

// cout << "info currmove " ;
// cout <<x_to_notation(list_surplus_moves->initial_position[i]%16);
// cout <<(8-list_surplus_moves->initial_position[i]/16);
// cout <<x_to_notation(list_surplus_moves->final_position[i]%16);
// cout <<(8-list_surplus_moves->final_position[i]/16);
//// порядковый номер расчитываемого хода
// cout << " currmovenumber " ;
// cout << Nodes_0;
// //cout <<"\n";
//// печатаем информацию о глубине просмотра
// cout << " depth "<<Depth_Max;

// 
// меняем знак в зависимости от глубины(четная она или нет)
z=1;
//if (div(PV_best_point.depth_max,2).rem == 0) z=-1;
//if (div(Depth_Max,2).rem == 0) z=1;
if ( value >= 0) z=-1;

if (mem_depth != Depth_Max) mem =-999999993;
//   if(mem==PV_best_point.score_move) cout <<"\n";
   if(mem==value) cout <<"\n";

// выводим вариант только в том случае если лучшая оценка а значит и лучший
// варинат изменились что бы не было вывода одного и того же сто раз 
#if TEST_PRINT
   if(1){
#else

//   if(mem!=PV_best_point.score_move){
   if(mem!=value){
//   mem =PV_best_point.score_move;
   mem =value;
   mem_depth =Depth_Max;
#endif

 cout << "info " ;
 cout << " nps "<<nodes/time_nodes;
 cout << " depth "<<Depth_Max;
// печатаем информацию о глубине форсировки
 cout << " seldepth "<<PV_best_point.depth_max;

// печатаем информацию о оценке позиции(это лучшая на данный момент оценка связанна с лучшим вариантом)
 if(PV_best_point.score_move>900000){
    cout << " score mate "<< z*(PV_best_point.depth_max-1);// дистанция до мата в полуходах
 }else{
 //   cout << " score cp "<<z*(PV_best_point.score_move);// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
    cout << " score cp "<< -value;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
 }//if(PV_best_point.score_move>900000){
// печатаем информацию о количестве просмотренных узлов
 cout << " nodes "<<nodes;

// если оценка в печатаемом варианте не совподает с оценкой переданной по рекурсии
// то бракуем линию
//if( value != (-z*PV_best_point.score_move) ) {
//if( value !=(-z*PV_best_point.score_move) ) {
// сравниваю по модулю потому что из за продлений на шахах запарки со знаком
// пришлось бы поднимать текущую максимальную глубину
if( (value*value) !=(PV_best_point.score_move*PV_best_point.score_move) ) {
//if(0) {
     cout <<"\n";
	 cout << "ERROR LINE " ;
     cout <<"\n";
     cout << " value = "<<value;
     cout <<"\n";
     cout << " -z*PV_best_point.score_move = "<<-z*PV_best_point.score_move;
     cout << " PV_best_point.score_move = "<<PV_best_point.score_move;
     cout <<"\n";
     cout <<"**********************"<<"\n";
     cout <<"  "<<"\n";
}else{//if( value != (-z*PV_best_point.score_move) ) {


// вывод линии варианта
 cout <<" pv ";
 for (int n=0;n<PV_best_point.depth_max;n++){
 cout <<x_to_notation(PV_best_point.initial_position[n]%8);
 cout <<(1 + PV_best_point.initial_position[n]/8);
 cout <<x_to_notation(PV_best_point.final_position[n]%8);
 cout <<(1 + PV_best_point.final_position[n]/8); 
 if (PV_best_point.final_pawn[n]!=0) cout <<int_to_char_board(PV_best_point.final_pawn[n],0);
 cout <<" ";
}//for (int n=1;n<(Depth_Max);n++){ 
 }//if(mem!=PV_best_point.score_move){
 cout <<"\n";
}//if( value != (-z*PV_best_point.score_move) ) {
 cout.flush();
}//Print_cout(T_list_surplus_moves list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i){
//	*******************************************************************


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// максимальная глубина,глубина перебора,доска,
#if TEST_L
void List_816_Print(int depth,const T_list_surplus_moves * list_surplus_moves){

Test_Loop.open ("_TEST_LOOP.txt",ios::app);	
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop << "\n";
Test_Loop << "------------------------------------------------- "<<"\n";
Test_Loop <<"depth= "<< depth << "\n";
Test_Loop << "\n";
Test_Loop << "Список возможных ходов фигуры "<<"\n";
for (int i=list_surplus_moves->start_list[depth];i<list_surplus_moves->end_list[depth];i++){	
     Test_Loop <<"i= "<<i<< "\n";  
     Test_Loop <<"вид хода "<<list_surplus_moves->description_move[i]<< "\n";
	 Test_Loop <<"начальная положение "<<list_surplus_moves->initial_position[i]<< "\n";
	 Test_Loop <<"конечное положение "<<list_surplus_moves->final_position[i]<< "\n";
     Test_Loop <<"Взятая фигура "<<int_to_char_board(list_surplus_moves->name_taking_figure[i],1)<< "\n";
	Test_Loop <<" "<<(x_to_notation(list_surplus_moves->initial_position[i]%8));
Test_Loop <<""<<(1 + list_surplus_moves->initial_position[i]/8);//<< "\n";
 //    Test_Loop<< "\n";
Test_Loop <<" "<<(x_to_notation(list_surplus_moves->final_position[i]%8));
Test_Loop <<""<<(1 + list_surplus_moves->final_position[i]/8)<< "\n";     
	 Test_Loop << "\n";
}//for (i=1,i<list_surplus_moves->amount_moves+1){

Test_Loop <<"list_surplus_moves->start_list= "<<list_surplus_moves->start_list[depth]<< "\n";
Test_Loop <<"list_surplus_moves->end_list= "<<list_surplus_moves->end_list[depth]<< "\n";
Test_Loop << "------------------------------------------------- "<<"\n";
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop.close();
}//void List_816_Print(int Depth,T_list_surplus_moves list_surplus_moves;){
#endif
//	*******************************************************************





