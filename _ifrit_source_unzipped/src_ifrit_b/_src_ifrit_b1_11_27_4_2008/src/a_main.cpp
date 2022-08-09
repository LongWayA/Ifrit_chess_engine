#include <iostream.h>

#include "b_Protokol_UCI.h"
#include "g_Test_Loop.h"
#include "j_Hash.h"

#include "d_Bitboard_gen.h"
//-----------------------------------------------------------------------------------
/*++
 * принимаем UCI команды от графической оболочки
 * в рамках консольного ввода вывода
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.09.2007
 *
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
*/
//-----------------------------------------------------------------------------------

int main(int argc, char * argv[]) {
		
 int test_perft=0;//(1-0)режим тестирования полного перебора (не стоит запускать такой екзешник в оболочке :)))
 // в файле g_Loop.h отключаем все эвристики
 // тест выводится в файл TEST_LOOP_PERFT.txt

 char string_in[65536];// буфер куда помещаем строку из потока

 char three_position[65536];// помещаем строку для анализа трехкратного повторения позиции
 T_Bitboard bitboard={0};//внутренняя битовая доска движка 

 int nodes=0;// количество рассмотренных узлов(позиций)

// отключаем буфферизацию
 cin.rdbuf()->setbuf(NULL,0);
 cout.rdbuf()->setbuf(NULL,0);

Ini_random_piese_key();// инициализируем хеш ключ

 // тестируем полный перебор(не забываем что нужно включить режим полного перебора (см. вверху)
 // и отключить все эвристики в файле g_Loop.h)
 if(test_perft){ 
  Test_Loop_Perft();
  return 0;
 }//if(Test_perft){ 

// для теста движка прямо в студии
//char string_in_1[] = "position fen 6k1/p4np1/1p2p2p/8/2r5/b2N2P1/qp2B1KP/1R3Q2 w - - 0 37";
////char string_in_1[] = "position fen rnbqkbnr/qqqqqqqq/8/8/8/8/QQQQQQQQ/RNBQKBNR w KQkq - 0 1";
//Parse_Protokol_UCI(&bitboard,string_in_1,three_position,nodes);
//char string_in_2[] = "go depth 10";
//Parse_Protokol_UCI(&bitboard,string_in_2,three_position,nodes);

 // слушаем входной консольный поток 
 while (cin.getline(string_in,65536,'\n')){
// обрабатываем UCI команды от оболочки
   Parse_Protokol_UCI(&bitboard,string_in,three_position,nodes);
 }//while (cin.getline(string_in,65536,'\n')
 return 0;
}//main(int argc, char * argv[])

