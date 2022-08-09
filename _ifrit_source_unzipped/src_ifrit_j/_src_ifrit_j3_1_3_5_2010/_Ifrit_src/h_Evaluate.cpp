#include <iostream> // TEST
#include <fstream>  // TEST
#include <string>


#include "h_Evaluate.h"


//---------------------------------------------------------------------------
/* +
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 24.02.2009
*/
//---------------------------------------------------------------------------


/*
оцениваем позицию

считаем 
материал (веса белых плюсуем а веса черных отнимаем)
позиционную оценку PST
мобильность фигур
атаку на окрестности короля
проходные


 надо помнить что при такой задаче на доску мы смотрим
 как на зеркально отображенную относительно центральной горизонтальной линии

 и действительно доска у нас выглядит как
  A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
  A7(48) B7(49) C7(50) D7(51) E7(52) F7(53) G7(54) H7(55)
  A6(40) B6(41) C6(42) D6(43) E6(44) F6(45) G6(46) H6(47)
  A5(32) B5(33) C5(34) D5(35) E5(36) F5(37) G5(38) H5(39)
  A4(24) B4(25) C4(26) D4(27) E4(28) F4(29) G4(30) H4(31)
  A3(16) B3(17) C3(18) D3(19) E3(20) F3(21) G3(22) H3(23)
  A2(8)  B2(9)  C2(10) D2(11) E2(12) F2(13) G2(14) H2(15)
  A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

  а массив читается слево направо и сверху вниз

  поэтому таблицы зеркальные относительно горизонтальной линии и белые и черные читают
  одинаково типа test[i]
*/


//	===================================================================
// функция получает позицию и оценивает ее статически смотрим позиционные факторы
__int32 EVALUATE::Eval_State(const T_Bitboard * p_bitboard// представление доски
			         ){
__int32 eval = 0;//статическая оценка
unsigned __int8 from_white_king = 0;// расположение белого короля
unsigned __int8 from_black_king = 0;// расположение черного короля
unsigned __int8 at_white_king = 0; // количество атакованных полей вокруг белого короля
unsigned __int8 at_black_king = 0;// количество атакованных полей вокруг черного короля

// нужно для вычисления стадии игры
unsigned __int16 stadia_w = 0; // сумарный вес фигур у белых
unsigned __int16 stadia_b = 0; // сумарный вес фигур у черных

//в начальной позиции мы имеем 400 + 400 + 600 + 1200 = 2600 (для одной стороны)
//                             2*400 + 2*400 + 2*600 + 1200 
//                             800   + 800   + 1200  + 1200 = 4000 (сумма для двух сторон) 
for(unsigned __int8 i = 0; i < 64; i++) {
	stadia_w = stadia_w + rubikon_kod[p_bitboard->white_name_from_coord[i]];
    stadia_b = stadia_b + rubikon_kod[p_bitboard->black_name_from_coord[i]];
}//for(int i=0 ;i<64;i++) {

//stadia_w = 2600; // сумарный вес фигур у белых
//stadia_b = 2600; // сумарный вес фигур у черных

// обсчитываем короля
EVALUATE_private::King_pst( p_bitboard,// представление доски
          from_white_king,// расположение белого короля
          from_black_king,// расположение черного короля
          stadia_w, // сумарный вес фигур у белых
          stadia_b, // сумарный вес фигур у черных
          eval //статическая оценка
         );

// обсчитываем пешки
EVALUATE_private::Pawn_pst( p_bitboard,// представление доски
          from_white_king,// расположение белого короля
          from_black_king,// расположение черного короля
          stadia_w, // сумарный вес фигур у белых
          stadia_b, // сумарный вес фигур у черных
          eval //статическая оценка
         );

// обсчитываем ферзь, ладью, слон, конь
// бонус расположения 
EVALUATE_private::PST( p_bitboard,// представление доски
     from_white_king,// расположение белого короля
     from_black_king,// расположение черного короля
     at_white_king, // количество атакованных полей вокруг белого короля
     at_black_king, // количество атакованных полей вокруг черного короля
     eval //статическая оценка
    );


return eval;

}//int EVALUATE::Eval_State
//	*******************************************************************

//	===================================================================
//
 inline void EVALUATE_private::King_pst(const T_Bitboard * p_bitboard,// представление доски
                           unsigned __int8 & from_white_king,// расположение белого короля
                           unsigned __int8 & from_black_king,// расположение черного короля
                           unsigned __int16 stadia_w, // сумарный вес фигур у белых
                           unsigned __int16 stadia_b, // сумарный вес фигур у черных
                           __int32 & eval //статическая оценка
                           ){

unsigned __int64 from_mask = 0; // маска расположения фигур одного типа

unsigned __int8 from; // координата поля откуда фигура ходит

/*
здесь обсчитываем рокировку и прикрытие короля фигурами 
по сути меры направлены на безопасность короля

Рубикон это граница перехода в эндшпиль.
есть две разные оценки - до рубикона и после. до рубикона действуют бонусы за прикрытие
фигурами и рокировку а после рубикона уже нет.

прикрытие фигурами(не обязательно пешками) это три фигуры перед королем. за прорехи в щите идет штраф

*/
// белый король
from_mask = p_bitboard->white_piece[6];

if(from_mask != 0) {
   from = Bit_scan_forward_win32(from_mask);
   from_white_king = from;
//cout << "короли " << "\n";
//cout << "from= "<< from << "\n";
//cout << "Pozicion_k_e[from]= "<< Pozicion_k_e[from] << "\n";

   if(stadia_b > rubikon){

//-----------------------------------------------------------
 // фрагмент доски у нас выглядит как
 // .........
 // A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

// срыв рокировки 0-0 т.е. король побежал прятаться без рокировки :)
// к сведению: 64 - 6 bit  128 - 7 bit 64 + 128 = 192 // включены 6 и 7 бит
//	  if ( 
//          ( (p_bitboard->white_piece[4] & 192) != 0 ) //  ладья заперта на 6 или 7 клетках
//		  && ( (from == 6) || (from == 5) ) //  белый король в зоне рокировки 0-0
//		  ) eval = eval - BONUS_CASTLING_O_O; // тогда штрафуем 
//
//// срыв рокировки 0-0-0 т.е. король побежал прятаться без рокировки :)
//// к сведению: 1 - 0 bit   2 - 1 bit
//// 1 + 2 = 3 // включены 0,1 биты
//	  if ( 
//          ( (p_bitboard->white_piece[4] & 3) != 0 ) //  ладья заперта на 6 или 7 клетках
//		  && ( (from == 1) || (from == 2) || (from == 3) ) //  белый король в зоне рокировки 0-0-0
//		  ) eval = eval - BONUS_CASTLING_O_O_O; // тогда штрафуем 
//-----------------------------------------------------------

         eval = eval + Pozicion_k[from] - King_shield(from,1,p_bitboard,BONUS_SHIELD);

     }else{// if(stadia_b > rubikon){

         // после рубикона король должен стремиться к центру
         eval = eval + Pozicion_k_e[from];

     }// if(stadia_b > rubikon){
}//if(from_mask != 0) {


//черный король
from_mask = p_bitboard->black_piece[6];

//from_mask = 0;
if(from_mask != 0) {
   from = Bit_scan_forward_win32(from_mask);
   from_black_king = from;

   if(stadia_w > rubikon){

//-----------------------------------------------------------
 // фрагмент доски у нас выглядит как
// A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
 // .........

// срыв рокировки 0-0 т.е. король побежал прятаться без рокировки :)
// к сведению: 4611686018427387904 - 62 bit  9223372036854775808 - 63 bit 
// 4611686018427387904 + 9223372036854775808 = 13835058055282163712  включены 62 и 63 бит
//	  if ( 
//          ( (p_bitboard->black_piece[4] & 13835058055282163712) != 0 ) //  ладья заперта на 6 или 7 клетках
//		  && ( (from == 61) || (from == 62) ) //  черный король в зоне рокировки 0-0
//		  ) eval = eval + BONUS_CASTLING_O_O; // тогда штрафуем 
//
//// срыв рокировки 0-0-0 т.е. король побежал прятаться без рокировки :)
//// к сведению: 72057594037927936 - 56 bit   144115188075855872 - 57 bit
//// 72057594037927936 + 144115188075855872 = 216172782113783808  включены 56,57 биты
//	  if ( 
//          ( (p_bitboard->black_piece[4] & 216172782113783808) != 0 ) //  ладья заперта на 6 или 7 клетках
//		  && ( (from == 57) || (from == 58) || (from == 59) ) //  черный король в зоне рокировки 0-0-0
//		  ) eval = eval + BONUS_CASTLING_O_O_O; // тогда штрафуем 
//-----------------------------------------------------------

         eval = eval - Pozicion_k[from] + King_shield(from,0,p_bitboard,BONUS_SHIELD);

    }else{

       // после рубикона король должен стремиться к центру
         eval = eval - Pozicion_k_e[from];

    }// if(stadia_w > rubikon){

}//if(from_mask != 0) {

}//inline void EVALUATE::King
//	*******************************************************************

//	===================================================================
// обсчет бонуса за расположение 
 inline void EVALUATE_private::Pawn_pst(const T_Bitboard * p_bitboard,// представление доски
                           unsigned __int8 & from_white_king,// расположение белого короля
                           unsigned __int8 & from_black_king,// расположение черного короля
                           unsigned __int16 stadia_w, // сумарный вес фигур у белых
                           unsigned __int16 stadia_b, // сумарный вес фигур у черных
                           __int32 & eval //статическая оценка
                           ){


 // нужно для определения сдвоенных пешек 
  unsigned __int8 Structure_pawn_w[]={
		     0,0,0,0,0,0,0,0
            };
  unsigned __int8 Structure_pawn_b[]={
	         0,0,0,0,0,0,0,0
            };

unsigned __int64 from_mask = 0; // маска расположения фигур одного типа

unsigned __int8 from; // координата поля откуда фигура ходит

bool end; // включаем бонус эндшпеля для проходных

// количество открытых линий перед пешкой считаем только если путь открыт
// т.е. по сути таких линий две - слева и справа от пешки
unsigned __int8 cnt_line_pawn_white = 0;
unsigned __int8 cnt_line_pawn_black = 0;

/*

Рубикон это граница перехода в эндшпиль.
есть две разные оценки - до рубикона и после.

за сдвоенные пешки штраф

*/

// белые

//пешки----------------------------------------------------------------------------------
end = 0;
from_mask = p_bitboard->white_piece[1];
while(from_mask != 0){

   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask-1);

   if(stadia_b > rubikon){
      eval = eval + Pawn[from];
      //eval = eval + test[from];
//cout << "from= "<< from << "\n";
//cout << "test[from]= "<< test[from] << "\n";
   }else{
      end = 1;
      eval = eval + Pawn_e[from];

//cout << "from= "<< from << "\n";
//cout << "from % 8= "<< from % 8 << "\n";

   }// if(stadia_b > rubikon){

	// определяем проходные
    // в случае если перед пешкой нет вражеской пешки
	if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){ 

		eval = eval + 2 * PERSPECTIVE_PAWNN;// 10
        cnt_line_pawn_white = 0;

		// для случая крайней проходной
        if( ((from % 8) == 7) || ((from % 8) == 0) ) cnt_line_pawn_white = 1;

        // в случае если справо перед пешкой нет вражеской пешки
		if((from % 8) != 7) if(!(e_rook_up_masks[from + 1] & p_bitboard->black_piece[1])){ 
			eval = eval + PERSPECTIVE_PAWNN;
			cnt_line_pawn_white = cnt_line_pawn_white + 1;
		}//if((from % 8) != 7) if(!(e_rook_up_masks[from + 1] & p_bitboard->black_piece[1])){

        // в случае если слево перед пешкой нет вражеской пешки
		if((from % 8) != 0) if(!(e_rook_up_masks[from - 1] & p_bitboard->black_piece[1])){
			eval = eval + PERSPECTIVE_PAWNN;
            cnt_line_pawn_white = cnt_line_pawn_white + 1;
		}//if((from % 8) != 0) if(!(e_rook_up_masks[from - 1] & p_bitboard->black_piece[1])){

        // все три линии открыты это чистая проходная.
        if(cnt_line_pawn_white == 2) eval = eval + end * Perspective_pawn_e[from]; 
		                                        // + 26 * PERSPECTIVE_PAWNN;

	}//if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){ 


 //если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
	Structure_pawn_w[from % 8] = Structure_pawn_w[from % 8] + 1;

}//while(from_mask != 0){


// черные

//пешки----------------------------------------------------------------------------------
end = 0;
from_mask = p_bitboard->black_piece[1];
while(from_mask != 0){
   from = Bit_scan_forward_win32(from_mask);
   from_mask = from_mask & (from_mask-1);

   if(stadia_w > rubikon){
     eval = eval - Pawn[63 - from];//
     //eval = eval - test[from];//
   }else{
     end = 1;
     eval = eval - Pawn_e[63 - from];

   }//if(stadia_w > rubikon){

	// определяем проходные
    // в случае если перед пешкой нет вражеской пешки
	if(!(e_rook_down_masks[from] & p_bitboard->white_piece[1])){ 

		eval = eval - 2 * PERSPECTIVE_PAWNN;
        cnt_line_pawn_black = 0;

		// для случая крайней проходной
        if( ((from % 8) == 7) || ((from % 8) == 0) ) cnt_line_pawn_black = 1;

        // в случае если справо перед пешкой нет вражеской пешки
		if((from % 8) != 7) if(!(e_rook_down_masks[from + 1] & p_bitboard->white_piece[1])){
			eval = eval - PERSPECTIVE_PAWNN;
            cnt_line_pawn_black = cnt_line_pawn_black + 1;
		}//if((from % 8) != 7) if(!(e_rook_down_masks[from + 1] & p_bitboard->white_piece[1])){

        // в случае если слево перед пешкой нет вражеской пешки
		if((from % 8) != 0) if(!(e_rook_down_masks[from - 1] & p_bitboard->white_piece[1])){
			eval = eval - PERSPECTIVE_PAWNN;
            cnt_line_pawn_black = cnt_line_pawn_black + 1;
		}//if((from % 8) != 0) if(!(e_rook_down_masks[from - 1] & p_bitboard->white_piece[1])){

        // все три линии открыты это чистая проходная.
        if(cnt_line_pawn_black == 2) eval = eval - end * Perspective_pawn_e[63 - from]; 
		                                         //- 26 * PERSPECTIVE_PAWNN;

	}//if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){


// если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
	Structure_pawn_b[from % 8] = Structure_pawn_b[from % 8] + 1;
}//while(from_mask != 0){

// штрафуем за сдвоенные пешки
  for(int ii = 0; ii < 8; ii++){
    if(Structure_pawn_w[ii] > 1) eval = eval - BONUS_PAWNN; // сейчас штраф 20
    if(Structure_pawn_b[ii] > 1) eval = eval + BONUS_PAWNN;
    //cout << "ii = "<< ii << "\n";
    //cout << "Structure_pawn_w[ii]= "<< Structure_pawn_w[ii] << "\n";
  }//for(int ii = 0; ii < 8; ii++){

}//inline void EVALUATE::Pawn
//	*******************************************************************



//	===================================================================
// обсчет бонуса за расположение ферзей, ладей, слонов, коней
// обсчет контролируемого пространства этими фигурами
// обсчет атаки полей вокруг королей этими фигурами
 inline void EVALUATE_private::PST(const T_Bitboard * p_bitboard,// представление доски
                      unsigned __int8 from_white_king,// расположение белого короля
                      unsigned __int8 from_black_king,// расположение черного короля
                      unsigned __int8 & at_white_king, // количество атаковнных полей вокруг белого короля
                      unsigned __int8 & at_black_king, // количество атаковнных полей вокруг черного короля
                      __int32 & eval //статическая оценка
                      ){

unsigned __int8 from; // координата поля откуда фигура ходит
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа

// для вычисления расстояния между фигурами
//int dx;// дистанция по горизонтали
//int dy;// дистанция по вертикали
//int distanse;// вектор расстояния
//const int kd = 7;// коэффициент дистанции

/*
ладьям и слонам бонус за захват пространства причем бонус несколько
странный(зато быстрее считается :)) он включает и занятые поля

кони стремятся к центру
*/

// ферзь-----------------------------------------------------------------
//from_mask = p_bitboard->white_piece[5];
//while(from_mask != 0){
//    from = Bit_scan_forward_win32(from_mask);
//    from_mask = from_mask & (from_mask - 1);
//
////dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
////dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
////distanse = kd/(dx + dy);
////distanse = 0;
//
//	eval = eval + CONTROL_SQUARE
//	* Queen_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
//	              from, from_black_king, at_black_king);
//
//  }//while(from_mask != 0)

//ладьи----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[4];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask - 1);

    eval = eval + CONTROL_SQUARE 
	* Rook_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	             from, from_black_king, at_black_king);

//    cout << "Rook_amoun_white(p_bitboard)= "<< Rook_amoun(p_bitboard, from) << "\n";
}//while(from_mask != 0)

//слон---------------------------------------------------------------------------------- 
from_mask = p_bitboard->white_piece[3];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval + Pozicion_b[from] 
	//+ CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//	                            from, from_black_king, at_black_king)
	;

//cout << "Bishop_amoun(p_bitboard, from)= "<< Bishop_amoun(p_bitboard, from) << "\n";
}//while(from_mask != 0)

//конь ----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[2];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask - 1);

    eval = eval + Pozicion_n[from] 
	//+ CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//	                           from, from_black_king, at_black_king)
    ;

 
}// while(from_mask != 0)


/////////////////////////////////////


//ферзь-----------------------------------------------------------------
//from_mask = p_bitboard->black_piece[5];
//while(from_mask != 0){
//    from = Bit_scan_forward_win32(from_mask);
//    from_mask = from_mask & (from_mask-1);
//
////dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
////dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
////distanse = kd/(dx + dy);
////distanse = 0;
//
//    eval = eval - CONTROL_SQUARE 
//	* Queen_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
//	              from_white_king, at_white_king);
//
//}//while(from_mask != 0)

//ладьи----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[4];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - CONTROL_SQUARE 
	* Rook_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
	             from_white_king, at_white_king);

}//while(from_mask != 0){

//слон---------------------------------------------------------------------------------- 
from_mask = p_bitboard->black_piece[3];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - Pozicion_b[from] 
	//- CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece,p_bitboard->all_black_piece, 
	//	                            from, from_white_king, at_white_king)
	;


}//while(from_mask != 0){

//конь ----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[2];
while(from_mask != 0){
    from = Bit_scan_forward_win32(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - Pozicion_n[from] 
	//- CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, 
	//	                           from, from_white_king, at_white_king)
	;


 }// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// бонус за атаку короля
//eval = eval + ATAK_KING * (at_black_king - at_white_king);


}// inline int EVALUATE::PST(const T_Bitboard * p_bitboard// представление доски
//	*******************************************************************


//	*******************************************************************


//	===================================================================
// штраф за разрушение щита перед королем. если нет прикрытия то назначается штраф 
 inline __int32 EVALUATE_private::King_shield(unsigned __int8 n,// положение короля (from)
					         unsigned __int8 z,// цвет короля
					         const T_Bitboard * p_bitboard,// представление доски
					         const unsigned __int8 SHIELD// бонус за прикрытие короля
					         ){

unsigned __int8 st = 0;

if(z == 1){
// смотрим на клетку вверх от короля
   if(p_bitboard->white_name_from_coord[n + 7] == 0) st = st + SHIELD;
// смотрим на клетку влево вверх
   if(p_bitboard->white_name_from_coord[n + 8] == 0) st = st + SHIELD;
// смотрим на клетку вправо вверх
   if(p_bitboard->white_name_from_coord[n + 9] == 0) st = st + SHIELD;
}else{
// смотрим на клетку вниз от короля
   if(p_bitboard->black_name_from_coord[n - 7] == 0) st = st + SHIELD;
// смотрим на клетку  влево вниз
   if(p_bitboard->black_name_from_coord[n - 8] == 0) st = st + SHIELD;
// смотрим на клетку вправо вниз
   if(p_bitboard->black_name_from_coord[n - 9] == 0) st = st + SHIELD;
}//if(z==1){
return st;
}//inline int EVALUATE::King_shield
//	*******************************************************************

//	===================================================================
// функция получает позицию и оценивает ее статически смотрим материал и только
__int32 EVALUATE::Eval_forced(const T_Bitboard * p_bitboard// представление доски
				      ){

unsigned __int64 from_mask; // маска расположения фигур одного типа
__int32 ocenka_f = 0;

// белые плюсуются

// король----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[6];
  if(from_mask != 0) ocenka_f = ocenka_f + KING;

// ферзь-----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[5];
  while(from_mask != 0){
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + QUEEN;
  }//while(from_mask != 0){

// ладья-----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + ROOK;
  }//while(from_mask != 0){

// слон------------------------------------------------------------------- 
  from_mask = p_bitboard->white_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + BISHOP;
 }//while(from_mask != 0){

// конь -------------------------------------------------------------------
  from_mask = p_bitboard->white_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + KNIGHT;
 }// while(from_mask != 0){

// пешка--------------------------------------------------------------------
  from_mask = p_bitboard->white_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + PAWN;
}//while(from_mask != 0){

///////////////////////////////////

// черные минусуются(отнимаются)

// король----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[6];
  if(from_mask != 0) ocenka_f = ocenka_f - KING;

// ферзь-----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[5];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - QUEEN;
  }//while(from_mask != 0){

// ладья-----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - ROOK;
  }//while(from_mask != 0){

// слон------------------------------------------------------------------- 
  from_mask = p_bitboard->black_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - BISHOP;
 }//while(from_mask != 0){

// конь -------------------------------------------------------------------
  from_mask = p_bitboard->black_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - KNIGHT;
 }// while(from_mask != 0){

// пешка--------------------------------------------------------------------
  from_mask = p_bitboard->black_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - PAWN;
}//while(from_mask != 0){

//--------------------------------------------------------------------------
// это другой вариант простой оценки. он проще но все же медленнее
//int eval = 0;
//
//for(int i = 0; i < 64; i++){
//	 eval = eval + kod[p_bitboard->white_name_from_coord[i]];
//   eval = eval - kod[p_bitboard->black_name_from_coord[i]];
//}//for(int i = 0; i < 64; i++) {
//
//if (eval != ocenka_f) cout << "KARAUUUUUUUUUUUUUUL"<< "\n";
//--------------------------------------------------------------------------

return ocenka_f;
}//int EVALUATE::Eval_forced
//	*******************************************************************


//	===================================================================
// просчитываем все возможные ходы ладьи. возвращаем количество ходов
// и количество атакованных полей вокруг короля
 inline unsigned __int8 EVALUATE_private::Rook_amoun(unsigned __int64  all_piece,// все фигуры
                       unsigned __int64  piece,// свои фигуры
					   unsigned __int8 from,// расположение ферзя
					   unsigned __int8 from_king,// расположение короля
					   unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
					        ){

unsigned __int64 blocking_square_mask = 0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

__int32 blocking_coordinate = 0; // координата блокированного поля 
unsigned __int8 number = 0; // координата поля откуда фигура ходит
unsigned __int8 number_n = 0; // координата поля откуда фигура ходит
// ходы ладьи----------------------------------------------------------------------------------

// смотрим в четырех различных направлениях

// луч вверх----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч влево----------------------------------------------------
// blocking_square_mask = all_piece & e_rook_left_masks[from];
// if(blocking_square_mask !=0){ 
//   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
//   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
// }else{
//   move_mask = e_rook_left_masks[from];
// }//if(blocking_square_mask !=0){
//move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вниз-----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------


// луч вправо---------------------------------------------------
// blocking_square_mask = all_piece & e_rook_right_masks[from];
// if(blocking_square_mask !=0){ 
//   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
//   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
// }else{
//   move_mask = e_rook_right_masks[from];
// }//if(blocking_square_mask !=0){ 
//move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// убираем взятия своих фигур
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// Заполняем список ходов
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

  return number_n;
}//inline int EVALUATE::Rook_amoun
//	*******************************************************************

//	===================================================================
// просчитываем все возможные ходы слона. возвращаем количество ходов
// и количество атакованных полей вокруг короля
 inline unsigned __int8 EVALUATE_private::Bishop_amoun(unsigned __int64  all_piece,// все фигуры
                       unsigned __int64  piece,// свои фигуры
					   unsigned __int8 from,// расположение ферзя
					   unsigned __int8 from_king,// расположение короля
					   unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
						      ){

unsigned __int64 blocking_square_mask=0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

__int32 blocking_coordinate = 0; // координата блокированного поля 
unsigned __int8 number = 0; // координата поля откуда фигура ходит
unsigned __int8 number_n = 0; // координата поля откуда фигура ходит

// ходы слона---------------------------------------------------------------------------------- 

// смотрим в четырех различных направлениях

// луч вверх - вправо
 blocking_square_mask = all_piece & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вверх - влево
   blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вниз - влево
 blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вниз - вправо
 blocking_square_mask = all_piece & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
  }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// убираем взятия своих фигур
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// Заполняем список ходов
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

  return number_n;
}//inline int EVALUATE::Bishop_amoun
//	*******************************************************************


//	===================================================================
// просчитываем все возможные ходы ферзя. возвращаем количество ходов
// и количество атакованных полей вокруг короля
 inline unsigned __int8 EVALUATE_private::Queen_amoun(unsigned __int64  all_piece,// все фигуры
                       unsigned __int64  piece,// свои фигуры
					   unsigned __int8 from,// расположение ферзя
					   unsigned __int8 from_king,// расположение короля
					   unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
					         ){

unsigned __int64 blocking_square_mask=0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

__int32 blocking_coordinate = 0; // координата блокированного поля 
unsigned __int8 number = 0; // координата поля откуда фигура ходит
unsigned __int8 number_n = 0; // координата поля откуда фигура ходит

// ходы ладьи----------------------------------------------------------------------------------

// смотрим в четырех различных направлениях

// луч вверх----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч влево----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_left_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вниз-----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------


// луч вправо---------------------------------------------------
 blocking_square_mask = all_piece & e_rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 

// смотрим в четырех различных направлениях

// луч вверх - вправо
 blocking_square_mask = all_piece & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вверх - влево
   blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_forward_win32(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вниз - влево
 blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// луч вниз - вправо
 blocking_square_mask = all_piece & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate = Bit_scan_reverse_win32(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
  }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// убираем взятия своих фигур
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// Заполняем список ходов
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

//if (number_n != number) cout << "ЗАСАДА!!!!!!!!!! " << "\n";

  return number_n;
}//inline int EVALUATE::Queen_amoun
//	*******************************************************************


//	===================================================================
// просчитываем все возможные ходы коня. возвращаем количество ходов
// и количество атакованных полей вокруг короля
 inline unsigned __int8 EVALUATE_private::Knigt_amoun(unsigned __int64  all_piece,// все фигуры
                       unsigned __int64  piece,// свои фигуры
					   unsigned __int8 from,// расположение ферзя
					   unsigned __int8 from_king,// расположение короля
					   unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
					         ){

unsigned __int64 blocking_square_mask=0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

__int32 blocking_coordinate = 0; // координата блокированного поля 
unsigned __int8 number = 0; // координата поля откуда фигура ходит
unsigned __int8 number_n = 0; // координата поля откуда фигура ходит

// находим ходы на свои фигуры
// а потом убираем их из всех ходов
move_mask = e_knigt_moves_masks[from] ^ (e_knigt_moves_masks[from] & piece) ;

if ((e_king_moves_masks[from_king] & move_mask) != 0 ) at_cnt = at_cnt + 1 ;

// Заполняем список ходов---------------------------------------
   while (move_mask !=0){
     move_mask = move_mask & (move_mask-1);
     number = number + 1;
   }//while (move !=0){
//--------------------------------------------------------------

  return number;
}//inline int EVALUATE::Knigt_amoun
//	*******************************************************************


//	===================================================================
// ищем первый установленный бит(т.е. бежим справа)
 __inline __int32 EVALUATE_private::Bit_scan_forward_win32(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
							 ){ 

   _asm { 
/*
из учебника по ассемблеру(This document saved from http://mind.telenet.ru/samouch/7/31/index.html):
Команда mov замещает первый операнд (приемник) вторым (источником). 
При этом исходное значение первого операнда теряется. 
Второй операнд не изменяется. В зависимости от описания операндов, 
пересылается слово или байт. Если операнды описаны по-разному или 
режим адресации не позволяет однозначно определить размер операнда, 
для уточнения размера передаваемых данных в команду следует включить один 
из атрибутных операторов byte ptr или word ptr. 
Команда не воздействует на флаги процессора. 
Часто возникает необходимость выполнить обратную операцию - 
к паре байтов обратиться, как к слову. Для этого надо использовать оператор word ptr:
*/
           mov  eax, dword ptr analyzed_number[0]
/*
Команда test выполняет операцию логического умножения 
И над двумя операндами и, в зависимости от результата, 
устанавливает флаги SF, ZF и PF. Флаги OF и CF сбрасываются, 
a AF имеет неопределенное значение. Состояние флагов можно затем 
проанализировать командами условных переходов. Команда test не изменяет 
ни один из операндов. В качестве первого операнда команды test можно 
указывать регистр (кроме сегментного) или ячейку памяти, в качестве 
второго - регистр (кроме сегментного), ячейку памяти или непосредственное 
значение, однако не допускается определять оба операнда одновременно как 
ячейки памяти. Операнды могут быть байтами или словами и представлять числа 
со знаком или без знака. 

Правила побитового умножения:

Первый операнд-бит 0101
Второй операнд-бит 0011
Бит результата 0001

Флаг SF устанавливается в 1, если в результате выполнения команды 
образовалось число с установленным знаковым битом.
Флаг ZF устанавливается в 1, если в результате выполнения команды 
образовалось число, состоящее из одних двоичных нулей.
Флаг PF устанавливается в 1, если в результате выполнения команды 
образовалось число с четным количеством двоичных единиц в его битах.
*/
           test eax, eax
/*
Команда Перейти, если Условие перехода
jz нуль ZF= 1
*/
           jz   grown_up
/*
Команда bsf сканирует слово или двойное слово в поисках бита, равного 1. 
Сканирование выполняется от младшего бита (0) к старшему. Если в слове 
не найдено установленных битов, то устанавливается флаг ZF. 
Если установленные биты есть, то номер первого установленного бита 
заносится в указанный в команде регистр. Номером бита считается его 
позиция в слове, отсчитываемая от бита 0. В качестве первого операнда команды bsf 
следует указывать регистр, куда будет помещен результат сканирования, в 
качестве второго - регистр или ячейку памяти со сканируемым словом. 
В команде bsf допустимо использование как 16-битовых, так и 32-битовых операндов 
(но и первый, и второй операнды должны быть одного типа).
*/
           bsf  eax, eax
/*
Команда jmp передает управление в указанную точку того же или другого программного 
сегмента. Адрес возврата не сохраняется. Команда не воздействует на флаги процессора.
*/
           jmp jump_end
           grown_up:    bsf  eax, dword ptr analyzed_number[4]
/*
сложение
*/
           add  eax, 20h
           jump_end:
       }//_asm {
}//__inline int EVALUATE::Bit_scan_forward_win32
//	*******************************************************************

//	===================================================================
// ищем последний установленный бит(т.е. бежим слева)
 __inline __int32 EVALUATE_private::Bit_scan_reverse_win32(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
							){ 

  _asm { 
           mov  eax, dword ptr analyzed_number[4]
           test eax, eax
           jz   younger
/*
Команда bsr сканирует слою или двойное слово в поисках бита, равного 1. 
Сканирование выполняется от старшего бита (15 или 31) к младшему. 
Если в слове не найдено установленных битов, то устанавливается флаг ZF. 
Если установленные биты есть, то номер первого установленного бита заносится 
в указанный в команде регистр. Номером бита считается его позиция в слове, 
отсчитываемая от бита 0. В качестве первого операнда команды bsr следует 
указывать регистр, куда будет помещен результат сканирования, в качестве 
второго - регистр или ячейку памяти со сканируемым словом. 
В команде bsr допустимо использование как 16-битовых, так и 32-битовых операндов, 
но и первый, и второй операнды должны быть одного типа (за исключением случая, 
когда второй операнд - константа).
*/
           bsr  eax, eax
           add  eax, 20h
           jmp  jump_end
           younger:    bsr  eax, dword ptr analyzed_number[0]
           jump_end:
       }//_asm { 
}//__inline int EVALUATE::Bit_scan_reverse_win32
//	*******************************************************************




