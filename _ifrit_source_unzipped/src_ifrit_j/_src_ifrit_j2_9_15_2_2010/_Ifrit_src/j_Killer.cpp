#include <fstream>  // TEST
#include <stdlib.h>
using namespace std;


#include "j_Killer.h"
#include "k_Util.h"

//-----------------------------------------------------------------------------------
/*
 * реализуем киллеры
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  25.02.2009
 *
*/
//---------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

 
//	===================================================================
// пишем киллера
void SKILLER::Save_killer(unsigned __int8 i,//номер хода в списке
				          struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
				          unsigned __int8 depth// текущая глубина
				          ){
	if (killer_moves_1.move[depth] != p_list_surplus_moves->move[i]){
         killer_moves_2.move[depth] = killer_moves_1.move[depth];
         killer_moves_1.move[depth] = p_list_surplus_moves->move[i];
	}//if (killer_moves_1[depth].move != p_list_surplus_moves->move[i])

}//void SKILLER::Save_killer
//	*******************************************************************

//	===================================================================
// вставляем киллера в начало списка(позиция start) 
void SKILLER::Insert_killer(struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
				            unsigned __int8 depth,// текущая глубина
				            unsigned __int8 start//позиция с которой ищем и на которую вставляем киллер
				            ){

__int32 move_s = 0;
__int32 score_s;// используем для запоминания оценки хода
unsigned __int8 j;

// 1 ищем киллер 1 ход в списке
for( j = start

    // крутим до тех пор пока не нашли первый киллер ход или не кончился список
	;( (p_list_surplus_moves->move[j] != killer_moves_1.move[depth]) 
	&& (j < p_list_surplus_moves->end_list) )

	; j++  );

// если такой ход нашелся то идем дальше
if(j < p_list_surplus_moves->end_list){	
//   ------------------------------------------------------------------------
//   2 запоминаем киллер ход 
	 move_s  = p_list_surplus_moves->move[j];
     score_s = p_list_surplus_moves->sorting_score[j];
//   ------------------------------------------------------------------------

//   3 сдвигаем все строчки вниз
//   от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
     for(unsigned __int8 i = j; i > start; i--){	
 	    p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
        p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
     }//for(int i = j; i > start; i--){

//   ------------------------------------------------------------------------
//   4 на стартовую позицию записываем киллера
	 p_list_surplus_moves->move[start] = move_s;
     p_list_surplus_moves->sorting_score[start] = score_s;
//   ------------------------------------------------------------------------

}//if(j < p_list_surplus_moves->end_list){	

start = start + 1;

// 1 ищем киллер 2 ход в списке
for( j = start

	// крутим до тех пор пока не нашли второй киллер ход или не кончился список
	;( (p_list_surplus_moves->move[j] != killer_moves_2.move[depth]) 
	&& (j<p_list_surplus_moves->end_list) )

	;j++);

// если такой ход нашелся то идем дальше
if(j < p_list_surplus_moves->end_list){	
//   ------------------------------------------------------------------------
//   2 запоминаем киллер ход 
	 move_s  = p_list_surplus_moves->move[j];
     score_s = p_list_surplus_moves->sorting_score[j];
//   ------------------------------------------------------------------------

//   3 сдвигаем все строчки вниз
//   от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
     for(unsigned __int8 i = j; i > start; i--){	
 	    p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
        p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
     }//for(int i = j; i > start; i--)

//   ------------------------------------------------------------------------
//   4 на стартовую позицию записываем киллера
	 p_list_surplus_moves->move[start] = move_s;
     p_list_surplus_moves->sorting_score[start] = score_s;
//   ------------------------------------------------------------------------

//   TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
     #if TEST_K
        Test_Killer.open ("_TEST_KILLER.txt",ios::app);
 //     Test_Hash.open ("_TEST_HASH.txt");
     #endif 
//   TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

     #if TEST_K
        Test_Killer << "записываем киллера"<< "\n";
        Test_Killer << "start= "<< start << "\n";
        Test_Killer << "j= "<< j << "\n";
        Test_Killer << "i= "<< i << "\n";
        Test_Killer << "i_end= "<< i_end << "\n";
        Test_Killer << "depth= "<< depth << "\n";
        Test_Killer << "p_list_surplus_moves->end_list[depth]= "<< p_list_surplus_moves->end_list[depth] << "\n";
        Test_Killer << "killer_moves[depth].initial_position= "<< killer_moves[depth].initial_position << "\n";
        Test_Killer << "killer_moves[depth].final_position= "<< killer_moves[depth].final_position << "\n";
        Test_Killer << "                "<< "\n";
        Test_Killer <<" "<<(x_to_notation(killer_moves[depth].initial_position%16));
        Test_Killer <<""<<(8-killer_moves[depth].initial_position/16);
        Test_Killer <<" "<<(x_to_notation(killer_moves[depth].final_position%16));
        Test_Killer <<""<<(8-killer_moves[depth].final_position/16) << "\n";
    #endif 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
    #if TEST_K
       Test_Killer.close();
    #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//if(j < p_list_surplus_moves->end_list){	

}//void SKILLER::Insert_killer
//	*******************************************************************

//	===================================================================
// инициализируем киллера
void SKILLER::Ini_killer(
				         ){
	for (unsigned __int8 i = 0; i < 100; i++ ){
        killer_moves_2.move[i] = 0;
        killer_moves_1.move[i] = 0;
	}//for (i = 0; i < 100; i++ )

}//void SKILLER::Ini_killer
//	*******************************************************************