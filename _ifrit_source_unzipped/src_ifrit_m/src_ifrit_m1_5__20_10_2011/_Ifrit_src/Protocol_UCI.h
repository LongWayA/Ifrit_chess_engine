
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 12.06.2007  11.11.2009 19.12.2010 1.1.2011
/// ������� ����� ������� 70%

#include "Position.h"

/// ��������-���������(1-0)  ����� ������ � 
/// ����(_TEST_FEN.txt) �������� ����������
#define TEST_P 0
				 
/// ��� � ���� ������� ������
#define NAME_OF_PROGRAM_AND_DATA_OF_RELEASE "id name Ifrit_m1_5_Beta_20_10_2011"


/// ������ ��� ������.

/// ������������ ��������� ������� �� ��������� ����������� ��������.

/// >1:uci
/// �������� �������� ���, ��� �������� �� ��� ���������.
/// � ����� �� ����� ��� ������ � ������ � ������������ 
/// ���������� �������� �� ����� ���������.
/// ����� uciok

/// >1:ucinewgame
/// ����� ����
/// ���������� ��� � ������� � ����.


/// >1:setoption name Hash value 2 (���� ������ ������� ������ � 2 ��)
/// ��������� ������������� ������ ���-������� � Mb
/// � �� ��������� ����� �������� ������ ��� ���-�������

/*
	> setoption name Ponder value true
	> setoption name Hash value 16
	> setoption name NalimovCache value 4
	> setoption name NalimovPath value D:\tbs
	> setoption name Position Learning value true
	> setoption name BookFile value dbbook.bin
	> setoption name Book Size value Big
	> setoption name Selectivity value 5
	���� �� �����������
*/

/// >1:isready
/// � ���� �����?
/// �������� ��� ��! (readyok)

/// ������� ���������� ��������� �������
/// >1:position startpos (��� ��������� �������)
/// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (��� ������� ������)
/// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1
/// "position startpos" ������ ��������� �������
/// "position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� 
/// + ���� ������� � ������� 
/// "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1" ������� ������� 
/// ����� ��� �������������
/// "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 moves b1c3 b8c6 g1f3 g8f6" 
/// ������� ������� ����� ��� ������������� + ���� ������� � ������� 
/// � ���������� ������ ������ �� �������� ���������� ������� ��������������� �������
/// �������������� � ��� - �������������

/// ������� �� ������ �������
/// >1:go depth 5 (��� ����� �� �������� ������� - � ������ ������ 5 ��������� )
/// >1:go wtime 300000 btime 300000 (���� 5 ��� - ������ ���)
/// >1:go movetime 5000 ( 5 ������ �� ���)
/// >1:go infinite (������ �������)

	/// ������
	/// ���� ����� ������� ������ ������ namespace Go 
	/// � � ��� ��������� ������� go � ��������� ������

	/// ������� ��� ����� ���� 5 ��� ���� 5 ��� + ���������� �� ��� � ��������
	/// > go btime 300000 wtime 300000 
	/// for game in 5min. 

	/// > go btime 302000 wtime 302000 winc 2000 binc 2000 
	/// with incremental clock 5 min + 2 sec. 

	/// ����� �� ������������ ���������� �����
	/// > go movestogo 40 wtime 300000 btime 300000 
	/// for 40 moves in 5 min. 
	/// ������ ����� ������ � ����
	/// > go wtime 300000 btime 300000  movestogo 40

	/// ��� ������� ��� ��� �������
	/// > go infinite 
	/// for analysing. 

	/// ����� �� ��� ������� ��� ���� �� ������������ � 
	///�� ����� ��� �� ������ ���� ���������
	/// > go movetime 300000 
	/// Exactly 5min for the next move, not supported by Fritz. 

	/// ����� �� �������� �������
	/// > go depth 5 

/// >1:quit
/// ������� �� �����
/// ���������� ������ � ��������� ���������

/// >1:stop
/// ������� �� ���������. ����������� � ������� ������ 

namespace Protocol_UCI
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

	/// ��������� �������� UCI ������� �� ����������� ��������� ��������
	void parse_protocol_UCI
	(
		struct Position & position,// ������������� �����
		const std::string string_in//�������� ������
	);


//////////////////////////////////////////////////////////////////////////////////////
//private:

	///--------------------------------------------------------------------------
	/// ��������� � ������ � ������ ������ ���� � ����������
	unsigned __int16 read_hash_size_Mb
	(
		const std::string string_in //�������� ������
	);

	///---------------------------------------------------------------------------
	/// ��������� �������� ��� - �������
	void parse_position_FEN
	(
		struct Position & position,// ������������� �����
		const std::string string_in//�������� ������
	);

	///---------------------------------------------------------------------------
	/// �������� � ������ ���� ���������� � ������� ������� �� ���������
	void move_to_board
	(
		struct Position & position,// ������������� �����
		const __int8 move_string[]// ������ ���������� ����
	);

};


