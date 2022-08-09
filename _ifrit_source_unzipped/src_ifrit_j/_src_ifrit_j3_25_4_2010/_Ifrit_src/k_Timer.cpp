#include <iostream>
using namespace std;

#include "k_Timer.h"

//-----------------------------------------------------------------------------------
/* +
 * ������ ������� �� ������ � ��������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 25.02.2009    5.12.2009
 *
 *  ������������
 * ���������������� ������ ���
*/

//---------------------------------------------------------------------------


//clock() - ������� ������������ ���������� ������������ ����� �� ������ ������� ���������
//CLOCKS_PER_SEC - ���������� ����� � ������� (� ���� ���� 1000)

// global
  clock_t start_timer,finish_timer;// ������ ��� ������ ������� �����
  double out;// ����� ������� � ��������


//	===================================================================
// ��������� ������	
void TIMER::Timer_start(){

    start_timer = clock();

}//void TIMER::Timer_start
//	*******************************************************************

//	===================================================================
// ������������� ������	
void TIMER::Timer_stop(){

    finish_timer = clock();

}//void TIMER::Timer_stop
//	*******************************************************************

//	===================================================================
// ��������� ��������� ����� (��������� � ��������)
double TIMER::Timer_delta(){

    out = (double)(finish_timer - start_timer) / (double)CLOCKS_PER_SEC;

	if (out < 0.01) out = 0.01;

 // ��������� ����� �������
  //cout <<"start_timer = "<< start_timer <<"\n";
  //cout <<"finish_timer = "<< finish_timer <<"\n";
  //cout <<"CLOCKS_PER_SEC = "<< CLOCKS_PER_SEC <<"\n";
  //cout <<"((end_1-start_1) / CLOCKS_PER_SEC) = "<< out <<"\n";
  //cout <<"((double)(finish_timer - start_timer) / CLOCKS_PER_SEC) = "
	 // << (double)((finish_timer - start_timer) / CLOCKS_PER_SEC) <<"\n";

    return out;

}//double TIMER::Timer_delta
//	*******************************************************************
