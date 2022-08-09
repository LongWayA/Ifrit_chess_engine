
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 10.5.2012
/// ������� ����� ������� %

#include "a_position_struct.h"
#include "a_move_list_struct.h"
#include "a_pv_line_struct.h"// ���� ����� ������
/*

UCI protocol
http://wbec-ridderkerk.nl/html/UCIProtocol.html

The UCI protocol as publiced by Stefan-Meyer Kahlen (ShredderChess):
Description of the universal chess interface (UCI)    April 2004

Move format:
------------

The move format is in long algebraic notation.
A nullmove from the Engine to the GUI should be send as 0000.
Examples:  e2e4, e7e5, e1g1 (white short castling), e7e8q (for promotion)

Engine to GUI:
--------------

* id
	* name 
		this must be sent after receiving the "uci" command to identify the engine,
		e.g. "id name Shredder X.Y\n"

	* author 
		this must be sent after receiving the "uci" command to identify the engine,
		e.g. "id author Stefan MK\n"

* uciok
	Must be sent after the id and optional options to tell the GUI that the engine
	has sent all infos and is ready in uci mode.

* readyok
	This must be sent when the engine has received an "isready" command and has
	processed all input and is ready to accept new commands now.
	It is usually sent after a command that can take some time to be able to 
	wait for the engine,
	but it can be used anytime, even when the engine is searching,
	and must always be answered with "isready".

* bestmove  [ ponder  ]
	the engine has stopped searching and found the move  best in this position.
	the engine can send the move it likes to ponder on. The engine must not 
	start pondering automatically.
	this command must always be sent if the engine stops searching, also in 
	pondering mode if there is a
	"stop" command, so for every "go" command a "bestmove" command is needed!
	Directly before that the engine should send a final "info" command with the 
	final search information,
	the the GUI has the complete statistics about the last search.

* copyprotection
....
         
* registration
....
	      
* info
	the engine wants to send infos to the GUI. This should be done whenever one of the info 
	has changed.
	The engine can send only selected infos and multiple infos can be send with one info 
	command,
	e.g. "info currmove e2e4 currmovenumber 1" or
	     "info depth 12 nodes 123456 nps 100000".
	Also all infos belonging to the pv should be sent together
	e.g. "info depth 2 score cp 214 time 1242 nodes 2124 nps 34928 pv e2e4 e7e5 g1f3"
	I suggest to start sending "currmove", "currmovenumber", "currline" and 
	"refutation" only after one second
	to avoid too much traffic.

	Additional info:

	* depth 
		search depth in plies

	* seldepth 
		selective search depth in plies,
		if the engine sends seldepth there must also a "depth" be present 
		in the same string.

	* time 
		the time searched in ms, this should be sent together with the pv.

	* nodes 
		x nodes searched, the engine should send this info regularly

	* pv
		the best line found

	* multipv 
	....

	* score

		* cp 
			the score from the engine's point of view in centipawns.

		* mate 
			mate in y moves, not plies.
			If the engine is getting mated use negativ values for y.

		* lowerbound
		....

		* upperbound
		....

	* currmove 
		currently searching this move

	* currmovenumber 
		currently searching move number x, for the first move x should be 1 not 0.

	* hashfull 
	....

	* nps 
		x nodes per second searched, the engine should send this info regularly

	* tbhits 
	....

	* cpuload 
	....

	* string 
	....

	* refutation   
	.... 

	* currline   
	.... 


* option
	This command tells the GUI which parameters can be changed in the engine.
	This should be sent once at engine startup after the "uci" and the "id" commands
	if any parameter can be changed in the engine.
	The GUI should parse this and build a dialog for the user to change the settings.
	Note that not every option needs to appear in this dialog as some options like
	"Ponder", "UCI_AnalyseMode", etc. are better handled elsewhere or are set 
	automatically.
	If the user wants to change some settings, the GUI will send a "setoption" 
	command to the engine.
	Note that the GUI need not send the setoption command when starting the engine 
	for every option if
	it doesn't want to change the default value.
	For all allowed combinations see the example below,
	as some combinations of this tokens don't make sense.
	One string will be sent for each parameter.

	* name 
		The option has the name id.
		Certain options have a fixed value for , which means that the semantics of 
		this option is fixed.
		Usually those options should not be displayed in the normal engine options 
		window of the GUI but
		get a special treatment. "Pondering" for example should be set automatically 
		when pondering is
		enabled or disabled in the GUI options. The same for "UCI_AnalyseMode" which 
		should also be set
		automatically by the GUI. All those certain options have the prefix "UCI_" 
		except for the
		first 6 options below. If the GUI get an unknown Option with the prefix 
		"UCI_", it should just
		ignore it and not display it in the engine's options dialog.

		*  = Hash, type is spin
			the value in MB for memory for hash tables can be changed,
			this should be answered with the first "setoptions" command at program boot
			if the engine has sent the appropriate "option name Hash" command,
			which should be supported by all engines!
			So the engine should use a very small hash first as default.

		*  = NalimovPath
		....

		*  = NalimovCache
		....

		*  = Ponder
		....

		*  = OwnBook
		....

		*  = MultiPV
		....

		*  = UCI_ShowCurrLine
		....

		*  = UCI_ShowRefutations
		....

		*  = UCI_LimitStrength, type check, should be false by default,
		....

		*  = UCI_Elo, type spin
		....

		*  = UCI_AnalyseMode, type check
		....

		*  = UCI_Opponent, type string
		....
		   		  
		* type 
		....

end link(����� ������)---------------------
*/

namespace Uci_engine_to_gui
{

/*
	������ ��� ���� < bestmove e2e4 
	��� ��� ����������� � ����(� �������) < bestmove d7d8N 
*/
	void do_bestmove();

/*
 �������� ���������� � �����
 � ����

 <1:info depth 2 seldepth 2 score cp 12 nodes 160 nps 5161 time 31 pv g1f3 g8f6
         ������� ��������� ������       ����      �������� �����   ����� ��������

*/
	void print_pv_line
	(
		struct Position & position,// ������������� �����
		const __int64 nodes,// ���������� ������������� �����(�������)
		const double time_current,  // ������� ������� ������ ��� ������ ��������
		const unsigned __int8 depth,
		bool & flag_mate,// ������ ��� 
		const __int64 nodes_y
	);

/*
	������� ��� ������� ����������
*/
	void print_currmove
	(
		const struct List & list_surplus_moves,///������ ��������� �����
		const unsigned __int8 nodes_0,/// ���������� ����� �� ������� �������
		const unsigned __int8 i,/// ����� ���� �� ������
		const unsigned __int8 Depth_Max/// ������������ �������
	);

/*
	������� ������� ������� � ��������
*/
	void print_pv_line_s
	(
		const bool colour_move,/// ���� ���������� �������
		const struct List & list_surplus_moves,///������ ��������� �����
		const struct PV_line & PV_best_point,/// ������ �������
		const __int64 print_nodes,/// ���������� �����
		const unsigned __int8 i,/// ����� ���� � ������
		const unsigned __int8 Depth_Max,/// ������������ �������
		const __int32 value,/// ������ �������
		const bool all_line/// ����� ���� �����
		//,const double time_nodes// ����� ������� ��������
	);

/*
	������� ����������� � ���� ��� ���� �� 9.01.06 
	������� ��� ������������ �������� :)) 
	������������ ����� ������ � �����
	0-��� ������, 1-�����, 2-����, 3-����, 4-�����, 5-�����, 6-������	
*/
	__int8 int_to_char_board
	(
		const int i,/// ��� ������
		const int z/// ���� ������
	);

/*
	�� ���������� � ����� ������ 
	����� ������� �������� 0 - '�', 1 - 'b', ..., 7 - 'h'	
*/
	__int8 x_to_notation
	(
		const int x/// �������������� ���������� ����� (0-7)
	);

};


