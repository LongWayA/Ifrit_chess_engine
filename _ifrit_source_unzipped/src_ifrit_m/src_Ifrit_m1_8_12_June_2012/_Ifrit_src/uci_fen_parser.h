
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010
/// ������� ����� ������� 70%



///!
#include "a_position_struct.h"

/*
Forsyth-Edwards Notation
http://chessprogramming.wikispaces.com/Forsyth-Edwards+Notation

Forsyth-Edwards Notation (FEN) describes a Chess Position. 
It is an one-line ASCII-string. FEN is based on a system 
created by Scotsman David Forsyth in the 19th century. 
Steven Edwards specified the FEN standard for computer 
chess applications as part of the Portable Game Notation

end link(����� ������)---------------------
*/

namespace Uci_fen_parser
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

/*
	��������� ������������� ����� (��� ��������� ������� �������� � ����)
*/
	void bitboard_ini
	(
		struct Position & position// ������������� �����
	);

/*
	�� ����� ������ ������ ������	
	'a'-0 'b'-1 'c'-2 'd'-3 'e'-4 'f'-5 'g'-6 'h'-7 ����� 777 :)
*/
	int koordinata_to_indeks
	(
		const __int8 ch
	);

/*
	�� ������� ����� ������ �����
	���������� ������ ������� � ���� ��� ����� �������� ��� ���� ������� :)
	'1'-0 '2'-1 '3'-2 '4'-3 '5'-4 '6'-5 '7'-6 '8'-7 ����� 777 :)
*/
	int ch_koordinata_to_indeks
	(
		const int ch
	);

/*
	�������� �� ������� ���� "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	�������� / ���������� ����������� ��������� �����
	����� �������� ���������� ������ ������ �� �����������
	����� ��� ������ ������ ������� ����� ������ ����� ���� � ��������� ������
	������ ��� �������� �������� ��������� ����
	w - - 0 1  ��� ����� ��������� ��� 
	b KQkq e3 0 1 ��� ������ ���� ��� ��������� � ������ �� �������
	�������� ��� ���������� ����� ����:
	�������� ����� ���� 1. e4:
	rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
	������� ������� ��� ������ �������� ���������� 
	������ ���� (��� ���� �4 ����� ���� �3)

	��������� ��� - ������� � �������������� ���������� ������� 
*/
	void fen_to_board
	(
		struct Position & position,// ������������� �����
		const std::string position_fen//������ ���������� ��� �������
	);
};