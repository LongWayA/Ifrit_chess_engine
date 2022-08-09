#include <iostream.h>
#include <fstream.h>
#include <cstring>      // используем для работы со строкой (сравнение строк)
#include <stdlib.h>

#include "c_Go.h"
#include "g_Loop.h"
#include "b_FEN.h"
#include "c_Iterative_Deepening.h"
#include "j_Hash.h"
//-----------------------------------------------------------------------------------
/*+
 * обрабатываем команду на старт
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 17.11.2007
 *
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
 * все что здесь проходит можно проверить в модуле d_Loop
*/
//	===================================================================
// глобальная
char string_in_l[65536];

//	===================================================================
//+
// выделяем подстроку с ходами нужно для определения повтора ходов
// аргументы: строка,строка
void parse_three_position(const char string_in[],char three_position[]){
 char * moves;
 char * ptr;
 int i=0;

// очищаем буфер 65536
  for(int b = 0; b<65536 ; b++){ 
         three_position[b] = 0;
  }// for(int b = 0; b<65536 ; b++) { 

// выдает указатель на moves
 moves = strstr(string_in,"moves ");
 if (moves != NULL) {
 // проматываем moves 
 ptr = moves + 6;
  i=0;
  while (*ptr != '\0') { 
         three_position[i] = *ptr++;
		 i=i+1;
  }//while (*ptr != '\0') {
 }//if (moves != NULL) {

}//void parse_three_position(char string_in[],char three_position[]){
//	*******************************************************************

//	===================================================================
// преобразуем строку с ходами
// аргументы: обычно представленые ходы(аннотация),внутренне представленные ходы(для одномерной доски)
// в структуре three_moves мы получаем последние 4 хода во внутреннем представлении
// нумерация идет с 0 т.е. 0,1,2,3
void transformation_moves(char three_position[],T_PV * three_moves){
int i=0;// количество символов в строке
int n=0;// начало последних 6 полуходов или меньше
int k=0;// реальное количество полуходов

// нас интересуют последние 6 полуходов
// их мы и загоняем во внутреннее представление

// прокручиваем строку до конца пересчитывая символы
  while (three_position[i] != '\0') { 
    i=i+1;
  }// while (three_position[i] != '\0') { 
// 01234567890123456789
// g1-f3 g8-f6 f3-g1 f6-g8 g1-f3 g8-f6
// 0  1  2  3  4  5  6 
// отсчитываем 6 полуходов назад если в строке столько есть
n= i-19;
// если нету то идет в начало
if (i<20) n=0;

// нумеруем полуходы(нужно было для теста)  
 for ( int q =0;q<5;q++){ 
   three_moves->initial_position[q]= q+1;
   three_moves->final_position[q]  = q+1;
   three_moves->depth_max =q;
  }//for ( q =0;q<7;q++){ 

//cout<<"i=" <<i << "\n";
//cout<<"n=" <<n << "\n";
// переводим полуходы во внутреннее програмное представление(индексы одномерной доски)
  while ( n < i ) { 
// to = 8 * bitboard->taking_passage_y + bitboard->taking_passage_x;
// первой идет цифра(т.е. у) а потом буква (т.е. х)
   three_moves->initial_position[k]= 8*Ch_Koordinata_To_Indeks(three_position[n+1]) + Koordinata_To_Indeks(three_position[n]);
   three_moves->final_position[k]  = 8*Ch_Koordinata_To_Indeks(three_position[n+3]) + Koordinata_To_Indeks(three_position[n+2]);
   three_moves->depth_max =k;

//-------------------------------------
//cout<< "three_position[n+1]= "<<three_position[n+1]<< " three_position[n]= "<< three_position[n]<<"\n";
//cout<< "three_position[n+3]= "<<three_position[n+3]<< " three_position[n+2]= "<< three_position[n+2]<<"\n";
//    
//cout<<"k=" <<k << "\n";
//cout<< "initial= "<<three_moves->initial_position[k]<< " final= "<<three_moves->final_position[k]<<"\n";
//-------------------------------------

   k=k+1;
   n=n+5;
  }//while ( n < i ) { 


}//void transformation_moves(char three_position[],T_PV three_moves){

//	*******************************************************************

//	===================================================================
/*
команды для блица типа 5 мин либо 5 мин+добавление за ход в секундах
> go btime 300000 wtime 300000 
for game in 5min. 
> go btime 302000 wtime 302000 winc 2000 binc 2000 
with incremental clock 5 min + 2 sec. 

время на определенное количество ходов
> go movestogo 40 wtime 300000 btime 300000 
for 40 moves in 5 min. 
причем арена выдает в виде
> go wtime 300000 btime 300000  movestogo 40

это понятно что для анализа
> go infinite 
for analysing. 

время на ход говорят что фриц не поддерживает а по моему все он держит надо проверять
> go movetime 300000 
Exactly 5min for the next move, not supported by Fritz. 

*/
// разбираем команду go и устанавливаем глубину перебора
// команда типа > go depth 5 
// аргументы: 
void parse_go(T_Bitboard * bitboard,const char string_in[],char three_position[],int & nodes){

const char * ptr;
int depth = -1;
T_PV three_moves;//строка ходов в которой ищем повторы 
double wtime= -1.0;
double btime= -1.0; 
double winc= -1.0;
double binc= -1.0;
int movestogo= -1;
double movetime= -1.0; 
bool infinite= false;
int mate= -1 ;
//char string_in_l[20];
strcpy (string_in_l,string_in);
// функция strtok работает как разбиватель строки
// при первом вызове задаем строку а потом она уже продолжает сама 
ptr = strtok(string_in_l," "); // пропускаем "go"
 
// тут мы бежим по строке выделяя фрагменты отделенные пробелами
for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {

   if (false) {

      } else if (string_equal(ptr,"depth")) {//
        ptr = strtok(NULL," ");
        depth = atoi(ptr);// функция atoi переводит строку в целое число

      } else if (string_equal(ptr,"wtime")) {// время которым распологают белые
		ptr = strtok(NULL," ");
        wtime = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"btime")) {// время которым распологают черные
        ptr = strtok(NULL," ");
        btime = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"winc")) {// добавочное время для белых
        ptr = strtok(NULL," ");
        winc = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"binc")) {// добавочное время для черных
        ptr = strtok(NULL," ");
        binc = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"movestogo")) {//количество ходов на которое выделено время
        ptr = strtok(NULL," ");
        movestogo = atoi(ptr);

      } else if (string_equal(ptr,"movetime")) {// время на ход
        ptr = strtok(NULL," ");
        movetime = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"infinite")) {// бесконечный анализ
        infinite = true;

// пока делать не буду ----------------------------------------------
//      } else if (string_equal(ptr,"mate")) {
//         ptr = strtok(NULL," ");
//         mate = atoi(ptr);
//      } else if (string_equal(ptr,"nodes")) {

//         ptr = strtok(NULL," ");
//         nodes = my_atoll(ptr);

//      } else if (string_equal(ptr,"ponder")) {
//         ponder = true;

//      } else if (string_equal(ptr,"searchmoves")) {
//-------------------------------------------------------------------

     }//   if (false) {

}//for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {

//------------------------------------------------------------------------------

// преобразуем строку с ходами в во внутреннее представление(нужно для определения трехкратного повторения ходов) 
 transformation_moves(three_position,&three_moves);
// cout<< "STROKA= "<<three_position<<"\n";
nodes=0;
// if (depth==-1) depth =6;
//ptr = NULL;
Start_position_random_key(bitboard);
Ini_hash_moves();

// получаем позицию и глубину перебора chess_board,Depth_Max
 Deepening(bitboard,depth,three_position,three_moves,nodes,wtime,btime,winc,binc,movestogo,movetime,infinite);

// получаем позицию и глубину перебора chess_board,Depth_Max
// Loop(&list_surplus_moves,board_list_816,depth,three_position,&three_moves,nodes);
}//parse_go(char string_in[],int chess_board[]){
//	*******************************************************************

//	===================================================================
// есть ли в строке искомая подстрока
static bool string_equal(const char s1[], const char s2[]) {
//функция показывает равны ли строки видимо уровень соответствия?
   return strcmp(s1,s2) == 0;
}
//	*******************************************************************

//	===================================================================
// есть ли в начале строки искомая подстрока
static bool string_start_with(const char s1[], const char s2[]) {
// выдает указатель на первое вхождение второй строки в первую
// ну и если они совпадают значит мы имеем то что нужно в самом начале
   return strstr(s1,s2) == s1;
}
//	*******************************************************************
