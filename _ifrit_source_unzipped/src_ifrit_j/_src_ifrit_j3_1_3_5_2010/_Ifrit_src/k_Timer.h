#include <time.h>
//---------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 25.02.2009    5.12.2009
*/
//---------------------------------------------------------------------------

/*
 ������ ������� �� ������ � ��������
 ����������� ������� �����
*/

namespace TIMER{

//////////////////////////////////////////////////////////////////////////////////////////
//public:

//---------------------------------------------------------------------------
// ��������� ��������� �����
void Time_start();

//-----------------------------------------------------------------
// ��������� ������� �����
void Time_stop();

//-----------------------------------------------------------------
// ��������� ��������� ����� (��������� � ��������)
double Time_delta();

};//namespace TIMER
