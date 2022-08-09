#include <iostream>
#include <fstream>  // TEST
using namespace std;

#include "g_Loop.h"
#include "c_Iterative_Deepening.h"
#include "k_Util.h"
#include "k_Timer.h"
#include "h_Estimation.h"
#include "j_Hash.h"
#include "d_Bitboard_gen.h"

//-----------------------------------------------------------------------------------
/*
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

#define TEST_WRITE_READ_WN 0 // включаем-выключаем(1-0) режим подсчета количества
                             // записанных и считанных позиций
                             // надо еще включить флаг TEST_WRITE_READ в j_Hash.h

//global
class TIMER timer;//отмеряет интервалы времени
class FULLSEARCH full_search_o;// поиск

extern class UTIL util_o;//некоторые вспомогательные функции
extern class EVAL  eval_o;//оценка позиции
extern class HASHM hash_o;// работаем с хеш-таблицей
extern class BITBOARDGENERATOR bitboard_generator_o;// генератор ходов

//	===================================================================
//
void Deepening(struct T_Bitboard * p_bitboard,//представление доски
			   unsigned __int8 depth_max,//глубина перебора
			   double wtime,//оставшееся время на игру за белых
			   double btime,//оставшееся время на игру за черных
			   double winc,//добавление времени за ход для белых
			   double binc,//добавление времени за ход для черных
			   unsigned __int8 movestogo,//количество ходов на которое выделено время
			   double movetime,//время на ход
			   bool infinite//бесконечный анализ
			   ){

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
 //   Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

// время смотрим в секундах
double time_terminate = -1.0;// время выделенное на ход
double time_current = -1.0;  // сколько времени прошло при данной итерации

__int32 Alpha = -9999999;
__int32 Beta  =  9999999;

__int8 inv = 0;// флаг цвета хода
__int32 value_root = 0;// оценка лучшего варианта и делаемого хода. 
__int32 d_m = 0; // тип хода. нужен для определения хода пешкой с превращением
__int64 static nodes_y = 0; // количество узлов на данной глубине
__int64 static nodes_previous = 0; // запоминаем полное количество узлов
__int32 test_eval = 0;// запоминаем оценку материала для контроля
bool flag_mate = 0;// найден мат 
unsigned __int64 key_control = 0;   // запоминаем хеш-ключ для контроля
struct T_list_surplus_moves root_list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)

__int64 nodes = 0;// количество рассмотренных узлов(позиций)
unsigned __int16 cnt_move ;// количество уже сделанных в партии полуходов

unsigned __int16 limit_cnt_move ;// сколько ходов осталось сделать (предполагаемая цифра)
//-------------------------------------------------------------------------------
//             > go btime 302000 wtime 302000 winc 2000 binc 2000 
// 5 min * 60 = 300 sek
// время у нас приходит в секундах хотя от оболочки в миллисекундах но мы
// еще раньше поделили на 1000
// выделяем время на ход(если конечно оно определенно). 
// сейчас организованно примитивно и тупо. в начале игры много в конце ничего
// так что в будущем это место следует переписать но для данного уровня игры пойдет и так
//if(p_bitboard->colour_move == 1){// 1-ход белых 0- ход черных
//    if(wtime != -1) time_terminate = wtime/100;
//}else{
//    if(btime != -1) time_terminate = btime/100;
//}//if (board_list_816->colour_move == 1){


// количество полуходов сделанных в партии
cnt_move = hash_o.get_save_cnt_move();

// мы предполагаем что все станет ясно за определенное количество ходов (80)
// это оставшееся количество ходов
limit_cnt_move = 80 - cnt_move/2 ;
// время на первые 40 ходов примерно одинаковое
// после 40 го хода идет уменьшение времени на ход что бы оттянуть просрочку по времени
if(limit_cnt_move < 40 ) limit_cnt_move = 40;

// умножаем на коэффициент перехода на другой уровень т.е. 
// насколько дольше будет считаться следующая глубина
limit_cnt_move = 3 * limit_cnt_move;


if(p_bitboard->colour_move == 1){// 1-ход белых 0- ход черных
    if(wtime != -1) time_terminate = wtime/limit_cnt_move;
}else{
    if(btime != -1) time_terminate = btime/limit_cnt_move;
}//if (board_list_816->colour_move == 1){


// TEST
//wtime = 60; // 5 min
//while (wtime > 1){
// limit_cnt_move = 80 - cnt_move/2 ;
// if(limit_cnt_move < 40 ) limit_cnt_move = 40;
// //limit_cnt_move = 3 * limit_cnt_move;
// time_terminate = wtime/limit_cnt_move;
//
// cout << "cnt_move = " << cnt_move << "\n";
// cout << "wtime = " << wtime << "\n";
// cout << "time_terminate = " << time_terminate << "\n";
// //cout << "limit_cnt_move = " << limit_cnt_move << "\n";
// cout << "   " << "\n";
//
// wtime = wtime - time_terminate;
// cnt_move = cnt_move + 1;
//}//while (wtime > 0)


// TEST
//cout << "cnt_move = " << cnt_move << "\n";
//cout << "limit_cnt_move = " << limit_cnt_move << "\n";
//cout << "time_terminate = " << time_terminate << "\n";


//             > go movetime 300000 
//задаем время на ход (если конечно оно определенно).
if (movetime != -1) time_terminate = movetime;
//--------------------------

// на глубине обрубаем только при существенном превышении времени на расчет
full_search_o.deep_time_terminate = time_terminate * 50;

//если время окончания хода определенно то
//ограничение по глубине 50
if(time_terminate != -1) depth_max = 50;
//--------------------------

//             > go infinite
// предел при бесконечном анализе. 50 полу ходов думаю достаточно
if(infinite == true) depth_max = 50;
//--------------------------

// если вдруг все оказалось не инициализировано  
if(depth_max == 0) depth_max = 1;
//-------------------------------------------------------------------------------

// TEST
//cout << "movetime = " << movetime << "\n";
//cout << "time_terminate = " << time_terminate << "\n";
//cout << "depth_max = " << depth_max << "\n";

// инициализируем глобальные переменные модуля loop
full_search_o.ini_global_loop(depth_max);

// считаем материал. в дальнейшем только корректируем оценку при взятиях.
p_bitboard->eval = eval_o.Eval_forced(p_bitboard);

// для проверки что после возвращения оценка вернулась к первоначальному положению
test_eval =  p_bitboard->eval;

// TEST
//cout << " N bitboard->eval = " << bitboard->eval << "\n";

// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
hash_o.Start_position_random_key(p_bitboard);

// записываем ключ для контроля при выныривании с глубины
key_control = p_bitboard->hash_key;

//инициализируем хеш - массив повторов возрастающими числами что бы исключить ложное срабатывание
hash_o.Ini_hash_three(); 

// записываем ключ для последующего анализа на повтор позиций
hash_o.Save_hash_three(p_bitboard->hash_key,0);

// TEST
// здесь очищаем хеш-таблицу
// инициализируем все ключи в хеш-таблице 0
//   hash_o.Ini_hash_moves();

// использую для тестирования подтверждением варианта
// TEST
//depth_max = depth_max - 1;

// TEST
//cout << "1111111 " << "\n";

  if(p_bitboard->colour_move == 1){  
	   // белые 
       //Заполняем список избыточных ходов. Именно эти списки и составляют дерево перебора.
       bitboard_generator_o.Generator_captures_white(p_bitboard,&root_list_surplus_moves);// генерируем взятия и превращения
       bitboard_generator_o.Generator_move_white(p_bitboard,&root_list_surplus_moves);// генерируем остальные ходы
  }else{
	   // черные
       //Заполняем список избыточных ходов. Именно эти списки и составляют дерево перебора.
       bitboard_generator_o.Generator_captures_black(p_bitboard,&root_list_surplus_moves);// генерируем взятия и превращения
       bitboard_generator_o.Generator_move_black(p_bitboard,&root_list_surplus_moves);// генерируем остальные ходы
  }//if(bitboard->colour_move ==1){

// TEST
//cout << "22222222222 " << "\n";
//cout << " depth_max " << depth_max << "\n";//глубина

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
  // TEST
  // обнуляем счетчики
  hash_o.Test_hash_ini();
#endif // #if hash_o.TEST_WRITE_READ_WN
//---------------------------------------------------------------

// если глубина меньше максимальной и не нашли мат и меньше 50 и не было команды на остановку то продолжаем счет
for(unsigned __int8 depth = 1; 
    ( (depth <= depth_max) && // глубина меньше максимальной заданной на весь перебор
	  (flag_mate == 0) && // нету мата
	  (depth < 51) && // глубина меньше максимальной 
	  (full_search_o.get_stop_search() == 0)// нет команды на остановку счета 
	); // нет команды на конец счета
    depth++
   ){
 
// запоминаем полное количество узлов с предыдущей итерации т.е глубины
  nodes_previous = nodes;

//сбрасываем счетчик позиций
  full_search_o.set_nodes_in_0();

// запускаем таймер
  timer.Timer_start();

  if(p_bitboard->colour_move == 1){                                               
       value_root = full_search_o.White_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
  }else{
       value_root = full_search_o.Black_root(p_bitboard,root_list_surplus_moves,Alpha,Beta,depth);
  }//if(bitboard->colour_move ==1){

// останавливаем таймер
  timer.Timer_stop();
  time_current = timer.Timer_delta();

nodes = full_search_o.get_nodes();

//количество узлов на слое считаем как полное количество узлов
//минус полное количество узлов на предыдущей итерации
  nodes_y = nodes - nodes_previous ;

// проверка оценки материала
if(test_eval != p_bitboard->eval) cout << "ERROR test_eval != bitboard->eval" << "\n";
 
//-----------------------------------------------
// проверяем хеш-ключ на корректность т.е. что после возвращения с глубины он вернулся к 
// состоянию которое он имел до погружения на глубину
    if(key_control != p_bitboard->hash_key){
	   	 cout <<"\n";
		 cout << "ERROR HESH Test_Deepening " ;
		 cout <<"\n";
	}//if(key != p_bitboard->hash_key){
//-----------------------------------------------

//---------------------------------------------------------------
// печатаем информацию в поток
  cout << "info " ;
  cout << " nps " << (__int32)(nodes / time_current);// скорость из расчета по всем узлам на данной глубине
  cout << " depth " << (__int32)depth;//глубина

// печатаем информацию о глубине форсированного варианта
   cout << " seldepth "<< (__int32)full_search_o.get_pv_best()->depth_max;

  p_bitboard->colour_move == 0 ? inv = -1 : inv = 1;

// печатаем оценку. для мата специальное представление в виде дистанции
  if(inv * value_root > 900000){ 
     flag_mate = 1;
     cout << " score mate "<< (__int32)full_search_o.get_pv_best()->depth_max;// дистанция до мата в полуходах
  }else if(inv * value_root < -900000){
     cout << " score mate "<< -(__int32)full_search_o.get_pv_best()->depth_max;// дистанция до мата в полуходах
     flag_mate = 1;
  }else{
     cout << " score cp "<< inv * value_root;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
     flag_mate = 0;
  }//if(inv * value_root > 900000){ 
 
// cout << " score cp "<< inv*value_root;// для теста матовой оценки

// печатаем линию варианта
 cout << " nodes " << (__int32)nodes; // полное количество узлов
//  cout << " nodes "<< nodes_y; // количество узлов на слое
  cout <<" pv ";

  for( unsigned __int8 n = 0; n < full_search_o.get_pv_best()->depth_max; n++){
      cout << util_o.x_to_notation(((full_search_o.get_pv_best()->move[n]>>12)& 63)%8);
      cout << (1 + ((full_search_o.get_pv_best()->move[n]>>12)& 63)/8);
      cout << util_o.x_to_notation(((full_search_o.get_pv_best()->move[n]>>18)& 63)%8);
      cout << (1 + ((full_search_o.get_pv_best()->move[n]>>18)& 63)/8);

//    считываем тип хода
      d_m = ((full_search_o.get_pv_best()->move[n]>>6)& 63);

//    если d_m больше 5 то значит имеем превращение пешки
      if(d_m > 5){
          if(d_m > 15){
               cout << util_o.int_to_char_board((d_m - 20),0);// превращение пешки с взятием
          }else{
               cout << util_o.int_to_char_board((d_m - 10),0);// просто превращение пешки
          }//if(d_m > 15){
      }//if(d_m > 5){

      cout << " ";
   }//for( int n = 0; n < full_search_o.get_pv_best()->depth_max; n++){

   cout <<"\n";

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
// TEST

// сколько элементов в таблице и сколько использованно т.е. во сколько мы записали
hash_o.TEST_Hash_index_print();

// печатаем инфу об эффективности кеширования
hash_o.Test_hash_print();
#endif // #if hash_o.TEST_WRITE_READ_WN
//---------------------------------------------------------------

// выходим по времени
   if((time_terminate != -1) && (time_terminate < time_current) && (depth >= 2)) break;

}//for(int depth = 1; ((depth < (depth_max + 1)) && (depth < 51) && (full_search_o.get_stop_search() == 0)); depth++){

// TEST
//cout << "cnt_move = " << cnt_move << "\n";
//cout << "wtime = " << wtime << "\n";
//cout << "time_terminate = " << time_terminate << "\n";
//cout <<"time_current = "<< time_current <<"\n";
//cout <<"   " <<"\n";

//-----------------------------------------------
// проверяем хеш-ключ на корректность т.е. что после возвращения с глубины он вернулся к 
// состоянию которое он имел до погружения на глубину
    if(key_control != p_bitboard->hash_key){
	   	 cout <<"\n";
		 cout << "ERROR HESH Test_Deepening " ;
		 cout <<"\n";
	}//if(key != p_bitboard->hash_key){
//-----------------------------------------------


//---------------------------------------------------------------------
// делаем ход вида < bestmove e2e4 
// или при превращении в коня(к примеру)  < bestmove d7d8N 
cout << "bestmove ";
cout << util_o.x_to_notation( full_search_o.get_from() % 8);
cout << (1 + full_search_o.get_from() / 8);
cout << util_o.x_to_notation( full_search_o.get_to() % 8);
cout << (1 + full_search_o.get_to() / 8);

// считываем тип хода
d_m = ((full_search_o.get_pv_best()->move[0]>>6)& 63);

// если d_m больше 5 то значит имеем превращение пешки
if(d_m > 5){
   if(d_m > 15){
      cout << util_o.int_to_char_board((d_m - 20),0);
   }else{
      cout << util_o.int_to_char_board((d_m - 10),0);
   }//if(d_m > 15){
}//if(d_m != 5){
cout <<"\n";

// контроль соответствия первого хода первому ходу линии варианта
if( full_search_o.get_from() != ((full_search_o.get_pv_best()->move[0]>>12)& 63)) cout << "ERROR from(Deepening)" <<"\n";
if( full_search_o.get_to() != ((full_search_o.get_pv_best()->move[0]>>18)& 63))     cout << "ERROR to(Deepening)" <<"\n";
//---------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
    Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//void Deepening
//	*******************************************************************
