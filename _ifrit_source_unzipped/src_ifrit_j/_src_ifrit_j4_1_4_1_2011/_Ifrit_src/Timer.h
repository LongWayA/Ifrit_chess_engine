#include <time.h>

//---------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 25.02.2009    5.12.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

 ������ ������� �� ������ � ��������
 ����������� ������� �����
*/

class Timer
{

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////////
private:

	// ������ ��� ������ ������� �����
	clock_t start_timer; 
	clock_t finish_timer;


//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// ��������� ��������� �����
	void time_start();

	//-----------------------------------------------------------------
	// ��������� ������� �����
	void time_stop();

	//-----------------------------------------------------------------
	// ��������� ��������� ����� (��������� � ��������)
	double time_delta();

};