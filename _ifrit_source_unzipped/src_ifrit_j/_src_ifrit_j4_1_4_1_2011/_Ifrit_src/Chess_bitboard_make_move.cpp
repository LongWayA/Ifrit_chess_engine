#include <iostream> // TEST

#include "Chess_bitboard.h"
#include "Evaluate.h"
#include "Evaluate_const.h"

//---------------------------------------------------------------------------
/*++
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 23.02.2009 26.06.2010  21.11.2010
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 99%

 ������ ������� �� ���������� �����

 ������� ������ �� ���������� ����� �� ������ List_surplus_moves
 � ����� ������ ����������� ����� Chess_bitboard
 
*/

class Evaluate evaluate;

//	===================================================================
// ��������� ���� ������ ��� �����
void Chess_bitboard::do_moves_white
(
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const class List_surplus_moves & list_surplus_moves//������ ��������� �����
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);// �������� ������� ������
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);// ��������� ������� ������
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);// ������ ������

	// ���������� ���� ������ �� �������
	undo.return_taking_passage = taking_passage_yes;

	// ������ ����������(����� ��� ���-�������)
	undo.taking_passage_x = taking_passage_x;
	undo.taking_passage_y = taking_passage_y;

	// ��������� ������ �� ������� 
	taking_passage_yes = 0;

	// ����� ������ ������ ������ ��� ������
	colour_move = 0;

	// ������ ���������� ����� ��������� (����� ��� ���-�������)
	undo.return_castling_K = castling_K;
	undo.return_castling_Q = castling_Q;
	undo.return_castling_k = castling_k;
	undo.return_castling_q = castling_q;

	/*
	��� ����
	0 - ��� ����
	1 � ������� ���
	2 � ������
	3 � ������� ���������
	4 � �������� ���������
	5 � ������ �� �������

	12 - ����������� � ���� 
	13 - ����������� � ����
	14 - ����������� � ����� 
	15 - ����������� � ����� 

	22 - ������ � ������������ � ���� 
	23 - ������ � ������������ � ����
	24 - ������ � ������������ � ����� 
	25 - ������ � ������������ � ����� 
	*/

	switch (((list_surplus_moves.move[i]>>6)& 63))
	{
		//1 ��� �� ������ ����
		case SIMPLE_MOVE:

			//������� ������ �� ������ �����---------------------------------------------- 
			//������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			//������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			//��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = white_name_from_coord[i_p];
			white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����------------------------------------------------
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];
			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			//---------------------------------------------------------------------------------
			// ����� �������� ������ �������� �� ��������� � ������ �� �������
			// �� ��� ������� ����� ������� ������ � ���� ��� ����� �� ��������� �� �����
			// �� �������� ������ �� ������� ���� ��� ������ �� �������� ��� ���������
			// � ���� ����� �� �������� ��������� � ��� 
			//---------------------------------------------------------------------------------

			// ����� - ���� ������ �� �������
			if (white_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					undo.taking_passage_x = taking_passage_x;
					undo.taking_passage_y = taking_passage_y;
					taking_passage_yes = 1;
					taking_passage_x =(i_p + 8) % 8;
					taking_passage_y =(i_p + 8) / 8;
				}
			}// ������ - ���� ���������
			else if (white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				undo.return_castling_K = castling_K;
				undo.return_castling_Q = castling_Q;
				castling_K = 0;
				castling_Q = 0;	   	
			}// ����� - ���� ���������  
			else if (white_name_from_coord[f_p] == N_ROOK)
			{
				if (i_p == A1)//0
				{
					undo.return_castling_Q = castling_Q;
					castling_Q = 0;
				}

				if (i_p == H1)//7
				{
					undo.return_castling_K = castling_K;
					castling_K = 0;
				}
			}
			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			all_black_piece = all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ ������
			// � ������ �������� ������
#if MATERIAL
			value_black_material = value_black_material 
				- KOD[black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// �������� ������ ������
			black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[f_p] = white_name_from_coord[i_p];
			white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];
			// ������ ������� ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ - ���� ���������
			if (white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				undo.return_castling_K = castling_K;
				undo.return_castling_Q = castling_Q;
				castling_K = 0;
				castling_Q = 0;		
  
			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (white_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A1)//0
				{
					undo.return_castling_Q = castling_Q;
					castling_Q =0;
				}

				if (i_p == H1)//7
				{
					undo.return_castling_K = castling_K;
					castling_K =0;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					undo.return_castling_q = castling_q;
					castling_q = 0;
				}

				if (f_p == H8)//63
				{
					undo.return_castling_k = castling_k;
					castling_k = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;
		break;

		//5 ������ �� �������
		case TAKING_PASSAGE:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = white_name_from_coord[i_p];
			white_name_from_coord[i_p] = 0;

			// ������� ������ ������ �� ����� ���� ������ �����
			all_black_piece = all_black_piece ^ move_masks[(f_p - 8)];
			// ����� ���� ������� ������ ������ �� �������� ������ �����
			black_piece[black_name_from_coord[(f_p - 8)]] = black_piece[black_name_from_coord[(f_p - 8)]] ^ move_masks[(f_p - 8)];

			// ������ ������
			// � ������ �������� �����
#if MATERIAL
			value_black_material = value_black_material 
				- PAWN;	
#endif//#if MATERIAL


			// �������� ������ ������
			black_name_from_coord[(f_p - 8)] = 0;

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//3 ��������� 0-0-0
		case LONG_CASTLING:

			//������� ������ �� ������ �����
			// ��� �������
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = white_name_from_coord[i_p];
			white_name_from_coord[i_p] = 0;

			// ��� ������
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[A1];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[A1]] 
			= white_piece[white_name_from_coord[A1]] ^ move_masks[A1];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[D1] = white_name_from_coord[A1];
			white_name_from_coord[A1] = 0;

			//������ ������ �� ����� �����------------------
			// ��� �������
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��� ������
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[D1];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[D1]] 
			= white_piece[white_name_from_coord[D1]] ^ move_masks[D1];

			// ������ - ���� ���������
			// ���������� ����� ���������
			undo.return_castling_Q = castling_Q;
			undo.return_castling_K = castling_K;
			castling_Q = 0;
			castling_K = 0;

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//4 ��������� 0-0
		case SHORT_CASTLING:

			//������� ������ �� ������ �����
			// ��� �������
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = white_name_from_coord[i_p];
			white_name_from_coord[i_p] = 0;

			// ��� ������
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[H1];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[H1]] 
			= white_piece[white_name_from_coord[H1]] ^ move_masks[H1];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[F1] = white_name_from_coord[H1];
			white_name_from_coord[H1] = 0;

			//������ ������ �� ����� �����
			// ��� �������
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��� ������
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[F1];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[F1]] 
			= white_piece[white_name_from_coord[F1]] ^ move_masks[F1];

			// ������ - ���� ���������
			// ���������� ����� ���������
			undo.return_castling_Q = castling_Q;
			undo.return_castling_K = castling_K;
			castling_Q = 0;
			castling_K = 0;

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//15 ����������� � �����
		case TRANSFORMATION_IN_QUEEN:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = N_QUEEN;
			white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
			value_white_material = value_white_material 
				+ QUEEN - PAWN;
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//14 ����������� � �����
		case TRANSFORMATION_IN_ROOK:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = N_ROOK;
			white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
			value_white_material = value_white_material 
				+ ROOK - PAWN;	
#endif//#if MATERIAL
			

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//13 ����������� � �����
		case TRANSFORMATION_IN_BISHOP:

			//������� ������ �� ������ ����� 
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = N_BISHOP;
			white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
				value_white_material = value_white_material 
					+ BISHOP - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//12 ����������� � ����
		case TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			white_name_from_coord[f_p] = N_KNIGHT;
			white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
			value_white_material = value_white_material 
				+ KNIGHT - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;
		break;

		//25 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			all_black_piece = all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			value_white_material = 
				value_white_material + QUEEN - PAWN;

			value_black_material = value_black_material	
				- KOD[black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[f_p] = N_QUEEN;
			white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������� ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = castling_q;
					castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = castling_k;
					castling_k = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		//24 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			all_black_piece = all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			value_white_material = 
				value_white_material + ROOK - PAWN;

			value_black_material = value_black_material 
				- KOD[black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			black_name_from_coord[f_p] = 0;
			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[f_p] = N_ROOK;
			white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];
			// ������ ������� ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = castling_q;
					castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = castling_k;
					castling_k = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		//23 ������ � ������������ �����
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			all_black_piece = all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];


			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			value_white_material = 
				value_white_material + BISHOP - PAWN;

			value_black_material = value_black_material 
				- KOD[black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// �������� ������ ������
			black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[f_p] = N_BISHOP;
			white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������� ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = castling_q;
					castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = castling_k;
					castling_k = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		//22 ������ � ������������  � ����
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// ������� ������ �� ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			all_black_piece = all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];


			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			value_white_material = 
				value_white_material + KNIGHT - PAWN;

			value_black_material = value_black_material 
				- KOD[black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[f_p] = N_KNIGHT;
			white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������ ������� ������ � ������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];  

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = castling_q;
					castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = castling_k;
					castling_k = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		default:;//��� �� ���������
   
	}

}

//	===================================================================
// ��������� ���� ������ ��� ������
void Chess_bitboard::do_moves_black
(
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const class List_surplus_moves & list_surplus_moves//������ ��������� �����
)
{
	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// ���������� ���� ������ �� �������
	undo.return_taking_passage= taking_passage_yes;

	// ������ ����������(����� ��� ���-�������)
	undo.taking_passage_x = taking_passage_x;
	undo.taking_passage_y = taking_passage_y;


	// ��������� ������ �� ������� 
	taking_passage_yes = 0;

	// ������ ������ ������ ������ ��� �����
	colour_move = 1;

	// ������ ���������� ����� ��������� (����� ��� ���-�������)
	undo.return_castling_K = castling_K;
	undo.return_castling_Q = castling_Q;
	undo.return_castling_k = castling_k;
	undo.return_castling_q = castling_q;


	/*
	��� ����
	0 - ��� ����
	1 � ������� ���
	2 � ������
	3 � ������� ���������
	4 � �������� ���������
	5 � ������ �� �������

	12 - ����������� � ���� 
	13 - ����������� � ����
	14 - ����������� � ����� 
	15 - ����������� � ����� 

	22 - ������ � ������������ � ���� 
	23 - ������ � ������������ � ����
	24 - ������ � ������������ � ����� 
	25 - ������ � ������������ � ����� 
	*/

	switch (((list_surplus_moves.move[i]>>6)& 63))
	{

		//1 ��� �� ������ ����
		case SIMPLE_MOVE:

			//������� ������ �� ������ ����� 
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = black_name_from_coord[i_p];
			black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����� - ���� ������ �� ������� 
			if (black_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					//cout << "i_p= "<< i_p<<"\n";
					//cout << "f_p= "<< f_p<<"\n";
					//cout << "black_name_from_coord[f_p]= "<< black_name_from_coord[f_p]<<"\n";
					undo.taking_passage_x = taking_passage_x;
					undo.taking_passage_y = taking_passage_y;
					taking_passage_yes =1;
					taking_passage_x =(i_p - 8) % 8;
					taking_passage_y =(i_p - 8) / 8;
				}
			
			}// ������ - ���� ���������
			else if (black_name_from_coord[f_p] == N_KING)
			{
				undo.return_castling_k = castling_k;
				undo.return_castling_q = castling_q;
				castling_k = 0;
				castling_q = 0;

    
			}// ����� - ���� ���������   
			else if (black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo.return_castling_q = castling_q;
					castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo.return_castling_k = castling_k;
					castling_k = 0;
				}
			}

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			//������� ������ �� ������ �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[f_p];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ �������
#if MATERIAL
			value_white_material = value_white_material 
				- KOD[white_name_from_coord[f_p]];
#endif//#if MATERIAL


			white_name_from_coord[f_p] = 0;

			black_name_from_coord[f_p] = black_name_from_coord[i_p];
			black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ - ���� ���������
			if (black_name_from_coord[f_p] == N_KING)
			{
				undo.return_castling_k = castling_k;
				undo.return_castling_q = castling_q;
				castling_k = 0;
				castling_q = 0;

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo.return_castling_q = castling_q;
					castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo.return_castling_k = castling_k;
					castling_k = 0;
				}
     
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = castling_Q;
					castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = castling_K;
					castling_K = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		// 5 ������ �� �������
		case TAKING_PASSAGE:

			//������� ������ �� ������ �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = black_name_from_coord[i_p];
			black_name_from_coord[i_p] = 0;

			all_white_piece = all_white_piece ^ move_masks[(f_p + 8)];
			white_piece[white_name_from_coord[(f_p + 8)]] = white_piece[white_name_from_coord[(f_p + 8)]] ^ move_masks[(f_p + 8)];
  
			// ������ �������
#if MATERIAL
			value_white_material = value_white_material 
				- PAWN;	
#endif//#if MATERIAL
			
			white_name_from_coord[(f_p + 8)] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//3 ��������� 0-0-0
		case LONG_CASTLING:

			//������� ������ �� ������ �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = black_name_from_coord[i_p];
			black_name_from_coord[i_p] = 0;

			all_black_piece = all_black_piece ^ move_masks[A8];
			black_piece[black_name_from_coord[A8]] 
			= black_piece[black_name_from_coord[A8]] ^ move_masks[A8];
			black_name_from_coord[D8] = black_name_from_coord[A8];
			black_name_from_coord[A8] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			all_black_piece = all_black_piece ^ move_masks[D8];
			black_piece[black_name_from_coord[D8]] 
			= black_piece[black_name_from_coord[D8]] ^ move_masks[D8];


			// ������ - ���� ��������� 
			undo.return_castling_q = castling_q;
			undo.return_castling_k = castling_k;
			castling_q = 0;
			castling_k = 0; 

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//4 ��������� 0-0
		case SHORT_CASTLING:

			//������� ������ �� ������ ����� 
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = black_name_from_coord[i_p];
			black_name_from_coord[i_p] = 0;

			all_black_piece = all_black_piece ^ move_masks[H8];
			black_piece[black_name_from_coord[H8]] 
			= black_piece[black_name_from_coord[H8]] ^ move_masks[H8];
			black_name_from_coord[F8] = black_name_from_coord[H8];
			black_name_from_coord[H8] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			all_black_piece = all_black_piece ^ move_masks[F8];
			black_piece[black_name_from_coord[F8]] 
			= black_piece[black_name_from_coord[F8]] ^ move_masks[F8];


			// ������ - ���� ���������
			undo.return_castling_q = castling_q;
			undo.return_castling_k = castling_k;
			castling_q = 0;
			castling_k = 0;

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//15 ����������� � �����
		case TRANSFORMATION_IN_QUEEN:

			//������� ������ �� ������ �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = N_QUEEN;
			black_name_from_coord[i_p] = 0;

			// ����������� �������
#if MATERIAL
			value_black_material = 
				value_black_material + QUEEN - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//14 ����������� � �����
		case TRANSFORMATION_IN_ROOK:

			//������� ������ �� ������ ����� 
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = N_ROOK;
			black_name_from_coord[i_p] = 0;

			// ����������� �������
			#if MATERIAL
				value_black_material = 
					value_black_material + ROOK - PAWN;	
			#endif//#if MATERIAL

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//13 ����������� � �����
		case TRANSFORMATION_IN_BISHOP:

			//������� ������ �� ������ �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = N_BISHOP;
			black_name_from_coord[i_p] = 0;

			// ����������� �������
#if MATERIAL
			value_black_material = 
				value_black_material + BISHOP - PAWN;	
#endif//#if MATERIAL


			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//12 ����������� � ����
		case TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ ����� 
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			black_name_from_coord[f_p] = N_KNIGHT;
			black_name_from_coord[i_p] = 0;

			// ����������� �������
#if MATERIAL
			value_black_material = 
				value_black_material + KNIGHT - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			all_piece = all_white_piece | all_black_piece;

		break;

		//25 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[f_p];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			value_black_material = 
				value_black_material + QUEEN - PAWN;

			value_white_material = value_white_material 
				- KOD[white_name_from_coord[f_p]];
#endif//#if MATERIAL


			white_name_from_coord[f_p] = 0;

			black_name_from_coord[f_p] = N_QUEEN;
			black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = castling_Q;
					castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = castling_K;
					castling_K = 0;
				}	
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		//24 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[f_p];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			value_black_material = 
				value_black_material + ROOK - PAWN;

			value_white_material = value_white_material 
				- KOD[white_name_from_coord[f_p]];
#endif//#if MATERIAL

			white_name_from_coord[f_p] = 0;

			black_name_from_coord[f_p] = N_ROOK;
			black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = castling_Q;
					castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = castling_K;
					castling_K = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		//23 ������ � ������������ �����
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[f_p];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			value_black_material = 
				value_black_material + BISHOP - PAWN;

			value_white_material = value_white_material 
				- KOD[white_name_from_coord[f_p]];
#endif//#if MATERIAL

			white_name_from_coord[f_p] = 0;

			black_name_from_coord[f_p] = N_BISHOP;
			black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = castling_Q;
					castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = castling_K;
					castling_K = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		//22 ������ � ������������  � ����
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[f_p];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			value_black_material 
				= value_black_material + KNIGHT - PAWN;

			value_white_material = value_white_material 
				- KOD[white_name_from_coord[f_p]];
#endif//#if MATERIAL

			white_name_from_coord[f_p] = 0;
			black_name_from_coord[f_p] = N_KNIGHT;
			black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = castling_Q;
					castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = castling_K;
					castling_K = 0;
				}
			}

			all_piece = all_white_piece | all_black_piece;

		break;

		default:;//��� �� ���������
   
	}

}

//	===================================================================

// ��� ����� ��� �����
void Chess_bitboard::undo_moves_white
(
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const class List_surplus_moves & list_surplus_moves//������ ��������� �����
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// ��������������� ���� ������ �� �������
	taking_passage_yes=undo.return_taking_passage;

	// ����� ������� ���� ��� ������ ����� �� ��� ����
	colour_move = 1;

	/*
	��� ����
	0 - ��� ����
	1 � ������� ���
	2 � ������
	3 � ������� ���������
	4 � �������� ���������
	5 � ������ �� �������

	12 - ����������� � ���� 
	13 - ����������� � ����
	14 - ����������� � ����� 
	15 - ����������� � ����� 

	22 - ������ � ������������ � ���� 
	23 - ������ � ������������ � ����
	24 - ������ � ������������ � ����� 
	25 - ������ � ������������ � ����� 
	*/

	switch (((list_surplus_moves.move[i]>>6)& 63))
	{
		//1 ��� �� ������ ����
		case SIMPLE_MOVE:
 
			// ����� - ���� ������ �� ������� 
			if (white_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					taking_passage_yes =undo.return_taking_passage;
					taking_passage_x = undo.taking_passage_x;
					taking_passage_y = undo.taking_passage_y;
				}

				
			}// ������ - ���� ��������� 
			else if (white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				castling_K = undo.return_castling_K;
				castling_Q = undo.return_castling_Q;   

	    
			}// ����� - ���� ���������   
			else if (white_name_from_coord[f_p] == N_ROOK)
			{ 
				if (i_p == A1)//0
				{
					castling_Q = undo.return_castling_Q;
				}

				if (i_p == H1)//7
				{
					castling_K = undo.return_castling_K;
				}
			}

			//������� ������ �� ������ �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[i_p] = white_name_from_coord[f_p];
			white_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];
			
			all_piece = all_white_piece | all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			// ������ - ���� ���������
			if (white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				castling_K = undo.return_castling_K;
				castling_Q = undo.return_castling_Q;   

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (white_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A1)//0
				{
					castling_Q = undo.return_castling_Q;
				}

				if (i_p == H1)//7
				{
					castling_K = undo.return_castling_K;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					castling_q = undo.return_castling_q;
				}

				if (f_p == H8)//63
				{
					castling_k = undo.return_castling_k;
				}
			}

			//������� ������ �� ������ ����� 
			// ������� ������ �� ������ ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[i_p] = white_name_from_coord[f_p];
			white_name_from_coord[f_p] = 0;

			// �������������� ��� ������ ������
			black_name_from_coord[f_p] = t_f;

			// ������ ������
			// � ������ ��������� ������ �� ���������� �������� ������ ������ 
			// (�������� ���)
#if MATERIAL
			value_black_material = value_black_material 
				+ KOD[t_f];
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// � ����� ��� ������ ������
			all_black_piece = all_black_piece ^ move_masks[f_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ������� ������ ������ ������
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			all_piece = all_white_piece | all_black_piece;

		break;

		//5 ������ �� �������
		case TAKING_PASSAGE:

			//������� ������ �� ������ �����
			// ������� ������ �� ������ ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[i_p] = white_name_from_coord[f_p];
			white_name_from_coord[f_p] = 0;

			// �������������� ��� ������ ������
			black_name_from_coord[(f_p - 8)] = t_f;

			// ������ ������
			// � ������ ��������� ������ �� ���������� �������� ������ ������ 
			// (�������� ���)
#if MATERIAL
			value_black_material = value_black_material 
				+ PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// � ����� ��� ������ ������
			all_black_piece = all_black_piece ^ move_masks[(f_p - 8)];

			// ������� ������ ������
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ������� ������ ������ ������
			black_piece[black_name_from_coord[(f_p - 8)]] = black_piece[black_name_from_coord[(f_p - 8)]] ^ move_masks[(f_p - 8)];

			all_piece = all_white_piece | all_black_piece;
   
		break;

		//3 0-0-0
		case LONG_CASTLING:

			// ������ - ���� ���������
			castling_Q = undo.return_castling_Q;
			castling_K = undo.return_castling_K;   

			//������� ������ �� ������ �����
			// ������
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[i_p] = white_name_from_coord[f_p];
			white_name_from_coord[f_p] = 0;

			// �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[D1];

			// ������� ������ ������
			white_piece[white_name_from_coord[D1]] 
			= white_piece[white_name_from_coord[D1]] ^ move_masks[D1];

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[A1] = white_name_from_coord[D1];
			white_name_from_coord[D1] = 0;

			//������ ������ �� ����� �����
			// ������
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[A1];

			// ������� ������ ������
			white_piece[white_name_from_coord[A1]] 
			= white_piece[white_name_from_coord[A1]] ^ move_masks[A1];

			all_piece = all_white_piece | all_black_piece;

		break;

		//4 0-0
		case SHORT_CASTLING:

			// ������ - ���� ���������
			castling_Q = undo.return_castling_Q;
			castling_K = undo.return_castling_K;   

			//������� ������ �� ������ �����
			// ������
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[i_p] = white_name_from_coord[f_p];
			white_name_from_coord[f_p] = 0;

			// �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[F1];

			// ������� ������ ������
			white_piece[white_name_from_coord[F1]] 
			= white_piece[white_name_from_coord[F1]] ^ move_masks[F1];

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[H1] = white_name_from_coord[F1];
			white_name_from_coord[F1] = 0;

			//������ ������ �� ����� �����
			// ������
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[H1];

			// ������� ������ ������
			white_piece[white_name_from_coord[H1]] 
			= white_piece[white_name_from_coord[H1]] ^ move_masks[H1];

			all_piece = all_white_piece | all_black_piece;

		break;

		//������ 15-����� 14-����� 13-����� 12-����
		case TRANSFORMATION_IN_QUEEN:
		case TRANSFORMATION_IN_ROOK:
		case TRANSFORMATION_IN_BISHOP:
		case TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� ������
			// �� ������ ��������� ����� �� �������� �������� ������ 
			// ����������� � ���������� �����
			// (�������� ���)
#if MATERIAL
			value_white_material = value_white_material 
				- KOD[white_name_from_coord[f_p]] + PAWN;
#endif//#if MATERIAL

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[i_p] = N_PAWN;
			white_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			// ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];
			
			all_piece = all_white_piece | all_black_piece;

		break;

		// ������ ������ � ������������ 25-����� 24-����� 23-����� 22-����
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			//  ���������� ���� ������� ����� 
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					castling_q = undo.return_castling_q;
				}

				if (f_p == H8)//63
				{
					castling_k = undo.return_castling_k;
				}
			}

			//������� ������ �� ������ �����
			// ������� ������ �� ������ ���� ����� �����
			all_white_piece = all_white_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];


			// ����������� ������
			// �� ������ ��������� ����� �� �������� �������� ������ 
			// ����������� � ���������� �����
			// � ������ ��������� ������ �� ���������� �������� ������ ������
			// (�������� ���)
#if MATERIAL
			value_white_material = value_white_material 
				- KOD[white_name_from_coord[f_p]] + PAWN;

			value_black_material = value_black_material 
				+ KOD[t_f];
#endif//#if MATERIAL
			//------------------------------------------------------------------------------

			// ��������� ��� ������ � �������� �� ������ ����������
			white_name_from_coord[i_p] = N_PAWN;
			white_name_from_coord[f_p] = 0;

			// ��������������� ��� ������ ������
			black_name_from_coord[f_p] = t_f;

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			all_white_piece = all_white_piece ^ move_masks[i_p];

			// � ����� ��� ������ ������
			all_black_piece = all_black_piece ^ move_masks[f_p];

			// ������� ������ ������
			white_piece[white_name_from_coord[i_p]] = white_piece[white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ������� ������ ������ ������
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			all_piece = all_white_piece | all_black_piece;

		break;

		default:;//��� �� ���������
	}
}


//	===================================================================
// ��� ����� ��� ������
void Chess_bitboard::undo_moves_black
(
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const class List_surplus_moves & list_surplus_moves//������ ��������� �����
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// �������������� ���� ������ �� �������
	taking_passage_yes=undo.return_taking_passage;

	// ������ ������� ���� ��� ������ ����� �� ��� ����
	colour_move = 0;

	/*
	��� ����
	0 - ��� ����
	1 � ������� ���
	2 � ������
	3 � ������� ���������
	4 � �������� ���������
	5 � ������ �� �������

	12 - ����������� � ���� 
	13 - ����������� � ����
	14 - ����������� � ����� 
	15 - ����������� � ����� 

	22 - ������ � ������������ � ���� 
	23 - ������ � ������������ � ����
	24 - ������ � ������������ � ����� 
	25 - ������ � ������������ � ����� 
	*/

	switch (((list_surplus_moves.move[i]>>6)& 63))
	{
		//1 ��� �� ������ ����
		case SIMPLE_MOVE:

			// ����� - ���� ������ �� ������� 
			if (black_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					taking_passage_yes =undo.return_taking_passage;
					taking_passage_x = undo.taking_passage_x;
					taking_passage_y = undo.taking_passage_y;
				}

			}// ������ - ���� ��������� 
			else if (black_name_from_coord[f_p] == N_KING)
			{
				castling_k = undo.return_castling_k;
				castling_q = undo.return_castling_q;

			}// ����� - ���� ���������  
			else if (black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					castling_q = undo.return_castling_q;
				}

				if (i_p == H8)//63
				{
					castling_k =  undo.return_castling_k;
				}

			}

			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];
			black_name_from_coord[i_p] = black_name_from_coord[f_p];
			black_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];

			
			all_piece = all_white_piece | all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			// ������ - ���� ���������
			if (black_name_from_coord[f_p] == N_KING)
			{
				castling_k = undo.return_castling_k;
				castling_q = undo.return_castling_q;

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					castling_q = undo.return_castling_q;
				}

				if (i_p == H8)//63
				{
					castling_k = undo.return_castling_k;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					castling_Q = undo.return_castling_Q;
				}

				if (f_p == H1)//7
				{
					castling_K = undo.return_castling_K;
				}
			}

			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			black_name_from_coord[i_p] = black_name_from_coord[f_p];
			black_name_from_coord[f_p] = 0;

			white_name_from_coord[f_p] = t_f;

			// ������ ������
			// (�������� ���)
#if MATERIAL
			value_white_material = value_white_material 
				+ KOD[t_f];
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[f_p];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];

			
			all_piece = all_white_piece | all_black_piece;

		break;

		//5 ������ �� �������
		case TAKING_PASSAGE:

			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			black_name_from_coord[i_p] = black_name_from_coord[f_p];
			black_name_from_coord[f_p] = 0;

			white_name_from_coord[(f_p + 8)] = t_f;

			// ������ ������
			// (�������� ���)
#if MATERIAL
			value_white_material = value_white_material 
				+ PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[(f_p + 8)];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[(f_p + 8)]] = white_piece[white_name_from_coord[(f_p + 8)]] ^ move_masks[(f_p + 8)];

			
			all_piece = all_white_piece | all_black_piece;
   
		break;

		//3 0-0-0
		case LONG_CASTLING:

			// ������ - ���� ���������
			castling_q = undo.return_castling_q;
			castling_k = undo.return_castling_k;

			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];
			black_name_from_coord[i_p] = black_name_from_coord[f_p];
			black_name_from_coord[f_p] = 0;

			all_black_piece = all_black_piece ^ move_masks[D8];
			black_piece[black_name_from_coord[D8]] 
			= black_piece[black_name_from_coord[D8]] ^ move_masks[D8];
			black_name_from_coord[A8] = black_name_from_coord[D8];
			black_name_from_coord[D8] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];

			all_black_piece = all_black_piece ^ move_masks[A8];
			black_piece[black_name_from_coord[A8]] 
			= black_piece[black_name_from_coord[A8]] ^ move_masks[A8];

			all_piece = all_white_piece | all_black_piece;

		break;

		//4 0-0
		case SHORT_CASTLING:
 
			// ������ - ���� ���������
			castling_q = undo.return_castling_q;
			castling_k = undo.return_castling_k;

			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];
			black_name_from_coord[i_p] = black_name_from_coord[f_p];
			black_name_from_coord[f_p] = 0;

			all_black_piece = all_black_piece ^ move_masks[F8];
			black_piece[black_name_from_coord[F8]] 
			= black_piece[black_name_from_coord[F8]] ^ move_masks[F8];
			black_name_from_coord[H8] = black_name_from_coord[F8];
			black_name_from_coord[F8] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];

			all_black_piece = all_black_piece ^ move_masks[H8];
			black_piece[black_name_from_coord[H8]] 
			= black_piece[black_name_from_coord[H8]] ^ move_masks[H8];

			all_piece = all_white_piece | all_black_piece;

		break;

		// ������ ������ 15-������ 14-����� 13-����� 12-����
		case TRANSFORMATION_IN_QUEEN:
		case TRANSFORMATION_IN_ROOK:
		case TRANSFORMATION_IN_BISHOP:
		case TRANSFORMATION_IN_KNIGHT:

			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];
		  
			// ����������� ������
			// (�������� ���)
#if MATERIAL
			value_black_material = value_black_material
				- KOD[black_name_from_coord[f_p]] + PAWN; 

#endif//#if MATERIAL
			
			black_name_from_coord[i_p] = N_PAWN;
			black_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			
			all_piece = all_white_piece | all_black_piece;

		break;

		//������ ������ � ������������ 25-������ 24-����� 23-����� 22-����
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					castling_Q = undo.return_castling_Q;
				}

				if (f_p == H1)//7
				{
					castling_K = undo.return_castling_K;
				}
			}
			all_black_piece = all_black_piece ^ move_masks[f_p];
			black_piece[black_name_from_coord[f_p]] = black_piece[black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� ������
			// (�������� ���)
#if MATERIAL
			value_black_material = value_black_material 
				- KOD[black_name_from_coord[f_p]] + PAWN; 

			value_white_material = 
				value_white_material + KOD[t_f];
#endif//#if MATERIAL

			black_name_from_coord[i_p] = N_PAWN;
			black_name_from_coord[f_p] = 0;

			white_name_from_coord[f_p] = t_f;

			//������ ������ �� ����� �����
			all_black_piece = all_black_piece ^ move_masks[i_p];
			all_white_piece = all_white_piece ^ move_masks[f_p];

			black_piece[black_name_from_coord[i_p]] = black_piece[black_name_from_coord[i_p]] ^ move_masks[i_p];
			white_piece[white_name_from_coord[f_p]] = white_piece[white_name_from_coord[f_p]] ^ move_masks[f_p];
			
			all_piece = all_white_piece | all_black_piece;

		break;

		default:;//��� �� ���������
	}
}

/*
 ��� ������� 

N_PAWN      1
N_KNIGHT    2
N_BISHOP    3
N_ROOK      4
N_QUEEN     5
N_KING      6
*/
