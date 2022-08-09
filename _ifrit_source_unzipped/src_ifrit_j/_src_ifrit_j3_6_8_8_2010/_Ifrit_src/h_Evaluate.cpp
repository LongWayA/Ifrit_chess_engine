#include <iostream> // TEST
#include <fstream>  // TEST
//#include <string>


#include "h_Evaluate.h"
#include "d_Bitboard_const.h"

//---------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 24.02.2009 03.07.2010
*/
//---------------------------------------------------------------------------


/*
 ������� ����� ������� 70%

��������� �������

������� 
�������� (���� ����� ������� � ���� ������ ��������)
����������� ������ PST
����������� �����
����� �� ����������� ������
���������

*/


//	===================================================================
// ������� �������� ������� � ��������� �� ���������� ������� ����������� �������
__int32 EVALUATE::public_eval_state
(
	const T_Bitboard * p_bitboard// ������������� �����
)
{
	__int32 eval = 0;//����������� ������
	unsigned __int8 from_white_king = 0;// ������������ ������ ������
	unsigned __int8 from_black_king = 0;// ������������ ������� ������
	unsigned __int8 at_white_king = 0; // ���������� ����������� ����� ������ ������ ������
	unsigned __int8 at_black_king = 0;// ���������� ����������� ����� ������ ������� ������

	// ����� ��� ���������� ������ ����
	unsigned __int16 stadia_w = 0; // �������� ��� ����� � �����
	unsigned __int16 stadia_b = 0; // �������� ��� ����� � ������

	//� ��������� ������� �� ����� 400 + 400 + 600 + 1200 = 2600 (��� ����� �������)
	//                             2*400 + 2*400 + 2*600 + 1200 
	//                             800   + 800   + 1200  + 1200 = 4000 (����� ��� ���� ������) 

	//stadia_w = 2600; // �������� ��� ����� � �����
	//stadia_b = 2600; // �������� ��� ����� � ������

	// TEST popcnt-----------------------------
	//unsigned __int64 move = 0;

	//for (int i = 0; i < 64; i++ )
	//{
	//	move = move | e_move_masks[i];
	//	//std::cout << "popcnt= " << popcnt(e_move_masks[i]) << "\n";
	//	std::cout << "popcnt= " << popcnt(move) << "\n";
	//}
    // TEST-------------------------------------

	stadia_w = p_bitboard->value_white_material;
	stadia_b = p_bitboard->value_black_material;

	// ����������� ������
	king_pst(p_bitboard, from_white_king, from_black_king, stadia_w, stadia_b, eval);

	// ����������� �����
	pawn_pst(p_bitboard, from_white_king, from_black_king, stadia_w, stadia_b, eval);

	// ����������� �����, �����, ����, ����
	// ����� ������������ 
	PST(p_bitboard, from_white_king, from_black_king, at_white_king, at_black_king, eval);

	return eval;
}

//	===================================================================
//
inline void EVALUATE::king_pst
(
	const T_Bitboard * p_bitboard,// ������������� �����
	unsigned __int8 & from_white_king,// ������������ ������ ������
	unsigned __int8 & from_black_king,// ������������ ������� ������
	const unsigned __int16 stadia_w, // �������� ��� ����� � �����
	const unsigned __int16 stadia_b, // �������� ��� ����� � ������
	__int32 & eval //����������� ������
)
{
	unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����
	unsigned __int8 from; // ���������� ���� ������ ������ �����

	/*
	����� ����������� ��������� � ��������� ������ �������� 
	�� ���� ���� ���������� �� ������������ ������

	������� ��� ������� �������� � ��������.
	���� ��� ������ ������ - �� �������� � �����. �� �������� ��������� ������ �� ���������
	�������� � ��������� � ����� �������� ��� ���.

	��������� ������� ��� ��� ����� ����� �������. �� ������� � ���� ���� �����

	*/

	// ����� ������
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_white_king = from;
		//cout << "������ " << "\n";
		//cout << "from= "<< from << "\n";
		//cout << "Pozicion_k_e[from]= "<< Pozicion_k_e[from] << "\n";


		if (stadia_b > rubikon)
		{
			//-----------------------------------------------------------
			// �������� ����� � ��� �������� ���
			// .........
			// A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

			// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
			// � ��������: 64 - 6 bit  128 - 7 bit 64 + 128 = 192 // �������� 6 � 7 ���

			//std::cout << "from= "<< (int)from << "\n";

			if (((p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] & 192) != 0) //  ����� ������� �� 6 ��� 7 �������
			&& ((from == G1) || (from == F1)) ) //  ����� ������ � ���� ��������� 0-0
			{
				eval = eval - PENALTY_CASTLING_O_O; // ����� �������� 
                //std::cout << "oo from= "<< (int)from << "\n";
				//std::cout << "eval= "<< eval << "\n";
			}

			// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
			// � ��������: 1 - 0 bit   2 - 1 bit
			// 1 + 2 = 3 // �������� 0,1 ����
			if (((p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] & 3) != 0) //  ����� ������� �� 6 ��� 7 �������
			&& ((from == B1) || (from == C1) || (from == D1))) //  ����� ������ � ���� ��������� 0-0-0
			{
				eval = eval - PENALTY_CASTLING_O_O_O; // ����� �������� 
				//std::cout << "ooo from= "<< (int)from << "\n";
				//std::cout << "eval= "<< eval << "\n";
			}
			//-----------------------------------------------------------

			eval = eval 
				+ pozicion_king_white[from] 
				- king_shield(from,1,p_bitboard,PENALTY_SHIELD)
				;

			//std::cout << "king from= "<< (int)from << "\n";
		}
		else
		{
			// ����� �������� ������ ������ ���������� � ������
			eval = eval + pozicion_king_white_end[from];
		}
	}

	//������ ������
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_black_king = from;

		if (stadia_w > rubikon)
		{

			//-----------------------------------------------------------
			// �������� ����� � ��� �������� ���
			// A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
			// .........

			// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
			// � ��������: 4611686018427387904 - 62 bit  9223372036854775808 - 63 bit 
			// 4611686018427387904 + 9223372036854775808 = 13835058055282163712  �������� 62 � 63 ���

			if (( (p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
			& 13835058055282163712) != 0 ) //  ����� ������� �� 6 ��� 7 �������
			&& ( (from == F8) || (from == G8))) //  ������ ������ � ���� ��������� 0-0
			{
				eval = eval + PENALTY_CASTLING_O_O; // ����� �������� 
			}

			// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
			// � ��������: 72057594037927936 - 56 bit   144115188075855872 - 57 bit
			// 72057594037927936 + 144115188075855872 = 216172782113783808  �������� 56,57 ����
			if (( (p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
			& 216172782113783808) != 0 ) //  ����� ������� �� 6 ��� 7 �������
			&& ( (from == B8) || (from == C8) || (from == D8))) //  ������ ������ � ���� ��������� 0-0-0
			{
				eval = eval + PENALTY_CASTLING_O_O_O; // ����� �������� 
			}
			//-----------------------------------------------------------

			eval = eval 
				- pozicion_king_black[from] 
				+ king_shield(from,0,p_bitboard,PENALTY_SHIELD)
				;

		}
		else
		{
			// ����� �������� ������ ������ ���������� � ������
			eval = eval - pozicion_king_black_end[from];
		}
	}
}

//	===================================================================
// ������ ������ �� ������������ 
inline void EVALUATE::pawn_pst
(
	const T_Bitboard * p_bitboard,// ������������� �����
	const unsigned __int8 from_white_king,// ������������ ������ ������
	const unsigned __int8 from_black_king,// ������������ ������� ������
	const unsigned __int16 stadia_w, // �������� ��� ����� � �����
	const unsigned __int16 stadia_b, // �������� ��� ����� � ������
	__int32 & eval //����������� ������
)
{
	// ����� ��� ����������� ��������� ����� 
	unsigned __int8 Structure_pawn_w[]=
	{
		0,0,0,0,0,0,0,0
	};

	unsigned __int8 Structure_pawn_b[]=
	{
		0,0,0,0,0,0,0,0
	};

	unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

	unsigned __int8 from; // ���������� ���� ������ ������ �����

	// ���������� �������� ����� ����� ������ ������� ������ ���� ���� ������
	// �.�. �� ���� ����� ����� ��� - ����� � ������ �� �����
	unsigned __int8 cnt_line_pawn_white = 0;
	unsigned __int8 cnt_line_pawn_black = 0;

	/*

	������� ��� ������� �������� � ��������.
	���� ��� ������ ������ - �� �������� � �����.

	�� ��������� ����� �����

	*/

	// �����-------------------------------------------------

	//�����

	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		if (stadia_b > rubikon)
		{
			
			if ((from_white_king == G1) || (from_white_king == H1))
			{// ���� ������� �������� ���������
				eval = eval + pozicion_pawn_king_in_oo_white[from];
			}
			else if ((from_white_king == A1) || (from_white_king == B1) 
			|| (from_white_king == C1))
			{// ���� ������� ������� ���������
				eval = eval + pozicion_pawn_king_in_ooo_white[from];
			}
			else
			{// ���� ��������� �� ����������
				eval = eval + pozicion_pawn_king_in_center_white[from];
			}
		}
		else
		{
			eval = eval + pozicion_pawn_white_end[from];
            //eval = eval + test[from];
			//cout << "from= "<< from << "\n";
			//cout << "test[from]= "<< test[from] << "\n";
			//cout << "from % 8= "<< from % 8 << "\n";
		}


		// ���������� ���������
		// � ������ ���� ����� ������ ��� ��������� �����
		if (!(e_rook_up_masks[from] & p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN]))
		{ 
			eval = eval + PERSPECTIVE_PAWNN;// 10
			cnt_line_pawn_white = 0;

			// ��� ������ ������� ���������
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_white = 1;

			// � ������ ���� ������ ����� ������ ��� ��������� �����
			if ((from % 8) != H1)
			{
				if (!(e_rook_up_masks[from + 1] 
				& p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN]))
				{ 
					eval = eval + PERSPECTIVE_PAWNN;
					cnt_line_pawn_white = cnt_line_pawn_white + 1;
				}
			}

			// � ������ ���� ����� ����� ������ ��� ��������� �����
			if ((from % 8) != A1)
			{
				if (!(e_rook_up_masks[from - 1] 
				& p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN]))
				{
					eval = eval + PERSPECTIVE_PAWNN;
					cnt_line_pawn_white = cnt_line_pawn_white + 1;
				}
			}

			// ��� ��� ����� ������� ��� ������ ���������.
			if (cnt_line_pawn_white == 2) eval = eval 
			+ pozicion_perspective_pawn_white_end[from]; 
		}

		//���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
		Structure_pawn_w[from % 8] = Structure_pawn_w[from % 8] + 1;
		//std::cout << "w from % 8= "<< from % 8 << "\n";
	}


// ������-------------------------------------------------

	//�����
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		if (stadia_w > rubikon)
		{
			
			if ((from_black_king == G8) || (from_black_king == H8))
			{// ���� ������� �������� ���������
				eval = eval - pozicion_pawn_king_in_oo_black[from];
			}
			else if ((from_black_king == A8) || (from_black_king == B8) 
			|| (from_black_king == C8))
			{// ���� ������� ������� ���������
				eval = eval - pozicion_pawn_king_in_ooo_black[from];
			}
			else
			{// ���� ��������� �� ����������
				eval = eval - pozicion_pawn_king_in_center_black[from];
			}
		}
		else
		{
			eval = eval - pozicion_pawn_black_end[from];
		}


		// ���������� ���������
		// � ������ ���� ����� ������ ��� ��������� �����
		if (!(e_rook_down_masks[from] & p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN]))
		{ 
			eval = eval - PERSPECTIVE_PAWNN;
			cnt_line_pawn_black = 0;

			// ��� ������ ������� ���������
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_black = 1;

			// � ������ ���� ������ ����� ������ ��� ��������� �����
			if ((from % 8) != H1)
			{
				if (!(e_rook_down_masks[from + 1] 
				& p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN]))
				{
					eval = eval - PERSPECTIVE_PAWNN;
					cnt_line_pawn_black = cnt_line_pawn_black + 1;
				}
			}

			// � ������ ���� ����� ����� ������ ��� ��������� �����
			if ((from % 8) != A1)
			{
				if (!(e_rook_down_masks[from - 1] 
				& p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN]))
				{
					eval = eval - PERSPECTIVE_PAWNN;
					cnt_line_pawn_black = cnt_line_pawn_black + 1;
				}
			}

			// ��� ��� ����� ������� ��� ������ ���������.
			if (cnt_line_pawn_black == 2) 
				eval = eval - pozicion_perspective_pawn_black_end[from];
		}

		// ���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
		Structure_pawn_b[from % 8] = Structure_pawn_b[from % 8] + 1;
		//std::cout << "b from % 8= "<< from % 8 << "\n";
	}

	// �������� �� ��������� �����
	for (int ii = 0; ii < 8; ii++)
	{
		if (Structure_pawn_w[ii] > 1) eval = eval - PENALTY_PAWNN; // ������ ����� 5 ��� 20

		if (Structure_pawn_b[ii] > 1) eval = eval + PENALTY_PAWNN;

		//std::cout << "ii = "<< ii << "\n";
		//std::cout << "Structure_pawn_w[ii]= "<< Structure_pawn_w[ii] << "\n";
		//std::cout << "Structure_pawn_b[ii]= "<< Structure_pawn_b[ii] << "\n";
	}

}

//	===================================================================
// ������ ������ �� ������������ ������, �����, ������, �����
// ������ ��������������� ������������ ����� ��������
// ������ ����� ����� ������ ������� ����� ��������
inline void EVALUATE::PST
(
	const T_Bitboard * p_bitboard,// ������������� �����
	const unsigned __int8 from_white_king,// ������������ ������ ������
	const unsigned __int8 from_black_king,// ������������ ������� ������
	unsigned __int8 & at_white_king, // ���������� ���������� ����� ������ ������ ������
	unsigned __int8 & at_black_king, // ���������� ���������� ����� ������ ������� ������
	__int32 & eval //����������� ������
)
{
	unsigned __int8 from; // ���������� ���� ������ ������ �����
	unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

	// ��� ���������� ���������� ����� ��������
	//int dx;// ��������� �� �����������
	//int dy;// ��������� �� ���������
	//int distanse;// ������ ����������
	//const int kd = 7;// ����������� ���������

	/*
	������ � ������ ����� �� ������ ������������ ������ ����� ���������
	��������(���� ������� ��������� :)) �� �������� � ������� ����

	���� ��������� � ������
	*/

	// �����-----------------------------------------------------------------
	//from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN];
	//
	//while (from_mask != 0)
	//{
	//	from = bit_scan_forward_win32(from_mask);
	//	from_mask = from_mask & (from_mask - 1);

	//	//dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
	//	//dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
	//	//distanse = kd/(dx + dy);
	//	//distanse = 0;

	//	eval = eval + CONTROL_SQUARE
	//	* queen_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//		from, from_black_king, at_black_king);
	//}

	//�����----------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		eval = eval + CONTROL_SQUARE 
		* rook_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, from, 
				from_black_king, at_black_king);

		// cout << "Rook_amoun_white(p_bitboard)= "<< Rook_amoun(p_bitboard, from) << "\n";
	}

	//����---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval + pozicion_bishop_white[from] 
			//+ CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
				//from, from_black_king, at_black_king)
					;

		//cout << "Bishop_amoun(p_bitboard, from)= "<< Bishop_amoun(p_bitboard, from) << "\n";
	}

	//���� ----------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		eval = eval + pozicion_knigh_white[from] 
			//+ CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
				//from, from_black_king, at_black_king)
					;
	}

/////////////////////////////////////

	//�����-----------------------------------------------------------------
	//from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN];

	//while (from_mask != 0)
	//{
	//	from = bit_scan_forward_win32(from_mask);
	//	from_mask = from_mask & (from_mask-1);

	//	//dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
	//	//dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
	//	//distanse = kd/(dx + dy);
	//	//distanse = 0;

	//	eval = eval - CONTROL_SQUARE 
	//		* queen_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
	//			from_white_king, at_white_king);

	//}

	//�����----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval - CONTROL_SQUARE 
			* rook_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
				from_white_king, at_white_king);

	}

	//����---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval - pozicion_bishop_black[from] 
			//- CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece,p_bitboard->all_black_piece, 
				//from, from_white_king, at_white_king)
					;

	}

	//���� ----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval - pozicion_knigh_black[from] 
			//- CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, 
				//from, from_white_king, at_white_king)
					;

	}

	// ����� �� ����� ������
	//eval = eval + ATAK_KING * (at_black_king - at_white_king);


}

//	===================================================================
// ����� �� ���������� ���� ����� �������. 
// ���� ��� ��������� ������ �� ����������� ����� 
// (������ ���� ��������� ����� �������, �� ��� ����� ����� :))
inline __int32 EVALUATE::king_shield
(
	unsigned __int8 king_from,// ��������� ������ (from)
	unsigned __int8 king_color,// ���� ������
	const T_Bitboard * p_bitboard,// ������������� �����
	const unsigned __int8 SHIELD// ����� �� ��������� ������
)
{
	unsigned __int8 value_king_shield = 0;

	if (king_color == 1)
	{
		// ������� �� ������ ����� �� ������
		if (p_bitboard->white_name_from_coord[king_from + 7] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 0)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}

		// ������� �� ������ ����� �����
		if (p_bitboard->white_name_from_coord[king_from + 8] != BITBOARDGENERATOR::N_PAWN) 
		{
			value_king_shield = value_king_shield + SHIELD;
		}

		// ������� �� ������ ������ �����
		if (p_bitboard->white_name_from_coord[king_from + 9] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 7)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}
	}
	else
	{
		// ������� �� ������ ���� �� ������
		if (p_bitboard->black_name_from_coord[king_from - 7] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 63)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}

		// ������� �� ������  ����� ����
		if (p_bitboard->black_name_from_coord[king_from - 8] != BITBOARDGENERATOR::N_PAWN)
		{
			value_king_shield = value_king_shield + SHIELD;
		}

		// ������� �� ������ ������ ����
		if (p_bitboard->black_name_from_coord[king_from - 9] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 56)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}
	}

	return value_king_shield;
}

//	===================================================================
// ������� �������� ������� � ��������� ������� ���������. ����� - ������
__int32 EVALUATE::public_eval_difference_material
(
	const T_Bitboard * p_bitboard// ������������� �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	__int32 value_difference_material = 0;

	// ����� ���������

	// ������----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_difference_material = value_difference_material + KING;

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0){
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + PAWN;
	}

	///////////////////////////////////

	// ������ ����������(����������)

	// ������----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_difference_material = value_difference_material - KING;

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - PAWN;
	}

	//--------------------------------------------------------------------------
	// ��� ������ ������� ������� ������. �� ����� �� ��� �� ���������
	//int eval = 0;
	//
	//for (int i = 0; i < 64; i++)
	//{
	//	eval = eval + kod[p_bitboard->white_name_from_coord[i]];
	//	eval = eval - kod[p_bitboard->black_name_from_coord[i]];
	//}
	//
	//if (eval != value_difference_material)
	//{
	//	cout << "KARAUUUUUUUUUUUUUUL"<< "\n";
	//}
	//--------------------------------------------------------------------------

	return value_difference_material;
}

//	===================================================================
// ������� �������� ������� � ��������� �������� ��� �����
__int32 EVALUATE::public_eval_white_material
(
	const T_Bitboard * p_bitboard// ������������� �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	__int32 value_white_material = 0;

	// ����� ���������

	// ������----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_white_material = value_white_material + KING;

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0){
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + PAWN;
	}

	return value_white_material;
}

//	===================================================================
// ������� �������� ������� � ��������� �������� ��� ������.
__int32 EVALUATE::public_eval_black_material
(
	const T_Bitboard * p_bitboard// ������������� �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	__int32 value_black_material = 0;

	// ������ ��������� ����

	// ������----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_black_material = value_black_material + KING;

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + PAWN;
	}

	return value_black_material;
}


//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVALUATE::rook_amoun
(
	unsigned __int64  all_piece,// ��� ������
	unsigned __int64  piece,// ���� ������
	unsigned __int8 from,// ������������ �����
	unsigned __int8 from_king,// ������������ ������
	unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
)
{
	unsigned __int64 blocking_square_mask = 0; // ����� ������������� �����
	unsigned __int64 move_mask = 0; // ����� �����
	unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

	__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
	unsigned __int8 number = 0; // 
	unsigned __int8 number_n = 0; // 

	// ���� �����----------------------------------------------------------------------------------

	// ������� � ������� ��������� ������������

	// ��� �����----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_up_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------

	// ��� �����----------------------------------------------------
	//blocking_square_mask = all_piece & e_rook_left_masks[from];
	//
	//if (blocking_square_mask != 0)
	//{ 
	//	blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
	//	move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
	//}
	//else
	//{
	//	move_mask = e_rook_left_masks[from];
	//}

	//move_mask_sum = move_mask_sum | move_mask;
	//--------------------------------------------------------------

	// ��� ����-----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_down_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;
	//--------------------------------------------------------------


	// ��� ������---------------------------------------------------
	//blocking_square_mask = all_piece & e_rook_right_masks[from];

	//if (blocking_square_mask != 0)
	//{ 
	//	blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
	//	move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
	//}
	//else
	//{
	//	move_mask = e_rook_right_masks[from];
	//}

	//move_mask_sum = move_mask_sum | move_mask;
	//--------------------------------------------------------------

	//if ((e_king_moves_masks[from_king] & move_mask_sum) != 0) at_cnt = at_cnt + 1 ;

	// ������� ������ ����� �����
	move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

	// ��������� ������ �����
	while (move_mask_sum != 0)
	{
		move_mask_sum = move_mask_sum & (move_mask_sum - 1);
		number_n = number_n + 1;
	}

	//	number_n = popcnt(move_mask_sum);

		//std::cout << "from= " << (int)from << "\n";
		//std::cout << "popcnt(move_mask_sum)= " << popcnt(move_mask_sum) << "\n";
		//std::cout << "number_n= " << (int)number_n << "\n";


	return number_n;

}

//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVALUATE::bishop_amoun
(
	unsigned __int64  all_piece,// ��� ������
	unsigned __int64  piece,// ���� ������
	unsigned __int8 from,// ������������ �����
	unsigned __int8 from_king,// ������������ ������
	unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
)
{
	unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
	unsigned __int64 move_mask = 0; // ����� �����
	unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

	__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
	unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
	unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

	// ���� �����---------------------------------------------------------------------------------- 

	// ������� � ������� ��������� ������������

	// ��� ����� - ������
	blocking_square_mask = all_piece & e_bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// ��� ����� - �����
	blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// ��� ���� - �����
	blocking_square_mask = all_piece & e_bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// ��� ���� - ������
	blocking_square_mask = all_piece & e_bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	if ((e_king_moves_masks[from_king] & move_mask_sum) != 0) 
		at_cnt = at_cnt + 1 ;

	// ������� ������ ����� �����
	move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

	// ��������� ������ �����
	while (move_mask_sum != 0)
	{
		move_mask_sum = move_mask_sum & (move_mask_sum - 1);
		number_n = number_n + 1;
	}

	return number_n;

}

//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVALUATE::queen_amoun
(
	unsigned __int64  all_piece,// ��� ������
	unsigned __int64  piece,// ���� ������
	unsigned __int8 from,// ������������ �����
	unsigned __int8 from_king,// ������������ ������
	unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
)
{
	unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
	unsigned __int64 move_mask = 0; // ����� �����
	unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

	__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
	unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
	unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

	// ���� �����----------------------------------------------------------------------------------

	// ������� � ������� ��������� ������������

	// ��� �����----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_up_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// ��� �����----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// ��� ����-----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_down_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// ��� ������---------------------------------------------------
	blocking_square_mask = all_piece & e_rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// ���� �����---------------------------------------------------------------------------------- 

	// ������� � ������� ��������� ������������

	// ��� ����� - ������
	blocking_square_mask = all_piece & e_bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_right_masks[from];
	}
 
	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// ��� ����� - �����
	blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// ��� ���� - �����
	blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// ��� ���� - ������
	blocking_square_mask = all_piece & e_bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------
	if ((e_king_moves_masks[from_king] & move_mask_sum) != 0) at_cnt = at_cnt + 1 ;

	// ������� ������ ����� �����
	move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

	// ��������� ������ �����
	while (move_mask_sum != 0)
	{
		move_mask_sum = move_mask_sum & (move_mask_sum - 1);
		number_n = number_n + 1;
	}

	//if (number_n != number) cout << "������!!!!!!!!!! " << "\n";

	return number_n;

}


//	===================================================================
// ������������ ��� ��������� ���� ����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVALUATE::knigt_amoun
(
	unsigned __int64  all_piece,// ��� ������
	unsigned __int64  piece,// ���� ������
	unsigned __int8 from,// ������������ �����
	unsigned __int8 from_king,// ������������ ������
	unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
)
{
	unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
	unsigned __int64 move_mask = 0; // ����� �����
	unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

	__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
	unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
	unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

	// ������� ���� �� ���� ������
	// � ����� ������� �� �� ���� �����
	move_mask = e_knigt_moves_masks[from] ^ (e_knigt_moves_masks[from] & piece) ;

	if ((e_king_moves_masks[from_king] & move_mask) != 0 ) at_cnt = at_cnt + 1 ;

	// ��������� ������ �����---------------------------------------
	while (move_mask != 0)
	{
		move_mask = move_mask & (move_mask-1);
		number = number + 1;
	}

	return number;
}


//	===================================================================
// ���� ������ ������������� ���(�.�. ����� ������)
__inline __int32 EVALUATE::bit_scan_forward_win32
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
		test  eax, eax

		/*
		������� �������, ���� ������� ��������
		jz ���� ZF= 1
		*/
		jz  grown_up

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
		jmp  jump_end
		grown_up:  bsf  eax, dword ptr analyzed_number[4]

		/*
		��������
		*/
		add  eax, 20h
		jump_end:
	}
}

//	===================================================================
// ���� ��������� ������������� ���(�.�. ����� �����)
__inline __int32 EVALUATE::bit_scan_reverse_win32
(
	unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
)
{ 
	_asm 
	{ 
		mov  eax, dword ptr analyzed_number[4]
		test  eax, eax
		jz  younger

		/*
		������� bsr ��������� ���� ��� ������� ����� � ������� ����, ������� 1. 
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
		younger:  bsr  eax, dword ptr analyzed_number[0]
		jump_end:
	}
}


//	===================================================================
// ������� ���������� ����
//inline __int32 EVALUATE::popcnt(unsigned __int64 analyzed_number)
//{ 
//	_asm
//	{
//		mov  ecx, dword ptr analyzed_number[4]
//		xor  eax, eax      
//		test  ecx, ecx
//		jz  label_2
//
//		label_1: 
//			lea  ebx, [ecx-1]
//			inc  eax
//			and  ecx, ebx
//			jnz  label_1
//
//			label_2:
//				mov  ecx, dword ptr analyzed_number[0]
//				test  ecx, ecx
//				jz  label_4
//
//				label_3:
//					lea  ebx, [ecx-1]
//					inc  eax     
//					and  ecx, ebx
//					jnz  label_3
//					label_4:
//	}
//}

