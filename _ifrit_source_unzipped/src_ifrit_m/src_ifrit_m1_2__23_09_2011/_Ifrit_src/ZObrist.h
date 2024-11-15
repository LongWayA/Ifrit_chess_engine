
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 2.05.10
/// ������� ����� ������� 50%


//!
#include "Header_constants.h"
#include "Undo.h"
#include "List.h"
#include "Position.h"

#define TEST_H 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������


/// ��������� ZObrist-�����


namespace ZObrist
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