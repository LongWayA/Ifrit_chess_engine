#include <iostream>
#include <fstream>
//#include <cstring>      // ���������� ��� ������ �� ������� (��������� �����)
//#include <stdlib.h>

#include "Go.h"
#include "Hash.h"

#include "Search_root.h"

//-----------------------------------------------------------------------------------
/*+
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009 25.12.2010
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


extern class Hash hash;

//	===================================================================
void Go::parse_go
(
	class Chess_bitboard & bitboard,//�����
	const std::string string_in//������ ������
)
{

	wtime_go_command = -1.0;// ���������� ����� �� ���� �� �����
	btime_go_command = -1.0;// ���������� ����� �� ���� �� ������
	winc_go_command = 0;//  ���������� ������� �� ��� ��� �����
	binc_go_command = 0;//  ���������� ������� �� ��� ��� ������

	depth_max_go_command = -1;// ������� ��������
	movetime_go_command = -1.0;// ����� �� ���
	movestogo_go_command = -1;//���������� ����� �� ������������ �����
	infinite_go_command = false;// ���� ������������ �������

	// 
	ini_go_command(string_in);

	//
	if(
		   (wtime_go_command == -1.0 && btime_go_command == -1.0) 
		&& (depth_max_go_command == -1)
		&& (movetime_go_command == -1.0)
		&& (infinite_go_command == false)
	  )
	{
		std::cout << "Ifrit: illegal go command. I am terminated." << "\n";

		hash.hash_size_free();
		exit(1);
	}

	// ����������� ������� �������
	hash.aging();

	/////////////////////////////////////////////////////////////////////////////////

	// ����� ��� � ��������
	// �������� ������� � ��������� ��������
	deepening(bitboard);

}



//	===================================================================
void Go::ini_go_command
(
	const std::string string_in//������ ������
)
{
	__int16 find;
	__int16 find_w;
	__int16 find_b;
	__int16 find_w2;
	__int16 find_b2;
	__int16 delta_find_w;
	__int16 delta_find_b;


	// �������������� �����. ������������ � ������ �������
	//> go infinite ----------------------------------------------- 

    // ������� ������ ��������� ��������� "infinite"
	find = (int)string_in.find("infinite");

	if (find != -1)
	{
		infinite_go_command = true;

		// ���� �������� ���������� ���������
		//std::cout << "infinite_go_command = " << infinite_go_command;
		//std::cout << "\n";
	}
	// > ----------------------------------------------------------


	// ����� �� �������� �������
    // > go depth 5 -----------------------------------------------

    // ������� ������ ��������� ��������� "depth"
	find = (int)string_in.find("depth");

	if (find != -1)
	{

		// ���������� ������� depth � ������� �� �����
		find = find + 6;

		// �������� ��������� ���������� �����
		std::string string_value = string_in.substr(find);

        // ������� atoi ��������� ������ �������� � ����� �����;
		depth_max_go_command = atoi(string_value.c_str() );

		// ���� �������� ���������� ���������
		//std::cout << "depth_max_go_command = " << (int)depth_max_go_command;
		//std::cout << "\n";

	}
	// > ----------------------------------------------------------


	//����� �� ���
	// > go movetime 300000 ---------------------------------------

    // ������� ������ ��������� ��������� "movetime"
	find = (int)string_in.find("movetime");

	if (find != -1)
	{

		// ���������� ������� movetime � ������� �� �����
		find = find + 9;

		// �������� ��������� ���������� �����
		std::string string_value = string_in.substr(find);

        // ������� atoi ��������� ������ �������� � ����� �����;
		// ������������ ��������� � ������� (/ 1000.0)
		movetime_go_command = atoi(string_value.c_str())/ 1000.0;

		// ���� �������� ���������� ���������
		//std::cout << "movetime_go_command = " << (int)movetime_go_command;
		//std::cout << "\n";

	}

	// > ----------------------------------------------------------

	//����� �� ������������ ���������� �����
	//> go movestogo 40 wtime 300000 btime 300000 
	//for 40 moves in 5 min. 
	//������ ����� ������ � ����
	//go wtime 300000 btime 300000 winc 0 binc 0 movestogo 40


    // ������� ������ ��������� ��������� "movestogo"
	find = (int)string_in.find("movestogo");

	if (find != -1)
	{

		// ���������� ������� movestogo � ������� �� �����
		find = find + 10;

		// �������� ��������� ���������� �����
		std::string string_value = string_in.substr(find);

        // ������� atoi ��������� ������ �������� � ����� �����;
		movestogo_go_command = atoi(string_value.c_str());

		// ���� �������� ���������� ���������
		//std::cout << "movestogo_go_command = " << (int)movestogo_go_command;
		//std::cout << "\n";

	}

	// > ----------------------------------------------------------



	// ������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
	//for game in 5min. 
    //> go btime 300000 wtime 300000 ------------------------------

    // ������� ������ ��������� ��������� "wtime"
	find_w = (int)string_in.find("wtime");

	if (find_w != -1)
	{
		// ���������� ������� wtime � ������� �� �����
		find_w = find_w + 6;
		find_w2 = find_w;

		while ( string_in.at(find_w2) != ' ' )
		{
			find_w2 = find_w2 + 1;

			if( find_w2 >= (int)string_in.length() ) break;
		}

		delta_find_w = find_w2 - find_w;

		// �������� ��������� ���������� �����
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		// ������������ ��������� � ������� (/ 1000.0)
		wtime_go_command = double(atoi(string_value_w.c_str())) / 1000.0;

		// ���� �������� �����
		//std::cout << "wtime_go_command = " << wtime_go_command;
		//std::cout << "\n";

	}


    // ������� ������ ��������� ��������� "btime"
	find_b = (int)string_in.find("btime");

	if (find_b != -1)
	{

		// ���������� ������� btime � ������� �� �����
		find_b = find_b + 6;
		find_b2 = find_b;

		while ( string_in.at(find_b2) != ' ' )
		{
			find_b2 = find_b2 + 1;

			if( find_b2 >= (int)string_in.length() ) break;
		}

		delta_find_b = find_b2 - find_b;

		// �������� ��������� ���������� �����
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		// ������������ ��������� � ������� (/ 1000.0)
		btime_go_command = double(atoi(string_value_b.c_str())) / 1000.0;

		// ���� �������� ���������� ���������
		//std::cout << "btime_go_command = " << btime_go_command;
		//std::cout << "\n";

	}


	//> go btime 302000 wtime 302000 winc 2000 binc 2000 
	// with incremental clock 5 min + 2 sec. 

    // ������� ������ ��������� ��������� "winc"
	find_w = (int)string_in.find("winc");

	if (find_w != -1)
	{
		// ���������� ������� winc � ������� �� �����
		find_w = find_w + 5;
		find_w2 = find_w;

		while ( string_in.at(find_w2) != ' ' )
		{
			find_w2 = find_w2 + 1;

			if( find_w2 >= (int)string_in.length() ) break;

		}

		delta_find_w = find_w2 - find_w;

		// �������� ��������� ���������� �����
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		// ������������ ��������� � ������� (/ 1000.0)
		winc_go_command = double(atoi(string_value_w.c_str())) / 1000.0;

		// ���� �������� �����
		//std::cout << "winc_go_command = " << winc_go_command;
		//std::cout << "\n";

	}

    // ������� ������ ��������� ��������� "binc"
	find_b = (int)string_in.find("binc");

	if (find_b != -1)
	{
		// ���������� ������� binc � ������� �� �����
		find_b = find_b + 5;
		find_b2 = find_b;

		while ( string_in.at(find_b2) != ' ' )
		{
			find_b2 = find_b2 + 1;

			if( find_b2 >= (int)string_in.length() ) break;
		}

		delta_find_b = find_b2 - find_b;

		// �������� ��������� ���������� �����
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		// ������������ ��������� � ������� (/ 1000.0)
		binc_go_command = double(atoi(string_value_b.c_str())) / 1000.0;

		// ���� �������� ���������� ���������
		//std::cout << "binc_go_command = " << binc_go_command;
		//std::cout << "\n";

	}

	// > ----------------------------------------------------------


		// ���� ������ �� ���� ----------------------------------------------
		//else if (util.string_equal(ptr,"mate"))
		//{
		//	ptr = strtok(NULL," ");
		//	mate = atoi(ptr);
		//}
		//else if (util.string_equal(ptr,"nodes"))
		//{
		//	ptr = strtok(NULL," ");
		//	nodes = my_atoll(ptr);
		//}
		//else if (util.string_equal(ptr,"ponder"))
		//{
		//	ponder = true;
		//}
		//else if (util.string_equal(ptr,"searchmoves"))
		//{
		//}
		//-------------------------------------------------------------------

}