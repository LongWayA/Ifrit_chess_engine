#include <fstream>  // TEST
#include <iostream> // TEST
//#include <stdlib.h>

#include "d_Bitboard_gen.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 21.02.2009
*/
//---------------------------------------------------------------------------

/*
   ������� ����� ������� 99,99999%

   ��������� ��������(bit board) ���������
   ��� ������� ��������� ��� ������� ��� �� ����� � ��� ���������
   ��� ������� ������:
   "Hyatt, Robert. Rotated bitmaps, a new twist on an old idea"
   ����� ��� ������� �������� ���������� � �� ��������� :)

   ������������
   ��������� � ������ �������� �� ����������� �������� ��������
   �� ������ ������(21.02.09) �����(bugs) �� ��������
*/


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_B
std::ofstream Test_Bitboard;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//public

//	===================================================================
// ���������� ������ ������ + ����������� ����� (��� �����)
void BITBOARDGENERATOR::public_generator_captures_white
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����

)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 one_pawn_mask; // ����� ������������ ����� �����
	unsigned __int64 move_conversion_mask; // ����� ����� ����� � ������������
	unsigned __int64 captures_conversion_mask; // ����� ������ ����� � �������������
	unsigned __int64 captures_mask; // ����� ������
	unsigned __int64 blocking_square_mask; // ����� ������������� �����

	unsigned __int8 p; // ������� ����� � ������
	unsigned __int8 from; // ���������� ���� ������ ������ �����
	unsigned __int8 to; // ���������� ���� ���� ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 

	// ���������� ���� ����� �����
	p = 0;

	// ���� �����----------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		//�������� ���� ����� 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);


		if ( from / 8 == 6 )
		{
			// ������� ���� � ������������ � ������ � ������������
			// 1 ������� ������� ���� ����� ����� �� ���� ���� ������
			move_conversion_mask = (one_pawn_mask << 8) & (~(p_bitboard->all_piece));

			// ������ � ������������
			captures_conversion_mask = pawn_capture_white[from] &  p_bitboard->all_black_piece;

			// ������� ���������� ����� � ������������ �����
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// �������� ����������� �����
				// ����������� � ����� 15,����������� � ����� 14,����������� � ����� 13,����������� � ���� 12
				list_record_special(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0,p_list_surplus_moves);
				list_record_special(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0,p_list_surplus_moves);
				list_record_special(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0,p_list_surplus_moves);
				list_record_special(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0,p_list_surplus_moves);
 
#if KONTROL_B
				KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
			}

			// ������� ���������� ������ � ������������ �����  
			while (captures_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(captures_conversion_mask);
				captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

				// �������� ����������� ����� � �������
				// ����������� � ����� 25,����������� � ����� 24,����������� � ����� 23,����������� � ���� 22
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,p_bitboard->black_name_from_coord[to],p_list_surplus_moves);

#if KONTROL_B
				KONTROL_captures_W(p_bitboard,&from,&to,&p,120);
#endif//KONTROL_B
			}

		}
		else
		{
			// ������� ������ ������
			captures_mask = pawn_capture_white[from] &  p_bitboard->all_black_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_white(N_PAWN,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 ������� ������ �� �������
		if (p_bitboard->taking_passage_yes == 1)
		{
			if ( from / 8 == 4 )
			{
				to = 8 * p_bitboard->taking_passage_y + p_bitboard->taking_passage_x;

				if (((from - to) == -9) && (from % 8 != 7))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->black_name_from_coord[to - 8],p_list_surplus_moves);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(p_bitboard,&from,&to,&p,130);
#endif//KONTROL_B
				}

				if (((from - to) == -7) && (from % 8 != 0))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->black_name_from_coord[to - 8],p_list_surplus_moves);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(p_bitboard,&from,&to,&p,130);
#endif//KONTROL_B
				}
			}
		}
	}
	// �����----------------------------------------------------------------------------------


	// ���� ���� ----------------------------------------------------------------------------------
	// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
	// ������������ ����� ������� ��� ��� ������� �� ��� ����������
	from_mask = p_bitboard->white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(N_KNIGHT,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// ���� ----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------
	}
	// �����---------------------------------------------------------------------------------- 


	// ���� �����---------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� �����------------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];
		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������-----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------
	}
	// �����----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------
	// ����� ������������ ������ �����
	from_mask = p_bitboard->white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);

		//������� � ������� ��������� ������������ (�� ���� �����)

		//��� �����------------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) ������� ���������� ������� �������������� ����
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);

			// 3) ������� ����� ������
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			// 4) ��������� ������ �����
			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����--------------------------------------------------------------------------------------


	// ���� ������--------------------------------------------------------------------------------
	// ����� ������������ ������ ������
	from_mask = p_bitboard->white_piece[N_KING];

	if (from_mask != 0)
	{
		// ������� ���������� ������(����� ������(��� ������ ����������� ������ ) ������������� ���)
		from = bit_scan_forward_win32(from_mask);

		// ������� ����� ������ ������
		captures_mask = (p_bitboard->all_black_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			// ��������� ������ �����
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(N_KING,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
			captures_mask = captures_mask & (captures_mask - 1);

		}

	}
	// ���� ������--------------------------------------------------------------------------------

	p_list_surplus_moves->end_list = p;
	p_list_surplus_moves->end_captures = p;

}

//	===================================================================
// ���������� ������ ������  + ����������� ����� (��� ������)
void BITBOARDGENERATOR::public_generator_captures_black
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 one_pawn_mask; // ����� ������������ ����� �����
	unsigned __int64 move_conversion_mask; // ����� ����� ����� � ������������
	unsigned __int64 captures_conversion_mask; // ����� ������ ����� � �������������
	unsigned __int64 captures_mask; // ����� ������
	unsigned __int64 blocking_square_mask; // ����� ������������� �����

	unsigned __int8 p; // ������� ����� � ������
	unsigned __int8 from; // ���������� ���� ������ ������ �����
	unsigned __int8 to; // ���������� ���� ���� ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 

	// ���������� ���� ������ �����
	p = 0;

	// ���� �����----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// �������� ���� ����� 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		if ( from / 8 == 1 )
		{
			// ������� ���� � ������������ � ������ � ������������
			// 1 ������� ������� ���� ������ ����� �� ���� ���� ������
			move_conversion_mask = (one_pawn_mask >> 8) & (~(p_bitboard->all_piece));

			// ������ � ������������
			captures_conversion_mask = pawn_capture_black[from] &  p_bitboard->all_white_piece;

			// ������� ���������� ����� � ������������ �����
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// �������� ����������� �����
				// ����������� � ����� 15,����������� � ����� 14,����������� � ����� 13,
				// ����������� � ���� 12
				list_record_special(N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0,p_list_surplus_moves);
				list_record_special(N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0,p_list_surplus_moves);
				list_record_special(N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0,p_list_surplus_moves);
				list_record_special(N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0,p_list_surplus_moves);
 
#if KONTROL_B
				KONTROL_move(&from,&to,&p);
#endif//KONTROL_B
			}

			// ������� ���������� ������ � ������������ �����  
			// ������� ���������� ������ � ������������ �����  
			while (captures_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(captures_conversion_mask);
				captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

				// �������� ����������� ����� � �������
				// ����������� � ����� 25,����������� � ����� 24,����������� � ����� 23,
				// ����������� � ���� 22
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);
				list_record_special(N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,p_bitboard->white_name_from_coord[to],p_list_surplus_moves);

#if KONTROL_B
				KONTROL_captures_B(p_bitboard,&from,&to,&p,120);
#endif//KONTROL_B
			}
		}
		else
		{
			// ������� ������ ������
			captures_mask =  pawn_capture_black[from] &  p_bitboard->all_white_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_black(N_PAWN,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 ������� ������ �� �������
		if (p_bitboard->taking_passage_yes == 1)
		{
			if ( from / 8 == 3 )
			{
				to = 8 * p_bitboard->taking_passage_y + p_bitboard->taking_passage_x;

				if (((from - to) == 9) && (from % 8 != 0))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->white_name_from_coord[to + 8],p_list_surplus_moves);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(p_bitboard,&from,&to,&p,130);
#endif//KONTROL_B
				}

				if (((from - to) == 7) && (from % 8 != 7))
				{
					list_record_special(N_PAWN,TAKING_PASSAGE,p,from,to,p_bitboard->white_name_from_coord[to + 8],p_list_surplus_moves);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(p_bitboard,&from,&to,&p,130);
#endif//KONTROL_B
				}
			}
		}
	}
	// �����----------------------------------------------------------------------------------


	// ���� ���� ----------------------------------------------------------------------------------
	// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
	// ������������ ����� ������� ��� ��� ������� �� ��� ����������
	// ���������� ����
	from_mask = p_bitboard->black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
		   to = bit_scan_forward_win32(captures_mask);
		   list_capture_black(N_KNIGHT,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		   captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// ���� ----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������
		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����---------------------------------------------------------------------------------- 


	// ���� �����-----------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� �����------------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������
		// ��� ����� - ������---------------------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,p_bitboard,p_list_surplus_moves);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� ������---------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		captures_mask = (p_bitboard->all_white_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_black(N_KING,CAPTURE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// ������---------------------------------------------------------------------------------


	p_list_surplus_moves->end_list = p;
	p_list_surplus_moves->end_captures = p;

}

//	===================================================================
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ + ����������� �����)
//(��� �����)
void BITBOARDGENERATOR::public_generator_move_white
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����     
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 move_mask; // ����� ����� ����� ������ ��� ����� ����� � ������������
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 one_pawn_mask; // ����� ������������ ����� �����
	unsigned __int64 move_mask_d; // ����� ���� ����� �� ��� ����

	unsigned __int8 p; // ������� ����� � ������
	unsigned __int8 from; // ���������� ���� ������ ������ �����
	unsigned __int8 to; // ���������� ���� ���� ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 

	p = p_list_surplus_moves->end_list;

	// ���������� ���� ����� �����
	// ���� ������--------------------------------------------------------------------------------
	// ����� ������������ ������ ������
	from_mask = p_bitboard->white_piece[N_KING];

	if (from_mask != 0)
	{
		// ������� ���������� ������(����� ������(��� ������ ����������� ������ ) 
		// ������������� ���)
		from = bit_scan_forward_win32(from_mask);

		// ������� ����� ����� ������(��� ������)
		move_mask = king_moves_masks[from] & (~p_bitboard->all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KING,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}

		// ��������� ������ ������

		// ���� ������� ��������� ���������
		if (p_bitboard->castling_Q)
		{
			// 
			to = 2;
#if KONTROL_B
			KONTROL_move(&from,&to,&p);
#endif//KONTROL_B

			// ����  1 2 3 ������  �� ��������� ���
			if (((~p_bitboard->all_piece) & CASTLING_Q ) == CASTLING_Q)
			{
				// ������� ��������� �����  0-0-0 ��������� ��� ���� �� ���� 2 3 4 
				if ((white_check_square(2,p_bitboard) == 0) 
				&& (white_check_square(3,p_bitboard) == 0) 
				&& (white_check_square(4,p_bitboard) == 0))
				{
					list_record_special(N_KING,LONG_CASTLING,p,from,2,0,p_list_surplus_moves);
				}
			}
		}

		// ���� �������� ��������� ��������� �� ��������� ��� 
		if (p_bitboard->castling_K)
		{
			to = 6;
#if KONTROL_B
			KONTROL_move(&from,&to,&p);
#endif//KONTROL_B

			//����  5 6 ������  �� ��������� ���
			if (((~p_bitboard->all_piece) & CASTLING_K ) == CASTLING_K)
			{
				// �������� ��������� ����� 0-0 ��������� ��� ���� �� ����  4 5 6
				if ((white_check_square(4,p_bitboard) == 0)
				&& (white_check_square(5,p_bitboard) == 0)
				&& (white_check_square(6,p_bitboard) == 0))
				{
					list_record_special(N_KING,SHORT_CASTLING,p,from,6,0,p_list_surplus_moves);
				}
			}
		}
	}
	// ���� ������--------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	// ����� ������������ ������ �����
	from_mask = p_bitboard->white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// ���� �������� ��� ����������� ���� b = b & (b-1)  
		// ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) ������� ���������� ������� �������������� ����
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------


		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) ������� ���������� ������� �������������� ����
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------
	}
	// �����----------------------------------------------------------------------------------



	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------
	}
	// �����---------------------------------------------------------------------------------- 


	// ���� ���� ----------------------------------------------------------------------------------
	// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
	// ������������ ����� ������� ��� ��� ������� �� ��� ����������
	from_mask = p_bitboard->white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		move_mask = knigt_moves_masks[from] & (~p_bitboard->all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
	}
	// ���� ----------------------------------------------------------------------------------

	// ���� �����----------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//�������� ���� ����� 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 ������� ������� ���� ����� ����� �� ���� ���� ������
		move_mask = (one_pawn_mask << 8) & (~(p_bitboard->all_piece));

		// 2 ������� ���� ����� �� 2 ���� ������ 
		// � ������ ��������� ������ ����� ������� �� 2 �����������
		move_mask_d = one_pawn_mask & LINE_1;

		// �������� ����� ���� ��� ��� �� ��������������
		move_mask_d = (move_mask_d << 8) & (~(p_bitboard->all_piece));

		// ����������� ������� ������ ���
		move_mask_d = (move_mask_d << 8) & (~(p_bitboard->all_piece));

		// ����� ��� ���� ����� ��� �� ���� ��� �� ��� ���� ������
		move_mask = (move_mask | move_mask_d) ;

		// ���� ��� �����������
		move_mask = move_mask & ~LINE_7;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
	}
	// �����----------------------------------------------------------------------------------

	p_list_surplus_moves->end_list = p;

}

//	===================================================================
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ + ����������� �����)(��� ������)
void BITBOARDGENERATOR::public_generator_move_black
(
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 move_mask; // ����� ����� ����� ������ ��� ����� ����� � ������������
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 one_pawn_mask; // ����� ������������ ����� �����
	unsigned __int64 move_mask_d; // ����� ���� ����� �� ��� ����

	unsigned __int8 p; // ������� ����� � ������
	unsigned __int8 from; // ���������� ���� ������ ������ �����
	unsigned __int8 to; // ���������� ���� ���� ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 

	p = p_list_surplus_moves->end_list;

	// ���������� ���� ������ �����
	// ���� ������---------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		move_mask = king_moves_masks[from] & (~(p_bitboard->all_piece));

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KING,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}

		// ��������� ������� ������

		// ���� ������� ��������� ���������
		if (p_bitboard->castling_q)
		{
			to = 58;
#if KONTROL_B
			KONTROL_move(&from,&to,&p);
#endif//KONTROL_B

			// � ����  1 2 3 ������  �� ��������� ���
			if ((~(p_bitboard->all_piece) & CASTLING_q )== CASTLING_q)
			{
				// ������� ��������� ������ ��������� 0-0-0 58 59 60
				if ((black_check_square(58,p_bitboard) == 0)
				&& (black_check_square(59,p_bitboard) == 0)
				&& (black_check_square(60,p_bitboard) == 0))
				{
					list_record_special(N_KING,LONG_CASTLING,p,from,58,0,p_list_surplus_moves);			 
				}
			}
		}

		// ���� �������� ��������� ��������� �� ��������� ��� 
		if (p_bitboard->castling_k)
		{
			to = 62;
#if KONTROL_B
			KONTROL_move(&from,&to,&p);
#endif//KONTROL_B

			// � ����  5 6 ������  �� ��������� ���
			if ((~(p_bitboard->all_piece) & CASTLING_k )== CASTLING_k)
			{
				// �������� ��������� ������ ��������� 0-0 60 61 62
				if ((black_check_square(60,p_bitboard) == 0)
				&& (black_check_square(61,p_bitboard) == 0)
				&&(black_check_square(62,p_bitboard) == 0))
				{
					list_record_special(N_KING,SHORT_CASTLING,p,from,62,0,p_list_surplus_moves);
				}
			}
		}
	}
	// ������---------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	// ����� ������������ �����
	from_mask = p_bitboard->black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� 
		//����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) ������� ���������� ������� �������������� ����
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------


		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������ (�� ���� �����)
		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_QUEEN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = p_bitboard->all_piece & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			//   2) ������� ���������� ������� �������������� ����
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_up_masks[from] ^ rook_up_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_up_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = p_bitboard->all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_left_masks[from] ^ rook_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

		if(blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = rook_down_masks[from] ^ rook_down_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_down_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = rook_right_masks[from] ^ rook_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = rook_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_ROOK,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------
	}
	// �����----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_right_masks[from] ^ bishop_up_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			move_mask = bishop_up_left_masks[from] ^ bishop_up_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_up_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_left_masks[from] ^ bishop_down_left_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_left_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

		if(blocking_square_mask != 0){ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			move_mask = bishop_down_right_masks[from] ^ bishop_down_right_masks[blocking_coordinate] ^ move_masks[blocking_coordinate];
		}
		else
		{
			move_mask = bishop_down_right_masks[from];
		}

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_BISHOP,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
		//---------------------------------------------------------------------------
	}
	// �����---------------------------------------------------------------------------------- 


	// ���� ���� ----------------------------------------------------------------------------------
	// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
	// ������������ ����� ������� ��� ��� ������� �� ��� ����������
	from_mask = p_bitboard->black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		move_mask = knigt_moves_masks[from] & (~p_bitboard->all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_KNIGHT,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
	}
	// ���� ----------------------------------------------------------------------------------

	// ���� �����----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 ������� ������� ���� ������ ����� �� ���� ���� ������
		move_mask = (one_pawn_mask >> 8) & (~(p_bitboard->all_piece));

		// 2 ������� ���� ����� �� 2 ���� ������ 
		// � ������ ��������� ������ ����� ������� �� 7 �����������
		move_mask_d = one_pawn_mask & LINE_6;

		// �������� ����� ���� ��� ��� �� ��������������
		move_mask_d = (move_mask_d >> 8) & (~(p_bitboard->all_piece));

		// ����������� ������� ������ ���
		move_mask_d = (move_mask_d >> 8) & (~(p_bitboard->all_piece));

		// ����� ��� ���� ����� ��� �� ���� ��� �� ��� ���� ������
		move_mask = (move_mask | move_mask_d) ;

		// ���� ��� �����������
		move_mask = move_mask & ~LINE_0;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move(N_PAWN,SIMPLE_MOVE,from,to,p,p_bitboard,p_list_surplus_moves);
		}
	}
	// �����----------------------------------------------------------------------------------

	p_list_surplus_moves->end_list = p;

}

//	===================================================================
// ��� ���� �� ������ ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
unsigned __int8 BITBOARDGENERATOR::public_king_white_check
(
	const struct T_Bitboard * const p_bitboard// ������������� �����
)
{

	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������


	unsigned __int8 from; // ���������� ���� ������ ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ����� ������������ ������ ������
	from_mask = p_bitboard->white_piece[6];

	// ������� ���������� ������(����� ������(��� ������ ����������� ������ ) ������������� ���)
	from = bit_scan_forward_win32(from_mask);

	// ��� �����------------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����------------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������-----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ������� � ������� ��������� ������������

	// ��� ����� - ������--------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����� - �����---------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - �����----------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - ������---------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����--------------------------------------------------------------
	captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[from]);

	while (captures_mask != 0 )
	{
		if (p_bitboard->black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------


	// �� ���� ������--------------------------------------------------------------------
	captures_mask = (p_bitboard->all_black_piece & king_moves_masks[from]);
	while (captures_mask != 0)
	{
		if (p_bitboard->black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

// ����� ������� ��������----------------------------------------------------
	if ((from / 7) < 8)
	{
		if (from % 8 != 0)
		{
			if (p_bitboard->black_name_from_coord[from + 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}

		if (from % 8 != 7)
		{
			if (p_bitboard->black_name_from_coord[from + 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}

	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// ��� ���� �� ������ ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
unsigned __int8 BITBOARDGENERATOR::public_king_black_check
(
	const struct T_Bitboard * const p_bitboard// ������������� �����
)
{

	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������


	unsigned __int8 from; // ���������� ���� ������ ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	from_mask = p_bitboard->black_piece[6];
	from = bit_scan_forward_win32(from_mask);
 
	// ������� � ������� ��������� ������������

	// ��� �����------------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����------------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ������� � ������� ��������� ������������
	// ��� ����� - ������--------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����� - �����---------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - �����----------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - ������---------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����--------------------------------------------------------------
	captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[from]);

	while (captures_mask != 0)
	{
		if (p_bitboard->white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		==  N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// �� ���� ������------------------------------------------------------------
	captures_mask = (p_bitboard->all_white_piece & king_moves_masks[from]);

	while (captures_mask != 0)
	{
		if (p_bitboard->white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	//--------------------------------------------------------------------------- 
	if ((from / 7) > 1 )
	{
		// ����� ������� ��������
		if (from % 8 != 7)
		{
			if (p_bitboard->white_name_from_coord[from - 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}
 
		if (from % 8 != 0)
		{
			if (p_bitboard->white_name_from_coord[from - 9] == N_PAWN)
			{
				return N_PAWN;
			}
		}
 
	}
	//---------------------------------------------------------------------------

	return 0;
 
}


//private

//	===================================================================
// ��� ���� �� ����� ���� ���� ���� ��� �� ������� ���������� 0
// ����� ����� �������� ������
inline unsigned __int8 BITBOARDGENERATOR::white_check_square
(
	const unsigned __int8 & sq,//������ ������������ ����(0-63) 
	const struct T_Bitboard * const p_bitboard// ������������� �����
)
{

	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������

	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ��� �����------------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_up_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������-----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ��� ����� - ������--------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_up_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����� - �����---------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_up_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����
	//---------------------------------------------------------------------------
	captures_mask = (p_bitboard->all_black_piece & knigt_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (p_bitboard->black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		==  N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// �� ���� ������
	//---------------------------------------------------------------------------
	captures_mask = (p_bitboard->all_black_piece & king_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (p_bitboard->black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------


	// ����� ������� ��������
	//---------------------------------------------------------------------------
	if (p_bitboard->black_name_from_coord[sq + 7] == N_PAWN)
	{
		return N_PAWN;
	}

	if (p_bitboard->black_name_from_coord[sq + 9] == N_PAWN)
	{
		return N_PAWN;
	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// ��� ���� �� ������ ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
inline unsigned __int8 BITBOARDGENERATOR::black_check_square
(
	const unsigned __int8 & sq,//������ ������������ ����(0-63) 
	const struct T_Bitboard * const p_bitboard// ������������� �����
)
{
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������

	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����------------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_down_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������----------------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & rook_right_masks[sq];

	if(blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ��� ���� - �����----------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_down_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - ������---------------------------------------------------------
	blocking_square_mask = (p_bitboard->all_piece) & bishop_down_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = p_bitboard->all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (p_bitboard->white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (p_bitboard->white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����
	//---------------------------------------------------------------------------
	captures_mask = (p_bitboard->all_white_piece & knigt_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (p_bitboard->white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// �� ���� ������
	//---------------------------------------------------------------------------
	captures_mask = (p_bitboard->all_white_piece & king_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (p_bitboard->white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// ����� ������� ��������
	//---------------------------------------------------------------------------
	if (p_bitboard->white_name_from_coord[sq - 7] == N_PAWN)
	{
		return N_PAWN;
	}

	if (p_bitboard->white_name_from_coord[sq - 9] == N_PAWN)
	{
		return N_PAWN;
	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// ��������� ������������� �����
inline void BITBOARDGENERATOR::bitboard_ini_t
(
	struct T_Bitboard * p_bitboard// ������������� �����
)
{
	p_bitboard->all_piece = 0;
	p_bitboard->all_white_piece = 0;
	p_bitboard->all_black_piece = 0;

	for (unsigned __int8 i = 0; i < 7; i++)
	{
		p_bitboard->white_piece[i]= 0;
		p_bitboard->black_piece[i]= 0;
	}

	// ��������� �� ���������
	for (unsigned __int8 j = 0; j < 64; j++)
	{
		p_bitboard->white_name_from_coord[j]= 0;
		p_bitboard->black_name_from_coord[j]= 0;
	}

}

//	===================================================================
// ��������� ������ ����� - ������ �� �����
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
inline void BITBOARDGENERATOR::list_capture_white
(
	unsigned __int8 name,// ��� ������� ������
	unsigned __int8 type_move,// ��� ����
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 & to,// ���� �����
	unsigned __int8 & count,// ����� ���� � ������
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{
	unsigned __int8 capture;// ������ ������

#if KONTROL_B
	KONTROL_captures_W(p_bitboard,&from,&to,&count,name);
#endif//KONTROL_B

	// ������ ������
	capture = p_bitboard->black_name_from_coord[to];

	// ���������� ������ ������(mvv/lva � ���� ���������) ��� ����������� ����������
	p_list_surplus_moves->sorting_score[count] = 10 * capture - name; 

	// 6 ��� 
	// �������� ��� ������ ������ (1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 
	// 6 - ������)
	p_list_surplus_moves->move[count] = capture

	// 6 ��� 
	// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
	//4 � �������� ��������� 5 � ������ �� �������
	//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 
	//15 - ����������� � ����� 
	//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 
	//24 - ������ � ������������ � ����� 
	//25 - ������ � ������������ � ����� )
	| (type_move << 6)

	// 6 ��� 
	// �������� ������ ����� (0 -63)
	| (from << 12)

	// 6 ��� 
	// �������� ���� ����� (0 -63)
	| (to << 18)

	// 3 ��� 
	// ������� ������ (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "p_list_surplus_moves->move[count]= "<< p_list_surplus_moves->move[count] << "\n";
	//
	//std::cout << "p_bitboard->black_name_from_coord[to]= "<< p_bitboard->black_name_from_coord[to] << "\n";
	//std::cout << "p_bitboard->black_name_from_coord[to]= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
	//
	//std::cout << "type_move= "<< type_move << "\n";
	//std::cout << "type_move= "<< ((p_list_surplus_moves->move[count]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((p_list_surplus_moves->move[count]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((p_list_surplus_moves->move[count]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((p_list_surplus_moves->move[count]>>24)& 7)  << "\n";
	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// ��������� ������ ����� - ������ �� ������
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
inline void BITBOARDGENERATOR::list_capture_black
(
	unsigned __int8 name,// ��� ������� ������
	unsigned __int8 type_move,// ��� ����
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 & to,// ���� �����
	unsigned __int8 & count,// ����� ���� � ������
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{
	unsigned __int8 capture;// ������ ������

#if KONTROL_B
	KONTROL_captures_B(p_bitboard,&from,&to,&count,name);
#endif//KONTROL_B

	// ������ ������
	capture = p_bitboard->white_name_from_coord[to];

	// ���������� ������ ������(mvv/lva � ���� ���������) ��� ����������� ����������
	p_list_surplus_moves->sorting_score[count] = 10 * capture - name; 

	// 6 ��� 
	// �������� ��� ������ ������ (1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 6 - ������)
	p_list_surplus_moves->move[count] = capture

	// 6 ��� 
	// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
	//4 � �������� ��������� 5 � ������ �� �������
	//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
	//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
	//25 - ������ � ������������ � ����� )
	| (type_move << 6)

	// 6 ��� 
	// �������� ������ ����� (0 -63)
	| (from << 12)

	// 6 ��� 
	// �������� ���� ����� (0 -63)
	| (to << 18)

	// 3 ��� 
	// ������� ������ (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "p_list_surplus_moves->move[count]= "<< p_list_surplus_moves->move[count] << "\n";
	//
	//std::cout << "p_bitboard->white_name_from_coord[to]= "<< p_bitboard->white_name_from_coord[to] << "\n";
	//std::cout << "p_bitboard->white_name_from_coord[to]= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
	//
	//std::cout << "type_move= "<< type_move << "\n";
	//std::cout << "type_move= "<< ((p_list_surplus_moves->move[count]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((p_list_surplus_moves->move[count]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((p_list_surplus_moves->move[count]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((p_list_surplus_moves->move[count]>>24)& 7)  << "\n";
	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// ��������� ������ �����
inline void BITBOARDGENERATOR::list_record_move
(
	unsigned __int8 name,// ��� ������� ������
	unsigned __int8 type_move,// ��� ����
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 & to,// ���� �����
	unsigned __int8 & count,// ����� ���� � ������
	const struct T_Bitboard * const p_bitboard,// ������������� �����
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{

#if KONTROL_B
	KONTROL_move(&from,&to,&count);
#endif//KONTROL_B

	// 6 ��� 
	// ������ ������ 0
	p_list_surplus_moves->move[count] = 0

	// 6 ��� 
	// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
	//4 � �������� ��������� 5 � ������ �� �������
	//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
	//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
	//25 - ������ � ������������ � ����� )
	| (type_move << 6)

	// 6 ��� 
	// �������� ������ ����� (0 -63)
	| (from << 12)

	// 6 ��� 
	// �������� ���� ����� (0 -63)
	| (to << 18)

	// 3 ��� 
	// ������� ������ (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "p_list_surplus_moves->move[p]= "<< p_list_surplus_moves->move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((p_list_surplus_moves->move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((p_list_surplus_moves->move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((p_list_surplus_moves->move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((p_list_surplus_moves->move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// ��������� ������ ����� ��� ������ ������� ���� ��������� ������ �� ������� � ����������� �����
inline void BITBOARDGENERATOR::list_record_special
(
	unsigned __int8 name,// ��� ������� ������
	unsigned __int8 d_m,// ��� ����
	unsigned __int8 & p,// ����� ���� � ������
	const unsigned __int8 & from,// ������ �����
	unsigned __int8 to,// ���� �����
	unsigned __int8 capture,// ��� ������ ������
	struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{
	// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ��� ��������� ����������� ����� ������ �� ������� 
	// ����� ����� ���� � ����� �� ���������� ������� ����� �������
	p_list_surplus_moves->sorting_score[p] = d_m;

	// 6 ��� 
	// ������ ������ 0
	p_list_surplus_moves->move[p] = capture

	// 6 ��� 
	// �������� ��� ���� (0 - ��� ���� 1 � ������� ��� 2 � ������ 3 � ������� ���������
	//4 � �������� ��������� 5 � ������ �� �������
	//12 - ����������� � ���� 13 - ����������� � ���� 14 - ����������� � ����� 15 - ����������� � ����� 
	//22 - ������ � ������������ � ���� 23 - ������ � ������������ � ���� 24 - ������ � ������������ � ����� 
	//25 - ������ � ������������ � ����� )
	| (d_m << 6)

	// 6 ��� 
	// �������� ������ ����� (0 -63)
	| (from << 12)

	// 6 ��� 
	// �������� ���� ����� (0 -63)
	| (to << 18)

	// 3 ��� 
	// ������� ������ (0 - 6)
	| (name << 24);

	//-------------------------------------------------------------------------------
	//std::cout << "p_list_surplus_moves->move[p]= "<< p_list_surplus_moves->move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( p_list_surplus_moves->move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((p_list_surplus_moves->move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((p_list_surplus_moves->move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((p_list_surplus_moves->move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((p_list_surplus_moves->move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	p = p + 1;
}

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ����� ����� ���������� ������
inline void BITBOARDGENERATOR::KONTROL_captures_W
(
	const struct T_Bitboard * p_bitboard,// ������������� �����
	const unsigned __int8 * p_from,// ������ ����� ������
	const unsigned __int8 * p_to,// ���� ����� ������
	const unsigned __int8 * p_p,// ����� ���� � ������
	unsigned __int8 name_figure// ��� ������� ������
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((*p_p < 0)||(*p_p > 256))
	{
		std::cout << "W ERROR Generator_captures p= "   << *p_p    << "\n";
	}

	if ((*p_from < 0)||(*p_from > 63))
	{
		std::cout << "W ERROR Generator_captures from= " << *p_from << "\n";
	}

	if ((*p_to < 0)||(*p_to > 63))
	{
		std::cout << "W ERROR Generator_captures to= "  << *p_to   << "\n";
	}

	if ((p_bitboard->white_name_from_coord[*p_from] < 1)
	||(p_bitboard->white_name_from_coord[*p_from] > 6))
	{
		std::cout << "W ERROR Generator_captures p_bitboard->white_name_from_coord[*from] = "  
			<< p_bitboard->white_name_from_coord[*p_from]<< "\n";
	}

	if ((p_bitboard->black_name_from_coord[*p_to] < 1)
	||(p_bitboard->black_name_from_coord[*p_to] > 6))
	{
		std::cout << "W ERROR Generator_captures p_bitboard->black_name_from_coord[*p_to]= " 
			<< p_bitboard->black_name_from_coord[*p_to]   << "\n";
		std::cout << "W ERROR Generator_captures from= "<< *p_from << "\n";
		std::cout << "W ERROR Generator_captures to= "  << *p_to   << "\n";
		std::cout << "W ERROR Generator_captures name_figure= "  << name_figure << "\n";
		Bitboard_print(p_bitboard);
	}

	//if (p_bitboard->black_name_from_coord[*to] ==6)
	//{
		//std::cout << "ERROR Generator_captures name_figure KING "  << "\n";
	//}
	///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ������ ����� ���������� ������
inline void BITBOARDGENERATOR::KONTROL_captures_B
(
	const struct T_Bitboard * p_bitboard,// ������������� �����
	const unsigned __int8 * p_from,// ������ ����� ������
	const unsigned __int8 * p_to,// ���� ����� ������
	const unsigned __int8 * p_p,// ����� ���� � ������
	unsigned __int8 name_figure// ��� ������� ������
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((*p_p < 0)||(*p_p > 256))
	{
		std::cout << "B ERROR Generator_captures p= "   << *p_p    << "\n";
	}

	if ((*p_from < 0)||(*p_from > 63))
	{
		std::cout << "B ERROR Generator_captures from= " << *p_from << "\n";
	}

	if ((*p_to < 0)||(*p_to > 63))
	{
		std::cout << "B ERROR Generator_captures to= "  << *p_to   << "\n";
	}

	if ((p_bitboard->black_name_from_coord[*p_from] < 1)
	||(p_bitboard->black_name_from_coord[*p_from] > 6))
	{
		std::cout << "B ERROR Generator_captures p_bitboard->black_name_from_coord[*p_from] = "
			<< p_bitboard->black_name_from_coord[*p_from]   << "\n";
	}

	if ((p_bitboard->white_name_from_coord[*p_to] < 1)
	||(p_bitboard->white_name_from_coord[*p_to] > 6))
	{
		std::cout << "B ERROR Generator_captures p_bitboard->white_name_from_coord[*p_to]= "
			<< p_bitboard->white_name_from_coord[*p_to]   << "\n";
		std::cout << "B ERROR Generator_captures from= "<< *p_from << "\n";
		std::cout << "B ERROR Generator_captures to= "  << *p_to   << "\n";
		std::cout << "B ERROR Generator_captures name_figure= "  << name_figure << "\n";
		Bitboard_print(p_bitboard);
	}

	//if (p_bitboard->white_name_from_coord[*to] ==6)
	//{
	//std::cout << "B ERROR Generator_captures name_figure KING "  << "\n";
	//}
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ 
// ��� ���������� ����� 
inline void BITBOARDGENERATOR::KONTROL_move
(
	const unsigned __int8 * p_from,// ������ ����� ������
	const unsigned __int8 * p_to,// ���� ����� ������
	const unsigned __int8 * p_p// ����� ���� � ������
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((*p_p < 0)||(*p_p > 256))
	{
		std::cout << "ERROR Generator_move p= "   << *p_p    << "\n";
	}

	if ((*p_from < 0)||(*p_from > 63))
	{
		std::cout << "ERROR Generator_move from= " << *p_from << "\n";
	}

	if ((*p_to < 0)||(*p_to > 63))
	{
		std::cout << "ERROR Generator_move to= "  << *p_to   << "\n";
	}
	///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// �������� �������� �����
#if TEST_B
void BITBOARDGENERATOR::Bitboard_print
(
	const struct T_Bitboard * p_bitboard// ������������� �����
)
{
	unsigned __int64 z = 0;
	Test_Bitboard.open ("_TEST_BITBOARD.txt",std::ios::app);

	Test_Bitboard << "bitboard->colour_move = "<< p_bitboard->colour_move <<"\n";
	Test_Bitboard << "\n";

	Test_Bitboard << "p_bitboard->taking_passage_yes= " << p_bitboard->taking_passage_yes << "\n";
	Test_Bitboard << "bitboard->taking_passage_x= " << p_bitboard->taking_passage_x << "\n";
	Test_Bitboard << "p_bitboard->taking_passage_y= " << p_bitboard->taking_passage_y << "\n";

	Test_Bitboard << "\n";
	Test_Bitboard << "p_bitboard->castling_K= " << p_bitboard->castling_K << "\n";
	Test_Bitboard << "p_bitboard->castling_Q= " << p_bitboard->castling_Q << "\n";
	Test_Bitboard << "p_bitboard->castling_k= " << p_bitboard->castling_k << "\n";
	Test_Bitboard << "p_bitboard->castling_q= " << p_bitboard->castling_q << "\n";

	Test_Bitboard << "------------------------"<<"\n";
	Test_Bitboard << "\n";

	// all_white_piece
	//-------------------------------------------------------------------------------------
	Test_Bitboard << "\n";
	Test_Bitboard << "all_white_piece"<< "\n";

	for (int i = 63; i > -1; i = i - 8)
	{
		for (int j = i - 7; j < i + 1; j++)
		{
			z = p_bitboard->all_white_piece & move_masks[j];

			if (z == 0)
			{
				Test_Bitboard << "0 ";
			}
			else
			{
				Test_Bitboard << "1 ";
			}
		}

		Test_Bitboard << "\n";
	}
	//-------------------------------------------------------------------------------------

	// all_black_piece
	//-------------------------------------------------------------------------------------
	Test_Bitboard << "\n";
	Test_Bitboard << "all_black_piece"<< "\n";

	for (int i = 63; i > -1; i = i - 8)
	{
		for (int j = i - 7; j < i + 1; j++)
		{
			z = p_bitboard->all_black_piece & move_masks[j];

			if (z == 0)
			{
				Test_Bitboard << "0 ";
			}
			else
			{
				Test_Bitboard << "1 ";
			}
		}

		Test_Bitboard << "\n";

	}
	//-------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------
	Test_Bitboard << "\n";
	Test_Bitboard << "white_piece"<< "\n";
	for (int p = 1; p < 7; p++)
	{
		Test_Bitboard << "\n";
		Test_Bitboard << p << "\n";

		for (int i = 63; i > -1; i = i - 8)
		{
			for (int j = i - 7; j < i + 1; j++)
			{
				z =  p_bitboard->white_piece[p] & move_masks[j];

				if (z == 0)
				{
					Test_Bitboard << "0 ";
				}
				else
				{
					Test_Bitboard << "1 ";
				}

			}

			Test_Bitboard << "\n";

		}
	}
	//-------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------
	Test_Bitboard << "\n";
	Test_Bitboard << "black_piece"<< "\n";

	for (int p = 1; p < 7; p++)
	{
		Test_Bitboard << "\n";
		Test_Bitboard << p << "\n";

		for (int i = 63; i > -1; i = i - 8)
		{
			for (int j = i - 7; j < i + 1; j++)
			{
				z =  p_bitboard->black_piece[p] & move_masks[j];

				if (z == 0)
				{
					Test_Bitboard << "0 ";
				}
				else
				{
					Test_Bitboard << "1 ";
				}

			}

			Test_Bitboard << "\n";

		}
	}
	//-------------------------------------------------------------------------------------

	Test_Bitboard << "============================================"<< "\n";

	Test_Bitboard.close();

}
#endif//TEST_B


/*
��� ��� ��� ������������ ����������� ���� ��������������� 
������������ �����
���� �����. - ��������� ��� ����������� Intel Pentium.
� ��� ����� ���� ���������(dos) ����� ������� ����� ������ ��������
� ������� ���������� �� ��������� �������
(��� ��� �������� � ������ ��������� :))
*/

//	===================================================================
// ���� ������ ������������� ���(�.�. ����� ������)
__inline __int32 BITBOARDGENERATOR::bit_scan_forward_win32
(
	unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
)
{ 
	_asm
	{ 
		/*
		�� �������� �� ����������(This document saved from http://mind.telenet.ru/samouch/7/31/index.html):
		������� mov �������� ������ ������� (��������) ������ (����������). 
		��� ���� �������� �������� ������� �������� ��������. 
		������ ������� �� ����������. � ����������� �� �������� ���������, 
		������������ ����� ��� ����. ���� �������� ������� ��-������� ��� 
		����� ��������� �� ��������� ���������� ���������� ������ ��������, 
		��� ��������� ������� ������������ ������ � ������� ������� �������� ���� 
		�� ���������� ���������� byte ptr ��� word ptr. 
		������� �� ������������ �� ����� ����������. 
		����� ��������� ������������� ��������� �������� �������� - 
		� ���� ������ ����������, ��� � �����. ��� ����� ���� ������������ �������� word ptr:
		*/
		mov  eax, dword ptr analyzed_number[0]

		/*
		������� test ��������� �������� ����������� ��������� 
		� ��� ����� ���������� �, � ����������� �� ����������, 
		������������� ����� SF, ZF � PF. ����� OF � CF ������������, 
		a AF ����� �������������� ��������. ��������� ������ ����� ����� 
		���������������� ��������� �������� ���������. ������� test �� �������� 
		�� ���� �� ���������. � �������� ������� �������� ������� test ����� 
		��������� ������� (����� �����������) ��� ������ ������, � �������� 
		������� - ������� (����� �����������), ������ ������ ��� ���������������� 
		��������, ������ �� ����������� ���������� ��� �������� ������������ ��� 
		������ ������. �������� ����� ���� ������� ��� ������� � ������������ ����� 
		�� ������ ��� ��� �����. 

		������� ���������� ���������:

		������ �������-��� 0101
		������ �������-��� 0011
		��� ���������� 0001

		���� SF ��������������� � 1, ���� � ���������� ���������� ������� 
		������������ ����� � ������������� �������� �����.
		���� ZF ��������������� � 1, ���� � ���������� ���������� ������� 
		������������ �����, ��������� �� ����� �������� �����.
		���� PF ��������������� � 1, ���� � ���������� ���������� ������� 
		������������ ����� � ������ ����������� �������� ������ � ��� �����.
		*/
		test eax, eax

		/*
		������� �������, ���� ������� ��������
		jz ���� ZF= 1
		*/
		jz   grown_up

		/*
		������� bsf ��������� ����� ��� ������� ����� � ������� ����, ������� 1. 
		������������ ����������� �� �������� ���� (0) � ��������. ���� � ����� 
		�� ������� ������������� �����, �� ��������������� ���� ZF. 
		���� ������������� ���� ����, �� ����� ������� �������������� ���� 
		��������� � ��������� � ������� �������. ������� ���� ��������� ��� 
		������� � �����, ������������� �� ���� 0. � �������� ������� �������� ������� bsf 
		������� ��������� �������, ���� ����� ������� ��������� ������������, � 
		�������� ������� - ������� ��� ������ ������ �� ����������� ������. 
		� ������� bsf ��������� ������������� ��� 16-�������, ��� � 32-������� ��������� 
		(�� � ������, � ������ �������� ������ ���� ������ ����).
		*/
		bsf  eax, eax

		/*
		������� jmp �������� ���������� � ��������� ����� ���� �� ��� ������� ������������ 
		��������. ����� �������� �� �����������. ������� �� ������������ �� ����� ����������.
		*/
		jmp jump_end
		grown_up:    bsf  eax, dword ptr analyzed_number[4]

		/*
		��������
		*/
		add  eax, 20h
		jump_end:
	}
}

//	===================================================================
// ���� ��������� ������������� ���(�.�. ����� �����)
__inline __int32 BITBOARDGENERATOR::bit_scan_reverse_win32
(
	unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
)
{ 

	_asm
	{ 
		mov  eax, dword ptr analyzed_number[4]
		test eax, eax
		jz   younger

		/*
		������� bsr ��������� ����� ��� ������� ����� � ������� ����, ������� 1. 
		������������ ����������� �� �������� ���� (15 ��� 31) � ��������. 
		���� � ����� �� ������� ������������� �����, �� ��������������� ���� ZF. 
		���� ������������� ���� ����, �� ����� ������� �������������� ���� ��������� 
		� ��������� � ������� �������. ������� ���� ��������� ��� ������� � �����, 
		������������� �� ���� 0. � �������� ������� �������� ������� bsr ������� 
		��������� �������, ���� ����� ������� ��������� ������������, � �������� 
		������� - ������� ��� ������ ������ �� ����������� ������. 
		� ������� bsr ��������� ������������� ��� 16-�������, ��� � 32-������� ���������, 
		�� � ������, � ������ �������� ������ ���� ������ ���� (�� ����������� ������, 
		����� ������ ������� - ���������).
		*/
		bsr  eax, eax
		add  eax, 20h
		jmp  jump_end
		younger:    bsr  eax, dword ptr analyzed_number[0]
		jump_end:
	}
}