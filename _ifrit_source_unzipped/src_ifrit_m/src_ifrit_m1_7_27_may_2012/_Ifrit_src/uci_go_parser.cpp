
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 22.02.2009 25.12.2010 23.5.2012

#include <fstream>
#include <iostream>

#include "uci_go_parser.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "a_go_parameters_struct.h"

#define TEST_TIME 0 // ��������-���������(1-0) ����� ������������ �������������
					// �������

#if TEST_TIME
// TEST
std::ofstream Test_TIME;
#endif//#if TEST_TIME

// ��������� ���������� ��������� ������� �� ������ (go)
struct Go_parameters go_parameters;

/*
	��������� ������� go � ���������� ��������� ����������� ������.

	������� ��� ����� ���� 5 ���
	> go btime 300000 wtime 300000 
	for game in 5min. 

	������� ��� ����� ���� 5 ��� + ���������� �� ��� � ��������(� ������� 2 ���)
	> go btime 302000 wtime 302000 winc 2000 binc 2000 
	with incremental clock 5 min + 2 sec. 

	����� �� ������������ ���������� ����� (� ������� 5 ��� �� 40 �����)
	go movestogo 40 wtime 300000 btime 300000 
	for 40 moves in 5 min. 
	������ ����� ������ � ����
	> go wtime 300000 btime 300000  movestogo 40

	����������� ����. ����������� � ������ �������������� �������
	> go infinite 
	for analysing. 

	����� �� ���(� ������� ������ 5 ��� �� ���) 
	> go movetime 300000 
	Exactly 5min for the next move

	����� �� �������� ������� (� ������� 5 ���������)
	> go depth 5 

*/
void Uci_go_parser::parsing
(
	struct Position & position,// ������������� �����
	const std::string string_in,//������ ������
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{

	// �������������� ��������� ������� �� ������
	ini_struct_go_parameters(string_in);

	// ��������� ��� ��������� ���� �����������������
	test_ini_struct_go_parameters();

	// �������������� ��������� ������������ ��������
	ini_parameters_iterative_deepening(position,depth_max,max_time_move_search);

}

/*
	��������� ������� go � �������������� ��������� go_parameters
*/
void Uci_go_parser::ini_struct_go_parameters
(
	const std::string string_in//������ ������
)
{

	go_parameters.wtime = -1.0;// ���������� ����� �� ���� �� �����
	go_parameters.btime = -1.0;// ���������� ����� �� ���� �� ������
	go_parameters.winc = 0;//  ���������� ������� �� ��� ��� �����
	go_parameters.binc = 0;//  ���������� ������� �� ��� ��� ������

	go_parameters.depth = -1;// ������� ��������
	go_parameters.movetime = -1.0;// ����� �� ���
	go_parameters.movestogo = -1;//���������� ����� �� ������������ �����
	go_parameters.infinite = false;// ���� ������������ �������

	// ��������� �� ����� ��������������� ������� �������
	__int16 find;
	__int16 find_w;
	__int16 find_w2;
	__int16 delta_find_w;
	__int16 find_b;
	__int16 find_b2;
	__int16 delta_find_b;

	// �������������� �����. ������������ � ������ �������
	//> go infinite

    // ������� ������ ��������� ��������� "infinite"
	find = (int)string_in.find("infinite");

	if (find != -1)
	{
		go_parameters.infinite = true;

		// ���� �������� ���������� ���������
		//std::cout << "go_parameters.infinite = " << go_parameters.infinite;
		//std::cout << std::endl;
	}

	// ����� �� �������� �������
    // > go depth 5

    // ������� ������ ��������� ��������� "depth"
	find = (int)string_in.find("depth");

	if (find != -1)
	{

		// ���������� ������� depth � ������� �� �����
		find = find + 6;

		// �������� ��������� ���������� �����
		std::string string_value = string_in.substr(find);

        // ������� atoi ��������� ������ �������� � ����� �����;
		go_parameters.depth = atoi(string_value.c_str() );

		// ���� �������� ���������� ���������
		//std::cout << "go_parameters.depth = " << (int)go_parameters.depth;
		//std::cout << std::endl;

	}

	//����� �� ���
	// > go movetime 300000

    // ������� ������ ��������� ��������� "movetime"
	find = (int)string_in.find("movetime");

	if (find != -1)
	{

		// ���������� ������� movetime � ������� �� �����
		find = find + 9;

		// �������� ��������� ���������� �����
		std::string string_value = string_in.substr(find);

        // ������� atoi ��������� ������ �������� � ����� �����;
		go_parameters.movetime = atoi(string_value.c_str());

		// ���� �������� ���������� ���������
		//std::cout << "go_parameters.movetime = " << (int)go_parameters.movetime;
		//std::cout << std::endl;

	}

	// > ----------------------------------------------------------
	// ���������� ����� �� ������������ �����
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
		go_parameters.movestogo = atoi(string_value.c_str());

		// ���� �������� ���������� ���������
		//std::cout << "go_parameters.movestogo = " << (int)go_parameters.movestogo;
		//std::cout << std::endl;

	}

	// > ----------------------------------------------------------
	// ������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
	//for game in 5min. 
    //> go btime 300000 wtime 300000

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

		// ������� ����� ��������� ���������� �����
		delta_find_w = find_w2 - find_w;

		// �������� ��������� ���������� �����
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		//
		go_parameters.wtime = double(atoi(string_value_w.c_str()));

		// ���� �������� �����
		//std::cout << "go_parameters.wtime = " << go_parameters.wtime;
		//std::cout << std::endl;

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

		// ������� ����� ��������� ���������� �����
		delta_find_b = find_b2 - find_b;

		// �������� ��������� ���������� �����
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		//
		go_parameters.btime = double(atoi(string_value_b.c_str()));

		// ���� �������� ���������� ���������
		//std::cout << "go_parameters.btime = " << go_parameters.btime;
		//std::cout << std::endl;

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

		// ������� ����� ��������� ���������� �����
		delta_find_w = find_w2 - find_w;

		// �������� ��������� ���������� �����
		std::string string_value_w = string_in.substr(find_w, delta_find_w);

		//
		go_parameters.winc = double(atoi(string_value_w.c_str()));

		// ���� �������� �����
		//std::cout << "go_parameters.winc = " << go_parameters.winc;
		//std::cout << std::endl;

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

		// ������� ����� ��������� ���������� �����
		delta_find_b = find_b2 - find_b;

		// �������� ��������� ���������� �����
		std::string string_value_b = string_in.substr(find_b, delta_find_b);

		//
		go_parameters.binc = double(atoi(string_value_b.c_str()));

		// ���� �������� ���������� ���������
		//std::cout << "go_parameters.binc = " << go_parameters.binc;
		//std::cout << std::endl;

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

/*
	�������������� ��������� ������������ ��������
	����������� ����� � ������� ������.
*/
void Uci_go_parser::ini_parameters_iterative_deepening
(
	struct Position & position,// ������������� �����
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{
	depth_max = MAX_DEPTH/2;

	//����� �� �������� �������
	// > go depth 5 
	if (go_parameters.depth != -1)
	{
		// TEST
		//std::cout << "go_parameters.depth " << std::endl;

		// ����� �� �������� �������
		depth_max = go_parameters.depth;
	}

	// ����������� ����. � ���� ����������� ��� ���������� ������� 50.
	// > go infinite
	else if (go_parameters.infinite != false) 
	{
		// TEST
		//std::cout << "go_parameters.infinite " << std::endl;
	}

	// ����� �� ���
	// > go movetime 300000 
	else if (go_parameters.movetime != -1)	
	{
		// TEST
		//std::cout << "movetime_go_command " << std::endl;

		//������ ����� �� ��� � ��������
		// ������������ ��������� � ������� (/ 1000.0)
		max_time_move_search = go_parameters.movetime / 1000.0;
	}

	// ������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
	// > go btime 300000 wtime 300000 
	else if ( (go_parameters.wtime != -1.0) || (go_parameters.btime != -1.0)) 
	{
		// TEST
		//std::cout << "go_parameters.wtime or go_parameters.btime " << std::endl;
		//���������� ����� �� ����

		// 	������������ ������������ ����� ���������� �� ������ ������� �������
		ini_max_time_move_search(position,depth_max,max_time_move_search);
	}
}

/*
 ������������ ������������ ����� ���������� �� ������ ������� �������

 ���� ������������� ����� �� ������� ����� ���� 1 ������ �� ������
 1 ��� = 60 ���
 ����� ������������ ��� ��� �� ������ 40 ����� 40 ���., � �� ����������� 20 ���.
 60 = 40 + 20
 ��� ��� ������ 40 ����� �������� �������� ��� � �������
 � � ���������� �.�. ����� 40-�� ���� ���� ���������� ������� �� ���

 ������ ������ ������ � ������ ��������� � ��� �� �����.
 �������, ��� 60 ����� ��� ������, ����� ����������� � ������.
 ����� �������, �� 60 ����� ������������� ������� �� ��� ����������,
 � ����� ���� ����������.
 ���������� ����� ����� �� ���������� ���������� �� 60 �����, 
 ������ �� �� ������ 20.
 ���������� ����� ����� �� ���.
 �� ��� � ����� ���������� ����� ��� �� ���.

 �������, ��� ��� ����� ���� ����� ������������.

*/
void Uci_go_parser::ini_max_time_move_search
(
	struct Position & position,// ������������� �����
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{
	unsigned __int16 cnt_half_move_already_made;// ���������� ��� ��������� � ������ ���������
	__int16 cnt_move_necessary_to_make;// ������� ����� �������� �������

// TEST
#if TEST_TIME
	Test_TIME.open ("_TEST_TIME.txt",std::ios::app);
	Test_TIME << " "<< std::endl;
	Test_TIME << "start ini"<< std::endl;
#endif//#if TEST_TIME

// TEST
#if TEST_TIME
	Test_TIME << "go_parameters.wtime = " << go_parameters.wtime << std::endl;
	Test_TIME << "go_parameters.btime = " << go_parameters.btime << std::endl;

	Test_TIME << "go_parameters.winc = " << go_parameters.winc << std::endl;
	Test_TIME << "go_parameters.binc = " << go_parameters.binc << std::endl;
#endif//#if TEST_TIME

	// ���������� ��������� ��������� � ������
	cnt_half_move_already_made = Zobrist_hashing::get_save_cnt_move();

// TEST
#if TEST_TIME
	Test_TIME << "cnt_half_move_already_made = " << cnt_half_move_already_made << std::endl;
#endif//#if TEST_TIME

	cnt_move_necessary_to_make = 60 - cnt_half_move_already_made/2 ; // 80

// TEST
#if TEST_TIME
	Test_TIME << "cnt_move_necessary_to_make = " << cnt_move_necessary_to_make << std::endl;
#endif//#if TEST_TIME

	// ����� �� ������ 60 ����� �������� ����������
	// ����� 60 �� ���� ���� ���������� ������� �� ��� ��� �� 
	// �������� ��������� �� �������
	if (cnt_move_necessary_to_make < 20)
	{
		cnt_move_necessary_to_make = 20;
	}

// TEST
#if TEST_TIME
	Test_TIME << "limit to 20 cnt_move_necessary_to_make = " << cnt_move_necessary_to_make << std::endl;
#endif//#if TEST_TIME

	if (position.colour_move == 1)// 1-��� ����� 0- ��� ������
	{

		max_time_move_search = go_parameters.winc * 0.2 
			+ go_parameters.wtime/cnt_move_necessary_to_make;
	}
	else
	{
		max_time_move_search = go_parameters.binc * 0.2 
			+ go_parameters.btime/cnt_move_necessary_to_make;
	}

	// ������ ����� �� ��� � ��������
	// ������������ ��������� � ������� (/ 1000.0)
	max_time_move_search = max_time_move_search / 1000.0;

// TEST
#if TEST_TIME
	Test_TIME << "max_time_move_search = " << max_time_move_search << std::endl;
#endif//#if TEST_TIME

	if (max_time_move_search != -1)
	{
		// �� ���������� � �������� �� ������� ��� ��������� �� ������ �������.
		// ������������ ��� �� ��������� ������ ����� ����� ����� � ��������� ���
		// ������. ������� �� ����� ����� �� ��� ����� ��� �� �� ���� ���������
		max_time_move_search = max_time_move_search / 3;
	}

// TEST
#if TEST_TIME
	Test_TIME << "max_time_move_search/3 = " << max_time_move_search << std::endl;

	Test_TIME << "deep_time_terminate( it is max_time_move_search * 15) = "
		<< (max_time_move_search * 15) << std::endl;

	Test_TIME.close();
#endif//#if TEST_TIME
}

/*
	��������� ��� ��������� ��������� go_parameters �����������������
*/
void Uci_go_parser::test_ini_struct_go_parameters()
{
	//
	if(
		   (go_parameters.wtime == -1.0 && go_parameters.btime == -1.0) 
		&& (go_parameters.depth == -1)
		&& (go_parameters.movetime == -1.0)
		&& (go_parameters.infinite == false)
	  )
	{
		std::cout << "Ifrit: illegal go command. I am terminated." << std::endl;

		Transposition_table::hash_size_free();
		exit(1);
	}
}