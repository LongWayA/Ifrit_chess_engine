#include <iostream> // TEST
#include <fstream>  // TEST
#include <cstring>    // используем для работы со строкой (сравнение строк)
#include <stdlib.h>   // обеспечиваем завершение потока функцией exit(1)
using namespace std;


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
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.02.2009 12.11.2009
 *
*/
//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_L
   ofstream Test_Loop;
#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

class UTIL util_o;

//	===================================================================
// выводим текущий вариант в оболочку
void FULLSEARCH::Print_currmove(const struct T_list_surplus_moves * const p_list_surplus_moves,//список ходов
				              const unsigned __int8 nodes_0,// количество ходов на нулевой глубине
				              const unsigned __int8 i,// номер хода по списку
				              const unsigned __int8 Depth_Max// максимальная глубина
				              ){

// рассчитываемый в момент печати ход
 cout << "info currmove " ;
 cout <<util_o.x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8);
 cout <<(1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);
 cout <<util_o.x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8);
 cout <<(1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8);
// порядковый номер рассчитываемого хода
 cout << " currmovenumber " ;
 cout << (__int32)nodes_0;
// печатаем информацию о глубине просмотра
 cout << " depth " << (__int32)Depth_Max;
 cout << "\n";
 cout.flush();
}//void FULLSEARCH::Print_cout_v
//	*******************************************************************


//	===================================================================
// выводим текущий вариант в оболочку
void FULLSEARCH::Print_pv_line(const bool colour_move,// цвет печатающей стороны
				            const struct T_list_surplus_moves * const p_list_surplus_moves,// список ходов
				            const struct T_PV * const p_PV_best_point,// лучший вариант
				            const __int64 print_nodes,// количество узлов
				            const unsigned __int8 i,// номер хода в списке
				            const unsigned __int8 Depth_Max,// максимальная глубина
				            const __int32 value// оценка позиции
				            //,const double time_nodes// время расчета варианта
				            ){

static __int32 save_value =-999999999; // запоминаем оценку
static unsigned __int8 save_Depth_Max = 0; // запоминаем глубину

__int32 inv = 0;// учитываем цвет ходящей стороны
__int32 d_m = 0;// обрабатываем случай превращения пешки
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

if(save_value == value) cout <<"\n";

// выводим вариант только в том случае если лучшая оценка а значит и лучший
// вариант изменились что бы не было вывода одного и того же сто раз 
#if TEST_PRINT_ALL_LINE
   if(1){
#else
// если оценка изменилась или изменилась глубина то выводим вариант
   if(  (save_value != value) // изменилась оценка
	    || (save_Depth_Max != Depth_Max) // изменилась глубина
	  ){
      save_value = value;
      save_Depth_Max = Depth_Max;
#endif//#if TEST_PRINT_ALL_LINE

      cout << "info " ;
//      cout << " nps "<<(int)(nodes/time_nodes);
      cout << " depth "<< (__int32)Depth_Max;
//    печатаем информацию о глубине форсированного варианта
      cout << " seldepth "<< (__int32)p_PV_best_point->depth_max;

//    печатаем информацию о оценке позиции(это лучшая на данный момент оценка связанна с лучшим вариантом)
      colour_move == 0 ? inv = -1 : inv = 1;
      if(inv * value > 900000){
            cout << " score mate " << (__int32)p_PV_best_point->depth_max;// дистанция до мата в полу ходах
      } else if(inv * value < -900000){
            cout << " score mate " << -(__int32)p_PV_best_point->depth_max;// дистанция до мата в полу ходах
      }else{
	        cout << " score cp " << inv * value;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
      }//if(inv * value > 900000)

//    печатаем информацию о количестве просмотренных узлов
      cout << " nodes "<<(__int32)print_nodes;

//    сравниваю по модулю потому что из за продлений на шахах запарки со знаком
      if( (value * value) != (p_PV_best_point->score_move * p_PV_best_point->score_move) ) {
    //if(0) {
           cout << "\n";
	       cout << "ERROR LINE " ;
           cout << "\n";
           cout << " value = " << value;
           cout << "\n";
           cout << " p_PV_best_point.score_move = " << p_PV_best_point->score_move;
           cout << "\n";
           cout << "**********************"<<"\n";
           cout << "  " << "\n";
      }else{
//         вывод линии варианта
           cout << " pv ";
           for (unsigned __int8 n = 0; n < p_PV_best_point->depth_max; n++){
                cout << util_o.x_to_notation(((p_PV_best_point->move[n]>>12)& 63)%8);
                cout << (1 + ((p_PV_best_point->move[n]>>12)& 63)/8);
                cout << util_o.x_to_notation(((p_PV_best_point->move[n]>>18)& 63)%8);
                cout << (1 + ((p_PV_best_point->move[n]>>18)& 63)/8);

                d_m = ((p_PV_best_point->move[n]>>6)& 63);
//              записываем фигуру превращения пешки
                if(d_m != 5){
                    if(d_m > 15){
                        cout <<util_o.int_to_char_board((d_m - 20),0);
                    }else{
                        cout <<util_o.int_to_char_board((d_m - 10),0);
                    }//if(d_m > 15)
                }//if(d_m != 5)
                cout <<" ";
           }//for (int n=0;n<p_PV_best_point.depth_max;n++)
      }//if( (value * value) != (p_PV_best_point.score_move * p_PV_best_point.score_move) )
      cout <<"\n";
  }// if(  (save_value != value)

cout.flush();// это уже не нужно но все равно оставлю :)

}//void FULLSEARCH::Print_cout
//	*******************************************************************


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// максимальная глубина, глубина перебора, доска,
#if TEST_L

void FULLSEARCH::List_Print(int depth,// текущая глубина
                struct T_list_surplus_moves * p_list_surplus_moves// список ходов
				){

Test_Loop.open ("_TEST_LOOP.txt",ios::app);	
//Test_Loop.open ("_TEST_LOOP.txt");	
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop << "\n";
Test_Loop << "\n";
Test_Loop << "\n";
Test_Loop << "------------------------------------------------- "<<"\n";
Test_Loop <<"depth= "<< depth << "\n";
Test_Loop << "\n";
Test_Loop << "Список возможных ходов фигуры "<<"\n";

for (int i = 0 ; i < p_list_surplus_moves->end_list ; i++){	
     Test_Loop << "Номер хода по списку i= " << i << "\n";  
     Test_Loop << "Ходящая фигура " << util_o.int_to_char_board(((p_list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
     Test_Loop << "Оценка хода " << p_list_surplus_moves->sorting_score[i] << "\n";

     // вывод хода вида e2-e4
     //------------------------------
	 Test_Loop << " " << (util_o.x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8));
     Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
 //  Test_Loop<< "\n";
     Test_Loop << " " << (util_o.x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8));
     Test_Loop << "" << (1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
     //------------------------------
 //тип хода 
 //0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка 4 – короткая рокировка 5 – взятие на проходе
 //12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
 //22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
 //25 - взятие с превращением в ферзь 
     Test_Loop << "вид хода " << ((p_list_surplus_moves->move[i]>>6)& 63)  << "\n";
     Test_Loop << "Взятая фигура " << util_o.int_to_char_board(( p_list_surplus_moves->move[i] & 63),1)<< "\n";
	 Test_Loop << "начальная положение " << ((p_list_surplus_moves->move[i]>>12)& 63)  << "\n";
	 Test_Loop << "конечное положение " << ((p_list_surplus_moves->move[i]>>18)& 63)  << "\n";
     

  
	 Test_Loop << "\n";
}//for (i=1,i<p_list_surplus_moves->amount_moves+1){

Test_Loop << "p_list_surplus_moves->end_captures= " <<p_list_surplus_moves->end_captures << "\n";
Test_Loop << "p_list_surplus_moves->end_list= " <<p_list_surplus_moves->end_list << "\n";
Test_Loop << "------------------------------------------------- " << "\n";
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop.close();
}//void FULLSEARCH::List_Print

#endif
//	*******************************************************************





