
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 12.06.2007  11.11.2009 19.12.2010 1.1.2011
/// ������� ����� ������� 70%

#include "a_position_struct.h"
				 
/*
UCI
http://chessprogramming.wikispaces.com/UCI

 UCI, the Universal Chess Interface is an open communication 
 protocol for chess engines to play games automatically, that 
 is to communicate with other programs including 
 Graphical User Interfaces. UCI was designed and developed 
 by Rudolf Huber and Stefan Meyer-Kahlen, released in November 2000 [1] . 
 It has, by-in-large, replaced the older Chess Engine Communication Protocol 
 (WinBoard/XBoard).

Design Philosophy
 The UCI capable GUI is not only View and Controller of a chess MVC, 
 but also keeps the Model with its internal game states. It is also 
 an "arbiter" instance to decide about the outcome of the game, for 
 instance in declaring a game as draw after a threefold repetition has 
 occurred - the UCI GUI may choose and play moves from an opening book 
 and endgame tablebase.

end link(����� ������)---------------------
*/


/*
UCI protocol
http://wbec-ridderkerk.nl/html/UCIProtocol.html

The UCI protocol as publiced by Stefan-Meyer Kahlen (ShredderChess):
Description of the universal chess interface (UCI)    April 2004
....
all communication is done via standard input and output with text commands,
....

GUI to engine:
--------------

These are all the command the engine gets from the interface.

uci
	tell engine to use the uci (universal chess interface),
	this will be send once as a first command after program boot
	to tell the engine to switch to uci mode.
	After receiving the uci command the engine must identify itself with the "id" 
	command
	and sent the "option" commands to tell the GUI which engine settings the engine 
	supports if any.
	After that the engine should sent "uciok" to acknowledge the uci mode.
	If no uciok is sent within a certain time period, the engine task will be killed 
	by the GUI.

debug [ on | off ]
....

isready
	this is used to synchronize the engine with the GUI. When the GUI has sent 
	a command or
	multiple commands that can take some time to complete,
	this command can be used to wait for the engine to be ready again or
	to ping the engine to find out if it is still alive.
	E.g. this should be sent after setting the path to the tablebases as this 
	can take some time.
	This command is also required once before the engine is asked to do any 
	search to wait for the engine to finish initializing.
	This command must always be answered with "readyok" and can be sent also 
	when the engine is calculating
	in which case the engine should also immediately answer with "readyok" 
	without stopping the search.

setoption name  [value ]
	this is sent to the engine when the user wants to change the internal parameters
	of the engine. For the "button" type no value is needed.
	One string will be sent for each parameter and this will only be sent when 
	the engine is waiting.
	The name of the option in  should not be case sensitive and can inludes spaces 
	like also the value.
	The substrings "value" and "name" should be avoided in  and  to allow 
	unambiguous parsing,
	for example do not use  = "draw value".
	Here are some strings for the example below:
	   "setoption name Nullmove value true\n"
      "setoption name Selectivity value 3\n"
	   "setoption name Style value Risky\n"
	   "setoption name Clear Hash\n"
	   "setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"

register
....

ucinewgame
   this is sent to the engine when the next search (started with "position" and "go") 
   will be from
   a different game. This can be a new game the engine should play or a new game 
   it should analyse but
   also the next position from a testsuite with positions only.
   If the GUI hasn't sent a "ucinewgame" before the first "position" command, 
   the engine shouldn't
   expect any further ucinewgame commands as the GUI is probably not supporting 
   the ucinewgame command.
   So the engine should not rely on this command even though all new GUIs should 
   support it.
   As the engine's reaction to "ucinewgame" can take some time the GUI should always 
   send "isready"
   after "ucinewgame" to wait for the engine to finish its operation.
   
position [fen  | startpos ]  moves  .... 
	set up the position described in fenstring on the internal board and
	play the moves on the internal chess board.
	if the game was played  from the start position the string "startpos" 
	will be sent
	Note: no "new" command is needed. However, if this position is from a 
	different game than
	the last position sent to the engine, the GUI should have sent a "ucinewgame" 
	inbetween.

go
....
( read in "uci_go_parser.h")
    
stop
	stop calculating as soon as possible,
	don't forget the "bestmove" and possibly the "ponder" token when finishing the search

ponderhit
....

quit
	quit the program as soon as possible

end link(����� ������)---------------------
*/

namespace Uci_gui_to_engine
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

/*
	��������� �������� UCI ������� �� ����������� ��������� ��������
	������������ ��������� ������� �� ��������� ����������� ��������.

	>1:uci
	�������� �������� ���, ��� �������� �� UCI ���������.
	� ����� �� ����� ��� ������ � ������ � ������������ 
	���������� �������� �� ����� ���������.
	����� uciok

	>1:isready
	� ���� �����?
	�������� ��� ��! (readyok)

	>1:setoption name Hash value 2 (���� ������ ������� ������ � 2 ��)
	��������� ������������� ������ ���-������� � Mb
	� �� ��������� ����� �������� ������ ��� ���-�������

	>1:ucinewgame
	����� ����
	���������� ��� � ������� � ����.

	>1:position ....
	������� ���������� ��������� �������
 
	����� ���� ��������� �����:
	>1:position startpos (��� ��������� �������)
	>1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (��� ������� ������)
	>1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1
	>1:position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 moves b1c3 b8c6 g1f3 g8f6

	"position startpos" ������ ��������� �������
	"position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� 
	+ ���� ������� � ������� 
	"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1" ������� ������� 
	����� ��� �������������
	"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 moves b1c3 b8c6 g1f3 g8f6" 
	������� ������� ����� ��� ������������� + ���� ������� � ������� �������
	������ fen ��������� � ������ "fen_parser.h"

	>1:go .....
	������� �� ������ �������
	������������ � ������ "uci_go_parser.h"

	>1:quit
	������� �� �����
	���������� ������ � ��������� ���������

	>1:stop
	������� �� ���������. ����� ����������� � ������� ������ 

	� ������ ����������� ������� "go ....." ���������� 1 ����� 0
*/
	__int32 parsing_command
	(
		struct Position & position,// ������������� �����
		const std::string gui_string_command//�������� ������
	);


//////////////////////////////////////////////////////////////////////////////////////
//private:

/*
	�������� ������ ����� ���:
	>1:setoption name Hash value 2 (���� ������ ������� ������ � 2 ��)
	��������� � ������ ������ ������� � �������� � ����� �����
	� ������ ������ ��� 2
*/
	unsigned __int16 hash_value_Mb_string_to_int
	(
		const std::string string_in //�������� ������
	);

/*
	�������� ������ ����� ���� ��������� �����:
	>1:position startpos (��� ��������� �������)
	>1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (��� ������� ������)
	>1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1
	>1:position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 moves b1c3 b8c6 g1f3 g8f6
	��� ������� fen ���������� ������ "fen_parser.h"
	��������� �������� ������ � ������������� ���������� ������������� �������
	��� ������� ���������� ������� move_to_position
	���������� ������ "zobrist_hashing.h"
*/
	void chess_position_string_to_internal_chess_board
	(
		struct Position & position,// ������������� �����
		const std::string string_in//�������� ������
	);

/*
	�������� � ������ ���� ���������� � ������� ������� �� ���������
	�������� ������ ����� ���: 
	moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	��������� ������ � ��������������� ����� ���� �������� �������� �������
	���������� ������ "move_generation.h", "make_move.h"
*/
	void move_to_position
	(
		struct Position & position,// ������������� �����
		const __int8 move_string[]// ������ ���������� ����
	);

};


