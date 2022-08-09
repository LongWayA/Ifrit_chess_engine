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
 // имя фигуры 1- пешка 2- ладья 3- конь 4- слон 5- ферзь 6- король 
int name_figure_s=0;
 // имя взятой фигуры 1- пешка 2- ладья 3- конь 4- слон 5- ферзь 6- король 
int name_taking_figure_s=0;
// начальное положение фигуры(откуда фигура ходит)
int initial_position_s=0; 
// конечное положение фигуры(куда фигура ходит)
int final_position_s=0;
// является ли ход ходом 0, взятием 1, взятием на проходе 2, рокировкой 3,
// превращением 4, превращением с взятием 5
int description_move_s=0;
int m=0;
int depth=0;

depth=list_surplus_moves->current_depth;
m=list_surplus_moves->end_captures[depth];


 for (int n=list_surplus_moves->start_list[depth];n<m;n++){	
   for (int i=n+1;i<m;i++){
     if((100*list_surplus_moves->name_taking_figure[n] - list_surplus_moves->name_figure[n])<(100*list_surplus_moves->name_taking_figure[i] - list_surplus_moves->name_figure[i])){ 
//     if(material[list_surplus_moves->name_figure[n]]>material[list_surplus_moves->name_figure[i]]){ 
// сохраняем данные из n     
	 name_figure_s        = list_surplus_moves->name_figure[n];
	 name_taking_figure_s = list_surplus_moves->name_taking_figure[n];
     initial_position_s   = list_surplus_moves->initial_position[n];
     final_position_s     = list_surplus_moves->final_position[n];
     description_move_s   = list_surplus_moves->description_move[n];

// копируем из i в n
	 list_surplus_moves->name_figure[n]        = list_surplus_moves->name_figure[i];
	 list_surplus_moves->name_taking_figure[n] = list_surplus_moves->name_taking_figure[i];
     list_surplus_moves->initial_position[n]   = list_surplus_moves->initial_position[i];
     list_surplus_moves->final_position[n]     = list_surplus_moves->final_position[i];
     list_surplus_moves->description_move[n]   = list_surplus_moves->description_move[i];


// восстанавливаем запись
	 list_surplus_moves->name_figure[i]        = name_figure_s;
	 list_surplus_moves->name_taking_figure[i] = name_taking_figure_s;
     list_surplus_moves->initial_position[i]   = initial_position_s;
     list_surplus_moves->final_position[i]     = final_position_s;
     list_surplus_moves->description_move[i]   = description_move_s;
	 }//     if(material[list_surplus_moves->name_figure[n]]>material[list_surplus_moves->name_figure[i]]){ 
   }//for (int i=0;i<m;i++){
 }//for (int n=0;n<m;n++){	
return n;
}//void Sorting_surplus_moves(T_list_surplus_moves list_surplus_moves,T_list_surplus_moves * sort_list_surplus_moves){
//	*******************************************************************













