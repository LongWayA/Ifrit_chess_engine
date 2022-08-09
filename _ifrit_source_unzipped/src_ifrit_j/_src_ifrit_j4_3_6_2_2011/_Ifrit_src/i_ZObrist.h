
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 2.05.10

//!
#include "a_All_constants.h"

#define TEST_H 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������

/*
 ������� ����� ������� 50%

 ��������� ZObrist-�����
*/

class ZObrist
{

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
private:

	// � ������ ��� ����� ��������� ��������������� ������ ��������� ����� 
	// ��������� � ������������� ������ �� ����� � ��� ��� ������ �� ���� �����
	// ��� ���������� ������ ���������� �������� ����, ���, ���������� ������
	// ������ ���� ���������� � �������� ��������� �����
	// � ���������� ��������� ��� ����� �� ������� ��������� ���� ��� �� �����
	// ���������� ��������� � ������ ��������
	// [���� ������][��� ������][���������� ������]
	// 2*6*64= 768 
	unsigned __int64 random_piese_key[2][MAX_NAME][MAX_COORDINATE];

	// ������ ��� ����������� ���-������ ������� ��� ���������� �� �������
	// �.�. � ��� ������������ �� ������ ���� � pv_current �� � ����� �������
	// ������� ������� ���������� ����� ������ �������
	unsigned __int64 hash_three_moves[MAX_DEPTH];

	// ������ ��� ����������� ���-������ �������
	// ��� ������� �������� ������ �� ��������
	// ������� ������� ���������� ����� ������ ������� ����������� �� �������� ������ �� ��������
	unsigned __int64 hash_string_moves[1000];

	// ���������� ���������� ����� � ������
	unsigned __int16 save_cnt_move;

//////////////////////////////////////////////////////////////////////////////////////
public:

	// ������ � ��� - ������ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//---------------------------------------------------------------------------
	// ������ ��������� ������� ������ ��������� ������ 
	// ����������� ��������� �����
	void ini_random_piese_key();

	//---------------------------------------------------------------------------
	// �������������� ���� ��������� �������
	void start_position_random_key();

	//---------------------------------------------------------------------------
	// ������ ���� �� ����� ����
	void modification_random_key
	(
		bool  colour_figure,// ���� ������� �������
		unsigned __int8 i,// ����� ���� �� ������
		class List_surplus_moves & list_surplus_moves,// ������ �����
		struct Undo & undo//���������� ��� ������ ����
	);


//////////////////////////////////////////////////////////////////////////////////////
private:

	//---------------------------------------------------------------------------
	// ���������� ��������� 64 ������ ����
	unsigned __int64 rand_64();


//////////////////////////////////////////////////////////////////////////////////////
public:

	// ������� ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//---------------------------------------------------------------------------
	// ���������� ���������� ����� � ������ ���� ���������� �� ��������
	__int32 get_save_cnt_move();

	//---------------------------------------------------------------------------
	// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
	void ini_hash_three();

	//---------------------------------------------------------------------------
	// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
	void ini_hash_string_moves();

	//---------------------------------------------------------------------------
	// ���������� ���-���� �� ������� �������
	// ����� ��� ��������� �������� � ����� ��������
	void save_hash_three
	(
		unsigned __int64 key,// ���-����
		unsigned __int8 depth // �������
	);

	//---------------------------------------------------------------------------
	// ���������� ���-���� ��� �������� ����
	// ����� ��� ��������� �������� � ����� ������ ���������� ���������
	void save_hash_string_moves
	(
		unsigned __int64 key,// ���-����
		unsigned __int16 cnt_move // ������� �����
	);

	//---------------------------------------------------------------------------
	// ����������� ����� ������ ���������� �� ������ ������. ���� ����� ���������� �� ���������� 1 ����� 0
	// ������� ���������� �� ������ ���� ����������� ���� ����� ��� ��� �����
	bool analysis_hash_string_moves
	(
		unsigned __int8 depth // �������
	);

};