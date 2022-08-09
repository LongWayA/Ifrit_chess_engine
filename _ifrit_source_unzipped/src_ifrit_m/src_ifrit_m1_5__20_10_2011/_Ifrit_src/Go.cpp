
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 22.02.2009 25.12.2010

#define TEST_TIME 0 // ��������-���������(1-0) ����� ������������ �������������
					// �������

#include <fstream>
#include <iostream>

#include "Go.h"
#include "ZObrist.h"
#include "Hash.h"


#if TEST_TIME
// TEST
std::ofstream Test_TIME;
#endif//#if TEST_TIME

	double wtime_go_command;     /// ���������� ����� �� ���� �� �����
	double btime_go_command;     /// ���������� ����� �� ���� �� ������
	double winc_go_command;      ///  ���������� ������� �� ��� ��� �����
	double binc_go_command;      ///  ���������� ������� �� ��� ��� ������

	__int8 depth_max_go_command; /// ������� ��������
	double movetime_go_command;  /// ����� �� ���
	__int8 movestogo_go_command; /// ���������� ����� �� ������������ �����
	bool infinite_go_command;    /// ���� ������������ �������


//	===================================================================
void Go::parse_go
(
	struct Position & position,// ������������� �����
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

	double max_time_move_search = -1.0;// ����� ���������� �� ���
	unsigned __int8 depth_max = 0;//������������ ������� ��������

	// �������������� ��������� ����
	ini_go_command(string_in);

	// ��������� ��� ��������� ���� �����������������
	test_ini_go_command();


	// �������������� ��������� ������������ ��������
	ini_deepening(position,depth_max,max_time_move_search);

	// ����������� ������� �������
	Hash::aging();

	/////////////////////////////////////////////////////////////////////////////////

	// ����� ��� � ��������
	// �������� ������� � ��������� ��������
	deepening(position,depth_max,max_time_move_search);

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

//	===================================================================
void Go::ini_deepening
(
	struct Position & position,// ������������� �����
	unsigned __int8 & depth_max,
	double & max_time_move_search
)
{

	unsigned __int16 cnt_move ;// ���������� ��� ��������� � ������ ���������
	__int16 limit_cnt_move ;// ������� ����� �������� �������

	/*
		��� ������������� ������� ���� �� ����� ������ � ����� ���� 
		��� ��� �� ���������
		����� ��� ����� ������� ������ �� ������ ���������:

	*/
 
		// TEST ---------
		//std::cout << "depth_max_go_command = " << (int)depth_max_go_command << "\n";
		//std::cout << "infinite_go_command = " << infinite_go_command << "\n";
		//std::cout << "movetime_go_command = " << movetime_go_command << "\n";
		//std::cout << "movestogo_go_command = " << (int)movestogo_go_command << "\n";

		//std::cout << "winc_go_command = " << winc_go_command << "\n";
		//std::cout << "binc_go_command = " << binc_go_command << "\n";
		//std::cout << "wtime_go_command = " << wtime_go_command << "\n";
		//std::cout << "btime_go_command = " << btime_go_command << "\n";
		// ---------------

		// TEST
		//std::cout << "koff_deepening = " << koff_deepening << "\n";

	// TEST
#if TEST_TIME
	Test_TIME.open ("_TEST_TIME.txt",std::ios::app);
	Test_TIME << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";
#endif//#if TEST_TIME

	//����� �� �������� �������
	// > go depth 5 
	if (depth_max_go_command != -1)
	{
		// TEST
		//std::cout << "depth_max_go_command " << "\n";

		// ����� �� �������� �������
		depth_max = depth_max_go_command;
	}

	// ����������� ����. � ���� ����������� �� ������� 50.
	// > go infinite
	else if (infinite_go_command != false) 
	{
		// TEST
		//std::cout << "infinite_go_command " << "\n";

		// ������ ��� ����������� �������. 50 ���� ����� ����� ����������
		depth_max = 50;
	}

	// ����� �� ���
	// > go movetime 300000 
	else if (movetime_go_command != -1)	
	{
		// TEST
		//std::cout << "movetime_go_command " << "\n";

		//������ ����� �� ���
		max_time_move_search = movetime_go_command;
		depth_max = 50;
	}

	// ����� �� ������������ ���������� �����
	// > go movestogo 40 wtime 300000 btime 300000 
	//else if (movestogo_go_command != -1) 
	//{
	//// ������ �� �� ��� ����������� �� ������� 
	//// ������� ����� ��������, �.�. �� ��������� movestogo
 //   // ������ ��������� �� ������ �����. ��� ��� ���� ��� ��������
 //   //----------------------------------------------------------------------------------
	//	//// TEST
	//	////std::cout << "movestogo_go_command " << "\n";

	//	//// ����� �� ������������ ���������� �����
	//	//// ��������(�������� �����) ���� ������� ������� ����� 
	//	//// ��������, �.�. ��������� movestogo

	//	//// ���������� ��������� ��������� � ������
	//	//cnt_move = ZObrist::get_save_cnt_move();

	//	//// ��� ���������� ���������� �����
	//	//limit_cnt_move = movestogo_go_command;

	//	//// TEST
	//	////std::cout << "cnt_move = " << cnt_move << "\n";
	//	////std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";

	//	//// ��������� ��������� �����
	//	//if (limit_cnt_move < 5)
	//	//{
	//	//	limit_cnt_move = 5;
	//	//}

	//	//// ����� ���������� �� ������ ����� �� ���������� ����� 
	//	//// ������ ��� ���������� ��������. ����� ����� �������� �������
	//	//if (bitboard.colour_move == 1)// 1-��� ����� 0- ��� ������
	//	//{

	//	//	max_time_move_search = winc_go_command +  wtime_go_command/limit_cnt_move;
	//	//}
	//	//else
	//	//{
	//	//	max_time_move_search = binc_go_command +  btime_go_command/limit_cnt_move;
	//	//}

 //	//	depth_max = 50;
 //   //----------------------------------------------------------------------------------

 //   //----------------------------------------------------------------------------------
	//	// ���������� ��������� ��������� � ������
	//	cnt_move = ZObrist::get_save_cnt_move();

	//	// �� ������������ ��� ��� ������ ���� �� ������������ ���������� ����� (80)
	//	// ��� ���������� ���������� �����
	//	limit_cnt_move = 80 - cnt_move/2 ;

	//	// ����� �� ������ 60 ����� �������� ����������
	//	// ����� 60 �� ���� ���� ���������� ������� �� ��� ��� �� 
	//	// �������� ��������� �� �������
	//	if (limit_cnt_move < 20)
	//	{
	//		limit_cnt_move = 20;
	//	}

	//	//limit_cnt_move = 20;

	//	if (bitboard.get_colour_move() == 1)// 1-��� ����� 0- ��� ������
	//	{

	//		max_time_move_search = winc_go_command + wtime_go_command/limit_cnt_move;
	//	}
	//	else
	//	{
	//		max_time_move_search = binc_go_command + btime_go_command/limit_cnt_move;
	//	}

 //		depth_max = 50;
 //   //----------------------------------------------------------------------------------
	//}



	// ������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
	// > go btime 300000 wtime 300000 
	else if ( (wtime_go_command != -1.0) || (btime_go_command != -1.0)) 
	{
		// TEST
		//std::cout << "wtime_go_command or btime_go_command " << "\n";
		//���������� ����� �� ����

#if TEST_TIME
		// TEST
		Test_TIME << "wtime_go_command = " << wtime_go_command << "\n";
		Test_TIME << "btime_go_command = " << btime_go_command << "\n";

		// TEST
		Test_TIME << "winc_go_command = " << winc_go_command << "\n";
		Test_TIME << "binc_go_command = " << binc_go_command << "\n";
#endif//#if TEST_TIME

		// ���������� ��������� ��������� � ������
		cnt_move = ZObrist::get_save_cnt_move();

#if TEST_TIME
		// TEST
		Test_TIME << "cnt_move = " << cnt_move << "\n";
#endif//#if TEST_TIME

/*
���� ������������� ����� �� ������� ����� ���� 1 ������ �� ������

1 ��� = 60 ���
����� ������������ ��� ��� �� ������ 40 ����� 40 ���., � �� ����������� 20 ���.
60 = 40 + 20
��� ��� ������ 40 ����� �������� �������� ��� � �������
� � ���������� �.�. ����� 40-�� ���� ���� ���������� ������� �� ���
*/
		limit_cnt_move = 60 - cnt_move/2 ; // 80

#if TEST_TIME
		// TEST
		Test_TIME << "limit_cnt_move 1 = " << limit_cnt_move << "\n";
#endif//#if TEST_TIME

		// ����� �� ������ 60 ����� �������� ����������
		// ����� 60 �� ���� ���� ���������� ������� �� ��� ��� �� 
		// �������� ��������� �� �������
		if (limit_cnt_move < 20)
		{
			limit_cnt_move = 20;
		}

#if TEST_TIME
		// TEST
		Test_TIME << "limit_cnt_move 2 = " << limit_cnt_move << "\n";
#endif//#if TEST_TIME

		if (position.colour_move == 1)// 1-��� ����� 0- ��� ������
		{

			max_time_move_search = winc_go_command * 0.2 
				+ wtime_go_command/limit_cnt_move;
		}
		else
		{
			max_time_move_search = binc_go_command * 0.2 
				+ btime_go_command/limit_cnt_move;
		}

 		depth_max = 50;
	}

#if TEST_TIME
	// TEST
	Test_TIME << "max_time_move_search 1 = " << max_time_move_search << "\n";
#endif//#if TEST_TIME

	if (max_time_move_search != -1)
	{
		// �� ���������� � �������� �� ������� ��� ��������� �� ������ �������.
		// ������������ ��� �� ��������� ������ ����� ����� ����� � ��������� ���
		// ������. ������� �� ����� ����� �� ��� ����� ��� �� �� ���� ���������
		max_time_move_search = max_time_move_search / 3;
	}

#if TEST_TIME
	// TEST
	Test_TIME << "max_time_move_search 2 = " << max_time_move_search << "\n";

	// TEST
	Test_TIME << "deep_time_terminate = " << (max_time_move_search * 15) << "\n";

	// TEST
	Test_TIME.close();
#endif//#if TEST_TIME

	// TEST
	//std::cout << "koff max_time_move_search = " << max_time_move_search << "\n";

}

//	===================================================================
void Go::test_ini_go_command()
{

	//
	if(
		   (wtime_go_command == -1.0 && btime_go_command == -1.0) 
		&& (depth_max_go_command == -1)
		&& (movetime_go_command == -1.0)
		&& (infinite_go_command == false)
	  )
	{
		std::cout << "Ifrit: illegal go command. I am terminated." << "\n";

		Hash::hash_size_free();
		exit(1);
	}
}