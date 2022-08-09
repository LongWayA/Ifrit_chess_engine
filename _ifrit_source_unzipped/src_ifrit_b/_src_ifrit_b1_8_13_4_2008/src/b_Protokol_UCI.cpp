#include <cstring>      // используем для работы со строкой (сравнение строк)
#include <fstream.h>    // TEST
#include <stdlib.h>     // обеспечиваем завершение потока функцией exit(1);

#include "b_Protokol_UCI.h"
#include "b_FEN.h"
#include "c_Go.h"


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
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 12.06.2007
 *
 *  ТЕСТИРОВАНИЕ
 * тестовую информащию выводим в файл _TEST_FEN.txt
 * печатаем входящую фен строку и полученную после разбора строки позицию
 * тестируем модуль c_FEN 
 * целью теста является контроль правильности инициализации внутренней доски
 * движка по входящей UCI позиции
 */
//---------------------------------------------------------------------------

//	===================================================================
// разбираем входящие UCI команды от графической шахматной оболочки
// аргументы :доска, входящая строка, строка повторов ходов,количество позиций 
void Parse_Protokol_UCI(T_Bitboard * bitboard,const char string_in[],char three_position[],int & nodes){
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
	Test_FEN.open ("_TEST_FEN.txt",ios::app);
    Test_FEN<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";
    Test_FEN<<string_in<< "\n";
    Test_FEN<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";  
    Test_FEN.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

 if (false) {
 } else if (string_equal(string_in,"uci")) {
  cout << "id name Ifrit_b1_8 Debug"<< "\n";
  cout << "id author ChessTerminator75"<< "\n";
  cout <<"uciok \n";

 } else if (string_equal(string_in,"isready")) {
// передаем что к работе готовы
    cout << "readyok \n" << "\n";

 } else if (string_start_with(string_in,"position ")) {
// разбираем входящую позицию инициализруем внутреннюю доску движка   
    parse_position(bitboard,string_in); 
// выделяем подстроку ходов в UCI команде инициализации входящей позиции
// используем для реализации правила трех повторов
    parse_three_position(string_in,three_position);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// выводим полученную после разбора фен строки позицию
#if TEST_P
	Test_FEN.open ("_TEST_FEN.txt",ios::app);
    Test_FEN<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
    Test_FEN<<string_in<< "\n";
    Test_FEN<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
    Test_FEN_Print_Board_816(board_list_816);
    Test_FEN.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

 } else if (string_start_with(string_in,"go")) {

 // запускаем движок на расчет позиции
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// выводим полученную после разбора фен строки позицию
#if TEST_P
	Test_FEN.open ("_TEST_FEN.txt",ios::app);
    Test_FEN<<"/////////////////////////////////////////////////////////"<< "\n";
    Test_FEN<<string_in<< "\n";
    Test_FEN<<"/////////////////////////////////////////////////////////"<< "\n";
    
    Test_FEN.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
    parse_go(bitboard,string_in,three_position,nodes);

   } else if (string_equal(string_in,"quit")) {
// команда завершения потока выполнения
     exit(1);

//-----------------------------------------------------------------------------------------
// еще не реализовал
   } else if (string_equal(string_in,"ucinewgame")) { 
                                          //должна быть инициализация
   } else if (string_start_with(string_in,"setoption ")) {
// в данный момент это нам не нужно
   } else if (string_start_with(string_in,"debug ")) {
// режим отладки не знаю как использовать может и смогу нужен для лучшей отладки движка
      // dummy
   } else if (string_equal(string_in,"ponderhit")) {
// не знаю что это                                                                         
   } else if (string_equal(string_in,"stop")) {
// команда стоп реализованна мною в другом месте                                                                         
   } else {
//если ничего не распознали
   }//if (false) {
//---------------------------------------------------------------------------

}//Parse_Protokol_UCI(char string_in[],char string_out[])
//	*******************************************************************

//	===================================================================
// есть ли в строке искомая подстрока
static bool string_equal(const char s1[], const char s2[]) {
//функция показывает равны ли строки видимо уровень соответствия?
   return strcmp(s1,s2) == 0;
}
//	*******************************************************************

//	===================================================================
// есть ли в начале строки искомая подстрока
static bool string_start_with(const char s1[], const char s2[]) {
// выдает указатель на первое вхождение второй строки в первую
// ну и если они совпадают значит мы имеем то что нужно в самом начале
   return strstr(s1,s2) == s1;
}
//	*******************************************************************

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем позицию полученную после разбора фен строки (УЖЕ НЕ АКТУАЛЬНА надо переписать)
#if TEST_P
void Test_FEN_Print_Board_816(T_board_list_816 * board_list_816){
Test_FEN << "================================"<<"\n";
Test_FEN << "\n";
//Test_FEN << "Доска с именами фигур 0-нет фигуры, 1-пешка, 2-ладья, 3-конь, 4-слон, 5-ферзь, 6-король"<<"\n";
//Test_FEN << "\n";
Test_FEN <<"Цвет хода= "<< board_list_816->colour_move << "\n";
Test_FEN << "\n";
int str=1;
for (int xv=0 ;xv<128;xv++){
  Test_FEN <<" " << board_list_816->name_figure_816[xv];
  if(str>15){
	Test_FEN << "\n";
    str=0;
   }//if(str>9){
str=str+1;
 }//for (int xv=1 ;xv<9;xv++){

Test_FEN << "\n";

str=1;

Test_FEN << "\n";
for (int xv=0 ;xv<128;xv++){
  Test_FEN <<" " << board_list_816->colour_figure_816[xv];
  if(str>15){
	Test_FEN << "\n";
    str=0;
   }//if(str>9){
str=str+1;
 }//for (int xv=1 ;xv<9;xv++){

Test_FEN << "\n";

 Test_FEN << "board_list_816->taking_passage_yes= " << board_list_816->taking_passage_yes << "\n";
 Test_FEN << "board_list_816->taking_passage_x= " << board_list_816->taking_passage_x << "\n";
 Test_FEN << "board_list_816->taking_passage_y= " << board_list_816->taking_passage_y << "\n";

Test_FEN << "\n";
 Test_FEN << "board_list_816->castling_K= " << board_list_816->castling_K << "\n";
 Test_FEN << "board_list_816->castling_Q= " << board_list_816->castling_Q << "\n";
 Test_FEN << "board_list_816->castling_k= " << board_list_816->castling_k << "\n";
 Test_FEN << "board_list_816->castling_q= " << board_list_816->castling_q << "\n";

Test_FEN << "================================"<<"\n";
Test_FEN << "\n";
}//void Board_816_Print(int Depth,T_board_list_816 * board_list_816){
#endif
//	*******************************************************************






