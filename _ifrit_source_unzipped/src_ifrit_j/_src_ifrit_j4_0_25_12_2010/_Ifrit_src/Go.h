#include "Structure.h"
#include "Chess_bitboard.h"

#define TEST_D 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������

#define TEST_WRITE_READ_WN 0 // ��������-���������(1-0) ����� �������� ����������
                             // ���������� � ��������� �������
                             // ���� ��� �������� ���� TEST_WRITE_READ � j_Hash.h

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009 25.12.2010
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

 ������������ ������� �� �����

 ��������� ������� go � ��������� ������

 ����������� ��������� ��������� ������:
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

class Go
{

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////////
private:

	__int64 nodes_y; // ���������� ����� �� ������ �������
	__int64 nodes_previous; // ���������� ������ ���������� �����
	double koff_deepening;  // ����������� ��������� ������ �� ������� 
							      // ��������� ��������� �������

	/*
	��������� ������� go � ������������� ����� ����

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

//
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//-----------------------------------------------------------------------------------
	void parse_go
	(
		class Chess_bitboard & bitboard,//�����
		const std::string string_in//������ ������
	);

// ���������� � �����
// c_Go_iterative_deepening.cpp
/////////////////////////////////////////////////////////////////////////////////////
private:

/*
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
	void deepening
	(
		class Chess_bitboard & bitboard,//������������� �����
		const __int8 depth_max_go_command,//������� ��������
		const double wtime_go_command,//���������� ����� �� ���� �� �����
		const double btime_go_command,//���������� ����� �� ���� �� ������
		const double winc_go_command,//���������� ������� �� ��� ��� �����
		const double binc_go_command,//���������� ������� �� ��� ��� ������
		const __int8 movestogo_go_command,//���������� ����� �� ������� �������� �����
		const double movetime_go_command,//����� �� ���
		const bool infinite_go_command//����������� ������
	);

	//
	void ini_go_command
	(
		const std::string string_in,//������ ������
		__int8 & depth_max_go_command,//������� ��������
		double & wtime_go_command,//���������� ����� �� ���� �� �����
		double & btime_go_command,//���������� ����� �� ���� �� ������
		double & winc_go_command,//���������� ������� �� ��� ��� �����
		double & binc_go_command,//���������� ������� �� ��� ��� ������
		__int8 & movestogo_go_command,//���������� ����� �� ������� �������� �����
		double & movetime_go_command,//����� �� ���
		bool & infinite_go_command//����������� ������
	);


};
