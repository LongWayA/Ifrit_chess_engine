#include <iostream>
#include <fstream>  // TEST

#include "j_Hash.h"
#include "k_Util.h"
#include "d_Bitboard_const.h"

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

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 std::ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


namespace 
{

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

}



// ���� ��� - ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TEST_WRITE_READ

//	===================================================================
// �������� ��������
 void HASHM::test_hash_ini()
 {
	// ��������� ������ � ��� �������--------------------------------------

	// ����� ���������� ���������� �������
	hash_write_all = 0;

	// ���������� ���������� ������� ��� ���������� �����
	hash_move_write = 0;

	// ���������� ���������� ������� ��� ������� �� ������
	hash_score_write_low = 0;

	// ���������� ���������� ������� ��� ������� �� ������
	hash_score_write_high = 0;

	// ��������� ������������� ������---------------------------------------- 

	// ���������� ��� ���������� �����
	hash_read_sorting = 0; 

	// ���������� ��� ������� �� ������ ����� ����������
	hash_read_score = 0;  

	hash_read_score_LOWER = 0;

	hash_read_score_UPPER = 0;

 }

//	===================================================================
// �������� ��������
void HASHM::test_hash_print()
{
	std::cout << " " << "\n";

	hash_write_all = hash_move_write + hash_score_write_low + hash_score_write_high;

	// �������� ���������� ���� ���������� �������
	std::cout << "hash_write_all = " << hash_write_all << "\n";

	if (hash_write_all != 0)
	{
		// ���������� ���������� ������� ��� ���������� �����
		std::cout << "hash_move_write = " << hash_move_write << "   100 * hash_move_write/hash_write_all = " << 100 * hash_move_write/hash_write_all << "\n";

		// ���������� ���������� ������� ��� ������� �� ������
		std::cout << "hash_score_write_low = " << hash_score_write_low << "   100 * hash_score_write_low/hash_write_all = " << 100 * hash_score_write_low/hash_write_all << "\n";

		// ���������� ���������� ������� ��� ������� �� ������
		std::cout << "hash_score_write_high = " << hash_score_write_high << "   100 * hash_score_write_high/hash_write_all = " << 100 * hash_score_write_high/hash_write_all << "\n";

		std::cout << " " << "\n";

		// �������� ���������� �������������� ��� ���������� �������
		std::cout << "hash_read_sorting = " << hash_read_sorting << "   100 * hash_read_sorting/hash_write_all = " << 100 * hash_read_sorting/hash_write_all << "\n";
	}

	// ������ ����� ����������� � ���������������
	std::cout << "hash_write_all - hash_read_sorting = " << (hash_write_all - hash_read_sorting) << "\n";


	std::cout << " " << "\n";

	hash_read_score = hash_read_score_LOWER + hash_read_score_UPPER;

	if ( hash_read_score != 0)
	{
		if (hash_write_all != 0)
		{
			// �������� ���������� ���� ��������� �������
			std::cout << "hash_read_score = " << hash_read_score << "   100 * hash_read_score/hash_write_all = " << 100 * hash_read_score/hash_write_all << "\n";

			// �������� ���������� ���� ��������� �������
			std::cout << "hash_read_all = " << (hash_read_sorting + hash_read_score) << "   100 * hash_read_all/hash_write_all = " << 100 * (hash_read_sorting + hash_read_score)/hash_write_all << "\n";
		}

		// �������� ���������� �������������� ��� ������� �� ������ �������
		std::cout << "hash_read_score_LOWER = " << hash_read_score_LOWER <<  "   100 * hash_read_score_LOWER/hash_read_score = " << 100 * hash_read_score_LOWER/hash_read_score << "\n";

		// �������� ���������� �������������� ��� ������� �� ������ �������
		std::cout << "hash_read_score_UPPER = " << hash_read_score_UPPER <<  "   100 * hash_read_score_UPPER/hash_read_score = " << 100 * hash_read_score_UPPER/hash_read_score << "\n";
	}

	std::cout << " " << "\n";
 }

#endif // TEST_WRITE_READ
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// ������ � ��� - �������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// �������� ������ ��� ���. ������ �������� � ����������
void HASHM::public_hash_size_ini_default
(
	unsigned __int16 hash_size// ������ ���-������� � ����������
)
{
	unsigned __int64 hash_n = 0;

	if (hash_size < 1) hash_size = 1;// ������ �� ������ ���������
	if (hash_size > 1024) hash_size = 1024;// ������ �� ������ ���������

	// ������� sizeof(struct T_hash_moves) ������ ������ � ������ �.�. int ��� 4
	// � ������ ������ struct T_hash_moves - 16 ����
	// ���������� ������ � �� � ����� � ����� � ���������� ���������
	max_index_hash = (unsigned __int64)(
		(hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));

	//std::cout << "max_index_hash in 1 mb = " << max_index_hash << "\n";
	//std::cout << "sizeof(T_hash_moves) = " << sizeof(struct T_hash_moves) << "\n";
	//std::cout << "sizeof(unsigned int) = " << sizeof(unsigned int) << "\n";

	// ��� ����� �������� ������ (2 � ������� n) -1 ��� �� ���������� 
	// ����� ���� 000..0111 � ����� ���� ������� ��� - ������ � ������� ������� ��������� &
	for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

	hash_n = hash_n / 2;

	if ((hash_n & (hash_n - 1)) != 0)
	{
		std::cout << "������! ������ �� ���� ������� ���� " << hash_n << "\n";
	}

	max_index_hash = hash_n - 1;

	// calloc
	// Allocates a block of memory for an array of num elements, 
	// each of them size bytes long, and initializes all its bits to zero.
	// ����������� ������ ��� ������� � ��������� �� ������
	p_hash_moves = (struct T_hash_moves*) calloc (
		(__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

	if (p_hash_moves == NULL) 
	{
		std::cout << "������ ��� " << "\n";
		exit(1); 
	}

	// TEST
	//std::cout << "sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
	//std::cout << "max_index_hash = " << max_index_hash << "\n";
	//std::cout << "hash_n = " << hash_n << "\n";

}

//	===================================================================
// ����������� ������ �� ��� ���-������� 
// � �� ����� �������� ������ ��� ���. ������ �������� � ����������
void HASHM::public_hash_size_resize
(
	unsigned __int16 hash_size// ������ ���-������� � ����������
)
{
	unsigned __int64 hash_n = 0;

	if (p_hash_moves == NULL) 
	{
		std::cout << "����������� ������ " << "\n";
		exit(1); 
	}

	free(p_hash_moves);// ����������� ���������� ����� ������

	if (hash_size < 1) hash_size = 1;// ������ �� ������ ���������
	if (hash_size > 1024) hash_size = 1024;// ������ �� ������ ���������

	// ���������� ������ � �� � ����� � ����� � ���������� ���������
	max_index_hash = (unsigned __int64)(
		(hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));

    // TEST
	//std::cout << "��������������� max_index_hash = " << max_index_hash << "\n";
 

	// ��� ����� �������� ������ (2 � ������� n) -1 ��� �� ���������� 
	// ����� ���� 000..0111 � ����� ���� ������� ��� - ������ � ������� ������� ��������� &
	for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

	hash_n = hash_n / 2;

	if ((hash_n & (hash_n - 1)) != 0)
	{
		std::cout << "������! ������ �� ���� ������� ���� " << hash_n << "\n";
	}

	max_index_hash = hash_n - 1;

	// ����������� ������ ��� ������� � ��������� �� ������
	p_hash_moves = (struct T_hash_moves*) calloc (
		(__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

	if (p_hash_moves == NULL) 
	{
		std::cout << "������ ��� " << "\n";
		exit(1); 
	}

// TEST
	//std::cout << " ������ ���-������� � ���������� hash_size = " << hash_size << "\n";
	//std::cout << " � ����� �������� ������ � �� = " << ((int)(max_index_hash + 3) * sizeof(T_hash_moves))/(1024 * 1024) << "\n";
	//std::cout << "������ �������� � ������ sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
	//std::cout << "����� ������� 2 hash_n = " << hash_n << "\n";
	//std::cout << "���������� ��������� � ���-������� max_index_hash = " << max_index_hash << "\n";

}

//	===================================================================
// ����������� ������ �� ��� ���-�������
void HASHM::public_hash_size_free()
{
	free(p_hash_moves);
}

//	===================================================================
// �������������� ��� ����� ���-������� 0
void HASHM::public_ini_hash_moves()
{                                                      
	for (unsigned __int32 ihm = 0; ihm <= max_index_hash; ihm++)
	{
		p_hash_moves[ihm].key_lock_32_move = 0;
		p_hash_moves[ihm].key_lock_32_score = 0;
	}
}

//	===================================================================
// �������� ������ �� ������ ������ ��� ��� ������ (������� �� ������)
void HASHM::public_save_best_hash_score_or_move
(
	unsigned __int8 i,// ����� ���� �� ������
	unsigned __int64 key,// ���-����
	struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
	__int32 score,// ������ ����
	unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 depth,// ������� ������ ����
	unsigned __int8 depth_max// ������������ ������� �������
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
	unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);
	unsigned __int8 delta_depth = depth_max - depth;

	if (delta_depth < 0) delta_depth = 0;

	/*
	������ ������� ���� � ������ ������ ���������
	������ ��� ��� ���������� � ����� ������ �� ����� �������������� ����������
	� ������� ������� � �������� ������ ���� � ������

	�� ����� ���� �� ����� ��� ������� � ���� ������ ���-�������
	��� ���������� � ���� �������� ���������� �������
	����� ������ ������� ����� ������� � ���� � �� �� ������
	*/

	// ������ ������� ����
	save_best_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	// ������ ������ ������
	save_best_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
}

//	===================================================================
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� ��������� ����������� ������
// ����� �� �����
__int32 HASHM::public_look_hash_score_white
(
	const unsigned __int64 key,// ���-����
	const unsigned __int8 depth,// �������
	const unsigned __int8 depth_max,// ������������ �������
	const __int32 alpha,//
	const __int32 beta,//
	unsigned __int8 & test_flag_hash_moves,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 & hash_depth,// ������������ �������
	unsigned __int8 & hash_depth_max// ������������ �������
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
	unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);


	/*
	(�������� �� ������� �����)

	��� ������������� ������ �� ���-������� �������� ��� ������:

	1)(� ���� ��������� ���� ������ �� ��������)
	EXACT (��������� score <  beta )
	��� �������� �������� ������ ������ �.�. beta > score > alpha
	��� ��� �� ������� � ������� ����� � ����� ���� �-� ������� ����� �������
	�� ������ ������ �� ����� ���� �� ����� �����
	������� ������ ��� �� ������������ ��������� ��� ��� ��� ��� ��� �� �����.
	��������� ������ �� score <  beta  
	������ ��� ������ ��� alpha ��� ������ �� �����
	��� ��� � ������ score < alpha �� ������ �������� 3-�� �������

	2)LOWER (��������� score >= beta )
	��� �������� �������� ������ ������� ������� ������� �� beta. ��� �������
	�.�. ��� ���������� ����� ������ �� ������ ����������.
	��� ������� ������ ��� �� ��� � ������ �������� ������� �� ���� ��� ���
	��������� ������ score >= beta

	3)UPPER (��������� score < alpha )
	��� �������� ������ �� �������� ����� � �� ������� ������� �� ����. ��� ���������
	��������. �������� ������ �� �� �����. �� ��� � �� ������� �� �������� ������� 
	��� �� ��� �� �������� �� �������� �����. 
	��� ��� ��������� score < alpha
	������� ���������� ����� score <= alpha �� ��� ������������ � �� ���� ��������� ���� ������
	*/

	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	if (p_hash_moves[ind].key_lock_32_score == key_lock_32)
	{
		// ���� ������� ��� ������������ �� ����� �������� ������ ��� ������ ��� �� �������
		// �� ���������� ����������� �� ����� ������ ������� ������ ��������
		if (p_hash_moves[ind].score_delta_depth >= (depth_max - depth))
		{
			if (p_hash_moves[ind].flag_hash_score == LOWER)
			{
				// ���� ��������� ��������� �� ������������ ������� ����� � � ������ ������
				// �� ��� - ������� ����� �� ������ ������ � �� ��� ������� �����. ���� �� ����� ��� 
				// ��������� �� ����� ��������� ������ �� ���������. �� ��� �� ��� ����� ������ ��� ��
				// ��� �������� ����� � � ������ ������.
				if (p_hash_moves[ind].score >= beta)
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_LOWER = hash_read_score_LOWER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				} 

			}
			else
			{

				// ��������� ��� ������ ��� ��� �� �������� �����
				// ���� ������� ��� ���� ������� ��� ���������� ���� � ������
				// � ���� ���� � � ��������� � ����� �� � ������� ���� � ������� ��������� 
				// ��� ����� ���� ��� � ������ ����� ���� �������
				// ������ ����� ����������� � ������� �� ������� ����� ������ ������� <= � �����������
				if (p_hash_moves[ind].score < alpha) 
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_UPPER = hash_read_score_UPPER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				}
			}
		}
	}

	return -1000000000;
}

//	===================================================================
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� ��������� ����������� ������
//����� �� ������
__int32 HASHM::public_look_hash_score_black
(
	const unsigned __int64 key,// ���-����
	const unsigned __int8 depth,// �������
	const unsigned __int8 depth_max,// ������������ �������
	const __int32 alpha,//
	const __int32 beta,//
	unsigned __int8 & test_flag_hash_moves,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 & hash_depth,// ������������ �������
	unsigned __int8 & hash_depth_max// ������������ �������
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
	unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);

	if (p_hash_moves[ind].key_lock_32_score == key_lock_32)
	{
		if (p_hash_moves[ind].score_delta_depth >= (depth_max - depth))
		{
			if (p_hash_moves[ind].flag_hash_score == LOWER)
			{
				if (p_hash_moves[ind].score <= alpha) 
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_LOWER = hash_read_score_LOWER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				}
			}
			else
			{
				if (p_hash_moves[ind].score > beta) 
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_UPPER = hash_read_score_UPPER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				}
			}
		}
	}
	return -1000000000;
}

//	===================================================================
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
void HASHM::public_look_hash_move
(
	struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
	const unsigned __int64 key,// ���-����
	const unsigned __int8 depth,// �������
	unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);

	// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
	if (p_hash_moves[ind].key_lock_32_move == (unsigned __int32)(key >> 32))
	{
#if TEST_WRITE_READ
		// ���������� ��������� ��� ���������� �������
		hash_read_sorting = hash_read_sorting + 1;
#endif // TEST_WRITE_READ

		// ������� ��� �� ������ ������� � ������
		insert_hash_moves(p_list_surplus_moves,ind,depth,flag_insert_hash_move);
	}
}

//	===================================================================
// �������� ������� �������
void HASHM::public_set_age_0()
{
	age = 0;
}

//	===================================================================
// ����������� ������� �������
void HASHM::public_aging()
{
	if (age > 250) age = 0;

	age = age + 1;
	//std::cout << "age= " << (__int32)age << "\n";
}


//////////////////////////////////////////////////////////////////////////////////////////
//private:


//	===================================================================
// �������� ������ �� ������ ������ ���
inline void HASHM::save_best_hash_move
(
	unsigned __int8 i,// ����� ���� �� ������
	unsigned __int32 key_lock_32,//
	struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
	unsigned __int8 delta_depth,// ������� ������ ����
	unsigned __int32 ind//
)
{
	/*

	��� ������ ���� � ������� �������� ��� ������:
	1) ������ ������� ������. � ������ ������ �������� ����� 0
	� ������ ������ ����� ������ 

	2) ������ ������ ��� �� �������� �����
	��� �������� ������
	�������������� � ������ ����� ������� ������� ������ �.�. ������ ����� ��������
	��� ����� ������ ������� �������������� ������

	��� ���� �� ��������� ������. �������� �� ������� ������� �������������� ������ ����
	�� �� ����� ������� ��� ���� �������. ���� ������� ��� ��� ������� ������ ������ ��� 
	�������� ������ � ���������� ���� ������� �������������� ������. ����� ���������
	��������������� ��������� ��-�� ����������� ���� �� ����������� ������.

	3) ������ ������ ������ �������� �����
	���� ���������� ������� �� �������� ������ ��
	�������������� � ������ ����� ������� ������� ������ �.�. ������ ����� ��������
	���� �� �� ����� ������� ������� �� �������������� ������. 

	-------------
	��������� �����:

	� ������� ������ ������� ����� ����� �������������� ������ ���� ������� �������� ������.
	��� ����� �������� � � ������ ���������� � �� �� ������ ������ �������.
	������� ����� � ��� ��� �� �������������� ������ �������� ����� ����� �� ��������� �����
	������������ � ����� ������ �������.

	��� ��� �� ������� ��������� ���������� ������ ���� ������� �������� ������,
	� ��� ����� ������� ������.
	����� ���������� ��� ����� ��� �����

	��� ���������� �����
	�� �� ���������� ���������� ������� ���������� ����� ������ ��� ��� ������
	������ ������ ���� ������ ��� ��� ������ � �������� ������ ���� �� ����� ����� ���������
	���. ��� ������� ���� �� ���������� ������� ���� �� ������ � ������ �� �������� � ������
	�������� ����� � ������ �� ������ ������� ���� �������. ������ ������� �� ����� ������ 
	������������ ������ ��� ����� ����� ���� �� ������� ���� ��� ��� ������ ���������� ����.

	������ ������� � ��� ��� ���� �� ����� ��������� ���� �� ���������� ������� �� ����� ��� 
	�� ���� ���������� ������� � ���������� ���� ������ ������� � ������������ �������� ���� 
	� pv ��������

	������ ������� ��� �� � ����� ������ ��-�� ������� ����������� � ��������� �������.
	� ����� ��� ���������� ����� � ��������� ��� ������ �������� ������ �.�. ���������
	� ������� �������
	*/
  
	//1) ������ ������
	if (p_hash_moves[ind].key_lock_32_move == 0)
	{
		// ���������� ������ ��� � ����� ������
		save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	}//2) �� �� �������
	else if (p_hash_moves[ind].key_lock_32_move == key_lock_32) 
	{
		// ����� ������� �����
		if (age == p_hash_moves[ind].move_age) 
		{
			//��� �������������� ���� ������� �������� ������ ��� �����
			if (delta_depth >= p_hash_moves[ind].move_delta_depth)
			{
				save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
			}
		}
		else//����� ���-�� �� ������� ��������� �������� ������ (���� � ������ ��� ��� �� �� �������)
		{
			// ����� ������� �������������� ������
			save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
		}

	}//3) ������ �������
	else if (p_hash_moves[ind].key_lock_32_move != key_lock_32) 
	{
		// ����� ������� �����
		if (age == p_hash_moves[ind].move_age) 
		{
			//��� �������������� ���� ������� �������� ������ ��� �����
			if (delta_depth >= p_hash_moves[ind].move_delta_depth)
			{
				save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
			}
		}
		else
		{
			// ����� ������� �������������� ������
			save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
		}
	}

}

//	===================================================================
// �������� ������ �� ������ ������ ��� 
inline void HASHM::save_hash_move
(
	unsigned __int8 i,// ����� ���� �� ������
	unsigned __int32 key_lock_32,//
	struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
	unsigned __int8 delta_depth,// ������� ������ ����
	unsigned __int32 ind//   
)
{
	// ���������� ������� 32 ���� �����
	p_hash_moves[ind].key_lock_32_move = key_lock_32;

	// ���������� ������ ���
	p_hash_moves[ind].move = p_list_surplus_moves->move[i];

	// ������� �������� ��� ������� ������� ������ ���
	p_hash_moves[ind].move_delta_depth = delta_depth;

	// ���������� ������� ����
	p_hash_moves[ind].move_age = age;

#if TEST_WRITE_READ
	// ���������� ���������� �������
	hash_move_write = hash_move_write + 1;
#endif // TEST_WRITE_READ

}

//	===================================================================
// �������� ������ �� ������ ������ ������
inline void HASHM::save_best_hash_score
(
	unsigned __int32 key_lock_32,//
	__int32 score,// ������ ����
	unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 delta_depth,// ������� ������ ����
	unsigned __int32 ind//
)
{
	// �������� ������ �� ������ ������ ������

	/*
	��� ������ ������ � ������� �������� ��� ������:
	1) ������ ������� ������. � ������ ������ �������� ����� 0
	� ������ ������ ����� ������ 

	2) ������ ������ ��� �� ��������
	�������������� ���� ������� ������� ������ �.�. ������ ����� ��������

	3) ������ ������ ������ ��������
	���� ������ �������� �� ������� �������, ����� ��������������, �� ��������������
	���� �� �������� � �������������� �� �������������� ������ ���������� ���������.

	(��� ���������� �������������� ������ ��� ����� ������� ����������� ������ �� ���������
	���������.)
	*/
  
	// 1) ������ ������
	if (p_hash_moves[ind].key_lock_32_score == 0)
	{
		// ���������� ������ ��� � ����� ������
		save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);

	}// 2) �� �� ����� ������� �������������� ���� ������� ������ ��� �����
	else if (p_hash_moves[ind].key_lock_32_score == key_lock_32)
	{
		// ������� ����� � ���������� �������� ���� ������� �������� ������
		if (age <= p_hash_moves[ind].score_age + 1)
		{
			//��� �������������� ���� ������� �������� ������ ��� �����
			if(delta_depth >= p_hash_moves[ind].score_delta_depth)
			{
				save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
			}

		}// ����� ������� �������������� ������
		else
		{
			save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
		}

	}// 3) ������ ��������� ������ ��������
	else if (p_hash_moves[ind].key_lock_32_score != key_lock_32)
	{
		// ������� ����� � ���������� �������� ���� ������� �������� ������
		if (age <= p_hash_moves[ind].score_age + 1)
		{
			//��� �������������� ���� ������� �������� ������ ��� �����
			if(delta_depth >= p_hash_moves[ind].score_delta_depth)
			{
				save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
			}

		}// ����� ������� �������������� ������
		else
		{
			save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
		}
	}
}

//	===================================================================
// �������� ������ �.�. ���������� �� � ���-�������
inline void HASHM::save_hash_score
(
	unsigned __int32 key_lock_32,//
	__int32 score,// ������ ����
	unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	unsigned __int8 delta_depth,// ������� ������ ����
	unsigned __int32 ind//
)
{
	// ���������� ������� 32 ���� �����
	p_hash_moves[ind].key_lock_32_score = key_lock_32;

	// ����� ������
	p_hash_moves[ind].score = score;

	// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
	p_hash_moves[ind].flag_hash_score = flag_hash_score;

	// ���������� ������� �������
	p_hash_moves[ind].score_delta_depth = delta_depth;

	// ���������� �������
	p_hash_moves[ind].score_age = age;


#if TEST_WRITE_READ

	if (flag_hash_score == HASHM::LOWER)
	{
		// ���������� ���������� �������
       hash_score_write_low = hash_score_write_low + 1;
	}
	else
	{
		// ���������� ���������� �������
		hash_score_write_high = hash_score_write_high + 1;
	}

#endif // TEST_WRITE_READ

}

//	===================================================================
// ��������� ��� ��� �� ������ ������� � ������
// ���� ��� ������� �� flag_insert_hash_move 2 ����� ���� 1
inline void HASHM::insert_hash_moves
(
	struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
	unsigned __int32 ind,// ���-����
	unsigned __int8 depth ,// �������
	unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
)
{
	__int32 move_s = 0;
	__int32 score_s;// ���������� ��� ����������� ������ ����
	unsigned __int8 j;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    
	static int kolizii = 0; 
	static int enter = 0; 
	int pozition = 0;

	int move_print;

	enter = enter + 1;

#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	flag_insert_hash_move = 1;// � ������� �����

	// 1 ���� ���-��� � ������
	for (j = 0; 
	( (p_list_surplus_moves->move[j] != p_hash_moves[ind].move) 
	&& (j < p_list_surplus_moves->end_list) ); 
	j++);


// ���� ������ � ������������. ����� ����� ����������
#if TEST_H

	//--------------------
	if (j == p_list_surplus_moves->end_list)
	//if (1)
	{	
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
		Test_Hash.open ("_TEST_HASH.txt",std::ios::app);
		//Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

		for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++)
		{
			if(p_hash_moves[ihm].key_lock_32_move != -1) pozition = pozition + 1;
		}

		Test_Hash << "����� ������� "<< max_index_hash <<"\n";
		Test_Hash << "��� ������� ������������ ������� "<< pozition <<"\n";

		move_print = p_hash_moves[ind].move;

		kolizii = kolizii + 1;

		Test_Hash << "��������� "<< enter <<"\n";
		Test_Hash << "�������� "<< kolizii <<"\n";
		Test_Hash << "depth "<< depth <<"\n";
		Test_Hash << "p_hash_moves[].move_delta_depth " << p_hash_moves[ind].move_delta_depth << "\n";
		//Test_Hash << "key "<< key <<"\n";
		Test_Hash << "p_hash_moves[].key " << p_hash_moves[ind].key_lock_32_move << "\n";
		Test_Hash << "p_hash_moves[].move "<< move_print <<"\n";

		//------------------------------------------
		Test_Hash << "������� ������ " << UTIL::public_int_to_char_board(((move_print>>24)& 7),1)<< "\n";
		Test_Hash << "p_hash_moves[].score_move " << p_hash_moves[ind].score << "\n";

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Hash << " " << (UTIL::public_x_to_notation(((move_print>>12)& 63)%8));
		Test_Hash << "" << (1 + ((move_print>>12)& 63)/8);//<< "\n";
		//Test_Hash<< "\n";
		Test_Hash << " " << (UTIL::public_x_to_notation(((move_print>>18)& 63)%8));
		Test_Hash << "" << (1 + ((move_print>>18)& 63)/8)<< "\n";   
		//------------------------------
		//��� ���� 
		//0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
		//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
		//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
		//25 - ������ � ������������ � ����� 
		Test_Hash << "��� ���� " << ((move_print>>6)& 63)  << "\n";
		Test_Hash << "������ ������ " << UTIL::public_int_to_char_board(( move_print & 63),1)<< "\n";
		//Test_Hash << "��������� ��������� " << ((move_print>>12)& 63)  << "\n";
		//Test_Hash << "�������� ��������� " << ((move_print>>18)& 63)  << "\n";
     
		Test_Hash << "\n";
		//------------------------------------------

		Test_Hash << "p_list_surplus_moves->end_list= "<< p_list_surplus_moves->end_list <<"\n";
		Test_Hash << "j= "<< j <<"\n";
		Test_Hash << "-----------------------------------"<<"\n";

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
		Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}	
#endif 


	// ���� ����� ��� ������� �� ���� ������
	if (j < p_list_surplus_moves->end_list)
	{	
		//------------------------------------------------------------------------
		// 2 ���������� ���-��� 
		move_s  = p_list_surplus_moves->move[j];
		score_s = p_list_surplus_moves->sorting_score[j];
		//------------------------------------------------------------------------

		// 3 �������� ��� ������� ����
		// �� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
		for (unsigned __int8 i = j; i > 0; i--)
		{	
			p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
			p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
		}	

		//------------------------------------------------------------------------
		// 4 �� ��������� ������� ���������� ���-���
		p_list_surplus_moves->move[0] = move_s;
		p_list_surplus_moves->sorting_score[0] = score_s;
		//------------------------------------------------------------------------

		flag_insert_hash_move = 2;// ��� �������
	}
}

//	===================================================================
// ��������� ���-�������. �������� ������ ���������� ��������� ������� ������������ � ������� ������
void HASHM::TEST_Hash_index_print()
{

	unsigned __int32 full_move = 0;
	unsigned __int32 full_score = 0;

	unsigned __int32 yes_move = 0;
	unsigned __int32 yes_score = 0;

	unsigned __int32 no_move = 0;
	unsigned __int32 no_score = 0;

	for (unsigned __int32 cnt = 0; cnt <= max_index_hash; cnt++)
	{
		if (p_hash_moves[cnt].key_lock_32_move != 0) yes_move = yes_move + 1 ;

		if (p_hash_moves[cnt].key_lock_32_score != 0) yes_score = yes_score + 1 ;

		if (p_hash_moves[cnt].key_lock_32_move == 0) no_move = no_move + 1 ;

		if (p_hash_moves[cnt].key_lock_32_score == 0) no_score = no_score + 1 ;
	}

	full_move = yes_move + no_move;

	full_score = yes_score + no_score;


	std::cout << "  "<< "\n";
	std::cout << " ������ ���������� ��������� " << max_index_hash << "\n";

	if (full_move != 0)
	{
		std::cout << "full_move = " << full_move << "\n";
	}

	if (full_score != 0)
	{
		std::cout << "full_score = " << full_score << "\n";
	}

	std::cout << "  "<< "\n";

	if (yes_move != 0)
	{
		std::cout << "yes_move = " << yes_move << "\n";
	}

	if (yes_score != 0) 
	{
		std::cout << "yes_score = " << yes_score << "\n";
	}

	std::cout << "  "<< "\n";

	if (no_move != 0)
	{
		std::cout << "no_move = " << no_move << "\n";
	}

	if (no_score != 0)
	{
		std::cout << "no_score = " << no_score << "\n";
	}

	std::cout << "  "<< "\n";

	if (no_move != 0)
	{
		std::cout << " 100 * yes_move/no_move = " << 100 * yes_move/no_move << "\n";
	}

	if (no_score != 0)
	{
		std::cout << " 100 * yes_score/no_score = " << 100 * yes_score/no_score << "\n";
	}

	std::cout << "  "<< "\n";
}
