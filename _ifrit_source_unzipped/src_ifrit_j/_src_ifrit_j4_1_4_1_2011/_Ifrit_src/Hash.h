#include "List_surplus_moves.h"

#define TEST_H 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������

#define TEST_WRITE_READ 0 // ��������-���������(1-0) ����� �������� ����������
                          // ���������� � ��������� �������
                          // ���� ��� �������� ���� TEST_WRITE_READ_WN � c_Iterative_Deepening.cpp

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ���������  9.03.2009 20.1.2010
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 40%

 ��������� ���-�������
 �������� � ���-�������� � ������� ��� ���
*/

// ������� � ���� ��������� ������ ��� ����� ����������� ������
// ������ ��� �������
// ��������� ����������� ������� ���� � ������ �������
struct T_hash_moves 
{
	// �������� ������ ���

	// ��� ���� ������� (������� �������� high 32 bits)
	// ���������� ������� ���� 64 ������� ��� - �����(high 32 bits)
	unsigned __int32 key_lock_32_move;

	// �������� ���� �������� � ��������� T_list_surplus_moves
	// � ���� ����� ���������� ��� ��� ������ ��� 32 ����� ������
	__int32 move; 

	// ������� �������� ��� ������� ������� ������ ���
	unsigned __int8 move_delta_depth;

	// ������� ����������� ����. ����� ��� �� �������������� �������� �� ������� �������
	unsigned __int8 move_age;


	// �������� ������ ������� �� ����(�����) � �� �������� �����(����)

	// ��� ���� ������� (������� �������� high 32 bits)
	// ���������� ������� ���� 64 ������� ��� - �����(high 32 bits)
	unsigned __int32 key_lock_32_score;

	// ������
	// ������ ��� ����� ����� ������� �� 16 �� �� ������
	__int32 score;

	// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 flag_hash_score;

	// ������� �������� ��� ������� �������� ������ ������
	unsigned __int8 score_delta_depth;

	// ������� ���������� ������. ����� ��� �� �������������� �������� �� ������� �������
	unsigned __int8 score_age;

};



class Hash
{

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////////
private:
//public:

	// ������������ ���������� ��������� � ���-�������
	unsigned __int64 max_index_hash;

	// ��������� �� ���-�������(��� ��� �� �����)
	struct T_hash_moves * p_hash_moves;

	// ������� �������� �������. ����� ��� �� �������������� �������� �� ������� �������
	unsigned __int8 age; 

#if TEST_WRITE_READ

	// ��������� ������ � ��� �������

	// ����� ���������� ���������� �������
	unsigned __int32 hash_write_all;

	// ���������� ���������� ������� ��� ���������� �����
	unsigned __int32 hash_move_write;

	// ���������� ���������� ������� ��� ������� �� ������ (LOWER // ������� �� ����(�����))
	unsigned __int32 hash_score_write_low;

	// ���������� ���������� ������� ��� ������� �� ������ (UPPER 3 �����(����) �������� �� �������)
	unsigned __int32 hash_score_write_high;


	// ��������� ������������� ����

	// ���������� ��� ���������� �����
	unsigned __int32 hash_read_sorting; 

	// ���������� ��� ������� �� ������. ����� ����������
	unsigned __int32 hash_read_score; 

	// ���������� ��� ������� �� ������. LOWER
	unsigned __int32 hash_read_score_LOWER; 

	// ���������� ��� ������� �� ������. UPPER 
	unsigned __int32 hash_read_score_UPPER; 

#endif // TEST_WRITE_READ


//////////////////////////////////////////////////////////////////////////////////////////
public:

// ���� ��� - ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TEST_WRITE_READ

	//---------------------------------------------------------------------------
	// �������� ��������
	void test_hash_ini();

	//---------------------------------------------------------------------------
	// �������� ��������
	void test_hash_print();

#endif // TEST_WRITE_READ
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ������ � ��� - �������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//---------------------------------------------------------------------------
	// �������� ������ ��� ���-�������. ������ �������� � ����������
	void hash_size_ini_default
	(
		unsigned __int16 hash_size// ������ ���-������� � ����������
	);

	//---------------------------------------------------------------------------
	// ����������� ������ �� ��� ���-������� � �� ����� �������� ������ ��� ���-�������.
	// ������ �������� � ����������
	void hash_size_resize
	(
		unsigned __int16 hash_size// ������ ���-������� � ����������
	);

	//---------------------------------------------------------------------------
	// ����������� ������ �� ��� ���-�������
	void hash_size_free();

	//---------------------------------------------------------------------------
	// �������������� ��� ����� � ���-������� 0
	void ini_hash_moves();

	//---------------------------------------------------------------------------
	// �������� ������ �� ������ ������ ��� ��� ������ (������� �� ������)
	void save_best_hash_score_or_move
	(
		unsigned __int8 i,// ����� ���� �� ������
		unsigned __int64 key,// ���-����
		class List_surplus_moves & list_surplus_moves,// ������ �����
		__int32 score,// ������ ����
		unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
		unsigned __int8 depth,// ������� ������ ����
		unsigned __int8 depth_max// ������������ ������� �������
	);

	//---------------------------------------------------------------------------
	// ������� �� ����������� �� ��� ����� �������. ���� ����������� 
	// �� ��������� ����������� ������
	// ������� ���������� ��� ������� �� ������ �� ���-�������
	__int32 look_hash_score_white
	(
		const unsigned __int64 key,// ���-����
		const unsigned __int8 depth,// �������
		const unsigned __int8 depth_max,// ������������ �������
		const __int32 alpha,//
		const __int32 beta,//
		unsigned __int8 & test_flag_hash_moves,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
		unsigned __int8 & hash_depth,// ������������ �������
		unsigned __int8 & hash_depth_max// ������������ �������
	);

	//---------------------------------------------------------------------------
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� 
	// �� ��������� ����������� ������
	// ������� ���������� ��� ������� �� ������ �� ���-�������
	__int32 look_hash_score_black
	(
		const unsigned __int64 key,// ���-����
		const unsigned __int8 depth,// �������
		const unsigned __int8 depth_max,// ������������ �������
		const __int32 alpha,//
		const __int32 beta,//
		unsigned __int8 & test_flag_hash_moves,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
		unsigned __int8 & hash_depth,// ������������ �������
		unsigned __int8 & hash_depth_max// ������������ �������
	);

	//---------------------------------------------------------------------------
	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	void look_hash_move
	(
		class List_surplus_moves & list_surplus_moves,// ������ �����
		const unsigned __int64 key,// ��� - ����
		const unsigned __int8 depth,// �������
		unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
	);

	//---------------------------------------------------------------------------
	// �������� ������� �������
	void set_age_0();

	//---------------------------------------------------------------------------
	// ����������� ������� �������
	void aging();


//////////////////////////////////////////////////////////////////////////////////////////
private:

	// ������ � ��� - �������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//---------------------------------------------------------------------------
	// �������� ������ �� ������ ������ ��� 
	inline void save_best_hash_move
	(
		unsigned __int8 i,// ����� ���� �� ������
		unsigned __int32 key_lock_32,//
		class List_surplus_moves & list_surplus_moves,// ������ �����
		unsigned __int8 delta_depth,// ������� ������ ����
		unsigned __int32 ind//
	);

	//---------------------------------------------------------------------------
	// �������� ��� �.�. ���������� ��� � ���-�������
	inline void save_hash_move
	(
		unsigned __int8 i,// ����� ���� �� ������
		unsigned __int32 key_lock_32,//
		class List_surplus_moves & list_surplus_moves,// ������ �����
		unsigned __int8 delta_depth,// ������� ������ ����
		unsigned __int32 ind//  
	);

	//---------------------------------------------------------------------------
	// �������� ������ �� ������ ������ ������
	inline void save_best_hash_score
	(
		unsigned __int32 key_lock_32,//
		__int32 score,// ������ ����
		unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
		unsigned __int8 delta_depth,// ������� ������ ����
		unsigned __int32 ind//
	);

	//---------------------------------------------------------------------------
	// �������� ������ �.�. ���������� �� � ���-�������
	inline void save_hash_score
	(
		unsigned __int32 key_lock_32,//
		__int32 score,// ������ ����
		unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
		unsigned __int8 delta_depth,// ������� ������ ����
		unsigned __int32 ind//  
	);

	//---------------------------------------------------------------------------
	// ��������� ���-�������. �������� ������ ���������� ��������� 
	// ������� ������������ � ������� ������
	void TEST_Hash_index_print();

};
