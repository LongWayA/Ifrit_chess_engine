#include <iostream> // TEST
#include <fstream>  // TEST

#include "a_Chess_bitboard.h"

//---------------------------------------------------------------------------
/*+
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.11.10
*/
//---------------------------------------------------------------------------

/*
 ������� ����� �������

 �� ���� ��������� ��������� � ���������� delta pruning.
 ���� �����, ��� ���� ������ ������ ����� �� ������� �����, �� ������� ����� 
 �������� �� ��������� � ����� ������ ����� ���������.  �� �� � ��� ������ �����.

*/

namespace 
{
	// 0 - ������ 1200 - ����� 600 - ����� 400 - ����  400 - ���� 100 - �����
	const unsigned __int16 ROOK_MARGIN = 700; // ������ ������ ������
	const unsigned __int16 BISHOP_KNIGHT_MARGIN = 500; // ������ ������  ������ � �����
	const unsigned __int8 PAWN_MARGIN = 160; // ������ ������  �����
}


//	===================================================================
// ������ �� �����
void Chess_bitboard::delta_pruning_remember_piese_white()
{
	// ���������� ������ �� �����
	all_black_or_white_piece = all_black_piece;

}

//	===================================================================
//
void Chess_bitboard::delta_pruning_white
(
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
	all_black_piece = all_black_piece 
		^ black_piece[N_PAWN];

	if (value_stat < alpha - BISHOP_KNIGHT_MARGIN )
	{
		// ������ ������ � ����� ���������
		all_black_piece = all_black_piece 
			^ black_piece[N_KNIGHT];

		all_black_piece = all_black_piece 
			^ black_piece[N_BISHOP];

		if (value_stat < alpha - ROOK_MARGIN )// ������ ����� ���������
		{
			all_black_piece = all_black_piece 
				^ black_piece[N_ROOK];
		}
	}

	value_stat = value_stat + PAWN_MARGIN;
}


//	===================================================================
// ���������� ������ �� �����
void Chess_bitboard::delta_pruning_return_piese_white()
{
	// ���������� �� ����� ������
	all_black_piece = all_black_or_white_piece;

}

//	===================================================================
//  ������ �� �����
void Chess_bitboard::delta_pruning_remember_piese_black()
{
	// ���������� ������ �� �����
	all_black_or_white_piece = all_white_piece;
}

//	===================================================================
//
void Chess_bitboard::delta_pruning_black
(
	const __int32 beta,//
	__int32 & value_stat // ����������� ������
)
{

	// ������ ����� ���������
	all_white_piece = all_white_piece 
		^ white_piece[N_PAWN];

	if (value_stat > beta + BISHOP_KNIGHT_MARGIN)
	{
		// ������ ������ � ����� ���������
		all_white_piece = all_white_piece 
			^ white_piece[N_KNIGHT];

		all_white_piece = all_white_piece 
			^ white_piece[N_BISHOP];      

		if (value_stat > beta + ROOK_MARGIN)
		{
			// ������ ����� ���������
			all_white_piece = all_white_piece 
				^ white_piece[N_ROOK];   
		}

	}
	value_stat = value_stat - PAWN_MARGIN;

}

//	===================================================================
// ���������� ������ �� �����
void Chess_bitboard::delta_pruning_return_piese_black()
{
	// ���������� �� ����� ������
	all_white_piece = all_black_or_white_piece;
}