#include <iostream.h>

#include "b_Protokol_UCI.h"
#include "g_Test_Loop.h"
#include "j_Hash.h"

#include "d_Bitboard_gen.h"
//-----------------------------------------------------------------------------------
/*++
 * ��������� UCI ������� �� ����������� ��������
 * � ������ ����������� ����� ������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 22.09.2007
 *
 *  ������������
 * ���������������� ������ ���
*/
//-----------------------------------------------------------------------------------

int main(int argc, char * argv[]) {
		
 int test_perft=0;//(1-0)����� ������������ ������� �������� (�� ����� ��������� ����� �������� � �������� :)))
 // ��������� � ����� g_Loop.h 
 // COMPILING_1 ������ � 1 � ��������  ���� ������ ���������� �����
 // ���� ��������� � ���� TEST_LOOP_PERFT.txt

 char string_in[65536];// ����� ���� �������� ������ �� ������
 char three_position[65536];// �������� ������ ��� ������� ������������ ���������� �������
 T_Bitboard bitboard={0};//���������� ������� ����� ������ 

 int nodes=0;

// ��������� ������������
 cin.rdbuf()->setbuf(NULL,0);
 cout.rdbuf()->setbuf(NULL,0);

//// ��������� ������������
//// modern C++ demands this format:
//cout.rdbuf()->pubsetbuf(NULL,0);
//cin.rdbuf()->pubsetbuf(NULL,0);

//Bitboard_ini_t(&bitboard);

Ini_random_piese_key();
 // ��������� ������ �������(�� �������� ��� COMPILING_1 ������ � 1)
 if(test_perft){ 
  Test_Loop_Perft();
  return 0;
 }//if(Test_perft){ 

 while (cin.getline(string_in,65536,'\n')){
// ������������ UCI ������� �� ��������
   Parse_Protokol_UCI(&bitboard,string_in,three_position,nodes);
 }//while (cin.getline(string_in,65536,'\n')
 return 0;
}//main(int argc, char * argv[])