#include "k_structure.h"

//-----------------------------------------------------------------------------------
/*
 * ��������� ��� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 15.12.2007
 *
*/
//---------------------------------------------------------------------------


#define TEST_H 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������

//---------------------------------------------------------------------------
void Ini_random_piese_key();

//---------------------------------------------------------------------------
void Print_hash_moves();

//---------------------------------------------------------------------------
void Ini_hash_moves();

//---------------------------------------------------------------------------
void Look_hash(T_list_surplus_moves * list_surplus_moves,int key,short int depth,short int & insert_hash);

//---------------------------------------------------------------------------
void Save_best_hash_key(short int i,T_list_surplus_moves * list_surplus_moves,int key,short int depth);

//---------------------------------------------------------------------------
void Save_hash_key(short int i,T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,short int depth);

//---------------------------------------------------------------------------
void Insert_hash_moves(T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,short int depth,short int & insert_hash);

//---------------------------------------------------------------------------
void Start_position_random_key(T_Bitboard * bitboard);

//---------------------------------------------------------------------------
void Modification_up_random_key(short int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard);

//---------------------------------------------------------------------------
void Modification_down_random_key(short int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard);

