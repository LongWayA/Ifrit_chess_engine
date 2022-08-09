#include <iostream>
using namespace std;

#include "a_Structure.h"
#include "b_Protocol_UCI.h"
#include "j_Hash.h"

//-----------------------------------------------------------------------------------
/*++
 * принимаем UCI команды от графической оболочки
 * в рамках консольного ввода вывода
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.09.2007 11.11.2009
 *
*/
//-----------------------------------------------------------------------------------

/*******************************************
организуем опрос входного консольного потока
что бы не было задержек при вводе-выводе отключаем буферизацию
программа реагирует только на команды в рамках UCI протокола

********************************************/

#define LENGTH_STRING 65536 // задаем длину буфера потока

// глобальные переменные
// global
char string_in[LENGTH_STRING];// буфер куда помещаем строку из потока


int main(int argc, char * argv[]) {
	
struct T_Bitboard bitboard = {0};//внутренняя битовая доска движка 

// отключаем буферизацию
cout.rdbuf()->pubsetbuf(NULL,0);
cin.rdbuf()->pubsetbuf(NULL,0);

//--------------------------------------------
// TEST 
// для теста движка прямо в студии
//char string_in_1[] = "uci";
//Parse_Protokol_UCI(&bitboard,string_in_1,three_position,nodes);
////char string_in_2[] = "option name Hash type spin default 32 min 1 max 1024";
////Parse_Protokol_UCI(&bitboard,string_in_2,three_position,nodes);
//char string_in_3[] = "position startpos moves e2e4 e7e5 g1f3 g8f6 d2d4 e5d4 e4e5 f6g8 f3d4 d8e7 d1e2 d7d6 f2f4 d6e5 f4e5 e7h4 e2f2 h4f2 e1f2 b8d7 e5e6 d7b6 f1b5 c7c6 d4c6 c8e6 c6a7 b6d7 b5d7 e8d7 h1d1 d7e7 c1e3 g8h6 f2g1 h6f5 ... e3c5";
////char string_in_3[] = "position fen rnbqkbnr/qqqqqqqq/8/8/8/8/QQQQQQQQ/RNBQKBNR w KQkq - 0 1";
//Parse_Protokol_UCI(&bitboard,string_in_3,three_position,nodes);
////char string_in_4[] = "go wtime 1 btime 13333";
//char string_in_4[] = "go depth 1";
//Parse_Protokol_UCI(&bitboard,string_in_4,three_position,nodes);
//--------------------------------------------

// слушаем входной консольный поток 
while(cin.getline(string_in, LENGTH_STRING, '\n')){

	 // смотрим что же мы считали из выходного потока
     //cout << "--------------------" <<"\n";
     //cout << string_in <<"\n";

     // обрабатываем UCI команды от оболочки
	PROTOCOLUCI::Parse_Protokol_UCI(&bitboard,string_in);

}//while (cin.getline(string_in, LENGTH_STRING, '\n')

  return 0;

}//main(int argc, char * argv[])

