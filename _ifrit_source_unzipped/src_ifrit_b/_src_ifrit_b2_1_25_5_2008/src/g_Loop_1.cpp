#include <iostream.h> // TEST
#include <fstream.h>  // TEST
#include <cstring>    // используем дл€ работы со строкой (сравнение строк)
#include <stdlib.h>   // обеспечиваем завершение потока функцией exit(1)


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
 * ј¬“ќ– ChessTerminator75
 * ѕќ—Ћ≈ƒЌ≈≈ »«ћ≈Ќ≈Ќ»≈ 22.4.2008
 *
 *  “≈—“»–ќ¬јЌ»≈
 * тестовую информащию выводим в файл _TEST_LOOP.txt
 * печатаем
 * тестируем модули 
 * целью теста €вл€етс€
 *
 * ¬ќ«ћќ∆Ќџ≈ »Ќƒ” “ќ–џ ќЎ»Ѕ »
 * тестировать и описывать тут очень много. надо будет все тщательно продумать
 * по сути это сердце программы и именно тут сход€тс€ все ниточки
 * нужно контролировать:
 * 1 генератор позиций - правильно ли генерируютс€ всевозможные позиции не пропущенно ли что
 * 2 лини€ варианта - правильно ли она печатаетс€ нет ли тут искажений
 * правильно ли печатаетс€ оценка и глубина варианта а также количество позиций
 * 3 альфа-бета отсечение - правильно ли оно работает нет ли где недостаточных
 *   или избыточных отсечений 
*/
//---------------------------------------------------------------------------


//	===================================================================
/*
решил вернутс€ к схеме альфа бета с амортизацией отказов
int AlphaBeta (pos, depth, alpha, beta) {
	if (depth == 0) return Evaluate(pos);
	best = -INFINITY;
	succ = Successors(pos);
	while (!Empty(succ)) {
		pos = RemoveOne(succ);
		value = -AlphaBeta(pos, depth - 1, -beta, -alpha);
		if (value > best) best = value;
	    if (value > alpha) alpha = value;
        if (alpha >= beta) return alpha;
	}
	return best;
}
----------------------------------------------------------------------------
так работает генератор позиций:
1 в начале генерируютс€ список избыточных ходов. избыточных потому что нет проверки на шах
  и в них есть такие ходы как рокировка под шахом или ход корол€ под шах
2 дальше мы сортируем этот список вывод€ вз€ти€ на верх и ход из хеша если он есть
3 дальше мы реализуем ход и провер€ем позицию на шах если пол€ рокировки под боем или 
 под боем наш король то ход откатываем иначе идем дальше
4 итерационное углубление
5 когда вернулись с глубины откатываем ход
6 если ходов не было то это случай мата или пата

*/
// глобальные переменные
int audit=0;// задержка опроса команды экстренного выхода

// поиск за белых. белые стрем€тс€ найти максимум
// перебор на нулевом уровне
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
int White_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // текуща€ оценка позиции
int value_max=-9999999;   // лучша€ в узле  оценка позиции  
int no_moves=0;           // флаг существовани€ легальных ходов (если 1 то легальных ходов не было)
int nodes_0=1;            // количество ходов на нулевом уровне
int key=0;                // хеш ключ
int insert_hash=0;        // поместили ли ход из кеша на первую позицию
double  time_nodes=0;     // врем€ изучени€ хода
struct T_PV pv_best_point;   // лучший дл€ данного узла вариант
int check=0;
int depth_max_z=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	//-----------------------------------------------
	#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------
//cout << "Rook_amoun_white(bitboard)="<< Rook_amoun_white(bitboard)<<"\n";
//cout << "Rook_amoun_black(bitboard)="<< Rook_amoun_black(bitboard)<<"\n";
//cout << "Bishop_amoun_white(bitboard)="<< Bishop_amoun_white(bitboard)<<"\n";
//cout << "Bishop_amoun_black(bitboard)="<< Bishop_amoun_black(bitboard)<<"\n";

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
 //
//«аполн€ем список избыточных ходов. »менно эти списки и составл€ют дерево перебора.
Generator_captures_white(bitboard,&list_surplus_moves);// генерируем вз€ти€ и превращени€
Generator_move_white(bitboard,&list_surplus_moves);// генерируем остальные ходы
//Bitboard_print(bitboard);// тестова€ печать битбоард доски
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// тестова€ печать списка и доски на нулевом уровне
	#if TEST_L
		List_816_Print(depth,list_surplus_moves);// устарела надо переписать
	//Bitboard_print(bitboard);
	#endif 
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if HASH
	// работаем с хешем
	// смотрим не встречалась ли нам така€ позици€ если встречалась то ее ход смотрим первым
	// т.е. мы в списке ход из хеша ставим первым а первый на то место где был ход из хеша
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	// пишем лучшего киллера
	#if KILLER
	// если неудача с хешем киллер пишем на первую позицию иначе на вторую
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for ( int i=0;i<list_surplus_moves.end_list;i++){

// реализуем ход из списка или другими словами генерируем позицию
 Onward_moves_white(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

// если король по шахом то
 if (King_white_check(bitboard)!=0){
// мы отмен€ем этот ход как некорректный
   Backs_moves_white(i,undo,&list_surplus_moves,bitboard);
 }else{
// ставим флаг что есть легальные ходы и засчитываем позицию
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// печатаем инфу о рассматриваемом ходе и глубине перебора
	Print_cout_v(&list_surplus_moves,nodes_0,i,depth_max);
	// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);
	// обновл€ем хеш ключ
	Modification_up_random_key(1,i,&list_surplus_moves,bitboard);
	// записываем ключ дл€ контрол€ при выныривании с глубины
	key=bitboard->hash_key;

	Timer_start();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// если король под шахом то увеличиваем максимальную глубину перебора 
	Extension_for_chek_white(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------
// рекурсивный вызов функции
value=Black(check,bitboard,alpha,beta,depth_max,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,0);
 

	//-----------------------------------------------
	#if SAVE_LINE
	Timer_stop();
	time_nodes= Timer_delta();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	// проверка хеш ключа на корректность т.е. что после возвращени€ с глубины он вернулс€ к 
	// состо€нию которое он имел до погружени€ на глубину
	#if HASH
	if(key!=bitboard->hash_key){
		cout <<"\n";
		cout << "ERROR HESH " ;
		cout <<"\n";
	}//if(key!=board_list_816->hash_key){
	#endif 
	//-----------------------------------------------

// восстанавливаем позицию
 Backs_moves_white(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	// восстанавливаем хеш ключ
	Modification_down_random_key(1,i,&list_surplus_moves,bitboard);
	//Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max);
    //-----------------------------------------------


 // ищем лучшую позицию за белых ищем максимум
 if (value > value_max){
   value_max = value;
   from=((list_surplus_moves.move[i]>>12)& 63);
   to=((list_surplus_moves.move[i]>>18)& 63);
// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
   Update_PV_best_point(&pv_best_point,pv_best);
 #if HASH
// хешируем лучший на данный момент ход
   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
 #endif
   //Test_Loop << "bitboard->hash_key="<<bitboard->hash_key << "\n";
  // Bitboard_print(bitboard);
 }//if (value < value_best){ 

// ЅЋќ  ALPHA BETA ќѕ“»ћ»«ј÷»» 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
// белые работают с альфой и пытаютс€ ее улучшить
 #if ALPHA_BETA_ON
 if (value > alpha){
   alpha = value;
 }//if (value < alpha){ 
// если альфа уже больше беты то можно ее возвращать 
// ведь дальше она будет только расти а мы видим что она уже сейчас будет откинута
// так что дальнейший счет смысла не имеет
//   if(alpha >= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

	//--------------------------------------------------------------------
	// выводим ход который обдумываем делаем это только на нулевой глубине т.е. у самой поверхности
	if (br!=1){
	  nodes_0 = nodes_0+1;
	  #if TEST_PRINT
	  // дл€ теста вариантов
	  Print_cout(1,bitboard,&list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value,time_nodes);
	  #else
	  // дл€ рабочего режима
	  Print_cout(1,bitboard,&list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max,value_max,time_nodes);
	  #endif
    }//if (br!=1){
    //-----------------------------------------------
    #endif //#if !COMPILING_1
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves.amount_moves;i++){
//3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333

//ЅЋќ 444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
// обработка случа€ когда не было легальных ходов

// если не одного хода сделанно не было то производим статическую оценку позиции
// это случай пата или мата
if (no_moves == 0){
  value=No_moves_white(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if SAVE_LINE // голый полный перебор
// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
 Update_PV_best(pv_best,pv_best_point);
//Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max);
// cout <<"alpha= "<<alpha;
// cout <<"\n";
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
//Print_name_from_coord(bitboard);
return value_max;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

}//int White_root(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

// перебор на нулевом уровне за черных ищем минимум
// выделил из за печати информации в оболочку. поскольку делаем это только на нулевом уровне
int Black_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // текуща€ оценка позиции
int value_min=9999999;   // лучша€ в узле  оценка позиции  
int no_moves=0;           // флаг существовани€ легальных ходов (если 1 то легальных ходов не было)
int nodes_0=1;            // количество ходов на нулевом уровне
int key=0;                // хеш ключ
int insert_hash=0;        // поместили ли ход из кеша на первую позицию
double  time_nodes=0;     // врем€ изучени€ хода
struct T_PV pv_best_point;   // лучший дл€ данного узла вариант
int check=0;
int depth_max_z=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	//-----------------------------------------------
	#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
 //
//«аполн€ем список избыточных ходов. »менно эти списки и составл€ют дерево перебора.
Generator_captures_black(bitboard,&list_surplus_moves);// генерируем вз€ти€ и превращени€
Generator_move_black(bitboard,&list_surplus_moves);// генерируем остальные ходы
//Bitboard_print(bitboard);// тестова€ печать битбоард доски
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// тестова€ печать списка и доски на нулевом уровне
	#if TEST_L
		List_816_Print(depth,list_surplus_moves);// устарела надо переписать
	//Bitboard_print(bitboard);
	#endif 
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//-----------------------------------------------
	#if HASH
	// работаем с хешем
	// смотрим не встречалась ли нам така€ позици€ если встречалась то ее ход смотрим первым
	// т.е. мы в списке ход из хеша ставим первым а первый на то место где был ход из хеша
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	// пишем лучшего киллера
	#if KILLER
	// если неудача с хешем киллер пишем на первую позицию иначе на вторую
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for ( int i=0;i<list_surplus_moves.end_list;i++){

// реализуем ход из списка или другими словами генерируем позицию
 Onward_moves_black(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

// если король по шахом то
 if (King_black_check(bitboard)!=0){
// мы отмен€ем этот ход как некорректный
   Backs_moves_black(i,undo,&list_surplus_moves,bitboard);
 }else{
// ставим флаг что есть легальные ходы и засчитываем позицию
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// печатаем инфу о рассматриваемом ходе и глубине перебора
	Print_cout_v(&list_surplus_moves,nodes_0,i,depth_max);
	// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);
	// обновл€ем хеш ключ
	Modification_up_random_key(0,i,&list_surplus_moves,bitboard);
	// записываем ключ дл€ контрол€ при выныривании с глубины
	key=bitboard->hash_key;

	Timer_start();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// если король под шахом то увеличиваем максимальную глубину перебора 
	Extension_for_chek_black(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------

// рекурсивный вызов функции
value=White(check,bitboard,alpha,beta,depth_max,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,0);
 

	//-----------------------------------------------
	#if SAVE_LINE
	Timer_stop();
	time_nodes= Timer_delta();
	#endif 
	//-----------------------------------------------

	//-----------------------------------------------
	// проверка хеш ключа на корректность т.е. что после возвращени€ с глубины он вернулс€ к 
	// состо€нию которое он имел до погружени€ на глубину
	#if HASH
	if(key!=bitboard->hash_key){
		cout <<"\n";
		cout << "ERROR HESH " ;
		cout <<"\n";
	}//if(key!=board_list_816->hash_key){
	#endif 
	//-----------------------------------------------

// восстанавливаем позицию
 Backs_moves_black(i,undo,&list_surplus_moves,bitboard);
// Bitboard_print(bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	// восстанавливаем хеш ключ
	Modification_down_random_key(0,i,&list_surplus_moves,bitboard);
	//Print_cout(list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max);
    //-----------------------------------------------


 // ищем лучшую позицию за черных ищем миниум
 if (value < value_min){
   value_min = value;
   from=((list_surplus_moves.move[i]>>12)& 63);
   to=((list_surplus_moves.move[i]>>18)& 63);
// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
   Update_PV_best_point(&pv_best_point,pv_best);
 #if HASH
// хешируем лучший на данный момент ход
   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
 #endif
   //Test_Loop << "bitboard->hash_key="<<bitboard->hash_key << "\n";
  // Bitboard_print(bitboard);
 }//if (value < value_best){ 

// ЅЋќ  ALPHA BETA ќѕ“»ћ»«ј÷»» 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
// черные пытаютс€ улучшить бету
 if (value < beta){
   beta = value;
 }//if (value < alpha){ 
// если бета меньше альфы то дальше она будет только уменьшатьс€ так что
// счет нужно прекращать
//   if(alpha >= beta) return beta; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222

	//--------------------------------------------------------------------
	// выводим ход который обдумываем делаем это только на нулевой глубине т.е. у самой поверхности
	if (br!=1){
	  nodes_0 = nodes_0+1;
	  #if TEST_PRINT
	  // дл€ теста вариантов
	  Print_cout(0,bitboard,&list_surplus_moves,*pv_best,nodes_0,nodes,i,depth_max,value,time_nodes);
	  #else
	  // дл€ рабочего режима
	  Print_cout(0,bitboard,&list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max,value_min,time_nodes);
	  #endif
    }//if (br!=1){
    //-----------------------------------------------
    #endif //#if !COMPILING_1
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves.amount_moves;i++){
//3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333

//ЅЋќ 444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
// обработка случа€ когда не было легальных ходов

// если не одного хода сделанно не было то производим статическую оценку позиции
// это случай пата или мата
if (no_moves == 0){
  value=No_moves_black(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444

//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
#if SAVE_LINE // голый полный перебор
// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
 Update_PV_best(pv_best,pv_best_point);
//Print_cout(list_surplus_moves,pv_best_point,nodes_0,nodes,i,depth_max);
// cout <<"alpha= "<<alpha;
// cout <<"\n";
#endif 
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK

return value_min;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111

}//int Black_root(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************


//	===================================================================
// ищем максимум в узле. перебор за белых 
int White(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // текуща€ оценка позиции
int value_max=-9999999;   // оценка позиции лучша€ в узле 
int no_moves=0;           // флаг существовани€ легальных ходов
int depth_max_z=0;        // форсировка на шахах
int key=0;                // хеш ключ
int insert_hash=0;        // поместили ли ход из кеша на первую позицию
struct T_PV pv_best_point;   // лучший дл€ данного узла вариант
int taking_passage_mem=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)

	//-----------------------------------------------
	#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------

//ѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕ
// на предельной глубине производим статическую оценку позиции
if (depth == depth_max){
  #if SAVE_LINE
    value=Max_depth_white(bitboard,alpha,beta,depth_max,depth,nodes,pv_current,pv_best,three_moves,description_move,br);
//  value=0;
  #endif 
 return value ;
}//	if (Depth == Depth_Max){
//ѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕ

	#if NULL_MOVE
// мы пропускаем свой ход и если даже при такой форе противник не находит
// лучшего хода эту ветку можно подрезать
// >3 потому что нужно оставить глубину на подрезку при нулевом ходе
// нулевой ход начинает работать при максимальной глубине 6 начина€ с глубины 2 (6-2=4>3)
if ((null_m==0)&&((depth_max-depth) > 3 )&&(description_move != 2)&&(check == 0)){
//if ((null_m==0)&&((depth_max-depth) > 3 )&&(check == 0)){
   taking_passage_mem = bitboard->taking_passage_yes;
   bitboard->taking_passage_yes = 0;
   null_m=1;// два раза метод в одной ветке не вызываем
   depth_max_z= depth_max;// редукци€ глубины у нас 3
   value=Black(check,bitboard,alpha,beta,depth_max_z,depth+4,nodes,pv_current,pv_best,((list_surplus_moves.move[1]>>6)& 63),three_moves,br,null_m); 
//   value=Black(bitboard,list_surplus_moves,(alpha),(alpha+1),depth_max_z,depth,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m); 
//   value=Black(bitboard,list_surplus_moves,(beta-1),(beta),depth_max_z,depth,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m); 
   bitboard->taking_passage_yes = taking_passage_mem;
//был ход черных при котором они пытались найти минимальную оценку т.е. улучшали бетту
//теперь оп€ть ход черных и если мы не смогли улучшить бетту т.е. наша оценка больше
//или равна бетте значит можно отсечь т.к. ветка нам не интересна
       if(value>=beta) return beta ;
   null_m=0;
}//if ((null_m==0)&&((depth_max-depth) > 2 )){
    #endif



// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
//насчитываем список избыточных ходов
// множество этих списков и создают дерево перебора
Generator_captures_white(bitboard,&list_surplus_moves);
Generator_move_white(bitboard,&list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
	//-----------------------------------------------
	#if HASH
	// смотрим не встречалась ли нам така€ позици€ если встречалась то ее ход смотрим первым
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif

	#if KILLER
	// если неудача с хешем киллер пишем на первую позицию иначе на вторую
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for (int i=0;i<list_surplus_moves.end_list;i++){

// реализуем ход из списка другими словами генерируем позицию
 Onward_moves_white(i,undo,&list_surplus_moves,bitboard);
// Bitboard_ini_t(bitboard,board_list_816);

// если король по шахом то
 if (King_white_check(bitboard)!=0){
// if (0){
// мы отмен€ем этот ход как некорректный
   Backs_moves_white(i,undo,&list_surplus_moves,bitboard);
 }else{
// ставим флаг что есть легальные ходы и засчитываем позицию
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);

	Modification_up_random_key(1,i,&list_surplus_moves,bitboard);
	key=bitboard->hash_key;
	#endif 
	//-----------------------------------------------

	depth_max_z= depth_max;

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// если король под шахом то увеличиваем максимальную глубину перебора 
	Extension_for_chek_white(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CAPTURES
       Extension_for_captures(i,0,((list_surplus_moves.move[i]>>6)& 63),depth_max,depth_max_z,depth);
    #endif
	//-----------------------------------------------

#if !NEGA_SCOUT
// рекурсивный вызов функции
 value=Black(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
#endif

	//------------------------------------------------------
	//
	#if NEGA_SCOUT
	if (i==0){
	// рекурсивный вызов функции
	value=Black(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}else{
	// рекурсивный вызов функции
	value=Black(check,bitboard,(alpha),(alpha+1),depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	if ((value > alpha)&&(value < beta)){
//	if (value > alpha){
	// рекурсивный вызов функции
		value=Black(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}//if ((value < alpha)&&(value < alpha)){
	}//if (i==list_surplus_moves->start_list[depth]){
	#endif
	//------------------------------------------------------

// восстанавливаем позицию
 Backs_moves_white(i,undo,&list_surplus_moves,bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	Modification_down_random_key(1,i,&list_surplus_moves,bitboard);
    //-----------------------------------------------

 if (value > value_max){
   value_max = value;

//TEST----------------------------------------------------
// используем дл€ теста киллера и хеша в полном переборе
//#if KILLER
//// пишем лучшего киллера
//  Save_killer(i,&list_surplus_moves,depth);
//#endif
// #if HASH
//// хешируем лучший на данный момент ход
//   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
// #endif
//----------------------------------------------------

// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
   Update_PV_best_point(&pv_best_point,pv_best);
 }//if (value < value_best){

// ЅЋќ  ALPHA BETA ќѕ“»ћ»«ј÷»» 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value > alpha){
   alpha = value;
//-----------------------------------------------
	#if KILLER
	// пишем лучшего киллера
	Save_killer(i,&list_surplus_moves,depth);
	#endif
	#if HASH
	// хешируем лучший на данный момент ход
	Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
	#endif
//-----------------------------------------------
 }//if (value < alpha){ 

   if(alpha >= beta) return alpha; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
#endif //#if !COMPILING_1

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){

//ЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌ
// обработка случа€ когда не было легальных ходов
// если не одного хода сделанно не было то производим статическую оценку позиции
// это случай пата или мата
if (no_moves == 0){
  value=No_moves_white(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//ЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌ

//-----------------------------------------------
#if SAVE_LINE // голый полный перебор
// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
 Update_PV_best(pv_best,pv_best_point);
#endif 
//-----------------------------------------------

return value_max;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
}//int White(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************


//	===================================================================
// ищем миниум в узле. перебор за черных
int Black(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_three_moves * three_moves, int & br, int null_m){
int value;              // текуща€ оценка позиции
int value_min=9999999;   // оценка позиции лучша€ в узле 
int no_moves=0;           // флаг существовани€ легальных ходов
int depth_max_z=0;        // форсировка на шахах
int key=0;                // хеш ключ
int insert_hash=0;        // поместили ли ход из кеша на первую позицию
struct T_PV pv_best_point;   // лучший дл€ данного узла вариант
int taking_passage_mem=0;
struct T_undo undo[1];
struct T_list_surplus_moves list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)
	//-----------------------------------------------
	#if SAVE_LINE
	// это отсечка при экстренном выходе
	if (br == 1) return 0;
	#endif 
	//-----------------------------------------------

//ѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕ
// на предельной глубине производим статическую оценку позиции
if (depth == depth_max){
  #if SAVE_LINE
    value=Max_depth_black(bitboard,alpha,beta,depth_max,depth,nodes,pv_current,pv_best,three_moves,description_move,br);
//  value=0;
  #endif 
 return value ;
}//	if (Depth == Depth_Max){
//ѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕѕ


	#if NULL_MOVE
// мы пропускаем свой ход и если даже при такой форе противник не находит
// лучшего хода эту ветку можно подрезать
if ((null_m==0)&&( (depth_max-depth)>3)&&(description_move != 2)&&(check == 0)){
//if ((null_m==0)&&((depth_max-depth) > 3 )&&(check == 0)){
   taking_passage_mem = bitboard->taking_passage_yes;
   bitboard->taking_passage_yes = 0;
   null_m=1;// два раза метод в одной ветке не вызываем
   depth_max_z= depth_max; //
   value=White(check,bitboard,alpha,beta,depth_max_z,depth+4,nodes,pv_current,pv_best,((list_surplus_moves.move[1]>>6)& 63),three_moves,br,null_m);
//   value=White(bitboard,list_surplus_moves,(beta-1),(beta),depth_max_z,depth+3,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m);
//   value=White(bitboard,list_surplus_moves,(alpha),(alpha+1),depth_max_z,depth+3,nodes,pv_current,pv_best,list_surplus_moves->description_move[1],three_moves,br,null_m);
   
   bitboard->taking_passage_yes = taking_passage_mem;
//был ход белых при котором они пытались найти максимальную оценку т.е. улучшали альфу
//теперь оп€ть ход белых и если мы не смогли улучшить альфу т.е. наша оценка меньше
//или равна альфе значит можно отсечь т.к. ветка нам не интересна
        if(value<=alpha) return alpha ;
   null_m=0;
}//

    #endif

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
//насчитываем список избыточных ходов
// множество этих списков и создают дерево перебора
Generator_captures_black(bitboard,&list_surplus_moves);
Generator_move_black(bitboard,&list_surplus_moves);
Sorting_captures_moves(&list_surplus_moves);
//Bitboard_print(bitboard);
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
	//-----------------------------------------------
	#if HASH
	// смотрим не встречалась ли нам така€ позици€ если встречалась то ее ход смотрим первым
	Look_hash(&list_surplus_moves,bitboard->hash_key,depth,insert_hash);
	#endif

	#if KILLER
	// если неудача с хешем киллер пишем на первую позицию иначе на вторую
	if(insert_hash==0){
		Insert_killer(&list_surplus_moves,depth,0);
	}else{
		Insert_killer(&list_surplus_moves,depth,1);
	}//if(insert_hash==0){
	#endif
	//-----------------------------------------------

// ЅЋќ  ѕќЋЌќ√ќ ѕ≈–≈Ѕќ–ј 111111111111111111111111111111111111111111111111111111111111111111111
no_moves = 0;
//бежим по списку избыточных ходов
// старт и конец цикла; должен быть согласован с генератором списка
for (int i=0;i<list_surplus_moves.end_list;i++){

// реализуем ход из списка другими словами генерируем позицию
 Onward_moves_black(i,undo,&list_surplus_moves,bitboard);
// Bitboard_ini_t(bitboard,board_list_816);

// если король по шахом то
 if (King_black_check(bitboard)!=0){
// if (0){
// мы отмен€ем этот ход как некорректный
   Backs_moves_black(i,undo,&list_surplus_moves,bitboard);
 }else{
// ставим флаг что есть легальные ходы и засчитываем позицию
 no_moves = 1;
 nodes = nodes+1;

	//-----------------------------------------------
	#if SAVE_LINE
	// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	Update_PV_current(i,depth,pv_current,&list_surplus_moves);

	Modification_up_random_key(0,i,&list_surplus_moves,bitboard);
	key=bitboard->hash_key;
	#endif 
	//-----------------------------------------------

	depth_max_z= depth_max;

	//-----------------------------------------------
	#if EXTENSION_CHEK
	// если король под шахом то увеличиваем максимальную глубину перебора 
	Extension_for_chek_black(bitboard,depth_max,depth_max_z,check);
	#endif
	//-----------------------------------------------

	//-----------------------------------------------
	#if EXTENSION_CAPTURES
       Extension_for_captures(i,0,((list_surplus_moves.move[i]>>6)& 63),depth_max,depth_max_z,depth);
    #endif
	//-----------------------------------------------


#if !NEGA_SCOUT
// рекурсивный вызов функции
 value=White(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
#endif

	//------------------------------------------------------
	//
	#if NEGA_SCOUT
	if (i==0){
	// рекурсивный вызов функции
	value=White(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}else{
	// рекурсивный вызов функции
	value=White(check,bitboard,(beta-1),(beta),depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
    if ((value > alpha)&&(value < beta)){
//	if (value < beta){
	// рекурсивный вызов функции
		value=White(check,bitboard,alpha,beta,depth_max_z,(depth+1),nodes,pv_current,pv_best,((list_surplus_moves.move[i]>>6)& 63),three_moves,br,null_m);
	}//if ((value < alpha)&&(value < alpha)){
	}//if (i==list_surplus_moves->start_list[depth]){
	#endif
	//------------------------------------------------------

// восстанавливаем позицию
 Backs_moves_black(i,undo,&list_surplus_moves,bitboard);

	//-----------------------------------------------
	#if SAVE_LINE
	Modification_down_random_key(0,i,&list_surplus_moves,bitboard);
    //-----------------------------------------------

 if (value < value_min){
   value_min = value;

//TEST----------------------------------------------------
// используем дл€ теста киллера и хеша в полном переборе
//#if KILLER
//// пишем лучшего киллера
//  Save_killer(i,&list_surplus_moves,depth);
//#endif
// #if HASH
//// хешируем лучший на данный момент ход
//   Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
// #endif
//----------------------------------------------------

// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
   Update_PV_best_point(&pv_best_point,pv_best);
 }//if (value < value_best){

// ЅЋќ  ALPHA BETA ќѕ“»ћ»«ј÷»» 222222222222222222222222222222222222222222222222222222222222222
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 #if ALPHA_BETA_ON
 if (value < beta){
   beta = value;
//-----------------------------------------------
	#if KILLER
	// пишем лучшего киллера
	Save_killer(i,&list_surplus_moves,depth);
	#endif
	#if HASH
	// хешируем лучший на данный момент ход
	Save_best_hash_key(i,&list_surplus_moves,bitboard->hash_key,depth);
	#endif
//-----------------------------------------------
 }//if (value < alpha){ 

   if(alpha >= beta) return beta; 
 #endif
//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
#endif //#if !COMPILING_1

}//if ((hod_check==1)||(hod_castling==1)){

}//for (int i=1;i<list_surplus_moves->amount_moves;i++){

//ЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌ
// обработка случа€ когда не было легальных ходов
// если не одного хода сделанно не было то производим статическую оценку позиции
// это случай пата или мата
if (no_moves == 0){
  value=No_moves_black(bitboard,depth,pv_best,pv_current);
return value;
}//	if (no_moves == 0){
//ЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌЌ

//-----------------------------------------------
#if SAVE_LINE // голый полный перебор
// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
 Update_PV_best(pv_best,pv_best_point);
#endif 
//-----------------------------------------------

return value_min;
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
}//int Black(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// если нет легальных ходов то это мат или пат. за белых
inline int No_moves_white(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current){
int score_m=0; 
if(King_white_check(bitboard)!=0){
  score_m = -(1000000-1000*depth);
}//if(Shah(n_king,*board_list_816 )==1){

// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
Update_PV_best_max_depth(score_m,depth,pv_best,pv_current);
 //cout <<"¬ызов нет ходов ";
 //cout <<"\n";
return score_m ;
}//int No_moves(int Depth,T_board_list_816 * board_list_816,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// если нет легальных ходов то это мат или пат. за черных
inline int No_moves_black(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current){
int score_m=0; 
if(King_black_check(bitboard)!=0){
  score_m = (1000000-1000*depth);
}//if(Shah(n_king,*board_list_816 )==1){

// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
Update_PV_best_max_depth(score_m,depth,pv_best,pv_current);
 //cout <<"¬ызов нет ходов ";
 //cout <<"\n";
return score_m ;
}//int No_moves(int Depth,T_board_list_816 * board_list_816,T_PV * PV_best,T_PV * PV_current){
//	*******************************************************************

//	===================================================================
// производим расчеты на максимальной глубине. за белых
inline int Max_depth_white(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_three_moves * three_moves,int description_move,int & br){
int score=0;
int score_f=0;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// обработка команд экстренного завершени€ счета 
if (audit>50000){
//if (0){
//если поступила команда то это или quit или stop
 if (Bioskey()){
// if (0){
char string_in2[65536];
cin.getline(string_in2,65536,'\n');
 if (strcmp(string_in2,"quit")== 0) exit(1);
 if ((strcmp(string_in2,"stop")== 0)){
// делаем ход
  br=1;
  }//if ((strcmp(string_in2,"stop")== 0)){
 }//if (Bioskey()){
 audit=0;
}//if (audit>100000){
audit=audit+1;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//------------------------------------------------------
// распознаем повторение ходов
// конечно это недостаточное условие но думаю пока сойдет
// (0)b1c3 (1)... (2)c3b1 (3)...(4)b1c3 (5)..
// обнаруживаем повторение последних двух ходов в уже сделанных ходах т.е. линии котора€ уже пришла к движку                                                                                                                                                                                        обнаруживаем повторение хода движком
//	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(pv_current->initial_position[0]==three_moves->final_position [2])&&(pv_current->final_position[0]==three_moves->initial_position[2])){
	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(((pv_current->move[0]>>12)& 63)==three_moves->final_position [2])&&(((pv_current->move[0]>>18)& 63)==three_moves->initial_position[2])){
//		if(0){
        score = 0;
// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
        Update_PV_best_max_depth(score,depth,pv_best,pv_current); 
        return score ;
	}else{
// форсировка
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// подключаем форсированный вариант
// мы уже погрузились на полуход так что альфа бета и глубина остаютс€ те же
// инверси€ оценки тут тоже будет лишней
 #if FORCED // включаем режим форсировки

// позиционную оценку мы считаем в конце основного перебора
    score = Eval_State(bitboard);
//    score = 0;
// материал мы считаем в конце форсированного варианта
// корректируем (alpha-score),(beta-score) что бы корректно обрабатывать отсечени€ ведь дальше считаем только материал
// Bitboard_print(bitboard);
	score_f=Forced_variant_white(bitboard,(alpha-score),(beta-score),depth,pv_current,pv_best);
// таким образом мы получаем комбинированную оценку в которой позиционные факторы учитываютс€
// сразу а материал только после форсированных разменов
    score = score + score_f;
    pv_best->score_move= score;
// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
// надо понимать что в pv_current ни оценки ни глубины не задаетс€ мы их сразу пишем в pv_best
//   Update_PV_best_max_depth(score,depth,pv_best,pv_current);
    return score ;
 #endif 

 #if !FORCED // включаем оценку без форсировки
// позиционную оценку мы считаем в конце основного перебора
 //   bitboard->colour_move == 1 ? inv=-1 : inv=1;
 //score = Eval_forced(bitboard);
   score = Eval_State(bitboard)+Eval_forced(bitboard);
// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
// надо понимать что в pv_current ни оценки ни глубины не задаетс€ мы их сразу пишем в pv_best
   Update_PV_best_max_depth(score,depth,pv_best,pv_current);

// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
// именно здесь можно проверить правильно ли рисуетс€ вариант на предельной глубине
// кроме  случа€ шаха и пата
//   Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
   return score ;
 #endif 
// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
 }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

//// 0 оцениваем данную позицию
//// наш статический оценщик выдает абсолютную оценку позиции. это значит что при
//// преимуществе белых оценка положительна а при приемуществе черных оценка отрицательна
//// если ход белых то оценку инвентируем потому что мы оцениваем уже схоженную позицию.
//// это что касаетс€ почему инверси€ именно при белом цвете
//// а вообще инверси€ нам нужна потому что мы используем негамакс
//      board_list_816->colour_move == 1 ? inv=-1 : inv=1;
//      score = inv*Eval_State_816(*board_list_816);
//// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
//      Update_PV_best_max_depth(score,Depth,PV_best,PV_current); 
//      return score ;
//    }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

}//int Max_depth(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_current,T_PV * PV_best,T_PV * three_moves){
//	*******************************************************************

//	===================================================================
// производим расчеты на максимальной глубине. за черных
inline int Max_depth_black(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_three_moves * three_moves,int description_move,int & br){
//int score_m=0;
int score=0;
int score_f=0;
int inv=0;          // инвертирование оценки позиции
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// обработка команд экстренного завершени€ счета 
if (audit>50000){
//if (0){
//если поступила команда то это или quit или stop
 if (Bioskey()){
// if (0){
char string_in2[65536];
cin.getline(string_in2,65536,'\n');
 if (strcmp(string_in2,"quit")== 0) exit(1);
 if ((strcmp(string_in2,"stop")== 0)){
// делаем ход
  br=1;
  }//if ((strcmp(string_in2,"stop")== 0)){
 }//if (Bioskey()){
 audit=0;
}//if (audit>100000){
audit=audit+1;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//------------------------------------------------------
// распознаем повторение ходов
// конечно это недостаточное условие но думаю пока сойдет
// (0)b1c3 (1)... (2)c3b1 (3)...(4)b1c3 (5)..
// обнаруживаем повторение последних двух ходов в уже сделанных ходах т.е. линии котора€ уже пришла к движку                                                                                                                                                                                        обнаруживаем повторение хода движком
//	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(pv_current->initial_position[0]==three_moves->final_position [2])&&(pv_current->final_position[0]==three_moves->initial_position[2])){
	if((three_moves->initial_position[0]==three_moves->final_position[2])&&(three_moves->final_position [0]==three_moves->initial_position [2])     &&(three_moves->initial_position[1]==three_moves->final_position[3])&&(three_moves->final_position [1]==three_moves->initial_position [3])   &&(((pv_current->move[0]>>12)& 63)==three_moves->final_position [2])&&(((pv_current->move[0]>>18)& 63)==three_moves->initial_position[2])){
//  if(0){
        score = 0;
// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
        Update_PV_best_max_depth(score,depth,pv_best,pv_current); 
        return score ;
	}else{
// форсировка
//FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// подключаем форсированный вариант
// мы уже погрузились на полуход так что альфа бета и глубина остаютс€ те же
// инверси€ оценки тут тоже будет лишней
 #if FORCED // включаем режим форсировки
//   score=Forced_variant_816(list_surplus_moves,alpha,beta,depth,board_list_816,nodes,pv_current,pv_best);
//   score=Forced_variant_816(list_surplus_moves,9999999,-9999999,depth,board_list_816,nodes,pv_current,pv_best);

// позиционную оценку мы считаем в конце основного перебора
    score = Eval_State(bitboard);
//    score = 0;
// материал мы считаем в конце форсированного варианта
// корректируем (alpha-score),(beta-score) что бы корректно обрабатывать отсечени€ ведь дальше считаем только материал
// Bitboard_print(bitboard);
	score_f=Forced_variant_black(bitboard,(alpha-score),(beta-score),depth,pv_current,pv_best);
// таким образом мы получаем комбинированную оценку в которой позиционные факторы учитываютс€
// сразу а материал только после форсированных разменов
    score = score + score_f;
    pv_best->score_move= score;
// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
// надо понимать что в pv_current ни оценки ни глубины не задаетс€ мы их сразу пишем в pv_best
//   Update_PV_best_max_depth(score,depth,pv_best,pv_current);
    return score ;
 #endif 

 #if !FORCED // включаем оценку без форсировки
// позиционную оценку мы считаем в конце основного перебора
 //   bitboard->colour_move == 1 ? inv=-1 : inv=1;
 //score = Eval_forced(bitboard);
   score = Eval_State(bitboard)+Eval_forced(bitboard);
// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
// надо понимать что в pv_current ни оценки ни глубины не задаетс€ мы их сразу пишем в pv_best
   Update_PV_best_max_depth(score,depth,pv_best,pv_current);

// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
// именно здесь можно проверить правильно ли рисуетс€ вариант на предельной глубине
// кроме  случа€ шаха и пата
//   Print_cout(list_surplus_moves,*pv_best,3,nodes,1,depth_max);
// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
   return score ;
 #endif 
// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
 }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

//// 0 оцениваем данную позицию
//// наш статический оценщик выдает абсолютную оценку позиции. это значит что при
//// преимуществе белых оценка положительна а при приемуществе черных оценка отрицательна
//// если ход белых то оценку инвентируем потому что мы оцениваем уже схоженную позицию.
//// это что касаетс€ почему инверси€ именно при белом цвете
//// а вообще инверси€ нам нужна потому что мы используем негамакс
//      board_list_816->colour_move == 1 ? inv=-1 : inv=1;
//      score = inv*Eval_State_816(*board_list_816);
//// у нас получилс€ вариант привод€щий к данной позиции и плюс оценка этой позиции
//      Update_PV_best_max_depth(score,Depth,PV_best,PV_current); 
//      return score ;
//    }//if(PV_current->initial_position [0]==PV_current->final_position [5]){

}//int Max_depth(int Alpha,int Beta,int Depth_Max,int Depth,T_board_list_816 * board_list_816,int & Nodes,T_PV * PV_current,T_PV * PV_best,T_PV * three_moves){
//	*******************************************************************

//	===================================================================
// реализуем продление на шахах
inline void Extension_for_chek_white(struct T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check){

// если есть шах то увеличиваем максимальную глубину перебора дл€ этой ветки
// но не более 20 полуходов
check=0;
 if(King_black_check(bitboard)!=0){
 check=1;
     if(depth_max<21){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(King_check(bitboard)==1){
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************

//	===================================================================
// реализуем продление на шахах
inline void Extension_for_chek_black(struct T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check){

// если есть шах то увеличиваем максимальную глубину перебора дл€ этой ветки
// но не более 20 полуходов
check=0;
 if(King_white_check(bitboard)!=0){
     check=1;
     if(depth_max<21){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(King_check(bitboard)==1){
}//Extension_for_chek(int for_chek,int depth_max,int & for_chek_l,int & depth_max_z)
//	*******************************************************************

//	===================================================================
// реализуем продление на вз€ти€х
inline void Extension_for_captures(int i,int start,int description_move,int depth_max,int & depth_max_new,int depth){

// если есть вз€тие то увеличиваем максимальную глубину перебора дл€ этой ветки
// но не более 10 полуходов
 if(description_move == 2){
// if(i == start){
     if(depth < 4){
 //    if((depth<5)&&(depth_max<15)){
	    depth_max_new=depth_max+1;
     }//if(depth_max<21){
 }//if(description_move == 2){
}//void Extension_for_captures(int description_move,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z){
//	*******************************************************************



//	===================================================================
void Print_name_from_coord(struct T_Bitboard * bitboard){

  for (int i=0;i<8;i++){
       for (int j=0;j<8;j++){
           cout << bitboard->white_name_from_coord[(7-i)*8 + j];
	   }//
       cout << "\n";
  }//
cout << "\n";
  for (int i=0;i<8;i++){
       for (int j=0;j<8;j++){
           cout << bitboard->black_name_from_coord[(7-i)*8 + j];
	   }//
       cout << "\n";
  }//

  //for (int i=0;i<64;i++){
  //   cout << bitboard->white_name_from_coord[i];
  //   if (i%8 ==7) cout << "\n";
  //}

}//void Print_name_from_coord(struct T_Bitboard * bitboard){


//	*******************************************************************



