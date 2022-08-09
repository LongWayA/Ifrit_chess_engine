#include "a_Structure.h"

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

namespace HASHM
{
	// ����� �������� ������������� �������
	const unsigned __int8 EXACT = 1; // ������ ���
	const unsigned __int8 LOWER = 2; // ������� �� ����(�����)
	const unsigned __int8 UPPER = 3; // �����(����) �������� �� �������


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
	void public_hash_size_ini_default
	(
		unsigned __int16 hash_size// ������ ���-������� � ����������
	);

	//---------------------------------------------------------------------------
	// ����������� ������ �� ��� ���-������� � �� ����� �������� ������ ��� ���-�������.
	// ������ �������� � ����������
	void public_hash_size_resize
	(
		unsigned __int16 hash_size// ������ ���-������� � ����������
	);

	//---------------------------------------------------------------------------
	// ����������� ������ �� ��� ���-�������
	void public_hash_size_free();

	//---------------------------------------------------------------------------
	// �������������� ��� ����� � ���-������� 0
	void public_ini_hash_moves();

	//---------------------------------------------------------------------------
	// �������� ������ �� ������ ������ ��� ��� ������ (������� �� ������)
	void public_save_best_hash_score_or_move
	(
		unsigned __int8 i,// ����� ���� �� ������
		unsigned __int64 key,// ���-����
		struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
		__int32 score,// ������ ����
		unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
		unsigned __int8 depth,// ������� ������ ����
		unsigned __int8 depth_max// ������������ ������� �������
	);

	//---------------------------------------------------------------------------
	// ������� �� ����������� �� ��� ����� �������. ���� ����������� 
	// �� ��������� ����������� ������
	// ������� ���������� ��� ������� �� ������ �� ���-�������
	__int32 public_look_hash_score_white
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
	__int32 public_look_hash_score_black
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
	void public_look_hash_move
	(
		struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
		const unsigned __int64 key,// ��� - ����
		const unsigned __int8 depth,// �������
		unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
	);

	//---------------------------------------------------------------------------
	// �������� ������� �������
	void public_set_age_0();

	//---------------------------------------------------------------------------
	// ����������� ������� �������
	void public_aging();

}

//////////////////////////////////////////////////////////////////////////////////////////
//private:

namespace HASHM{

	// ������ � ��� - �������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//---------------------------------------------------------------------------
	// �������� ������ �� ������ ������ ��� 
	inline void save_best_hash_move
	(
		unsigned __int8 i,// ����� ���� �� ������
		unsigned __int32 key_lock_32,//
		struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
		unsigned __int8 delta_depth,// ������� ������ ����
		unsigned __int32 ind//
	);

	//---------------------------------------------------------------------------
	// �������� ��� �.�. ���������� ��� � ���-�������
	inline void save_hash_move
	(
		unsigned __int8 i,// ����� ���� �� ������
		unsigned __int32 key_lock_32,//
		struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
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
	// ��������� ���-��� �� ������ ������� � ������ ����� ����������� ��� ������ �������
	inline void insert_hash_moves
	(
		struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
		unsigned __int32 ind,// ��� - ����
		unsigned __int8 depth ,// �������
		unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
	);

	//---------------------------------------------------------------------------
	// ��������� ���-�������. �������� ������ ���������� ��������� 
	// ������� ������������ � ������� ������
	void TEST_Hash_index_print();

}
