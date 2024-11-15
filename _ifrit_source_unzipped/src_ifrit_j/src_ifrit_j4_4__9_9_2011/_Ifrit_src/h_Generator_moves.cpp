
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 21.02.2009  21.11.2010

#include <fstream>
#include <iostream>

#include "a_Search_switch.h"
#include "a_List_const.h"
#include "a_Position_const.h"
#include "h_Generator_moves.h"
#include "h_Generator_moves_const.h"
#include "k_History.h"

extern class k_History history;


//	===================================================================
// ���������� ������ ������ + ����������� ����� (��� �����)
void h_Generator_moves::generator_captures_white
(
	struct List & list,// ������������� ������ �����
	const struct Position & position // ������������� �����    
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
	from_mask = position.white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//�������� ���� ����� 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		/* 
		����������� ����� �������� ������ ��������:
		line_index = 8 * rank_index + file_index
		to = 8 * position.taking_passage_y + position.taking_passage_x;

		�������� ����������� ������ �� �����:
		file_index   = line_index modulo 8 = line_index % 8  = line_index & 7
		rank_index   = line_index div 8    = line_index / 8  = line_index >> 3

		����� � ���:
		line_index � ���������� ���������� ����� (�� 0 �� 63)
		rank_index � ���������� ����� �� ���������(�� 0 �� 7)
		file_index � ���������� ����� �� ����������� (�� 0 �� 7)
		*/

		// ������� ���� � ������������ � ������ � ������������
		if ( from / 8 == 6 )// ���� �� �� ������������� ����������� �.�. ��������� ���
							// ����� � ������������
		{
			// 1 ������� ������� ���� ����� ����� �� ���� ���� ������
			move_conversion_mask = (one_pawn_mask << 8) & (~(position.all_piece));

			// ������ � ������������
			captures_conversion_mask = pawn_capture_white[from] &  position.all_black_piece;

			// ������� ���������� ����� � ������������ �����
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// �������� ����������� �����
				// ����������� � ����� 15,����������� � ����� 14,����������� � ����� 13,����������� � ���� 12
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0);
 
#if KONTROL_B
				KONTROL_move(from,to,p);
#endif//KONTROL_B
			}

			// ������� ���������� ������ � ������������ �����  
			while (captures_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(captures_conversion_mask);
				captures_conversion_mask = captures_conversion_mask & (captures_conversion_mask - 1);

				// �������� ����������� ����� � �������
				// ����������� � ����� 25,����������� � ����� 24,����������� � ����� 23,����������� � ���� 22
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,position.black_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,position.black_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,position.black_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,position.black_name_from_coord[to]);

#if KONTROL_B
				KONTROL_captures_W(position,from,to,p,120);
#endif//KONTROL_B
			}

		}
		else
		{
			// ������� ������ ������
			captures_mask = pawn_capture_white[from] &  position.all_black_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_white(list,N_PAWN,CAPTURE_MOVE,from,to,p,position);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 ������� ������ �� �������
		if (position.taking_passage_yes == 1)
		{
			// ������ ����� ����� ������� �� ��� ���� ������ � ��������� �������
			// ������ ������ ���� ����� ����� ����� �� 4 ������ � ���� �����������
			// ������ ����� ����� ������� ����� ����� ���� �.�. 
			// ����� ��������� ������ �� �������
			if ( from / 8 == 4 )
			{
				to = 8 * position.taking_passage_y + position.taking_passage_x;

				// ������� ������ �� �����. ����� �� ������ ���� �� ����� ������ ���������
				if (((from - to) == -9) && (from % 8 != 7))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.black_name_from_coord[to - 8]);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(position,from,to,p,130);
#endif//KONTROL_B
				}

				// ������� ������ �� ������. ����� �� ������ ���� �� ����� ����� ���������
				if (((from - to) == -7) && (from % 8 != 0))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.black_name_from_coord[to - 8]);

#if KONTROL_B
					to = to - 8;
					KONTROL_captures_W(position,from,to,p,130);
#endif//KONTROL_B
				}
			}
		}
	}
	// �����----------------------------------------------------------------------------------


	// ���� ���� ----------------------------------------------------------------------------------
	// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
	// ������������ ����� ������� ��� ��� ������� �� ��� ����������
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (position.all_black_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(list,N_KNIGHT,CAPTURE_MOVE,from,to,p,position);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// ���� ----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------
	}
	// �����---------------------------------------------------------------------------------- 


	// ���� �����---------------------------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� �����------------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_left_masks[from];
		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������-----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------
	}
	// �����----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------
	// ����� ������������ ������ �����
	from_mask = position.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);

		//������� � ������� ��������� ������������ (�� ���� �����)

		//��� �����------------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = (position.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			// 2) ������� ���������� ������� �������������� ����
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);

			// 3) ������� ����� ������
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			// 4) ��������� ������ �����
			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = position.all_piece & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_white(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����--------------------------------------------------------------------------------------


	// ���� ������--------------------------------------------------------------------------------
	// ����� ������������ ������ ������
	from_mask = position.white_piece[N_KING];

	if (from_mask != 0)
	{
		// ������� ���������� ������(����� ������(��� ������ ����������� ������ ) ������������� ���)
		from = bit_scan_forward_win32(from_mask);

		// ������� ����� ������ ������
		captures_mask = (position.all_black_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			// ��������� ������ �����
			to = bit_scan_forward_win32(captures_mask);
			list_capture_white(list,N_KING,CAPTURE_MOVE,from,to,p,position);
			captures_mask = captures_mask & (captures_mask - 1);

		}

	}
	// ���� ������--------------------------------------------------------------------------------

	list.end_list = p;
	list.end_captures = p;

}

//	===================================================================
// ���������� ������ ������  + ����������� ����� (��� ������)
void h_Generator_moves::generator_captures_black
(
	struct List & list,// ������������� ������ �����
	const struct Position & position // ������������� �����   
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
	from_mask = position.black_piece[N_PAWN];

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
			move_conversion_mask = (one_pawn_mask >> 8) & (~(position.all_piece));

			// ������ � ������������
			captures_conversion_mask = pawn_capture_black[from] &  position.all_white_piece;

			// ������� ���������� ����� � ������������ �����
			while (move_conversion_mask != 0)
			{
				to = bit_scan_forward_win32(move_conversion_mask);
				move_conversion_mask = move_conversion_mask & (move_conversion_mask - 1);

				// �������� ����������� �����
				// ����������� � ����� 15,����������� � ����� 14,����������� � ����� 13,
				// ����������� � ���� 12
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_QUEEN,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_ROOK,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_BISHOP,p,from,to,0);
				list_record_special(list,N_PAWN,TRANSFORMATION_IN_KNIGHT,p,from,to,0);
 
#if KONTROL_B
				KONTROL_move(from,to,p);
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
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_QUEEN,p,from,to,position.white_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_ROOK,p,from,to,position.white_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_BISHOP,p,from,to,position.white_name_from_coord[to]);
				list_record_special(list,N_PAWN,CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT,p,from,to,position.white_name_from_coord[to]);

#if KONTROL_B
				KONTROL_captures_B(position,from,to,p,120);
#endif//KONTROL_B
			}
		}
		else
		{
			// ������� ������ ������
			captures_mask =  pawn_capture_black[from] &  position.all_white_piece;

			while (captures_mask != 0)
			{
				to = bit_scan_forward_win32(captures_mask);
				list_capture_black(list,N_PAWN,CAPTURE_MOVE,from,to,p,position);
				captures_mask = captures_mask & (captures_mask - 1);
			}
		}

		// 4 ������� ������ �� �������
		if (position.taking_passage_yes == 1)
		{
			if ( from / 8 == 3 )
			{
				to = 8 * position.taking_passage_y + position.taking_passage_x;

				if (((from - to) == 9) && (from % 8 != 0))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.white_name_from_coord[to + 8]);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(position,from,to,p,130);
#endif//KONTROL_B
				}

				if (((from - to) == 7) && (from % 8 != 7))
				{
					list_record_special(list,N_PAWN,TAKING_PASSAGE,p,from,to,position.white_name_from_coord[to + 8]);

#if KONTROL_B
					to = to + 8;
					KONTROL_captures_B(position,from,to,p,130);
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
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		captures_mask = (position.all_white_piece & knigt_moves_masks[from]);

		while (captures_mask != 0)
		{
		   to = bit_scan_forward_win32(captures_mask);
		   list_capture_black(list,N_KNIGHT,CAPTURE_MOVE,from,to,p,position);
		   captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// ���� ----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������
		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_BISHOP,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����---------------------------------------------------------------------------------- 


	// ���� �����-----------------------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� �����------------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_ROOK,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_up_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������
		// ��� ����� - ������---------------------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

		if (blocking_square_mask != 0)
		{ 
			blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
			captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

			if (captures_mask != 0)
			{
				list_capture_black(list,N_QUEEN,CAPTURE_MOVE,from,blocking_coordinate,p,position);
			}
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� ������---------------------------------------------------------------------------------
	from_mask = position.black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		captures_mask = (position.all_white_piece & king_moves_masks[from]);

		while (captures_mask != 0)
		{
			to = bit_scan_forward_win32(captures_mask);
			list_capture_black(list,N_KING,CAPTURE_MOVE,from,to,p,position);
			captures_mask = captures_mask & (captures_mask - 1);
		}
	}
	// ������---------------------------------------------------------------------------------


	list.end_list = p;
	list.end_captures = p;

}

//	===================================================================
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ + ����������� �����)
//(��� �����)
void h_Generator_moves::generator_move_white
(
	struct List & list,// ������������� ������ �����
	const struct Position & position // ������������� �����   
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

	p = list.end_list;

	// ���������� ���� ����� �����
	// ���� ������--------------------------------------------------------------------------------
	// ����� ������������ ������ ������
	from_mask = position.white_piece[N_KING];

	if (from_mask != 0)
	{
		// ������� ���������� ������(����� ������(��� ������ ����������� ������ ) 
		// ������������� ���)
		from = bit_scan_forward_win32(from_mask);

		// ������� ����� ����� ������(��� ������)
		move_mask = king_moves_masks[from] & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_KING,SIMPLE_MOVE,from,to,p,position);
		}

		// ��������� ������ ������

		// ���� ������� ��������� ���������
		if (position.castling_Q)
		{
			// 
			to = C1;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// ����  1 2 3 ������  �� ��������� ���
			if (((~position.all_piece) & CASTLING_Q ) == CASTLING_Q)
			{
				// ������� ��������� �����  0-0-0 ��������� ��� ���� �� ���� 2 3 4 
				if ((white_check_square(C1,position) == 0) 
				&& (white_check_square(D1,position) == 0) 
				&& (white_check_square(E1,position) == 0))
				{
					list_record_special(list,N_KING,LONG_CASTLING,p,from,to,0);
				}
			}
		}

		// ���� �������� ��������� ��������� �� ��������� ��� 
		if (position.castling_K)
		{
			to = G1;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			//����  5 6 ������  �� ��������� ���
			if (((~position.all_piece) & CASTLING_K ) == CASTLING_K)
			{
				// �������� ��������� ����� 0-0 ��������� ��� ���� �� ����  4 5 6
				if ((white_check_square(E1,position) == 0)
				&& (white_check_square(F1,position) == 0)
				&& (white_check_square(G1,position) == 0))
				{
					list_record_special(list,N_KING,SHORT_CASTLING,p,from,to,0);
				}
			}
		}
	}
	// ���� ������--------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	// ����� ������������ ������ �����
	from_mask = position.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// ���� �������� ��� ����������� ���� b = b & (b-1)  
		// ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = position.all_piece & rook_up_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = position.all_piece & rook_left_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------


		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

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
			list_record_move_white(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = position.all_piece & rook_up_masks[from];

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
			list_record_move_white(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = position.all_piece & rook_left_masks[from];

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
			list_record_move_white(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

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
			list_record_move_white(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

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
			list_record_move_white(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------
	}
	// �����----------------------------------------------------------------------------------



	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

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
			list_record_move_white(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

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
			list_record_move_white(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

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
			list_record_move_white(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

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
			list_record_move_white(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------
	}
	// �����---------------------------------------------------------------------------------- 


	// ���� ���� ----------------------------------------------------------------------------------
	// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
	// ������������ ����� ������� ��� ��� ������� �� ��� ����������
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		move_mask = knigt_moves_masks[from] & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_KNIGHT,SIMPLE_MOVE,from,to,p,position);
		}
	}
	// ���� ----------------------------------------------------------------------------------

	// ���� �����----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		//�������� ���� ����� 
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 ������� ������� ���� ����� ����� �� ���� ���� ������
		move_mask = (one_pawn_mask << 8) & (~(position.all_piece));

		// 2 ������� ���� ����� �� 2 ���� ������ 
		// � ������ ��������� ������ ����� ������� �� 2 �����������
		move_mask_d = one_pawn_mask & LINE_1;

		// �������� ����� ���� ��� ��� �� ��������������
		move_mask_d = (move_mask_d << 8) & (~(position.all_piece));

		// ����������� ������� ������ ���
		move_mask_d = (move_mask_d << 8) & (~(position.all_piece));

		// ����� ��� ���� ����� ��� �� ���� ��� �� ��� ���� ������
		move_mask = (move_mask | move_mask_d) ;

		// ���� ��� �����������
		move_mask = move_mask & ~LINE_7;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_white(list,N_PAWN,SIMPLE_MOVE,from,to,p,position);
		}
	}
	// �����----------------------------------------------------------------------------------

	list.end_list = p;

}

//	===================================================================
// ���������� ������ ��������� ����� (�.�. ���� ����� ����� ������ + ����������� �����)(��� ������)
void h_Generator_moves::generator_move_black
(
	struct List & list,// ������������� ������ �����
	const struct Position & position // ������������� �����   
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

	p = list.end_list;

	// ���������� ���� ������ �����
	// ���� ������---------------------------------------------------------------------------------
	from_mask = position.black_piece[N_KING];

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		move_mask = king_moves_masks[from] & (~(position.all_piece));

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_KING,SIMPLE_MOVE,from,to,p,position);
		}

		// ��������� ������� ������

		// ���� ������� ��������� ���������
		if (position.castling_q)
		{
			to = C8;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// � ����  1 2 3 ������  �� ��������� ���
			if ((~(position.all_piece) & CASTLING_q )== CASTLING_q)
			{
				// ������� ��������� ������ ��������� 0-0-0 58 59 60
				if ((black_check_square(C8,position) == 0)
				&& (black_check_square(D8,position) == 0)
				&& (black_check_square(E8,position) == 0))
				{
					list_record_special(list,N_KING,LONG_CASTLING,p,from,to,0);			 
				}
			}
		}

		// ���� �������� ��������� ��������� �� ��������� ��� 
		if (position.castling_k)
		{
			to = G8;
#if KONTROL_B
			KONTROL_move(from,to,p);
#endif//KONTROL_B

			// � ����  5 6 ������  �� ��������� ���
			if ((~(position.all_piece) & CASTLING_k )== CASTLING_k)
			{
				// �������� ��������� ������ ��������� 0-0 60 61 62
				if ((black_check_square(E8,position) == 0)
				&& (black_check_square(F8,position) == 0)
				&&(black_check_square(G8,position) == 0))
				{
					list_record_special(list,N_KING,SHORT_CASTLING,p,from,to,0);
				}
			}
		}
	}
	// ������---------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	// ����� ������������ �����
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);

		// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� 
		//����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = position.all_piece & rook_up_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = position.all_piece & rook_left_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------


		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

		// ������� � ������� ��������� ������������ (�� ���� �����)
		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

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
			list_record_move_black(list,N_QUEEN,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

	}
	// �����----------------------------------------------------------------------------------


	// ���� �����----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� �����-----------------------------------------------------------------
		// 1) ������� ������������� ����.
		blocking_square_mask = position.all_piece & rook_up_masks[from];

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
			list_record_move_black(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� �����-----------------------------------------------------------------
		blocking_square_mask = position.all_piece & rook_left_masks[from];

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
			list_record_move_black(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����------------------------------------------------------------------  
		blocking_square_mask = (position.all_piece) & rook_down_masks[from];

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
			list_record_move_black(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ������----------------------------------------------------------------
		blocking_square_mask = (position.all_piece) & rook_right_masks[from];

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
			list_record_move_black(list,N_ROOK,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------
	}
	// �����----------------------------------------------------------------------------------


	// ���� �����---------------------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		// ������� � ������� ��������� ������������ (�� ���� �����)

		// ��� ����� - ������--------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

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
			list_record_move_black(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ����� - �����---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

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
			list_record_move_black(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - �����----------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

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
			list_record_move_black(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------

		// ��� ���� - ������---------------------------------------------------------
		blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

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
			list_record_move_black(list,N_BISHOP,SIMPLE_MOVE,from,to,p,position);
		}
		//---------------------------------------------------------------------------
	}
	// �����---------------------------------------------------------------------------------- 


	// ���� ���� ----------------------------------------------------------------------------------
	// ������� ����������������� ���� � ������� ������ � ���� ��� �� ������� ����� �� ����� 
	// ������������ ����� ������� ��� ��� ������� �� ��� ����������
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);
		move_mask = knigt_moves_masks[from] & (~position.all_piece);

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_KNIGHT,SIMPLE_MOVE,from,to,p,position);
		}
	}
	// ���� ----------------------------------------------------------------------------------

	// ���� �����----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_PAWN];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		one_pawn_mask = from_mask & move_masks[from];
		from_mask = from_mask & (from_mask - 1);

		// 1 ������� ������� ���� ������ ����� �� ���� ���� ������
		move_mask = (one_pawn_mask >> 8) & (~(position.all_piece));

		// 2 ������� ���� ����� �� 2 ���� ������ 
		// � ������ ��������� ������ ����� ������� �� 7 �����������
		move_mask_d = one_pawn_mask & LINE_6;

		// �������� ����� ���� ��� ��� �� ��������������
		move_mask_d = (move_mask_d >> 8) & (~(position.all_piece));

		// ����������� ������� ������ ���
		move_mask_d = (move_mask_d >> 8) & (~(position.all_piece));

		// ����� ��� ���� ����� ��� �� ���� ��� �� ��� ���� ������
		move_mask = (move_mask | move_mask_d) ;

		// ���� ��� �����������
		move_mask = move_mask & ~LINE_0;

		while (move_mask != 0)
		{
			to = bit_scan_forward_win32(move_mask);
			move_mask = move_mask & (move_mask - 1);
			list_record_move_black(list,N_PAWN,SIMPLE_MOVE,from,to,p,position);
		}
	}
	// �����----------------------------------------------------------------------------------

	list.end_list = p;

}

//	===================================================================
// ��� ���� �� ������ ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
unsigned __int8 h_Generator_moves::king_white_check
(
	const struct Position & position // ������������� �����   
)
{

	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������


	unsigned __int8 from; // ���������� ���� ������ ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ����� ������������ ������ ������
	from_mask = position.white_piece[N_KING];

	// ������� ���������� ������(����� ������(��� ������ ����������� ������ ) ������������� ���)
	from = bit_scan_forward_win32(from_mask);

	// ��� �����------------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����------------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������-----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ������� � ������� ��������� ������������

	// ��� ����� - ������--------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����� - �����---------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - �����----------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - ������---------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����--------------------------------------------------------------
	captures_mask = (position.all_black_piece & knigt_moves_masks[from]);

	while (captures_mask != 0 )
	{
		if (position.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------


	// �� ���� ������--------------------------------------------------------------------
	captures_mask = (position.all_black_piece & king_moves_masks[from]);
	while (captures_mask != 0)
	{
		if (position.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
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
			if (position.black_name_from_coord[from + 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}

		if (from % 8 != 7)
		{
			if (position.black_name_from_coord[from + 9] == N_PAWN)
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
unsigned __int8 h_Generator_moves::king_black_check
(
	const struct Position & position // ������������� �����   
)
{

	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������


	unsigned __int8 from; // ���������� ���� ������ ������ �����
	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ���������� ����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	from_mask = position.black_piece[N_KING];
	from = bit_scan_forward_win32(from_mask);
 
	// ������� � ������� ��������� ������������

	// ��� �����------------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����------------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ������� � ������� ��������� ������������
	// ��� ����� - ������--------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����� - �����---------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - �����----------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - ������---------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����--------------------------------------------------------------
	captures_mask = (position.all_white_piece & knigt_moves_masks[from]);

	while (captures_mask != 0)
	{
		if (position.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		==  N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// �� ���� ������------------------------------------------------------------
	captures_mask = (position.all_white_piece & king_moves_masks[from]);

	while (captures_mask != 0)
	{
		if (position.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
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
			if (position.white_name_from_coord[from - 7] == N_PAWN)
			{
				return N_PAWN;
			}
		}
 
		if (from % 8 != 0)
		{
			if (position.white_name_from_coord[from - 9] == N_PAWN)
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
inline unsigned __int8 h_Generator_moves::white_check_square
(
	const unsigned __int8 & sq,//������ ������������ ����(0-63) 
	const struct Position & position // ������������� �����   
)
{

	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������

	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ��� �����------------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_up_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������-----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ��� ����� - ������--------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_up_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����� - �����---------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_up_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_black_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.black_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.black_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����
	//---------------------------------------------------------------------------
	captures_mask = (position.all_black_piece & knigt_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (position.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		==  N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// �� ���� ������
	//---------------------------------------------------------------------------
	captures_mask = (position.all_black_piece & king_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (position.black_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------


	// ����� ������� ��������
	//---------------------------------------------------------------------------
	if (position.black_name_from_coord[sq + 7] == N_PAWN)
	{
		return N_PAWN;
	}

	if (position.black_name_from_coord[sq + 9] == N_PAWN)
	{
		return N_PAWN;
	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// ��� ���� �� ������ ���� ���� ���� ��� �� ������� ���������� 0 ����� ����� �������� ������
inline unsigned __int8 h_Generator_moves::black_check_square
(
	const unsigned __int8 & sq,//������ ������������ ����(0-63) 
	const struct Position & position // ������������� �����   
)
{
	unsigned __int64 blocking_square_mask; // ����� ������������� �����
	unsigned __int64 captures_mask; // ����� ������

	__int32 blocking_coordinate; // ���������� �������������� ���� 
	unsigned __int8 i; // ���������� 

	// ��� �����-----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ����------------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_down_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ������----------------------------------------------------------------
	blocking_square_mask = (position.all_piece) & rook_right_masks[sq];

	if(blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_ROOK)
			{
				return N_ROOK;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// ��� ���� - �����----------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_down_left_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	// ��� ���� - ������---------------------------------------------------------
	blocking_square_mask = (position.all_piece) & bishop_down_right_masks[sq];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		captures_mask = position.all_white_piece & move_masks[blocking_coordinate];

		if (captures_mask != 0)
		{
			i = bit_scan_forward_win32(captures_mask);

			if (position.white_name_from_coord[i] == N_BISHOP)
			{
				return N_BISHOP;
			}

			if (position.white_name_from_coord[i] == N_QUEEN)
			{
				return N_QUEEN;
			}
		}
	}
	//---------------------------------------------------------------------------

	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	// �� ���� ����
	//---------------------------------------------------------------------------
	captures_mask = (position.all_white_piece & knigt_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (position.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KNIGHT)
		{
			return N_KNIGHT;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// �� ���� ������
	//---------------------------------------------------------------------------
	captures_mask = (position.all_white_piece & king_moves_masks[sq]);

	while (captures_mask != 0)
	{
		if (position.white_name_from_coord[bit_scan_forward_win32(captures_mask)] 
		== N_KING)
		{
			return N_KING;
		}

		captures_mask = captures_mask & (captures_mask - 1);
	}
	//---------------------------------------------------------------------------

	// ����� ������� ��������
	//---------------------------------------------------------------------------
	if (position.white_name_from_coord[sq - 7] == N_PAWN)
	{
		return N_PAWN;
	}

	if (position.white_name_from_coord[sq - 9] == N_PAWN)
	{
		return N_PAWN;
	}
	//---------------------------------------------------------------------------

	return 0; 
}

//	===================================================================
// ��������� ������ ����� - ������ �� �����
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
inline void h_Generator_moves::list_capture_white
(
	struct List & list,// ������������� ������ �����
	const unsigned __int8 name,// ��� ������� ������
	const unsigned __int8 type_move,// ��� ����
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 & to,// ���� �����
	unsigned __int8 & count,// ����� ���� � ������
	const struct Position & position // ������������� �����   
)
{
	unsigned __int8 capture;// ������ ������

#if KONTROL_B
	KONTROL_captures_W(position,from,to,count,name);
#endif//KONTROL_B

	// ������ ������
	capture = position.black_name_from_coord[to];

	// ���������� ������ ������(mvv/lva � ���� ���������) ��� ����������� ����������
	list.sorting_score[count] = 10 * capture - name; 

	// 6 ��� 
	// �������� ��� ������ ������ (1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 
	// 6 - ������)
	list.move[count] = capture

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
	//std::cout << "move[count]= "<< move[count] << "\n";
	//
	//std::cout << "position.black_name_from_coord[to]= "<< position.black_name_from_coord[to] << "\n";
	//std::cout << "position.black_name_from_coord[to]= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "type_move= "<< type_move << "\n";
	//std::cout << "type_move= "<< ((move[count]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[count]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[count]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[count]>>24)& 7)  << "\n";
	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// ��������� ������ ����� - ������ �� ������
// ��� ������� ������ �� �� ������� �������� �������(������ ������)
inline void h_Generator_moves::list_capture_black
(
	struct List & list,// ������������� ������ �����
	const unsigned __int8 name,// ��� ������� ������
	const unsigned __int8 type_move,// ��� ����
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 & to,// ���� �����
	unsigned __int8 & count,// ����� ���� � ������
	const struct Position & position // ������������� �����   
)
{
	unsigned __int8 capture;// ������ ������

#if KONTROL_B
	KONTROL_captures_B(position,from,to,count,name);
#endif//KONTROL_B

	// ������ ������
	capture = position.white_name_from_coord[to];

	// ���������� ������ ������(mvv/lva � ���� ���������) ��� ����������� ����������
	list.sorting_score[count] = 10 * capture - name; 

	// 6 ��� 
	// �������� ��� ������ ������ (1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 6 - ������)
	list.move[count] = capture

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
	//std::cout << "move[count]= "<< move[count] << "\n";
	//
	//std::cout << "position.white_name_from_coord[to]= "<< position.white_name_from_coord[to] << "\n";
	//std::cout << "position.white_name_from_coord[to]= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "type_move= "<< type_move << "\n";
	//std::cout << "type_move= "<< ((move[count]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[count]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[count]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[count]>>24)& 7)  << "\n";
	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// ��������� ������ �����
inline void h_Generator_moves::list_record_move_white
(
	struct List & list,// ������������� ������ �����
	const unsigned __int8 name,// ��� ������� ������
	const unsigned __int8 type_move,// ��� ����
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 & to,// ���� �����
	unsigned __int8 & count,// ����� ���� � ������
	const struct Position & position // ������������� �����   
)
{

#if KONTROL_B
	KONTROL_move(from,to,count);
#endif//KONTROL_B

#if SW_HISTORY
	//  ���������� �������
	list.sorting_score[count] = history.get_white_history(name,to); 
#endif//#if SW_HISTORY

	// 6 ��� 
	// ������ ������ 0
	list.move[count] = 0

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
	//std::cout << "move[p]= "<< move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// ��������� ������ �����
inline void h_Generator_moves::list_record_move_black
(
	struct List & list,// ������������� ������ �����
	const unsigned __int8 name,// ��� ������� ������
	const unsigned __int8 type_move,// ��� ����
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 & to,// ���� �����
	unsigned __int8 & count,// ����� ���� � ������
	const struct Position & position // ������������� �����   
)
{

#if KONTROL_B
	KONTROL_move(from,to,count);
#endif//KONTROL_B

#if SW_HISTORY
	// ���������� �������
	list.sorting_score[count] =  history.get_black_history(name,to); 
#endif//#if SW_HISTORY

	// 6 ��� 
	// ������ ������ 0
	list.move[count] = 0

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
	//std::cout << "move[p]= "<< move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	count = count + 1;
}


//	===================================================================
// ��������� ������ ����� ��� ������ ������� ���� ��������� ������ �� ������� � ����������� �����
inline void h_Generator_moves::list_record_special
(
	struct List & list,// ������������� ������ �����
	const unsigned __int8 name,// ��� ������� ������
	const unsigned __int8 d_m,// ��� ����
	unsigned __int8 & count,// ����� ���� � ������
	const unsigned __int8 & from,// ������ �����
	const unsigned __int8 to,// ���� �����
	const unsigned __int8 capture// ��� ������ ������
)
{
	// ��������� ������ �����+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ��� ��������� ����������� ����� ������ �� ������� 
	// ����� ����� ���� � ����� �� ���������� ������� ����� �������
	list.sorting_score[count] = 110000;   //d_m;

	// 6 ��� 
	// ������ ������ 0
	list.move[count] = capture

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
	//std::cout << "move[p]= "<< move[p] << "\n";
	//
	//std::cout << "0= "<< 0 << "\n";
	//std::cout << "0= "<< ( move[p] & 63) << "\n";
	//
	//std::cout << "d_m= "<< d_m << "\n";
	//std::cout << "d_m= "<< ((move[p]>>6)& 63)  << "\n";
	//
	//std::cout << "from= "<< from << "\n";
	//std::cout << "from= "<< ((move[p]>>12)& 63)  << "\n";
	//
	//std::cout << "to= "<< to << "\n";
	//std::cout << "to= "<< ((move[p]>>18)& 63)  << "\n";
	//
	//std::cout << "name= "<< name << "\n";
	//std::cout << "name= "<<  ((move[p]>>24)& 7)  << "\n";

	//-------------------------------------------------------------------------------

	count = count + 1;
}

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ����� ����� ���������� ������
inline void h_Generator_moves::KONTROL_captures_W
(
	const struct Position & position, // ������������� �����   
	const unsigned __int8 & from,// ������ ����� ������
	const unsigned __int8 & to,// ���� ����� ������
	const unsigned __int8 & p,// ����� ���� � ������
	const unsigned __int8 name_figure// ��� ������� ������
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((p < 0)||(p > 256))
	{
		std::cout << "W ERROR Generator_captures p= "   << p    << "\n";
	}

	if ((from < 0)||(from > 63))
	{
		std::cout << "W ERROR Generator_captures from= " << from << "\n";
	}

	if ((to < 0)||(to > 63))
	{
		std::cout << "W ERROR Generator_captures to= "  << to   << "\n";
	}

	if ((position.white_name_from_coord[from] < 1)
	||(position.white_name_from_coord[from] > 6))
	{
		std::cout << "W ERROR Generator_captures position.white_name_from_coord[*from] = "  
			<< position.white_name_from_coord[from]<< "\n";
	}

	if ((position.black_name_from_coord[to] < 1)
	||(position.black_name_from_coord[to] > 6))
	{
		std::cout << "W ERROR Generator_captures position.black_name_from_coord[*to]= " 
			<< position.black_name_from_coord[to]   << "\n";
		std::cout << "W ERROR Generator_captures from= "<< from << "\n";
		std::cout << "W ERROR Generator_captures to= "  << to   << "\n";
		std::cout << "W ERROR Generator_captures name_figure= "  << name_figure << "\n";
		position_print(position);
	}

	//if (position.black_name_from_coord[to] ==6)
	//{
		//std::cout << "ERROR Generator_captures name_figure KING "  << "\n";
	//}
	///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
// ��� ��� ��� ������ ����� ���������� ������
inline void h_Generator_moves::KONTROL_captures_B
(
	const struct Position & position, // ������������� �����   
	const unsigned __int8 & from,// ������ ����� ������
	const unsigned __int8 & to,// ���� ����� ������
	const unsigned __int8 & p,// ����� ���� � ������
	const unsigned __int8 name_figure// ��� ������� ������
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((p < 0)||(p > 256))
	{
		std::cout << "B ERROR Generator_captures p= "   << p    << "\n";
	}

	if ((from < 0)||(from > 63))
	{
		std::cout << "B ERROR Generator_captures from= " << from << "\n";
	}

	if ((to < 0)||(to > 63))
	{
		std::cout << "B ERROR Generator_captures to= "  << to   << "\n";
	}

	if ((position.black_name_from_coord[from] < 1)
	||(position.black_name_from_coord[from] > 6))
	{
		std::cout << "B ERROR Generator_captures position.black_name_from_coord[from] = "
			<< position.black_name_from_coord[from]   << "\n";
	}

	if ((position.white_name_from_coord[to] < 1)
	||(position.white_name_from_coord[to] > 6))
	{
		std::cout << "B ERROR Generator_captures position.white_name_from_coord[to]= "
			<< position.white_name_from_coord[to]   << "\n";
		std::cout << "B ERROR Generator_captures from= "<< from << "\n";
		std::cout << "B ERROR Generator_captures to= "  << to   << "\n";
		std::cout << "B ERROR Generator_captures name_figure= "  << name_figure << "\n";
		position_print(position);
	}

	//if (position.white_name_from_coord[to] ==6)
	//{
	//std::cout << "B ERROR Generator_captures name_figure KING "  << "\n";
	//}
///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

//	===================================================================
// ������������ ����� �� ������� �����, �� ������� ������ 
// ��� ���������� ����� 
inline void h_Generator_moves::KONTROL_move
(
	const unsigned __int8 & from,// ������ ����� ������
	const unsigned __int8 & to,// ���� ����� ������
	const unsigned __int8 & p// ����� ���� � ������
)
{

#if KONTROL_B
	///////////////////////////////////////////////////////////////////////////////////
	if ((p < 0)||(p > 256))
	{
		std::cout << "ERROR Generator_move p= "   << p    << "\n";
	}

	if ((from < 0)||(from > 63))
	{
		std::cout << "ERROR Generator_move from= " << from << "\n";
	}

	if ((to < 0)||(to > 63))
	{
		std::cout << "ERROR Generator_move to= "  << to   << "\n";
	}
	///////////////////////////////////////////////////////////////////////////////////
#endif//KONTROL_B
}

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
__inline __int32 h_Generator_moves::bit_scan_forward_win32
(
	const unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
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
__inline __int32 h_Generator_moves::bit_scan_reverse_win32
(
	const unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
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


/*
������
             � &  0 0 1 ������ �������
                  0 1 1 ������ �������
                  -----
                  0 0 1 ���������

           ��� |  0 0 1
                  0 1 1
                  -----
                  0 1 1

   ����������� ^  0 0 1
   ���            0 1 1
                  -----
                  0 1 0

*/