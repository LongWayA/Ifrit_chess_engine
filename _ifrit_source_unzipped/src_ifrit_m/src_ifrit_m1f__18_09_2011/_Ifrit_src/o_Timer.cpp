
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 25.02.2009 5.12.2009

#include <iostream>

#include "o_Timer.h"

//clock() - ������� ������������ ���������� ������������ ����� �� ������ ������� ���������
//CLOCKS_PER_SEC - ���������� ����� � ������� (� ���� ���� 1000)

//	===================================================================
// ��������� ��������� �����	
void o_Timer::time_start()
{
	start_timer = clock();
}

//	===================================================================
// ��������� ������� �����
void o_Timer::time_stop()
{
	finish_timer = clock();
}

//	===================================================================
// ��������� ��������� ����� (��������� � ��������)
double o_Timer::time_delta()
{
	double out = (double)(finish_timer - start_timer) / (double)CLOCKS_PER_SEC;

	if (out < 0.01) out = 0.01;

	// ��������� ����� �������
	//cout <<"start_timer = "<< start_timer <<"\n";
	//cout <<"finish_timer = "<< finish_timer <<"\n";
	//cout <<"CLOCKS_PER_SEC = "<< CLOCKS_PER_SEC <<"\n";
	//cout <<"((end_1-start_1) / CLOCKS_PER_SEC) = "<< out <<"\n";
	//cout <<"((double)(finish_timer - start_timer) / CLOCKS_PER_SEC) = "
	// << (double)((finish_timer - start_timer) / CLOCKS_PER_SEC) <<"\n";

	return out;
}

