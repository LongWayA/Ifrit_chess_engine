#include <iostream>
#include <fstream>
//#include <cstring>      // ���������� ��� ������ �� ������� (��������� �����)
//#include <stdlib.h>

#include "c_Go.h"
#include "c_Iterative_Deepening.h"
#include "j_Hash.h"

#include "g_Search_root.h"

// �������� �������������� ������� �������� �� �� �������������
// ������� 'strtok'
// 'strtok': This function or variable may be unsafe. 
// Consider using strtok_s instead. To disable deprecation, 
// use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable : 4996)

//-----------------------------------------------------------------------------------
/*+
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009
*/
//-----------------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

 ������������ ������� �� �����

// ��������� ������� go � ������������� ����� ���� go command 

 ������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
 > go btime 300000 wtime 300000 
 for game in 5min. 

 > go btime 302000 wtime 302000 winc 2000 binc 2000 
 with incremental clock 5 min + 2 sec. 

 ����� �� ������������ ���������� �����
 > go movestogo 40 wtime 300000 btime 300000 
 for 40 moves in 5 min. 
 ������ ����� ������ � ����
 go wtime 300000 btime 300000 winc 0 binc 0 movestogo 40

 ��� ������� ��� ��� �������
 > go infinite 
 for analysing. 

 ����� �� ��� ������� ��� ���� �� ������������ � �� ����� ��� �� ������ ���� ���������
 > go movetime 300000 
 Exactly 5min for the next move, not supported by Fritz. 

 ����� �� �������� �������
 > go depth 5 
*/


namespace
{
	char string_in_l[LENGTH_STRING];//���������� � ������� parse_go
}

//	===================================================================
void GO::public_parse_go
(
	class Chess_bitboard * const p_bitboard,//�����
	const char string_in[]//������ ������
)
{
	const char * ptr;// ��������������� ���������

	//int mate = -1 ;

	double wtime_go_command = -1.0;// ���������� ����� �� ���� �� �����
	double btime_go_command = -1.0;// ���������� ����� �� ���� �� ������
	double winc_go_command = 0;//  ���������� ������� �� ��� ��� �����
	double binc_go_command = 0;//  ���������� ������� �� ��� ��� ������

	__int8 depth_max_go_command = -1;// ������� ��������
	double movetime_go_command = -1.0;// ����� �� ���
	__int8 movestogo_go_command = -1;//���������� ����� �� ������������ �����
	bool infinite_go_command = false;// ���� ������������ �������


	// �������� ������ string_in � ������ string_in_l ��� ����� ������ ��� � ����������
	// ������ ����� ��������� ��� �������
	strcpy (string_in_l,string_in);

	// ������� strtok �������� ��� �����������(����������� �� ���������) 
	// ������ ������ ��� ���� ��� ��������� ������
	// ��� ������ ������ ������ ������ � ����� ��� ��� ���������� ���� 
	ptr = strtok(string_in_l," "); // ���������� "go"
	 
	// ��� �� ����� �� ������ ������� ��������� ���������� ���������
	for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) 
	{

		if (string_equal(ptr,"depth"))
		{//������� ��������
			ptr = strtok(NULL," ");// ��������� �� depth �������� ���������� ��������(����� �����)
			depth_max_go_command = atoi(ptr);// ������� atoi ��������� ������ � ����� �����
		}
		else if (string_equal(ptr,"wtime"))
		{// ����� ������� ����������� �����
			ptr = strtok(NULL," ");
			wtime_go_command = double(atoi(ptr)) / 1000.0;// ������������ ��������� � ������� (/ 1000.0)
		}
		else if (string_equal(ptr,"btime"))
		{// ����� ������� ����������� ������
			ptr = strtok(NULL," ");
			btime_go_command = double(atoi(ptr)) / 1000.0;
		}
		else if (string_equal(ptr,"winc"))
		{// ���������� ����� ��� �����
			ptr = strtok(NULL," ");
			winc_go_command = double(atoi(ptr)) / 1000.0;
		}
		else if (string_equal(ptr,"binc"))
		{// ���������� ����� ��� ������
			ptr = strtok(NULL," ");
			binc_go_command = double(atoi(ptr)) / 1000.0;
		}
		else if (string_equal(ptr,"movestogo"))
		{//���������� ����� �� ������� �������� �����
			ptr = strtok(NULL," ");
			movestogo_go_command = atoi(ptr);

		}
		else if (string_equal(ptr,"movetime"))
		{// ����� �� ���
			ptr = strtok(NULL," ");
			movetime_go_command = double(atoi(ptr)) / 1000.0;

		}
		else if (string_equal(ptr,"infinite"))
		{// ����������� ������
			infinite_go_command = true;

		// ���� ������ �� ���� ----------------------------------------------
		//}
		//else if (string_equal(ptr,"mate"))
		//{
		//	ptr = strtok(NULL," ");
		//	mate = atoi(ptr);
		//}
		//else if (string_equal(ptr,"nodes"))
		//{
		//	ptr = strtok(NULL," ");
		//	nodes = my_atoll(ptr);
		//}
		//else if (string_equal(ptr,"ponder"))
		//{
		//	ponder = true;
		//}
		//else if (string_equal(ptr,"searchmoves"))
		//{
		//-------------------------------------------------------------------

		}

	}


	ptr = NULL;

	// ����������� ������� �������
	HASHM::public_aging();

	/////////////////////////////////////////////////////////////////////////////////

	// ����� ��� � ��������
	// �������� ������� � ��������� ��������
	public_deepening(p_bitboard,depth_max_go_command,wtime_go_command,btime_go_command
		,winc_go_command,binc_go_command,movestogo_go_command
		,movetime_go_command,infinite_go_command);

}

//	===================================================================
// ���� �� � ������ ������� ���������
bool GO::string_equal
(
	const char s1[], 
	const char s2[]
)
{
	//������� ���������� ����� �� ������ ������ ������� ������������?
	return strcmp(s1,s2) == 0;
}

//	===================================================================
// ���� �� � ������ ������ ������� ���������
bool GO::string_start_with
(
	const char s1[], 
	const char s2[]
)
{
	// ������ ��������� �� ������ ��������� ������ ������ � ������
	// �� � ���� ��� ��������� ������ �� ����� �� ��� ����� � ����� ������
	return strstr(s1,s2) == s1;
}
