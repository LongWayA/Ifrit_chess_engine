#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * перебор позиций
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 17.05.2007
*/


//---------------------------------------------------------------------------
#define TEST_F 0; // включаем-выключаем(1-0) режим вывода в файл тестовой информации

//---------------------------------------------------------------------------
//
int Forced_variant_white(struct T_Bitboard * bitboard,int alpha,int beta,int depth,struct T_PV * pv_current,struct T_PV * pv_best);

//---------------------------------------------------------------------------
//
int Forced_variant_black(struct T_Bitboard * bitboard,int alpha,int beta,int depth,struct T_PV * pv_current,struct T_PV * pv_best);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// глубина перебора,список
#if TEST_F
void List_816_Print_F(int Depth,const struct T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// сортируем взятия возвращаем количество взятий
inline int Sorting_captures_moves(struct T_list_surplus_moves * list_surplus_moves);
