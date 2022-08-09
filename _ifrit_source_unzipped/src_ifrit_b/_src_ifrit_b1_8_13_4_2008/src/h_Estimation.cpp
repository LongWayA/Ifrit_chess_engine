#include "h_Estimation.h"
#include "k_Util.h"
//#include "d_Bitboard_gen.h"
//-----------------------------------------------------------------------------------
/* +
 * оценка позиции
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 12.04.2008
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
 * все что здесь проходит можно проверить в модуле d_Loop
*/
//---------------------------------------------------------------------------

// фигуры должны стремиться к центру
int Pozicion[] ={
				 100,100,100,100,100,100,100,100,
 		         100,105,105,100,100,105,105,100,
 		         100,105,115,115,115,115,105,100,
 		         100,105,115,110,110,115,105,100,
 		         100,105,115,110,110,115,105,100,
 		         100,105,115,115,115,115,105,100,
 		         100,105,105,100,100,105,105,100,
 		         100,100,100,100,100,100,100,100,
 		         };
// добавляю бонус в пешку за существование фигуры 
// цель не допустить необоснованных разменов материала
int Pozicion_r[] ={
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
	         0,0,0,0,0,0,0,0,
		     50,50,50,50,50,50,50,50,
		     40,40,40,40,40,40,40,40,
		     10,10,10,15,15,10,10,10,
		     5,5,30,50,50,5,5,5,
		     0,0,0,30,30,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0
		     };

 // рокировка дело хорошее 
 // приоритет короткой рокировке
 int Castling[]={
	         0,0,30,0,0,0,50,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,0,0,0,0,0,0,
		     0,0,30,0,0,0,50,0
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
  if(from_mask != 0) ocenka_f= ocenka_f + 900000;

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
  if(from_mask != 0) ocenka_f= ocenka_f - 900000;

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
int from = 0; // координата поля откуда фигура ходит
ocenka_p=0;

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения белого короля
  from_mask = bitboard->white_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
	  ocenka_p= ocenka_p + Castling[from] - King_shield(from,1,bitboard);
//	  ocenka_p= ocenka_p + Castling[from];
  }

// ходы ферзя-----------------------------------------------------------------
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pozicion_r[from];
  }//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pozicion_r[from] + 2*Rook_amoun_white(bitboard);
//    ocenka_p= ocenka_p + Pozicion_r[from];
  }//while(from_mask != 0){
// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pozicion[from] + 2*Bishop_amoun_white(bitboard);
//    ocenka_p= ocenka_p + Pozicion[from];
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
    ocenka_p= ocenka_p + Pozicion[from];
 }// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p + Pawn[63 - from];
// если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
	Structure_pawn_w[from % 8]=Structure_pawn_w[from % 8] + 1;
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------

///////////////////
///////////////////////////////////
/////////////////


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения короля
  from_mask = bitboard->black_piece[6];
  if(from_mask != 0) {
      from = first_one(from_mask);
	  ocenka_p= ocenka_p - Castling[from] - King_shield(from,0,bitboard);
//	  ocenka_p= ocenka_p - Castling[from];
  }

// ходы ферзя-----------------------------------------------------------------
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pozicion_r[from];
  }//while(from_mask != 0){
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pozicion_r[from] - 2*Rook_amoun_black(bitboard);
//    ocenka_p= ocenka_p - Pozicion_r[from];
  }//while(from_mask != 0){

// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pozicion[from] - 2*Bishop_amoun_black(bitboard);
//    ocenka_p= ocenka_p - Pozicion[from];
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
    ocenka_p= ocenka_p - Pozicion[from];
 }// while(from_mask != 0){
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    ocenka_p= ocenka_p - Pawn[from];
// если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
	Structure_pawn_b[from % 8]=Structure_pawn_b[from % 8] + 1;
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------
// штрафуем за сдвоенные пешки
  for(int ii=0;ii<8;ii++){
    if(Structure_pawn_w[ii]>1)ocenka_p=ocenka_p-20*Structure_pawn_w[ii];
    if(Structure_pawn_b[ii]>1)ocenka_p=ocenka_p+20*Structure_pawn_b[ii];
  }//for(int ii=1;ii<9;ii++){

   return ocenka_p;
}//int Eval_State(T_Bitboard * bitboard){

//	*******************************************************************

//	===================================================================
// штраф за разрушение щита перед королем
inline int King_shield(int n,int z,const T_Bitboard * bitboard){
 int st=0;
st=0;
	if(z==1){
// идем на клетку вверх
       if(bitboard->white_name_from_coord[n + 7]==0) st=st+30;
// идем на клетку влево вверх
       if(bitboard->white_name_from_coord[n + 8]==0) st=st+30;
// идем на клетку вправо вверх
       if(bitboard->white_name_from_coord[n + 9]==0) st=st+30;
	}else{
// идем на клетку вниз
       if(bitboard->black_name_from_coord[n - 7]==0) st=st-30;
// идем на клетку  влево вниз
       if(bitboard->black_name_from_coord[n - 8]==0) st=st-30;
// идем на клетку вправо вниз
       if(bitboard->black_name_from_coord[n - 9]==0) st=st-30;
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