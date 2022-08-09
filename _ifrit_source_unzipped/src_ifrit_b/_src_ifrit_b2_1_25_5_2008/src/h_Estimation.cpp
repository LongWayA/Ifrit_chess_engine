#include <fstream.h>  // TEST
#include "h_Estimation.h"
#include "k_Util.h"
//#include "d_Bitboard_gen.h"
//-----------------------------------------------------------------------------------
/* +
 * оценка позиции
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 26.04.2008
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
 * все что здесь проходит можно проверить в модуле d_Loop
*/
//---------------------------------------------------------------------------
//
// кони должны стремиться к центру
// 100 это бонус за существование коней сделанно что бы конь не менялся на 3 пешки
static int Pozicion_n[] ={
				 90,90 ,90 ,90 ,90 ,90 ,90 ,90,
 		         90,100,100,100,100,100,100,90,
 		         90,100,115,115,115,115,100,90,
 		         90,100,115,110,110,115,100,90,
 		         90,100,115,110,110,115,100,90,
 		         90,100,115,115,115,115,100,90,
 		         90,100,100,100,100,100,100,90,
 		         90,90 ,90 ,90 ,90 ,90 ,90 ,90,
 		         };
//
// слоны должны стремиться контролировать главные диагонали
// 100 это бонус за существование слонов сделанно что бы слон не менялся на 3 пешки
static int Pozicion_b[] ={
				 100,100,90 ,90 ,90 ,90 ,100,100,
 		         100,115,110,100,100,110,115,100,
 		         90 ,110,115,110,110,115,110,90,
 		         90 ,100,110,115,115,110,100,90,
 		         90 ,100,110,115,115,110,100,90,
 		         90 ,110,115,110,110,115,110,90,
 		         100,115,110,100,100,110,115,100,
 		         100,100 ,90 ,90 ,90 ,90 ,100,100,
 		         };


// 100 это бонус за существование ферзей и ладей
int Pozicion_q[] ={
				 100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         100,100,100,100,100,100,100,100,
 		         };
 
//бонус пешкам за продвижение
int Pawn[] ={
	         0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
		     50,50,50,50,50,50,50,50,
		     10,10,10,30,30,10,10,10,
		     5 ,5 ,5 ,30,30 ,5 ,5 ,5 ,
		     0 ,0 ,10,30,30,0 ,0 ,0 ,
		     0 ,0 ,0 ,5 ,5 ,0 ,0 ,0 ,
		     0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
		     0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
		     };

 // рокировка дело хорошее 
 int Castling[]={
	         0,0,50,0,0,0,50,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,50,0,0,0,50,0
		     };
// кони должны стремиться к центру
// 100 это бонус за существование коней сделанно что бы конь не менялся на 3 пешки
static int Pozicion_k_e[] ={
				 100,100,100,100,100,100,100,100,
 		         100,105,105,105,105,105,105,100,
 		         100,105,110,110,110,110,105,100,
 		         100,105,110,115,115,110,105,100,
 		         100,105,110,115,115,110,105,100,
 		         100,105,110,110,110,110,105,100,
 		         100,105,105,105,105,105,105,100,
 		         100,100,100,100,100,100,100,100,
 		         };

//бонус пешкам за продвижение
int Pawn_e[] ={
	         0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		     100,100,100,100,100,100,100,100,
		     50 ,50 ,50 ,50 ,50 ,50 ,50 ,50 ,
		     20 ,20 ,20 ,20 ,20 ,20 ,20 ,20 ,
		     10 ,10 ,10 ,10 ,10 ,10 ,10 ,10 ,
		     5  ,5  ,5  ,5  ,5  ,5  ,5  ,5  ,
		     0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		     0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		     };


//	===================================================================
// ищем первый установленный бит(т.е. бежим справа)
static __inline int first_one(unsigned __int64 analyzed_number){ 
   _asm { 
/*
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
}//static __inline int first_one(unsigned __int64 analyzed_number){ 
//	*******************************************************************

//	===================================================================
// ищем последний установленный бит(т.е. бежим слева)
static __inline int last_one(unsigned __int64 analyzed_number){ 
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
}//static __inline int last_one(unsigned __int64 analyzed_number){ 
//	*******************************************************************

//	===================================================================
// функция получает позицию и оценивает ее статически смотрим материал и только
int Eval_forced(const T_Bitboard * bitboard ){

unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
static  int ocenka_f=0;
ocenka_f=0;

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения белого короля
  from_mask = bitboard->white_piece[6];
  if(from_mask != 0) ocenka_f= ocenka_f + 90000;

// ходы ферзя-----------------------------------------------------------------
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 1000;
  }//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 500;
  }//while(from_mask != 0){

// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 300;
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 

// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f + 300;
 }// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f=ocenka_f + 100;
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------

///////////////////
///////////////////////////////////
/////////////////


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения короля
  from_mask = bitboard->black_piece[6];
  if(from_mask != 0) ocenka_f= ocenka_f - 90000;

// ходы ферзя-----------------------------------------------------------------
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 1000;
  }//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 500;
  }//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 300;
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 

// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f= ocenka_f - 300;
 }// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask-1);
    ocenka_f=ocenka_f - 100;
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------

//const  int kod[]={0,100,300,300,500,1000,90000};
//
//int ocenka_p=0;
//
//for(int i=0;i<64;i++) {
//	ocenka_p = ocenka_p + kod[bitboard->white_name_from_coord[i]];
//    ocenka_p = ocenka_p - kod[bitboard->black_name_from_coord[i]];
//}//for(int i=0 ;i<64;i++) {
//
//if (ocenka_p != ocenka_f) cout << "KARAUUUUUUUUUUUUUUL"<< "\n";


   return ocenka_f;
}//int Eval_forced_816(T_board_list_816 * board_list_816){
//	*******************************************************************






//	===================================================================
// функция получает позицию и оценивает ее статически смотрим позиционные факторы
int Eval_State(const T_Bitboard * bitboard){

 // нужен для определения сдвоенных пешек 
  int Structure_pawn_w[]={
		     0,0,0,0,0,0,0,0
            };
  int Structure_pawn_b[]={
	         0,0,0,0,0,0,0,0
            };

unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
static  int ocenka_p=0;
static  int from = 0; // координата поля откуда фигура ходит
static  int from_white_king = 0;
static  int from_black_king = 0;
static  int dx;
static  int dy;
static  int distanse;
static  int stadia_w=0;
static  int stadia_b=0;

const  int kd = 7;// коофиц. дистанции
const  int rubikon=1500;//рубеж перехода в эндшпиль
// массив перекодировок материала
const  int kod[]={0,0,300,300,500,1000,0};

stadia_w=0;
stadia_b=0;
ocenka_p=0;

for(int i=0;i<64;i++) {
	stadia_w = stadia_w + kod[bitboard->white_name_from_coord[i]];
    stadia_b = stadia_b + kod[bitboard->black_name_from_coord[i]];
}//for(int i=0 ;i<64;i++) {

/*
 имя фигуры
 0 - нет фигуры 1 - пешка 2 - конь 3 - слон 4 - ладья 5 - ферзь 6 - король

в начальной позиции мы имеем 600(2*300) + 620(2*310) + 1000(2*500) + 1000 = 3220

*/



//cout << "короли " << "\n";
//cout << "bitboard->white_piece[6]= "<< first_one(bitboard->white_piece[6]) << "\n";
//cout << "bitboard->black_piece[6]= "<< first_one(bitboard->black_piece[6]) << "\n";
//cout << "ладьи " << "\n";
//cout << "белые  32 8" << "\n";
//cout << "bitboard->white_piece[4]= 32  "<< first_one(32) << "\n";
//cout << "bitboard->white_piece[4]= 8   "<< first_one(8) << "\n";
//
//cout << "bitboard->white_piece[4]= "<< first_one(bitboard->white_piece[4]) << "\n";
//
//cout << "черные  32 8" << "\n";
//cout << "bitboard->black_piece[4]= 2305843009213693952  "<< first_one(2305843009213693952) << "\n";
//cout << "bitboard->black_piece[4]= 576460752303423488   "<< first_one(576460752303423488) << "\n";
//
//cout << "bitboard->black_piece[4]= "<< first_one(bitboard->black_piece[4]) << "\n";

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения белого короля
  from_mask = bitboard->white_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
      from_white_king = from;
//	  ocenka_p= ocenka_p + Castling[from]- King_shield(from,1,bitboard);
  if(stadia_b > rubikon){
// король на поле рокировки 0-0
	  if (from==6){
// ладья на поле рокировки
         if ((bitboard->white_piece[4]& 32) == 32){
	        ocenka_p= ocenka_p + 50 ;//- King_shield(from,1,bitboard);
         }else{
            ocenka_p= ocenka_p ;//- King_shield(from,1,bitboard);
		 }//if (bitboard->white_piece[4]==32){
// король на поле рокировки 0-0-0
	  }else if (from==2){//if (from==6){
// ладья на поле рокировки
         if ((bitboard->white_piece[4]& 8) == 8){
	        ocenka_p= ocenka_p + 50 ;//- King_shield(from,1,bitboard);
         }else{
            ocenka_p= ocenka_p ;//- King_shield(from,1,bitboard);
		 }//if (bitboard->white_piece[6]==8){
       }else{
         ocenka_p= ocenka_p ;//- King_shield(from,1,bitboard);
	  }//}else if (from==2){//if (from==6){
  }else{
        ocenka_p= ocenka_p + Pozicion_k_e[from];
  }// if(stadia_b > rubikon){


  }//if(from_mask != 0) {



// маска расположения черного короля
  from_mask = bitboard->black_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
      from_black_king = from;
//	  ocenka_p= ocenka_p - Castling[from] + King_shield(from,0,bitboard);
//	  ocenka_p= ocenka_p - Castling[from] + King_shield(from,0,bitboard);
  //	  ocenka_p= ocenka_p + Castling[from];
 if(stadia_w > rubikon){
// король на поле рокировки 0-0
	  if (from==62){
// ладья на поле рокировки
         if ((bitboard->black_piece[4]& 2305843009213693952) == 2305843009213693952){
	        ocenka_p= ocenka_p - 50 + King_shield(from,0,bitboard);
         }else{
            ocenka_p= ocenka_p + King_shield(from,0,bitboard);
		 }//if (bitboard->white_piece[4]==32){
// король на поле рокировки 0-0-0
	  }else if (from==58){//if (from==6){
// ладья на поле рокировки
         if ((bitboard->black_piece[4] & 576460752303423488)==576460752303423488){
	        ocenka_p= ocenka_p - 50 + King_shield(from,0,bitboard);
         }else{
            ocenka_p= ocenka_p + King_shield(from,0,bitboard);
		 }//if (bitboard->white_piece[6]==8){
       }else{
         ocenka_p= ocenka_p + King_shield(from,0,bitboard);
	  }//}else if (from==2){//if (from==6){
}else{
        ocenka_p= ocenka_p - Pozicion_k_e[from];
 }// if(stadia_b > rubikon){

  }//if(from_mask != 0) {

// ходы ферзя-----------------------------------------------------------------
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);
dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
	ocenka_p= ocenka_p + Pozicion_q[from] + distanse;
  }//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p + Pozicion_q[from];
dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p + Pozicion_q[from] + Rook_amoun_white(bitboard) + distanse;
  }//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p + Pozicion_b[from];
    ocenka_p= ocenka_p + Pozicion_b[from] + Bishop_amoun_white(bitboard);
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 

// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p + Pozicion_n[from] + distanse;
 }// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
// ocenka_p= ocenka_p + Pawn[63 - from];
 if(stadia_b > rubikon){
    ocenka_p= ocenka_p + Pawn[63 - from];
 }else{
    ocenka_p= ocenka_p + Pawn_e[63 - from];;
  }// if(stadia_b > rubikon){
// если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
	Structure_pawn_w[from % 8]=Structure_pawn_w[from % 8] + 1;
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------

///////////////////
///////////////////////////////////
/////////////////


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ходы ферзя-----------------------------------------------------------------
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);
dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p - Pozicion_q[from] - distanse;
  }//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p - Pozicion_q[from];
dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p - Pozicion_q[from] - Rook_amoun_black(bitboard) - distanse;
  }//while(from_mask != 0){

// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//    ocenka_p= ocenka_p - Pozicion_b[from];
   ocenka_p= ocenka_p - Pozicion_b[from] - Bishop_amoun_black(bitboard);
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 

// ходы коня ----------------------------------------------------------------------------------
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
distanse = kd/(dx + dy);
//distanse = 0;
    ocenka_p= ocenka_p - Pozicion_n[from] - distanse;
 }// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
//  ocenka_p= ocenka_p - Pawn[from];
  if(stadia_w > rubikon){
    ocenka_p= ocenka_p - Pawn[from];
  }else{
    ocenka_p= ocenka_p - Pawn_e[from];
  }//if(stadia_w > rubikon){

// если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
	Structure_pawn_b[from % 8]=Structure_pawn_b[from % 8] + 1;
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------
// штрафуем за сдвоенные пешки
  for(int ii=0;ii<8;ii++){
    if(Structure_pawn_w[ii]>1)ocenka_p=ocenka_p - 20;
    if(Structure_pawn_b[ii]>1)ocenka_p=ocenka_p + 20;
  }//for(int ii=1;ii<9;ii++){

   return ocenka_p;
}//int Eval_State(T_Bitboard * bitboard){

//	*******************************************************************


//	===================================================================
// штраф за разрушение щита перед королем
inline int King_shield(int n,int z,const T_Bitboard * bitboard){
static int st=0;
st=0;
	if(z==1){
// идем на клетку вверх
       if(bitboard->white_name_from_coord[n + 7]==0) st=st+20;
// идем на клетку влево вверх
       if(bitboard->white_name_from_coord[n + 8]==0) st=st+20;
// идем на клетку вправо вверх
       if(bitboard->white_name_from_coord[n + 9]==0) st=st+20;
	}else{
// идем на клетку вниз
       if(bitboard->black_name_from_coord[n - 7]==0) st=st+20;
// идем на клетку  влево вниз
       if(bitboard->black_name_from_coord[n - 8]==0) st=st+20;
// идем на клетку вправо вниз
       if(bitboard->black_name_from_coord[n - 9]==0) st=st+20;
	}//if(z==1){

return st;

}//int King_shield(int n,int z,T_board_list_816 * board_list_816){
//	*******************************************************************

//	===================================================================
// просчитываем все возможные ходы ладьи возвращаем количество ходов
inline int Rook_amoun_white(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 blocking_square_mask=0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
//unsigned __int64 captures_mask = 0; // маска взятий


int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int blocking_coordinate = 0; // координата блокированного поля 
int number = 0; // координата поля откуда фигура ходит
// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & e_rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
 blocking_square_mask = (bitboard->all_piece) & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & e_rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------
  return number;
}//int Rook_amoun_white(T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// просчитываем все возможные ходы ладьи возвращаем количество ходов
inline int Rook_amoun_black(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 blocking_square_mask=0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
//unsigned __int64 captures_mask = 0; // маска взятий


int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int blocking_coordinate = 0; // координата блокированного поля 
int number = 0; // координата поля откуда фигура ходит
// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
 blocking_square_mask = (bitboard->all_piece) & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & e_rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
   blocking_square_mask = (bitboard->all_piece) & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & e_rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------
  return number;
}//int Rook_amoun_black(T_Bitboard * bitboard){
//	*******************************************************************


//	===================================================================
// просчитываем все возможные ходы слона возвращаем количество ходов
inline int Bishop_amoun_white(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 blocking_square_mask=0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
//unsigned __int64 captures_mask = 0; // маска взятий


int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int blocking_coordinate = 0; // координата блокированного поля 
int number = 0; // координата поля откуда фигура ходит
// ходы слона---------------------------------------------------------------------------------- 
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - вправо
 blocking_square_mask = (bitboard->all_piece) & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
   blocking_square_mask = (bitboard->all_piece) & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_black_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 
  return number;
}//int Bishop_amoun_white(T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// просчитываем все возможные ходы слона возвращаем количество ходов
inline int Bishop_amoun_black(const T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 blocking_square_mask=0; // маска блокированных полей
unsigned __int64 move_mask = 0; // маска ходов
//unsigned __int64 captures_mask = 0; // маска взятий


int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int blocking_coordinate = 0; // координата блокированного поля 
int number = 0; // координата поля откуда фигура ходит
// ходы слона---------------------------------------------------------------------------------- 
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// луч вверх - вправо
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
 blocking_square_mask = (bitboard->all_piece) & e_bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
   //captures_mask = bitboard->all_white_piece & e_move_masks[blocking_coordinate];
   //move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & e_move_masks[blocking_coordinate]));
 }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask ^ e_move_masks[to];
     number = number + 1;
   }//while (move !=0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
// слона---------------------------------------------------------------------------------- 
  return number;
}//int Bishop_amoun_black(T_Bitboard * bitboard){
//	*******************************************************************