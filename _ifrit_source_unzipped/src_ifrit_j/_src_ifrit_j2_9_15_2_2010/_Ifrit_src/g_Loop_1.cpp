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
#include "j_Killer.h"
#include "m_Line.h"
#include "k_Timer.h"
#include "d_Bitboard_gen.h"


//-----------------------------------------------------------------------------------
/*
 * перебор позиций
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009
 *
 *  ТЕСТИРОВАНИЕ
 * тестовую информацию выводим в файл _TEST_LOOP.txt
 *
 * ВОЗМОЖНЫЕ ИНДУКТОРЫ ОШИБКИ
 * тестировать и описывать тут очень много. надо будет все тщательно продумать
 * по сути это сердце программы и именно тут сходятся все ниточки
 * нужно контролировать:
 * 1 генератор позиций - правильно ли генерируются всевозможные позиции не пропущено ли что
 * 2 линия варианта - правильно ли она печатается нет ли тут искажений
 * правильно ли печатается оценка и глубина варианта а также количество позиций
 * 3 альфа-бета отсечение - правильно ли оно работает нет ли где недостаточных
 *   или избыточных отсечений 
*/
//---------------------------------------------------------------------------


//	===================================================================
/*
----------------------------------------------------------------------------
 так работает генератор позиций:
 1 в начале генерируются список избыточных ходов. избыточных потому что нет проверки на шах
   и в них есть такие ходы как ход короля под шах
 2 дальше мы сортируем этот список выводя взятия на верх и ход из хеш-таблицы если он есть и киллер
 3 дальше мы реализуем ход и проверяем позицию на шах если 
   под боем наш король то ход откатываем иначе идем дальше
 4 итерационное углубление
 5 когда вернулись с глубины откатываем ход
 6 если ходов не было то это случай мата или пата

*/

// NULL MOVE
#define NULL_YES 1 // мы внутри нулевого хода 
#define NULL_NO  0 // мы снаружи нулевого хода 
#define NULL_DEPTH 2 // дистанция до дна меньше которой NULL не вызывается
#define SWITCH_NULL_REDUCTION_1_2 5 // глубина на которой меняется редукция нулевого хода 
                                    // с NULL_REDUCTION_ONE на NULL_REDUCTION_TWO
#define NULL_REDUCTION 4 // 4 сокращение глубины перебора для NULL
#define NULL_REDUCTION_ONE 4 // 4 сокращение глубины перебора для динамического NULL
#define NULL_REDUCTION_TWO 3 // 3 сокращение глубины перебора для динамического NULL
#define VERIFICATOIN_SEARCH_REDUCTION 5 // сокращение глубины перебора 
                                        //для поиска по обнаружению цугцванга
// LMR
#define LMR_DEPTH 3 // дистанция до дна меньше которой LMR не вызывается
#define LMR_REDUCTION_D 1 // сокращение глубины перебора для LMR
#define FULL_SEARCH 2 // было 2
                      // число веток с полным поиском без сокращений 
                      // ход из хеш-таблицы, лучшее взятие,
                      // два киллера, все взятия не редуцируются
                      // хеш-ход два киллера это 3 и минус один потому что с нуля 
                      // надо еще помнить что условие редукции i > lmr_reduction



// константы
#define CHECK_YES 1 // есть шах
#define CHECK_NO  0 // нет шаха


#define MOVE_IS_CAPTURE 2 // ход это взятие
#define CAPTURE_NO  0 // нет взятий

#define PV_YES 1 // основной вариант
#define PV_NO  0 // 

#define VALUE_MATE  1000000 // оценка мата
#define DELTA_MATE  1000 // дельта для нахождения скорейшего мата

// согласовать с файлом j_Hash.cpp
 #define EXACT 1 // точный ход
 #define LOWER 2 // отсечка по бете(альфе)
 #define UPPER 3 // альфу(бету) улучшить не удалось

// работа этих эвристик меня не устраивает.-----------------------------

// FUTILITY PRUNING
# define FUTILITY_MARGIN 120 // поле отсечки для futility pruning
# define FUTILITY_DEPTH 1 // глубина срабатывания для futility pruning

//-------------------------------------------------------------------------



// global
extern class TIMER timer;// таймер. отмеряет интервалы времени
class BITBOARDGENERATOR bitboard_generator_o;// генератор ходов
class MOVES moves_o;// делатель ходов
class QUIESCENCESEARCH quiescence_search_o;// форсированный поиск
class SKILLER killer_o;// киллеры
class HASHM hash_o;// кэширование
class PVSAVE pv_save_o;// запись основного варианта
class EVAL eval_o;// статическая оценка позиции

extern class UTIL util_o;

//	===================================================================
// поиск за белых. белые стремятся найти максимум
// перебор на нулевом уровне
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
__int32 FULLSEARCH::White_root(struct T_Bitboard * const p_bitboard,// представление доски
                           struct T_list_surplus_moves & list_surplus_moves,// список возможных ходов (всевозможные ходы из данного узла)
			               __int32 alpha,//
			               const __int32 beta,// 
			               const unsigned __int8 depth_max// максимальная глубина
			               ){

__int32 value;// текущая оценка позиции
struct T_undo undo[1];//информация используемая при возврате хода
__int32 value_max = -9999999;// лучшая в узле  оценка позиции  
unsigned __int8 depth_max_new = 0;//новая глубина перебора
bool flag_save_best = 0;// пишем ли лучший вариант
bool flag_is_legal_moves = 0;// флаг существования легальных ходов (если 0 то легальных ходов не было)
bool flag_check = 0;//флаг шаха
const unsigned __int8 depth = 0;// глубина
const unsigned __int8 new_depth = 1; // следующая глубина

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	unsigned __int8 nodes_0 = 1;            // количество ходов на нулевом уровне
	struct T_PV pv_best_point;// лучший для данного узла вариант
    struct T_Bitboard test_bitboard;// представление доски. использую для теста
    unsigned __int8 flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
    const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
    unsigned __int8 i_hash_move = 0; // номер кешируемого хода
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

depth_max_rem = depth_max;
audit = 0;// задержка опроса команды экстренного выхода
lmr_reduction = 0;//
flag_stop_search = 0;//

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
		// это отсечка при экстренном выходе
		if (flag_stop_search == 1) return 0;

// инициализируем массив киллеров
killer_o.Ini_killer(); 

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

Sorting_moves(&list_surplus_moves);
//Sorting_white_moves_root(&list_surplus_moves);// сортируем ходы

	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// тестовая печать списка и доски на нулевом уровне
	#if (TEST_L && SAVE_LINE)
         List_Print(depth_max,&list_surplus_moves);
        //if(depth_max == 4) List_Print(depth,&list_surplus_moves);
		//Bitboard_print(p_bitboard);
	#endif//#if TEST_L
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if (HASH && SAVE_LINE)
		// работаем с хеш-таблицей
		// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
		// т.е. мы в списке ход из хеш-таблицы ставим первым а остальные ходы сдвигаем
		hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

	// для полного теста нужно отключать отсечку по оценке
    #if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

//cout << "хеша корень белые flag_insert_hash_move " << flag_insert_hash_move << "\n";

       if (flag_insert_hash_move == 0) {
          cout << "хеша не нашли корень белые++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
       }//if (flag_insert_hash_move == 0) {

       if (flag_insert_hash_move == 1) {
           cout << "коллизия хеша корень белые++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 1) {
    #endif//#if HASH_TEST

	#endif//#if HASH
	//-----------------------------------------------

	////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	//// тестовая печать списка и доски на нулевом уровне
	//#if (TEST_L && SAVE_LINE)
 //       List_Print(depth,&list_surplus_moves);
	//	//if(depth_max == 4) List_Print(depth,&list_surplus_moves);
	//	//Bitboard_print(p_bitboard);
	//#endif//#if TEST_L
	////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	//------------------------------------------------------------
    //запоминаем доску для дальнейшей проверки ее идентичности
    //после возврата с глубины 
    Save_test_bitboard(p_bitboard,&test_bitboard);
	//------------------------------------------------------------

/*
 имя фигуры
 0 - нет фигуры
 1 - пешка 
 2 - конь 
 3 - слон
 4 - ладья 
 5 - ферзь 
 6 - король
*/
 //   // оцениваем количество и тип фигур если меньше чем нужно то отключаем нулевой ход
	//if( (p_bitboard->white_piece[5] == 0) && // если у белых нет ферзей
 //       (p_bitboard->white_piece[4] == 0) && // нет ладей
 //       (p_bitboard->white_piece[3] == 0)  // нет слонов
	//   ){ 
 //       //   cout <<"нулевой ход белые выключили ";
	//       //cout <<"\n";
	//       flag_stop_null_move = 0; // то выключаем нулевой ход
	//}else {
	//       flag_stop_null_move = 1; // иначе он включен
	//}// if( p_bitboard->white_piece[5] == 0 &&

flag_stop_null_move = 1;

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

  // реализуем ход из списка или другими словами генерируем позицию
  moves_o.Onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);
 // Bitboard_print(p_bitboard);

  // если король под шахом то (детектор шахов возвращает имя шахующей фигуры)
  if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
     // мы отменяем этот ход как некорректный
     moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);
// записываем оценку в корневой список ходов
list_surplus_moves.sorting_score[i] = -1000000;

  }else{

  // ставим флаг что есть легальные ходы и засчитываем позицию
     flag_is_legal_moves = 1;
     nodes = nodes + 1;
     depth_max_new = depth_max;

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	    // печатаем информацию о рассматриваемом ходе и глубине перебора
	    Print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

	    // из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине
	    pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	    // обновляем хеш ключ
	    hash_o.Modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

// TEST-----------
// тут ключ считается полностью по новой
// тестировал хеш-таблицу
// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
//hash_o.Start_position_random_key(p_bitboard);
//----------------

		// записываем ключ для последующего анализа на повтор позиций
        hash_o.Save_hash_three(p_bitboard->hash_key,new_depth);

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
    
	//-----------------------------------------------
	#if (EXTENSION_CHEK && SAVE_LINE)
	// если король под шахом то увеличиваем максимальную глубину перебора 
	    Extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
	#endif//#if EXTENSION_CHEK
	//-----------------------------------------------


#if (ALPHA_BETA_ON && SAVE_LINE)
	// здесь у нас классический негаскаут(NegaScout)
	// первую линию считаем главной и смотрим с полным окном остальные пытаемся
    // отсечь поиском с нулевым окном и если вдруг оценка улучшилась то пересчитываем
	// с полным окном
	if (i == 0){
#else
    // в режиме полного перебора смотрим все и все линии считаем главными
    if (1){
#endif//#if ALPHA_BETA_ON

            //рекурсивный вызов функции
            value = Black(p_bitboard,alpha,beta,depth_max_new,
			              new_depth,((list_surplus_moves.move[i]>>6) & 63),
					      PV_YES,flag_check,NULL_NO);

    }else{

             // вызываем функцию  с нулевым окном. это уже не главный вариант
	         value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
			               new_depth,((list_surplus_moves.move[i]>>6) & 63),
						   PV_NO,flag_check,NULL_NO);

	         if(value > alpha){

        	      // если вдруг alpha улчшили то смотрим по новой как главный вариант
		          value = Black(p_bitboard,alpha,beta,depth_max_new,
					            new_depth,((list_surplus_moves.move[i]>>6) & 63),
								PV_YES,flag_check,NULL_NO);

	         }//if (value > alpha)
    }//if (i == 0)

 // восстанавливаем позицию
    moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

// записываем оценку в корневой список ходов
list_surplus_moves.sorting_score[i] = value;

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE

// восстанавливаем хеш-ключ доски
p_bitboard->hash_key = key_undo;

   // ищем лучшую позицию. за белых ищем максимум
      if (value > value_max){
         value_max = value;
         i_hash_move = i;
         from = ((list_surplus_moves.move[i]>>12)& 63);
         to = ((list_surplus_moves.move[i]>>18)& 63);

        // TEST
      
			 
			 //-----------------------------------------------
        //       cout << "улучшили максимум. ход записанный в хеш " << "\n";
        //       cout << "depth= " << depth << "\n";
        //       cout << "depth_max_rem= " << depth_max_rem << "\n";
        //       cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				    //<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
        //       cout << "Ходящая фигура " << util_o.int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
        //       
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
        //       cout << "  " << "\n"; 
               //-----------------------------------------------



         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
         // белые работают с альфой и пытаются ее улучшить
         #if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
	         if (value > alpha){ 
		        alpha = value;       
		        flag_save_best = 1;
                // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
                pv_save_o.Update_PV_best_point(&pv_best_point);
	            // проверяем совпадение варианта и оценки
                if(value != pv_save_o.get_pv_best()->score_move){
		           cout << "ERROR white value " ;
		           cout << "\n";
	            }//if(value != pv_best.score_move){

        // TEST
        //-----------------------------------------------
        //       cout << "улучшили альфу. ход записанный в хеш " << "\n";
        //       cout << "depth= " << depth << "\n";
        //       cout << "depth_max_rem= " << depth_max_rem << "\n";
        //       cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				    //<< ((list_surplus_moves.move[i]>>6)& 63) << "\n";
        //       cout << "Ходящая фигура " << util_o.int_to_char_board(((list_surplus_moves.move[i]>>24)& 7),1)<< "\n";
        //       
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
        //       cout <<util_o.x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
        //       cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8) << "\n";
        //       cout << "  " << "\n";     
               //-----------------------------------------------

	         }//if (value > alpha){ 
         #else///#if ALPHA_BETA_ON
            flag_save_best = 1;
            // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
            pv_save_o.Update_PV_best_point(&pv_best_point);

            // проверяем совпадение варианта и оценки
            if (value != pv_save_o.get_pv_best()->score_move){
		        cout << "ERROR white value " ;
		        cout << "\n";
	        }//if(value != pv_best.score_move){

         #endif//#if ALPHA_BETA_ON
         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

    }//if (value > value_max){

	  //--------------------------------------------------------------------
	  // выводим ход который обдумываем делаем это только на нулевой глубине т.е. у самой поверхности
	  if (flag_stop_search != 1){
	     nodes_0 = nodes_0 + 1;
	     #if TEST_PRINT_ALL_LINE
	        // для теста вариантов
	        Print_pv_line(1,&list_surplus_moves,pv_save_o.get_pv_best(),nodes,i,depth_max,value,time_nodes);
	     #else
	        // для рабочего режима
	        //Print_pv_line(1,&list_surplus_moves,&pv_best_point,nodes,i,depth_max,value_max);
	     #endif//#if TEST_PRINT_ALL_LINE
       }//if (flag_stop_search != 1){
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

  }//if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

}//for (int i = 0; i < list_surplus_moves.end_list; i++){

//------------------------------------------------------------
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
         // проверяем восстановилась ли доска после прохождения глубины
         Compare_test_bitboard(p_bitboard,&test_bitboard);
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
//------------------------------------------------------------

// если не одного хода сделано не было то производим статическую оценку позиции
// это случай пата или мата
if (flag_is_legal_moves == 0){
    value = 0;
    if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
      value = -(VALUE_MATE - DELTA_MATE * depth);//
     }//if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

    // у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
    pv_save_o.Update_PV_best_max_depth(value,depth);

    return value;
}//	if (flag_is_legal_moves == 0){

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      if(flag_save_best == 1){
          // теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
          pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(flag_save_best == 1){
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    #if HASH
       // кэшируем лучший ход
       hash_o.Save_best_hash(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
				             value_max, UPPER, depth, depth_max);
    #endif//#if HASH

return value_max;

}//int FULLSEARCH::White_root
//	*******************************************************************

//	===================================================================
// перебор на нулевом уровне за черных ищем минимум
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
__int32 FULLSEARCH::Black_root(struct T_Bitboard * const p_bitboard,// представление доски
                           struct T_list_surplus_moves & list_surplus_moves,// список возможных ходов (всевозможные ходы из данного узла)
			               const __int32 alpha,//
			               __int32 beta,// 
			               const unsigned __int8 depth_max// максимальная глубина
			               ){

__int32 value;              // текущая оценка позиции
struct T_undo undo[1];//информация используемая при возврате хода

__int32 value_min = 9999999;// лучшая в узле  оценка позиции  
unsigned __int8 depth_max_new = 0;//новая глубина перебора
bool flag_save_best = 0;// пишем ли лучший вариант
bool flag_is_legal_moves = 0;// флаг существования легальных ходов (если 0 то легальных ходов не было)
bool flag_check = 0;//флаг шаха

const unsigned __int8 depth = 0;// глубина
const unsigned __int8 new_depth = 1; // следующая глубина

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	  unsigned __int8 nodes_0 = 1;            // количество ходов на нулевом уровне
	  struct T_PV pv_best_point;// лучший для данного узла вариант
      struct T_Bitboard test_bitboard;// представление доски. использую для теста
	  unsigned __int8  flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
      const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
      unsigned __int8 i_hash_move = 0; // номер кешируемого хода
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

depth_max_rem = depth_max;
audit = 0;// задержка опроса команды экстренного выхода
lmr_reduction = 0;//
flag_stop_search = 0;//

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
   // это отсечка при экстренном выходе
        if (flag_stop_search == 1) return 0;

// инициализируем массив киллеров
killer_o.Ini_killer(); 

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

Sorting_moves(&list_surplus_moves);
//Sorting_black_moves_root(&list_surplus_moves);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// тестовая печать списка и доски на нулевом уровне
	#if (TEST_L && SAVE_LINE)
     List_Print(depth_max,&list_surplus_moves);
	 //if(depth_max == 3) List_Print(depth,&list_surplus_moves);
	//	Bitboard_print(p_bitboard);
	#endif//#if TEST_L 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if (HASH && SAVE_LINE)
	// работаем с хеш-таблицей
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// т.е. мы в списке ход из хеш-таблицы ставим первым а остальные ходы сдвигаем
	hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

    #if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

      if (flag_insert_hash_move == 0) {
          cout << "хеша не нашли корень черные++++++++++++++++++++++++++++++++++++++++++++++" << "\n";   
       }//if (flag_insert_hash_move == 0) {

       if (flag_insert_hash_move == 1) {
           cout << "коллизия хеша корень черные++++++++++++++++++++++++++++++++++++++++++++++" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 0) {

    #endif//#if HASH_TEST
	#endif//#if HASH



	//-----------------------------------------------

////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//	// тестовая печать списка и доски на нулевом уровне
//	#if (TEST_L && SAVE_LINE)
//	 if(depth_max == 3) List_Print(depth,&list_surplus_moves);
//	//	Bitboard_print(p_bitboard);
//	#endif//#if TEST_L 
////TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	//------------------------------------------------------------
    //запоминаем доску для дальнейшей проверки ее идентичности
    Save_test_bitboard(p_bitboard,&test_bitboard);
	//------------------------------------------------------------
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

 //   // оцениваем количество и тип фигур если меньше чем нужно то отключаем нулевой ход
	//if( (p_bitboard->black_piece[5] == 0) && // если у белых нет ферзей
 //       (p_bitboard->black_piece[4] == 0) && // нет ладей
 //       (p_bitboard->black_piece[3] == 0)  // нет слонов
	//   ){ 
 //       //   cout <<"нулевой ход черные выключили ";
	//       //cout <<"\n";
	//       flag_stop_null_move = 0; // то выключаем нулевой ход
	//}else {
	//       flag_stop_null_move = 1; // иначе он включен
	//}// if( p_bitboard->white_piece[5] == 0 &&

flag_stop_null_move = 1; // иначе он включен


//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for(unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

  // реализуем ход из списка или другими словами генерируем позицию
   moves_o.Onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);
// Bitboard_print(p_bitboard);

// если король под шахом то
   if (bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
    // мы отменяем этот ход как некорректный
       moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);
// записываем оценку в корневой список ходов
list_surplus_moves.sorting_score[i] = 1000000;
   }else{
    // ставим флаг что есть легальные ходы и засчитываем позицию
       flag_is_legal_moves = 1;
       nodes = nodes + 1;
       depth_max_new = depth_max;

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
   	//    печатаем информацию о рассматриваемом ходе и глубине перебора
	      Print_currmove(&list_surplus_moves,nodes_0,i,depth_max);

	//    из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	      pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	//    обновляем хеш ключ
	      hash_o.Modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);

// TEST-----------
// тут ключ считается полностью по новой
// тестировал хеш-таблицу
// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
//hash_o.Start_position_random_key(p_bitboard);
//----------------

		  // записываем ключ для последующего анализа на повтор позиций
          hash_o.Save_hash_three(p_bitboard->hash_key,new_depth);

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

	   //-----------------------------------------------
	   #if (EXTENSION_CHEK && SAVE_LINE)
	   // если король под шахом то увеличиваем максимальную глубину перебора 
	      Extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);
	   #endif//#if EXTENSION_CHEK
	   //-----------------------------------------------

    #if (ALPHA_BETA_ON && SAVE_LINE)
	  if (i == 0) {
    #else
      if (1) {
    #endif//#if ALPHA_BETA_ON

               value = White(p_bitboard,alpha,beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_YES,flag_check,NULL_NO);

      }else{

	           //рекурсивный вызов функции
               value = White(p_bitboard,(beta - 1),beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
						     PV_NO,flag_check,NULL_NO);

               if(value < beta){

	               //рекурсивный вызов функции
		           value = White(p_bitboard,alpha,beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 PV_YES,flag_check,NULL_NO);

	           }//if(value < beta)

      }//if (i == 0)

//     восстанавливаем позицию
       moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

// записываем оценку в корневой список ходов
list_surplus_moves.sorting_score[i] = value;


	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE

// восстанавливаем хеш-ключ доски
p_bitboard->hash_key = key_undo;

        //ищем лучшую позицию за черных ищем минимум
        if(value < value_min){
           value_min = value;
           i_hash_move = i; // номер кешируемого хода
           from = ((list_surplus_moves.move[i] >> 12) & 63);
           to = ((list_surplus_moves.move[i] >> 18) & 63);
  
           //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
           #if (ALPHA_BETA_ON && !TEST_PRINT_ALL_LINE)
              // черные пытаются улучшить бету
              if (value < beta){
                 beta = value;
                 flag_save_best = 1;
                 // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
                 pv_save_o.Update_PV_best_point(&pv_best_point);
	             // проверяем совпадение варианта и оценки
                 if (value != pv_save_o.get_pv_best()->score_move){
		            cout << "ERROR black value " ;
		            cout << "\n";
	             }//if(value != pv_best.score_move){

              }//if (value < beta){
           #else//#if ALPHA_BETA_ON
             flag_save_best = 1;
             // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
             pv_save_o.Update_PV_best_point(&pv_best_point);

	         // проверяем совпадение варианта и оценки
             if (value != pv_save_o.get_pv_best()->score_move){
		         cout << "ERROR black value " ;
		         cout << "\n";
	         }//if(value != pv_best.score_move){

           #endif//#if ALPHA_BETA_ON
           //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        }//if (value < value_min){ 

	   //--------------------------------------------------------------------
	   // выводим ход который обдумываем делаем это только на нулевой глубине т.е. у самой поверхности
	   if (flag_stop_search != 1){
	       nodes_0 = nodes_0 + 1;
	       #if TEST_PRINT_ALL_LINE
	        // для теста вариантов
	          Print_pv_line(0,&list_surplus_moves,pv_save_o.get_pv_best(),nodes,i,depth_max,value,time_nodes);
	       #else
	        // для рабочего режима
	          //Print_pv_line(0,&list_surplus_moves,&pv_best_point,nodes,i,depth_max,value_min);
	       #endif//#if TEST_PRINT_ALL_LINE
        }//if (flag_stop_search != 1){

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

  }//if (bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){

}//for(int i = 0; i < list_surplus_moves.end_list; i++){

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
         // проверяем восстановилась ли доска после прохождения глубины
         Compare_test_bitboard(p_bitboard,&test_bitboard);
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

// если не одного хода сделано не было то производим статическую оценку позиции
// это случай пата или мата
if (flag_is_legal_moves == 0){
  value = 0;
  if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
    value = (VALUE_MATE - DELTA_MATE * depth);// может это слишком большое число?
  }//if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){

  // у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
  pv_save_o.Update_PV_best_max_depth(value,depth);

  return value;
}//	if (flag_is_legal_moves == 0){

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      if(flag_save_best == 1){
          // теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
          pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(flag_save_best == 1){
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    #if HASH
      //кэшируем лучший ход
      hash_o.Save_best_hash(i_hash_move, p_bitboard->hash_key, &list_surplus_moves, 
				            value_min, UPPER, depth, depth_max);
    #endif//#if HASH


return value_min;
}//int FULLSEARCH::Black_root
//	*******************************************************************


//	===================================================================
// ищем максимум в узле. перебор за белых 
__int32 FULLSEARCH::White(struct T_Bitboard * const p_bitboard,// представление доски
					  __int32 alpha,//
		              const __int32 beta,//
		              const unsigned __int8 depth_max,// максимальная глубина
		              const unsigned __int8 depth,// глубина
		              const __int32 description_move,// тип хода который привел к данной позиции
		              const bool flag_pv_f,// основной ли это вариант (pv)
		              const bool old_flag_check,//флаг шаха
		              const bool flag_null_m// флаг нулевого хода
		              ){

__int32 value;              // текущая оценка позиции
__int32 value_max = -9999999;   // оценка позиции лучшая в узле 
struct T_undo undo[1]; // использую при возврате хода
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
bool flag_is_legal_moves = 0;           // флаг существования легальных ходов
bool flag_save_best = 0;           // пишем ли лучший вариант
const unsigned __int8 new_depth = depth + 1; // следующая глубина

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      unsigned __int8 depth_max_new = 0;//новая глубина перебора
      bool flag_check = 0;//флаг шаха
	  struct T_PV pv_best_point;// лучший для данного узла вариант

	  bool taking_passage_yes_mem = 0;// использую для запоминания перед нулевым ходом
	      unsigned __int8 taking_passage_y_mem = 0;
	      unsigned __int8 taking_passage_x_mem = 0;
	  
      // все для кеширования
      const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
      unsigned __int8  flag_insert_hash_move = 0; // поместили ли ход из хеш-таблицы на первую позицию
      __int32 hash_score = -1000000000;// хеш-оценка позиции
      unsigned __int8 i_hash_moves_1;// ход который улучшает альфу (1- улучшили альфу) нужно для кеширования 
      unsigned __int8 i_hash_moves_3;// ход дающий локальный максимум (3-просто локальный максимум )нужно для кеширования
      unsigned __int8 hash_moves_test13;// для теста
      unsigned __int8 test_flag_hash_moves;// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
      unsigned __int8 hash_depth;// максимальная глубина
      unsigned __int8 hash_depth_max;// максимальная глубина
//      unsigned __int64 test_key_control;
//      int dynamic_null_move_reduction;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss


	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (flag_stop_search == 1) return 0;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss


//--------------------------------------------
// на предельной глубине производим статическую оценку позиции
if (depth >= depth_max){
 	   //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	   #if SAVE_LINE
         value = Max_depth_white(p_bitboard,alpha,beta,flag_pv_f,depth);
	   #endif//#if SAVE_LINE 
	   //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
       return value ;
}//	if (depth >= depth_max){
//--------------------------------------------

//-----------------------------------------------
    #if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
      // обнаружили повторение позиции
	  if ( hash_o.Analysis_hash_string_moves(depth) ){
        // у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
        if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(0 // оценка ничья
			                                             ,depth);
		return 0;
	  }//if ( hash_o.Analysis_hash_string_moves(depth) )
    #endif//#if ALPHA_BETA_ON
//-----------------------------------------------

	//--------------------------------------------
	#if (FUTILITY_PRUNING && SAVE_LINE)
     //
	 if((depth_max - depth) == FUTILITY_DEPTH){
		if( (description_move != MOVE_IS_CAPTURE)
			&& (flag_check == CHECK_NO) 
			&& (flag_pv_f == PV_NO)){

			if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) - FUTILITY_MARGIN) 
				>= beta ){
				return beta;
			}//	if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) - FUTILITY_MARGIN) >= beta )

		}//	if((flag_null_m == NULL_NO) && (description_move != MOVE_IS_CAPTURE)

	  }//if((depth_max - depth) == FUTILITY_DEPTH)
	#endif//#if FUTILITY_PRUNING
	//--------------------------------------------



	//--------------------------------------------
#if (HASH_SCORE)
	#if (HASH && SAVE_LINE)
// отсечку по хеш оценке не используем для главного варианта
// главным образом для достоверности оценки и линии варианта и для полноты картины
// считаю обрывочный главный вариант неприемлемым.
      if(!flag_pv_f){ 
	     // смотрим не встречалась ли нам такая позиция если встречалась то 
		 // пытаемся применить сохраненную оценку
	     hash_score = hash_o.Look_hash_score_white(
			                          p_bitboard->hash_key,depth,depth_max,
									  alpha,beta,test_flag_hash_moves,hash_depth,
									  hash_depth_max);

		  if (hash_score != -1000000000 ) {
//return_hash_score = return_hash_score + 1 ;
           #if (!HASH_TEST_SCORE)
			  return hash_score;
           #endif//#if (!HASH_TEST_SCORE)

		  }//if (hash_score != -1000000000 ) {
       }//if(!flag_pv_f){
     #endif//#if HASH
 #endif//#if (HASH_SCORE)
	//--------------------------------------------

	//--------------------------------------------
	//if(!flag_pv_f){
 //     // мега редукция по оценке :)
 //     // идея в том что начиная с глубины 8 начинаем пропускать ходы плохие по материалу
 //    if ( depth > 8 )
 //        if((p_bitboard->eval - 700) >= beta ) return beta;

	//}//if(!flag_pv_f){
	//--------------------------------------------


	//--------------------------------------------
	#if (NULL_MOVE && SAVE_LINE)
	// очень эффективная эвристика известная под именем нулевого хода
    // я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
    // суть его в том что
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать

    if ((flag_stop_null_move == NULL_YES) // если цугцванг нам не грозит
	 && (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	 && (depth_max > NULL_REDUCTION ) // иначе уходим в минус а у нас глубина всегда положительна
	 && ((depth_max - depth) >= NULL_DEPTH ) // если до максимальной глубины расстояние достаточное
	 && (flag_pv_f == PV_NO) // если мы не в главном варианте
	 && (description_move != MOVE_IS_CAPTURE) // если ход не взятие
	 && (old_flag_check == CHECK_NO) // если шаха нет
//	 && (p_bitboard->eval >= beta) // дикая потеря эффективности. почему не понимаю
                                   // наверное дело в том что я не использую статической
								   // составляющей оценки а только материал
		){ // тогда вызываем нулевой ход

// запоминаем флаги взятия на проходе и обнуляем разрешение.
		taking_passage_yes_mem = p_bitboard->taking_passage_yes;
		taking_passage_x_mem = p_bitboard->taking_passage_x;
		taking_passage_y_mem = p_bitboard->taking_passage_y;

// меняем хеш-ключ под нулевой ход -------------------------
// учитываем взятие на проходе
if( p_bitboard->taking_passage_yes ){
	p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//if( p_bitboard->taking_passage_yes )

// учитываем цвет хода
// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
// некоторые позиции отличаются только цветом хода
 p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);
//---------------------------------------------

		p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

        p_bitboard->colour_move = !p_bitboard->colour_move;

        const __int64 nodes_null_rem = nodes;

// TEST тестировал изменение хеш-ключа
// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
//hash_o.Start_position_random_key(p_bitboard);

// не динамическая редукция
value = Black(p_bitboard,alpha,(alpha + 1),(depth_max - NULL_REDUCTION),
			         new_depth,description_move, 
					 PV_NO,old_flag_check,NULL_YES); 

//--------------------------------------------------------
// динамическая редукция нулевого хода при дистанции до максимальной глубины
//больше равной 5 редуцируем 4 иначе 3
//dynamic_null_move_reduction = 
// ((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
// ? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
//
//value = Black(p_bitboard,alpha,(alpha + 1),(depth_max - dynamic_null_move_reduction),
//			         new_depth,description_move, 
//					 PV_NO,flag_check,NULL_YES); 
//--------------------------------------------------------

// узлы насчитанные в нулевом поиске не учитываем
        nodes = nodes_null_rem;

// TEST
//cout << "p_bitboard->eval "<< p_bitboard->eval << "\n";
//cout << "i_move= "<< i_move << "\n";
//cout << "description_move= "<< description_move << "\n";
//cout << "check= "<< check << "\n";
//cout << "       "<< "\n";
//
//if(value < beta){
//cout << "осталось "<< "\n";
//cout << "depth_max= "<< depth_max <<"\n";
//cout << "depth= "<< depth <<"\n";
//cout << "value= "<< value <<"\n";
//cout << "beta= "<< beta <<"\n";
//cout << "       "<< "\n";
//}

		p_bitboard->taking_passage_yes = taking_passage_yes_mem;
		p_bitboard->taking_passage_x = taking_passage_x_mem ;
		p_bitboard->taking_passage_y = taking_passage_y_mem ;

        p_bitboard->colour_move = !p_bitboard->colour_move;

// восстанавливаем хеш-ключ доски
p_bitboard->hash_key = key_undo;

		//был ход черных при котором они пытались найти минимальную оценку т.е. улучшали бету
		//белые пропустили свой ход дав таким образом черным фору
		//теперь опять ход черных и если мы не смогли улучшить бету т.е. наша оценка больше
		//или равна бете значит можно отсечь т.к. ветка нам не интересна
		//потому что если даже с такой форой не смогли улучшить то без форы и подавно не сможем 
		if(value >= beta){

			  //TEST
              //cout << "отсекли "<< "\n";
              //cout << "depth_max= "<< depth_max <<"\n";
              //cout << "depth= "<< depth <<"\n";
              //cout << "value= "<< value <<"\n";
              //cout << "beta= "<< beta <<"\n";
              //cout << "       "<< "\n";

//         return beta;

//------------------------------
if ((depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1) ) return beta;

       // пробуем обойти цугцванг
       value = White(p_bitboard,(beta - 1),beta,(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
			          depth,description_move, 
					  PV_NO,old_flag_check,NULL_YES);

       if(value >= beta)  return beta;
//------------------------------

		}//if(value >= beta)

	}//    if ((flag_null_m == NULL_NO) && ((depth_max - depth) >= NULL_DEPTH ) && 

	#endif//#if NULL_MOVE
	//--------------------------------------------


//насчитываем список избыточных ходов
// множество этих списков и создают дерево перебора
bitboard_generator_o.Generator_captures_white(p_bitboard,&list_surplus_moves);
bitboard_generator_o.Generator_move_white(p_bitboard,&list_surplus_moves);
Sorting_moves(&list_surplus_moves);

 
	//-----------------------------------------------
	#if (KILLER && SAVE_LINE)
	// если нет взятий или взятие плохое
    //то киллер вставляем на первую позицию иначе на вторую
    if((list_surplus_moves.end_captures == CAPTURE_NO) || 
		(list_surplus_moves.sorting_score[0] < 0)){

		killer_o.Insert_killer(&list_surplus_moves,depth,0);

	}else{

		killer_o.Insert_killer(&list_surplus_moves,depth,1);

	}//   if((list_surplus_moves.end_captures == CAPTURE_NO) || 

	#endif//#if KILLER
	//-----------------------------------------------

	//--------------------------------------------
	#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	// точнее мы поднимаем ход из хеш таблицы на первое место сдвигая остальные вниз
	hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

	// для полного теста нужно отключать отсечку по оценке
    #if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

       if (flag_insert_hash_move == 1) {
           cout << "коллизия хеша белые" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 0) {
    #endif//#if HASH_TEST

	#endif//#if HASH
	//--------------------------------------------

#if (LMR && SAVE_LINE)
   // ход из хеш-таблицы, лучшее взятие, два киллера, все взятия не редуцируются
   lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
   //lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR

//int r;

//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

//--------------------------------------------
// к моему удивлению выигрыш где то полуход да и то под сомнением
// а при чистой альфа бете вообще только тормозит по моему
	//if(!flag_pv_f){
 //      // мега редукция можно назвать это супер лмр(lmr) :)
 //     // идея в том что начиная с глубины 6 начинаем пропускать последние ходы
	//  // но четыре хода остается в любом случае
	// r = 5 + list_surplus_moves.end_list - depth;
	// if ( r < 0) r = 0;
 //    if ( i >= 4 + r ) continue;
	//}//if(!flag_pv_f){
//--------------------------------------------

  // реализуем ход из списка другими словами генерируем позицию
  moves_o.Onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);

  // если король под шахом то
  if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

     // мы отменяем этот ход как некорректный
     moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

  }else{

   // ставим флаг что есть легальные ходы и засчитываем позицию
   flag_is_legal_moves = 1;
   nodes = nodes + 1;

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	 // из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	 if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	 hash_o.Modification_random_key(1,i,&list_surplus_moves,p_bitboard,undo);

//TEST-----------
#if HASH_TEST_KEY

  unsigned __int64 test_key_control = p_bitboard->hash_key;

 //тут ключ считается полностью по новой
 //тестировал хеш-таблицу
 //инициализируем ключ начальной позиции(мы пишем его в структуру доски)
  hash_o.Start_position_random_key(p_bitboard);

  if(test_key_control != p_bitboard->hash_key){
       cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
       cout << "collision white depth = " << depth << "\n";
       exit(1);
  }//if(key_control != p_bitboard->hash_key){

#endif//#if HASH_TEST_KEY
//----------------------------

	 // записываем ключ для последующего анализа на повтор позиций
     hash_o.Save_hash_three(p_bitboard->hash_key,new_depth); 

	 // каждый раз сбрасываем новую максимальную глубину что бы продлевать ветку 
	 // а не весь куст
     depth_max_new = depth_max;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

   //-----------------------------------------------
   #if (EXTENSION_CHEK && SAVE_LINE)
	// если король под шахом то увеличиваем на 1 максимальную глубину перебора
	    Extension_for_chek_white(p_bitboard,depth_max,depth_max_new,flag_check);
   #endif//#if EXTENSION_CHEK
   //-----------------------------------------------

	//-----------------------------------------------
	#if (EXTENSION_CAPTURES && SAVE_LINE)
      if(flag_pv_f == PV_YES){//

//         if (description_move == 2){

             // если ход это взятие
             if (((list_surplus_moves.move[i]>>6) & 63) == 2){
                 // если взятая фигура лучше берущей
                 if ( ((list_surplus_moves.move[i]>>24) & 7) // ходим
                        <= (list_surplus_moves.move[i] & 63) ){ // взятая

                           depth_max_new = depth_max + 1;
                          //Extension_for_captures(((list_surplus_moves.move[i]>>6) & 63)
		                  //                  ,description_move,depth_max,depth_max_new);

				 }//if ( ((list_surplus_moves.move[i]>>24) & 7)

			 }//if (((list_surplus_moves.move[i]>>6) & 63) == 2){

//		 }//if (description_move == 2){

	  }//if(flag_pv_f == PV_YES)
    #endif//#if EXTENSION_CAPTURES
	//-----------------------------------------------

// попытка сделать продление на единственном ответе 
// с единственным ответом у меня неразрешимая проблема
// дело в том что у меня нет уклонений от шаха а генерировать все ходы
// по моему непозволительная роскошь
  //   if(flag_pv_f == PV_YES){//

  //      if ((flag_check) // если нам шах
		//	&&(list_surplus_moves.end_list < 10)  // и ходов меньше 10
		//	&&(depth_max_new < 51) ){			
  //     //   cnt_single = 0; 
		//		depth_max_new = depth_max + 1;
		//}//if ((flag_check) // если нам шах

	 // }//if(flag_pv_f == PV_YES)




//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
 #if !SAVE_LINE
    // рекурсивный вызов функции
    value = Black(p_bitboard,alpha,beta,depth_max,new_depth,
		          ((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
				  flag_check,flag_null_m);
 #endif//#if !SAVE_LINE


//------------------------------------------------------
    #if (!LMR && SAVE_LINE)

       #if (ALPHA_BETA_ON && SAVE_LINE)
	      if (i == 0) {
       #else
          if (1) {
       #endif//#if ALPHA_BETA_ON

             value = Black(p_bitboard,alpha,beta,depth_max_new,new_depth,
			              ((list_surplus_moves.move[i]>>6) & 63),
					        flag_pv_f,flag_check,flag_null_m);

          }else{

             // рекурсивный вызов функции
	         value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
// TEST
//value = Black(p_bitboard,alpha,beta,depth_max_new,
				           new_depth,((list_surplus_moves.move[i]>>6) & 63),
						   PV_NO,flag_check,flag_null_m);

// суть поиска с нулевым ходом в том что мы не ждем что вариант улучшит 
// альфу поэтому считаем его
// с максимально узким окном если нас ждет сюрприз то пересчитываем по полному
	         if((value > alpha) && (value < beta)){
 
            
//cout << "white depth = " << depth << "\n";
//cout << "value = " << value << "\n";
//cout << "alpha = " << alpha << "\n";
//cout << "beta = " << beta << "\n";

		          value = Black(p_bitboard,alpha,beta,depth_max_new,
					           new_depth,((list_surplus_moves.move[i]>>6) & 63),
							   flag_pv_f,flag_check,flag_null_m);

//cout << "value mod = " << value << "\n";
//cout << "   " <<"\n";

	         }//if ((value > alpha) && (value < beta))
          }//if (i==0)
    #endif//#if !LMR
    //------------------------------------------------------

	//------------------------------------------------------
	#if  (LMR && SAVE_LINE)

	  if(flag_pv_f){
          // если мы в главном варианте то считаем по методике с нулевым окном
		  // т.е. от alpha до beta = alpha + 1

	      if (i == 0) {
            // первый ход смотрим на полной глубине и с полным окном
		    // мы предполагаем что если он первый то лучший и достоин полного просмотра
             value = Black(p_bitboard,alpha,beta,depth_max_new,
				           new_depth,((list_surplus_moves.move[i]>>6) & 63),
					       PV_YES,flag_check,flag_null_m);

          }else{

             // пытаемся обойтись перебором с нулевым окном
	         value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
				           new_depth,((list_surplus_moves.move[i]>>6) & 63),
						   PV_NO,flag_check,flag_null_m);

              // суть поиска с нулевым ходом в том что мы не ждем что вариант улучшит 
              // альфу поэтому считаем его с максимально узким т.е. нулевым окном
              //  если нас ждет сюрприз то пересчитываем c полным окном
	         if(value > alpha){
                  //
		          value = Black(p_bitboard,alpha,beta,depth_max_new,
					           new_depth,((list_surplus_moves.move[i]>>6) & 63),
							   PV_YES,flag_check,flag_null_m);

	         }//if (value > alpha)
		  }//if (i == 0) 

	  }else{//if(flag_pv_f)
          // если мы не в главном варианте то считаем по методике LMR

          // после сортировки первый ход из хеш таблицы второй это 
		  // лучшее взятие третий четвертый это киллеры дальше идут 
		  // просто взятия и дальше все остальные ходы
          // все это учитывается при задании lmr_reduction
          // мы редуцируем только остальные(тихие) ходы если они не шах 
		  // на этой и предыдущей глубине
          if((i > lmr_reduction) 
			 && (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
             && ((depth_max_new - depth) >= LMR_DEPTH )
			 ){
               // ищем с редукцией глубины и с нулевым окном
               value = Black(p_bitboard,alpha,(alpha + 1),(depth_max_new - LMR_REDUCTION_D),
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

         }else{
               // это просто формальный прием что бы выполнилось условие value > alpha
			   // для случая когда ход не редуцируем по лмр(lmr)
               value = alpha + 1; 
         }//if((i > lmr_reduction)

// условия персчета многократно менялись
// в начале было if((value > alpha) && (value < beta)) это грубейшая ошибка потому что
// вариант не пересчитывался в случае value >= beta и получалось недостоверное отсечение
// потом было value >= beta но это только вносит неясность хотя работает правильно
// сейчас у нас value > alpha что очень наглядно.

	         if(value > alpha){
                  //
		          value = Black(p_bitboard,alpha,(alpha + 1),depth_max_new,
					           new_depth,((list_surplus_moves.move[i]>>6) & 63),
							   PV_NO,flag_check,flag_null_m);

	         }//if (value > alpha)

      }//if(flag_pv_f)

	#endif//#if  LMR
	//------------------------------------------------------



    // восстанавливаем позицию
    moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
 
// восстанавливаем хеш-ключ доски
p_bitboard->hash_key = key_undo;

    //--------------------------------------------
    if (value > value_max){

    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    #if ALPHA_BETA_ON
       if (value > alpha){

         //-----------------------------------------------
	     if(value >= beta) {        
	         #if KILLER
	            // пишем киллера
			 if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE){
				 killer_o.Save_killer(i,&list_surplus_moves,depth);
			 }//if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE)
	         #endif//#if KILLER

             #if HASH
               // кэшируем лучший на данный момент ход
               hash_o.Save_best_hash(i, p_bitboard->hash_key, &list_surplus_moves, 
				                     value, LOWER, depth, depth_max);
             #endif

// TEST
//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 ))
//cout << "отсечение по хеш таблице белые " << "\n";

		     return value; 
	     }//if(value >= beta) 
         //-----------------------------------------------

// TEST------------
#if HASH_TEST_SCORE
// конечно что бы этот тест работал нельзя выше отсекаться по хеш оценке :)
// если мы должны отсекаться но тем не менее ход не вызвал отсечку то пишем об этом
// подробнее за белых
// суть в том что мы взяли оценку из хеш-таблицы и увидели что она больше беты т.е. вызовет
// отсечение. теперь мы просчитываем ее и если оценка подтверждается то до этого места
// счет не дойдет так как отсечется раньше
		 if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 )){
			 cout << "осечка отсечения по хеш таблице белые " << "\n";
             cout << "depth= " << depth << "\n";
             cout << "depth_max_rem= " << depth_max_rem << "\n";
             cout << "hash_depth= " << (__int32)hash_depth << "\n";
             cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
             cout << "hash_score= " << hash_score  << "\n";
             cout << "value= " << value << "\n";
             cout << "alpha= " << alpha << "\n";
             cout << "beta= " << beta << "\n";
             cout << "  " << "\n";

             exit(1);
		 }//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 )){

#endif//#if HASH_TEST_SCORE
//-----------------


         alpha = value;
         flag_save_best = 1;
         i_hash_moves_1 = i;// ход который улучшает альфу (1- улучшили альфу) нужно для кеширования 

         // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
        if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
 
      }//if (value > alpha)

    #else // #if ALPHA_BETA_ON
        // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
        if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
        flag_save_best = 1;
        hash_moves_1 = i;// при тестировании полного перебора
    #endif// #if ALPHA_BETA_ON
    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

       value_max = value;
       i_hash_moves_3 = i;// ход дающий локальный максимум (3-просто локальный максимум)нужно для кеширования

   }//if (value > value_max)

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

 }//if (bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO)

}//for (int i = 0; i<list_surplus_moves.end_list; i++)

//--------------------------------------------
// обработка случая когда не было легальных ходов
// если не одного хода сделано не было то производим статическую оценку позиции
// это случай пата или мата
if (flag_is_legal_moves == 0){
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
       value = 0;
       if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
           value = -(VALUE_MATE - DELTA_MATE * depth);//
       }//if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

       // у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
     if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value,depth);
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    return value;
}//if (flag_is_legal_moves == 0){
//--------------------------------------------

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
      if(flag_save_best == 1){
          // теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
       if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(flag_save_best == 1){
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

	#if (HASH && SAVE_LINE)
	  if( flag_save_best == 1 ){
	    // кэшируем ход улучшающий альфу т.е. флаг 1
	    hash_o.Save_best_hash(i_hash_moves_1, p_bitboard->hash_key, &list_surplus_moves, 
			                  alpha, EXACT, depth, depth_max);
	  }else{
	    // кэшируем ход дающий локальный максимум т.е. флаг 3
	    hash_o.Save_best_hash(i_hash_moves_3, p_bitboard->hash_key, &list_surplus_moves, 
			                  value_max, UPPER, depth, depth_max);
	  }//if(flag_save_best == 1){
	#endif//#if HASH

// это тест показал что есть позиции где расходятся ходы и оценки
// и даже если а б одинаковы и длина поиска та же и без нулевого окна
//все равно ходы как и оценки могут быть разными
// все это для случая оценки меньше альфы
//
// TEST------------
#if HASH_TEST_SCORE

// конечно что бы этот тест работал нельзя выше отсекаться по хеш оценке :)
/*
взятая из хеш-таблицы оценка меньше альфы
если в процессе поиска она вдруг стала больше альфы то мы об этом пишем
*/
		 if ( hash_score != -1000000000 ){
         //if ( 0 ){

            hash_moves_test13 = hash_moves_3;
            if(flag_save_best == 1) hash_moves_test13 = hash_moves_1;

			//if ( hash_moves_test13 != 0 ) {
            //if ( hash_score != value_max ) {
               if ( value_max > alpha ) {
				cout << "хешированный и найденный ходы дающие максимум разные белые " << "\n";
               cout << "p_bitboard->hash_key= " << p_bitboard->hash_key << "\n";
               cout << "depth= " << depth << "\n";
               cout << "depth_max_rem= " << depth_max_rem << "\n";
               cout << "hash_depth= " << (__int32)hash_depth << "\n";
               cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
               cout << "test_flag_hash_moves= " << test_flag_hash_moves  << "\n";
               cout << "hash_score= " << hash_score  << "\n";
               cout << "value_max= " << value_max << "\n";
               cout << "alpha= " << alpha << "\n";
               cout << "beta= " << beta << "\n";

  cout << "ход из хеш таблицы " << "\n";
               cout << "((list_surplus_moves.move[0]>>6)& 63)= " 
				    << ((list_surplus_moves.move[0]>>6)& 63) << "\n";
               cout << "Ходящая фигура " << util_o.int_to_char_board(((list_surplus_moves.move[0]>>24)& 7),1)<< "\n";
               
               cout <<util_o.x_to_notation(((list_surplus_moves.move[0]>>12)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[0]>>12)& 63)/8);
               cout <<util_o.x_to_notation(((list_surplus_moves.move[0]>>18)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[0]>>18)& 63)/8) << "\n";

cout << "ход найденный в поиске " << "\n";
cout << "hash_moves_test12= " << hash_moves_test13  << "\n";
               cout << "((list_surplus_moves.move[i]>>6)& 63)= " 
				    << ((list_surplus_moves.move[hash_moves_test13]>>6)& 63) << "\n";
               cout << "Ходящая фигура " << util_o.int_to_char_board(((list_surplus_moves.move[hash_moves_test13]>>24)& 7),1)<< "\n";
               
               cout <<util_o.x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>12)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>12)& 63)/8);
               cout <<util_o.x_to_notation(((list_surplus_moves.move[hash_moves_test13]>>18)& 63)%8);
               cout <<(1 + ((list_surplus_moves.move[hash_moves_test13]>>18)& 63)/8) << "\n";
               cout << "  " << "\n";     
  exit(1);
			}//if ( hash_moves_test13 != 0 ) {

		 }//if ( hash_score != -1000000000 ){

#endif //#if HASH_TEST_SCORE
//-----------------

return value_max;

}//int FULLSEARCH::White
//	*******************************************************************

//	===================================================================
// ищем минимум в узле. перебор за черных
__int32 FULLSEARCH::Black(struct T_Bitboard * const p_bitboard,// представление доски
					  const __int32 alpha,//
		              __int32 beta,//
		              const unsigned __int8 depth_max,// максимальная глубина
		              const unsigned __int8 depth,// глубина
		              const __int32 description_move,// тип хода который привел к данной позиции
		              const bool flag_pv_f,// основной ли это вариант (pv)
		              const bool old_flag_check,//флаг шаха
		              const bool flag_null_m// флаг нулевого хода
		              ){

__int32 value;              // текущая оценка позиции
__int32 value_min = 9999999;   // оценка позиции лучшая в узле 
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
bool flag_is_legal_moves = 0;           // флаг существования легальных ходов
bool flag_save_best = 0;           // пишем ли лучший вариант
const unsigned __int8 new_depth = depth + 1; // следующая глубина

	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
    unsigned __int8 depth_max_new = 0;//новая глубина перебора
    bool flag_check = 0;//флаг шаха
	struct T_PV pv_best_point;// лучший для данного узла вариант
	bool taking_passage_yes_mem = 0;
	   unsigned __int8 taking_passage_y_mem = 0;
	   unsigned __int8 taking_passage_x_mem = 0;
	unsigned __int8 flag_insert_hash_move = 0;        // поместили ли ход из хеш-таблицы на первую позицию
    const unsigned __int64 key_undo = p_bitboard->hash_key;   // запоминаем хеш-ключ даннного узла
    __int32 hash_score = -1000000000;// хеш-оценка позиции
    unsigned __int8 i_hash_moves_1;// ход который улучшает бету (1- улучшили бету) нужно для кеширования 
    unsigned __int8 i_hash_moves_3;// ход дающий локальный минимум (3-просто локальный минимум)нужно для кеширования
    unsigned __int8 test_flag_hash_moves;// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
    unsigned __int8 hash_depth;// максимальная глубина
    unsigned __int8 hash_depth_max;// максимальная глубина
//    unsigned __int64 test_key_control;// запоминаем хеш-ключ даннного узла
//    int dynamic_null_move_reduction;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss



 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (flag_stop_search == 1) return 0;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

//--------------------------------------------
// на предельной глубине производим статическую оценку позиции
if (depth >= depth_max){
  	      //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	      #if SAVE_LINE
             value = Max_depth_black(p_bitboard,alpha,beta,flag_pv_f,depth);
	      #endif//#if SAVE_LINE 
	      //sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
          return value ;
}//if (depth >= depth_max){
//-----------------------------------------------

//-----------------------------------------------
       #if (ALPHA_BETA_ON && SAVE_LINE && REPETITION)   
// обнаружили повторение позиции
	if ( hash_o.Analysis_hash_string_moves(depth) ){
        // у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
        if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(0 // оценка ничья
			                                             ,depth);
		return 0;
	}//if ( hash_o.Analysis_hash_string_moves(depth) )
      #endif//#if ALPHA_BETA_ON
//-----------------------------------------------

	//--------------------------------------------
	#if(FUTILITY_PRUNING && SAVE_LINE)
	//
	if((depth_max - depth) == FUTILITY_DEPTH){

		if((description_move != MOVE_IS_CAPTURE) 
			 && (flag_check == CHECK_NO) 
			 && (flag_pv_f == PV_NO)){

			if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) + FUTILITY_MARGIN) 
				<= alpha ){ 
				return alpha;
			}//if((p_bitboard->eval + eval_o.Eval_State(p_bitboard) + FUTILITY_MARGIN) <= alpha ){ 

		}//if((flag_null_m == NULL_NO) && (description_move != MOVE_IS_CAPTURE) 
		 //	    && (flag_check == CHECK_NO) && (flag_pv_f == PV_NO)){

	}//if((depth_max-depth) == FUTILITY_DEPTH){
	#endif//#if FUTILITY_PRUNING
	//--------------------------------------------


	//--------------------------------------------
#if (HASH_SCORE)
	#if (HASH && SAVE_LINE)
// отсечку по хеш оценке не используем для главного варианта
// главным образом для достоверности оценки и линии варианта и для полноты картины
// считаю обрывочный главный вариант неприемлемым.
      if(!flag_pv_f){ 
	     // смотрим не встречалась ли нам такая позиция если встречалась то 
		 // пытаемся применить сохраненную оценку
	     hash_score = hash_o.Look_hash_score_black(
			                          p_bitboard->hash_key,depth,depth_max,
									  alpha,beta,test_flag_hash_moves,hash_depth,
									  hash_depth_max);

		  if (hash_score != -1000000000 ) {
//return_hash_score = return_hash_score + 1 ;
		   #if (!HASH_TEST_SCORE)
			  return hash_score;
           #endif//#if (!HASH_TEST_SCORE)
		  }//if (hash_score != -1000000000 ) {
	  }//if(!flag_pv_f){ 
     #endif//#if HASH
 #endif//#if (HASH_SCORE)
	//--------------------------------------------


	//--------------------------------------------
	//if(!flag_pv_f){
 //     // мега редукция по оценке :)
 //     // идея в том что начиная с глубины 8 начинаем пропускать ходы плохие по материалу
 //    if ( depth > 8 )
 //        if((p_bitboard->eval + 700) <= alpha ) return alpha;

	//}//if(!flag_pv_f){
	//--------------------------------------------




//--------------------------------------------
	#if (NULL_MOVE && SAVE_LINE)
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
    if ((flag_stop_null_move == NULL_YES) // если цугцванг нам не грозит
	 && (flag_null_m == NULL_NO) // если мы не в ветке нулевого хода
	 && (depth_max > NULL_REDUCTION ) // иначе уходим в минус а у нас глубина всегда положительна
	 && ((depth_max - depth) >= NULL_DEPTH ) // если до максимальной глубины расстояние достаточное
	 && (flag_pv_f == PV_NO) // если мы не в главном варианте
	 && (description_move != MOVE_IS_CAPTURE) // если ход не взятие
	 && (old_flag_check == CHECK_NO) // если шаха нет
//	 && (p_bitboard->eval <= alpha) //
		){ // тогда вызываем нулевой ход

        taking_passage_yes_mem = p_bitboard->taking_passage_yes;
        taking_passage_x_mem = p_bitboard->taking_passage_x;
        taking_passage_y_mem = p_bitboard->taking_passage_y;

// меняем хеш-ключ под нулевой ход -------------------------
// учитываем взятие на проходе
if( p_bitboard->taking_passage_yes ){
	p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0xA3853B80F17F58EE);
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     p_bitboard->hash_key = p_bitboard->hash_key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//
// учитываем цвет хода
// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
// некоторые позиции отличаются только цветом хода
 p_bitboard->hash_key = p_bitboard->hash_key ^ U64(0x7C0228DD624EC390);

//---------------------------------------------


        p_bitboard->taking_passage_yes = 0;
		p_bitboard->taking_passage_x = 0;
		p_bitboard->taking_passage_y = 0;

        p_bitboard->colour_move = !p_bitboard->colour_move;
	
        const __int64 nodes_null_rem = nodes;

// TEST тестировал изменение хеш-ключа
// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
//hash_o.Start_position_random_key(p_bitboard);


         value = White(p_bitboard,(beta - 1),beta,(depth_max - NULL_REDUCTION),
			          new_depth,description_move, 
					  PV_NO,old_flag_check,NULL_YES);

//--------------------------------------------------------
//dynamic_null_move_reduction = 
// ((depth_max - depth) >= SWITCH_NULL_REDUCTION_1_2 
// ? NULL_REDUCTION_ONE : NULL_REDUCTION_TWO); // 
//
//         value = White(p_bitboard,(beta - 1),beta,(depth_max - dynamic_null_move_reduction),
//			          new_depth,description_move, 
//					  PV_NO,flag_check,NULL_YES);
//--------------------------------------------------------

        nodes = nodes_null_rem;

        p_bitboard->taking_passage_yes = taking_passage_yes_mem;
        p_bitboard->taking_passage_x = taking_passage_x_mem ;
        p_bitboard->taking_passage_y = taking_passage_y_mem ;

        p_bitboard->colour_move = !p_bitboard->colour_move;

// восстанавливаем хеш-ключ доски
p_bitboard->hash_key = key_undo;


        //был ход белых при котором они пытались найти максимальную оценку т.е. улучшали альфу
		//черные пропустили ход
        //теперь опять ход белых и если мы не смогли улучшить альфу т.е. наша оценка меньше
        //или равна альфе значит можно отсечь т.к. ветка нам не интересна
        if(value <= alpha){ 
              //cout << "отсекли "<< "\n";
              //cout << "depth_max= "<< depth_max <<"\n";
              //cout << "depth= "<< depth <<"\n";
              //cout << "value= "<< value <<"\n";
              //cout << "alpha= "<< alpha <<"\n";
              //cout << "       "<< "\n";

//         return alpha;

//------------------------------
if( (depth_max - depth) < (VERIFICATOIN_SEARCH_REDUCTION + 1) )  return alpha;

        // пробуем обойти цугцванг
	    value = Black(p_bitboard,alpha,(alpha + 1),(depth_max - VERIFICATOIN_SEARCH_REDUCTION),
			         depth,description_move, 
					 PV_NO,old_flag_check,NULL_YES);

     if(value <= alpha) return alpha;
//------------------------------

	    }//if(value <= alpha)

	}//   if((flag_null_m == NULL_NO) && ((depth_max - depth) >= NULL_DEPTH ) && 

	#endif//#if NULL_MOVE


// насчитываем список избыточных ходов
// множество этих списков и создают дерево перебора
bitboard_generator_o.Generator_captures_black(p_bitboard,&list_surplus_moves);
bitboard_generator_o.Generator_move_black(p_bitboard,&list_surplus_moves);
Sorting_moves(&list_surplus_moves);

	//-----------------------------------------------
	#if (KILLER && SAVE_LINE)
	// если нет взятий или взятие проигрывающее
    // то киллер вставляем на первую позицию иначе на вторую
    if((list_surplus_moves.end_captures == CAPTURE_NO) || 
	   (list_surplus_moves.sorting_score[0] < 0)){

		 killer_o.Insert_killer(&list_surplus_moves,depth,0);

	}else{

		 killer_o.Insert_killer(&list_surplus_moves,depth,1);

	}//   if((list_surplus_moves.end_captures == CAPTURE_NO) || 

	#endif//#if KILLER

	#if (HASH && SAVE_LINE)
	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	hash_o.Look_hash_move(&list_surplus_moves,p_bitboard->hash_key,depth,flag_insert_hash_move);

	// для полного теста нужно отключать отсечку по оценке
    #if HASH_TEST
	// флаг 0 значит что позицию по хеш ключу не нашли
	// флаг 1 значит что позицию нашли но в текущем списке нет хода записанного в ней
	// флаг 2 значит что по ключу позицию нашли и нашли ход записанный в списке ходов

       if (flag_insert_hash_move == 1) {
           cout << "коллизия хеша черные" << "\n";
	       exit(1); 
       }//if (flag_insert_hash_move == 0) {
    #endif//#if HASH_TEST


	#endif//#if HASH
	//-----------------------------------------------

#if (LMR && SAVE_LINE)
   // ход из хеш-таблицы, лучшее взятие, два киллера, все взятия не редуцируются
     lmr_reduction = FULL_SEARCH + list_surplus_moves.end_captures ; // 
   //lmr_reduction = FULL_SEARCH ; // 
#endif//#if LMR

//int r;

// бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++){

//--------------------------------------------
// к моему удивлению выигрыш где то полуход да и то под сомнением
// а при чистой альфа бете вообще только тормозит по моему
	//if(!flag_pv_f){
 //      // мега редукция можно назвать это супер лмр(lmr) :)
 //     // идея в том что начиная с глубины 6 начинаем пропускать последние ходы
	//  // но четыре хода остается в любом случае
	// r = 5 + list_surplus_moves.end_list - depth;
	// if ( r < 0) r = 0;
 //    if ( i >= 4 + r ) continue;
	//}//if(!flag_pv_f){
//--------------------------------------------

  // реализуем ход из списка другими словами генерируем позицию
  moves_o.Onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);

  // если король по шахом то
  if (bitboard_generator_o.King_black_check(p_bitboard)!= CHECK_NO){

// мы отменяем этот ход как некорректный
     moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

  }else{
 // ставим флаг что есть легальные ходы и засчитываем позицию
    flag_is_legal_moves = 1;
    nodes = nodes + 1;

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	//-----------------------------------------------
	  // из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	 if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

	  hash_o.Modification_random_key(0,i,&list_surplus_moves,p_bitboard,undo);


//TEST-----------
#if HASH_TEST_KEY

  unsigned __int64 test_key_control = p_bitboard->hash_key;

 //тут ключ считается полностью по новой
 //тестировал хеш-таблицу
 //инициализируем ключ начальной позиции(мы пишем его в структуру доски)
  hash_o.Start_position_random_key(p_bitboard);

  if(test_key_control != p_bitboard->hash_key){
       cout << "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK " << "\n";
       cout << "collision black depth = " << depth << "\n";
       exit(1);
  }//if(key_control != p_bitboard->hash_key){

#endif//#if HASH_TEST_KEY 
//----------------------------

	  // записываем ключ для последующего анализа на повтор позиций
      hash_o.Save_hash_three(p_bitboard->hash_key,new_depth); 
	//-----------------------------------------------
    depth_max_new = depth_max;
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

	//-----------------------------------------------
	#if (EXTENSION_CHEK && SAVE_LINE)

	   // если король под шахом то увеличиваем максимальную глубину перебора 
	      Extension_for_chek_black(p_bitboard,depth_max,depth_max_new,flag_check);

	#endif//#if EXTENSION_CHEK
	//-----------------------------------------------

	//-----------------------------------------------
	#if (EXTENSION_CAPTURES && SAVE_LINE)
      if(flag_pv_f == PV_YES){//

//            if (description_move == 2){

             // если ход это взятие
             if (((list_surplus_moves.move[i]>>6) & 63) == 2){

                 // если взятая фигура лучше берущей
                 if ( ((list_surplus_moves.move[i]>>24) & 7) // ходим
                        <= (list_surplus_moves.move[i] & 63) ){ // берем

                           depth_max_new = depth_max + 1;
                          //Extension_for_captures(((list_surplus_moves.move[i]>>6) & 63)
		                  //                  ,description_move,depth_max,depth_max_new);

				 }//if ( ((list_surplus_moves.move[i]>>24) & 7)

			 }//if (((list_surplus_moves.move[i]>>6) & 63) == 2){

//		 }//if (description_move == 2){

	  }//if(flag_pv_f == PV_YES)
    #endif//#if EXTENSION_CAPTURES
	//-----------------------------------------------

// попытка сделать продление на единственном ответе     
  //   if(flag_pv_f == PV_YES){//

  //      if ((flag_check) // если нам шах
		//	&&(list_surplus_moves.end_list < 10)  // и ходов меньше 10
		//	&&(depth_max_new < 51) ){			
  //     //   cnt_single = 0; 
		//		depth_max_new = depth_max + 1;
		//}//if ((flag_check) // если нам шах

	 // }//if(flag_pv_f == PV_YES)

    #if !SAVE_LINE
   // рекурсивный вызов функции
      value = White(p_bitboard,alpha,beta,depth_max,new_depth,
		            ((list_surplus_moves.move[i]>>6) & 63),PV_YES,flag_check,
					flag_null_m);
    #endif//#if !SAVE_LINE


    #if (!LMR && SAVE_LINE)
      // рекурсивный вызов функции
      //depth_max_z
    #if ALPHA_BETA_ON
	  if (i == 0) {
    #else
      if (1) {
    #endif//#if ALPHA_BETA_ON

              value = White(p_bitboard,alpha,beta,depth_max_new,new_depth,
			               ((list_surplus_moves.move[i]>>6) & 63),flag_pv_f,
				        	 flag_check,flag_null_m);

       }else{

               value = White(p_bitboard,(beta - 1),beta,depth_max_new,
// TEST
//value = White(p_bitboard,alpha,beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

               if((value > alpha) && (value < beta)){

		           value = White(p_bitboard,alpha,beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 flag_pv_f,flag_check,flag_null_m);

	           }//if((value > alpha)&&(value < beta))
        }//if (i == 0)
    #endif//#if !NEGA_SCOUT

	//------------------------------------------------------
	#if  (LMR && SAVE_LINE)

	  if(flag_pv_f){
          // если мы в главном варианте то считаем по методике с нулевым окном

	      if (i == 0) {
              //
              value = White(p_bitboard,alpha,beta,depth_max_new,
				            new_depth,((list_surplus_moves.move[i]>>6) & 63),
							PV_YES,flag_check,flag_null_m);
          }else{

              value = White(p_bitboard,(beta - 1),beta,depth_max_new,
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

              if(value < beta){

		           value = White(p_bitboard,alpha,beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 PV_YES,flag_check,flag_null_m);

	          }//if(value < beta)

        }//if (i == 0)

      }else{
          // если мы не в главном варианте то считаем по методике LMR

        if((i > lmr_reduction)
			&& (flag_check == CHECK_NO) && (old_flag_check == CHECK_NO)
			&& ((depth_max_new - depth) >= LMR_DEPTH ) 
			){

               value = White(p_bitboard,(beta - 1),beta,(depth_max_new - LMR_REDUCTION_D),
				             new_depth,((list_surplus_moves.move[i]>>6) & 63),
							 PV_NO,flag_check,flag_null_m);

        }else{

               value = beta - 1;

        }// if((i > lmr_reduction)

             if(value < beta){

		           value = White(p_bitboard,(beta - 1),beta,depth_max_new,
					             new_depth,((list_surplus_moves.move[i]>>6)& 63),
								 PV_NO,flag_check,flag_null_m);

	          }//if(value < beta)

	  }//if(flag_pv_f){

	#endif//#if  LMR
	//------------------------------------------------------

// восстанавливаем позицию
   moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE

// восстанавливаем хеш-ключ доски
p_bitboard->hash_key = key_undo;

   //-----------------------------------------------

   if (value < value_min){

     //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      #if ALPHA_BETA_ON
        if (value < beta){

           //-----------------------------------------------
		   if(value <= alpha) {
	          #if KILLER
			 if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE){
				 killer_o.Save_killer(i,&list_surplus_moves,depth);
			 }//if(((list_surplus_moves.move[i]>>6)& 63) != MOVE_IS_CAPTURE){
	          #endif//#if KILLER

              #if HASH
	             // кэшируем лучший на данный момент ход
	             hash_o.Save_best_hash(i, p_bitboard->hash_key, &list_surplus_moves, 
					                   value, LOWER, depth, depth_max);	
	          #endif//#if HASH

			  return value; 
		   }//if(value <= alpha) {
         //-----------------------------------------------

// TEST------------
#if HASH_TEST_SCORE
// конечно что бы этот тест работал нельзя выше отсекаться по хеш оценке :)
// если мы должны отсекаться но тем не менее ход не вызвал отсечку то пишем об этом
// подробнее за черных
// суть в том что мы взяли оценку из хеш-таблицы и увидели что она меньше альфы т.е. вызовет
// отсечение. теперь мы просчитываем ее и если оценка подтверждается то до этого места
// счет не дойдет так как отсечется раньше
		 if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 2 )){
			 cout << "осечка отсечения по хеш таблице черные" << "\n";
             cout << "depth= " << depth << "\n";
             cout << "depth_max_rem= " << depth_max_rem << "\n";
             cout << "hash_depth= " << (__int32)hash_depth << "\n";
             cout << "hash_depth_max= " << (__int32)hash_depth_max << "\n";
             cout << "hash_score= " << hash_score  << "\n";
             cout << "value= " << value << "\n";
             cout << "alpha= " << alpha << "\n";
             cout << "beta= " << beta << "\n";
             cout << "  " << "\n";

             exit(1);
		 }//if (( hash_score != -1000000000 ) && ( test_flag_hash_moves == 3 )){

#endif//#if HASH_TEST_SCORE
//-------------------

           beta = value;
           flag_save_best = 1;
           i_hash_moves_1 = i;// ход который улучшает бету (1- улучшили бету) нужно для кеширования 

           // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
           if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);

        }//if (value < beta){

      #else // #if ALPHA_BETA_ON

        // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
        if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
        flag_save_best = 1;
        hash_moves_1 = i;// при тестировании полного перебора
      #endif//#if ALPHA_BETA_ON
    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

       value_min = value;
       i_hash_moves_3 = i;// ход дающий локальный минимум (3-просто локальный минимум)нужно для кеширования

   }//if (value < value_min){

	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

 }//  if (bitboard_generator_o.King_black_check(p_bitboard)!= CHECK_NO){

}//for (int i=0;i<list_surplus_moves.end_list;i++){

//-----------------------------------------------
// обработка случая когда не было легальных ходов
// если не одного хода сделано не было то производим статическую оценку позиции
// это случай пата или мата
if (flag_is_legal_moves == 0){
 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
     value = 0;
     if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
         value = (VALUE_MATE - DELTA_MATE * depth);//
     }//if(King_white_check(p_bitboard) != 0){

      // у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
      if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value,depth);
    #endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
  return value;
}//	if (no_moves == 0){
//-----------------------------------------------

 	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	#if SAVE_LINE
	     if(flag_save_best == 1){
          // теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
          if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	  }//if(save_best == 1){
	#endif//#if SAVE_LINE 
	//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

    #if (HASH && SAVE_LINE)
	  if( flag_save_best == 1 ) {
	    // кэшируем ход улучшающий бету т.е. флаг 1
	    hash_o.Save_best_hash(i_hash_moves_1, p_bitboard->hash_key, &list_surplus_moves, 
			                  beta, EXACT, depth, depth_max);
	  }else{
	    // кэшируем ход дающий локальный минимум т.е. флаг 3
	    hash_o.Save_best_hash(i_hash_moves_3, p_bitboard->hash_key, &list_surplus_moves, 
			                  value_min, UPPER, depth, depth_max);
	  }//if(flag_save_best == 1){
	#endif//#if HASH

return value_min;
}//int FULLSEARCH::Black
//	*******************************************************************

//	===================================================================
// производим расчеты на максимальной глубине. за белых
inline __int32 FULLSEARCH::Max_depth_white(struct T_Bitboard * const p_bitboard,// представление доски
									   const __int32 alpha,//
						               const __int32 beta,//
                                       const bool flag_pv_f,// основной ли это вариант (pv)
						               const unsigned __int8 depth// глубина
						               ){

__int32 score = 0;
__int32 score_f = 0;
double deep_time_current;

//-----------------------------------------------
// обработка команд экстренного завершения счета 
if (audit > 50000){
//if (0){
  //если поступила команда то это или quit или stop
  if (Bioskey()){
// if (0){
//     char string_in2[65536];
     cin.getline(string_in2,65536, '\n');
     if (strcmp(string_in2, "quit") == 0) {
        hash_o.Hash_size_free();
	    exit(1);
     }//if (strcmp(string_in2, "quit")== 0) {
     if ((strcmp(string_in2, "stop") == 0)){
     // делаем ход
        flag_stop_search = 1;
     }//if ((strcmp(string_in2, "stop")== 0)){
   }//if (Bioskey()){

// экстренный выход с глубины. просрочили время на поиск
  timer.Timer_stop();
  deep_time_current = timer.Timer_delta();
// выходим по времени
   if((deep_time_terminate > 0) && (deep_time_terminate < deep_time_current)) flag_stop_search = 1;
//------------------------

   audit = 0;

}//if (audit>50000){

audit = audit + 1;
//-----------------------------------------------

//------------------------------------------------------
   // форсировка
   //FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
   // подключаем форсированный вариант
   // мы уже погрузились на полу ход так что глубина остаются те же
   #if FORCED // включаем режим просчета только взятий
  	   score = quiescence_search_o.Forced_variant_white(p_bitboard,alpha,beta,depth,flag_pv_f,(depth + 5));
       return score ;
   #endif//#if FORCED

//----------------------------------------------
// возвращаемся к мегаскоростной и мегакривой схеме
   //#if FORCED // включаем режим просчета только взятий
   //    score = eval_o.Eval_State(p_bitboard);
  	//   score_f = quiescence_search_o.Forced_variant_white(p_bitboard,(alpha - score),(beta + score),depth,(depth + 5));
   //    score = score + score_f;
   //    pv_save_o.pv_best.score_move = score;
   //    return score ;
   //#endif//#if FORCED
//----------------------------------------------


// включаем оценку без форсировки(просчета только взятий)
   #if !FORCED 
      score = eval_o.Eval_State(p_bitboard) + p_bitboard->eval;
      //score = p_bitboard->eval;

////--------------------------------------------------------------------------
//// TEST
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "WHITE  Eval_forced(p_bitboard) != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) 
////--------------------------------------------------------------------------

//    у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
//    надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
      if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(score,depth);

//    TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
//    именно здесь можно проверить правильно ли рисуется вариант на предельной глубине
//    кроме  случая шаха и пата
//    Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
//    TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
      return score ;
 #endif//#if !FORCED 

}//inline int FULLSEARCH::Max_depth_white
//	*******************************************************************

//	===================================================================
// производим расчеты на максимальной глубине. за черных
inline __int32 FULLSEARCH::Max_depth_black(struct T_Bitboard * const p_bitboard,// представление доски
									   const __int32 alpha,//
						               const __int32 beta,//
                                       const bool flag_pv_f,// основной ли это вариант (pv)
						               const unsigned __int8 depth// глубина
						               ){

__int32 score = 0;
__int32 score_f = 0;
double deep_time_current;

//-----------------------------------------------
// обработка команд экстренного завершения счета 
if (audit > 50000){
//if (0){
//если поступила команда то это или quit или stop
  if (Bioskey()){
// if (0){
//    char string_in2[65536];
    cin.getline(string_in2,65536, '\n');
    if (strcmp(string_in2, "quit") == 0) {
       hash_o.Hash_size_free();
	   exit(1);
    }//if (strcmp(string_in2, "quit") == 0) 
    if ((strcmp(string_in2, "stop") == 0)){
    // делаем ход
       flag_stop_search = 1;
    }//if ((strcmp(string_in2, "stop")== 0))
  }//if (Bioskey())

// экстренный выход с глубины. просрочили время на поиск
  timer.Timer_stop();
  deep_time_current = timer.Timer_delta();
// выходим по времени
   if((deep_time_terminate  > 0) && (deep_time_terminate < deep_time_current)) flag_stop_search = 1;
//------------------------

  audit = 0;

}//if (audit>50000)

audit = audit + 1;
//-----------------------------------------------

//------------------------------------------------------
//  форсировка
//  FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
//  подключаем форсированный вариант
//  мы уже погрузились на полу ход так что глубина остаются те же
    #if FORCED // включаем режим форсировки(просчета только взятий)
       score = quiescence_search_o.Forced_variant_black(p_bitboard,alpha,beta,depth,flag_pv_f,(depth + 5));
       return score ;
    #endif//#if FORCED 

//----------------------------------------------
// возвращаемся к мегаскоростной и мегакривой  схеме 
   //#if FORCED // включаем режим просчета только взятий
   //    score = eval_o.Eval_State(p_bitboard);
  	//   score_f = quiescence_search_o.Forced_variant_black(p_bitboard,(alpha - score),(beta + score),depth,(depth + 5));
   //    score = score + score_f;
   //    pv_save_o.pv_best.score_move = score;
   //    return score ;
   //#endif//#if FORCED
//----------------------------------------------


// включаем оценку без форсировки(просчета только взятий)
    #if !FORCED
       score = eval_o.Eval_State(p_bitboard) + p_bitboard->eval;
       //score = p_bitboard->eval;

////--------------------------------------------------------------------------
//// TEST
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "BLACK  Eval_forced(p_bitboard) != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) 
////--------------------------------------------------------------------------

//     у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
//     надо понимать что в pv_current ни оценки ни глубины не задается мы их сразу пишем в pv_best
       if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(score,depth);

//     TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
//     именно здесь можно проверить правильно ли рисуется вариант на предельной глубине
//     кроме  случая шаха и пата
//     Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
//     TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
       return score ;
    #endif//#if !FORCED 

}//inline int FULLSEARCH::Max_depth_black
//	*******************************************************************

//	===================================================================
// реализуем продление на шахах -
inline void FULLSEARCH::Extension_for_chek_white(const struct T_Bitboard * const p_bitboard,// представление доски
	                                             const unsigned __int8 depth_max,// максимальная глубина
									             unsigned __int8 & depth_max_new,// новая максимальная глубина
									             bool & flag_check// флаг шаха
									             ){

// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){
      flag_check = CHECK_YES;
      depth_max_new = depth_max + 1;
}else{
      flag_check = CHECK_NO;
}//if(bitboard_generator_o.King_black_check(p_bitboard) != CHECK_NO){

}//inline void FULLSEARCH::Extension_for_chek_white
//	*******************************************************************

//	===================================================================
// реализуем продление на шахах -
inline void FULLSEARCH::Extension_for_chek_black(const struct T_Bitboard * const p_bitboard,// представление доски
	                                             const unsigned __int8 depth_max,// максимальная глубина
									             unsigned __int8 & depth_max_new,// новая максимальная глубина
									             bool & flag_check// флаг шаха
									             ){

// если есть шах то увеличиваем максимальную глубину перебора для этой ветки
if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){
      flag_check = CHECK_YES;
      depth_max_new = depth_max + 1;
}else{
      flag_check = CHECK_NO;
}//if(bitboard_generator_o.King_white_check(p_bitboard) != CHECK_NO){

}//inline void FULLSEARCH::Extension_for_chek_black
//	*******************************************************************

//	===================================================================
// реализуем продление на взятиях 
inline void FULLSEARCH::Extension_for_captures(const __int32 description_move,// тип хода
								               const __int32 description_move_up,// тип хода на предыдущей глубине
								               const unsigned __int8 depth_max,// максимальная глубина
								               unsigned __int8 & depth_max_new// новая максимальная глубина
								               ){

// если есть размен то увеличиваем максимальную глубину перебора для этой ветки
// возможно что следует сделать не просто размен а выигрываюший
// (когда взятая фигура дороже берущей)
     if((description_move == 2) && (description_move_up == 2)){
	         depth_max_new = depth_max + 1;
     }//if((description_move == 2) && (description_move_up == 2)){
}//inline void FULLSEARCH::Extension_for_captures
//	*******************************************************************

//	===================================================================
// полная сортировка
inline void FULLSEARCH::Sorting_moves(struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						              ){
 // 
__int32 move_s;// используем для запоминания хода
__int32 score_s;// используем для запоминания оценки хода
unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//количество взятий
unsigned __int8 end_list = p_list_surplus_moves->end_list;//количество ходов


/*
список у нас начинается с 0 и имеет вид
0 взятие
1 взятие
2 взятие
3 ход          p_list_surplus_moves->end_captures
т.е. количество взятий на 1 превышает реальный индекс последнего взятия
4 ход
5 ход
6 ход
7              p_list_surplus_moves->end_list
получается что у нас настоящее количество взятий и ходов т.е. если считать с 1

счетчик взятий и ходов в списке генератор взятий изначально ставит в ноль

*/

// 1 сортируем взятия----------------------------------
/*
если у нас есть взятия тогда мы их сортируем по оценке обсчитанной еще в генераторе взятий
идем по списку и сравниваем(меньше ли она) текущую строку со всеми нижестоящими
понятно что если у нас текущая строка k то следующая строка будет k + 1
если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и текущую строку
таким образом когда мы доходим до конца все взятия у нас отсортированы по убывающей.
*/
   for(unsigned __int8 k = 0; k < end_captures; k++){	
       for(unsigned __int8 i = k + 1; i < end_captures; i++){
           if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
//            сохраняем данные   
	          move_s = p_list_surplus_moves->move[k];
              score_s = p_list_surplus_moves->sorting_score[k];

//            копируем из i в k
	          p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
              p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

//            восстанавливаем запись
	          p_list_surplus_moves->move[i] = move_s;
              p_list_surplus_moves->sorting_score[i] = score_s;
	       }//if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
       }//for(int i = k + 1; i < captures; i++){
   }//for(int k = 0; k < captures; k++){	

}//inline void FULLSEARCH::Sorting_moves
//	*******************************************************************

//	===================================================================
// полная сортировка корневого списка
inline void FULLSEARCH::Sorting_white_moves_root(struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						              ){
 // 
__int32 move_s;// используем для запоминания хода
__int32 score_s;// используем для запоминания оценки хода
unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//количество взятий
unsigned __int8 end_list = p_list_surplus_moves->end_list;//количество ходов


/*
список у нас начинается с 0 и имеет вид
0 взятие
1 взятие
2 взятие
3 ход          p_list_surplus_moves->end_captures
т.е. количество взятий на 1 превышает реальный индекс последнего взятия
4 ход
5 ход
6 ход
7              p_list_surplus_moves->end_list
получается что у нас настоящее количество взятий и ходов т.е. если считать с 1

счетчик взятий и ходов в списке генератор взятий изначально ставит в ноль

*/

// 1 сортируем корневые ходы----------------------------------
/*
мы их сортируем по оценке обсчитанной на предыдущей итерации
идем по списку и сравниваем(меньше ли она) текущую строку со всеми нижестоящими
понятно что если у нас текущая строка k то следующая строка будет k + 1
если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и текущую строку
таким образом когда мы доходим до конца все ходы у нас отсортированы по убывающей.
*/
   for(unsigned __int8 k = 0; k < end_list; k++){	
       for(unsigned __int8 i = k + 1; i < end_list; i++){
           if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
//            сохраняем данные   
	          move_s = p_list_surplus_moves->move[k];
              score_s = p_list_surplus_moves->sorting_score[k];

//            копируем из i в k
	          p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
              p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

//            восстанавливаем запись
	          p_list_surplus_moves->move[i] = move_s;
              p_list_surplus_moves->sorting_score[i] = score_s;
	       }//if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
       }//for(int i = k + 1; i < captures; i++){
   }//for(int k = 0; k < captures; k++){	

}//inline void FULLSEARCH::Sorting_moves
//	*******************************************************************

//	===================================================================
// полная сортировка корневого списка
inline void FULLSEARCH::Sorting_black_moves_root(struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						              ){
 // 
__int32 move_s;// используем для запоминания хода
__int32 score_s;// используем для запоминания оценки хода
unsigned __int8 end_captures = p_list_surplus_moves->end_captures;//количество взятий
unsigned __int8 end_list = p_list_surplus_moves->end_list;//количество ходов


/*
список у нас начинается с 0 и имеет вид
0 взятие
1 взятие
2 взятие
3 ход          p_list_surplus_moves->end_captures
т.е. количество взятий на 1 превышает реальный индекс последнего взятия
4 ход
5 ход
6 ход
7              p_list_surplus_moves->end_list
получается что у нас настоящее количество взятий и ходов т.е. если считать с 1

счетчик взятий и ходов в списке генератор взятий изначально ставит в ноль

*/

// 1 сортируем корневые ходы----------------------------------
/*
мы их сортируем по оценке обсчитанной на предыдущей итерации
идем по списку и сравниваем(меньше ли она) текущую строку со всеми нижестоящими
понятно что если у нас текущая строка k то следующая строка будет k + 1
если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и текущую строку
таким образом когда мы доходим до конца все ходы у нас отсортированы по убывающей.
*/
   for(unsigned __int8 k = 0; k < end_list; k++){	
       for(unsigned __int8 i = k + 1; i < end_list; i++){
           if(p_list_surplus_moves->sorting_score[k] > p_list_surplus_moves->sorting_score[i]){ 
//            сохраняем данные   
	          move_s = p_list_surplus_moves->move[k];
              score_s = p_list_surplus_moves->sorting_score[k];

//            копируем из i в k
	          p_list_surplus_moves->move[k] = p_list_surplus_moves->move[i];
              p_list_surplus_moves->sorting_score[k] = p_list_surplus_moves->sorting_score[i];

//            восстанавливаем запись
	          p_list_surplus_moves->move[i] = move_s;
              p_list_surplus_moves->sorting_score[i] = score_s;
	       }//if(p_list_surplus_moves->sorting_score[k] < p_list_surplus_moves->sorting_score[i]){ 
       }//for(int i = k + 1; i < captures; i++){
   }//for(int k = 0; k < captures; k++){	

}//inline void FULLSEARCH::Sorting_moves
//	*******************************************************************

//	===================================================================
// использую для теста. сейчас уже не помню для чего он мне был нужен
void FULLSEARCH::Print_name_from_coord(const struct T_Bitboard * const p_bitboard// представление доски
									   ){

for(unsigned __int8 i = 0; i < 8; i++){
   for(unsigned __int8 j = 0; j < 8; j++){
       cout << p_bitboard->white_name_from_coord[(7 - i) * 8 + j];
   }//for(int j = 0; j < 8; j++){
   cout << "\n";
}//for(int i = 0; i < 8; i++){
cout << "\n";
for(unsigned __int8 i = 0; i < 8; i++){
   for(unsigned __int8 j = 0; j < 8; j++){
       cout << p_bitboard->black_name_from_coord[(7 - i) * 8 + j];
   }//for(int j = 0; j < 8; j++){
   cout << "\n";
}//for(int i = 0; i < 8; i++){

  //for(int i= 0; i < 64; i++){
  //   cout << bitboard->white_name_from_coord[i];
  //   if(i%8 == 7) cout << "\n";
  //}//for(int i= 0; i < 64; i++){
}//void FULLSEARCH::Print_name_from_coord
//	*******************************************************************

//	===================================================================
// использую для теста. запоминаем доску
inline void FULLSEARCH::Save_test_bitboard(const struct T_Bitboard * const p_bitboard,// представление доски
                                           struct T_Bitboard * p_test_bitboard// представление доски
						                  ){

	p_test_bitboard->all_piece = p_bitboard->all_piece;// все фигуры
	p_test_bitboard->all_white_piece = p_bitboard->all_white_piece;// все белые фигуры
	p_test_bitboard->all_black_piece = p_bitboard->all_black_piece;// все черные фигуры
	p_test_bitboard->colour_move = p_bitboard->colour_move;
	p_test_bitboard->taking_passage_yes = p_bitboard->taking_passage_yes;
	p_test_bitboard->taking_passage_x = p_bitboard->taking_passage_x;
	p_test_bitboard->taking_passage_y = p_bitboard->taking_passage_y;
	p_test_bitboard->castling_Q = p_bitboard->castling_Q;
	p_test_bitboard->castling_K = p_bitboard->castling_K;
	p_test_bitboard->castling_q = p_bitboard->castling_q;
	p_test_bitboard->castling_k = p_bitboard->castling_k;
	p_test_bitboard->hash_key =  p_bitboard->hash_key;
	p_test_bitboard->eval = p_bitboard->eval;

	for(unsigned __int8 n =0; n < 7; n++ ){
       p_test_bitboard->white_piece[n] = p_bitboard->white_piece[n];
       p_test_bitboard->black_piece[n] = p_bitboard->black_piece[n];
	}//for(int n =0; n < 7; n++ ){

    for(unsigned __int8 k =0; k < 64; k++ ){
        p_test_bitboard->white_name_from_coord[k] = p_bitboard->white_name_from_coord[k];
        p_test_bitboard->black_name_from_coord[k] = p_bitboard->black_name_from_coord[k];
	}//for(int k =0; k < 64; k++ ){

}//inline void FULLSEARCH::Save_test_bitboard
//	*******************************************************************

//	===================================================================
// использую для теста. сравниваем доски
inline void FULLSEARCH::Compare_test_bitboard(const struct T_Bitboard * const p_bitboard,// представление доски
                                              struct T_Bitboard * p_test_bitboard// представление доски
						                     ){
// проверяем востановилась ли доска после прохождения глубины
if(p_test_bitboard->all_piece != p_bitboard->all_piece) cout << "white test_all_piece != bitboard->all_piece" << "\n";// все фигуры
if(p_test_bitboard->all_white_piece != p_bitboard->all_white_piece) cout << "white test_all_white_piece != bitboard->all_white_piece" << "\n";// все белые фигуры
if(p_test_bitboard->all_black_piece != p_bitboard->all_black_piece) cout << "white test_all_black_piece != bitboard->all_black_piece" << "\n";// все черные фигуры
if(p_test_bitboard->colour_move != p_bitboard->colour_move) cout << "white test_colour_move != bitboard->colour_move" << "\n";
if(p_test_bitboard->taking_passage_yes != p_bitboard->taking_passage_yes) cout << "white test_taking_passage_yes != bitboard->taking_passage_yes" << "\n";
if(p_test_bitboard->taking_passage_x != p_bitboard->taking_passage_x) cout << "white test_taking_passage_x != bitboard->taking_passage_x" << "\n";
if(p_test_bitboard->taking_passage_y != p_bitboard->taking_passage_y) cout << "white test_taking_passage_y != bitboard->taking_passage_y" << "\n";
if(p_test_bitboard->castling_Q != p_bitboard->castling_Q) cout << "white test_castling_Q != bitboard->castling_Q" << "\n";
if(p_test_bitboard->castling_K != p_bitboard->castling_K) cout << "white test_castling_K != bitboard->castling_K" << "\n";
if(p_test_bitboard->castling_q != p_bitboard->castling_q) cout << "white test_castling_q != bitboard->castling_q" << "\n";
if(p_test_bitboard->castling_k != p_bitboard->castling_k) cout << "white test_castling_k != bitboard->castling_k" << "\n";
if(p_test_bitboard->hash_key !=  p_bitboard->hash_key) cout << "white test_hash_key !=  bitboard->hash_key" << "\n";
if(p_test_bitboard->eval != p_bitboard->eval) cout << "white test_eval != bitboard->eval" << "\n";

	for(unsigned __int8 n2 = 0; n2 < 7; n2++ ){
       if(p_test_bitboard->white_piece[n2] != p_bitboard->white_piece[n2])  cout << "white test_white_piece[n] = bitboard->white_piece[n]" << n2 << "\n";
       if(p_test_bitboard->black_piece[n2] != p_bitboard->black_piece[n2])  cout << "white test_black_piece[n] = bitboard->black_piece[n]" << n2 << "\n";
	}//for(int n =0; n < 7; n++ ){

    for(unsigned __int8 k2 = 0; k2 < 64; k2++ ){
       if(p_test_bitboard->white_name_from_coord[k2] != p_bitboard->white_name_from_coord[k2])  cout << "white test_white_name_from_coord[k] = bitboard->white_name_from_coord[k]" << k2 << "\n";;
       if(p_test_bitboard->black_name_from_coord[k2] != p_bitboard->black_name_from_coord[k2])  cout << "white test_black_name_from_coord[k] = bitboard->black_name_from_coord[k]" << k2 << "\n";;
	}//for(int k2 =0; k2 < 64; k2++ ){

}//inline void FULLSEARCH::Compare_test_bitboard
//	*******************************************************************

//	===================================================================
//
void FULLSEARCH::ini_global_loop(unsigned __int8 depth_max//
								 ){
        audit = 0;// задержка опроса команды экстренного выхода
        lmr_reduction = 0;//
        flag_stop_search = 0;//
}//void FULLSEARCH::ini_global_loop
//	*******************************************************************

//	===================================================================
// возвращаем флаг остановки поиска. так если в глубине поиска получили команду
// на остановку то передаем ее наружу
bool FULLSEARCH::get_stop_search(){
	return flag_stop_search;
}//bool FULLSEARCH::get_stop_search
//	*******************************************************************

//	===================================================================
// возвращаем лучший вариант
struct T_PV * FULLSEARCH::get_pv_best(){
	return pv_save_o.get_pv_best();
}//struct T_PV FULLSEARCH::get_pv_best
//	*******************************************************************

//	===================================================================
// возвращаем количество рассмотренных узлов
__int64  FULLSEARCH::get_nodes(){
	return nodes;// количество узлов;
}//__int64  FULLSEARCH::get_nodes
//	*******************************************************************

//	===================================================================
// обнуляем количество рассмотренных узлов
 void FULLSEARCH::set_nodes_in_0(){
	nodes = 0;// количество узлов;
}//__int64  FULLSEARCH::get_nodes
//	*******************************************************************

//	===================================================================
// откуда делается ход
unsigned __int8 FULLSEARCH::get_from(){
	return from;
}//int FULLSEARCH::get_from
//	*******************************************************************

//	===================================================================
// куда делается ход
unsigned __int8 FULLSEARCH::get_to(){
	return to;
}//int FULLSEARCH::get_to
//	*******************************************************************
