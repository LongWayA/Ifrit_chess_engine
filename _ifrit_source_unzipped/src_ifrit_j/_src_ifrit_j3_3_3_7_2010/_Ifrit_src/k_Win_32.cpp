/* Windows-version */
#include <windows.h>
#include <conio.h>


#include "k_Win_32.h"
//--------------------------------------------------------------------------
/* 
 * ����� ��� ��������� ����������� �� Glaurung121 by Tord Romstad
 * ������ 17.09.2007
 * ��������� ��������� 1.05.2010
*/
//---------------------------------------------------------------------------

// ������� ����������� � ���� Glaurung121 by Tord Romstad
// ���-������ �� ������������ � ����� :)

namespace 
{
	int init = 0;
	int pipe;
	HANDLE inh;
}

//	===================================================================
// ������������� � ��� ��� � ������ ��������� ����������
int public_bioskey(void)
{
	DWORD dw;

	/* If we're running under XBoard then we can't use _kbhit() as the input
	* commands are sent to us directly over the internal pipe */

#if defined(FILE_CNT)
	if (stdin->_cnt > 0)
	{
		return stdin->_cnt;
	}
#endif

	if (!init) 
	{
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);

        if (!pipe) 
		{
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}

    if (pipe) 
	{
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
		{		
			return 1;
		}
		return dw;
    } 
	else 
	{
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;
	}
}
