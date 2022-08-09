#include <iostream> // TEST
#include <fstream>  // TEST

#include "a_Chess_bitboard.h"

//---------------------------------------------------------------------------
/*+
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.11.10
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ

 Не знаю насколько правильно я реализовал delta pruning.
 Идея такая, что если оценка меньше альфы на полторы пешки, то взятием пешки 
 ситуацию не исправить и такие взятия можно исключить.  То же и для других фигур.

*/

namespace 
{
	// 0 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка
	const unsigned __int16 ROOK_MARGIN = 700; // отсчка взятий ладьей
	const unsigned __int16 BISHOP_KNIGHT_MARGIN = 500; // отсчка взятий  слонов и коней
	const unsigned __int8 PAWN_MARGIN = 160; // отсчка взятий  пешек
}


//	===================================================================
// фигуры на доску
void Chess_bitboard::delta_pruning_remember_piese_white()
{
	// запоминаем фигуры до резки
	all_black_or_white_piece = all_black_piece;

}

//	===================================================================
//
void Chess_bitboard::delta_pruning_white
(
	const __int32 alpha,//
	__int32 & value_stat // статическая оценка
)
{
	/*
	напомню что при взятиях черных фигур пишем что то типа
	captures_mask = (all_black_piece & knigt_moves_masks[from]);
	таким образом если в all_black_piece фигуры нет 
	то и взятия не будет
	*/

	// взятие пешек исключаем
	all_black_piece = all_black_piece 
		^ black_piece[N_PAWN];

	if (value_stat < alpha - BISHOP_KNIGHT_MARGIN )
	{
		// взятие слонов и коней исключаем
		all_black_piece = all_black_piece 
			^ black_piece[N_KNIGHT];

		all_black_piece = all_black_piece 
			^ black_piece[N_BISHOP];

		if (value_stat < alpha - ROOK_MARGIN )// взятие ладьи исключаем
		{
			all_black_piece = all_black_piece 
				^ black_piece[N_ROOK];
		}
	}

	value_stat = value_stat + PAWN_MARGIN;
}


//	===================================================================
// возвращаем фигуры на доску
void Chess_bitboard::delta_pruning_return_piese_white()
{
	// возвращаем на доску фигуры
	all_black_piece = all_black_or_white_piece;

}

//	===================================================================
//  фигуры на доску
void Chess_bitboard::delta_pruning_remember_piese_black()
{
	// запоминаем фигуры до резки
	all_black_or_white_piece = all_white_piece;
}

//	===================================================================
//
void Chess_bitboard::delta_pruning_black
(
	const __int32 beta,//
	__int32 & value_stat // статическая оценка
)
{

	// взятие пешек исключаем
	all_white_piece = all_white_piece 
		^ white_piece[N_PAWN];

	if (value_stat > beta + BISHOP_KNIGHT_MARGIN)
	{
		// взятие слонов и коней исключаем
		all_white_piece = all_white_piece 
			^ white_piece[N_KNIGHT];

		all_white_piece = all_white_piece 
			^ white_piece[N_BISHOP];      

		if (value_stat > beta + ROOK_MARGIN)
		{
			// взятие ладьи исключаем
			all_white_piece = all_white_piece 
				^ white_piece[N_ROOK];   
		}

	}
	value_stat = value_stat - PAWN_MARGIN;

}

//	===================================================================
// возвращаем фигуры на доску
void Chess_bitboard::delta_pruning_return_piese_black()
{
	// возвращаем на доску фигуры
	all_white_piece = all_black_or_white_piece;
}