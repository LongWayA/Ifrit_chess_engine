
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 23.02.2009 26.06.2010  21.11.2010

#include <iostream>


#include "b_constants.h"
#include "evaluate.h"
#include "b_evaluate_const.h"
#include "b_move_generation_const.h"
#include "make_move.h"


//���������� ��� ������ ����
struct Undo d_undo[MAX_DEPTH];

//	===================================================================
struct Undo Make_move::get_d_undo(const unsigned __int8 depth)
{
	return  d_undo[depth];
};

//	===================================================================
// ��������� ���� ������ ��� �����
void Make_move::do_moves_white
(
	struct Position & position, //
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const struct List & list_surplus_moves//������ ��������� �����
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12) & 63);// �������� ������� ������
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18) & 63);// ��������� ������� ������
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);// ������ ������

	// ���������� ���� ������ �� �������
	undo.return_taking_passage = position.taking_passage_yes;

	// ������ ����������(����� ��� ���-�������)
	undo.taking_passage_x = position.taking_passage_x;
	undo.taking_passage_y = position.taking_passage_y;

	// ��������� ������ �� ������� 
	position.taking_passage_yes = 0;

	// ����� ������ ������ ������ ��� ������
	position.colour_move = 0;

	// ������ ���������� ����� ��������� (����� ��� ���-�������)
	undo.return_castling_K = position.castling_K;
	undo.return_castling_Q = position.castling_Q;
	undo.return_castling_k = position.castling_k;
	undo.return_castling_q = position.castling_q;

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

	switch (((list_surplus_moves.move[i]>>6) & 63))
	{
		//1 ��� �� ������ ����
		case SIMPLE_MOVE:

			//������� ������ �� ������ �����---------------------------------------------- 
			//������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			//������� ������ �� �������� ������ �����
			position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			//��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����------------------------------------------------
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];
			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			//---------------------------------------------------------------------------------
			// ����� �������� ������ �������� �� ��������� � ������ �� �������
			// �� ��� ������� ����� ������� ������ � ���� ��� ����� �� ��������� �� �����
			// �� �������� ������ �� ������� ���� ��� ������ �� �������� ��� ���������
			// � ���� ����� �� �������� ��������� � ��� 
			//---------------------------------------------------------------------------------

			// ����� - ���� ������ �� �������
			if ( position.white_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					undo.taking_passage_x = position.taking_passage_x;
					undo.taking_passage_y = position.taking_passage_y;
					position.taking_passage_yes = 1;
					position.taking_passage_x =(i_p + 8) % 8;
					position.taking_passage_y =(i_p + 8) / 8;
				}
			}// ������ - ���� ���������
			else if ( position.white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				undo.return_castling_K = position.castling_K;
				undo.return_castling_Q = position.castling_Q;
				position.castling_K = 0;
				position.castling_Q = 0;	   	
			}// ����� - ���� ���������  
			else if ( position.white_name_from_coord[f_p] == N_ROOK)
			{
				if (i_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (i_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}
			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ ������
			// � ������ �������� ������
#if MATERIAL
			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// �������� ������ ������
			position.black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];
			// ������ ������� ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ - ���� ���������
			if ( position.white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				undo.return_castling_K = position.castling_K;
				undo.return_castling_Q = position.castling_Q;
				position.castling_K = 0;
				position.castling_Q = 0;		
  
			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if ( position.white_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q =0;
				}

				if (i_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K =0;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)//63
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;
		break;

		//5 ������ �� �������
		case TAKING_PASSAGE:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			// ������� ������ ������ �� ����� ���� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[(f_p - 8)];
			// ����� ���� ������� ������ ������ �� �������� ������ �����
			position.black_piece[position.black_name_from_coord[(f_p - 8)]] = position.black_piece[position.black_name_from_coord[(f_p - 8)]] ^ move_masks[(f_p - 8)];

			// ������ ������
			// � ������ �������� �����
#if MATERIAL
			position.value_black_material = position.value_black_material 
				- PAWN;	
#endif//#if MATERIAL


			// �������� ������ ������
			position.black_name_from_coord[(f_p - 8)] = 0;

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//3 ��������� 0-0-0
		case LONG_CASTLING:

			//������� ������ �� ������ �����
			// ��� �������
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			// ��� ������
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[A1];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[A1]] 
			= position.white_piece[ position.white_name_from_coord[A1]] ^ move_masks[A1];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[D1] =  position.white_name_from_coord[A1];
			 position.white_name_from_coord[A1] = 0;

			//������ ������ �� ����� �����------------------
			// ��� �������
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��� ������
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[D1];

			// ������ ������ � ������� ������ �����
			 position.white_piece[position.white_name_from_coord[D1]] 
			= position.white_piece[position.white_name_from_coord[D1]] ^ move_masks[D1];

			// ������ - ���� ���������
			// ���������� ����� ���������
			undo.return_castling_Q = position.castling_Q;
			undo.return_castling_K = position.castling_K;
			position.castling_Q = 0;
			position.castling_K = 0;

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//4 ��������� 0-0
		case SHORT_CASTLING:

			//������� ������ �� ������ �����
			// ��� �������
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] =  position.white_name_from_coord[i_p];
			 position.white_name_from_coord[i_p] = 0;

			// ��� ������
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[H1];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[H1]] 
			= position.white_piece[ position.white_name_from_coord[H1]] ^ move_masks[H1];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[F1] =  position.white_name_from_coord[H1];
			 position.white_name_from_coord[H1] = 0;

			//������ ������ �� ����� �����
			// ��� �������
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��� ������
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[F1];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[F1]] 
			= position.white_piece[ position.white_name_from_coord[F1]] ^ move_masks[F1];

			// ������ - ���� ���������
			// ���������� ����� ���������
			undo.return_castling_Q = position.castling_Q;
			undo.return_castling_K = position.castling_K;
			position.castling_Q = 0;
			position.castling_K = 0;

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//15 ����������� � �����
		case TRANSFORMATION_IN_QUEEN:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_QUEEN;
			 position.white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ QUEEN - PAWN;
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//14 ����������� � �����
		case TRANSFORMATION_IN_ROOK:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_ROOK;
			 position.white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ ROOK - PAWN;	
#endif//#if MATERIAL
			

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//13 ����������� � �����
		case TRANSFORMATION_IN_BISHOP:

			//������� ������ �� ������ ����� 
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_BISHOP;
			 position.white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
				position.value_white_material = position.value_white_material 
					+ BISHOP - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//12 ����������� � ����
		case TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_KNIGHT;
			 position.white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ KNIGHT - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;
		break;

		//25 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + QUEEN - PAWN;

			position.value_black_material = position.value_black_material	
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			position.black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_QUEEN;
			 position.white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������� ������ � ������� ������ �����
			 position.white_piece[position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//24 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + ROOK - PAWN;

			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			position.black_name_from_coord[f_p] = 0;
			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_ROOK;
			 position.white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];
			// ������ ������� ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//23 ������ � ������������ �����
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];


			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + BISHOP - PAWN;

			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// �������� ������ ������
			position.black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_BISHOP;
			 position.white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������� ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//22 ������ � ������������  � ����
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// ������� ������ �� ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];


			// ������ � ������������ ������
			// � ������ ��������� �����
			// �� ���������� �������� ������ ����������� � �������� �����
			// �� ������ �������� ������ �������� ������ ������
#if MATERIAL
			position.value_white_material = 
				position.value_white_material + KNIGHT - PAWN;

			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			position.black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[f_p] = N_KNIGHT;
			 position.white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������ ������� ������ � ������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];  

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == N_ROOK)
			{
				if (f_p == A8)
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (f_p == H8)
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		default:;//��� �� ���������
   
	}

}

//	===================================================================
// ��������� ���� ������ ��� ������
void Make_move::do_moves_black
(
	struct Position & position, //
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const struct List & list_surplus_moves//������ ��������� �����
)
{
	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// ���������� ���� ������ �� �������
	undo.return_taking_passage= position.taking_passage_yes;

	// ������ ����������(����� ��� ���-�������)
	undo.taking_passage_x = position.taking_passage_x;
	undo.taking_passage_y = position.taking_passage_y;


	// ��������� ������ �� ������� 
	position.taking_passage_yes = 0;

	// ������ ������ ������ ������ ��� �����
	position.colour_move = 1;

	// ������ ���������� ����� ��������� (����� ��� ���-�������)
	undo.return_castling_K = position.castling_K;
	undo.return_castling_Q = position.castling_Q;
	undo.return_castling_k = position.castling_k;
	undo.return_castling_q = position.castling_q;


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
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����� - ���� ������ �� ������� 
			if (position.black_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					//cout << "i_p= "<< i_p<<std::endl;
					//cout << "f_p= "<< f_p<<std::endl;
					//cout << "position.black_name_from_coord[f_p]= "<< position.black_name_from_coord[f_p]<<std::endl;
					undo.taking_passage_x = position.taking_passage_x;
					undo.taking_passage_y = position.taking_passage_y;
					position.taking_passage_yes =1;
					position.taking_passage_x =(i_p - 8) % 8;
					position.taking_passage_y =(i_p - 8) / 8;
				}
			
			}// ������ - ���� ���������
			else if (position.black_name_from_coord[f_p] == N_KING)
			{
				undo.return_castling_k = position.castling_k;
				undo.return_castling_q = position.castling_q;
				position.castling_k = 0;
				position.castling_q = 0;

    
			}// ����� - ���� ���������   
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
			}

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			//������� ������ �� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ �������
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL


			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������ - ���� ���������
			if (position.black_name_from_coord[f_p] == N_KING)
			{
				undo.return_castling_k = position.castling_k;
				undo.return_castling_q = position.castling_q;
				position.castling_k = 0;
				position.castling_q = 0;

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					undo.return_castling_q = position.castling_q;
					position.castling_q = 0;
				}

				if (i_p == H8)//63
				{
					undo.return_castling_k = position.castling_k;
					position.castling_k = 0;
				}
     
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		// 5 ������ �� �������
		case TAKING_PASSAGE:

			//������� ������ �� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			position.all_white_piece = position.all_white_piece ^ move_masks[(f_p + 8)];
			 position.white_piece[ position.white_name_from_coord[(f_p + 8)]] = position.white_piece[ position.white_name_from_coord[(f_p + 8)]] ^ move_masks[(f_p + 8)];
  
			// ������ �������
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- PAWN;	
#endif//#if MATERIAL
			
			 position.white_name_from_coord[(f_p + 8)] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//3 ��������� 0-0-0
		case LONG_CASTLING:

			//������� ������ �� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			 position.all_black_piece =  position.all_black_piece ^ move_masks[A8];
			position.black_piece[position.black_name_from_coord[A8]] 
			= position.black_piece[position.black_name_from_coord[A8]] ^ move_masks[A8];
			position.black_name_from_coord[D8] = position.black_name_from_coord[A8];
			position.black_name_from_coord[A8] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			 position.all_black_piece =  position.all_black_piece ^ move_masks[D8];
			position.black_piece[position.black_name_from_coord[D8]] 
			= position.black_piece[position.black_name_from_coord[D8]] ^ move_masks[D8];


			// ������ - ���� ��������� 
			undo.return_castling_q = position.castling_q;
			undo.return_castling_k = position.castling_k;
			position.castling_q = 0;
			position.castling_k = 0; 

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//4 ��������� 0-0
		case SHORT_CASTLING:

			//������� ������ �� ������ ����� 
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = position.black_name_from_coord[i_p];
			position.black_name_from_coord[i_p] = 0;

			 position.all_black_piece =  position.all_black_piece ^ move_masks[H8];
			position.black_piece[position.black_name_from_coord[H8]] 
			= position.black_piece[position.black_name_from_coord[H8]] ^ move_masks[H8];
			position.black_name_from_coord[F8] = position.black_name_from_coord[H8];
			position.black_name_from_coord[H8] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			 position.all_black_piece =  position.all_black_piece ^ move_masks[F8];
			position.black_piece[position.black_name_from_coord[F8]] 
			= position.black_piece[position.black_name_from_coord[F8]] ^ move_masks[F8];


			// ������ - ���� ���������
			undo.return_castling_q = position.castling_q;
			undo.return_castling_k = position.castling_k;
			position.castling_q = 0;
			position.castling_k = 0;

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//15 ����������� � �����
		case TRANSFORMATION_IN_QUEEN:

			//������� ������ �� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_QUEEN;
			position.black_name_from_coord[i_p] = 0;

			// ����������� �������
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + QUEEN - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//14 ����������� � �����
		case TRANSFORMATION_IN_ROOK:

			//������� ������ �� ������ ����� 
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_ROOK;
			position.black_name_from_coord[i_p] = 0;

			// ����������� �������
			#if MATERIAL
				position.value_black_material = 
					position.value_black_material + ROOK - PAWN;	
			#endif//#if MATERIAL

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//13 ����������� � �����
		case TRANSFORMATION_IN_BISHOP:

			//������� ������ �� ������ �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_BISHOP;
			position.black_name_from_coord[i_p] = 0;

			// ����������� �������
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + BISHOP - PAWN;	
#endif//#if MATERIAL


			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//12 ����������� � ����
		case TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ ����� 
			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			position.black_name_from_coord[f_p] = N_KNIGHT;
			position.black_name_from_coord[i_p] = 0;

			// ����������� �������
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + KNIGHT - PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ������������ ����� ���� �����
			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//25 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + QUEEN - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL


			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = N_QUEEN;
			position.black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}	
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//24 ������ � ������������ � �����
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + ROOK - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL

			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = N_ROOK;
			position.black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//23 ������ � ������������ �����
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			position.value_black_material = 
				position.value_black_material + BISHOP - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL

			 position.white_name_from_coord[f_p] = 0;

			position.black_name_from_coord[f_p] = N_BISHOP;
			position.black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece =  position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece |  position.all_black_piece;

		break;

		//22 ������ � ������������  � ����
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			 position.all_black_piece =  position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� � ������� �������
#if MATERIAL
			position.value_black_material 
				= position.value_black_material + KNIGHT - PAWN;

			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]];
#endif//#if MATERIAL

			 position.white_name_from_coord[f_p] = 0;
			position.black_name_from_coord[f_p] = N_KNIGHT;
			position.black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			 position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					undo.return_castling_Q = position.castling_Q;
					position.castling_Q = 0;
				}

				if (f_p == H1)//7
				{
					undo.return_castling_K = position.castling_K;
					position.castling_K = 0;
				}
			}

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		default:;//��� �� ���������
   
	}

}

//	===================================================================

// ��� ����� ��� �����
void Make_move::undo_moves_white
(
	struct Position & position, //
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const struct List & list_surplus_moves//������ ��������� �����
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// ��������������� ���� ������ �� �������
	position.taking_passage_yes=undo.return_taking_passage;

	// ����� ������� ���� ��� ������ ����� �� ��� ����
	position.colour_move = 1;

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
			if ( position.white_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == 16)
				{
					position.taking_passage_yes =undo.return_taking_passage;
					position.taking_passage_x = undo.taking_passage_x;
					position.taking_passage_y = undo.taking_passage_y;
				}

				
			}// ������ - ���� ��������� 
			else if ( position.white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				position.castling_K = undo.return_castling_K;
				position.castling_Q = undo.return_castling_Q;   

	    
			}// ����� - ���� ���������   
			else if (position.white_name_from_coord[f_p] == N_ROOK)
			{ 
				if (i_p == A1)//0
				{
					position.castling_Q = undo.return_castling_Q;
				}

				if (i_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}

			//������� ������ �� ������ �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			// ������ - ���� ���������
			if (position.white_name_from_coord[f_p] == N_KING)
			{
				// ���������� ����� ���������
				position.castling_K = undo.return_castling_K;
				position.castling_Q = undo.return_castling_Q;   

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if ( position.white_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A1)//0
				{
					position.castling_Q = undo.return_castling_Q;
				}

				if (i_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A8)//56
				{
					position.castling_q = undo.return_castling_q;
				}

				if (f_p == H8)//63
				{
					position.castling_k = undo.return_castling_k;
				}
			}

			//������� ������ �� ������ ����� 
			// ������� ������ �� ������ ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// �������������� ��� ������ ������
			position.black_name_from_coord[f_p] = t_f;

			// ������ ������
			// � ������ ��������� ������ �� ���������� �������� ������ ������ 
			// (�������� ���)
#if MATERIAL
			position.value_black_material = position.value_black_material 
				+ KOD[t_f];
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// � ����� ��� ������ ������
			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ������� ������ ������ ������
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//5 ������ �� �������
		case TAKING_PASSAGE:

			//������� ������ �� ������ �����
			// ������� ������ �� ������ ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// �������������� ��� ������ ������
			position.black_name_from_coord[(f_p - 8)] = t_f;

			// ������ ������
			// � ������ ��������� ������ �� ���������� �������� ������ ������ 
			// (�������� ���)
#if MATERIAL
			position.value_black_material = position.value_black_material 
				+ PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// � ����� ��� ������ ������
			position.all_black_piece = position.all_black_piece ^ move_masks[(f_p - 8)];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ������� ������ ������ ������
			position.black_piece[position.black_name_from_coord[(f_p - 8)]] = position.black_piece[position.black_name_from_coord[(f_p - 8)]] ^ move_masks[(f_p - 8)];

			position.all_piece = position.all_white_piece | position.all_black_piece;
   
		break;

		//3 0-0-0
		case LONG_CASTLING:

			// ������ - ���� ���������
			position.castling_Q = undo.return_castling_Q;
			position.castling_K = undo.return_castling_K;   

			//������� ������ �� ������ �����
			// ������
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[D1];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[D1]] 
			= position.white_piece[ position.white_name_from_coord[D1]] ^ move_masks[D1];

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[A1] =  position.white_name_from_coord[D1];
			 position.white_name_from_coord[D1] = 0;

			//������ ������ �� ����� �����
			// ������
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[A1];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[A1]] 
			= position.white_piece[ position.white_name_from_coord[A1]] ^ move_masks[A1];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//4 0-0
		case SHORT_CASTLING:

			// ������ - ���� ���������
			position.castling_Q = undo.return_castling_Q;
			position.castling_K = undo.return_castling_K;   

			//������� ������ �� ������ �����
			// ������
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[i_p] =  position.white_name_from_coord[f_p];
			 position.white_name_from_coord[f_p] = 0;

			// �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[F1];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[F1]] 
			= position.white_piece[ position.white_name_from_coord[F1]] ^ move_masks[F1];

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[H1] =  position.white_name_from_coord[F1];
			 position.white_name_from_coord[F1] = 0;

			//������ ������ �� ����� �����
			// ������
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[H1];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[H1]] 
			= position.white_piece[ position.white_name_from_coord[H1]] ^ move_masks[H1];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//������ 15-����� 14-����� 13-����� 12-����
		case TRANSFORMATION_IN_QUEEN:
		case TRANSFORMATION_IN_ROOK:
		case TRANSFORMATION_IN_BISHOP:
		case TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� ������
			// �� ������ ��������� ����� �� �������� �������� ������ 
			// ����������� � ���������� �����
			// (�������� ���)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]] + PAWN;
#endif//#if MATERIAL

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[i_p] = N_PAWN;
			 position.white_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			// ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

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
					position.castling_q = undo.return_castling_q;
				}

				if (f_p == H8)//63
				{
					position.castling_k = undo.return_castling_k;
				}
			}

			//������� ������ �� ������ �����
			// ������� ������ �� ������ ���� ����� �����
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			// ������� ������ �� �������� ������ �����
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];


			// ����������� ������
			// �� ������ ��������� ����� �� �������� �������� ������ 
			// ����������� � ���������� �����
			// � ������ ��������� ������ �� ���������� �������� ������ ������
			// (�������� ���)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				- KOD[ position.white_name_from_coord[f_p]] + PAWN;

			position.value_black_material = position.value_black_material 
				+ KOD[t_f];
#endif//#if MATERIAL
			//------------------------------------------------------------------------------

			// ��������� ��� ������ � �������� �� ������ ����������
			 position.white_name_from_coord[i_p] = N_PAWN;
			 position.white_name_from_coord[f_p] = 0;

			// ��������������� ��� ������ ������
			position.black_name_from_coord[f_p] = t_f;

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			position.all_white_piece = position.all_white_piece ^ move_masks[i_p];

			// � ����� ��� ������ ������
			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];

			// ������� ������ ������
			 position.white_piece[ position.white_name_from_coord[i_p]] = position.white_piece[ position.white_name_from_coord[i_p]] ^ move_masks[i_p];

			// ������� ������ ������ ������
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		default:;//��� �� ���������
	}
}


//	===================================================================
// ��� ����� ��� ������
void Make_move::undo_moves_black
(
	struct Position & position, //
	const unsigned __int8 & i,//����� ���� �� ������
	const unsigned __int8 & depth,//������� ������
	const struct List & list_surplus_moves//������ ��������� �����
)
{

	struct Undo & undo = d_undo[depth];

	unsigned __int8 i_p = ((list_surplus_moves.move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves.move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves.move[i] & 63);


	// �������������� ���� ������ �� �������
	position.taking_passage_yes=undo.return_taking_passage;

	// ������ ������� ���� ��� ������ ����� �� ��� ����
	position.colour_move = 0;

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
			if (position.black_name_from_coord[f_p] == N_PAWN)
			{
				if (f_p - i_p == -16)
				{
					position.taking_passage_yes =undo.return_taking_passage;
					position.taking_passage_x = undo.taking_passage_x;
					position.taking_passage_y = undo.taking_passage_y;
				}

			}// ������ - ���� ��������� 
			else if (position.black_name_from_coord[f_p] == N_KING)
			{
				position.castling_k = undo.return_castling_k;
				position.castling_q = undo.return_castling_q;

			}// ����� - ���� ���������  
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					position.castling_q = undo.return_castling_q;
				}

				if (i_p == H8)//63
				{
					position.castling_k =  undo.return_castling_k;
				}

			}

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];

			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//2 ������
		case CAPTURE_MOVE:

			// ������ - ���� ���������
			if (position.black_name_from_coord[f_p] == N_KING)
			{
				position.castling_k = undo.return_castling_k;
				position.castling_q = undo.return_castling_q;

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (position.black_name_from_coord[f_p] == N_ROOK)
			{	
				if (i_p == A8)//56
				{
					position.castling_q = undo.return_castling_q;
				}

				if (i_p == H8)//63
				{
					position.castling_k = undo.return_castling_k;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == N_ROOK)
			{
				if (f_p == A1)//0
				{
					position.castling_Q = undo.return_castling_Q;
				}

				if (f_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			position.white_name_from_coord[f_p] = t_f;

			// ������ ������
			// (�������� ���)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ KOD[t_f];
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];

			
			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//5 ������ �� �������
		case TAKING_PASSAGE:

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			 position.white_name_from_coord[(f_p + 8)] = t_f;

			// ������ ������
			// (�������� ���)
#if MATERIAL
			position.value_white_material = position.value_white_material 
				+ PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[(f_p + 8)];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[(f_p + 8)]] = position.white_piece[ position.white_name_from_coord[(f_p + 8)]] ^ move_masks[(f_p + 8)];

			
			position.all_piece = position.all_white_piece | position.all_black_piece;
   
		break;

		//3 0-0-0
		case LONG_CASTLING:

			// ������ - ���� ���������
			position.castling_q = undo.return_castling_q;
			position.castling_k = undo.return_castling_k;

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			position.all_black_piece = position.all_black_piece ^ move_masks[D8];
			position.black_piece[position.black_name_from_coord[D8]] 
			= position.black_piece[position.black_name_from_coord[D8]] ^ move_masks[D8];
			position.black_name_from_coord[A8] = position.black_name_from_coord[D8];
			position.black_name_from_coord[D8] = 0;

			//������ ������ �� ����� �����
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];

			position.all_black_piece = position.all_black_piece ^ move_masks[A8];
			position.black_piece[position.black_name_from_coord[A8]] 
			= position.black_piece[position.black_name_from_coord[A8]] ^ move_masks[A8];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		//4 0-0
		case SHORT_CASTLING:
 
			// ������ - ���� ���������
			position.castling_q = undo.return_castling_q;
			position.castling_k = undo.return_castling_k;

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
			position.black_name_from_coord[i_p] = position.black_name_from_coord[f_p];
			position.black_name_from_coord[f_p] = 0;

			position.all_black_piece = position.all_black_piece ^ move_masks[F8];
			position.black_piece[position.black_name_from_coord[F8]] 
			= position.black_piece[position.black_name_from_coord[F8]] ^ move_masks[F8];
			position.black_name_from_coord[H8] = position.black_name_from_coord[F8];
			position.black_name_from_coord[F8] = 0;

			//������ ������ �� ����� �����
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];

			position.all_black_piece = position.all_black_piece ^ move_masks[H8];
			position.black_piece[position.black_name_from_coord[H8]] 
			= position.black_piece[position.black_name_from_coord[H8]] ^ move_masks[H8];

			position.all_piece = position.all_white_piece | position.all_black_piece;

		break;

		// ������ ������ 15-������ 14-����� 13-����� 12-����
		case TRANSFORMATION_IN_QUEEN:
		case TRANSFORMATION_IN_ROOK:
		case TRANSFORMATION_IN_BISHOP:
		case TRANSFORMATION_IN_KNIGHT:

			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];
		  
			// ����������� ������
			// (�������� ���)
#if MATERIAL
			position.value_black_material = position.value_black_material
				- KOD[position.black_name_from_coord[f_p]] + PAWN; 

#endif//#if MATERIAL
			
			position.black_name_from_coord[i_p] = N_PAWN;
			position.black_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

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
					position.castling_Q = undo.return_castling_Q;
				}

				if (f_p == H1)//7
				{
					position.castling_K = undo.return_castling_K;
				}
			}
			position.all_black_piece = position.all_black_piece ^ move_masks[f_p];
			position.black_piece[position.black_name_from_coord[f_p]] = position.black_piece[position.black_name_from_coord[f_p]] ^ move_masks[f_p];

			// ����������� ������
			// (�������� ���)
#if MATERIAL
			position.value_black_material = position.value_black_material 
				- KOD[position.black_name_from_coord[f_p]] + PAWN; 

			position.value_white_material = 
				position.value_white_material + KOD[t_f];
#endif//#if MATERIAL

			position.black_name_from_coord[i_p] = N_PAWN;
			position.black_name_from_coord[f_p] = 0;

			 position.white_name_from_coord[f_p] = t_f;

			//������ ������ �� ����� �����
			position.all_black_piece = position.all_black_piece ^ move_masks[i_p];
			position.all_white_piece = position.all_white_piece ^ move_masks[f_p];

			position.black_piece[position.black_name_from_coord[i_p]] = position.black_piece[position.black_name_from_coord[i_p]] ^ move_masks[i_p];
			 position.white_piece[ position.white_name_from_coord[f_p]] = position.white_piece[ position.white_name_from_coord[f_p]] ^ move_masks[f_p];
			
			position.all_piece = position.all_white_piece | position.all_black_piece;

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
