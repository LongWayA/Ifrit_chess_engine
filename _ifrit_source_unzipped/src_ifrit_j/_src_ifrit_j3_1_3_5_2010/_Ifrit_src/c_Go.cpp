#include <iostream>
#include <fstream>
#include <cstring>      // ���������� ��� ������ �� ������� (��������� �����)
#include <stdlib.h>

#include "c_Go.h"
#include "g_Search.h"
#include "b_FEN.h"
#include "c_Iterative_Deepening.h"
#include "j_Hash.h"

//-----------------------------------------------------------------------------------
/*+
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009
*/
//-----------------------------------------------------------------------------------

/*

* ������������ ������� �� �����

// ��������� ������� go � ������������� ����� ����

 ������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
 > go btime 300000 wtime 300000 
 for game in 5min. 

 > go btime 302000 wtime 302000 winc 2000 binc 2000 
 with incremental clock 5 min + 2 sec. 

 ����� �� ������������ ���������� �����
 > go movestogo 40 wtime 300000 btime 300000 
 for 40 moves in 5 min. 
 ������ ����� ������ � ����
 > go wtime 300000 btime 300000  movestogo 40

 ��� ������� ��� ��� �������
 > go infinite 
 for analysing. 

 ����� �� ��� ������� ��� ���� �� ������������ � �� ����� ��� �� ������ ���� ���������
 > go movetime 300000 
 Exactly 5min for the next move, not supported by Fritz. 

����� �� �������� �������
> go depth 5 
*/

namespace {
  char string_in_l[LENGTH_STRING];//���������� � ������� parse_go
}//namespace {

//	===================================================================
void GO::parse_go(struct T_Bitboard * const p_bitboard,//�����
			      const char string_in[]//������ ������
			      ){

const char * ptr;// ��������������� ���������

//int mate = -1 ;
unsigned __int8 depth_max = 0;// ������� ��������
unsigned __int8 movestogo = 0;//���������� ����� �� ������������ �����

double wtime = -1.0;// ���������� ����� �� ���� �� �����
double btime = -1.0;// ���������� ����� �� ���� �� ������
double winc = -1.0;//  ���������� ������� �� ��� ��� �����
double binc = -1.0;//  ���������� ������� �� ��� ��� ������
double movetime = -1.0;// ����� �� ���

bool infinite = false;// ���� ������������ �������


// �������� ������ string_in � ������ string_in_l ��� ����� ������ ��� � ����������
// ������ ����� ��������� ��� �������
strcpy (string_in_l,string_in);

// ������� strtok �������� ��� �����������(����������� �� ���������) ������ ������ ��� ���� ��� ��������� ������
// ��� ������ ������ ������ ������ � ����� ��� ��� ���������� ���� 
ptr = strtok(string_in_l," "); // ���������� "go"
 
// ��� �� ����� �� ������ ������� ��������� ���������� ���������
for(ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {

   if (string_equal(ptr,"depth")) {//������� ��������
        ptr = strtok(NULL," ");// ��������� �� depth �������� ���������� ��������(����� �����)
        depth_max = atoi(ptr);// ������� atoi ��������� ������ � ����� �����

   }else if (string_equal(ptr,"wtime")) {// ����� ������� ����������� �����
		ptr = strtok(NULL," ");
        wtime = double(atoi(ptr)) / 1000.0;// ������������ ��������� � ������� (/ 1000.0)

   }else if (string_equal(ptr,"btime")) {// ����� ������� ����������� ������
        ptr = strtok(NULL," ");
        btime = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"winc")) {// ���������� ����� ��� �����
        ptr = strtok(NULL," ");
        winc = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"binc")) {// ���������� ����� ��� ������
        ptr = strtok(NULL," ");
        binc = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"movestogo")) {//���������� ����� �� ������� �������� �����
        ptr = strtok(NULL," ");
        movestogo = atoi(ptr);

   }else if (string_equal(ptr,"movetime")) {// ����� �� ���
        ptr = strtok(NULL," ");
        movetime = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"infinite")) {// ����������� ������
        infinite = true;

// ���� ������ �� ���� ----------------------------------------------
//  }else if (string_equal(ptr,"mate")) {
//         ptr = strtok(NULL," ");
//         mate = atoi(ptr);
//  }else if (string_equal(ptr,"nodes")) {

//         ptr = strtok(NULL," ");
//         nodes = my_atoll(ptr);

//  }else if (string_equal(ptr,"ponder")) {
//         ponder = true;

//  }else if (string_equal(ptr,"searchmoves")) {
//-------------------------------------------------------------------

    }//if (string_equal(ptr,"depth")) {

}//for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {

//------------------------------------------------------------------------------

ptr = NULL;

// ����������� ������� �������
HASHM::Aging();
//////////////////////////////////////////////////////////////////////////////////////////

// ����� ��� � ��������
// �������� ������� � ������� �������� chess_board, Depth_Max
//depth_max = depth_max - 1;
Deepening(p_bitboard,depth_max,wtime,btime,winc,binc,movestogo,movetime,infinite);

}//parse_go(char string_in[],int chess_board[]){
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������� ���������
bool GO::string_equal(const char s1[], const char s2[]) {
//������� ���������� ����� �� ������ ������ ������� ������������?
   return strcmp(s1,s2) == 0;
}
//	*******************************************************************

//	===================================================================
// ���� �� � ������ ������ ������� ���������
bool GO::string_start_with(const char s1[], const char s2[]) {
// ������ ��������� �� ������ ��������� ������ ������ � ������
// �� � ���� ��� ��������� ������ �� ����� �� ��� ����� � ����� ������
   return strstr(s1,s2) == s1;
}
//	*******************************************************************
