#include <fstream.h>  // TEST
#include <stdlib.h>


#include "d_Bitboard_gen.h"
#include "k_Util.h"
#include "d_Bitboard_const.h"
//-----------------------------------------------------------------------------------
/*+
 * реализуем битбоард генератор
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.05.2008
 *
 *  ТЕСТИРОВАНИЕ
 *  тестируем в полном переборе на специальных тестовых позициях
 *  на данный момент(14.03.08) багов не замеченно
*/
//---------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_B
 ofstream Test_Bitboard;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*
логика
             и &  0 0 1 первый операнд
                  0 1 1 второй операнд
                  -----
                  0 0 1 результат

           или |  0 0 1
                  0 1 1
                  -----
                  0 1 1

   исключающее ^  0 0 1
   или            0 1 1
                  -----
                  0 1 0

*/

//	===================================================================
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для белых фигур генератора взятий
inline void KONTROL_captures_W(const struct T_Bitboard * bitboard,const int * from,const int * to,const int * p,int name_figure){
#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p<0)||(*p>252595))   cout << "ERROR Generator_captures p= "   << *p    << "\n";
if ((*from<0)||(*from>63)) cout << "ERROR Generator_captures from= "<< *from << "\n";
if ((*to<0)||(*to>63))     cout << "ERROR Generator_captures to= "  << *to   << "\n";
if ((bitboard->black_name_from_coord[*to] <1)||(bitboard->black_name_from_coord[*to]>6)){
	cout << "ERROR Generator_captures bitboard->black_name_from_coord[*to]= "  << bitboard->black_name_from_coord[*to]   << "\n";
    cout << "ERROR Generator_captures from= "<< *from << "\n";
    cout << "ERROR Generator_captures to= "  << *to   << "\n";
    cout << "ERROR Generator_captures name_figure= "  << name_figure << "\n";
    Bitboard_print(bitboard);
}//if ((bitboard->black_name_from_coord[*to] <1)||(bitboard->black_name_from_coord[*to]>6)){
//if (bitboard->black_name_from_coord[*to] ==6) cout << "ERROR Generator_captures name_figure KING "  << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

//	===================================================================
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для черных фигур генератора взятий
inline void KONTROL_captures_B(const struct T_Bitboard * bitboard,const int * from,const int * to,const int * p,int name_figure){
#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p<0)||(*p>252595))   cout << "ERROR Generator_captures p= "   << *p    << "\n";
if ((*from<0)||(*from>63)) cout << "ERROR Generator_captures from= "<< *from << "\n";
if ((*to<0)||(*to>63))     cout << "ERROR Generator_captures to= "  << *to   << "\n";
if ((bitboard->white_name_from_coord[*to] <1)||(bitboard->white_name_from_coord[*to]>6)){
	cout << "ERROR Generator_captures bitboard->white_name_from_coord[*to]= "  << bitboard->white_name_from_coord[*to]   << "\n";
    cout << "ERROR Generator_captures from= "<< *from << "\n";
    cout << "ERROR Generator_captures to= "  << *to   << "\n";
    cout << "ERROR Generator_captures name_figure= "  << name_figure << "\n";
    Bitboard_print(bitboard);
}//if ((bitboard->white_name_from_coord[*to] <1)||(bitboard->white_name_from_coord[*to]>6)){
//if (bitboard->white_name_from_coord[*to] ==6) cout << "ERROR Generator_captures name_figure KING "  << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

//	===================================================================
// контролируем выход за пределы доски, за пределы списка 
// для генератора ходов 
inline void KONTROL_move(const int * from,const int * to,const int * p){
#if KONTROL_B
///////////////////////////////////////////////////////////////////////////////////
if ((*p<0)||(*p>252595))   cout << "ERROR Generator_move p= "   << *p    << "\n";
if ((*from<0)||(*from>63)) cout << "ERROR Generator_move from= "<< *from << "\n";
if ((*to<0)||(*to>63))     cout << "ERROR Generator_move to= "  << *to   << "\n";
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}//void KONTROL(){
//	*******************************************************************

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
// заполняем список ходов - взятия за белых
// две функции только из за разного контроля списков(строки взятия)
static inline void list_record_w(int name,int d_m,int & p,int & from,unsigned __int64 & captures_mask,const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
int to; // координата поля куда фигура ходит
// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  while (captures_mask !=0){
     to = first_one(captures_mask);
     captures_mask = captures_mask & (captures_mask-1);
#if KONTROL_B
KONTROL_captures_W(bitboard,&from,&to,&p,name);
#endif//KONTROL_B

// 6 бит 
// записали имя взятой фигуры (31 - пешка 32 - конь 33 - слон 34 - ладья 35 - ферзь 36 - король)
list_surplus_moves->move[p] = bitboard->black_name_from_coord[to]

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (d_m << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "bitboard->black_name_from_coord[to]= "<< bitboard->black_name_from_coord[to] << "\n";
//cout << "bitboard->black_name_from_coord[to]= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------
  //   list_surplus_moves->name_figure[p]=name;
	 //list_surplus_moves->initial_position[p] = from;
	 //list_surplus_moves->final_position[p] = to;
	 //list_surplus_moves->description_move[p] = d_m;
  //   list_surplus_moves->name_taking_figure[p] = bitboard->black_name_from_coord[to];
     p=p+1;
   }//while (captures !=0){
}//
//	*******************************************************************

//	===================================================================
// заполняем список ходов - взятия за черных
// две функции только из за разного контроля списков(строки взятия)
static inline void list_record_b(int name,int d_m,int & p,int & from,unsigned __int64 & captures_mask,const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
int to; // координата поля куда фигура ходит
// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  while (captures_mask !=0){
     to = first_one(captures_mask);
     captures_mask = captures_mask & (captures_mask-1);
#if KONTROL_B
KONTROL_captures_B(bitboard,&from,&to,&p,name);
#endif//KONTROL_B


// 6 бит 
// записали имя взятой фигуры (31 - пешка 32 - конь 33 - слон 34 - ладья 35 - ферзь 36 - король)
list_surplus_moves->move[p] = bitboard->white_name_from_coord[to]

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (d_m << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "bitboard->white_name_from_coord[to]= "<< bitboard->white_name_from_coord[to] << "\n";
//cout << "bitboard->white_name_from_coord[to]= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------


  //   list_surplus_moves->name_figure[p]=name;
	 //list_surplus_moves->initial_position[p] = from;
	 //list_surplus_moves->final_position[p] = to;
	 //list_surplus_moves->description_move[p] = d_m;
  //   list_surplus_moves->name_taking_figure[p] = bitboard->white_name_from_coord[to];
     p=p+1;
   }//while (captures !=0){
}//
//	*******************************************************************

//	===================================================================
// заполняем список ходов
static inline void list_record_m(int name,int d_m,int & p,int & from,unsigned __int64 & move_mask,const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
int to; // координата поля куда фигура ходит
// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   while (move_mask !=0){
     to = first_one(move_mask);
     move_mask = move_mask & (move_mask-1);
#if KONTROL_B
 KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// 6 бит 
// взятая фигура 0
list_surplus_moves->move[p] = 0

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (d_m << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "0= "<< 0 << "\n";
//cout << "0= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------

     p=p+1;
   }//while (move !=0){
}//
//	*******************************************************************


//	===================================================================
// заполняем список ходов для особых случаев типа рокировки взятия на проходе и превращений пешек
static inline void list_record_os(int name,int d_m,int & p,int & from,int to,int capture,struct T_list_surplus_moves * list_surplus_moves){
// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
// 6 бит 
// взятая фигура 0
list_surplus_moves->move[p] = capture

// 6 бит 
// записали тип хода (0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка
//4 – короткая рокировка 5 – взятие на проходе
//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
//25 - взятие с превращением в ферзь )
 | (d_m << 6)

// 6 бит 
// записали откуда ходим (0 -63)
 | (from << 12)

// 6 бит 
// записали куда ходим (0 -63)
 | (to << 18)

// 3 бит 
// ходящая фигура (0 - 6)
 | (name << 24);

//-------------------------------------------------------------------------------
//cout << "list_surplus_moves->move[p]= "<< list_surplus_moves->move[p] << "\n";
//
//cout << "0= "<< 0 << "\n";
//cout << "0= "<< ( list_surplus_moves->move[p] & 63) << "\n";
//
//cout << "d_m= "<< d_m << "\n";
//cout << "d_m= "<< ((list_surplus_moves->move[p]>>6)& 63)  << "\n";
//
//cout << "from= "<< from << "\n";
//cout << "from= "<< ((list_surplus_moves->move[p]>>12)& 63)  << "\n";
//
//cout << "to= "<< to << "\n";
//cout << "to= "<< ((list_surplus_moves->move[p]>>18)& 63)  << "\n";
//
//cout << "name= "<< name << "\n";
//cout << "name= "<<  ((list_surplus_moves->move[p]>>24)& 7)  << "\n";

//-------------------------------------------------------------------------------

  //   list_surplus_moves->name_figure[p]=name;
	 //list_surplus_moves->initial_position[p] = from;
	 //list_surplus_moves->final_position[p] = to;
	 //list_surplus_moves->description_move[p] =d_m; 
  //   list_surplus_moves->name_taking_figure[p] = capture;
     p=p+1;
}//
//	*******************************************************************




//	===================================================================
// генерируем список взятий и превращений пешки
void Generator_captures_white(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

unsigned __int64 move_mask_d = 0; // маска хода пешки на два поля
unsigned __int64 one_pawn_mask = 0; // маска расположения одной пешки
unsigned __int64 move_conversion_mask = 0; // маска ходов пешки с превращеинем
unsigned __int64 captures_conversion_mask = 0; // маска взятий пешки с превращениями

// константы
const unsigned __int64 LINE_0 = 255; // 1 горизонталь доски

const unsigned __int64 LINE_7 = 18374686479671623680; // 8 горизонталь доски

const unsigned __int64 COLUMN_0 = 72340172838076673; // 1 вертикаль доски
const unsigned __int64 COLUMN_7 = 9259542123273814144; // 8 вертикаль доски


int blocking_coordinate = 0; // координата блокированного поля 
int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int p =0; // количество фигур в списке 

// генерируем ходы белых фигур
// ходы короля--------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения белого короля
  from_mask = bitboard->white_piece[6];
 // находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
  from = first_one(from_mask);
// находим маску взятий короля
  captures_mask = (bitboard->all_black_piece & king_moves_masks[from]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(6,2,p,from,captures_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
}
// ходы короля--------------------------------------------------------------------------------

// ходы ферзя----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения белого ферзя
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) находим блокированные биты.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];

 if(blocking_square_mask !=0){ 
// 2) находим координату первого блокированного бита
   blocking_coordinate =  first_one(blocking_square_mask);
// 4) находим маску взятий
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч вниз  
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч вверх - влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){

}
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
   blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// слона---------------------------------------------------------------------------------- 

// ходы коня ----------------------------------------------------------------------------------
{// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    captures_mask = (bitboard->all_black_piece & knigt_moves_masks[from]);

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(2,2,p,from,captures_mask,bitboard,list_surplus_moves);

 }// while(from_mask != 0){
}
// коня ----------------------------------------------------------------------------------

// ходы пешки----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
 // выделяем одну пешку 
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 находим простые ходы белых пешек на одно поле вперед
  move_mask = (one_pawn_mask << 8) & (~(bitboard->all_piece));

// ходы с превращением
  move_conversion_mask = move_mask & LINE_7;

// 3 находим взятия пешек
// вправо ( слева надо отсечь т.е. вертикаль с а1) влево  ( справа надо отсечь т.е. вертикаль с h1)
 captures_mask = ((one_pawn_mask << 9) & ~COLUMN_0) | ((one_pawn_mask << 7) & ~COLUMN_7);
 captures_mask = captures_mask &  bitboard->all_black_piece;

// взятия с превращением
captures_conversion_mask = captures_mask & LINE_7;

// просто взятия
captures_mask = captures_mask & ~LINE_7;

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_w(1,2,p,from,captures_mask,bitboard,list_surplus_moves);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// находим координаты ходов с превращением пешки
  while (move_conversion_mask !=0){
    to = first_one(move_conversion_mask);
    move_conversion_mask = move_conversion_mask & (move_conversion_mask-1);

// отмечаем превращение пешки
// превращение в ладью 14,превращение в коня 12,превращение в слона 13,превращение в ферзь 15,
list_record_os(1,15,p,from,to,0,list_surplus_moves);

list_record_os(1,14,p,from,to,0,list_surplus_moves);

list_record_os(1,13,p,from,to,0,list_surplus_moves);

list_record_os(1,12,p,from,to,0,list_surplus_moves);
 
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
   }//while (captures !=0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// находим координаты взятий с превращением пешки  
  while (captures_conversion_mask !=0){
    to = first_one(captures_conversion_mask);
    captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask-1);

// отмечаем превращение пешки с взятием
// превращение с взятием в ферзь
list_record_os(1,25,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);

// превращение с взятием в ладью
list_record_os(1,24,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);
 
// превращение с взятием в слона
list_record_os(1,23,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);

// превращение с взятием в коня
list_record_os(1,22,p,from,to,bitboard->black_name_from_coord[to],list_surplus_moves);

#if KONTROL_B
 KONTROL_captures_W(bitboard,&from,&to,&p,120);
#endif//KONTROL_B
   }//while (captures !=0){

// 4 находим взятия на проходе
 if(bitboard->taking_passage_yes == 1){
// взятие на проходе
   to = 8 * bitboard->taking_passage_y + bitboard->taking_passage_x;

// кажется что проверка ряда излишня потом надо будет вернуться
   if ( ((from - to) == -9) && (from % 8 != 7) ){
list_record_os(1,5,p,from,to,bitboard->black_name_from_coord[to - 8],list_surplus_moves);

#if KONTROL_B
to = to - 8;
KONTROL_captures_W(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ((from - to) == 9) && (from % 8 != 0)){

   if ( ((from - to) == -7) && (from % 8 != 0) ){
list_record_os(1,5,p,from,to,bitboard->black_name_from_coord[to - 8],list_surplus_moves);

#if KONTROL_B
to = to - 8;
KONTROL_captures_W(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ( ( (from - to) == 9) | ((from - to) == 7)){

  }//if(bitboard->taking_passage_yes == 1){

}//while(from_mask != 0){

}
// пешки----------------------------------------------------------------------------------

list_surplus_moves->end_list = p;
list_surplus_moves->end_captures = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// генерируем список взятий и превращений пешки
void Generator_captures_black(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

unsigned __int64 move_mask_d = 0; // маска хода пешки на два поля
unsigned __int64 one_pawn_mask = 0; // маска расположения одной пешки
unsigned __int64 move_conversion_mask = 0; // маска ходов пешки с превращеинем
unsigned __int64 captures_conversion_mask = 0; // маска взятий пешки с превращениями

// константы
const unsigned __int64 LINE_0 = 255; // 1 горизонталь доски

const unsigned __int64 LINE_7 = 18374686479671623680; // 8 горизонталь доски

const unsigned __int64 COLUMN_0 = 72340172838076673; // 1 вертикаль доски
const unsigned __int64 COLUMN_7 = 9259542123273814144; // 8 вертикаль доски


int blocking_coordinate = 0; // координата блокированного поля 
int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int p =0; // количество фигур в списке 

// генерируем ходы черных фигур
// ходы короля---------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[6];
  from = first_one(from_mask);
  captures_mask = (bitboard->all_white_piece & king_moves_masks[from]);

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(6,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}
// короля---------------------------------------------------------------------------------

// ходы ферзя----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
  from = first_one(from_mask);
  from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(5,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// ферзя----------------------------------------------------------------------------------

// ходы ладьи----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
   blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(4,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// ладьи----------------------------------------------------------------------------------

// ходы слона---------------------------------------------------------------------------------- 
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// луч вверх - вправо
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(3,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// слона---------------------------------------------------------------------------------- 

// ходы коня ----------------------------------------------------------------------------------
{
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    captures_mask = (bitboard->all_white_piece & knigt_moves_masks[from]);

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(2,2,p,from,captures_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }// while(from_mask != 0){
//-------------------------------------------------------------------------------------
}
// коня ----------------------------------------------------------------------------------

// ходы пешки----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 находим простые ходы черных пешек на одно поле вперед
  move_mask = (one_pawn_mask >> 8) & (~(bitboard->all_piece));

// ходы с превращением
  move_conversion_mask = move_mask & LINE_0;

// 3 находим взятия пешек
// вправо ( слева надо отсечь т.е. вертикаль с а1) влево  ( справа надо отсечь т.е. вертикаль с h1)
 captures_mask = ((one_pawn_mask >> 7) & ~COLUMN_0) | ((one_pawn_mask >> 9) & ~COLUMN_7 );
 captures_mask = captures_mask &  bitboard->all_white_piece;

// взятия с превращением
captures_conversion_mask = captures_mask & LINE_0;

// просто взятия
captures_mask = captures_mask & ~LINE_0;

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_b(1,2,p,from,captures_mask,bitboard,list_surplus_moves);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// находим координаты ходов с превращением пешки
  while (move_conversion_mask !=0){
    to = first_one(move_conversion_mask);
    move_conversion_mask = move_conversion_mask & (move_conversion_mask-1);

// отмечаем превращение пешки
// превращение в ладью 14,превращение в коня 12,превращение в слона 13,превращение в ферзь 15,
list_record_os(1,15,p,from,to,0,list_surplus_moves);

list_record_os(1,14,p,from,to,0,list_surplus_moves);

list_record_os(1,13,p,from,to,0,list_surplus_moves);
 
list_record_os(1,12,p,from,to,0,list_surplus_moves);

#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
   }//while (captures !=0){

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// находим координаты взятий с превращением пешки  
  while (captures_conversion_mask !=0){
    to = first_one(captures_conversion_mask);
    captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask-1);

// отмечаем превращение пешки с взятием
// превращение с взятием в коня
list_record_os(1,25,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

// превращение с взятием в слона
list_record_os(1,24,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

// превращение с взятием в ладью
list_record_os(1,23,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

// превращение с взятием в ферзь
list_record_os(1,22,p,from,to,bitboard->white_name_from_coord[to],list_surplus_moves);

#if KONTROL_B
KONTROL_captures_B(bitboard,&from,&to,&p,120);
#endif//KONTROL_B
   }//while (captures !=0){

// 4 находим взятия на проходе
 if(bitboard->taking_passage_yes == 1){
// взятие на проходе
    to = 8 * bitboard->taking_passage_y + bitboard->taking_passage_x;

   if ( ((from - to) == 9) && (from % 8 != 0) ){
list_record_os(1,5,p,from,to,bitboard->white_name_from_coord[to + 8],list_surplus_moves);

#if KONTROL_B
 to = to + 8;
 KONTROL_captures_B(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ((from - to) == 9) && (from % 8 != 0)){

   if ( ((from - to) == 7) && (from % 8 != 7) ){
list_record_os(1,5,p,from,to,bitboard->white_name_from_coord[to + 8],list_surplus_moves);

#if KONTROL_B
to = to + 8;
 KONTROL_captures_B(bitboard,&from,&to,&p,130);
#endif//KONTROL_B
   }//if ( ( (from - to) == 9) | ((from - to) == 7)){

  }//if(bitboard->taking_passage_yes == 1){

}//while(from_mask != 0){

}
// пешки----------------------------------------------------------------------------------

list_surplus_moves->end_list = p;
list_surplus_moves->end_captures = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий и превращений пешек)
void Generator_move_white(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

unsigned __int64 move_mask_d = 0; // маска хода пешки на два поля
unsigned __int64 one_pawn_mask = 0; // маска расположения одной пешки
unsigned __int64 move_conversion_mask = 0; // маска ходов пешки с превращеинем
unsigned __int64 captures_conversion_mask = 0; // маска взятий пешки с превращениями

// константы
const unsigned __int64 LINE_0 = 255; // 1 горизонталь доски
const unsigned __int64 LINE_1 = 255<<8; // 2 горизонталь доски

const unsigned __int64 LINE_6 = 71776119061217280; // 7 горизонталь доски
const unsigned __int64 LINE_7 = 18374686479671623680; // 8 горизонталь доски

const unsigned __int64 CASTLING_q = 1008806316530991104; // поля  B8 C8 D8 заняты
const unsigned __int64 CASTLING_k = 6917529027641081856; // поля F8 G8 заняты

const unsigned __int64 CASTLING_Q = 14; // поля  B1 C1 D1 заняты
const unsigned __int64 CASTLING_K = 96; // поля F1 G1 заняты


int blocking_coordinate = 0; // координата блокированного поля 
int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int p ; // количество фигур в списке 

// используем старые списки. после отладки и написания реализатора перейдем на новые
  
 p = list_surplus_moves->end_list;

// генерируем ходы белых фигур
// ходы короля--------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения белого короля
  from_mask = bitboard->white_piece[6];
 // находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
    from = first_one(from_mask);
// находим маску ходов короля(без взятий)
   move_mask = king_moves_masks[from] & (~bitboard->all_piece);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(6,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// рокировки белого короля
// если король на месте то можем дальше рассматривать возможность рокировки
   if (from == 4){
// если длинная рокировка разрешена
	   if(bitboard->castling_Q){
// и поля  1 2 3 пустые  то добавляем ход
to = 2;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
          if(((~bitboard->all_piece) & CASTLING_Q ) == CASTLING_Q ){
// длинная рокировка белых проверяем 0-0-0 2 3 4
			if((White_check_square(2,bitboard)==0)&&(White_check_square(3,bitboard)==0)&&(White_check_square(4,bitboard)==0)){
list_record_os(6,3,p,from,2,0,list_surplus_moves);

			}//if((White_check_square(2,bitboard)==0)&&(White_check_square(3,bitboard)==0)&&(White_check_square(4,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 14)){ 
	   }//if(bitboard->castling_Q){

// если короткая рокировка разрешена то добавляем ход 
	   if(bitboard->castling_K){
to = 6;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// и поля  5 6 пустые  то добавляем ход
          if( ( (~bitboard->all_piece) & CASTLING_K ) == CASTLING_K ){

// короткая рокировка белых проверяем 0-0 4 5 6
	       if((White_check_square(4,bitboard)==0)&&(White_check_square(5,bitboard)==0)&&(White_check_square(6,bitboard)==0)){
 list_record_os(6,4,p,from,6,0,list_surplus_moves);
			   
			}//if((White_check_square(4,bitboard)==0)&&(White_check_square(5,bitboard)==0)&&(White_check_square(6,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 96)){ 
	   }//if(bitboard->castling_K){
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
   }//if (from_mask == 4){
}
// ходы короля--------------------------------------------------------------------------------




// ходы ферзя----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// маска расположения белого ферзя
  from_mask = bitboard->white_piece[5];
  while(from_mask != 0){
    from = first_one(from_mask);
// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях (по типу ладьи)
// луч вверх
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) находим блокированные биты.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];

 if(blocking_square_mask !=0){ 
// 2) находим координату первого блокированного бита
   blocking_coordinate =  first_one(blocking_square_mask);
// 3) находим маску разрешенных ходов (пока вместе с взятием своих фигур)
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
// 4) находим маску взятий
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
// 5) находим маску ходов
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 


// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч вниз  
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях (по типу слона)
// луч вверх - вправо
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч вверх - влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  }//while(from_mask != 0){

}
// ферзя----------------------------------------------------------------------------------



// ходы ладьи----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// ладьи----------------------------------------------------------------------------------



// ходы слона---------------------------------------------------------------------------------- 
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
   blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_white_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// слона---------------------------------------------------------------------------------- 




// ходы коня ----------------------------------------------------------------------------------
{// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    move_mask = knigt_moves_masks[from] & (~bitboard->all_piece);

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(2,1,p,from,move_mask,bitboard,list_surplus_moves);

 }// while(from_mask != 0){
}
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->white_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
 // выделяем одну пешку 
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 находим простые ходы белых пешек на одно поле вперед
  move_mask = (one_pawn_mask << 8) & (~(bitboard->all_piece));
// 2 находим ходы пешек на 2 поля вперед 
// в начале оставляем только пешки стоящие на 2 горизонтали
  move_mask_d = one_pawn_mask & LINE_1;
// отметаем пешки если они чем то заблокированны
  move_mask_d = (move_mask_d <<8) & (~(bitboard->all_piece));
// оставшимися пешками делаем ход
  move_mask_d = (move_mask_d << 8) & (~(bitboard->all_piece));

// здесь все ходы пешки как на одно так на два поля вперед
  move_mask = (move_mask | move_mask_d) ;

// ходы без превращений
  move_mask = move_mask & ~LINE_7;

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(1,1,p,from,move_mask,bitboard,list_surplus_moves);
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------
}

list_surplus_moves->end_list = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// генерируем список остальных ходов (т.е. всех ходов кроме взятий и превращений пешек)
void Generator_move_black(const struct T_Bitboard * bitboard,struct T_list_surplus_moves * list_surplus_moves){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

unsigned __int64 move_mask_d = 0; // маска хода пешки на два поля
unsigned __int64 one_pawn_mask = 0; // маска расположения одной пешки
unsigned __int64 move_conversion_mask = 0; // маска ходов пешки с превращеинем
unsigned __int64 captures_conversion_mask = 0; // маска взятий пешки с превращениями

// константы
const unsigned __int64 LINE_0 = 255; // 1 горизонталь доски
const unsigned __int64 LINE_1 = 255<<8; // 2 горизонталь доски

const unsigned __int64 LINE_6 = 71776119061217280; // 7 горизонталь доски
const unsigned __int64 LINE_7 = 18374686479671623680; // 8 горизонталь доски

const unsigned __int64 CASTLING_q = 1008806316530991104; // поля  B8 C8 D8 заняты
const unsigned __int64 CASTLING_k = 6917529027641081856; // поля F8 G8 заняты

const unsigned __int64 CASTLING_Q = 14; // поля  B1 C1 D1 заняты
const unsigned __int64 CASTLING_K = 96; // поля F1 G1 заняты


int blocking_coordinate = 0; // координата блокированного поля 
int from = 0; // координата поля откуда фигура ходит
int to = 0; // координата поля куда фигура ходит
int p =0; // количество фигур в списке 

p = list_surplus_moves->end_list;

// генерируем ходы черных фигур
// ходы короля---------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[6];
  from = first_one(from_mask);
  move_mask = king_moves_masks[from] & (~(bitboard->all_piece));

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(6,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// рокировки черного короля
// если король на месте то можем дальше рассматривать возможность рокировки
   if (from == 60){
// если длинная рокировка разрешена
	   if(bitboard->castling_q){
to = 58;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// и поля  1 2 3 пустые  то добавляем ход
          if((~(bitboard->all_piece) & CASTLING_q )== CASTLING_q ){
// длинная рокировка черных проверяем 0-0-0 58 59 60
			 if((Black_check_square(58,bitboard)==0)&&(Black_check_square(59,bitboard)==0)&&(Black_check_square(60,bitboard)==0)){
 
list_record_os(6,3,p,from,58,0,list_surplus_moves);
				 
			 }//if(Black_check_square(58,bitboard)==0)&&(Black_check_square(59,bitboard)==0)&&(Black_check_square(60,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 14)){ 
	   }//if(bitboard->castling_Q){
// если короткая рокировка разрешена то добавляем ход 
	   if(bitboard->castling_k){
to = 62;
#if KONTROL_B
KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
// и поля  5 6 пустые  то добавляем ход
          if((~(bitboard->all_piece) & CASTLING_k )==CASTLING_k){
// короткая рокировка черных проверяем 0-0 60 61 62
			if((Black_check_square(60,bitboard)==0)&&(Black_check_square(61,bitboard)==0)&&(Black_check_square(62,bitboard)==0)){
 list_record_os(6,4,p,from,62,0,list_surplus_moves);

			}//if((Black_check_square(60,bitboard)==0)&&(Black_check_square(61,bitboard)==0)&&(Black_check_square(62,bitboard)==0)){
		  }//if(( (bitboard->all_piece) & 96)){ 
	   }//if(bitboard->castling_K){
   
   }//if (from_mask == 60){

}
// короля---------------------------------------------------------------------------------




// ходы ферзя----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[5];
  while(from_mask != 0){
  from = first_one(from_mask);
  from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(5,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// ферзя----------------------------------------------------------------------------------



// ходы ладьи----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[4];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч влево
 blocking_square_mask = (bitboard->all_piece) & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз
   blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_down_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(4,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  }//while(from_mask != 0){
}
// ладьи----------------------------------------------------------------------------------



// ходы слона---------------------------------------------------------------------------------- 
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[3];
  while(from_mask != 0){
    from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

// смотрим в четырех различных направлениях
// луч вверх - вправо
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
   move_mask = move_mask ^ (captures_mask | ( bitboard->all_black_piece & move_masks[blocking_coordinate]));
 }else{
   move_mask = bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){ 

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(3,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }//while(from_mask != 0){
}
// слона---------------------------------------------------------------------------------- 




// ходы коня ----------------------------------------------------------------------------------
{
// находим месторасположение коня в битовом списке и пока оно не нулевое бежим по маске 
// расположения коней обнуляя тот бит который мы уже посмотрели
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[2];
  while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);
    move_mask = knigt_moves_masks[from] & (~(bitboard->all_piece));

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(2,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 }// while(from_mask != 0){
}
// коня ----------------------------------------------------------------------------------

// коды пешки----------------------------------------------------------------------------------
{
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[1];
  while(from_mask != 0){
    from = first_one(from_mask);
    one_pawn_mask = from_mask & move_masks[from];
    from_mask = from_mask & (from_mask-1);

// 1 находим простые ходы черных пешек на одно поле вперед
  move_mask = (one_pawn_mask >> 8) & (~(bitboard->all_piece));

// 2 находим ходы пешек на 2 поля вперед 
// в начале оставляем только пешки стоящие на 7 горизонтали
  move_mask_d = one_pawn_mask & LINE_6;

// отметаем пешки если они чем то заблокированны
  move_mask_d = (move_mask_d >>8) & (~(bitboard->all_piece));
// оставшимися пешками делаем ход
  move_mask_d = (move_mask_d >> 8) & (~(bitboard->all_piece));

// здесь все ходы пешки как на одно так на два поля вперед
  move_mask = (move_mask | move_mask_d) ;

// ходы без превращений
  move_mask = move_mask & ~LINE_0;

// Заполняем список ходов+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
list_record_m(1,1,p,from,move_mask,bitboard,list_surplus_moves);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}//while(from_mask != 0){
// пешки----------------------------------------------------------------------------------
}

list_surplus_moves->end_list = p;

}//void Generator_move(){


//	*******************************************************************

//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
int King_white_check(const struct T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

int blocking_coordinate = 0; // координата блокированного поля 
int from = 0; // координата поля откуда фигура ходит
int i = 0; // координата 

// маска расположения белого короля
  from_mask = bitboard->white_piece[6];
 // находим координату фигуры(самый первый(для нашего отображения правый ) установленный бит)
  from = first_one(from_mask);

// луч вверх
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) находим блокированные биты.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];

 if(blocking_square_mask !=0){ 
// 2) находим координату первого блокированного бита
   blocking_coordinate =  first_one(blocking_square_mask);
// 3) находим маску разрешенных ходов (пока вместе с взятием своих фигур)
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
// 4) находим маску взятий
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);

   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч вниз  
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вверх - вправо
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч вверх - влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



// Конь
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_black_piece & knigt_moves_masks[from]);

// Смотрим коня++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0 ){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Король
// находим маску взятий короля
  captures_mask = (bitboard->all_black_piece & king_moves_masks[from]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Смотрим короля++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

if ((from / 7) < 8){
// Пешки считаем напрямую
   if (from % 8 != 0){
     if( bitboard->black_name_from_coord[from + 7] ==1) return 1;
   }//if (from % 8 != 1){
   if (from % 8 != 7){
     if( bitboard->black_name_from_coord[from + 9] ==1) return 1;
   }//if (from % 8 != 7){
}//if ((from / 7) < 8){

 return 0; 
}//int White_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// под боем ли король если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
int King_black_check(const struct T_Bitboard * bitboard){
unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

int blocking_coordinate = 0; // координата блокированного поля 
int from = 0; // координата поля откуда фигура ходит
int i = 0; // координата 

 // Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  from_mask = bitboard->black_piece[6];
  from = first_one(from_mask);
 
 // Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх
 blocking_square_mask = (bitboard->all_piece) & rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
 
// луч вниз  
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вверх - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// Конь
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_white_piece & knigt_moves_masks[from]);

// Смотрим коня++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Король
// находим маску взятий короля
  captures_mask = (bitboard->all_white_piece & king_moves_masks[from]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Смотрим короля++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

if ((from / 7) > 1 ){
// Пешки считаем напрямую
   if (from % 8 != 7){
     if( bitboard->white_name_from_coord[from - 7] ==1) return 1;
   }// if (from % 8 != 7){
   if (from % 8 != 0){
     if( bitboard->white_name_from_coord[from - 9] ==1) return 1;
   }// if (from % 8 != 7){
}//if ((from / 7) >0){

 return 0;
 
}//int White_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// под боем ли белые поля если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
static inline int White_check_square(int sq, const struct T_Bitboard * bitboard){

unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

int blocking_coordinate = 0; // координата блокированного поля 
int i = 0; // координата 

// луч вверх
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 1) находим блокированные биты.
   blocking_square_mask = (bitboard->all_piece) & rook_up_masks[sq];

 if(blocking_square_mask !=0){ 
// 2) находим координату первого блокированного бита
   blocking_coordinate =  first_one(blocking_square_mask);
// 3) находим маску разрешенных ходов (пока вместе с взятием своих фигур)
   move_mask = rook_up_masks[sq] ^ rook_up_masks[blocking_coordinate];
// 4) находим маску взятий
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[sq] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[sq] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==4) return 4;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вверх - вправо
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_right_masks[sq] ^ bishop_up_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч вверх - влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & bishop_up_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = bishop_up_left_masks[sq] ^ bishop_up_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_black_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->black_name_from_coord[i] ==3) return 3;
   if( bitboard->black_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// Конь
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_black_piece & knigt_moves_masks[sq]);

// Смотрим коня++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0 ){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Король
// находим маску взятий короля
  captures_mask = (bitboard->all_black_piece & king_moves_masks[sq]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Смотрим короля++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->black_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Пешки считаем напрямую
 if( bitboard->black_name_from_coord[sq + 7] ==1) return 1;
 if( bitboard->black_name_from_coord[sq + 9] ==1) return 1;

 return 0; 
}//int White_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// под боем ли черные поля если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
static inline int Black_check_square(int sq, const struct T_Bitboard * bitboard){

unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
unsigned __int64 captures_mask = 0; // маска взятий
unsigned __int64 move_mask = 0; // маска ходов

unsigned __int64 blocking_square_mask=0; // маска блокированных полей

int blocking_coordinate = 0; // координата блокированного поля 
int i = 0; // координата 

// луч вниз  
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = (bitboard->all_piece) & rook_down_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_down_masks[sq] ^ rook_down_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// луч влево
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 blocking_square_mask = bitboard->all_piece & rook_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = rook_left_masks[sq] ^ rook_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 


// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вправо
 blocking_square_mask = (bitboard->all_piece) & rook_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = rook_right_masks[sq] ^ rook_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){

// Смотрим на луче ладью и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==4) return 4;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// луч вниз - вправо
 blocking_square_mask = (bitboard->all_piece) & bishop_down_right_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_right_masks[sq] ^ bishop_down_right_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// луч вниз - влево
 blocking_square_mask = (bitboard->all_piece) & bishop_down_left_masks[sq];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = bishop_down_left_masks[sq] ^ bishop_down_left_masks[blocking_coordinate];
   captures_mask = bitboard->all_white_piece & move_masks[blocking_coordinate];
 }else{
   captures_mask =0;
 }//if(blocking_square_mask !=0){ 

// Смотрим на луче слона и ферзя++++++++++++++++++++++++++++++++++++++++++++++++
 if(captures_mask !=0){
   i = first_one(captures_mask);
   if( bitboard->white_name_from_coord[i] ==3) return 3;
   if( bitboard->white_name_from_coord[i] ==5) return 5;
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// Конь
// Генерируем ходы++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   captures_mask = (bitboard->all_white_piece & knigt_moves_masks[sq]);

// Смотрим коня++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==2) return 2;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Король
// находим маску взятий короля
  captures_mask = (bitboard->all_white_piece & king_moves_masks[sq]);
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Смотрим короля++++++++++++++++++++++++++++++++++++++++++++++++
 while(captures_mask !=0){
   if( bitboard->white_name_from_coord[first_one(captures_mask)] ==6) return 6;
   captures_mask = captures_mask & (captures_mask-1);
 }//if(captures_mask ==0){
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Пешки считаем напрямую

 if( bitboard->white_name_from_coord[sq - 7] ==1) return 1;
 if( bitboard->white_name_from_coord[sq - 9] ==1) return 1;

 return 0; 
}//int Black_check_square(int sq, T_Bitboard * bitboard){
//	*******************************************************************

//	===================================================================
// печатаем битбоард доску
#if TEST_B
void Bitboard_print(const struct T_Bitboard * bitboard){
unsigned __int64 z=0;
    Test_Bitboard.open ("_TEST_BITBOARD.txt",ios::app);

Test_Bitboard << "bitboard->colour_move = "<< bitboard->colour_move <<"\n";
Test_Bitboard << "\n";

 Test_Bitboard << "bitboard->taking_passage_yes= " << bitboard->taking_passage_yes << "\n";
 Test_Bitboard << "bitboard->taking_passage_x= " << bitboard->taking_passage_x << "\n";
 Test_Bitboard << "bitboard->taking_passage_y= " << bitboard->taking_passage_y << "\n";

Test_Bitboard << "\n";
 Test_Bitboard << "bitboard->castling_K= " << bitboard->castling_K << "\n";
 Test_Bitboard << "bitboard->castling_Q= " << bitboard->castling_Q << "\n";
 Test_Bitboard << "bitboard->castling_k= " << bitboard->castling_k << "\n";
 Test_Bitboard << "bitboard->castling_q= " << bitboard->castling_q << "\n";

Test_Bitboard << "------------------------"<<"\n";
Test_Bitboard << "\n";
// all_white_piece
//-------------------------------------------------------------------------------------
Test_Bitboard << "\n";
Test_Bitboard << "all_white_piece"<< "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z= bitboard->all_white_piece & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------

// all_black_piece
//-------------------------------------------------------------------------------------
Test_Bitboard << "\n";
Test_Bitboard << "all_black_piece"<< "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z= bitboard->all_black_piece & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
 Test_Bitboard << "\n";
 Test_Bitboard << "white_piece"<< "\n";
for(int p=1 ; p<7 ; p++){
Test_Bitboard << "\n";
Test_Bitboard << p << "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z=  bitboard->white_piece[p] & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
}//for(int i=0 ; i<8 ; i++){
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
 Test_Bitboard << "\n";
 Test_Bitboard << "black_piece"<< "\n";
for(int p=1 ; p<7 ; p++){
Test_Bitboard << "\n";
Test_Bitboard << p << "\n";
	for (int i=63; i>-1 ; i=i-8){
       for (int j=i-7; j<i+1 ; j++){
         z=  bitboard->black_piece[p] & move_masks[j];

         if (z == 0){
           Test_Bitboard << "0 ";
         }else{
           Test_Bitboard << "1 ";
         }//if (z == 0){ 
	   }//for ( j=i-7; i>j+1 ; j++){
       Test_Bitboard << "\n";
   }//for (int i=63; i>-1 ; i--){
}//for(int i=0 ; i<8 ; i++){
//-------------------------------------------------------------------------------------

Test_Bitboard <<"============================================"<< "\n";

    Test_Bitboard.close();
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
}//void Bitboard_print(T_Bitboard * bitboard){
#endif//TEST_B
//	*******************************************************************

//	===================================================================
// начальная инициализация доски
void Bitboard_ini_t(struct T_Bitboard * bitboard){

bitboard->all_piece =0;
bitboard->all_white_piece =0;
bitboard->all_black_piece =0;

for(int i=0 ; i<7 ; i++){
  bitboard->white_piece[i]=0;
  bitboard->black_piece[i]=0;
}//for(int i=0 ; i<8 ; i++){

// медленней но очевидней
for(int j=0 ; j<64 ; j++){
  bitboard->white_name_from_coord[j]=0;
  bitboard->black_name_from_coord[j]=0;
}//for(int j=0 ; j<64 ; j++){

}//void Bitboard_ini_t(T_Bitboard * bitboard){
//	*******************************************************************


