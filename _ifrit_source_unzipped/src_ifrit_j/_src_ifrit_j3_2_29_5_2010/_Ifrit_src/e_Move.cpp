#include <iostream> // TEST

#include "e_Move.h"
#include "h_Evaluate.h"
#include "d_Bitboard_const.h"

//---------------------------------------------------------------------------
/*++
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 23.02.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 99,99999%

 ������ ������� �� ���������� �����
*/

//	===================================================================
// ��������� ���� ������ ��� �����
void MOVES::public_onward_moves_white
(
	const unsigned __int8 & i,//����� ���� �� ������
	struct T_undo undo[],//���������� ��� ������ ����
	const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
	struct T_Bitboard * const p_bitboard// ������������� �������
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);// �������� ������� ������
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);// ��������� ������� ������
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);// ������ ������

	// ���������� ���� ������ �� �������
	undo->return_taking_passage = p_bitboard->taking_passage_yes;

	// ������ ����������(����� ��� ���-�������)
	undo->taking_passage_x = p_bitboard->taking_passage_x;
	undo->taking_passage_y = p_bitboard->taking_passage_y;

	// ��������� ������ �� ������� 
	p_bitboard->taking_passage_yes = 0;

	// ����� ������ ������ ������ ��� ������
	p_bitboard->colour_move = 0;

	// ������ ���������� ����� ��������� (����� ��� ���-�������)
	undo->return_castling_K = p_bitboard->castling_K;
	undo->return_castling_Q = p_bitboard->castling_Q;
	undo->return_castling_k = p_bitboard->castling_k;
	undo->return_castling_q = p_bitboard->castling_q;

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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{
		//1 ��� �� ������ ����
		case BITBOARDGENERATOR::SIMPLE_MOVE:

			//������� ������ �� ������ �����---------------------------------------------- 
			//������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			//������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			//��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����------------------------------------------------
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			//---------------------------------------------------------------------------------
			// ����� �������� ������ �������� �� ��������� � ������ �� �������
			// �� ��� ������� ����� ������� ������ � ���� ��� ����� �� ��������� �� �����
			// �� �������� ������ �� ������� ���� ��� ������ �� �������� ��� ���������
			// � ���� ����� �� �������� ��������� � ��� 
			//---------------------------------------------------------------------------------

			// ����� - ���� ������ �� �������
			if (p_bitboard->white_name_from_coord[f_p] == 1)
			{
				if (f_p - i_p == 16)
				{
					undo->taking_passage_x = p_bitboard->taking_passage_x;
					undo->taking_passage_y = p_bitboard->taking_passage_y;
					p_bitboard->taking_passage_yes = 1;
					p_bitboard->taking_passage_x =(i_p + 8) % 8;
					p_bitboard->taking_passage_y =(i_p + 8) / 8;
				}
			}// ������ - ���� ���������
			else if (p_bitboard->white_name_from_coord[f_p] == 6)
			{
				// ���������� ����� ���������
				undo->return_castling_K = p_bitboard->castling_K;
				undo->return_castling_Q = p_bitboard->castling_Q;
				p_bitboard->castling_K = 0;
				p_bitboard->castling_Q = 0;	   	
			}// ����� - ���� ���������  
			else if (p_bitboard->white_name_from_coord[f_p] == 4)
			{
				if (i_p == 0)
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (i_p == 7)
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}
			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 ������
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������ ������
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// �������� ������ ������
			p_bitboard->black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
			// ������ ������� ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������ - ���� ���������
			if (p_bitboard->white_name_from_coord[f_p] == 6)
			{
				// ���������� ����� ���������
				undo->return_castling_K = p_bitboard->castling_K;
				undo->return_castling_Q = p_bitboard->castling_Q;
				p_bitboard->castling_K = 0;
				p_bitboard->castling_Q = 0;		
  
			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (p_bitboard->white_name_from_coord[f_p] == 4)
			{	
				if (i_p == 0)
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q =0;
				}

				if (i_p == 7)
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K =0;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 56)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == 63)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
		break;

		//5 ������ �� �������
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ������� ������ ������ �� ����� ���� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];
			// ����� ���� ������� ������ ������ �� �������� ������ �����
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

			// ������ ������
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::PAWN;	
#endif//#if MATERIAL


			// �������� ������ ������
			p_bitboard->black_name_from_coord[(f_p - 8)] = 0;

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//3 ��������� 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			//������� ������ �� ������ �����
			// ��� �������
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ��� ������
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[0];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] ^ move_masks_m[0];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[3] = p_bitboard->white_name_from_coord[0];
			p_bitboard->white_name_from_coord[0] = 0;

			//������ ������ �� ����� �����------------------
			// ��� �������
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ��� ������
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[3];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] ^ move_masks_m[3];

			// ������ - ���� ���������
			// ���������� ����� ���������
			undo->return_castling_Q = p_bitboard->castling_Q;
			undo->return_castling_K = p_bitboard->castling_K;
			p_bitboard->castling_Q = 0;
			p_bitboard->castling_K = 0;

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 ��������� 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:

			//������� ������ �� ������ �����
			// ��� �������
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = p_bitboard->white_name_from_coord[i_p];
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ��� ������
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[7];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] ^ move_masks_m[7];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[5] = p_bitboard->white_name_from_coord[7];
			p_bitboard->white_name_from_coord[7] = 0;

			//������ ������ �� ����� �����
			// ��� �������
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ��� ������
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[5];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] ^ move_masks_m[5];

			// ������ - ���� ���������
			// ���������� ����� ���������
			undo->return_castling_Q = p_bitboard->castling_Q;
			undo->return_castling_K = p_bitboard->castling_K;
			p_bitboard->castling_Q = 0;
			p_bitboard->castling_K = 0;

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//15 ����������� � �����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 5;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::QUEEN - EVALUATE::PAWN;
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//14 ����������� � �����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 4;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::ROOK - EVALUATE::PAWN;	
#endif//#if MATERIAL
			

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//13 ����������� � �����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:

			//������� ������ �� ������ ����� 
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 3;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� +����� � -������
#if MATERIAL
				p_bitboard->eval = p_bitboard->eval + EVALUATE::BISHOP - EVALUATE::PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//12 ����������� � ����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ �����
			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ��������� ��� ������ � ����� ���������� � �������� ��� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 2;
			p_bitboard->white_name_from_coord[i_p] = 0;

			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::KNIGHT - EVALUATE::PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// ������ ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
		break;

		//25 ������ � ������������ � �����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������ � ������������ ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::QUEEN - EVALUATE::PAWN + EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			p_bitboard->black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 5;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������� ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == 4)
			{
				if (f_p == 56)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == 63)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//24 ������ � ������������ � �����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������ � ������������ ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::ROOK - EVALUATE::PAWN + EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			p_bitboard->black_name_from_coord[f_p] = 0;
			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 4;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];
			// ������ ������� ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == 4)
			{
				if (f_p == 56)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == 63)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//23 ������ � ������������ �����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];


			// ������ � ������������ ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::BISHOP - EVALUATE::PAWN + EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL

			// �������� ������ ������
			p_bitboard->black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 3;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������� ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == 4)
			{
				if (f_p == 56)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == 63)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//22 ������ � ������������  � ����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// ������� ������ �� ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������ �� ����� ���� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ����� ���� ������� ������ ������ �� �������� ������ �����
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];


			// ������ � ������������ ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::KNIGHT - EVALUATE::PAWN + EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]];
#endif//#if MATERIAL


			// �������� ������ ������
			p_bitboard->black_name_from_coord[f_p] = 0;

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[f_p] = 2;
			p_bitboard->white_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			// ������ ������� ������ � ����� ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������ ������� ������ � ������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];  

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f  == 4)
			{
				if (f_p == 56)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (f_p == 63)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//��� �� ���������
   
	}

}

//	===================================================================
// ��������� ���� ������ ��� ������
void MOVES::public_onward_moves_black
(
	const unsigned __int8 & i,//����� ���� �� ������
	struct T_undo undo[],//���������� ��� ������ ����
	const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
	struct T_Bitboard * const p_bitboard// ������������� �������
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


	// ���������� ���� ������ �� �������
	undo->return_taking_passage= p_bitboard->taking_passage_yes;

	// ������ ����������(����� ��� ���-�������)
	undo->taking_passage_x = p_bitboard->taking_passage_x;
	undo->taking_passage_y = p_bitboard->taking_passage_y;


	// ��������� ������ �� ������� 
	p_bitboard->taking_passage_yes = 0;

	// ������ ������ ������ ������ ��� �����
	p_bitboard->colour_move = 1;

	// ������ ���������� ����� ��������� (����� ��� ���-�������)
	undo->return_castling_K = p_bitboard->castling_K;
	undo->return_castling_Q = p_bitboard->castling_Q;
	undo->return_castling_k = p_bitboard->castling_k;
	undo->return_castling_q = p_bitboard->castling_q;


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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{

		//1 ��� �� ������ ����
		case BITBOARDGENERATOR::SIMPLE_MOVE:

			//������� ������ �� ������ ����� 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ����� - ���� ������ �� ������� 
			if (p_bitboard->black_name_from_coord[f_p] ==1)
			{
				if (f_p - i_p == -16)
				{
					//cout << "i_p= "<< i_p<<"\n";
					//cout << "f_p= "<< f_p<<"\n";
					//cout << "p_bitboard->black_name_from_coord[f_p]= "<< p_bitboard->black_name_from_coord[f_p]<<"\n";
					undo->taking_passage_x = p_bitboard->taking_passage_x;
					undo->taking_passage_y = p_bitboard->taking_passage_y;
					p_bitboard->taking_passage_yes =1;
					p_bitboard->taking_passage_x =(i_p - 8) % 8;
					p_bitboard->taking_passage_y =(i_p - 8) / 8;
				}
			
			}// ������ - ���� ���������
			else if (p_bitboard->black_name_from_coord[f_p] == 6)
			{
				undo->return_castling_k = p_bitboard->castling_k;
				undo->return_castling_q = p_bitboard->castling_q;
				p_bitboard->castling_k = 0;
				p_bitboard->castling_q = 0;

    
			}// ����� - ���� ���������   
			else if (p_bitboard->black_name_from_coord[f_p] == 4)
			{	
				if (i_p == 56)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (i_p == 63)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
			}

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 ������
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			//������� ������ �� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������ �������
			// �� ������ �� �������� �������� ������ ������
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL


			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������ - ���� ���������
			if (p_bitboard->black_name_from_coord[f_p] == 6)
			{
				undo->return_castling_k = p_bitboard->castling_k;
				undo->return_castling_q = p_bitboard->castling_q;
				p_bitboard->castling_k = 0;
				p_bitboard->castling_q = 0;

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (p_bitboard->black_name_from_coord[f_p] == 4)
			{	
				if (i_p == 56)
				{
					undo->return_castling_q = p_bitboard->castling_q;
					p_bitboard->castling_q = 0;
				}

				if (i_p == 63)
				{
					undo->return_castling_k = p_bitboard->castling_k;
					p_bitboard->castling_k = 0;
				}
     
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 0)
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == 7)
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		// 5 ������ �� �������
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			//������� ������ �� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];
  
			// ������ �������
			// �� ������ �� �������� �������� ������ ������
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::PAWN;	
#endif//#if MATERIAL
			
			p_bitboard->white_name_from_coord[(f_p + 8)] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//3 ��������� 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			//������� ������ �� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[56];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] ^ move_masks_m[56];
			p_bitboard->black_name_from_coord[59] = p_bitboard->black_name_from_coord[56];
			p_bitboard->black_name_from_coord[56] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[59];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] ^ move_masks_m[59];


			// ������ - ���� ��������� 
			undo->return_castling_q = p_bitboard->castling_q;
			undo->return_castling_k = p_bitboard->castling_k;
			p_bitboard->castling_q = 0;
			p_bitboard->castling_k = 0; 

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 ��������� 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:

			//������� ������ �� ������ ����� 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = p_bitboard->black_name_from_coord[i_p];
			p_bitboard->black_name_from_coord[i_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[63];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] ^ move_masks_m[63];
			p_bitboard->black_name_from_coord[61] = p_bitboard->black_name_from_coord[63];
			p_bitboard->black_name_from_coord[63] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[61];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] ^ move_masks_m[61];


			// ������ - ���� ���������
			undo->return_castling_q = p_bitboard->castling_q;
			undo->return_castling_k = p_bitboard->castling_k;
			p_bitboard->castling_q = 0;
			p_bitboard->castling_k = 0;

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//15 ����������� � �����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:

			//������� ������ �� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = 5;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::QUEEN + EVALUATE::PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//14 ����������� � �����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:

			//������� ������ �� ������ ����� 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = 4;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
			#if MATERIAL
				p_bitboard->eval = p_bitboard->eval - EVALUATE::ROOK + EVALUATE::PAWN;	
			#endif//#if MATERIAL

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//13 ����������� � �����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:

			//������� ������ �� ������ �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = 3;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::BISHOP + EVALUATE::PAWN;	
#endif//#if MATERIAL


			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//12 ����������� � ����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ ����� 
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->black_name_from_coord[f_p] = 2;
			p_bitboard->black_name_from_coord[i_p] = 0;

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::KNIGHT + EVALUATE::PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ������������ ����� ���� �����
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//25 ������ � ������������ � �����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::QUEEN + EVALUATE::PAWN - EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL


			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = 5;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 0)
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == 7)
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}	
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//24 ������ � ������������ � �����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::ROOK + EVALUATE::PAWN - EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL

			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = 4;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 0)
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == 7)
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//23 ������ � ������������ �����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::BISHOP + EVALUATE::PAWN - EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL

			p_bitboard->white_name_from_coord[f_p] = 0;

			p_bitboard->black_name_from_coord[f_p] = 3;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 0)
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == 7)
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//22 ������ � ������������  � ����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ����������� �������
			// �� ������ �� �������� �������� ������ ����������� � ���������� �����
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::KNIGHT + EVALUATE::PAWN - EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]];
#endif//#if MATERIAL

			p_bitboard->white_name_from_coord[f_p] = 0;
			p_bitboard->black_name_from_coord[f_p] = 2;
			p_bitboard->black_name_from_coord[i_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 0)
				{
					undo->return_castling_Q = p_bitboard->castling_Q;
					p_bitboard->castling_Q = 0;
				}

				if (f_p == 7)
				{
					undo->return_castling_K = p_bitboard->castling_K;
					p_bitboard->castling_K = 0;
				}
			}

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//��� �� ���������
   
	}

}

//	===================================================================
// ��� ����� ��� �����
void MOVES::public_backs_moves_white
(
	const unsigned __int8 & i,//����� ���� �� ������
	const struct T_undo undo[],//���������� ��� ������ ����
	const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
	struct T_Bitboard * const p_bitboard// ������������� �������
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


	// ��������������� ���� ������ �� �������
	p_bitboard->taking_passage_yes=undo->return_taking_passage;

	// ����� ������� ���� ��� ������ ����� �� ��� ����
	p_bitboard->colour_move = 1;

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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{
		//1 ��� �� ������ ����
		case BITBOARDGENERATOR::SIMPLE_MOVE:
 
			// ����� - ���� ������ �� ������� 
			if (p_bitboard->white_name_from_coord[f_p] == 1)
			{
				if (f_p - i_p == 16)
				{
					p_bitboard->taking_passage_yes =undo->return_taking_passage;
					p_bitboard->taking_passage_x = undo->taking_passage_x;
					p_bitboard->taking_passage_y = undo->taking_passage_y;
				}

				
			}// ������ - ���� ��������� 
			else if (p_bitboard->white_name_from_coord[f_p] == 6)
			{
				// ���������� ����� ���������
				p_bitboard->castling_K = undo->return_castling_K;
				p_bitboard->castling_Q = undo->return_castling_Q;   

	    
			}// ����� - ���� ���������   
			else if (p_bitboard->white_name_from_coord[f_p] == 4)
			{ 
				if (i_p == 0)
				{
					p_bitboard->castling_Q = undo->return_castling_Q;
				}

				if (i_p == 7)
				{
					p_bitboard->castling_K = undo->return_castling_K;
				}
			}

			//������� ������ �� ������ �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 ������
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			// ������ - ���� ���������
			if (p_bitboard->white_name_from_coord[f_p] == 6)
			{
				// ���������� ����� ���������
				p_bitboard->castling_K = undo->return_castling_K;
				p_bitboard->castling_Q = undo->return_castling_Q;   

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (p_bitboard->white_name_from_coord[f_p] == 4)
			{	
				if (i_p == 0)
				{
					p_bitboard->castling_Q = undo->return_castling_Q;
				}

				if (i_p == 7)
				{
					p_bitboard->castling_K = undo->return_castling_K;
				}
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 56)
					p_bitboard->castling_q = undo->return_castling_q;

				if (f_p == 63)
					p_bitboard->castling_k = undo->return_castling_k;
			}

			//������� ������ �� ������ ����� 
			// ������� ������ �� ������ ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// �������������� ��� ������ ������
			p_bitboard->black_name_from_coord[f_p] = t_f;

			// ������ ������
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::kod[t_f];
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// � ����� ��� ������ ������
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ������� ������ ������ ������
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//5 ������ �� �������
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			//������� ������ �� ������ �����
			// ������� ������ �� ������ ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// �������������� ��� ������ ������
			p_bitboard->black_name_from_coord[(f_p - 8)] = t_f;

			// ������ ������
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// � ����� ��� ������ ������
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[(f_p - 8)];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ������� ������ ������ ������
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[(f_p - 8)]] ^ move_masks_m[(f_p - 8)];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
		break;

		//3 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			// ������ - ���� ���������
			p_bitboard->castling_Q = undo->return_castling_Q;
			p_bitboard->castling_K = undo->return_castling_K;   

			//������� ������ �� ������ �����
			// ������
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[3];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[3]] ^ move_masks_m[3];

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[0] = p_bitboard->white_name_from_coord[3];
			p_bitboard->white_name_from_coord[3] = 0;

			//������ ������ �� ����� �����
			// ������
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[0];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[0]] ^ move_masks_m[0];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:

			// ������ - ���� ���������
			p_bitboard->castling_Q = undo->return_castling_Q;
			p_bitboard->castling_K = undo->return_castling_K;   

			//������� ������ �� ������ �����
			// ������
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[i_p] = p_bitboard->white_name_from_coord[f_p];
			p_bitboard->white_name_from_coord[f_p] = 0;

			// �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[5];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[5]] ^ move_masks_m[5];

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[7] = p_bitboard->white_name_from_coord[5];
			p_bitboard->white_name_from_coord[5] = 0;

			//������ ������ �� ����� �����
			// ������
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[7];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[7]] ^ move_masks_m[7];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//������ 15-����� 14-����� 13-����� 12-����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			//������� ������ �� ������ �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]] + EVALUATE::PAWN;
#endif//#if MATERIAL

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[i_p] = 1;
			p_bitboard->white_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			// ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		// ������ ������ � ������������ 25-����� 24-����� 23-����� 22-����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			//  ���������� ���� ������� ����� 
			if (t_f == 4)
			{
				if (f_p == 56)
					p_bitboard->castling_q = undo->return_castling_q;

				if(f_p == 63)
					p_bitboard->castling_k = undo->return_castling_k;

			}

			//������� ������ �� ������ �����
			// ������� ������ �� ������ ���� ����� �����
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			// ������� ������ �� �������� ������ �����
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];


			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval - EVALUATE::kod[p_bitboard->white_name_from_coord[f_p]] + EVALUATE::PAWN - EVALUATE::kod[t_f];
#endif//#if MATERIAL
			//------------------------------------------------------------------------------

			// ��������� ��� ������ � �������� �� ������ ����������
			p_bitboard->white_name_from_coord[i_p] = 1;
			p_bitboard->white_name_from_coord[f_p] = 0;

			// ��������������� ��� ������ ������
			p_bitboard->black_name_from_coord[f_p] = t_f;

			//������ ������ �� ����� �����
			// � ����� ��� ����� ������
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[i_p];

			// � ����� ��� ������ ������
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];

			// ������� ������ ������
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[i_p]] ^ move_masks_m[i_p];

			// ������� ������ ������ ������
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//��� �� ���������
	}
}


//	===================================================================
// ��� ����� ��� ������
void MOVES::public_backs_moves_black
(
	const unsigned __int8 & i,//����� ���� �� ������
	const struct T_undo undo[],//���������� ��� ������ ����
	const struct T_list_surplus_moves * const list_surplus_moves,//������ ��������� �����
	struct T_Bitboard * const p_bitboard// ������������� �������
)
{

	unsigned __int8 i_p = ((list_surplus_moves->move[i]>>12)& 63);
	unsigned __int8 f_p = ((list_surplus_moves->move[i]>>18)& 63);
	unsigned __int8 t_f = ( list_surplus_moves->move[i] & 63);


	// �������������� ���� ������ �� �������
	p_bitboard->taking_passage_yes=undo->return_taking_passage;

	// ������ ������� ���� ��� ������ ����� �� ��� ����
	p_bitboard->colour_move = 0;

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

	switch (((list_surplus_moves->move[i]>>6)& 63))
	{
		//1 ��� �� ������ ����
		case BITBOARDGENERATOR::SIMPLE_MOVE:

			// ����� - ���� ������ �� ������� 
			if (p_bitboard->black_name_from_coord[f_p] ==1)
			{
				if (f_p - i_p == -16)
				{
					p_bitboard->taking_passage_yes =undo->return_taking_passage;
					p_bitboard->taking_passage_x = undo->taking_passage_x;
					p_bitboard->taking_passage_y = undo->taking_passage_y;
				}

			}// ������ - ���� ��������� 
			else if (p_bitboard->black_name_from_coord[f_p] == 6)
			{
				p_bitboard->castling_k = undo->return_castling_k;
				p_bitboard->castling_q = undo->return_castling_q;

			}// ����� - ���� ���������  
			else if (p_bitboard->black_name_from_coord[f_p] == 4)
			{	
				if (i_p == 56)
					p_bitboard->castling_q = undo->return_castling_q;

				if(i_p == 63)
					p_bitboard->castling_k =  undo->return_castling_k;

			}

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//2 ������
		case BITBOARDGENERATOR::CAPTURE_MOVE:

			// ������ - ���� ���������
			if (p_bitboard->black_name_from_coord[f_p] == 6)
			{
				p_bitboard->castling_k = undo->return_castling_k;
				p_bitboard->castling_q = undo->return_castling_q;

			}// ������� � �� ������ �� �� ������ � ���� �� �� ��������� � ��� �������� 
			else if (p_bitboard->black_name_from_coord[f_p] == 4)
			{	
				if (i_p == 56)
					p_bitboard->castling_q = undo->return_castling_q;

				if(i_p == 63)
					p_bitboard->castling_k = undo->return_castling_k;
     
			}

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 0)
					p_bitboard->castling_Q = undo->return_castling_Q;

				if (f_p == 7)
					p_bitboard->castling_K = undo->return_castling_K;

			}

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->white_name_from_coord[f_p] = t_f;

			// ������ ������
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::kod[t_f];
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];

			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//5 ������ �� �������
		case BITBOARDGENERATOR::TAKING_PASSAGE:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->white_name_from_coord[(f_p + 8)] = t_f;

			// ������ ������
			// � ������ �� ���������� �������� ������ ������ 
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::PAWN;	
#endif//#if MATERIAL

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[(f_p + 8)];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[(f_p + 8)]] ^ move_masks_m[(f_p + 8)];

			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;
   
		break;

		//3 0-0-0
		case BITBOARDGENERATOR::LONG_CASTLING:

			// ������ - ���� ���������
			p_bitboard->castling_q = undo->return_castling_q;
			p_bitboard->castling_k = undo->return_castling_k;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[59];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[59]] ^ move_masks_m[59];
			p_bitboard->black_name_from_coord[56] = p_bitboard->black_name_from_coord[59];
			p_bitboard->black_name_from_coord[59] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[56];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[56]] ^ move_masks_m[56];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//4 0-0
		case BITBOARDGENERATOR::SHORT_CASTLING:
 
			// ������ - ���� ���������
			p_bitboard->castling_q = undo->return_castling_q;
			p_bitboard->castling_k = undo->return_castling_k;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
			p_bitboard->black_name_from_coord[i_p] = p_bitboard->black_name_from_coord[f_p];
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[61];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[61]] ^ move_masks_m[61];
			p_bitboard->black_name_from_coord[63] = p_bitboard->black_name_from_coord[61];
			p_bitboard->black_name_from_coord[61] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[63];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[63]] ^ move_masks_m[63];

			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		// ������ ������ 15-������ 14-����� 13-����� 12-����
		case BITBOARDGENERATOR::TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::TRANSFORMATION_IN_KNIGHT:

			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];
		  
			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]] - EVALUATE::PAWN;
#endif//#if MATERIAL
			
			p_bitboard->black_name_from_coord[i_p] = 1;
			p_bitboard->black_name_from_coord[f_p] = 0;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		//������ ������ � ������������ 25-������ 24-����� 23-����� 22-����
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_QUEEN:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_ROOK:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_BISHOP:
		case BITBOARDGENERATOR::CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT:

			// ���� ������� ����� �� ��������� ��������� � �� ��������
			if (t_f == 4)
			{
				if (f_p == 0)
					p_bitboard->castling_Q = undo->return_castling_Q;

				if (f_p == 7)
					p_bitboard->castling_K = undo->return_castling_K;

			}
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[f_p];
			p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[f_p]] ^ move_masks_m[f_p];

			// ����������� ������
			// � ������ �� ���������� �������� ������ ����������� � �������� �����
			// ������ ��� � ������ ������ ������� + ����� � - ������
			// (�������� ���)
#if MATERIAL
			p_bitboard->eval = p_bitboard->eval + EVALUATE::kod[p_bitboard->black_name_from_coord[f_p]] - EVALUATE::PAWN + EVALUATE::kod[t_f];
#endif//#if MATERIAL

			p_bitboard->black_name_from_coord[i_p] = 1;
			p_bitboard->black_name_from_coord[f_p] = 0;

			p_bitboard->white_name_from_coord[f_p] = t_f;

			//������ ������ �� ����� �����
			p_bitboard->all_black_piece = p_bitboard->all_black_piece ^ move_masks_m[i_p];
			p_bitboard->all_white_piece = p_bitboard->all_white_piece ^ move_masks_m[f_p];

			p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] = p_bitboard->black_piece[p_bitboard->black_name_from_coord[i_p]] ^ move_masks_m[i_p];
			p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] = p_bitboard->white_piece[p_bitboard->white_name_from_coord[f_p]] ^ move_masks_m[f_p];
			
			p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

		break;

		default:;//��� �� ���������
	}
}


