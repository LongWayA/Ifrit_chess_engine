#include <time.h>
//-----------------------------------------------------------------------------------
/* +
 * ������ ������� �� ������ � ��������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 25.02.2009
*/

//---------------------------------------------------------------------------

/*******************************************

   ����������� ������� �����

********************************************/
namespace TIMER{

//////////////////////////////////////////////////////////////////////////////////////////
//public:

//---------------------------------------------------------------------------
// ��������� ������	
void Timer_start();

//-----------------------------------------------------------------
// ������������� ������	
void Timer_stop();

//-----------------------------------------------------------------
// ��������� ��������� ����� (��������� � ��������)
double Timer_delta();

};//namespace TIMER
