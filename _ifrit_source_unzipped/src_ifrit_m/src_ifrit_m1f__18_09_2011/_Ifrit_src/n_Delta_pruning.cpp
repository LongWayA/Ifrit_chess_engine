
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 22.11.10

#include <iostream>

#include "a_List_const.h"
#include "n_Delta_pruning.h"
#include "n_Delta_pruning_const.h"


//	===================================================================
// ������ �� �����
void n_Delta_pruning::delta_pruning_remember_piese_white
(
	struct Position & position// ������������� �����
)
{
	// ���������� ������ �� �����
	all_black_or_white_piece = position.all_black_piece;

}

//	===================================================================
// ? ������� �������� ������� ������ ������!
void n_Delta_pruning::delta_pruning_white
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	__int32 & value_stat // ����������� ������
)
{
	/*
	������� ��� ��� ������� ������ ����� ����� ��� �� ����
	captures_mask = (all_black_piece & knigt_moves_masks[from]);
	����� ������� ���� � all_black_piece ������ ��� 
	�� � ������ �� �����
	*/

	// ������ ����� ���������
	position.all_black_piece = position.all_black_piece 
		^ position.black_piece[N_PAWN];

	if (value_stat < alpha - BISHOP_KNIGHT_MARGIN )
	{
		// ������ ������ � ����� ���������
		position.all_black_piece = position.all_black_piece 
			^ position.black_piece[N_KNIGHT];

		position.all_black_piece = position.all_black_piece 
			^ position.black_piece[N_BISHOP];

		if (value_stat < alpha - ROOK_MARGIN )// ������ ����� ���������
		{
			position.all_black_piece = position.all_black_piece 
				^ position.black_piece[N_ROOK];
		}
	}

	value_stat = value_stat + PAWN_MARGIN;
}


//	===================================================================
// ���������� ������ �� �����
void n_Delta_pruning::delta_pruning_return_piese_white
(
	struct Position & position// ������������� �����
)
{
	// ���������� �� ����� ������
	position.all_black_piece = all_black_or_white_piece;

}

//	===================================================================
//  ������ �� �����
void n_Delta_pruning::delta_pruning_remember_piese_black
(
	struct Position & position// ������������� �����
)
{
	// ���������� ������ �� �����
	all_black_or_white_piece = position.all_white_piece;
}

//	===================================================================
// ? ������� �������� ������� ������ ������!
void n_Delta_pruning::delta_pruning_black
(
	struct Position & position,// ������������� �����
	const __int32 beta,//
	__int32 & value_stat // ����������� ������
)
{

	// ������ ����� ���������
	position.all_white_piece = position.all_white_piece 
		^ position.white_piece[N_PAWN];

	if (value_stat > beta + BISHOP_KNIGHT_MARGIN)
	{
		// ������ ������ � ����� ���������
		position.all_white_piece = position.all_white_piece 
			^ position.white_piece[N_KNIGHT];

		position.all_white_piece = position.all_white_piece 
			^ position.white_piece[N_BISHOP];      

		if (value_stat > beta + ROOK_MARGIN)
		{
			// ������ ����� ���������
			position.all_white_piece = position.all_white_piece 
				^ position.white_piece[N_ROOK];   
		}

	}
	value_stat = value_stat - PAWN_MARGIN;

}

//	===================================================================
// ���������� ������ �� �����
void n_Delta_pruning::delta_pruning_return_piese_black
(
	struct Position & position// ������������� �����
)
{
	// ���������� �� ����� ������
	position.all_white_piece = all_black_or_white_piece;
}