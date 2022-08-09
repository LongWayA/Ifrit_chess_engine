#include <fstream.h>  // TEST

#include "g_Loop.h"
#include "c_Iterative_Deepening.h"
#include "k_Util.h"
#include "k_Timer.h"
//-----------------------------------------------------------------------------------
/* ? нужно внимательно все посмотреть
 * перебор с циклическим увеличением глубины
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.02.2008
 *
 *  ТЕСТИРОВАНИЕ
 * тестов пока нет тут надо очень много смотреть
*/
//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// мы передаем массив доски, максимальную глубину,строку повторов ходов
void Deepening(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,short int depth,char three_position[],T_PV three_moves,int & nodes,double wtime,double btime, double winc,double binc,int movestogo,double movetime,bool infinite){


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
 //   Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
T_PV PV_current;// текущая линия
T_PV PV_best;   // лучшая линия

int Alpha=9999999;
int Beta=-9999999;
short int br =0;// переменная для отсечки перебора при экстренном выходе

// время смотрим в секундах
double time_terminate= -1.0;// время выделенное на ход
double time_current= -1.0;  // сколько времени прошло при данной итерации
int value_root=0;           // оценка лучшего варианта и делаемого хода. 
//> go btime 300000 wtime 300000 
// 5 min * 60 = 300 sek
// время у нас приходит в секундах хотя от оболочки в милисекундах но мы
// еще раньше поделили на 1000


// выделяем время на ход(если конечно оно определенно). 
// сейчас организованно по дибильному. в начале игры много в конце ничего
// деление на 40 тоже силы не прибавляет и если в начале игры смотрится
// нормально то в конце уже явный маразм
if (bitboard->colour_move == 1){// 1-ход белых 0- ход черных
  if (wtime != -1) time_terminate= wtime/100;
}else{
  if (btime != -1) time_terminate= btime/100;
}//if (board_list_816->colour_move == 1){

// задаем время на ход (если конечно оно определенно).
 if (movetime != -1) time_terminate= movetime;

//  
if (depth == -1) depth=6;

// если время окончания хода определенно то
// ограничение по глубине 20 не знаю насколько это актуально :)
if (time_terminate!=-1) depth=20;

// предел при бесконечном анализе 100 полуходов думаю достаточно :)
if (infinite==true) depth=100;

//time_current=0;

//Print_hash_moves();
//Test_Deepening << "board_list_816->hash_key="<<board_list_816->hash_key << "\n";
//Test_Deepening << "========================================= "<< "\n";
//Test_Deepening << "wtime= "<<wtime << "\n";
//Test_Deepening << "btime= "<<btime << "\n";
//Test_Deepening << "movetime= "<<movetime << "\n";
//Test_Deepening << "time_terminate= "<<time_terminate << "\n";
nodes=0;
int static nodes_y = 0;
//-----------------------------------------------------
for (int d=1;((d<(depth+1))&&(br==0));d++){
  Timer_start_d();
  nodes_y = nodes;
  nodes=0;
  value_root=-Alpha_beta_root(bitboard,list_surplus_moves,-Beta,-Alpha,d,0,nodes,&PV_current,&PV_best,0,&three_moves,br,0,0);
 
  nodes_y = nodes - nodes_y ;
  Timer_stop_d();
  time_current= Timer_delta_d();
//---------------------------------------------------------------
 cout << "info " ;
 cout << " nps "<<nodes/time_current;
 cout << " depth "<<d;
 if(value_root>900000){
    cout << " score mate "<< (PV_best.depth_max - 1);// дистанция до мата в полуходах
 }else if(value_root<-900000){
   cout << " score mate "<< -(PV_best.depth_max - 1);// дистанция до мата в полуходах
 }else{
    cout << " score cp "<< value_root;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
 }//if(value_root>900000){
 
// cout << " nodes "<<nodes;
 cout << " nodes "<< nodes_y;
 cout <<" pv ";
 for (short int n=0;n<PV_best.depth_max;n++){
 cout <<x_to_notation(PV_best.initial_position[n]%8);
 cout <<(1 + PV_best.initial_position[n]/8);
 cout <<x_to_notation(PV_best.final_position[n]%8);
 cout <<(1 + PV_best.final_position[n]/8); 
 if (PV_best.final_pawn[n]!=0) cout <<int_to_char_board(PV_best.final_pawn[n],0);
 cout <<" ";
}//for (int n=1;n<(Depth_Max);n++){ 
 cout <<"\n";
//---------------------------------------------------------------
//Test_Deepening << "d= "<<d << "\n";
//Test_Deepening << "time_current= "<<time_current << "\n";
  if((time_terminate!=-1)&&(time_terminate<time_current)) break;
}//for (int d=1;d<(Depth_Max+1);d++){

//Print_hash_moves();
//Test_Deepening << "board_list_816->hash_key="<<board_list_816->hash_key << "\n";
//---------------------------------------------------------------------
// делаем ход вида < bestmove e2e4 
// или при превращении в коня(к примеру)  < bestmove d7d8N 
 cout <<"bestmove " <<x_to_notation(PV_best.initial_position[0]%8);
 cout <<(1 + PV_best.initial_position[0]/8);
 cout <<x_to_notation(PV_best.final_position[0]%8);
 cout <<(1 + PV_best.final_position[0]/8);
 // если есть фигура превращения
 if (PV_best.final_pawn[0] !=0) cout <<int_to_char_board(PV_best.final_pawn[0],0);
 cout <<"\n";
//---------------------------------------------------------------------

//Test_Deepening << "\n";
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
}//void Loop(int chess_board[],int Depth_Max,int print_bestmove[],int Eval,int Nodes,int pv[]){
//	*******************************************************************