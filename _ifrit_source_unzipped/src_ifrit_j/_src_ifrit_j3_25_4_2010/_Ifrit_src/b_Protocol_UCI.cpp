#include <iostream>
#include <cstring>      // используем для работы со строкой (сравнение строк)
#include <fstream>    // TEST
#include <stdlib.h>     // обеспечиваем завершение потока функцией exit(1);
using namespace std;

#include "b_Protocol_UCI.h"
#include "b_FEN.h"
#include "c_Go.h"
#include "d_Bitboard_gen.h"
#include "j_Hash.h"
#include "h_Evaluate.h"

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
 ofstream Test_FEN;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------------------------------------------
/* ++
 * обрабатываем информацию от графической оболочки в рамках протокола UCI
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 12.06.2007  11.11.2009
 *
 *  ТЕСТИРОВАНИЕ
 * тестовую информацию выводим в файл _TEST_FEN.txt
 * печатаем входящую фен строку и полученную после разбора строки позицию
 * тестируем модуль c_FEN 
 * целью теста является контроль правильности инициализации внутренней доски
 * движка по входящей UCI позиции
 */
//---------------------------------------------------------------------------

// global

// имя и дата выпуска версии
#define NAME_OF_PROGRAM_AND_DATA_OF_RELEASE "id name Ifrit_j3_0_Beta_25_4_2010"


//	===================================================================
// разбираем входящие UCI команды от графической шахматной оболочки
void PROTOCOLUCI::Parse_Protokol_UCI(struct T_Bitboard * p_bitboard,//доска 
						             char string_in[]//входящая строка
						             ){

unsigned __int16 hash_size_Mb ;// размер хеш-таблицы в мегабайтах 

	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
Test_FEN.open ("_TEST_FEN.txt",ios::app);
    Test_FEN<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";
    Test_FEN<<string_in<< "\n";
    Test_FEN<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";  
    Test_FEN.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// выдается один раз при первом запуске движка
// в арене выглядит так: 
// >1:uci
if(string_equal(string_in,"uci")) {

   cout << NAME_OF_PROGRAM_AND_DATA_OF_RELEASE << "\n";
   cout << "id author ChessTerminator75" << "\n";

//  cout << "option name Ponder type check default false"<< "\n";

   // задаем диапазон размера кеша и размер по умолчанию  в Мб
   cout << "option name Hash type spin default 1 min 1 max 1024" << "\n";

//  cout << "option name NalimovCache type spin min 1 max 16 default 2"<< "\n";
//  cout << "option name NalimovPath type string default <empty>"<< "\n";
//  cout << "option name MultiPV type spin min 1 max 20 default 1"<< "\n";
//  cout << "option name OwnBook type check default true"<< "\n"; 


// инициализируем массив хеш - ключей фигур
/* делаем это один раз 
   вообще по уму нужно заранее посчитать его в таблицу и озаботиться тем
   что бы не было повторных значений ключа а пока мы его генерируем в данном месте
*/
    HASHM::Ini_random_piese_key();

//--------------------------------------------------------------------------
    // на всякий случай выделаем память под хеш-таблицу(по умолчанию 1 Мб)
    //(если вдруг не пройдет команда setoption)
	// и ведь пригодилась! дело в том что когда задаешь 1 мб то арена не выдает команды setoption
    HASHM::Hash_size_ini_default(1);

    // инициализируем все ключи в хеш-таблице 0
    HASHM::Ini_hash_moves();
//--------------------------------------------------------------------------

// возраст партии в 0
HASHM::Set_age_0();

    // да нам известен  uci протокол :)
    cout << "uciok \n";



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// видимо выдается при каждой смене настроек движка в том числе и размера хеша
// в арене выглядит так: 
// >1:setoption name Hash value 2 (если размер таблицы задали в 2 мб)
}else if(string_start_with(string_in,"setoption ")) {

/*
если считанный из входящей строки размер хеш-таблицы 
удалось перевести в число(Hash_size(string_in) != -1) 
то запоминаем его и выделяем память под хеш-таблицу
*/

   // TEST
 //  cout << "Read_hash_size_Mb(string_in) = " << Read_hash_size_Mb(string_in) << "\n";


   if(Read_hash_size_Mb(string_in) != 0) {

        // считываем установленный размер хеш-таблицы в Mb
		hash_size_Mb = Read_hash_size_Mb(string_in);

        //выделяем память под хеш-таблицу
        HASHM::Hash_size_resize(hash_size_Mb);

        // инициализируем все ключи в хеш-таблице 0  
        HASHM::Ini_hash_moves();

    }//if(Read_hash_size(string_in) != -1) {

/*
> setoption name Ponder value true
> setoption name Hash value 16
> setoption name NalimovCache value 4
> setoption name NalimovPath value D:\tbs
> setoption name Position Learning value true
> setoption name BookFile value dbbook.bin
> setoption name Book Size value Big
> setoption name Selectivity value 5
*/



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// в арене выдается при каждой новой игре а вот во фрице похоже нет
// в арене выглядит так: 
// >1:isready
}else if(string_equal(string_in,"isready")) {

    // передаем что к работе готовы
    cout << "readyok \n" << "\n";




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// выдается перед каждым ходом. тут передается позиция
// в арене выглядит так: 
// >1:position startpos (для стартовой позиции)
// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (для текущей партии)
// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1 (для произвольной позиции)
}else if(string_start_with(string_in,"position ")) {

//  разбираем входящую позицию инициализируем внутреннюю доску движка   
	FEN::parse_position(p_bitboard,string_in); 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  выводим полученную после разбора фен строки позицию
    #if TEST_P
	   Test_FEN.open ("_TEST_FEN.txt",ios::app);
       Test_FEN<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
       Test_FEN<<string_in<< "\n";
       Test_FEN<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
       Bitboard_print(bitboard);
       Test_FEN.close();
    #endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// команда на старт обсчета позиции
// в арене выглядит так: 
// >1:go depth 5 (для счета на заданную глубину - в данном случае 5 полуходов )
// >1:go wtime 300000 btime 300000 (блиц 5 мин - первый ход)
// >1:go movetime 5000 ( 5 секунд на ход)
// >1:go infinite (анализ позиции)
}else if(string_start_with(string_in,"go")) {

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  выводим полученную после разбора фен строки позицию
    #if TEST_P
	   Test_FEN.open ("_TEST_FEN.txt",ios::app);
       Test_FEN<<"/////////////////////////////////////////////////////////"<< "\n";
       Test_FEN<<string_in<< "\n";
       Test_FEN<<"/////////////////////////////////////////////////////////"<< "\n";
    
       Test_FEN.close();
    #endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//  запускаем движок на расчет позиции
	   GO::parse_go(p_bitboard,string_in);

// команда на выход
// в арене выглядит так: 
// >1:quit
}else if(string_equal(string_in,"quit")) {
	// освобождаем память из под хеш-таблицы
    HASHM::Hash_size_free();

//  команда завершения потока выполнения
    exit(1);




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------
// еще не реализовал
}else if(string_equal(string_in,"ucinewgame")) { 
                                          //должна быть инициализация
      // возраст партии в 0
      HASHM::Set_age_0();
      // инициализируем все ключи в хеш-таблице 0  
      HASHM::Ini_hash_moves();

}else if(string_start_with(string_in,"debug ")) {
//   режим отладки не знаю как использовать может и смогу нужен для лучшей отладки движка
  // dummy

}else if(string_equal(string_in,"ponderhit")) {
// не знаю что это

// в арене выглядит так: 
//>1:stop
}else if(string_equal(string_in,"stop")) {
// команда стоп реализована мною в другом месте  

}else {
//если ничего не распознали
     cout << "WARNING ! Unknown command" << "\n";

}//if(string_equal(string_in,"uci")) {

}//void PROTOCOLUCI::Parse_Protokol_UCI
//	*******************************************************************

//	===================================================================
// считывает в подстроке вида "Hash value 32" размер хеш-таблицы (в данном случае 32)
unsigned __int16 PROTOCOLUCI::Read_hash_size_Mb(const char string_in[]) {
const char * hash_value; 

// находим указатель на подстроку Hash value
hash_value = strstr(string_in,"Hash value ");

if (hash_value != NULL){
// пропускаем надпись Hash value и выходим на число(размер хеш-таблицы)
    hash_value = hash_value + 11;
    return atoi(hash_value);// функция atoi переводит строку в целое число;
}//if (hash_value != NULL){

return 0;
}//int PROTOCOLUCI::Read_hash_size
//	*******************************************************************

//	===================================================================
// есть ли в строке искомая подстрока
bool PROTOCOLUCI::string_equal(const char s1[], const char s2[]) {

//функция показывает равны ли строки видимо уровень соответствия?
   return strcmp(s1,s2) == 0;
}//bool PROTOCOLUCI::string_equal
//	*******************************************************************

//	===================================================================
// есть ли в начале строки искомая подстрока
bool PROTOCOLUCI::string_start_with(const char s1[], const char s2[]) {

// выдает указатель на первое вхождение второй строки в первую
// ну и если они совпадают значит мы имеем то что нужно в самом начале
   return strstr(s1,s2) == s1;
}//bool PROTOCOLUCI::string_start_with
//	*******************************************************************



