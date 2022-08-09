
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.11.10

#include <iostream>

#include "a_List_const.h"
#include "n_Delta_pruning.h"
#include "n_Delta_pruning_const.h"


//	===================================================================
// фигуры на доску
void n_Delta_pruning::delta_pruning_remember_piese_white
(
	struct Position & position// представление доски
)
{
	// запоминаем фигуры до резки
	all_black_or_white_piece = position.all_black_piece;

}

//	===================================================================
// ? функция изменяет внешние классу данные!
void n_Delta_pruning::delta_pruning_white
(
	struct Position & position,// представление доски
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
	position.all_black_piece = position.all_black_piece 
		^ position.black_piece[N_PAWN];

	if (value_stat < alpha - BISHOP_KNIGHT_MARGIN )
	{
		// взятие слонов и коней исключаем
		position.all_black_piece = position.all_black_piece 
			^ position.black_piece[N_KNIGHT];

		position.all_black_piece = position.all_black_piece 
			^ position.black_piece[N_BISHOP];

		if (value_stat < alpha - ROOK_MARGIN )// взятие ладьи исключаем
		{
			position.all_black_piece = position.all_black_piece 
				^ position.black_piece[N_ROOK];
		}
	}

	value_stat = value_stat + PAWN_MARGIN;
}


//	===================================================================
// возвращаем фигуры на доску
void n_Delta_pruning::delta_pruning_return_piese_white
(
	struct Position & position// представление доски
)
{
	// возвращаем на доску фигуры
	position.all_black_piece = all_black_or_white_piece;

}

//	===================================================================
//  фигуры на доску
void n_Delta_pruning::delta_pruning_remember_piese_black
(
	struct Position & position// представление доски
)
{
	// запоминаем фигуры до резки
	all_black_or_white_piece = position.all_white_piece;
}

//	===================================================================
// ? функция изменяет внешние классу данные!
void n_Delta_pruning::delta_pruning_black
(
	struct Position & position,// представление доски
	const __int32 beta,//
	__int32 & value_stat // статическая оценка
)
{

	// взятие пешек исключаем
	position.all_white_piece = position.all_white_piece 
		^ position.white_piece[N_PAWN];

	if (value_stat > beta + BISHOP_KNIGHT_MARGIN)
	{
		// взятие слонов и коней исключаем
		position.all_white_piece = position.all_white_piece 
			^ position.white_piece[N_KNIGHT];

		position.all_white_piece = position.all_white_piece 
			^ position.white_piece[N_BISHOP];      

		if (value_stat > beta + ROOK_MARGIN)
		{
			// взятие ладьи исключаем
			position.all_white_piece = position.all_white_piece 
				^ position.white_piece[N_ROOK];   
		}

	}
	value_stat = value_stat - PAWN_MARGIN;

}

//	===================================================================
// возвращаем фигуры на доску
void n_Delta_pruning::delta_pruning_return_piese_black
(
	struct Position & position// представление доски
)
{
	// возвращаем на доску фигуры
	position.all_white_piece = all_black_or_white_piece;
}