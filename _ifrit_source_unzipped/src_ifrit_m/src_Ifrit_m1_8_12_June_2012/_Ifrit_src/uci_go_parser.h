
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 22.02.2009 25.12.2010 23.5.2012
/// ������� ����� ������� 50%

#include "a_position_struct.h"

/*
UCI protocol
http://wbec-ridderkerk.nl/html/UCIProtocol.html

The UCI protocol as publiced by Stefan-Meyer Kahlen (ShredderChess):
Description of the universal chess interface (UCI)    April 2004


GUI to engine:
--------------

* go
	start calculating on the current position set up with the "position" command.
	There are a number of commands that can follow this command, all will be 
	sent in the same string.
	If one command is not send its value should be interpreted as it would not 
	influence the search.

	* searchmoves  
	.... 

	* ponder 
	....

	* wtime 
		white has x msec left on the clock

	* btime 
		black has x msec left on the clock

	* winc 
		white increment per move in mseconds if x > 0

	* binc 
		black increment per move in mseconds if x > 0

	* movestogo 
      there are x moves to the next time control,
		this will only be sent if x > 0,
		if you don't get this and get the wtime and btime it's sudden death

	* depth 
		search x plies only.

	* nodes 
	....

	* mate 
	....

	* movetime 
		search exactly x mseconds

	* infinite
		search until the "stop" command. Do not exit the search without 
		being told so in this mode!

end link(����� ������)---------------------
*/

namespace Uci_go_parser
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

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
	void parsing
	(
		struct Position & position,// ������������� �����
		const std::string string_in,//������ ������
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

//////////////////////////////////////////////////////////////////////////////////////
//private:

/*
	��������� ������� go � �������������� ��������� go_parameters
*/
	void ini_struct_go_parameters
	(
		const std::string string_in//������ ������
	);

/*
	�������������� ��������� ������������ ��������
	����������� ����� � ������� ������.
*/
	void ini_parameters_iterative_deepening
	(
		struct Position & position,// ������������� �����
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

/*
	������������ ������������ ����� ���������� �� ������ ������� �������
*/
	void ini_max_time_move_search
	(
		struct Position & position,// ������������� �����
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

/*
	��������� ��� ��������� ��������� go_parameters �����������������
*/
	void test_ini_struct_go_parameters();

};
