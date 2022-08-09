#include <iostream>
#include <fstream>  // TEST
#include <stdlib.h>
using namespace std;

#include "j_Hash.h"
#include "k_Util.h"


//-----------------------------------------------------------------------------------
/* реализуем хеш-таблицу
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  2.11.2009 28.1.2010
 *
*/
//---------------------------------------------------------------------------

//extern class UTIL util_o;

// задаем имя фигур
#define N_KING    6 // король
#define N_QUEEN   5 // ферзь
#define N_ROOK    4 // ладья
#define N_BISHOP  3 // слон
#define N_KNIGHT  2 // конь
#define N_PAWN    1 // пешка

 //           тип хода
 #define NO_MOVE 0 // нет хода
 
 #define SIMPLE_MOVE 1 //простой ход
 #define CAPTURE_MOVE 2 //взятие
 #define LONG_CASTLING 3 //длинная рокировка
 #define SHORT_CASTLING 4 //короткая рокировка
 #define TAKING_PASSAGE 5 //взятие на проходе

 #define TRANSFORMATION_IN_KNIGHT 12 //превращение в конь 
 #define TRANSFORMATION_IN_BISHOP 13 //превращение в слон
 #define TRANSFORMATION_IN_ROOK  14 //превращение в ладья 
 #define TRANSFORMATION_IN_QUEEN 15 //превращение в ферзь 

 #define CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT 22 //взятие с превращением в конь 
 #define CAPTURE_WITH_TRANSFORMATION_IN_BISHOP 23 //взятие с превращением в слон
 #define CAPTURE_WITH_TRANSFORMATION_IN_ROOK 24 //взятие с превращением в ладья 
 #define CAPTURE_WITH_TRANSFORMATION_IN_QUEEN 25 //взятие с превращением в ферзь 

// согласовать с файлом g_Loop.cpp
 #define EXACT 1 // точный ход
 #define LOWER 2 // отсечка по бете(альфе)
 #define UPPER 3 // альфу(бету) улучшить не удалось

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



// global

// максимальное количество элементов в хеш-таблице
unsigned __int64 max_index_hash;

// указатель на хеш-таблицу(это еще Си стиль)
struct T_hash_moves * p_hash_moves;

// в начале нам нужно заполнить вспомогательный массив случайных чисел 
// связанных с расположением фигуры на доске в нем все фигуры на всех полях
// это трехмерный массив координаты которого цвет, имя, координата фигуры
// массив этот глобальный и содержит случайные числа
// в дальнейшем складывая эти числа мы получим случайный ключ тем не менее
// однозначно связанный с данной позицией
// [цвет фигуры][имя фигуры][координата фигуры]
// 2*6*64= 768 
unsigned __int64 random_piese_key[2][7][64];

// массив для запоминания хеш-ключей позиций при погружении на глубину
// т.е. у нас записываются не только ходы в pv_current но и ключи позиций
// другими словами запоминаем ветку ключей позиций
unsigned __int64 hash_three_moves[120];

// массив для запоминания хеш-ключей позиций
// при анализе входящей строки от оболочки
// другими словами запоминаем ветку ключей позиций расчитанных по входящей строке от оболочки
unsigned __int64 hash_string_moves[1000];

// запоминаем количество ходов в строке
unsigned __int16 save_cnt_move; 

// возраст элемента таблицы. нужен что бы перезаписывать элементы от прошлых позиций
unsigned __int8 age; 




// ТЕСТ ХЕШ - ТАБЛИЦЫ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TEST_WRITE_READ

//	===================================================================
// обнуляем счетчики
 void HASHM::Test_hash_ini(){
// тестируем запись в хеш таблицу--------------------------------------

// общее количество записанных позиций
hash_write_all = 0;

// количество записанных позиций для сортировки ходов
hash_move_write = 0;

// количество записанных позиций для отсечки по оценке
hash_score_write_low = 0;

// количество записанных позиций для отсечки по оценке
hash_score_write_high = 0;

// тестируем использование записи---------------------------------------- 

// используем для сортировки ходов
hash_read_sorting = 0; 

// используем для отсечки по оценке общее количество
hash_read_score = 0;  

hash_read_score_LOWER = 0;

hash_read_score_UPPER = 0;

 }//void HASHM::Test_hash_ini()
//	*******************************************************************

//	===================================================================
// печатаем счетчики
void HASHM::Test_hash_print(){


cout << " " << "\n";

hash_write_all = hash_move_write + hash_score_write_low + hash_score_write_high;

// печатаем количество всех записанных позиций
cout << "hash_write_all = " << hash_write_all << "\n";

if (hash_write_all != 0){
// количество записанных позиций для сортировки ходов
cout << "hash_move_write = " << hash_move_write << "   100 * hash_move_write/hash_write_all = " << 100 * hash_move_write/hash_write_all << "\n";

// количество записанных позиций для отсечки по оценке
cout << "hash_score_write_low = " << hash_score_write_low << "   100 * hash_score_write_low/hash_write_all = " << 100 * hash_score_write_low/hash_write_all << "\n";

// количество записанных позиций для отсечки по оценке
cout << "hash_score_write_high = " << hash_score_write_high << "   100 * hash_score_write_high/hash_write_all = " << 100 * hash_score_write_high/hash_write_all << "\n";

cout << " " << "\n";

// печатаем количество использованных для сортировки позиций
cout << "hash_read_sorting = " << hash_read_sorting << "   100 * hash_read_sorting/hash_write_all = " << 100 * hash_read_sorting/hash_write_all << "\n";

}

// дельта между записанными и использованными
cout << "hash_write_all - hash_read_sorting = " << (hash_write_all - hash_read_sorting) << "\n";


cout << " " << "\n";

hash_read_score = hash_read_score_LOWER + hash_read_score_UPPER;

if( hash_read_score != 0){

if (hash_write_all != 0){
   // печатаем количество всех считанных позиций
   cout << "hash_read_score = " << hash_read_score << "   100 * hash_read_score/hash_write_all = " << 100 * hash_read_score/hash_write_all << "\n";

   // печатаем количество всех считанных позиций
   cout << "hash_read_all = " << (hash_read_sorting + hash_read_score) << "   100 * hash_read_all/hash_write_all = " << 100 * (hash_read_sorting + hash_read_score)/hash_write_all << "\n";
}
   // печатаем количество использованных для отсчеки по оценки позиций
   cout << "hash_read_score_LOWER = " << hash_read_score_LOWER <<  "   100 * hash_read_score_LOWER/hash_read_score = " << 100 * hash_read_score_LOWER/hash_read_score << "\n";

   // печатаем количество использованных для отсчеки по оценки позиций
   cout << "hash_read_score_UPPER = " << hash_read_score_UPPER <<  "   100 * hash_read_score_UPPER/hash_read_score = " << 100 * hash_read_score_UPPER/hash_read_score << "\n";


}//if( hash_read_score != 0)

cout << " " << "\n";

 }// void HASHM::Test_hash_print()
//	*******************************************************************

#endif // TEST_WRITE_READ
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// РАБОТА С ХЕШ - ТАБЛИЦЕЙ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// выделяем память под кэш. размер задается в мегабайтах
 void HASHM::Hash_size_ini_default(unsigned __int16 hash_size// размер хеш-таблицы в мегабайтах
						           ){
unsigned __int64 hash_n = 0;

if (hash_size < 1) hash_size = 1;// память не меньше мегабайта
if (hash_size > 1024) hash_size = 1024;// память не больше гигабайта

// функция sizeof(struct T_hash_moves) выдает размер в байтах т.е. int это 4
// в данный момент struct T_hash_moves - 16 байт
// перегоняем размер в Мб в байты а потом в количество элементов
max_index_hash = (unsigned __int64)((hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));
        // cout << "max_index_hash in 1 mb = " << max_index_hash << "\n";
        // cout << "sizeof(T_hash_moves) = " << sizeof(struct T_hash_moves) << "\n";
         //cout << "sizeof(unsigned int) = " << sizeof(unsigned int) << "\n";

// нам нужно получить размер (2 в степени n) -1 что бы получилось 
// число вида 000..0111 и можно было вгонять хеш - индекс в пределы массива операцией &
for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

hash_n = hash_n / 2;

if ((hash_n & (hash_n - 1)) != 0) cout << "ЗАСАДА! размер не вида степени двух " << hash_n << "\n";

max_index_hash = hash_n - 1;

// calloc
// Allocates a block of memory for an array of num elements, 
// each of them size bytes long, and initializes all its bits to zero.
// резервируем память под таблицу и заполняем ее нулями
p_hash_moves = (struct T_hash_moves*) calloc ((__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

if (p_hash_moves == NULL) {
    cout << "памяти нет " << "\n";
	exit(1); 
}//if (p_hash_moves == NULL) {

    // TEST
    //cout << "sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
    //cout << "max_index_hash = " << max_index_hash << "\n";
    //cout << "hash_n = " << hash_n << "\n";

}//void HASHM::Hash_size_ini_default
//	*******************************************************************

//	===================================================================
// освобождаем память из под хеш-таблицы 
// и по новой выделяем память под кэш. размер задается в мегабайтах
void HASHM::Hash_size_resize(unsigned __int16 hash_size// размер хеш-таблицы в мегабайтах
				          ){

unsigned __int64 hash_n = 0;

if (p_hash_moves == NULL) {
    cout << "освобождать нечего " << "\n";
	exit(1); 
}//if (p_hash_moves == NULL) {

free(p_hash_moves);// освобождаем предыдущий объем памяти

if (hash_size < 1) hash_size = 1;// память не меньше мегабайта
if (hash_size > 1024) hash_size = 1024;// память не больше гигабайта

// перегоняем размер в Мб в байты а потом в количество элементов
max_index_hash = (unsigned __int64)((hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));

    // TEST
//    cout << "предварительный max_index_hash = " << max_index_hash << "\n";
 

// нам нужно получить размер (2 в степени n) -1 что бы получилось 
// число вида 000..0111 и можно было вгонять хеш - индекс в пределы массива операцией &
for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

hash_n = hash_n / 2;

if ((hash_n & (hash_n - 1)) != 0) cout << "ЗАСАДА! размер не вида степени двух " << hash_n << "\n";

max_index_hash = hash_n - 1;

// резервируем память под таблицу и заполняем ее нулями
p_hash_moves = (struct T_hash_moves*) calloc ((__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

if (p_hash_moves == NULL) {
    cout << "памяти нет " << "\n";
	exit(1); 
}//if (p_hash_moves == NULL) {

// TEST
 //cout << " размер хеш-таблицы в мегабайтах hash_size = " << hash_size << "\n";
 //cout << " в итоге выделили памяти в мб = " << ((int)(max_index_hash + 3) * sizeof(T_hash_moves))/(1024 * 1024) << "\n";
 //cout << "размер элемента в байтах sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
 //cout << "число степени 2 hash_n = " << hash_n << "\n";
 //cout << "количество элементов в хеш-таблице max_index_hash = " << max_index_hash << "\n";

}//void HASHM::Hash_size_resize
//	*******************************************************************

//	===================================================================
// освобождаем память из под хеш-таблицы
void HASHM::Hash_size_free(){

   free(p_hash_moves);

}//void HASHM::Hash_size_free
//	*******************************************************************

//	===================================================================
// инициализируем все ключи хеш-таблицы 0
void HASHM::Ini_hash_moves(){

              //TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
                 #if TEST_H
                     hash_save = 0;
                 #endif 
                 //TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
                                                             
for(unsigned __int32 ihm = 0; ihm <= max_index_hash; ihm++){

   p_hash_moves[ihm].key_lock_32_move = 0;

   p_hash_moves[ihm].key_lock_32_score = 0;


}//for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){

}//void HASHM::Ini_hash_moves
//	*******************************************************************

//	===================================================================
// кэшируем лучший на данный момент ход или оценку (зависит от поиска)
void HASHM::Save_best_hash(unsigned __int8 i,// номер хода по списку
						   unsigned __int64 key,// хеш-ключ
						   struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
						   __int32 score,// оценка хода
						   unsigned __int8 flag_hash_score,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
						   unsigned __int8 depth,// глубина записи хода
						   unsigned __int8 depth_max// максимальная глубина расчета
						   ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);
unsigned __int8 delta_depth = depth_max - depth;

if (delta_depth < 0) delta_depth = 0;

/*
запись лучшего хода и лучшей оценки разделены
потому что при перезаписи в общем случае мы имеем противоречивые требования

на самом деле мы пишем две позиции в одну ячейку хеш-таблицы
для уменьшения в двое коллизии перезаписи позиции
когда разные позиции хотят залезть в одну и ту же ячейку

*/
 // запись лучшего хода
 Save_best_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

 // запись лучшей оценки
 Save_best_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);


}//void HASHM::Save_best_hash_key
//	*******************************************************************

//	===================================================================
// кэшируем лучший на данный момент ход
void HASHM::Save_best_hash_move(unsigned __int8 i,// номер хода по списку
						        unsigned __int32 key_lock_32,//
						        struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
						        unsigned __int8 delta_depth,// глубина записи хода
                                unsigned __int32 ind//
						        ){

/*

при записи хода в таблицу возможно три случая:
1) ячейка таблицы пустая. в пустой ячейке значение ключа 0
   в пустую ячейку пишем всегда 

2) ячейка занята той же позицией тогда
         перезаписываем в случае когда позиция изучена глубже т.е. больше длина варианта

3) ячейка занята другой позицией тогда
     если записанная позиция из текущего и предыдущего поиска то
         перезаписываем в случае когда позиция изучена глубже т.е. больше длина варианта
     если же от более древних поисков то перезаписываем всегда. 

-------------
некоторые мысли:

в текущем поиске позицию имеет смысл перезаписывать только если глубина варианта больше.
это имеет значение и в случае перезаписи в ту же ячейку другой позиции.
главный смысл в том что бы низкоуровневый муссор которого очень много не уничтожал лучше
просчитанные и более ценные позиции.

для той же позиции оставляем перезапись только если глубина варианта больше
в случае перезаписи для одного предыдущего слоя по глубине а все более поздние всегда.
тесты показывают что вроде так лучше

для сортировки ходов
мы не используем результаты поисков предыдущих ходов главным образом потому что для разных
глубин лучшие ходы разные так что смысла в глубоком лучшем ходе во время новой иттерации
нет. как правило пока не достигнута глубина хода он другой а значит не ускоряет а только
тормозит поиск и только на равной глубине есть выигрыш. вопрос баланса по моему сильно 
неоднозначен потому что поиск может идти по другому пути чем при поиске предыущего хода.

*/
  
// 1) ячейка пустая------------------------------------------------------------------------
if(p_hash_moves[ind].key_lock_32_move == 0){

    // записываем лучший ход в любом случае
    //-----------------------------------------------------------------------
    Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
    //-----------------------------------------------------------------------

// 2) та же позиция------------------------------------------------------------------------
}else if(p_hash_moves[ind].key_lock_32_move == key_lock_32) {

     // имеем текущий поиск
     if( age == p_hash_moves[ind].move_age ) {

         //ход перезаписываем если глубина варианта больше или равна
		 if( delta_depth >= p_hash_moves[ind].move_delta_depth  ){ 

             Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

		 }//if( delta_depth >= p_hash_moves[ind].move_delta_depth  )

	 }else{
         // более поздние перезаписываем всегда
         Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	 }// if( age == p_hash_moves[ind].age ) 

// 3) другая позиция------------------------------------------------------------------------
}else if(p_hash_moves[ind].key_lock_32_move != key_lock_32) {

     // имеем текущий поиск
     if( age == p_hash_moves[ind].move_age ) {

         //ход перезаписываем если глубина варианта больше или равна
		 if( delta_depth >= p_hash_moves[ind].move_delta_depth  ){ 

              Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

		 }//if( delta_depth >= p_hash_moves[ind].move_delta_depth  )

	 }else{

          // более поздние перезаписываем всегда
          Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	 }// if( age == p_hash_moves[ind].age ) 

}//if(p_hash_moves[ind].key_lock_32_move == 0){---------------------------------------------

}//void HASHM::Save_best_hash_key
//	*******************************************************************

//	===================================================================
// кэшируем лучший на данный момент ход 
void HASHM::Save_hash_move(unsigned __int8 i,// номер хода по списку
						   unsigned __int32 key_lock_32,//
						   struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
						   unsigned __int8 delta_depth,// глубина записи хода
                           unsigned __int32 ind//   
				           ){

 // запоминаем верхние 32 бита ключа
 p_hash_moves[ind].key_lock_32_move = key_lock_32;

 // запоминаем лучший ход
 p_hash_moves[ind].move = p_list_surplus_moves->move[i];

// глубина варианта при которой получен данный ход
 p_hash_moves[ind].move_delta_depth = delta_depth;

 // запоминаем возраст хода
 p_hash_moves[ind].move_age = age;

#if TEST_WRITE_READ
  // количество записанных позиций
  hash_move_write = hash_move_write + 1;
#endif // TEST_WRITE_READ


}//void HASHM::Save_hash_key
//	*******************************************************************

//	===================================================================
// кэшируем лучшую на данный момент оценку
void HASHM::Save_best_hash_score(unsigned __int32 key_lock_32,//
						         __int32 score,// оценка хода
						         unsigned __int8 flag_hash_score,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
						         unsigned __int8 delta_depth,// глубина записи хода
                                 unsigned __int32 ind//
						         ){


	// кэшируем лучшую на данный момент оценку

/*

при записи оценки в таблицу возможно три случая:
1) ячейка таблицы пустая. в пустой ячейке значение ключа 0
   в пустую ячейку пишем всегда 

2) ячейка занята той же позицией
   если записанная позиция из текущего поиска то
   перезаписываем если позиция изучена глубже т.е. больше длина варианта


3) ячейка занята другой позицией
   если занята позицией из прошлых поисков, кроме предпоследнего, то перезаписываем
   если из текущего и предпоследнего то перезаписываем глубже изученными позициями.

   (это необходимо контролировать потому что иначе позиции вытесняются ничего не значащими
   позициями.)

*/
  
// 1) ячейка пустая
if(p_hash_moves[ind].key_lock_32_score == 0){

       // записываем лучший ход в любом случае
       Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);


// 2) ту же самую позицию перезаписываем если вариант глубже или равен
}else if( p_hash_moves[ind].key_lock_32_score == key_lock_32){

              //ход перезаписываем если глубина варианта больше или равна
		      if( delta_depth >= p_hash_moves[ind].score_delta_depth ){ 

                    Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);

		      }//if( delta_depth >= p_hash_moves[ind].score_delta_depth )

 
// 3) ячейка заполнена другой позицией
}else if(p_hash_moves[ind].key_lock_32_score != key_lock_32){

	          // текущий поиск и предыдущий заменяем если глубина варианта больше
	          if( age <= p_hash_moves[ind].score_age + 1) { // 

                     //ход перезаписываем если глубина варианта больше или равна
		             if( delta_depth >= p_hash_moves[ind].score_delta_depth ){

                         Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);

		             }//if( delta_depth >= p_hash_moves[ind].score_delta_depth )


			  }else{
                     // более поздние перезаписываем всегда
                     Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);


			  }//if( age <= p_hash_moves[ind].score_age + 1)


}//if(p_hash_moves[ind].key_lock_32_score == 0)

}//void HASHM::Save_best_hash_key
//	*******************************************************************


//	===================================================================
// кэшируем оценку т.е. записываем ее в хеш-таблицу
void HASHM::Save_hash_score(unsigned __int32 key_lock_32,//
						    __int32 score,// оценка хода
						    unsigned __int8 flag_hash_score,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
						    unsigned __int8 delta_depth,// глубина записи хода
                            unsigned __int32 ind//
				            ){

 // запоминаем верхние 32 бита ключа
 p_hash_moves[ind].key_lock_32_score = key_lock_32;

 // пишем оценку
 p_hash_moves[ind].score = score;

// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
 p_hash_moves[ind].flag_hash_score = flag_hash_score;


 // запоминаем текущую глубину
 p_hash_moves[ind].score_delta_depth = delta_depth;

 // запоминаем возраст
 p_hash_moves[ind].score_age = age;


#if TEST_WRITE_READ

  if (flag_hash_score == LOWER){
       // количество записанных позиций
       hash_score_write_low = hash_score_write_low + 1;
  }else{
       // количество записанных позиций
       hash_score_write_high = hash_score_write_high + 1;
  }//

#endif // TEST_WRITE_READ


}//void HASHM::Save_hash_key
//	*******************************************************************


//	===================================================================
// смотрим не встречалась ли нам такая позиция если встречалась то применяем сохраненную оценку
// поиск за белых
__int32 HASHM::Look_hash_score_white(const unsigned __int64 key,// хеш-ключ
			                         const unsigned __int8 depth,// глубина
                                     const unsigned __int8 depth_max,// максимальная глубина
					                 const __int32 alpha,//
		                             const __int32 beta,//
                                     unsigned __int8 & test_flag_hash_moves,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
                                     unsigned __int8 & hash_depth,// максимальная глубина
                                     unsigned __int8 & hash_depth_max// максимальная глубина
			                         ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);


/*
(описание со стороны белых)

при использовании оценки из хеш-таблицы возможно три случая:

1) (в моей программе этот случай не работает)
   EXACT (проверяем score <  beta )
   при переборе получили точную оценку т.е. beta > score > alpha
   так как мы считаем с нулевым окном и кроме того а-б пределы могут менятся
   то точная оценка на самом деле не очень точна
   поэтому прежде чем ее использовать проверяем что она все еще как бы точна.
   проверяем только на score <  beta  
   потому что больше она alpha или меньше не важно
   так как в случае score < alpha мы просто получаем 3-ий вариант

2) LOWER (проверяем score >= beta )
   при переборе получили оценку которая вызвала отсечку по beta. это минимум
   т.е. при дальнейшем счете оценка бы только повышалась.
   нас волнует только что бы она и теперь вызывала отсечку по бете так что
   проверяем только score >= beta

3) UPPER (проверяем score < alpha )
   при переборе оценка не улучшила альфу и не вызвала отсечку по бете. это локальный
   максимум. реальной оценки мы не знаем. да нас и не волнует ее точность главное 
   что бы она по прежнему не улучшала альфу. 
   так что проверяем score < alpha
   конечно правильнее будет score <= alpha но при тестировании я не смог проверить этот случай

*/

// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
if(p_hash_moves[ind].key_lock_32_score == key_lock_32){

  // если позиция уже рассмотренна на длину варианта больше или равную той на которую
  // мы собираемся просмотреть то можно просто вернуть оценку варианта
  if( p_hash_moves[ind].score_delta_depth >= (depth_max - depth)){


    if (p_hash_moves[ind].flag_hash_score == LOWER){
       // надо проверить отекается ли кешированной оценкой поиск и в данном случае
       // из хеш - таблицы имеем не точную оценку а ту что отсекла поиск. если бы поиск был 
	   // продолжен то почти наверняка оценка бы усилилась. но нам от нее нужно только что бы
	   // она отсекала поиск и в данном случае.
		   if( p_hash_moves[ind].score >= beta ){ 

			   #if TEST_WRITE_READ
                  // 
                  hash_read_score_LOWER = hash_read_score_LOWER + 1;
               #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if( p_hash_moves[ind].score >= beta )
    
//----------------------------

	}else{//if (p_hash_moves[ind].flag_hash_score == LOWER)

// проверяем что оценка все еще не улучшает альфу
// тест показал что есть позиции где расходятся ходы и оценки
// и даже если а б одинаковы и поиск не в нулевом окне и глубина одинакова 
// все равно ходы как и оценки могут быть разными
// оценка очень нестабильна и зависит от порядка ходов однако условие <= а соблюдается
               if(p_hash_moves[ind].score < alpha) { 

			      #if TEST_WRITE_READ
                    // 
                    hash_read_score_UPPER = hash_read_score_UPPER + 1;
                  #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if(p_hash_moves[ind].score_high < alpha)

	}//if (p_hash_moves[ind].flag_hash_score == LOWER)

    }//if( p_hash_moves[ind].score_delta_depth >= (depth_max - depth)){

}//if(p_hash_moves[ind].key_lock_32_score == key_lock_32)

return -1000000000;

}// void HASHM::Look_hash
//	*******************************************************************

//	===================================================================
// смотрим не встречалась ли нам такая позиция если встречалась то применяем сохраненную оценку
//поиск за черных
__int32 HASHM::Look_hash_score_black(const unsigned __int64 key,// хеш-ключ
			                         const unsigned __int8 depth,// глубина
                                     const unsigned __int8 depth_max,// максимальная глубина
					                 const __int32 alpha,//
		                             const __int32 beta,//
                                     unsigned __int8 & test_flag_hash_moves,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
                                     unsigned __int8 & hash_depth,// максимальная глубина
                                     unsigned __int8 & hash_depth_max// максимальная глубина
			                         ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);


if(p_hash_moves[ind].key_lock_32_score == key_lock_32 ){

  if(p_hash_moves[ind].score_delta_depth >= (depth_max - depth)){

     if (p_hash_moves[ind].flag_hash_score == LOWER){

			if(p_hash_moves[ind].score <= alpha) { 

			   #if TEST_WRITE_READ
                  // 
                  hash_read_score_LOWER = hash_read_score_LOWER + 1;
               #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if(p_hash_moves[ind].score_move <= alpha)

	 }else{//if (p_hash_moves[ind].flag_hash_score == LOWER)

//---------------------------
            if(p_hash_moves[ind].score > beta) { 
			      #if TEST_WRITE_READ
                    // 
                    hash_read_score_UPPER = hash_read_score_UPPER + 1;
                  #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if(p_hash_moves[ind].score_move > beta)

      }//if (p_hash_moves[ind].flag_hash_score == LOWER)
   }//if(p_hash_moves[ind].score_delta >= (depth_max - depth))

}//if(p_hash_moves[ind].key_lock_32_score == (unsigned __int32)(key >>32) )

return -1000000000;

}// void HASHM::Look_hash
//	*******************************************************************

//	===================================================================
// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
void HASHM::Look_hash_move(struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
			               const unsigned __int64 key,// хеш-ключ
			               const unsigned __int8 depth,// глубина
			               unsigned __int8  & flag_insert_hash_move//флаг того что удалось найти хеш-ход и поднять на первую позицию
			               ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);

// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
if(p_hash_moves[ind].key_lock_32_move == (unsigned __int32)(key >> 32) ){
     
#if TEST_WRITE_READ
  // количество считанных для сортировки позиций
  hash_read_sorting = hash_read_sorting + 1;
#endif // TEST_WRITE_READ

      // выводим ход на первую позицию в списке
      Insert_hash_moves(p_list_surplus_moves,ind,depth,flag_insert_hash_move);

}//if(p_hash_moves[key & max_index_hash ].key == key ){


}// void HASHM::Look_hash
//	*******************************************************************


//	===================================================================
// поднимаем кэш ход на первую позицию в списке
// если ход нашелся то flag_insert_hash_move 2 иначе флаг 1
void HASHM::Insert_hash_moves(struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
					          unsigned __int32 ind,// хеш-ключ
					          unsigned __int8 depth ,// глубина
					          unsigned __int8  & flag_insert_hash_move//флаг того что удалось найти хеш-ход и поднять на первую позицию
					          ){


__int32 move_s = 0;
__int32 score_s;// используем для запоминания оценки хода
unsigned __int8 j;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    
static int kolizii = 0; 
static int enter = 0; 
int pozition = 0;

int move_print;

enter = enter + 1;

#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

flag_insert_hash_move = 1;// в функцию зашли


// 1 ищем хеш-ход в списке
for(j = 0; 
   ( (p_list_surplus_moves->move[j] != p_hash_moves[ind].move) 
   && (j < p_list_surplus_moves->end_list) ); 
   j++);


//
#if TEST_H

//--------------------
if (j == p_list_surplus_moves->end_list){	
//if (1){	
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.open ("_TEST_HASH.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){
    if(p_hash_moves[ihm].key != -1)  pozition = pozition + 1;
}//for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){

Test_Hash << "Всего позиций "<< max_index_hash <<"\n";
Test_Hash << "Для записей использовали позиций "<< pozition <<"\n";

move_print = p_hash_moves[key & max_index_hash ].move;

kolizii = kolizii + 1;

 Test_Hash << "Записи "<< hash_save <<"\n";
 Test_Hash << "Вхождения "<< enter <<"\n";
 Test_Hash << "Коллизия "<< kolizii <<"\n";
 Test_Hash << "depth "<< depth <<"\n";
 Test_Hash << "p_hash_moves[].depth " << p_hash_moves[key & max_index_hash ].depth << "\n";
 Test_Hash << "p_hash_moves[].depth_max " << p_hash_moves[key & max_index_hash ].depth_max << "\n";
 Test_Hash << "p_hash_moves[].flag_hash_moves " << p_hash_moves[key & max_index_hash ].flag_hash_moves << "\n";
 Test_Hash << "key "<< key <<"\n";
Test_Hash << "p_hash_moves[].key " << p_hash_moves[key & max_index_hash ].key << "\n";
 Test_Hash << "p_hash_moves[].move "<< move_print <<"\n";

//------------------------------------------
     Test_Hash << "Ходящая фигура " << util_o.int_to_char_board(((move_print>>24)& 7),1)<< "\n";
     Test_Hash << "p_hash_moves[].score_move " << p_hash_moves[key & max_index_hash ].score_move << "\n";

     // вывод хода вида e2-e4
     //------------------------------
	 Test_Hash << " " << (util_o.x_to_notation(((move_print>>12)& 63)%8));
     Test_Hash << "" << (1 + ((move_print>>12)& 63)/8);//<< "\n";
 //  Test_Hash<< "\n";
     Test_Hash << " " << (util_o.x_to_notation(((move_print>>18)& 63)%8));
     Test_Hash << "" << (1 + ((move_print>>18)& 63)/8)<< "\n";   
     //------------------------------
 //тип хода 
 //0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка 4 – короткая рокировка 5 – взятие на проходе
 //12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
 //22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
 //25 - взятие с превращением в ферзь 
     Test_Hash << "вид хода " << ((move_print>>6)& 63)  << "\n";
     Test_Hash << "Взятая фигура " << util_o.int_to_char_board(( move_print & 63),1)<< "\n";
	 //Test_Hash << "начальная положение " << ((move_print>>12)& 63)  << "\n";
	 //Test_Hash << "конечное положение " << ((move_print>>18)& 63)  << "\n";
     
	 Test_Hash << "\n";
//------------------------------------------

 Test_Hash << "p_list_surplus_moves->end_list= "<< p_list_surplus_moves->end_list <<"\n";
 Test_Hash << "j= "<< j <<"\n";
 Test_Hash << "-----------------------------------"<<"\n";

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//if (j == p_list_surplus_moves->end_list){	
//--------------------
#endif 


// если такой ход нашелся то идем дальше
if(j < p_list_surplus_moves->end_list){	
//------------------------------------------------------------------------
// 2 запоминаем хеш-ход 
  move_s  = p_list_surplus_moves->move[j];
  score_s = p_list_surplus_moves->sorting_score[j];
//------------------------------------------------------------------------

  // 3 сдвигаем все строчки вниз
  // от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
  for(unsigned __int8 i = j; i > 0; i--){	
 	 p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
     p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
  }//for(int i = j; i > start; i--){	

  //------------------------------------------------------------------------
  // 4 на стартовую позицию записываем хеш-ход
  p_list_surplus_moves->move[0] = move_s;
  p_list_surplus_moves->sorting_score[0] = score_s;
  //------------------------------------------------------------------------

  flag_insert_hash_move = 2;// ход нашелся

}//if(j < p_list_surplus_moves->end_list)


}//void HASHM::Insert_hash_moves
//	*******************************************************************

//	===================================================================
// тестируем хеш-таблицу. печатаем полное количество элементов сколько использовали а сколько пустых
void HASHM::TEST_Hash_index_print(){

unsigned __int32 full_move = 0;
unsigned __int32 full_score = 0;

unsigned __int32 yes_move = 0;
unsigned __int32 yes_score = 0;

unsigned __int32 no_move = 0;
unsigned __int32 no_score = 0;

   for(unsigned __int32 cnt = 0; cnt <= max_index_hash; cnt++){

      if(p_hash_moves[cnt].key_lock_32_move != 0)  yes_move = yes_move + 1 ;

      if(p_hash_moves[cnt].key_lock_32_score != 0)  yes_score = yes_score + 1 ;


      if(p_hash_moves[cnt].key_lock_32_move == 0)  no_move = no_move + 1 ;

      if(p_hash_moves[cnt].key_lock_32_score == 0)  no_score = no_score + 1 ;


   }//for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){

full_move = yes_move + no_move;

full_score = yes_score + no_score;


 cout << "  "<< "\n";
 cout << " полное количество элементов " << max_index_hash << "\n";

if (full_move != 0) cout << "full_move = " << full_move << "\n";
if (full_score != 0) cout << "full_score = " << full_score << "\n";
cout << "  "<< "\n";
if (yes_move != 0) cout << "yes_move = " << yes_move << "\n";
if (yes_score != 0) cout << "yes_score = " << yes_score << "\n";
cout << "  "<< "\n";
if (no_move != 0) cout << "no_move = " << no_move << "\n";
if (no_score != 0) cout << "no_score = " << no_score << "\n";
cout << "  "<< "\n";
if (no_move != 0) cout << " 100 * yes_move/no_move = " << 100 * yes_move/no_move << "\n";
if (no_score != 0) cout << " 100 * yes_score/no_score = " << 100 * yes_score/no_score << "\n";
cout << "  "<< "\n";


}//HASHM::TEST_Hash_index()
//	*******************************************************************

//	===================================================================
// обнуляем возраст позиции
void HASHM::Set_age_0(){

	age = 0;

}//void HASHM::set_age_0()
//	*******************************************************************

//	===================================================================
// следующий век :)
// увеличиваем возраст позиции
void HASHM::Aging(){

	if (age > 250) age = 0;
	age = age + 1;
 //   cout << "age= " << (__int32)age << "\n";

}//void HASHM::aging()
//	*******************************************************************





// РАБОТА С ХЕШ - КЛЮЧОМ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// генерируем случайный 64 битный ключ
unsigned __int64 HASHM::rand_64(){
//в основе генерации случайных чисел лежит стандартная функция библиотеки
//<cstdlib>	возвращающая псевдо случайное число в диапазоне 0 до 32767

/*
 int rand ( void ); 
 Returns a pseudo-random integral number in the range 0 to RAND_MAX.
 RAND_MAX is a constant defined in <cstdlib>. Its default value may vary between 
 implementations but it is granted to be at least 32767.
*/

return rand() ^ ( ((unsigned __int64)rand() << 15) ^ ((unsigned __int64)rand() << 31) ^ 
				   ((unsigned __int64)rand() << 47) ^ ((unsigned __int64)rand() << 59));

}//unsigned __int64 HASHM::rand_64
//	*******************************************************************

//	===================================================================
// каждой возможной позиции каждой возможной фигуры 
// присваиваем случайное число
void HASHM::Ini_random_piese_key(){

__int32 qq = 0;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.open ("_TEST_HASH.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//[цвет фигуры][имя фигуры][координата фигуры]
 for(unsigned __int8 color_piece = 0; color_piece < 2; color_piece++){

	 for(unsigned __int8 name_piece = 1; name_piece < 7; name_piece++){

		 for(unsigned __int8 coordinate_piece = 0; coordinate_piece < 64; coordinate_piece++){

             random_piese_key[color_piece][name_piece][coordinate_piece] = rand_64();


     //        Test_Hash << random_piese_key[color_piece][name_piece][coordinate_piece] << "\n";
	     }//for(int coordinate_piece = 0; coordinate_piece < 64; coordinate_piece++){

	 }//for(int name_piece = 1; name_piece < 7; name_piece++){

 }//for(int color_piece = 0; color_piece < 2; color_piece++){

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
// ищем коллизии в самом генераторе
 for(int c = 0; c < 2; c++){
	 for(int n = 1; n < 7; n++){
		for(int q = 0; q < 64; q++){

 for(int cv = 0; cv < 2; cv++){
	 for(int nv = 1; nv < 7; nv++){
		for(int qv = 0; qv < 64; qv++){
if ( (c != cv) || (n != nv) || (q != qv) ) {
			if ( random_piese_key[c][n][q] == random_piese_key[cv][nv][qv]) {
			   qq = qq + 1;
               Test_Hash << "Повтор " << qq << "\n";
			}//if ( random_piese_key[c][n][q] == random_piese_key[cv][nv][qv]) {
 }//if ( (c != cv) || (n != nv) || (q != qv) ) {
	    }//for(int qv = 0; qv < 64; qv++){
	 }//for(int nv = 1; nv < 7; nv++){
 }//for(int cv = 0; cv < 2; cv++){

	    }//for(int q = 0; q < 120; q++){
	 }//for(int n = 1; n < 7; n++){
 }//for(int c = 1; c < 3; c++){
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//void HASHM::Ini_random_piese_key
//	*******************************************************************

//	===================================================================
// инициализируем ключ начальной позиции
void HASHM::Start_position_random_key(struct T_Bitboard * p_bitboard// представление доски                                    
							          ){

unsigned __int8  colour = 0;// цвет фигуры
unsigned __int8  name = 0;// имя фигуры
unsigned __int64 key = 0;// нулевое значение ключа

//cout << "--------------------------------------------- " << "\n";

// тестировал ключи
//for (int i = 0; i < 64; i++){
//      
//      name = p_bitboard->white_name_from_coord[i];
//	  if(name != 0) {
//		  key = key ^ random_piese_key[1][name][i];
//		  cout << "white_name name = " << name << " i = " << i << "  random_piese_key[1][name][i] = " << random_piese_key[1][name][i] << "\n";
//	  }
//
//   //   name = p_bitboard->black_name_from_coord[i];
//	  //if(name != 0) {
//		 // key = key ^ random_piese_key[0][name][i];
//   //       cout << "black_name name = " << name << "\n";
//	  //}
//
//}//for (int i = 0; i < 64; i++){

for (unsigned __int8 i = 0; i < 64; i++){
      
      name = p_bitboard->white_name_from_coord[i];
	  if(name != 0) {
		  key = key ^ random_piese_key[1][name][i];
	  }

      name = p_bitboard->black_name_from_coord[i];
	  if(name != 0) {
		  key = key ^ random_piese_key[0][name][i];
   //       cout << "black_name name = " << name << " i = " << i << "  random_piese_key[1][name][i] = " << random_piese_key[1][name][i] << "\n";
	  }

}//for (int i = 0; i < 64; i++){

// учитываем цвет хода
// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
// некоторые позиции отличаются только цветом хода
if( p_bitboard->colour_move ) key = key ^ U64(0x7C0228DD624EC390);

// учитываем рокировки
if( p_bitboard->castling_K ) key = key ^ U64(0xB3598055CE64A656);
if( p_bitboard->castling_Q ) key = key ^ U64(0x12A8F886AF9418C2);
if( p_bitboard->castling_k ) key = key ^ U64(0x1877B67E57A764D5);
if( p_bitboard->castling_q ) key = key ^ U64(0xD4490AD346F14431);

// учитываем взятие на проходе
if( p_bitboard->taking_passage_yes ){
	key = key ^ U64(0xA3853B80F17F58EE);
     key = key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     key = key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//

p_bitboard->hash_key = key;

//cout << "p_bitboard->hash_key = " << p_bitboard->hash_key << "\n";
//cout << "   "<< "\n";

}//void HASHM::Start_position_random_key
//	*******************************************************************

//	===================================================================
// меняем ключ при прямом ходе
void HASHM::Modification_random_key(bool colour_figure,// цвет ходящей стороны
								    unsigned __int8 i,// номер хода по списку
								    struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
								    struct T_Bitboard * p_bitboard,// представление доски
                                    struct T_undo undo[]//информация для отката хода
								       ){

unsigned __int64 key; // скорректированный хеш-ключ
unsigned __int8 name_figure;// имя фигуры которой ходим
unsigned __int8 name_figure_capture;// имя фигуры которую побили
unsigned __int8  initial_position;// начальная координата хода
unsigned __int8  final_position;//   конечная координата хода
unsigned __int8  type_move;//   тип хода

key = p_bitboard->hash_key;
name_figure = ((p_list_surplus_moves->move[i]>>24)& 7);
initial_position = ((p_list_surplus_moves->move[i]>>12)& 63);
final_position = ((p_list_surplus_moves->move[i]>>18)& 63);
type_move = ((p_list_surplus_moves->move[i]>>6)& 63);
name_figure_capture = (p_list_surplus_moves->move[i] & 63);

// тут надо учесть тип хода
switch(type_move){

// ход на пустое поле------------------
case SIMPLE_MOVE :

    //от общего хеш-ключа позиции отнимаем ключ фигуры начальной координаты
    //и прибавляем ключ фигуры конечной координаты
    key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
    key = key ^ random_piese_key[colour_figure][name_figure][final_position];

break;

// взятие------------------------------
case CAPTURE_MOVE :

 key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
 key = key ^ random_piese_key[colour_figure][name_figure][final_position];
 key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

break;

//рокировка 0-0-0-----------------------
case LONG_CASTLING :
  key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
  key = key ^ random_piese_key[colour_figure][name_figure][final_position];

  if (colour_figure){
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][0];
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][3];
  }else{
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][56];
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][59];
  }//if (colour_figure)

break;

//рокировка 0-0-------------------------
case SHORT_CASTLING :
 key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
 key = key ^ random_piese_key[colour_figure][name_figure][final_position];

 if (colour_figure){
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][7];
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][5];
 }else{
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][63];
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][61];
 }//if (colour_figure)

break;

// взятие на проходе------------------
case TAKING_PASSAGE :

  key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
  key = key ^ random_piese_key[colour_figure][name_figure][final_position];

  if (colour_figure){
    key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][(final_position - 8)];
  }else{
    key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][(final_position + 8)];
  }//if (colour_figure)

break;

// ходы пешки с превращением------------------------------
case TRANSFORMATION_IN_KNIGHT : //превращение в конь 
case TRANSFORMATION_IN_BISHOP : //превращение в слон
case TRANSFORMATION_IN_ROOK   : //превращение в ладья 
case TRANSFORMATION_IN_QUEEN  : //превращение в ферзь 

    //
    //
    key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
    key = key ^ random_piese_key[colour_figure][(type_move - 10)][final_position];


break;

// взятие пешкой с превращением---------------------------------------------------------------------
case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT : //взятие с превращением в конь 
case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP : //взятие с превращением в слон
case CAPTURE_WITH_TRANSFORMATION_IN_ROOK   : //взятие с превращением в ладья 
case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN  : //взятие с превращением в ферзь 

    //
    //
    key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
    key = key ^ random_piese_key[colour_figure][(type_move - 20)][final_position];
    key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

break;


default:;//ход не распознан
   
}// switch(name){

// учитываем цвет хода (меняем на каждом ходе)
// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
// некоторые позиции отличаются только цветом хода
key = key ^ U64(0x7C0228DD624EC390);

// корректируем флаги рокировки
// идея такая что если флаг меняется то нужно подправить хеш-ключ
if( p_bitboard->castling_K != undo->return_castling_K) key = key ^ U64(0xB3598055CE64A656);//?
if( p_bitboard->castling_Q != undo->return_castling_Q) key = key ^ U64(0x12A8F886AF9418C2);
if( p_bitboard->castling_k != undo->return_castling_k) key = key ^ U64(0x1877B67E57A764D5);
if( p_bitboard->castling_q != undo->return_castling_q) key = key ^ U64(0xD4490AD346F14431);

//if( p_bitboard->castling_K != undo->return_castling_K) cout << "p_bitboard->castling_K = " << p_bitboard->castling_K 
//<< "undo->return_castling_K = " << undo->return_castling_K<< "\n";
//if( p_bitboard->castling_Q != undo->return_castling_Q) cout << "p_bitboard->castling_Q = " << p_bitboard->castling_Q 
//<< "undo->return_castling_Q = " << undo->return_castling_Q<< "\n";
//if( p_bitboard->castling_k != undo->return_castling_k)  cout << "p_bitboard->castling_k = " << p_bitboard->castling_k 
//<< "undo->return_castling_k = " << undo->return_castling_k<< "\n";
//if( p_bitboard->castling_q != undo->return_castling_q)  cout << "p_bitboard->castling_q = " << p_bitboard->castling_q 
//<< "undo->return_castling_q = " << undo->return_castling_q<< "\n";

// учитываем взятие на проходе
// если оно было до хода значит его нужно убрать
if( undo->return_taking_passage){
	key = key ^ U64(0xA3853B80F17F58EE);
     key = key ^ (undo->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     key = key ^ (undo->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//


// учитываем взятие на проходе
// если оно появилось после хода значит его нужно добавить
if( p_bitboard->taking_passage_yes){
	key = key ^ U64(0xA3853B80F17F58EE);
     key = key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     key = key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//

p_bitboard->hash_key = key;

}//void HASHM::Modification_up_random_key
//	*******************************************************************




// ПОВТОРЫ ПОЗИЦИЙ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// инициализируем хеш-ключи повторов возрастающими числами что бы исключить ложное срабатывание
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры в текущем варианте
void HASHM::Ini_hash_three(){
  for(unsigned __int8 i = 0; i < 120 ; i++){
     hash_three_moves[i] = i;
  }//
}//void HASHM::Ini_hash_moves
//	*******************************************************************

//	===================================================================
// инициализируем хеш-ключи повторов возрастающими числами что бы исключить ложное срабатывание
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры переданной из оболочки
void HASHM::Ini_hash_string_moves(){
  for(unsigned __int16 i = 0; i < 1000 ; i++){
     hash_string_moves[i] = i;
  }//
}//void HASHM::Ini_hash_moves
//	*******************************************************************

//	===================================================================
// запоминаем хеш-ключ на текущей глубине
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры в текущем варианте
void HASHM::Save_hash_three(unsigned __int64 key,// хеш-ключ(hash-key) 
					        unsigned __int8 depth // глубина
					        ){
  hash_three_moves[depth] = key;

}//void HASHM::Insert_hash_moves
//	*******************************************************************

//	===================================================================
// запоминаем хеш-ключ на текущей глубине
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры переданной из оболочки
void HASHM::Save_hash_string_moves(unsigned __int64 key,// хеш-ключ
					               unsigned __int16 cnt_move // счетчик ходов в строке партии
					               ){
  hash_string_moves[cnt_move] = key;
  save_cnt_move = cnt_move;

}//void HASHM::Insert_hash_moves
//	*******************************************************************

//	===================================================================
// анализируем нисходящую ветку ключей. если видим совпадение то возвращаем 1 иначе 0
// (используем для поиска повторов позиций для определения ничьей)
bool HASHM::Analysis_hash_string_moves(unsigned __int8 depth // глубина
								       ){

// TEST--------------------------------------------------------
//if (depth == 2){
//
//// TEST-------------
//cout << "depth= "
//     << depth
//     <<"\n";
//for (int i = 0; i <= depth; i++ ){
//cout << "i= "
//     << i
//     << " hash_three_moves[i]= "
//     << hash_three_moves[i]
//     <<"\n";
//}//for (int i = 0; i <= save_cnt_move; i++ )
//// TEST-------------
//
//
//// TEST-------------
//for (int i = 0; i <= save_cnt_move; i++ ){
//cout << "i= "
//     << i
//     << " hash_string_moves[i]= "
//     << hash_string_moves[i]
//     <<"\n";
//}//for (int i = 0; i <= save_cnt_move; i++ )
//// TEST-------------
//
//}//if (depth == 4){
// TEST--------------------------------------------------------

/*
работаем с правилом ничьей при трехкратном повторе позиции

если глубина погружения 4 и выше то ищем повтор в ветке варианта
если меньше то ищем повтор в строке игры переданной оболочкой.

достаточно одного совпадения позиции что бы оценка стала ничейной
таким образом присваиваем ноль уже при двухкратном повторе
меня мучают смутные подозрения что тут я сильно упрощаю :)))

*/

if (depth > 3){

    for (unsigned __int8 j = 4; (depth - j) >= 0; j = j + 2 ){

	    if (hash_three_moves[depth] == hash_three_moves[depth - j]){


       // // TEST--------------------------------------------------------
       //      cout << " DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD "
       //           << "\n";
       //      cout << "depth= "
       //           << depth
       //           <<"\n";
       //      
       //      cout << "j= "
       //           << j
       //           <<"\n";
       //      // TEST-------------
       //      for (int i = 0; i <= depth; i++ ){
       //           cout << "i= "
       //                << i
       //                << " hash_three_moves[i]= "
       //                << hash_three_moves[i]
       //                <<"\n";
       //      }//for (int i = 0; i <= depth; i++ )
       //      // TEST-------------
       //// TEST--------------------------------------------------------

		     return 1;

		}//if (hash_three_moves[depth] == hash_three_moves[depth - j])

    }// for (int j = 4; (depth - j) >= 0; j = j + 2 )


}else{// if (depth > 3)

   for (__int32 k = 4; (depth - k + save_cnt_move) >= 0; k = k + 2 ){

     if (hash_three_moves[depth] == hash_string_moves[ depth - k + save_cnt_move ] ){

     //// TEST--------------------------------------------------------
     //    cout << " CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC "
     //    << "\n";
     //    cout << "depth= "
     //    << depth
     //    <<"\n";
     //    // TEST-------------
     //    for (int i = 0; i <= depth; i++ ){
     //         cout << "i= "
     //              << i
     //              << " hash_three_moves[i]= "
     //              << hash_three_moves[i]
     //              <<"\n";
     //    }//for (int i = 0; i <= depth; i++ )
     //    cout << "  "
     //    << "\n";
     //    for (int i = 0; i <= save_cnt_move; i++ ){
     //        cout << "i= "
     //             << i
     //             << " hash_string_moves[i]= "
     //             << hash_string_moves[i]
     //             <<"\n";
     //    }//for (int i = 0; i <= save_cnt_move; i++ )
     //    // TEST-------------
     //  // TEST--------------------------------------------------------

		return 1;

	 }//if (hash_three_moves[depth] == hash_string_moves[ depth - k + save_cnt_move ] )

	}//for (int j = 4; (depth - j + save_cnt_move) >= 0; j = j + 2 )
		

}//if (depth > 4){

return 0;

}//HASHM::Analysis_hash_string_moves

//	*******************************************************************

//	===================================================================
// // возвращаем количество ходов в строке игры переданной из оболочки
__int32 HASHM::get_save_cnt_move(){
	return save_cnt_move;
}//int FULLSEARCH::get_from
//	*******************************************************************







//	===================================================================
// устарела
void HASHM::Print_hash_moves(){
//ofstream Test_Loop;
//int key;
//  Test_Loop.open ("_TEST_LOOP.txt",ios::app);
//
////for(int hm=1 ; hm<1000 ; hm++){
////   Test_Loop << " p_hash_moves[ " << hm << "].key= " << p_hash_moves[hm].key  << "\n";
//////  Test_Loop << p_hash_moves[hm] << "\n";
////}
//Test_Loop << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< "\n";
//key=574582;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=574484;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=563472;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=585502;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//Test_Loop.close();

}//void HASHM::Print_hash_moves
//	*******************************************************************