#include <iostream>
#include <fstream>  // TEST
#include <stdlib.h>
using namespace std;

#include "j_Hash.h"
#include "k_Util.h"


//-----------------------------------------------------------------------------------
/* ��������� ���-�������
 *
 * ����� ChessTerminator75
 * ��������� ���������  2.11.2009 28.1.2010
 *
*/
//---------------------------------------------------------------------------

//extern class UTIL util_o;

// ������ ��� �����
#define N_KING    6 // ������
#define N_QUEEN   5 // �����
#define N_ROOK    4 // �����
#define N_BISHOP  3 // ����
#define N_KNIGHT  2 // ����
#define N_PAWN    1 // �����

 //           ��� ����
 #define NO_MOVE 0 // ��� ����
 
 #define SIMPLE_MOVE 1 //������� ���
 #define CAPTURE_MOVE 2 //������
 #define LONG_CASTLING 3 //������� ���������
 #define SHORT_CASTLING 4 //�������� ���������
 #define TAKING_PASSAGE 5 //������ �� �������

 #define TRANSFORMATION_IN_KNIGHT 12 //����������� � ���� 
 #define TRANSFORMATION_IN_BISHOP 13 //����������� � ����
 #define TRANSFORMATION_IN_ROOK  14 //����������� � ����� 
 #define TRANSFORMATION_IN_QUEEN 15 //����������� � ����� 

 #define CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT 22 //������ � ������������ � ���� 
 #define CAPTURE_WITH_TRANSFORMATION_IN_BISHOP 23 //������ � ������������ � ����
 #define CAPTURE_WITH_TRANSFORMATION_IN_ROOK 24 //������ � ������������ � ����� 
 #define CAPTURE_WITH_TRANSFORMATION_IN_QUEEN 25 //������ � ������������ � ����� 

// ����������� � ������ g_Loop.cpp
 #define EXACT 1 // ������ ���
 #define LOWER 2 // ������� �� ����(�����)
 #define UPPER 3 // �����(����) �������� �� �������

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



// global

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




// ���� ��� - ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TEST_WRITE_READ

//	===================================================================
// �������� ��������
 void HASHM::Test_hash_ini(){
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

 }//void HASHM::Test_hash_ini()
//	*******************************************************************

//	===================================================================
// �������� ��������
void HASHM::Test_hash_print(){


cout << " " << "\n";

hash_write_all = hash_move_write + hash_score_write_low + hash_score_write_high;

// �������� ���������� ���� ���������� �������
cout << "hash_write_all = " << hash_write_all << "\n";

if (hash_write_all != 0){
// ���������� ���������� ������� ��� ���������� �����
cout << "hash_move_write = " << hash_move_write << "   100 * hash_move_write/hash_write_all = " << 100 * hash_move_write/hash_write_all << "\n";

// ���������� ���������� ������� ��� ������� �� ������
cout << "hash_score_write_low = " << hash_score_write_low << "   100 * hash_score_write_low/hash_write_all = " << 100 * hash_score_write_low/hash_write_all << "\n";

// ���������� ���������� ������� ��� ������� �� ������
cout << "hash_score_write_high = " << hash_score_write_high << "   100 * hash_score_write_high/hash_write_all = " << 100 * hash_score_write_high/hash_write_all << "\n";

cout << " " << "\n";

// �������� ���������� �������������� ��� ���������� �������
cout << "hash_read_sorting = " << hash_read_sorting << "   100 * hash_read_sorting/hash_write_all = " << 100 * hash_read_sorting/hash_write_all << "\n";

}

// ������ ����� ����������� � ���������������
cout << "hash_write_all - hash_read_sorting = " << (hash_write_all - hash_read_sorting) << "\n";


cout << " " << "\n";

hash_read_score = hash_read_score_LOWER + hash_read_score_UPPER;

if( hash_read_score != 0){

if (hash_write_all != 0){
   // �������� ���������� ���� ��������� �������
   cout << "hash_read_score = " << hash_read_score << "   100 * hash_read_score/hash_write_all = " << 100 * hash_read_score/hash_write_all << "\n";

   // �������� ���������� ���� ��������� �������
   cout << "hash_read_all = " << (hash_read_sorting + hash_read_score) << "   100 * hash_read_all/hash_write_all = " << 100 * (hash_read_sorting + hash_read_score)/hash_write_all << "\n";
}
   // �������� ���������� �������������� ��� ������� �� ������ �������
   cout << "hash_read_score_LOWER = " << hash_read_score_LOWER <<  "   100 * hash_read_score_LOWER/hash_read_score = " << 100 * hash_read_score_LOWER/hash_read_score << "\n";

   // �������� ���������� �������������� ��� ������� �� ������ �������
   cout << "hash_read_score_UPPER = " << hash_read_score_UPPER <<  "   100 * hash_read_score_UPPER/hash_read_score = " << 100 * hash_read_score_UPPER/hash_read_score << "\n";


}//if( hash_read_score != 0)

cout << " " << "\n";

 }// void HASHM::Test_hash_print()
//	*******************************************************************

#endif // TEST_WRITE_READ
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// ������ � ��� - �������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// �������� ������ ��� ���. ������ �������� � ����������
 void HASHM::Hash_size_ini_default(unsigned __int16 hash_size// ������ ���-������� � ����������
						           ){
unsigned __int64 hash_n = 0;

if (hash_size < 1) hash_size = 1;// ������ �� ������ ���������
if (hash_size > 1024) hash_size = 1024;// ������ �� ������ ���������

// ������� sizeof(struct T_hash_moves) ������ ������ � ������ �.�. int ��� 4
// � ������ ������ struct T_hash_moves - 16 ����
// ���������� ������ � �� � ����� � ����� � ���������� ���������
max_index_hash = (unsigned __int64)((hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));
        // cout << "max_index_hash in 1 mb = " << max_index_hash << "\n";
        // cout << "sizeof(T_hash_moves) = " << sizeof(struct T_hash_moves) << "\n";
         //cout << "sizeof(unsigned int) = " << sizeof(unsigned int) << "\n";

// ��� ����� �������� ������ (2 � ������� n) -1 ��� �� ���������� 
// ����� ���� 000..0111 � ����� ���� ������� ��� - ������ � ������� ������� ��������� &
for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

hash_n = hash_n / 2;

if ((hash_n & (hash_n - 1)) != 0) cout << "������! ������ �� ���� ������� ���� " << hash_n << "\n";

max_index_hash = hash_n - 1;

// calloc
// Allocates a block of memory for an array of num elements, 
// each of them size bytes long, and initializes all its bits to zero.
// ����������� ������ ��� ������� � ��������� �� ������
p_hash_moves = (struct T_hash_moves*) calloc ((__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

if (p_hash_moves == NULL) {
    cout << "������ ��� " << "\n";
	exit(1); 
}//if (p_hash_moves == NULL) {

    // TEST
    //cout << "sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
    //cout << "max_index_hash = " << max_index_hash << "\n";
    //cout << "hash_n = " << hash_n << "\n";

}//void HASHM::Hash_size_ini_default
//	*******************************************************************

//	===================================================================
// ����������� ������ �� ��� ���-������� 
// � �� ����� �������� ������ ��� ���. ������ �������� � ����������
void HASHM::Hash_size_resize(unsigned __int16 hash_size// ������ ���-������� � ����������
				          ){

unsigned __int64 hash_n = 0;

if (p_hash_moves == NULL) {
    cout << "����������� ������ " << "\n";
	exit(1); 
}//if (p_hash_moves == NULL) {

free(p_hash_moves);// ����������� ���������� ����� ������

if (hash_size < 1) hash_size = 1;// ������ �� ������ ���������
if (hash_size > 1024) hash_size = 1024;// ������ �� ������ ���������

// ���������� ������ � �� � ����� � ����� � ���������� ���������
max_index_hash = (unsigned __int64)((hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));

    // TEST
//    cout << "��������������� max_index_hash = " << max_index_hash << "\n";
 

// ��� ����� �������� ������ (2 � ������� n) -1 ��� �� ���������� 
// ����� ���� 000..0111 � ����� ���� ������� ��� - ������ � ������� ������� ��������� &
for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

hash_n = hash_n / 2;

if ((hash_n & (hash_n - 1)) != 0) cout << "������! ������ �� ���� ������� ���� " << hash_n << "\n";

max_index_hash = hash_n - 1;

// ����������� ������ ��� ������� � ��������� �� ������
p_hash_moves = (struct T_hash_moves*) calloc ((__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

if (p_hash_moves == NULL) {
    cout << "������ ��� " << "\n";
	exit(1); 
}//if (p_hash_moves == NULL) {

// TEST
 //cout << " ������ ���-������� � ���������� hash_size = " << hash_size << "\n";
 //cout << " � ����� �������� ������ � �� = " << ((int)(max_index_hash + 3) * sizeof(T_hash_moves))/(1024 * 1024) << "\n";
 //cout << "������ �������� � ������ sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
 //cout << "����� ������� 2 hash_n = " << hash_n << "\n";
 //cout << "���������� ��������� � ���-������� max_index_hash = " << max_index_hash << "\n";

}//void HASHM::Hash_size_resize
//	*******************************************************************

//	===================================================================
// ����������� ������ �� ��� ���-�������
void HASHM::Hash_size_free(){

   free(p_hash_moves);

}//void HASHM::Hash_size_free
//	*******************************************************************

//	===================================================================
// �������������� ��� ����� ���-������� 0
void HASHM::Ini_hash_moves(){

              //TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
                 #if TEST_H
                     hash_save = 0;
                 #endif 
                 //TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
                                                             
for(unsigned __int32 ihm = 0; ihm <= max_index_hash; ihm++){

   p_hash_moves[ihm].key_lock_32_move = 0;

   p_hash_moves[ihm].key_lock_32_score = 0;


}//for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){

}//void HASHM::Ini_hash_moves
//	*******************************************************************

//	===================================================================
// �������� ������ �� ������ ������ ��� ��� ������ (������� �� ������)
void HASHM::Save_best_hash(unsigned __int8 i,// ����� ���� �� ������
						   unsigned __int64 key,// ���-����
						   struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
						   __int32 score,// ������ ����
						   unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
						   unsigned __int8 depth,// ������� ������ ����
						   unsigned __int8 depth_max// ������������ ������� �������
						   ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);
unsigned __int8 delta_depth = depth_max - depth;

if (delta_depth < 0) delta_depth = 0;

/*
������ ������� ���� � ������ ������ ���������
������ ��� ��� ���������� � ����� ������ �� ����� �������������� ����������

�� ����� ���� �� ����� ��� ������� � ���� ������ ���-�������
��� ���������� � ���� �������� ���������� �������
����� ������ ������� ����� ������� � ���� � �� �� ������

*/
 // ������ ������� ����
 Save_best_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

 // ������ ������ ������
 Save_best_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);


}//void HASHM::Save_best_hash_key
//	*******************************************************************

//	===================================================================
// �������� ������ �� ������ ������ ���
void HASHM::Save_best_hash_move(unsigned __int8 i,// ����� ���� �� ������
						        unsigned __int32 key_lock_32,//
						        struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
						        unsigned __int8 delta_depth,// ������� ������ ����
                                unsigned __int32 ind//
						        ){

/*

��� ������ ���� � ������� �������� ��� ������:
1) ������ ������� ������. � ������ ������ �������� ����� 0
   � ������ ������ ����� ������ 

2) ������ ������ ��� �� �������� �����
         �������������� � ������ ����� ������� ������� ������ �.�. ������ ����� ��������

3) ������ ������ ������ �������� �����
     ���� ���������� ������� �� �������� � ����������� ������ ��
         �������������� � ������ ����� ������� ������� ������ �.�. ������ ����� ��������
     ���� �� �� ����� ������� ������� �� �������������� ������. 

-------------
��������� �����:

� ������� ������ ������� ����� ����� �������������� ������ ���� ������� �������� ������.
��� ����� �������� � � ������ ���������� � �� �� ������ ������ �������.
������� ����� � ��� ��� �� �������������� ������ �������� ����� ����� �� ��������� �����
������������ � ����� ������ �������.

��� ��� �� ������� ��������� ���������� ������ ���� ������� �������� ������
� ������ ���������� ��� ������ ����������� ���� �� ������� � ��� ����� ������� ������.
����� ���������� ��� ����� ��� �����

��� ���������� �����
�� �� ���������� ���������� ������� ���������� ����� ������� ������� ������ ��� ��� ������
������ ������ ���� ������ ��� ��� ������ � �������� ������ ���� �� ����� ����� ���������
���. ��� ������� ���� �� ���������� ������� ���� �� ������ � ������ �� �������� � ������
�������� ����� � ������ �� ������ ������� ���� �������. ������ ������� �� ����� ������ 
������������ ������ ��� ����� ����� ���� �� ������� ���� ��� ��� ������ ���������� ����.

*/
  
// 1) ������ ������------------------------------------------------------------------------
if(p_hash_moves[ind].key_lock_32_move == 0){

    // ���������� ������ ��� � ����� ������
    //-----------------------------------------------------------------------
    Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
    //-----------------------------------------------------------------------

// 2) �� �� �������------------------------------------------------------------------------
}else if(p_hash_moves[ind].key_lock_32_move == key_lock_32) {

     // ����� ������� �����
     if( age == p_hash_moves[ind].move_age ) {

         //��� �������������� ���� ������� �������� ������ ��� �����
		 if( delta_depth >= p_hash_moves[ind].move_delta_depth  ){ 

             Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

		 }//if( delta_depth >= p_hash_moves[ind].move_delta_depth  )

	 }else{
         // ����� ������� �������������� ������
         Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	 }// if( age == p_hash_moves[ind].age ) 

// 3) ������ �������------------------------------------------------------------------------
}else if(p_hash_moves[ind].key_lock_32_move != key_lock_32) {

     // ����� ������� �����
     if( age == p_hash_moves[ind].move_age ) {

         //��� �������������� ���� ������� �������� ������ ��� �����
		 if( delta_depth >= p_hash_moves[ind].move_delta_depth  ){ 

              Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

		 }//if( delta_depth >= p_hash_moves[ind].move_delta_depth  )

	 }else{

          // ����� ������� �������������� ������
          Save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	 }// if( age == p_hash_moves[ind].age ) 

}//if(p_hash_moves[ind].key_lock_32_move == 0){---------------------------------------------

}//void HASHM::Save_best_hash_key
//	*******************************************************************

//	===================================================================
// �������� ������ �� ������ ������ ��� 
void HASHM::Save_hash_move(unsigned __int8 i,// ����� ���� �� ������
						   unsigned __int32 key_lock_32,//
						   struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
						   unsigned __int8 delta_depth,// ������� ������ ����
                           unsigned __int32 ind//   
				           ){

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


}//void HASHM::Save_hash_key
//	*******************************************************************

//	===================================================================
// �������� ������ �� ������ ������ ������
void HASHM::Save_best_hash_score(unsigned __int32 key_lock_32,//
						         __int32 score,// ������ ����
						         unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
						         unsigned __int8 delta_depth,// ������� ������ ����
                                 unsigned __int32 ind//
						         ){


	// �������� ������ �� ������ ������ ������

/*

��� ������ ������ � ������� �������� ��� ������:
1) ������ ������� ������. � ������ ������ �������� ����� 0
   � ������ ������ ����� ������ 

2) ������ ������ ��� �� ��������
   ���� ���������� ������� �� �������� ������ ��
   �������������� ���� ������� ������� ������ �.�. ������ ����� ��������


3) ������ ������ ������ ��������
   ���� ������ �������� �� ������� �������, ����� ��������������, �� ��������������
   ���� �� �������� � �������������� �� �������������� ������ ���������� ���������.

   (��� ���������� �������������� ������ ��� ����� ������� ����������� ������ �� ���������
   ���������.)

*/
  
// 1) ������ ������
if(p_hash_moves[ind].key_lock_32_score == 0){

       // ���������� ������ ��� � ����� ������
       Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);


// 2) �� �� ����� ������� �������������� ���� ������� ������ ��� �����
}else if( p_hash_moves[ind].key_lock_32_score == key_lock_32){

              //��� �������������� ���� ������� �������� ������ ��� �����
		      if( delta_depth >= p_hash_moves[ind].score_delta_depth ){ 

                    Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);

		      }//if( delta_depth >= p_hash_moves[ind].score_delta_depth )

 
// 3) ������ ��������� ������ ��������
}else if(p_hash_moves[ind].key_lock_32_score != key_lock_32){

	          // ������� ����� � ���������� �������� ���� ������� �������� ������
	          if( age <= p_hash_moves[ind].score_age + 1) { // 

                     //��� �������������� ���� ������� �������� ������ ��� �����
		             if( delta_depth >= p_hash_moves[ind].score_delta_depth ){

                         Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);

		             }//if( delta_depth >= p_hash_moves[ind].score_delta_depth )


			  }else{
                     // ����� ������� �������������� ������
                     Save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);


			  }//if( age <= p_hash_moves[ind].score_age + 1)


}//if(p_hash_moves[ind].key_lock_32_score == 0)

}//void HASHM::Save_best_hash_key
//	*******************************************************************


//	===================================================================
// �������� ������ �.�. ���������� �� � ���-�������
void HASHM::Save_hash_score(unsigned __int32 key_lock_32,//
						    __int32 score,// ������ ����
						    unsigned __int8 flag_hash_score,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
						    unsigned __int8 delta_depth,// ������� ������ ����
                            unsigned __int32 ind//
				            ){

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

  if (flag_hash_score == LOWER){
       // ���������� ���������� �������
       hash_score_write_low = hash_score_write_low + 1;
  }else{
       // ���������� ���������� �������
       hash_score_write_high = hash_score_write_high + 1;
  }//

#endif // TEST_WRITE_READ


}//void HASHM::Save_hash_key
//	*******************************************************************


//	===================================================================
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� ��������� ����������� ������
// ����� �� �����
__int32 HASHM::Look_hash_score_white(const unsigned __int64 key,// ���-����
			                         const unsigned __int8 depth,// �������
                                     const unsigned __int8 depth_max,// ������������ �������
					                 const __int32 alpha,//
		                             const __int32 beta,//
                                     unsigned __int8 & test_flag_hash_moves,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
                                     unsigned __int8 & hash_depth,// ������������ �������
                                     unsigned __int8 & hash_depth_max// ������������ �������
			                         ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);


/*
(�������� �� ������� �����)

��� ������������� ������ �� ���-������� �������� ��� ������:

1) (� ���� ��������� ���� ������ �� ��������)
   EXACT (��������� score <  beta )
   ��� �������� �������� ������ ������ �.�. beta > score > alpha
   ��� ��� �� ������� � ������� ����� � ����� ���� �-� ������� ����� �������
   �� ������ ������ �� ����� ���� �� ����� �����
   ������� ������ ��� �� ������������ ��������� ��� ��� ��� ��� ��� �� �����.
   ��������� ������ �� score <  beta  
   ������ ��� ������ ��� alpha ��� ������ �� �����
   ��� ��� � ������ score < alpha �� ������ �������� 3-�� �������

2) LOWER (��������� score >= beta )
   ��� �������� �������� ������ ������� ������� ������� �� beta. ��� �������
   �.�. ��� ���������� ����� ������ �� ������ ����������.
   ��� ������� ������ ��� �� ��� � ������ �������� ������� �� ���� ��� ���
   ��������� ������ score >= beta

3) UPPER (��������� score < alpha )
   ��� �������� ������ �� �������� ����� � �� ������� ������� �� ����. ��� ���������
   ��������. �������� ������ �� �� �����. �� ��� � �� ������� �� �������� ������� 
   ��� �� ��� �� �������� �� �������� �����. 
   ��� ��� ��������� score < alpha
   ������� ���������� ����� score <= alpha �� ��� ������������ � �� ���� ��������� ���� ������

*/

// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
if(p_hash_moves[ind].key_lock_32_score == key_lock_32){

  // ���� ������� ��� ������������ �� ����� �������� ������ ��� ������ ��� �� �������
  // �� ���������� ����������� �� ����� ������ ������� ������ ��������
  if( p_hash_moves[ind].score_delta_depth >= (depth_max - depth)){


    if (p_hash_moves[ind].flag_hash_score == LOWER){
       // ���� ��������� ��������� �� ������������ ������� ����� � � ������ ������
       // �� ��� - ������� ����� �� ������ ������ � �� ��� ������� �����. ���� �� ����� ��� 
	   // ��������� �� ����� ��������� ������ �� ���������. �� ��� �� ��� ����� ������ ��� ��
	   // ��� �������� ����� � � ������ ������.
		   if( p_hash_moves[ind].score >= beta ){ 

			   #if TEST_WRITE_READ
                  // 
                  hash_read_score_LOWER = hash_read_score_LOWER + 1;
               #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if( p_hash_moves[ind].score >= beta )
    
//----------------------------

	}else{//if (p_hash_moves[ind].flag_hash_score == LOWER)

// ��������� ��� ������ ��� ��� �� �������� �����
// ���� ������� ��� ���� ������� ��� ���������� ���� � ������
// � ���� ���� � � ��������� � ����� �� � ������� ���� � ������� ��������� 
// ��� ����� ���� ��� � ������ ����� ���� �������
// ������ ����� ����������� � ������� �� ������� ����� ������ ������� <= � �����������
               if(p_hash_moves[ind].score < alpha) { 

			      #if TEST_WRITE_READ
                    // 
                    hash_read_score_UPPER = hash_read_score_UPPER + 1;
                  #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if(p_hash_moves[ind].score_high < alpha)

	}//if (p_hash_moves[ind].flag_hash_score == LOWER)

    }//if( p_hash_moves[ind].score_delta_depth >= (depth_max - depth)){

}//if(p_hash_moves[ind].key_lock_32_score == key_lock_32)

return -1000000000;

}// void HASHM::Look_hash
//	*******************************************************************

//	===================================================================
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� ��������� ����������� ������
//����� �� ������
__int32 HASHM::Look_hash_score_black(const unsigned __int64 key,// ���-����
			                         const unsigned __int8 depth,// �������
                                     const unsigned __int8 depth_max,// ������������ �������
					                 const __int32 alpha,//
		                             const __int32 beta,//
                                     unsigned __int8 & test_flag_hash_moves,// ���� ���� (1- �������� �����(����); 3-������ ��������� ��������(�������); 2-������� �� ����(�����))
                                     unsigned __int8 & hash_depth,// ������������ �������
                                     unsigned __int8 & hash_depth_max// ������������ �������
			                         ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);


if(p_hash_moves[ind].key_lock_32_score == key_lock_32 ){

  if(p_hash_moves[ind].score_delta_depth >= (depth_max - depth)){

     if (p_hash_moves[ind].flag_hash_score == LOWER){

			if(p_hash_moves[ind].score <= alpha) { 

			   #if TEST_WRITE_READ
                  // 
                  hash_read_score_LOWER = hash_read_score_LOWER + 1;
               #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if(p_hash_moves[ind].score_move <= alpha)

	 }else{//if (p_hash_moves[ind].flag_hash_score == LOWER)

//---------------------------
            if(p_hash_moves[ind].score > beta) { 
			      #if TEST_WRITE_READ
                    // 
                    hash_read_score_UPPER = hash_read_score_UPPER + 1;
                  #endif // TEST_WRITE_READ

			   return p_hash_moves[ind].score;
		   }//if(p_hash_moves[ind].score_move > beta)

      }//if (p_hash_moves[ind].flag_hash_score == LOWER)
   }//if(p_hash_moves[ind].score_delta >= (depth_max - depth))

}//if(p_hash_moves[ind].key_lock_32_score == (unsigned __int32)(key >>32) )

return -1000000000;

}// void HASHM::Look_hash
//	*******************************************************************

//	===================================================================
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
void HASHM::Look_hash_move(struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
			               const unsigned __int64 key,// ���-����
			               const unsigned __int8 depth,// �������
			               unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
			               ){

unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);

// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
if(p_hash_moves[ind].key_lock_32_move == (unsigned __int32)(key >> 32) ){
     
#if TEST_WRITE_READ
  // ���������� ��������� ��� ���������� �������
  hash_read_sorting = hash_read_sorting + 1;
#endif // TEST_WRITE_READ

      // ������� ��� �� ������ ������� � ������
      Insert_hash_moves(p_list_surplus_moves,ind,depth,flag_insert_hash_move);

}//if(p_hash_moves[key & max_index_hash ].key == key ){


}// void HASHM::Look_hash
//	*******************************************************************


//	===================================================================
// ��������� ��� ��� �� ������ ������� � ������
// ���� ��� ������� �� flag_insert_hash_move 2 ����� ���� 1
void HASHM::Insert_hash_moves(struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
					          unsigned __int32 ind,// ���-����
					          unsigned __int8 depth ,// �������
					          unsigned __int8  & flag_insert_hash_move//���� ���� ��� ������� ����� ���-��� � ������� �� ������ �������
					          ){


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
for(j = 0; 
   ( (p_list_surplus_moves->move[j] != p_hash_moves[ind].move) 
   && (j < p_list_surplus_moves->end_list) ); 
   j++);


//
#if TEST_H

//--------------------
if (j == p_list_surplus_moves->end_list){	
//if (1){	
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.open ("_TEST_HASH.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){
    if(p_hash_moves[ihm].key != -1)  pozition = pozition + 1;
}//for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){

Test_Hash << "����� ������� "<< max_index_hash <<"\n";
Test_Hash << "��� ������� ������������ ������� "<< pozition <<"\n";

move_print = p_hash_moves[key & max_index_hash ].move;

kolizii = kolizii + 1;

 Test_Hash << "������ "<< hash_save <<"\n";
 Test_Hash << "��������� "<< enter <<"\n";
 Test_Hash << "�������� "<< kolizii <<"\n";
 Test_Hash << "depth "<< depth <<"\n";
 Test_Hash << "p_hash_moves[].depth " << p_hash_moves[key & max_index_hash ].depth << "\n";
 Test_Hash << "p_hash_moves[].depth_max " << p_hash_moves[key & max_index_hash ].depth_max << "\n";
 Test_Hash << "p_hash_moves[].flag_hash_moves " << p_hash_moves[key & max_index_hash ].flag_hash_moves << "\n";
 Test_Hash << "key "<< key <<"\n";
Test_Hash << "p_hash_moves[].key " << p_hash_moves[key & max_index_hash ].key << "\n";
 Test_Hash << "p_hash_moves[].move "<< move_print <<"\n";

//------------------------------------------
     Test_Hash << "������� ������ " << util_o.int_to_char_board(((move_print>>24)& 7),1)<< "\n";
     Test_Hash << "p_hash_moves[].score_move " << p_hash_moves[key & max_index_hash ].score_move << "\n";

     // ����� ���� ���� e2-e4
     //------------------------------
	 Test_Hash << " " << (util_o.x_to_notation(((move_print>>12)& 63)%8));
     Test_Hash << "" << (1 + ((move_print>>12)& 63)/8);//<< "\n";
 //  Test_Hash<< "\n";
     Test_Hash << " " << (util_o.x_to_notation(((move_print>>18)& 63)%8));
     Test_Hash << "" << (1 + ((move_print>>18)& 63)/8)<< "\n";   
     //------------------------------
 //��� ���� 
 //0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ��������� 4 � �������� ��������� 5 � ������ �� �������
 //12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
 //22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
 //25 - ������ � ������������ � ����� 
     Test_Hash << "��� ���� " << ((move_print>>6)& 63)  << "\n";
     Test_Hash << "������ ������ " << util_o.int_to_char_board(( move_print & 63),1)<< "\n";
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

}//if (j == p_list_surplus_moves->end_list){	
//--------------------
#endif 


// ���� ����� ��� ������� �� ���� ������
if(j < p_list_surplus_moves->end_list){	
//------------------------------------------------------------------------
// 2 ���������� ���-��� 
  move_s  = p_list_surplus_moves->move[j];
  score_s = p_list_surplus_moves->sorting_score[j];
//------------------------------------------------------------------------

  // 3 �������� ��� ������� ����
  // �� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
  for(unsigned __int8 i = j; i > 0; i--){	
 	 p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
     p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
  }//for(int i = j; i > start; i--){	

  //------------------------------------------------------------------------
  // 4 �� ��������� ������� ���������� ���-���
  p_list_surplus_moves->move[0] = move_s;
  p_list_surplus_moves->sorting_score[0] = score_s;
  //------------------------------------------------------------------------

  flag_insert_hash_move = 2;// ��� �������

}//if(j < p_list_surplus_moves->end_list)


}//void HASHM::Insert_hash_moves
//	*******************************************************************

//	===================================================================
// ��������� ���-�������. �������� ������ ���������� ��������� ������� ������������ � ������� ������
void HASHM::TEST_Hash_index_print(){

unsigned __int32 full_move = 0;
unsigned __int32 full_score = 0;

unsigned __int32 yes_move = 0;
unsigned __int32 yes_score = 0;

unsigned __int32 no_move = 0;
unsigned __int32 no_score = 0;

   for(unsigned __int32 cnt = 0; cnt <= max_index_hash; cnt++){

      if(p_hash_moves[cnt].key_lock_32_move != 0)  yes_move = yes_move + 1 ;

      if(p_hash_moves[cnt].key_lock_32_score != 0)  yes_score = yes_score + 1 ;


      if(p_hash_moves[cnt].key_lock_32_move == 0)  no_move = no_move + 1 ;

      if(p_hash_moves[cnt].key_lock_32_score == 0)  no_score = no_score + 1 ;


   }//for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++){

full_move = yes_move + no_move;

full_score = yes_score + no_score;


 cout << "  "<< "\n";
 cout << " ������ ���������� ��������� " << max_index_hash << "\n";

if (full_move != 0) cout << "full_move = " << full_move << "\n";
if (full_score != 0) cout << "full_score = " << full_score << "\n";
cout << "  "<< "\n";
if (yes_move != 0) cout << "yes_move = " << yes_move << "\n";
if (yes_score != 0) cout << "yes_score = " << yes_score << "\n";
cout << "  "<< "\n";
if (no_move != 0) cout << "no_move = " << no_move << "\n";
if (no_score != 0) cout << "no_score = " << no_score << "\n";
cout << "  "<< "\n";
if (no_move != 0) cout << " 100 * yes_move/no_move = " << 100 * yes_move/no_move << "\n";
if (no_score != 0) cout << " 100 * yes_score/no_score = " << 100 * yes_score/no_score << "\n";
cout << "  "<< "\n";


}//HASHM::TEST_Hash_index()
//	*******************************************************************

//	===================================================================
// �������� ������� �������
void HASHM::Set_age_0(){

	age = 0;

}//void HASHM::set_age_0()
//	*******************************************************************

//	===================================================================
// ��������� ��� :)
// ����������� ������� �������
void HASHM::Aging(){

	if (age > 250) age = 0;
	age = age + 1;
 //   cout << "age= " << (__int32)age << "\n";

}//void HASHM::aging()
//	*******************************************************************





// ������ � ��� - ������ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// ���������� ��������� 64 ������ ����
unsigned __int64 HASHM::rand_64(){
//� ������ ��������� ��������� ����� ����� ����������� ������� ����������
//<cstdlib>	������������ ������ ��������� ����� � ��������� 0 �� 32767

/*
 int rand ( void ); 
 Returns a pseudo-random integral number in the range 0 to RAND_MAX.
 RAND_MAX is a constant defined in <cstdlib>. Its default value may vary between 
 implementations but it is granted to be at least 32767.
*/

return rand() ^ ( ((unsigned __int64)rand() << 15) ^ ((unsigned __int64)rand() << 31) ^ 
				   ((unsigned __int64)rand() << 47) ^ ((unsigned __int64)rand() << 59));

}//unsigned __int64 HASHM::rand_64
//	*******************************************************************

//	===================================================================
// ������ ��������� ������� ������ ��������� ������ 
// ����������� ��������� �����
void HASHM::Ini_random_piese_key(){

__int32 qq = 0;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.open ("_TEST_HASH.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//[���� ������][��� ������][���������� ������]
 for(unsigned __int8 color_piece = 0; color_piece < 2; color_piece++){

	 for(unsigned __int8 name_piece = 1; name_piece < 7; name_piece++){

		 for(unsigned __int8 coordinate_piece = 0; coordinate_piece < 64; coordinate_piece++){

             random_piese_key[color_piece][name_piece][coordinate_piece] = rand_64();


     //        Test_Hash << random_piese_key[color_piece][name_piece][coordinate_piece] << "\n";
	     }//for(int coordinate_piece = 0; coordinate_piece < 64; coordinate_piece++){

	 }//for(int name_piece = 1; name_piece < 7; name_piece++){

 }//for(int color_piece = 0; color_piece < 2; color_piece++){

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
// ���� �������� � ����� ����������
 for(int c = 0; c < 2; c++){
	 for(int n = 1; n < 7; n++){
		for(int q = 0; q < 64; q++){

 for(int cv = 0; cv < 2; cv++){
	 for(int nv = 1; nv < 7; nv++){
		for(int qv = 0; qv < 64; qv++){
if ( (c != cv) || (n != nv) || (q != qv) ) {
			if ( random_piese_key[c][n][q] == random_piese_key[cv][nv][qv]) {
			   qq = qq + 1;
               Test_Hash << "������ " << qq << "\n";
			}//if ( random_piese_key[c][n][q] == random_piese_key[cv][nv][qv]) {
 }//if ( (c != cv) || (n != nv) || (q != qv) ) {
	    }//for(int qv = 0; qv < 64; qv++){
	 }//for(int nv = 1; nv < 7; nv++){
 }//for(int cv = 0; cv < 2; cv++){

	    }//for(int q = 0; q < 120; q++){
	 }//for(int n = 1; n < 7; n++){
 }//for(int c = 1; c < 3; c++){
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//void HASHM::Ini_random_piese_key
//	*******************************************************************

//	===================================================================
// �������������� ���� ��������� �������
void HASHM::Start_position_random_key(struct T_Bitboard * p_bitboard// ������������� �����                                    
							          ){

unsigned __int8  colour = 0;// ���� ������
unsigned __int8  name = 0;// ��� ������
unsigned __int64 key = 0;// ������� �������� �����

//cout << "--------------------------------------------- " << "\n";

// ���������� �����
//for (int i = 0; i < 64; i++){
//      
//      name = p_bitboard->white_name_from_coord[i];
//	  if(name != 0) {
//		  key = key ^ random_piese_key[1][name][i];
//		  cout << "white_name name = " << name << " i = " << i << "  random_piese_key[1][name][i] = " << random_piese_key[1][name][i] << "\n";
//	  }
//
//   //   name = p_bitboard->black_name_from_coord[i];
//	  //if(name != 0) {
//		 // key = key ^ random_piese_key[0][name][i];
//   //       cout << "black_name name = " << name << "\n";
//	  //}
//
//}//for (int i = 0; i < 64; i++){

for (unsigned __int8 i = 0; i < 64; i++){
      
      name = p_bitboard->white_name_from_coord[i];
	  if(name != 0) {
		  key = key ^ random_piese_key[1][name][i];
	  }

      name = p_bitboard->black_name_from_coord[i];
	  if(name != 0) {
		  key = key ^ random_piese_key[0][name][i];
   //       cout << "black_name name = " << name << " i = " << i << "  random_piese_key[1][name][i] = " << random_piese_key[1][name][i] << "\n";
	  }

}//for (int i = 0; i < 64; i++){

// ��������� ���� ����
// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
// ��������� ������� ���������� ������ ������ ����
if( p_bitboard->colour_move ) key = key ^ U64(0x7C0228DD624EC390);

// ��������� ���������
if( p_bitboard->castling_K ) key = key ^ U64(0xB3598055CE64A656);
if( p_bitboard->castling_Q ) key = key ^ U64(0x12A8F886AF9418C2);
if( p_bitboard->castling_k ) key = key ^ U64(0x1877B67E57A764D5);
if( p_bitboard->castling_q ) key = key ^ U64(0xD4490AD346F14431);

// ��������� ������ �� �������
if( p_bitboard->taking_passage_yes ){
	key = key ^ U64(0xA3853B80F17F58EE);
     key = key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     key = key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//

p_bitboard->hash_key = key;

//cout << "p_bitboard->hash_key = " << p_bitboard->hash_key << "\n";
//cout << "   "<< "\n";

}//void HASHM::Start_position_random_key
//	*******************************************************************

//	===================================================================
// ������ ���� ��� ������ ����
void HASHM::Modification_random_key(bool colour_figure,// ���� ������� �������
								    unsigned __int8 i,// ����� ���� �� ������
								    struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
								    struct T_Bitboard * p_bitboard,// ������������� �����
                                    struct T_undo undo[]//���������� ��� ������ ����
								       ){

unsigned __int64 key; // ����������������� ���-����
unsigned __int8 name_figure;// ��� ������ ������� �����
unsigned __int8 name_figure_capture;// ��� ������ ������� ������
unsigned __int8  initial_position;// ��������� ���������� ����
unsigned __int8  final_position;//   �������� ���������� ����
unsigned __int8  type_move;//   ��� ����

key = p_bitboard->hash_key;
name_figure = ((p_list_surplus_moves->move[i]>>24)& 7);
initial_position = ((p_list_surplus_moves->move[i]>>12)& 63);
final_position = ((p_list_surplus_moves->move[i]>>18)& 63);
type_move = ((p_list_surplus_moves->move[i]>>6)& 63);
name_figure_capture = (p_list_surplus_moves->move[i] & 63);

// ��� ���� ������ ��� ����
switch(type_move){

// ��� �� ������ ����------------------
case SIMPLE_MOVE :

    //�� ������ ���-����� ������� �������� ���� ������ ��������� ����������
    //� ���������� ���� ������ �������� ����������
    key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
    key = key ^ random_piese_key[colour_figure][name_figure][final_position];

break;

// ������------------------------------
case CAPTURE_MOVE :

 key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
 key = key ^ random_piese_key[colour_figure][name_figure][final_position];
 key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

break;

//��������� 0-0-0-----------------------
case LONG_CASTLING :
  key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
  key = key ^ random_piese_key[colour_figure][name_figure][final_position];

  if (colour_figure){
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][0];
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][3];
  }else{
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][56];
     key = key ^ random_piese_key[(colour_figure)][N_ROOK][59];
  }//if (colour_figure)

break;

//��������� 0-0-------------------------
case SHORT_CASTLING :
 key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
 key = key ^ random_piese_key[colour_figure][name_figure][final_position];

 if (colour_figure){
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][7];
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][5];
 }else{
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][63];
    key = key ^ random_piese_key[(colour_figure)][N_ROOK][61];
 }//if (colour_figure)

break;

// ������ �� �������------------------
case TAKING_PASSAGE :

  key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
  key = key ^ random_piese_key[colour_figure][name_figure][final_position];

  if (colour_figure){
    key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][(final_position - 8)];
  }else{
    key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][(final_position + 8)];
  }//if (colour_figure)

break;

// ���� ����� � ������������------------------------------
case TRANSFORMATION_IN_KNIGHT : //����������� � ���� 
case TRANSFORMATION_IN_BISHOP : //����������� � ����
case TRANSFORMATION_IN_ROOK   : //����������� � ����� 
case TRANSFORMATION_IN_QUEEN  : //����������� � ����� 

    //
    //
    key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
    key = key ^ random_piese_key[colour_figure][(type_move - 10)][final_position];


break;

// ������ ������ � ������������---------------------------------------------------------------------
case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT : //������ � ������������ � ���� 
case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP : //������ � ������������ � ����
case CAPTURE_WITH_TRANSFORMATION_IN_ROOK   : //������ � ������������ � ����� 
case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN  : //������ � ������������ � ����� 

    //
    //
    key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
    key = key ^ random_piese_key[colour_figure][(type_move - 20)][final_position];
    key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

break;


default:;//��� �� ���������
   
}// switch(name){

// ��������� ���� ���� (������ �� ������ ����)
// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
// ��������� ������� ���������� ������ ������ ����
key = key ^ U64(0x7C0228DD624EC390);

// ������������ ����� ���������
// ���� ����� ��� ���� ���� �������� �� ����� ���������� ���-����
if( p_bitboard->castling_K != undo->return_castling_K) key = key ^ U64(0xB3598055CE64A656);//?
if( p_bitboard->castling_Q != undo->return_castling_Q) key = key ^ U64(0x12A8F886AF9418C2);
if( p_bitboard->castling_k != undo->return_castling_k) key = key ^ U64(0x1877B67E57A764D5);
if( p_bitboard->castling_q != undo->return_castling_q) key = key ^ U64(0xD4490AD346F14431);

//if( p_bitboard->castling_K != undo->return_castling_K) cout << "p_bitboard->castling_K = " << p_bitboard->castling_K 
//<< "undo->return_castling_K = " << undo->return_castling_K<< "\n";
//if( p_bitboard->castling_Q != undo->return_castling_Q) cout << "p_bitboard->castling_Q = " << p_bitboard->castling_Q 
//<< "undo->return_castling_Q = " << undo->return_castling_Q<< "\n";
//if( p_bitboard->castling_k != undo->return_castling_k)  cout << "p_bitboard->castling_k = " << p_bitboard->castling_k 
//<< "undo->return_castling_k = " << undo->return_castling_k<< "\n";
//if( p_bitboard->castling_q != undo->return_castling_q)  cout << "p_bitboard->castling_q = " << p_bitboard->castling_q 
//<< "undo->return_castling_q = " << undo->return_castling_q<< "\n";

// ��������� ������ �� �������
// ���� ��� ���� �� ���� ������ ��� ����� ������
if( undo->return_taking_passage){
	key = key ^ U64(0xA3853B80F17F58EE);
     key = key ^ (undo->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     key = key ^ (undo->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//


// ��������� ������ �� �������
// ���� ��� ��������� ����� ���� ������ ��� ����� ��������
if( p_bitboard->taking_passage_yes){
	key = key ^ U64(0xA3853B80F17F58EE);
     key = key ^ (p_bitboard->taking_passage_x ^ U64(0x5B23E522E4B1B4EF));
     key = key ^ (p_bitboard->taking_passage_y ^ U64(0x934E1DE72D36D310));
}//

p_bitboard->hash_key = key;

}//void HASHM::Modification_up_random_key
//	*******************************************************************




// ������� ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// �������������� ���-����� �������� ������������� ������� ��� �� ��������� ������ ������������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� � ������� ��������
void HASHM::Ini_hash_three(){
  for(unsigned __int8 i = 0; i < 120 ; i++){
     hash_three_moves[i] = i;
  }//
}//void HASHM::Ini_hash_moves
//	*******************************************************************

//	===================================================================
// �������������� ���-����� �������� ������������� ������� ��� �� ��������� ������ ������������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� ���������� �� ��������
void HASHM::Ini_hash_string_moves(){
  for(unsigned __int16 i = 0; i < 1000 ; i++){
     hash_string_moves[i] = i;
  }//
}//void HASHM::Ini_hash_moves
//	*******************************************************************

//	===================================================================
// ���������� ���-���� �� ������� �������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� � ������� ��������
void HASHM::Save_hash_three(unsigned __int64 key,// ���-����(hash-key) 
					        unsigned __int8 depth // �������
					        ){
  hash_three_moves[depth] = key;

}//void HASHM::Insert_hash_moves
//	*******************************************************************

//	===================================================================
// ���������� ���-���� �� ������� �������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� ���������� �� ��������
void HASHM::Save_hash_string_moves(unsigned __int64 key,// ���-����
					               unsigned __int16 cnt_move // ������� ����� � ������ ������
					               ){
  hash_string_moves[cnt_move] = key;
  save_cnt_move = cnt_move;

}//void HASHM::Insert_hash_moves
//	*******************************************************************

//	===================================================================
// ����������� ���������� ����� ������. ���� ����� ���������� �� ���������� 1 ����� 0
// (���������� ��� ������ �������� ������� ��� ����������� ������)
bool HASHM::Analysis_hash_string_moves(unsigned __int8 depth // �������
								       ){

// TEST--------------------------------------------------------
//if (depth == 2){
//
//// TEST-------------
//cout << "depth= "
//     << depth
//     <<"\n";
//for (int i = 0; i <= depth; i++ ){
//cout << "i= "
//     << i
//     << " hash_three_moves[i]= "
//     << hash_three_moves[i]
//     <<"\n";
//}//for (int i = 0; i <= save_cnt_move; i++ )
//// TEST-------------
//
//
//// TEST-------------
//for (int i = 0; i <= save_cnt_move; i++ ){
//cout << "i= "
//     << i
//     << " hash_string_moves[i]= "
//     << hash_string_moves[i]
//     <<"\n";
//}//for (int i = 0; i <= save_cnt_move; i++ )
//// TEST-------------
//
//}//if (depth == 4){
// TEST--------------------------------------------------------

/*
�������� � �������� ������ ��� ����������� ������� �������

���� ������� ���������� 4 � ���� �� ���� ������ � ����� ��������
���� ������ �� ���� ������ � ������ ���� ���������� ���������.

���������� ������ ���������� ������� ��� �� ������ ����� ��������
����� ������� ����������� ���� ��� ��� ����������� �������
���� ������ ������� ���������� ��� ��� � ������ ������� :)))

*/

if (depth > 3){

    for (unsigned __int8 j = 4; (depth - j) >= 0; j = j + 2 ){

	    if (hash_three_moves[depth] == hash_three_moves[depth - j]){


       // // TEST--------------------------------------------------------
       //      cout << " DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD "
       //           << "\n";
       //      cout << "depth= "
       //           << depth
       //           <<"\n";
       //      
       //      cout << "j= "
       //           << j
       //           <<"\n";
       //      // TEST-------------
       //      for (int i = 0; i <= depth; i++ ){
       //           cout << "i= "
       //                << i
       //                << " hash_three_moves[i]= "
       //                << hash_three_moves[i]
       //                <<"\n";
       //      }//for (int i = 0; i <= depth; i++ )
       //      // TEST-------------
       //// TEST--------------------------------------------------------

		     return 1;

		}//if (hash_three_moves[depth] == hash_three_moves[depth - j])

    }// for (int j = 4; (depth - j) >= 0; j = j + 2 )


}else{// if (depth > 3)

   for (__int32 k = 4; (depth - k + save_cnt_move) >= 0; k = k + 2 ){

     if (hash_three_moves[depth] == hash_string_moves[ depth - k + save_cnt_move ] ){

     //// TEST--------------------------------------------------------
     //    cout << " CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC "
     //    << "\n";
     //    cout << "depth= "
     //    << depth
     //    <<"\n";
     //    // TEST-------------
     //    for (int i = 0; i <= depth; i++ ){
     //         cout << "i= "
     //              << i
     //              << " hash_three_moves[i]= "
     //              << hash_three_moves[i]
     //              <<"\n";
     //    }//for (int i = 0; i <= depth; i++ )
     //    cout << "  "
     //    << "\n";
     //    for (int i = 0; i <= save_cnt_move; i++ ){
     //        cout << "i= "
     //             << i
     //             << " hash_string_moves[i]= "
     //             << hash_string_moves[i]
     //             <<"\n";
     //    }//for (int i = 0; i <= save_cnt_move; i++ )
     //    // TEST-------------
     //  // TEST--------------------------------------------------------

		return 1;

	 }//if (hash_three_moves[depth] == hash_string_moves[ depth - k + save_cnt_move ] )

	}//for (int j = 4; (depth - j + save_cnt_move) >= 0; j = j + 2 )
		

}//if (depth > 4){

return 0;

}//HASHM::Analysis_hash_string_moves

//	*******************************************************************

//	===================================================================
// // ���������� ���������� ����� � ������ ���� ���������� �� ��������
__int32 HASHM::get_save_cnt_move(){
	return save_cnt_move;
}//int FULLSEARCH::get_from
//	*******************************************************************







//	===================================================================
// ��������
void HASHM::Print_hash_moves(){
//ofstream Test_Loop;
//int key;
//  Test_Loop.open ("_TEST_LOOP.txt",ios::app);
//
////for(int hm=1 ; hm<1000 ; hm++){
////   Test_Loop << " p_hash_moves[ " << hm << "].key= " << p_hash_moves[hm].key  << "\n";
//////  Test_Loop << p_hash_moves[hm] << "\n";
////}
//Test_Loop << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< "\n";
//key=574582;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=574484;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=563472;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=585502;
//Test_Loop << " p_hash_moves[ " << key  << "].key= " << p_hash_moves[key].key  << "\n";
//Test_Loop << "p_hash_moves[key].initial_position= "<< p_hash_moves[key].initial_position << "\n";
//Test_Loop << "p_hash_moves[key].final_position= "<< p_hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(p_hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-p_hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//Test_Loop.close();

}//void HASHM::Print_hash_moves
//	*******************************************************************