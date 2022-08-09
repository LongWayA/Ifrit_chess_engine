#include <iostream.h>
#include <fstream.h>
#include <cstring>      // ���������� ��� ������ �� ������� (��������� �����)
#include <stdlib.h>

#include "c_Go.h"
#include "g_Loop.h"
#include "b_FEN.h"
#include "c_Iterative_Deepening.h"
#include "j_Hash.h"
//-----------------------------------------------------------------------------------
/*+
 * ������������ ������� �� �����
 *
 * ����� ChessTerminator75
 * ��������� ��������� 17.11.2007
 *
 *  ������������
 * ���������������� ������ ���
 * ��� ��� ����� �������� ����� ��������� � ������ d_Loop
*/
//	===================================================================
// ����������
char string_in_l[65536];

//	===================================================================
//+
// �������� ��������� � ������ ����� ��� ����������� ������� �����
// ���������: ������,������
void parse_three_position(const char string_in[],char three_position[]){
 char * moves;
 char * ptr;
 int i=0;

// ������� ����� 65536
  for(int b = 0; b<65536 ; b++){ 
         three_position[b] = 0;
  }// for(int b = 0; b<65536 ; b++) { 

// ������ ��������� �� moves
 moves = strstr(string_in,"moves ");
 if (moves != NULL) {
 // ����������� moves 
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
// ����������� ������ � ������
// ���������: ������ ������������� ����(���������),��������� �������������� ����(��� ���������� �����)
// � ��������� three_moves �� �������� ��������� 4 ���� �� ���������� �������������
// ��������� ���� � 0 �.�. 0,1,2,3
void transformation_moves(char three_position[],T_PV * three_moves){
int i=0;// ���������� �������� � ������
int n=0;// ������ ��������� 6 ��������� ��� ������
int k=0;// �������� ���������� ���������

// ��� ���������� ��������� 6 ���������
// �� �� � �������� �� ���������� �������������

// ������������ ������ �� ����� ������������ �������
  while (three_position[i] != '\0') { 
    i=i+1;
  }// while (three_position[i] != '\0') { 
// 01234567890123456789
// g1-f3 g8-f6 f3-g1 f6-g8 g1-f3 g8-f6
// 0  1  2  3  4  5  6 
// ����������� 6 ��������� ����� ���� � ������ ������� ����
n= i-19;
// ���� ���� �� ���� � ������
if (i<20) n=0;

// �������� ��������(����� ���� ��� �����)  
 for ( int q =0;q<5;q++){ 
   three_moves->initial_position[q]= q+1;
   three_moves->final_position[q]  = q+1;
   three_moves->depth_max =q;
  }//for ( q =0;q<7;q++){ 

//cout<<"i=" <<i << "\n";
//cout<<"n=" <<n << "\n";
// ��������� �������� �� ���������� ���������� �������������(������� ���������� �����)
  while ( n < i ) { 
// to = 8 * bitboard->taking_passage_y + bitboard->taking_passage_x;
// ������ ���� �����(�.�. �) � ����� ����� (�.�. �)
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
������� ��� ����� ���� 5 ��� ���� 5 ���+���������� �� ��� � ��������
> go btime 300000 wtime 300000 
for game in 5min. 
> go btime 302000 wtime 302000 winc 2000 binc 2000 
with incremental clock 5 min + 2 sec. 

����� �� ������������ ���������� �����
> go movestogo 40 wtime 300000 btime 300000 
for 40 moves in 5 min. 
������ ����� ������ � ����
> go wtime 300000 btime 300000  movestogo 40

��� ������� ��� ��� �������
> go infinite 
for analysing. 

����� �� ��� ������� ��� ���� �� ������������ � �� ����� ��� �� ������ ���� ���������
> go movetime 300000 
Exactly 5min for the next move, not supported by Fritz. 

*/
// ��������� ������� go � ������������� ������� ��������
// ������� ���� > go depth 5 
// ���������: 
void parse_go(T_Bitboard * bitboard,const char string_in[],char three_position[],int & nodes){

const char * ptr;
int depth = -1;
T_PV three_moves;//������ ����� � ������� ���� ������� 
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
// ������� strtok �������� ��� ����������� ������
// ��� ������ ������ ������ ������ � ����� ��� ��� ���������� ���� 
ptr = strtok(string_in_l," "); // ���������� "go"
 
// ��� �� ����� �� ������ ������� ��������� ���������� ���������
for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {

   if (false) {

      } else if (string_equal(ptr,"depth")) {//
        ptr = strtok(NULL," ");
        depth = atoi(ptr);// ������� atoi ��������� ������ � ����� �����

      } else if (string_equal(ptr,"wtime")) {// ����� ������� ����������� �����
		ptr = strtok(NULL," ");
        wtime = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"btime")) {// ����� ������� ����������� ������
        ptr = strtok(NULL," ");
        btime = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"winc")) {// ���������� ����� ��� �����
        ptr = strtok(NULL," ");
        winc = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"binc")) {// ���������� ����� ��� ������
        ptr = strtok(NULL," ");
        binc = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"movestogo")) {//���������� ����� �� ������� �������� �����
        ptr = strtok(NULL," ");
        movestogo = atoi(ptr);

      } else if (string_equal(ptr,"movetime")) {// ����� �� ���
        ptr = strtok(NULL," ");
        movetime = double(atoi(ptr)) / 1000.0;

      } else if (string_equal(ptr,"infinite")) {// ����������� ������
        infinite = true;

// ���� ������ �� ���� ----------------------------------------------
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

// ����������� ������ � ������ � �� ���������� �������������(����� ��� ����������� ������������ ���������� �����) 
 transformation_moves(three_position,&three_moves);
// cout<< "STROKA= "<<three_position<<"\n";
nodes=0;
// if (depth==-1) depth =6;
//ptr = NULL;
Start_position_random_key(bitboard);
Ini_hash_moves();

// �������� ������� � ������� �������� chess_board,Depth_Max
 Deepening(bitboard,depth,three_position,three_moves,nodes,wtime,btime,winc,binc,movestogo,movetime,infinite);

// �������� ������� � ������� �������� chess_board,Depth_Max
// Loop(&list_surplus_moves,board_list_816,depth,three_position,&three_moves,nodes);
}//parse_go(char string_in[],int chess_board[]){
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������� ���������
static bool string_equal(const char s1[], const char s2[]) {
//������� ���������� ����� �� ������ ������ ������� ������������?
   return strcmp(s1,s2) == 0;
}
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������ ������� ���������
static bool string_start_with(const char s1[], const char s2[]) {
// ������ ��������� �� ������ ��������� ������ ������ � ������
// �� � ���� ��� ��������� ������ �� ����� �� ��� ����� � ����� ������
   return strstr(s1,s2) == s1;
}
//	*******************************************************************
