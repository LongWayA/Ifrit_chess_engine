#include "a_Structure.h"
#include "a_Chess_bitboard.h"

#define TEST_D 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������

#define TEST_WRITE_READ_WN 0 // ��������-���������(1-0) ����� �������� ����������
                             // ���������� � ��������� �������
                             // ���� ��� �������� ���� TEST_WRITE_READ � j_Hash.h


//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 10.02.2008  11.11.2009
*/
//---------------------------------------------------------------------------


/*
 ������� ����� ������� 70%

������� � ����������� ����������� �������

���������� ���������� ���������� ������� �� ��� ��� ���� �� ��������� �������.
�.�. � ������ �� ������� 1 ����� 2 � �.�.
��� ����� �� ������ ��� ���������� ���-�������
�� ������ ��� �� ����������� ����� ��������. �������� ���� �� �� ���������� �������
������� ������� �� �� ��������� � ������� � �������� ��� ������.


������������ ��������� ���������� ������� ����

> go depth 5 
����� �� �������� �������
��� ������ ��������� �� �������� ������� depth_max ���������� ���������

 > go btime 300000 wtime 300000 
 for game in 5min. 
 > go btime 302000 wtime 302000 winc 2000 binc 2000 
 with incremental clock 5 min + 2 sec. 
������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
��� ������ ����� ����� ��������� time_terminate ���������� ���������
������ ���� ������� ����������� time_terminate = wtime/100;
�.�. �� ����� ���������� ����� ��� ���� �� ������ � ����� �� ���.
���������� ����� �� ������� �� ������ ���� ������� � �������� ��� � �������

> go infinite
��� ������ ������� infinite ������ ������ depth_max ������ 50. ������� ����� � �������
�������� ����� ������ ������� quit ��� stop

> go movetime 300000 
������� ������� �� ��� ���� ����� ������� time_terminate = movetime;


 > go movestogo 40 wtime 300000 btime 300000 
 for 40 moves in 5 min. 
����� �� ������������ ���������� �����
�� �������������� ��� ������� ����� ���������� ���
> go wtime 300000 btime 300000 
������ ���� ������ �� �������� �� � ������� ���� ����� �������

� ����� ������� �������� ������� � ������ ���

*/

namespace GO
{
	void public_deepening
	(
		class Chess_bitboard * p_bitboard,//������������� �����
		const __int8 depth_max_go_command,//������� ��������
		const double wtime_go_command,//���������� ����� �� ���� �� �����
		const double btime_go_command,//���������� ����� �� ���� �� ������
		const double winc_go_command,//���������� ������� �� ��� ��� �����
		const double binc_go_command,//���������� ������� �� ��� ��� ������
		const __int8 movestogo_go_command,//���������� ����� �� ������� �������� �����
		const double movetime_go_command,//����� �� ���
		const bool infinite_go_command//����������� ������
	);
}