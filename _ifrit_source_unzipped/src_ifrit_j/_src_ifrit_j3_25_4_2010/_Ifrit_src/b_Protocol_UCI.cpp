#include <iostream>
#include <cstring>      // ���������� ��� ������ �� ������� (��������� �����)
#include <fstream>    // TEST
#include <stdlib.h>     // ������������ ���������� ������ �������� exit(1);
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
 * ������������ ���������� �� ����������� �������� � ������ ��������� UCI
 *
 * ����� ChessTerminator75
 * ��������� ��������� 12.06.2007  11.11.2009
 *
 *  ������������
 * �������� ���������� ������� � ���� _TEST_FEN.txt
 * �������� �������� ��� ������ � ���������� ����� ������� ������ �������
 * ��������� ������ c_FEN 
 * ����� ����� �������� �������� ������������ ������������� ���������� �����
 * ������ �� �������� UCI �������
 */
//---------------------------------------------------------------------------

// global

// ��� � ���� ������� ������
#define NAME_OF_PROGRAM_AND_DATA_OF_RELEASE "id name Ifrit_j3_0_Beta_25_4_2010"


//	===================================================================
// ��������� �������� UCI ������� �� ����������� ��������� ��������
void PROTOCOLUCI::Parse_Protokol_UCI(struct T_Bitboard * p_bitboard,//����� 
						             char string_in[]//�������� ������
						             ){

unsigned __int16 hash_size_Mb ;// ������ ���-������� � ���������� 

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
// �������� ���� ��� ��� ������ ������� ������
// � ����� �������� ���: 
// >1:uci
if(string_equal(string_in,"uci")) {

   cout << NAME_OF_PROGRAM_AND_DATA_OF_RELEASE << "\n";
   cout << "id author ChessTerminator75" << "\n";

//  cout << "option name Ponder type check default false"<< "\n";

   // ������ �������� ������� ���� � ������ �� ���������  � ��
   cout << "option name Hash type spin default 1 min 1 max 1024" << "\n";

//  cout << "option name NalimovCache type spin min 1 max 16 default 2"<< "\n";
//  cout << "option name NalimovPath type string default <empty>"<< "\n";
//  cout << "option name MultiPV type spin min 1 max 20 default 1"<< "\n";
//  cout << "option name OwnBook type check default true"<< "\n"; 


// �������������� ������ ��� - ������ �����
/* ������ ��� ���� ��� 
   ������ �� ��� ����� ������� ��������� ��� � ������� � ����������� ���
   ��� �� �� ���� ��������� �������� ����� � ���� �� ��� ���������� � ������ �����
*/
    HASHM::Ini_random_piese_key();

//--------------------------------------------------------------------------
    // �� ������ ������ �������� ������ ��� ���-�������(�� ��������� 1 ��)
    //(���� ����� �� ������� ������� setoption)
	// � ���� �����������! ���� � ��� ��� ����� ������� 1 �� �� ����� �� ������ ������� setoption
    HASHM::Hash_size_ini_default(1);

    // �������������� ��� ����� � ���-������� 0
    HASHM::Ini_hash_moves();
//--------------------------------------------------------------------------

// ������� ������ � 0
HASHM::Set_age_0();

    // �� ��� ��������  uci �������� :)
    cout << "uciok \n";



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ������ �������� ��� ������ ����� �������� ������ � ��� ����� � ������� ����
// � ����� �������� ���: 
// >1:setoption name Hash value 2 (���� ������ ������� ������ � 2 ��)
}else if(string_start_with(string_in,"setoption ")) {

/*
���� ��������� �� �������� ������ ������ ���-������� 
������� ��������� � �����(Hash_size(string_in) != -1) 
�� ���������� ��� � �������� ������ ��� ���-�������
*/

   // TEST
 //  cout << "Read_hash_size_Mb(string_in) = " << Read_hash_size_Mb(string_in) << "\n";


   if(Read_hash_size_Mb(string_in) != 0) {

        // ��������� ������������� ������ ���-������� � Mb
		hash_size_Mb = Read_hash_size_Mb(string_in);

        //�������� ������ ��� ���-�������
        HASHM::Hash_size_resize(hash_size_Mb);

        // �������������� ��� ����� � ���-������� 0  
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
// � ����� �������� ��� ������ ����� ���� � ��� �� ����� ������ ���
// � ����� �������� ���: 
// >1:isready
}else if(string_equal(string_in,"isready")) {

    // �������� ��� � ������ ������
    cout << "readyok \n" << "\n";




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// �������� ����� ������ �����. ��� ���������� �������
// � ����� �������� ���: 
// >1:position startpos (��� ��������� �������)
// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (��� ������� ������)
// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1 (��� ������������ �������)
}else if(string_start_with(string_in,"position ")) {

//  ��������� �������� ������� �������������� ���������� ����� ������   
	FEN::parse_position(p_bitboard,string_in); 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  ������� ���������� ����� ������� ��� ������ �������
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
// ������� �� ����� ������� �������
// � ����� �������� ���: 
// >1:go depth 5 (��� ����� �� �������� ������� - � ������ ������ 5 ��������� )
// >1:go wtime 300000 btime 300000 (���� 5 ��� - ������ ���)
// >1:go movetime 5000 ( 5 ������ �� ���)
// >1:go infinite (������ �������)
}else if(string_start_with(string_in,"go")) {

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  ������� ���������� ����� ������� ��� ������ �������
    #if TEST_P
	   Test_FEN.open ("_TEST_FEN.txt",ios::app);
       Test_FEN<<"/////////////////////////////////////////////////////////"<< "\n";
       Test_FEN<<string_in<< "\n";
       Test_FEN<<"/////////////////////////////////////////////////////////"<< "\n";
    
       Test_FEN.close();
    #endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//  ��������� ������ �� ������ �������
	   GO::parse_go(p_bitboard,string_in);

// ������� �� �����
// � ����� �������� ���: 
// >1:quit
}else if(string_equal(string_in,"quit")) {
	// ����������� ������ �� ��� ���-�������
    HASHM::Hash_size_free();

//  ������� ���������� ������ ����������
    exit(1);




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------
// ��� �� ����������
}else if(string_equal(string_in,"ucinewgame")) { 
                                          //������ ���� �������������
      // ������� ������ � 0
      HASHM::Set_age_0();
      // �������������� ��� ����� � ���-������� 0  
      HASHM::Ini_hash_moves();

}else if(string_start_with(string_in,"debug ")) {
//   ����� ������� �� ���� ��� ������������ ����� � ����� ����� ��� ������ ������� ������
  // dummy

}else if(string_equal(string_in,"ponderhit")) {
// �� ���� ��� ���

// � ����� �������� ���: 
//>1:stop
}else if(string_equal(string_in,"stop")) {
// ������� ���� ����������� ���� � ������ �����  

}else {
//���� ������ �� ����������
     cout << "WARNING ! Unknown command" << "\n";

}//if(string_equal(string_in,"uci")) {

}//void PROTOCOLUCI::Parse_Protokol_UCI
//	*******************************************************************

//	===================================================================
// ��������� � ��������� ���� "Hash value 32" ������ ���-������� (� ������ ������ 32)
unsigned __int16 PROTOCOLUCI::Read_hash_size_Mb(const char string_in[]) {
const char * hash_value; 

// ������� ��������� �� ��������� Hash value
hash_value = strstr(string_in,"Hash value ");

if (hash_value != NULL){
// ���������� ������� Hash value � ������� �� �����(������ ���-�������)
    hash_value = hash_value + 11;
    return atoi(hash_value);// ������� atoi ��������� ������ � ����� �����;
}//if (hash_value != NULL){

return 0;
}//int PROTOCOLUCI::Read_hash_size
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������� ���������
bool PROTOCOLUCI::string_equal(const char s1[], const char s2[]) {

//������� ���������� ����� �� ������ ������ ������� ������������?
   return strcmp(s1,s2) == 0;
}//bool PROTOCOLUCI::string_equal
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������ ������� ���������
bool PROTOCOLUCI::string_start_with(const char s1[], const char s2[]) {

// ������ ��������� �� ������ ��������� ������ ������ � ������
// �� � ���� ��� ��������� ������ �� ����� �� ��� ����� � ����� ������
   return strstr(s1,s2) == s1;
}//bool PROTOCOLUCI::string_start_with
//	*******************************************************************



