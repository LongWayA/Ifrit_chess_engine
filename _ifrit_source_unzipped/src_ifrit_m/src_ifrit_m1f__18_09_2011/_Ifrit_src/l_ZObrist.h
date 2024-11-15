
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 2.05.10
/// ������� ����� ������� 50%


//!
#include "a_Header_constants.h"
#include "a_Undo.h"
#include "a_List.h"
#include "a_Position.h"

#define TEST_H 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������


/// ��������� ZObrist-�����


class l_ZObrist
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
private:

	/// � ������ ��� ����� ��������� ��������������� ������ ��������� ����� 
	/// ��������� � ������������� ������ �� ����� � ��� ��� ������ �� ���� �����
	/// ��� ���������� ������ ���������� �������� ����, ���, ���������� ������
	/// ������ ���� ���������� � �������� ��������� �����
	/// � ���������� ��������� ��� ����� �� ������� ��������� ���� ��� �� �����
	/// ���������� ��������� � ������ ��������
	/// [���� ������][��� ������][���������� ������]
	/// 2*6*64= 768 
	unsigned __int64 random_piese_key[2][MAX_NAME][MAX_COORDINATE];

	/// ������ ��� ����������� ���-������ ������� ��� ���������� �� �������
	/// �.�. � ��� ������������ �� ������ ���� � pv_current �� � ����� �������
	/// ������� ������� ���������� ����� ������ �������
	unsigned __int64 hash_three_moves[MAX_DEPTH];

	/// ������ ��� ����������� ���-������ �������
	/// ��� ������� �������� ������ �� ��������
	/// ������� ������� ���������� ����� ������ ������� ����������� �� �������� ������ �� ��������
	unsigned __int64 hash_string_moves[1000];

	/// ���������� ���������� ����� � ������
	unsigned __int16 save_cnt_move;

//////////////////////////////////////////////////////////////////////////////////////
public:

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
private:

	///---------------------------------------------------------------------------
	/// ���������� ��������� 64 ������ ����
	unsigned __int64 rand_64();


//////////////////////////////////////////////////////////////////////////////////////
public:

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