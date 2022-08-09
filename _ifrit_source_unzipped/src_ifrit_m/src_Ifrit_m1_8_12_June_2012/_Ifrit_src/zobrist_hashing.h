
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 2.05.10
/// ������� ����� ������� 50%


//!
#include "a_undo_struct.h"
#include "a_move_list_struct.h"
#include "a_position_struct.h"

#define TEST_H 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������

/*

Zobrist Hashing
http://chessprogramming.wikispaces.com/Zobrist+Hashing

 Zobrist Hashing,
 a technique to transform a board position of arbitrary size into 
 a number of a set length, with an equal distribution over all 
 possible numbers, invented by Albert Zobrist [1]. In an early 
 Usenet post in 1982, Tom Truscott mentioned Jim Gillogly's 
 n-bit hashing technique [2], who apparently read Zobrist's paper 
 early, and credits Zobrist in a 1997 rgcc post [3].

 The main purpose of Zobrist hash codes in chess programming 
 is to get an almost unique index number for any chess position, 
 with a very important requirement that two similar positions 
 generate entirely different indices. These index numbers are 
 used for faster and more space efficient Hash tables or databases, 
 e.g. transposition tables and opening books.

end link(����� ������)---------------------
*/

/*
 ��������� ZObrist-�����
*/

namespace Zobrist_hashing
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

	/// ������ � ��� - ������ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	///---------------------------------------------------------------------------
	/// ������ ��������� ������� ������ ��������� ������ 
	/// ����������� ��������� �����
	void ini_random_piese_key();

	///---------------------------------------------------------------------------
	/// �������������� ���� ��������� �������
	void start_position_random_key
	(
		struct Position & position/// ������������� �����
	);

	///---------------------------------------------------------------------------
	/// ������ ���� �� ����� ����
	void modification_random_key
	(
		struct Position & position,/// ������������� �����
		bool  colour_figure,/// ���� ������� �������
		unsigned __int8 i,/// ����� ���� �� ������
		struct List & list_surplus_moves,/// ������������� ������ �����
		struct Undo & undo///���������� ��� ������ ����
	);


//////////////////////////////////////////////////////////////////////////////////////
//private:

	///---------------------------------------------------------------------------
	/// ���������� ��������� 64 ������ ����
	unsigned __int64 rand_64();


//////////////////////////////////////////////////////////////////////////////////////
//public:

	/// ������� ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	///---------------------------------------------------------------------------
	/// ���������� ���������� ����� � ������ ���� ���������� �� ��������
	__int32 get_save_cnt_move();

	///---------------------------------------------------------------------------
	/// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
	void ini_hash_three();

	///---------------------------------------------------------------------------
	/// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
	void ini_hash_string_moves();

	///---------------------------------------------------------------------------
	/// ���������� ���-���� �� ������� �������
	/// ����� ��� ��������� �������� � ����� ��������
	void save_hash_three
	(
		unsigned __int64 key,/// ���-����
		unsigned __int8 depth /// �������
	);

	///---------------------------------------------------------------------------
	/// ���������� ���-���� ��� �������� ����
	/// ����� ��� ��������� �������� � ����� ������ ���������� ���������
	void save_hash_string_moves
	(
		unsigned __int64 key,/// ���-����
		unsigned __int16 cnt_move /// ������� �����
	);

	///---------------------------------------------------------------------------
	/// ����������� ����� ������ ���������� �� ������ ������. ���� ����� ���������� �� ���������� 1 ����� 0
	/// ������� ���������� �� ������ ���� ����������� ���� ����� ��� ��� �����
	bool analysis_hash_string_moves
	(
		unsigned __int8 depth /// �������
	);

};