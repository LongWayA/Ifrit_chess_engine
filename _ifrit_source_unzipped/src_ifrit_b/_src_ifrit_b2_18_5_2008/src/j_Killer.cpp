#include <fstream.h>  // TEST
#include <stdlib.h>


#include "j_Killer.h"
#include "k_Util.h"

//-----------------------------------------------------------------------------------
/* ? нужно внимательно все посмотреть
 * реализуем киллеры
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 13.1.2008
 *
 *  ТЕСТИРОВАНИЕ
 * тестов пока нет тут надо очень много смотреть
*/
//---------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


// глобальные переменные
 T_killer_moves killer_moves[100]={0};


//	===================================================================
//
void Save_killer(int i,T_list_surplus_moves * list_surplus_moves,int depth){

 killer_moves[depth].initial_position =((list_surplus_moves->move[i]>>12)& 63);
 killer_moves[depth].final_position   =((list_surplus_moves->move[i]>>18)& 63);

}//void Save_killer(int i,T_list_surplus_moves * list_surplus_moves,int depth){
//	*******************************************************************

//	===================================================================
void Insert_killer(T_list_surplus_moves * list_surplus_moves,int depth,int start){
 // 
int move_s=0;


// 1 ищем киллер ход в списке
//for (int j=start;(!((list_surplus_moves->initial_position[j]==killer_moves[depth].initial_position)&&(list_surplus_moves->final_position[j]==killer_moves[depth].final_position))&&(j<list_surplus_moves->end_list[depth]));j++);
for (int j=start;(!((((list_surplus_moves->move[j]>>12)& 63)==killer_moves[depth].initial_position)&&(((list_surplus_moves->move[j]>>18)& 63)==killer_moves[depth].final_position))&&(j<list_surplus_moves->end_list));j++);	
// если такой ход нашелся то идем дальше
if (j<list_surplus_moves->end_list){	
//------------------------------------------------------------------------
// 2 запоминаем киллер ход 
	 move_s  = list_surplus_moves->move[j];
//------------------------------------------------------------------------

// 3 сдвигаем все строчки вниз
 for (int i=j;i>start;i--){	
 	 list_surplus_moves->move[i] = list_surplus_moves->move[i-1];

}//for (int i=j;i>start;i--){	

//------------------------------------------------------------------------
// 4 на стартовую позицию записываем киллера
	 list_surplus_moves->move[start] = move_s;
//------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
    Test_Killer.open ("_TEST_KILLER.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if TEST_K
 Test_Killer << "записываем киллера"<< "\n";
 Test_Killer << "start= "<< start << "\n";
 Test_Killer << "j= "<< j << "\n";
 Test_Killer << "i= "<< i << "\n";
 Test_Killer << "i_end= "<< i_end << "\n";
 Test_Killer << "depth= "<< depth << "\n";
 Test_Killer << "list_surplus_moves->end_list[depth]= "<< list_surplus_moves->end_list[depth] << "\n";
 Test_Killer << "killer_moves[depth].initial_position= "<< killer_moves[depth].initial_position << "\n";
 Test_Killer << "killer_moves[depth].final_position= "<< killer_moves[depth].final_position << "\n";
 Test_Killer << "                "<< "\n";
 Test_Killer <<" "<<(x_to_notation(killer_moves[depth].initial_position%16));
 Test_Killer <<""<<(8-killer_moves[depth].initial_position/16);
 Test_Killer <<" "<<(x_to_notation(killer_moves[depth].final_position%16));
 Test_Killer <<""<<(8-killer_moves[depth].final_position/16) << "\n";
#endif 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
    Test_Killer.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//if (j<list_surplus_moves->end_list[depth]){	

}//Insert_hash_moves(T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,int depth){
//	*******************************************************************