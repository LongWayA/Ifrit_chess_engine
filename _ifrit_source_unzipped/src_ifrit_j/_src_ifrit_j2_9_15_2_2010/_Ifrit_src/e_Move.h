#include "a_structure.h"
//-----------------------------------------------------------------------------------
/*
 * задаем функции по реализации ходов
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.02.2009
 * 
*/
//---------------------------------------------------------------------------

#define MATERIAL 1 // включаем-выключаем(1-0) режим расчета материала

/*******************************************
класс MOVES

 в классе собраны методы по реализации ходов из списка T_list_surplus_moves
 который раньше был сгенерирован объектом класса BITBOARDGENERATOR
 в итоге работы класса обновляется доска T_Bitboard

********************************************/
class MOVES{

// с помощью этой маски пишем и стираем фигуры из битового массива
static const unsigned __int64 move_masks_m[];

//////////////////////////////////////////////////////////////////////////////////////////
public:

//---------------------------------------------------------------------------
// реализуем ходы вперед для белых
void Onward_moves_white(const unsigned __int8 & i,//номер хода по списку
                        struct T_undo undo[],//информация для отката хода
					    const struct T_list_surplus_moves * const p_list_surplus_moves,//список возможных ходов
						struct T_Bitboard * const p_bitboard// представление позиции
						);

//---------------------------------------------------------------------------
// реализуем ходы вперед для черных
void Onward_moves_black(const unsigned __int8 & i,//номер хода по списку
                        struct T_undo undo[],//информация для отката хода
					    const struct T_list_surplus_moves * const p_list_surplus_moves,//список возможных ходов
						struct T_Bitboard * const p_bitboard// представление позиции
						);

//---------------------------------------------------------------------------
// реализуем ходы назад для белых
void Backs_moves_white(const unsigned __int8 & i,//номер хода по списку
					   const struct T_undo undo[],//информация для отката хода
					   const struct T_list_surplus_moves * const p_list_surplus_moves,//список возможных ходов
                       struct T_Bitboard * const p_bitboard// представление позиции
					   );

//---------------------------------------------------------------------------
// реализуем ходы назад для черных
void Backs_moves_black(const unsigned __int8 & i,//номер хода по списку
					   const struct T_undo undo[],//информация для отката хода
					   const struct T_list_surplus_moves * const p_list_surplus_moves,//список возможных ходов
                       struct T_Bitboard * const p_bitboard// представление позиции
					   );


};//class MOVES
