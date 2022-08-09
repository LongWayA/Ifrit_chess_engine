#include "k_structure.h"

//-----------------------------------------------------------------------------------
/*
 * реализуем хеш таблицу
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 15.12.2007
 *
*/
//---------------------------------------------------------------------------


#define TEST_H 0; // включаем-выключаем(1-0) режим вывода в файл тестовой информации

//---------------------------------------------------------------------------
void Ini_random_piese_key();

//---------------------------------------------------------------------------
void Print_hash_moves();

//---------------------------------------------------------------------------
void Ini_hash_moves();

//---------------------------------------------------------------------------
void Look_hash(T_list_surplus_moves * list_surplus_moves,int key,int depth,int & insert_hash);

//---------------------------------------------------------------------------
void Save_best_hash_key(int i,T_list_surplus_moves * list_surplus_moves,int key,int depth);

//---------------------------------------------------------------------------
void Save_hash_key(int i,T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,int depth);

//---------------------------------------------------------------------------
void Insert_hash_moves(T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,int depth,int & insert_hash);

//---------------------------------------------------------------------------
void Start_position_random_key(T_Bitboard * bitboard);

//---------------------------------------------------------------------------
void Modification_up_random_key(int colour_figure,int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard);

//---------------------------------------------------------------------------
void Modification_down_random_key(int colour_figure,int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard);

