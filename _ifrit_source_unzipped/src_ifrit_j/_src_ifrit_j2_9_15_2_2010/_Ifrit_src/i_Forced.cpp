#include <iostream> // TEST
#include <fstream>  // TEST
using namespace std;

#include "i_Forced.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "k_Util.h"
#include "m_Line.h"
#include "d_Bitboard_gen.h"

//-----------------------------------------------------------------------------------
/*+
 * форсированный перебор позиций 
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.12.2008 19.11.2009
 *
 *  ТЕСТИРОВАНИЕ
 *
*/
//---------------------------------------------------------------------------
#define DELTA_PRUNING 1 // включаем-выключаем(1-0) режим
// нужно основательно тестировать

// задаем имя фигур(обязательно должно быть согласованно с модулем b_FEN.cpp и d_Bitboard_gen.cpp)
#define N_KING    6 // король
#define N_QUEEN   5 // ферзь
#define N_ROOK    4 // ладья
#define N_BISHOP  3 // слон
#define N_KNIGHT  2 // конь
#define N_PAWN    1 // пешка

// 90000 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка
#define ROOK_MARGIN    700 // отсчка взятий ладьей
#define BISHOP_KNIGHT_MARGIN  500 // отсчка взятий  слонов и коней
#define PAWN_MARGIN 160 // отсчка взятий  пешок

// global
extern class PVSAVE pv_save_o;// записываем вариант
extern class EVAL  eval_o;// оценка позиции
extern class BITBOARDGENERATOR bitboard_generator_o;// генератор ходов
extern class MOVES moves_o;// реализатор ходов

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
 ofstream Test_Loop_f;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*
 Опишу алгоритм расчета взятий для белых. Ясно что для черных аналогично.

 Главное отличие данного алгоритма от обычной альфа-бета оптимизации это использование
 статической оценки для завышения альфы и отсечения по бете. Надо еще заметить что 
 генерируем мы только взятия.

 Что бы понять данный алгоритм нужно вспомнить что такое альфа-бета оптимизация
 и какие особенности добавляет работа со статической оценкой.

 Белые ищут максимум среди минимумов которые нашли черные. Самая большая оценка которую
 белые уже нашли в предыдущем поиске и есть альфа. Таким образом белые стремятся увеличить 
 альфу. Черные ищут минимум. И самая маленькая оценка которую они уже нашли и есть бета.
 Белых интересуют оценки которые больше альфы а все остальные они отбрасывают. Допустим 
 белые сходили и теперь ход черных и найденная оценка меньше альфы. Ясно что поскольку 
 черные ищут минимум в дальнейшем оценка будет только уменьшаться. Но искать дальше 
 бесполезно так как уже ясно что белые эту оценку отбросят.
   
 В форсированном поиске мы считаем только взятия так что программа не видит что брать
 не обязательно а можно просто уклониться. Для того что бы программа не делала 
 проигрывающих взятий мы увеличиваем альфу статической оценкой. В итоге если программа
 найдет лучший вариант то она выберет его а если нет то откажется от дальнейших взятий.

 Когда мы отсекаемся статической оценкой по бете мы исходим из предположения что если 
 позиция уже в нашу пользу а тут еще есть наше взятие то мы либо улучшим оценку либо 
 удержим ее. Так как мы смотрим только взятия и не видим шахов такое предположение не
 уменьшает точность форсированного варианта.

1) Находим статическую оценку позиции.

  1.1 Если статическая оценка больше альфы то присваиваем ее альфе. Проще говоря 
   завышаем альфу статической оценкой.

  1.2 Если альфу завысили то пытаемся сразу отсечь по бете. 

  1.3 Фрагмент кода по пункту 1:

	if(value_stat > alpha){ 
	    if(value_stat >= beta) return value_stat;
		alpha = value_stat;
        save_best = 1;
	}

---------------------------------------
2) Насчитываем и сортируем по mvv/lva список возможных взятий. Сортировка имеет
   принципиальное значение для ускорения поиска.

  2.1  Дальше обычная альфа-бета. 

  2.2 Фрагмент кода по пункту 2:
  value_max_din = value_stat;
  if(value_din > value_max_din){
         value_max_din = value_din;
		 if(value_din > alpha) {
             if(value_din >= beta) return value_din;
			 alpha = value_din; 
             //здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
             pv_save_o.Update_PV_best_point(&pv_best_point);
             save_best = 1;
		 }
     }

  Принципиальный момент заключается в том что 
  мы пишем только те цепочки взятий или статическую оценку которые улучшают альфу
if(save_best == 1){
    if(value_max_din == value_stat){
         // статическую оценку улучшить не удалось

         // здесь мы обновили лучший выходной вариант особенностью является то что мы 
         // в лучший вариант сразу записали оценку
         pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
	}else{
         // теперь мы местный лучший вариант скопируем в структуру PV_best что бы передать на более высокий уровень
         pv_save_o.Update_PV_best(&pv_best_point);
	}//if(value_max_din == value_stat){
}//if(save_best == 1){
---------------------------------------

*/

//	===================================================================
// просчитываем только взятия для белых
 __int32 QUIESCENCESEARCH::Forced_variant_white(struct T_Bitboard * const p_bitboard,// представление доски
						                    __int32 alpha,//
						                    __int32 beta,//
						                    unsigned __int8 depth,// глубина
		                                    const bool flag_pv_f,// основной ли это вариант (pv)
						                    const unsigned __int8 & depth_max// максимальная глубина
						                    ){

bool save_best = 0;// пишем ли лучший вариант
__int32 value_din; // динамическая оценка
__int32 value_stat; // статическая оценка
__int32 value_max_din; // максимум в узле (было = -9999999)


struct T_undo undo[1];// информация для возврата хода
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
struct T_PV pv_best_point;// лучший для данного узла вариант

#if DELTA_PRUNING

unsigned __int64 all_black_piece;

#endif //#if DELTA_PRUNING

    // находим статическую оценку позиции
    value_stat = p_bitboard->eval + eval_o.Eval_State(p_bitboard);

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    value_stat = p_bitboard->eval;

 //------------------------------------------------
// TEST
// проверяем на правильность пошаговую оценку материала
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "WHITE  value_stat != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) {
//------------------------------------------------

	// завышаем альфу что бы не делать проигрывающих взятий.
	if(value_stat > alpha){ 
	    // пытаемся сразу отсечь по бете 
        // почему именно по бете?
        // дело в том что в форсированном поиске на предыдущей глубине поиск ведут черные и они ищут шанса улучшить
        // именно бету
        // мы исходим из того что если у нас позиция уже не нравиться черным
        // то сделав ход мы сможем ее усилить
	    if(value_stat >= beta) return value_stat;
		alpha = value_stat;
        save_best = 1;
	}//if(value_stat > alpha){ 
    //
    value_max_din = value_stat;


#if DELTA_PRUNING

    // запоминаем фигуры до резки
    all_black_piece = p_bitboard->all_black_piece;

	if(value_stat < alpha - PAWN_MARGIN ){
       // взятие пешек исключаем
       p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_PAWN];

       if(value_stat < alpha - BISHOP_KNIGHT_MARGIN ){
          // взятие слонов и коней исключаем
          p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_KNIGHT];
          p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_BISHOP];

          if(value_stat < alpha - ROOK_MARGIN ){
              // взятие ладьи исключаем
              p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ p_bitboard->black_piece[N_ROOK];

	      }//if(value_stat < alpha - 700 ){

	   }//if(value_stat < alpha - 500 ){

	}//if(value_stat < alpha - 300 ){


#endif //#if DELTA_PRUNING


//насчитываем список возможных взятий и сортируем его
bitboard_generator_o.Generator_captures_white(p_bitboard,&list_surplus_moves);
//if (depth == 2) List_Print_F(depth, &list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);
//if (depth == 2) List_Print_F(depth, &list_surplus_moves);

#if DELTA_PRUNING
     // возвращаем на доску фигуры
	 p_bitboard->all_black_piece = all_black_piece;
#endif //#if DELTA_PRUNING

//бежим по списку возможных ходов
// режим с альфа-бета отсечением
for(unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++){
              //TEST
              //cout << "white depth= " << depth<<"\n";
              //cout << "i = " << i <<"\n";
              //cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

// реализуем ход
   moves_o.Onward_moves_white(i,undo,&list_surplus_moves,p_bitboard);

// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
   if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);


// если короля не съели
   if(( list_surplus_moves.move[i] & 63) != 6){

//   рекурсивный вызов функции
     value_din = Forced_variant_black(p_bitboard,alpha,beta,(depth+1),flag_pv_f,depth_max);

//   восстанавливаем позицию
     moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);

     if(value_din > value_max_din){
         value_max_din = value_din;
         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
		 if(value_din > alpha) {
             if(value_din >= beta) return value_din;
			 alpha = value_din; 
             //   здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
             if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
             save_best = 1;
		 }//if(value_din > alpha) {
         //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
     }//if(value_din > value_max_din){

  //-------------------------------------------------------------------
// обрабатываем случай взятого короля
  }else{//if (list_surplus_moves->name_taking_figure[i] !=6){

//  короля сожрали !
//------------------------------------------------
// что тут ставить + или - ??
// ход сделали белые так что битый король конечно черный значит должен быть +
// прямые опыты подтвердили это
    value_stat = (1000000 - depth * 1000);
    moves_o.Backs_moves_white(i,undo,&list_surplus_moves,p_bitboard);
    // здесь мы обновили лучший выходной вариант особенностью является то что мы 
    // в лучший вариант сразу записали оценку
    if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
    return value_stat;

  }//if (list_surplus_moves->name_taking_figure[i] !=6){
  //--------------------------------------------------------------------

}//for(int i = 0; i < list_surplus_moves.end_captures; i++){

/*
принципиальный момент заключается в том что мы пишем только те цепочки взятий
или статическую оценку которые улучшают альфу
*/
if(save_best == 1){
    if(value_max_din == value_stat){
         // статическую оценку улучшить не удалось

         // здесь мы обновили лучший выходной вариант особенностью является то что мы 
         // в лучший вариант сразу записали оценку
         if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
	}else{
         // теперь мы местный лучший вариант скопируем в структуру PV_best что бы передать на более высокий уровень
         if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	}//if(value_max_din == value_stat){
}//if(save_best == 1){


return value_max_din;
}//int QUIESCENCESEARCH::Forced_variant_white
//	*******************************************************************

//	===================================================================
// просчитываем только взятия для черных
__int32 QUIESCENCESEARCH::Forced_variant_black(struct T_Bitboard * const p_bitboard,// представление доски
						                   __int32 alpha,//
						                   __int32 beta,//
						                   unsigned __int8 depth,// глубина
		                                   const bool flag_pv_f,// основной ли это вариант (pv)
						                   const unsigned __int8 & depth_max// максимальная глубина
						                   ){

bool save_best = 0;// пишем ли лучший вариант
__int32 value_din; // динамическая оценка
__int32 value_stat; // статическая оценка
__int32 value_min_din; //минимум в узле (было = 9999999)

struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
struct T_PV pv_best_point;// лучший для данного узла вариант

#if DELTA_PRUNING

unsigned __int64 all_white_piece;

#endif //#if DELTA_PRUNING

    value_stat = p_bitboard->eval + eval_o.Eval_State(p_bitboard);
 
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//    value_stat = p_bitboard->eval;

//------------------------------------------------
// TEST
// проверяем на правильность пошаговую оценку материала
//if (eval_o.Eval_forced(p_bitboard) != p_bitboard->eval) {
//	cout << "BLACK value_stat != p_bitboard->eval" <<"\n";
//    cout << "Eval_forced(p_bitboard)= "<< eval_o.Eval_forced(p_bitboard)<<"\n";
//    cout << "p_bitboard->eval= "<< p_bitboard->eval<<"\n";
//}//if (Eval_forced(p_bitboard) != p_bitboard->eval) {
//------------------------------------------------

	if(value_stat < beta) {
	    if(value_stat <= alpha) return value_stat;
		beta = value_stat;
        save_best = 1;
	}//if(value_stat < beta)
	//
    value_min_din = value_stat;

#if DELTA_PRUNING

    // запоминаем фигуры до резки
    all_white_piece = p_bitboard->all_white_piece;

	if(value_stat > beta + PAWN_MARGIN){
       // взятие пешек исключаем
       p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_PAWN];

       if(value_stat > beta + BISHOP_KNIGHT_MARGIN){
          // взятие слонов и коней исключаем
          p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_KNIGHT];
          p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_BISHOP];      

          if(value_stat > beta + ROOK_MARGIN){
              // взятие ладьи исключаем
              p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ p_bitboard->white_piece[N_ROOK];   

	      }//if(value_stat > beta + 700){

	   }//if(value_stat > beta + 500){

	}//if(value_stat > beta + 300){


#endif //#if DELTA_PRUNING


//насчитываем список возможных взятий
bitboard_generator_o.Generator_captures_black(p_bitboard,&list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);

#if DELTA_PRUNING
   // возвращаем на доску фигуры
   p_bitboard->all_white_piece = all_white_piece;
#endif //#if DELTA_PRUNING


//если до сих пор отсечек не было значит статическая оценка не катит

//бежим по списку возможных ходов
// режим с альфа бета отсечением
for(unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++){
                       //TEST
                       //cout << "black depth= " << depth<<"\n";
                       //cout << "i = " << i <<"\n";
                       //cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

//  реализуем ход
    moves_o.Onward_moves_black(i,undo,&list_surplus_moves,p_bitboard);

//  из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
    if(flag_pv_f) pv_save_o.Update_PV_current(i,depth,&list_surplus_moves);

// если короля не съели
    if(( list_surplus_moves.move[i] & 63) != 6){

//      рекурсивный вызов функции
        value_din = Forced_variant_white(p_bitboard,alpha,beta,(depth+1),flag_pv_f,depth_max);
//      восстанавливаем позицию
        moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);

        if(value_din < value_min_din){
             value_min_din = value_din;
             //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
			 if(value_din < beta) {
                 if(value_din <= alpha) return value_din;
				 beta = value_din; 
                 // здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
                 if(flag_pv_f) pv_save_o.Update_PV_best_point(&pv_best_point);
                 save_best = 1;
			 }//if(value_din < beta) {
             //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        }//if(value_din < value_min_din){

// обрабатываем случай взятого короля
   }else{//if (list_surplus_moves->name_taking_figure[i] !=6){

//------------------------------------------------
//      короля сожрали !

        value_stat = -(1000000 - depth * 1000);
        moves_o.Backs_moves_black(i,undo,&list_surplus_moves,p_bitboard);
        // здесь мы обновили лучший выходной вариант особенностью является то что мы 
        // в лучший вариант сразу записали оценку
        if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
        return value_stat;

   }//if (list_surplus_moves->name_taking_figure[i] !=6){

}//for(int i = 0; i < list_surplus_moves.end_list; i++){

if(save_best == 1){
    if(value_min_din == value_stat){
         // статическую оценку улучшить не удалось

         // здесь мы обновили лучший выходной вариант особенностью является то что мы 
         // в лучший вариант сразу записали оценку
         if(flag_pv_f) pv_save_o.Update_PV_best_max_depth(value_stat,depth); 
	}else{
         // теперь мы местный лучший вариант скопируем в структуру PV_best что бы передать на более высокий уровень
         if(flag_pv_f) pv_save_o.Update_PV_best(&pv_best_point);
	}//if(value_min_din == value_stat)
}//if(save_best == 1){

return value_min_din;
}//int QUIESCENCESEARCH::Forced_variant_black
//	*******************************************************************

//	===================================================================
// сортируем взятия возвращаем количество взятий
inline unsigned __int8 QUIESCENCESEARCH::Sorting_captures_moves(struct T_list_surplus_moves * const list_surplus_moves// список ходов
								                    ){
 // 
__int32 move_s;// используем для запоминания хода
__int32 score_s;// используем для запоминания оценки хода
unsigned __int8 end_captures = list_surplus_moves->end_captures;//количество взятий
unsigned __int8 n;

/*
если у нас есть взятия тогда мы их сортируем по оценке обсчитанной еще в генераторе взятий
идем по списку и сравниваем(меньше ли она) текущую строку со всеми нижестоящими
понятно что если у нас текущая строка k то следующая строка будет k + 1
если такая(т.е. больше текущей) строка нашлась то мы меняем местами ее и текущую строку
таким образом когда мы доходим до конца все взятия у нас отсортированы по убывающей.
*/
for(n = 0; n < end_captures; n++){	
    for(unsigned __int8 i = n + 1; i < end_captures; i++){
 
        if(list_surplus_moves->sorting_score[n] < list_surplus_moves->sorting_score[i]){ 
//            сохраняем данные из n     
	          move_s = list_surplus_moves->move[n];
              score_s = list_surplus_moves->sorting_score[n];

//            копируем из i в n
	          list_surplus_moves->move[n] = list_surplus_moves->move[i];
              list_surplus_moves->sorting_score[n] = list_surplus_moves->sorting_score[i];

//            восстанавливаем запись
	          list_surplus_moves->move[i] = move_s;
              list_surplus_moves->sorting_score[i] = score_s;

	    }// if(list_surplus_moves->sorting_score[n] < list_surplus_moves->sorting_score[i]){ 
    }//for(int i = n + 1; i < captures; i++)
}//for(n = 0 ; n < captures ; n++){		
return n;
}//inline int QUIESCENCESEARCH::Sorting_captures_moves
//	*******************************************************************


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
#if TEST_F
void QUIESCENCESEARCH::List_Print_F(int depth,// текущая глубина
				  const struct T_list_surplus_moves * list_surplus_moves//список ходов
				  ){

Test_Loop_f.open ("_TEST_LOOP_FORCED.txt",ios::app);	
//Test_Loop_f.open ("_TEST_LOOP_FORCED.txt");	
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop_f << "\n";
Test_Loop_f << "------------------------------------------------- "<<"\n";
Test_Loop_f <<"depth= "<< depth << "\n";
Test_Loop_f << "\n";
Test_Loop_f << "Список возможных ходов фигуры "<<"\n";

for (int i = 0 ; i < list_surplus_moves->end_list ; i++){	
     Test_Loop_f << "Номер хода по списку i= " << i << "\n";  
     Test_Loop_f << "Ходящая фигура " << int_to_char_board(((list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
     Test_Loop_f << "Оценка хода " << list_surplus_moves->sorting_score[i] << "\n";

     // вывод хода вида e2-e4
     //------------------------------
	 Test_Loop_f << " " << (x_to_notation(((list_surplus_moves->move[i]>>12)& 63)%8));
     Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
 //  Test_Loop_f << "\n";
     Test_Loop_f << " " << (x_to_notation(((list_surplus_moves->move[i]>>18)& 63)%8));
     Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
     //------------------------------

     Test_Loop_f << "вид хода " << ((list_surplus_moves->move[i]>>6)& 63)  << "\n";
     Test_Loop_f << "Взятая фигура " << int_to_char_board(( list_surplus_moves->move[i] & 63),1)<< "\n";
	 Test_Loop_f << "начальная положение " << ((list_surplus_moves->move[i]>>12)& 63)  << "\n";
	 Test_Loop_f << "конечное положение " << ((list_surplus_moves->move[i]>>18)& 63)  << "\n";
      
	 Test_Loop_f << "\n";
}//for (i=1,i<list_surplus_moves->amount_moves+1){

Test_Loop_f << "list_surplus_moves->end_captures= " <<list_surplus_moves->end_captures << "\n";
Test_Loop_f << "list_surplus_moves->end_list= " <<list_surplus_moves->end_list << "\n";
Test_Loop_f << "------------------------------------------------- " << "\n";
//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
Test_Loop_f.close();
}//void QUIESCENCESEARCH::List_Print_F
#endif
//	*******************************************************************
