#include <fstream.h>  // TEST
#include <stdlib.h>   // обеспечиваем div

#include "l_Sorting.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "k_Util.h"
//-----------------------------------------------------------------------------------
/* ? нужно внимательно все посмотреть
 * сортировка ходов
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.1.2008
 *
 *  ТЕСТИРОВАНИЕ
 * тестов пока нет тут надо очень много смотреть
*/
//---------------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_Sor
 ofstream Test_Loop_s;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// сортируем взятия возвращаем количество взятий
int Sorting_captures_moves(T_list_surplus_moves * list_surplus_moves){
 // 
int move_s;
int m;

m=list_surplus_moves->end_captures;


 for (int n=0;n<m;n++){	
   for (int i=n+1;i<m;i++){
 //    if((100*list_surplus_moves->name_taking_figure[n] - list_surplus_moves->name_figure[n])<(100*list_surplus_moves->name_taking_figure[i] - list_surplus_moves->name_figure[i])){ 
    if((100*( list_surplus_moves->move[n] & 63) - ((list_surplus_moves->move[n]>>24)& 7))<(100*( list_surplus_moves->move[i] & 63) - ((list_surplus_moves->move[i]>>24)& 7))){ 
//     if(material[list_surplus_moves->name_figure[n]]>material[list_surplus_moves->name_figure[i]]){ 
// сохраняем данные из n     
	 move_s = list_surplus_moves->move[n];

// копируем из i в n
	 list_surplus_moves->move[n] = list_surplus_moves->move[i];


// восстанавливаем запись
	 list_surplus_moves->move[i] = move_s;
	 }//     if(material[list_surplus_moves->name_figure[n]]>material[list_surplus_moves->name_figure[i]]){ 
   }//for (int i=0;i<m;i++){
 }//for (int n=0;n<m;n++){	
return n;
}//void Sorting_surplus_moves(T_list_surplus_moves list_surplus_moves,T_list_surplus_moves * sort_list_surplus_moves){
//	*******************************************************************













