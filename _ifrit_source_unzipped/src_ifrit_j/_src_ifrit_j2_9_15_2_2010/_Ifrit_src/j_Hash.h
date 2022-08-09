#include "a_structure.h"

//-----------------------------------------------------------------------------------
/*
 * ��������� ���-�������
 *
 * ����� ChessTerminator75
 * ��������� ���������  9.03.2009 20.1.2010
 *
*/
//---------------------------------------------------------------------------

#define TEST_H 0 // ��������-���������(1-0) ����� ������ � ���� �������� ����������

#define TEST_WRITE_READ 0 // ��������-���������(1-0) ����� �������� ����������
                          // ���������� � ��������� �������
                          // ���� ��� �������� ���� TEST_WRITE_READ_WN � c_Iterative_Deepening.cpp

#define U64(u) (u##ui64)

/*******************************************
����� HASHM

�������� � ���-�������� � ������� ��� ���

********************************************/
class HASHM{


// ������������ ���������� ��������� � ���-�������
unsigned __int64 max_index_hash;

// ��������� �� ���-�������(��� ��� �� �����)
struct T_hash_moves * p_hash_moves;

// � ������ ��� ����� ��������� ��������������� ������ ��������� ����� 
// ��������� � ������������� ������ �� ����� � ��� ��� ������ �� ���� �����
// ��� ���������� ������ ���������� �������� ����, ���, ���������� ������
// ������ ���� ���������� � �������� ��������� �����
// � ���������� ��������� ��� ����� �� ������� ��������� ���� ��� �� �����
// ���������� ��������� � ������ ��������
// [���� ������][��� ������][���������� ������]
// 2*6*64= 768 
unsigned __int64 random_piese_key[2][7][64];

// ������ ��� ����������� ���-������ ������� ��� ���������� �� �������
// �.�. � ��� ������������ �� ������ ���� � pv_current �� � ����� �������
// ������� ������� ���������� ����� ������ �������
unsigned __int64 hash_three_moves[120];

// ������ ��� ����������� ���-������ �������
// ��� ������� �������� ������ �� ��������
// ������� ������� ���������� ����� ������ ������� ����������� �� �������� ������ �� ��������
unsigned __int64 hash_string_moves[1000];

// ���������� ���������� ����� � ������
unsigned __int16 save_cnt_move; 

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


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 int hash_save;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//////////////////////////////////////////////////////////////////////////////////////////
public:

// ���� ��� - ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TEST_WRITE_READ

//---------------------------------------------------------------------------
// �������� ��������
void Test_hash_ini();

//---------------------------------------------------------------------------
// �������� ��������
void Test_hash_print();

#endif // TEST_WRITE_READ
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




// ������ � ��� - �������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� ��������� ����������� ������
// ������� ���������� ��� ������� �� ������
__int32 Look_hash_score_white(const unsigned __int64 key,// ���-����
			                  const unsigned __int8 depth,// �������
                              const unsigned __int8 depth_max,// ������������ �������
					          const __int32 alpha,//
		                      const __int32 beta,//
                              unsigned __int8 & test_flag_hash_moves,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
                              unsigned __int8 & hash_depth,// ������������ �������
                              unsigned __int8 & hash_depth_max// ������������ �������
			                  );

//---------------------------------------------------------------------------
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� ��������� ����������� ������
// ������� ���������� ��� ������� �� ������
__int32 Look_hash_score_black(const unsigned __int64 key,// ���-����
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
void Look_hash_move(struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
			        const unsigned __int64 key,// ��� - ����
			        const unsigned __int8 depth,// �������
			        unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
			        );

//---------------------------------------------------------------------------
// �������� ������ �� ������ ������ ��� ��� ������ (������� �� ������)
void Save_best_hash(unsigned __int8 i,// ����� ���� �� ������
				    unsigned __int64 key,// ���-����
				    struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
				    __int32 score,// ������ ����
					unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
					unsigned __int8 depth,// ������� ������ ����
					unsigned __int8 depth_max// ������������ ������� �������
					);

//---------------------------------------------------------------------------
// ����������� ������ �� ��� ���-�������
void Hash_size_free();

//---------------------------------------------------------------------------
// �������� ������ ��� ���-�������. ������ �������� � ����������
void Hash_size_ini_default(unsigned __int16 hash_size// ������ ���-������� � ����������
						   );

//---------------------------------------------------------------------------
// �������������� ��� ����� � ���-������� 0
void Ini_hash_moves();

//---------------------------------------------------------------------------
// ����������� ������ �� ��� ���-������� � �� ����� �������� ������ ��� ���-�������. ������ �������� � ����������
void Hash_size_resize(unsigned __int16 hash_size// ������ ���-������� � ����������
				      );

//---------------------------------------------------------------------------
// ��������� ���-�������. �������� ������ ���������� ��������� ������� ������������ � ������� ������
void TEST_Hash_index_print();

//---------------------------------------------------------------------------
// �������� ������� �������
void Set_age_0();

//---------------------------------------------------------------------------
// ��������� ��� :)
// ����������� ������� �������
void Aging();



// ������ � ��� - ������ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// ������ ���� �� ����� ����
void Modification_random_key(bool  colour_figure,// ���� ������� �������
							 unsigned __int8 i,// ����� ���� �� ������
							 struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
							 struct T_Bitboard * p_bitboard,// ������������� �����
                             struct T_undo undo[]//���������� ��� ������ ����
							 );

//---------------------------------------------------------------------------
// �������������� ���� ��������� �������
void Start_position_random_key(struct T_Bitboard * p_bitboard// ������������� �����
							   );

//---------------------------------------------------------------------------
// ������ ��������� ������� ������ ��������� ������ 
// ����������� ��������� �����
void Ini_random_piese_key();



// ������� ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// ���������� ���������� ����� � ������ ���� ���������� �� ��������
__int32 get_save_cnt_move();

//---------------------------------------------------------------------------
// ���������� ���-���� �� ������� �������
// ����� ��� ��������� �������� � ����� ��������
void Save_hash_three(unsigned __int64 key,// ���-����
					 unsigned __int8 depth // �������
					 );

//---------------------------------------------------------------------------
// ���������� ���-���� ��� �������� ����
// ����� ��� ��������� �������� � ����� ������ ���������� ���������
void Save_hash_string_moves(unsigned __int64 key,// ���-����
					        unsigned __int16 cnt_move // ������� �����
					        );

//---------------------------------------------------------------------------
// ����������� ����� ������ ���������� �� ������ ������. ���� ����� ���������� �� ���������� 1 ����� 0
// ������� ���������� �� ������ ���� ����������� ���� ����� ��� ��� �����
bool Analysis_hash_string_moves(unsigned __int8 depth // �������
						        );

//---------------------------------------------------------------------------
// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
void HASHM::Ini_hash_three();

//---------------------------------------------------------------------------
// �������������� ���-������ �������� ������������� ������� ��� �� ��������� ������ ������������
void HASHM::Ini_hash_string_moves();


//////////////////////////////////////////////////////////////////////////////////////////
private:

// ������ � ��� - �������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// �������� ������ �� ������ ������ ��� 
void Save_best_hash_move(unsigned __int8 i,// ����� ���� �� ������
						 unsigned __int32 key_lock_32,//
						 struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
						 unsigned __int8 delta_depth,// ������� ������ ����
                         unsigned __int32 ind//
						 );

//---------------------------------------------------------------------------
// �������� ��� �.�. ���������� ��� � ���-�������
void Save_hash_move(unsigned __int8 i,// ����� ���� �� ������
				    unsigned __int32 key_lock_32,//
					struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
				    unsigned __int8 delta_depth,// ������� ������ ����
                    unsigned __int32 ind//  
				    );

//---------------------------------------------------------------------------
// �������� ������ �� ������ ������ ������
void Save_best_hash_score(unsigned __int32 key_lock_32,//
						  __int32 score,// ������ ����
						  unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
						  unsigned __int8 delta_depth,// ������� ������ ����
                          unsigned __int32 ind//
						  );

//---------------------------------------------------------------------------
// �������� ������ �.�. ���������� �� � ���-�������
void Save_hash_score(unsigned __int32 key_lock_32,//
					 __int32 score,// ������ ����
				     unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
				     unsigned __int8 delta_depth,// ������� ������ ����
                     unsigned __int32 ind//  
				     );


//---------------------------------------------------------------------------
// ��������� ���-��� �� ������ ������� � ������ ����� ����������� ��� ������ �������
void Insert_hash_moves(struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
					   unsigned __int32 ind,// ��� - ����
					   unsigned __int8 depth ,// �������
					   unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
					   );

//---------------------------------------------------------------------------

// ������ � ��� - ������ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// ���������� ��������� 64 ������ ����
unsigned __int64 rand_64();


// ������� ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ��������
void Print_hash_moves();

};//class HASHM



