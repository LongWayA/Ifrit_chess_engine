#include <cstring>      // ���������� ��� ������ �� ������� (��������� �����)
#include <fstream.h>    // TEST
#include <stdlib.h>     // ������������ ���������� ������ �������� exit(1);

#include "b_Protokol_UCI.h"
#include "b_FEN.h"
#include "c_Go.h"
#include "d_Bitboard_gen.h"

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
 ofstream Test_FEN;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//-----------------------------------------------------------------------------------
/* ++
 * ������������ ���������� �� ����������� �������� � ������ ��������� UCI
 *
 * ����� ChessTerminator75
 * ��������� ��������� 12.06.2007
 *
 *  ������������
 * �������� ���������� ������� � ���� _TEST_FEN.txt
 * �������� �������� ��� ������ � ���������� ����� ������� ������ �������
 * ��������� ������ c_FEN 
 * ����� ����� �������� �������� ������������ ������������� ���������� �����
 * ������ �� �������� UCI �������
 */
//---------------------------------------------------------------------------

//	===================================================================
// ��������� �������� UCI ������� �� ����������� ��������� ��������
// ��������� :�����, �������� ������, ������ �������� �����,���������� ������� 
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
  cout << "id name Ifrit_b2_1 Debug"<< "\n";
  cout << "id author ChessTerminator75"<< "\n";
  cout <<"uciok \n";

 } else if (string_equal(string_in,"isready")) {
// �������� ��� � ������ ������
    cout << "readyok \n" << "\n";

 } else if (string_start_with(string_in,"position ")) {
// ��������� �������� ������� ������������� ���������� ����� ������   
    parse_position(bitboard,string_in); 
// �������� ��������� ����� � UCI ������� ������������� �������� �������
// ���������� ��� ���������� ������� ���� ��������
    parse_three_position(string_in,three_position);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// ������� ���������� ����� ������� ��� ������ �������
#if TEST_P
	Test_FEN.open ("_TEST_FEN.txt",ios::app);
    Test_FEN<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
    Test_FEN<<string_in<< "\n";
    Test_FEN<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
    Bitboard_print(bitboard);
    Test_FEN.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

 } else if (string_start_with(string_in,"go")) {

 // ��������� ������ �� ������ �������
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// ������� ���������� ����� ������� ��� ������ �������
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
// ������� ���������� ������ ����������
     exit(1);

//-----------------------------------------------------------------------------------------
// ��� �� ����������
   } else if (string_equal(string_in,"ucinewgame")) { 
                                          //������ ���� �������������
   } else if (string_start_with(string_in,"setoption ")) {
// � ������ ������ ��� ��� �� �����
   } else if (string_start_with(string_in,"debug ")) {
// ����� ������� �� ���� ��� ������������ ����� � ����� ����� ��� ������ ������� ������
      // dummy
   } else if (string_equal(string_in,"ponderhit")) {
// �� ���� ��� ���                                                                         
   } else if (string_equal(string_in,"stop")) {
// ������� ���� ������������ ���� � ������ �����                                                                         
   } else {
//���� ������ �� ����������
   }//if (false) {
//---------------------------------------------------------------------------

}//Parse_Protokol_UCI(char string_in[],char string_out[])
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������� ���������
static bool string_equal(const char s1[], const char s2[]) {
//������� ���������� ����� �� ������ ������ ������� ������������?
   return strcmp(s1,s2) == 0;
}
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������ ������� ���������
static bool string_start_with(const char s1[], const char s2[]) {
// ������ ��������� �� ������ ��������� ������ ������ � ������
// �� � ���� ��� ��������� ������ �� ����� �� ��� ����� � ����� ������
   return strstr(s1,s2) == s1;
}
//	*******************************************************************







