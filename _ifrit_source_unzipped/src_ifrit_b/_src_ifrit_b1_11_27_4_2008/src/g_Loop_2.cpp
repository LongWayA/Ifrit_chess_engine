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
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 12.4.2008
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
void Print_cout(int colour_move,T_Bitboard * bitboard,const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int nodes_0, int nodes,int i,int Depth_Max,int value,double  time_nodes){
static int mem =-999999993;
static int mem_depth = 0; 
//static int z=-1;
int inv=0;
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
  colour_move == 0 ? inv=-1 : inv=1;
 if(inv*value>900000){
    cout << " score mate "<< (PV_best_point.depth_max-1);// дистанция до мата в полуходах
 } else if(inv*value<-900000){
    cout << " score mate "<< -(PV_best_point.depth_max-1);// дистанция до мата в полуходах
 }else{
 //   cout << " score cp "<<z*(PV_best_point.score_move);// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
	 cout << " score cp "<< inv*value;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
 }//if(value>900000){

// печатаем информацию о количестве просмотренных узлов
 cout << " nodes "<<nodes;

// если оценка в печатаемом варианте не совпадает с оценкой переданной по рекурсии
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
     cout << " PV_best_point.score_move = "<<PV_best_point.score_move;
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
// реализуем продление на шахах
void Extension_for_chek_white(T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check){

// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
// но не более 20 полуходов
check=0;
 if(King_black_check(bitboard)!=0){
 check=1;
     if(depth_max<21){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(King_check(bitboard)==1){
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************

//	===================================================================
// реализуем продление на шахах
void Extension_for_chek_black(T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check){

// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
// но не более 20 полуходов
check=0;
 if(King_white_check(bitboard)!=0){
     check=1;
     if(depth_max<21){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(King_check(bitboard)==1){
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************

//	===================================================================
// реализуем продление на взятиях
void Extension_for_captures(int i,int start,int description_move,int description_move_up,int depth_max,int & depth_max_new,int depth){

// если есть взятие то увеличиваем максимальную глубину перебора для этой ветки
// но не более 10 полуходов
 if((description_move == 2)&&(description_move_up == 2)){
// if(i == start){
     if(depth < 4){
 //    if((depth<5)&&(depth_max<15)){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(description_move == 2){
}//void Extension_for_captures(int description_move,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z){
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





