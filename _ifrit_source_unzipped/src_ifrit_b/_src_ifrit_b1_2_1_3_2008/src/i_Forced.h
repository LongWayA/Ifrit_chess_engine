#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * перебор позиций
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 6.11.2007
*/


//---------------------------------------------------------------------------
#define TEST_F 0; // включаем-выключаем(1-0) режим вывода в файл тестовой информации

//---------------------------------------------------------------------------
//
int Forced_variant_816(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth,T_PV * pv_current,T_PV * pv_best);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// глубина перебора,список
#if TEST_F
void List_816_Print_F(int Depth,const T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------

