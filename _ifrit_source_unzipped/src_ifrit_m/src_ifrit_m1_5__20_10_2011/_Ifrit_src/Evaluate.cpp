
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 24.02.2009 03.07.2010 19.09.2011 26.09.2011 09.10.2011

#include <fstream>
#include <iostream>

#include "List_const.h"
#include "Position_const.h"
#include "Evaluate.h"
#include "Evaluate_const.h"
#include "Generator_moves.h"
#include "Generator_moves_const.h"
#include "Test_chess_bitboard.h" // test

/*

���� ���� ��� ������ ���� ������.
���� ����� ������ ��������� ��������� (������� �� WildCat(����� ��������)
1. ���������                -195 ��
2. �������� �����           -173 ��
4. �����������              - 78 ��
3. �������� ���             - 69 ��
5. ������� ������           - 29 ?
6. �������� ���������       - 25 ?
7. ����������� ����         - 18 ?
8. ����� �� ������          -  9 ��
9. ��������                 -  0 ?

�������� ���������� ����:

���: �������� ��
10. ���������               - 19 ?

*/

//	===================================================================
// ������� �������� ������� � ��������� �� ����������. ������� ����������� �������
__int32 Evaluate::eval_state
(
	struct Position & position// ������������� �����
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

	unsigned __int64 mask_mobility_from_coord[MAX_COORDINATE];

	//� ��������� ������� �� ����� 400 + 400 + 600 + 1200 = 2600 (��� ����� �������)
	//                             2*400 + 2*400 + 2*600 + 1200 
	//                             800   + 800   + 1200  + 1200 = 4000 (����� ��� ���� ������) 

	//stadia_w = 2600; // �������� ��� ����� � �����
	//stadia_b = 2600; // �������� ��� ����� � ������

	stadia_w = position.value_white_material;
	stadia_b = position.value_black_material;

	/*
	������� ��� ������� �������� � ��������.
	���� ��� ������ ������ - �� �������� � �����.
	*/

	if ( (stadia_w > RUBIKON_ENDGAME) && (stadia_b > RUBIKON_ENDGAME) )
	{

		// ����������� ������
		king_pst_opening(position,from_white_king, from_black_king, stadia_w, stadia_b, eval);

		// ����������� �����
		pawn_pst_opening(position,from_white_king, from_black_king, stadia_w, stadia_b, eval);

		// ����������� �����, �����, ����, ����
		// ����� ������������ 
		PST_opening(mask_mobility_from_coord,position,from_white_king, 
			from_black_king, at_white_king, at_black_king, eval);
	}
	else
	{
		// ����������� ������
		king_pst_endgame(position,from_white_king, from_black_king, stadia_w, stadia_b, eval);

		// ����������� �����
		pawn_pst_endgame(position,from_white_king, from_black_king, stadia_w, stadia_b, eval);

		// ����������� �����, �����, ����, ����
		// ����� ������������ 
		//PST_endgame(mask_mobility_from_coord,position,from_white_king, 
		//	from_black_king, at_white_king, at_black_king, eval);

		PST_opening(mask_mobility_from_coord,position,from_white_king, 
			from_black_king, at_white_king, at_black_king, eval);
	}

	return (__int32)(eval/32);
}

//	===================================================================
//
inline void Evaluate::king_pst_opening
(
	struct Position & position,// ������������� �����
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
	from_mask = position.white_piece[N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_white_king = from;
		//cout << "������ " << "\n";
		//cout << "from= "<< from << "\n";
		//cout << "Pozicion_k_e[from]= "<< Pozicion_k_e[from] << "\n";

		//-----------------------------------------------------------
		// �������� ����� � ��� �������� ���
		// .........
		// A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

		// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
		// � ��������: 64 - 6 bit  128 - 7 bit 64 + 128 = 192 // �������� 6 � 7 ���

		//std::cout << "from= "<< (int)from << "\n";

		if (((position.white_piece[N_ROOK] & 192) != 0) //  ����� ������� �� 6 ��� 7 �������
		&& ((from == G1) || (from == F1)) ) //  ����� ������ � ���� ��������� 0-0
		{
			eval = eval - PENALTY_CASTLING_O_O; // ����� �������� 
			//std::cout << "oo from= "<< (int)from << "\n";
			//std::cout << "eval= "<< eval << "\n";
		}

		// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
		// � ��������: 1 - 0 bit   2 - 1 bit
		// 1 + 2 = 3 // �������� 0,1 ����
		if (((position.white_piece[N_ROOK] & 3) != 0) //  ����� ������� �� 0 ��� 1 �������
		&& ((from == B1) || (from == C1) || (from == D1))) //  ����� ������ � ���� ��������� 0-0-0
		{
			eval = eval - PENALTY_CASTLING_O_O_O; // ����� �������� 
			//std::cout << "ooo from= "<< (int)from << "\n";
			//std::cout << "eval= "<< eval << "\n";
		}
		//-----------------------------------------------------------

		eval = eval 
			+ pozicion_king_white_opening[from] 
			//- king_shield(position,from,1,PENALTY_SHIELD)
			;
	}

	//������ ������
	from_mask = position.black_piece[N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_black_king = from;

		//-----------------------------------------------------------
		// �������� ����� � ��� �������� ���
		// A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
		// .........

		// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
		// � ��������: 4611686018427387904 - 62 bit  9223372036854775808 - 63 bit 
		// 4611686018427387904 + 9223372036854775808 = 13835058055282163712  �������� 62 � 63 ���

		if (( (position.black_piece[N_ROOK] 
		& 13835058055282163712) != 0 ) //  ����� ������� �� 6 ��� 7 �������
		&& ( (from == F8) || (from == G8))) //  ������ ������ � ���� ��������� 0-0
		{
			eval = eval + PENALTY_CASTLING_O_O; // ����� �������� 
		}

		// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
		// � ��������: 72057594037927936 - 56 bit   144115188075855872 - 57 bit
		// 72057594037927936 + 144115188075855872 = 216172782113783808  �������� 56,57 ����
		if (( (position.black_piece[N_ROOK] 
		& 216172782113783808) != 0 ) //  ����� ������� �� 0 ��� 1 �������
		&& ( (from == B8) || (from == C8) || (from == D8))) //  ������ ������ � ���� ��������� 0-0-0
		{
			eval = eval + PENALTY_CASTLING_O_O_O; // ����� �������� 
		}
		//-----------------------------------------------------------

		eval = eval 
			- pozicion_king_black_opening[from] 
			//+ king_shield(position,from,0,PENALTY_SHIELD)
			;
	}
}


//	===================================================================
//
inline void Evaluate::king_pst_endgame
(
	struct Position & position,// ������������� �����
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
	from_mask = position.white_piece[N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_white_king = from;
		//cout << "������ " << "\n";
		//cout << "from= "<< from << "\n";
		//cout << "Pozicion_k_e[from]= "<< Pozicion_k_e[from] << "\n";

		// ����� �������� ������ ������ ���������� � ������
		eval = eval + pozicion_king_white_endgame[from];
	}

	//������ ������
	from_mask = position.black_piece[N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_black_king = from;

		// ����� �������� ������ ������ ���������� � ������
		eval = eval - pozicion_king_black_endgame[from];
	}
}

//	===================================================================
// ������ ������ �� ������������ 
inline void Evaluate::pawn_pst_opening
(
	struct Position & position,// ������������� �����
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


	// �����-------------------------------------------------

	//�����

	from_mask = position.white_piece[N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);
	
		if ((from_white_king == G1) || (from_white_king == H1) || (from_white_king == F1))
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

		// ���������� ���������
		// � ������ ���� ����� ������ ��� ��������� �����
		if (!(rook_up_masks[from] & position.black_piece[N_PAWN]))
		{ 
			eval = eval + PERSPECTIVE_PAWNN;// 10
			cnt_line_pawn_white = 0;

			// ��� ������ ������� ���������
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_white = 1;

			// � ������ ���� ������ ����� ������ ��� ��������� �����
			if ((from % 8) != H1)
			{
				if (!(rook_up_masks[from + 1] 
				& position.black_piece[N_PAWN]))
				{ 
					eval = eval + PERSPECTIVE_PAWNN;
					cnt_line_pawn_white = cnt_line_pawn_white + 1;
				}
			}

			// � ������ ���� ����� ����� ������ ��� ��������� �����
			if ((from % 8) != A1)
			{
				if (!(rook_up_masks[from - 1] 
				& position.black_piece[N_PAWN]))
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
	from_mask = position.black_piece[N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);
			
		if ((from_black_king == G8) || (from_black_king == H8) || (from_black_king == F8))
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

		// ���������� ���������
		// � ������ ���� ����� ������ ��� ��������� �����
		if (!(rook_down_masks[from] & position.white_piece[N_PAWN]))
		{ 
			eval = eval - PERSPECTIVE_PAWNN;
			cnt_line_pawn_black = 0;

			// ��� ������ ������� ���������
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_black = 1;

			// � ������ ���� ������ ����� ������ ��� ��������� �����
			if ((from % 8) != H1)
			{
				if (!(rook_down_masks[from + 1] 
				& position.white_piece[N_PAWN]))
				{
					eval = eval - PERSPECTIVE_PAWNN;
					cnt_line_pawn_black = cnt_line_pawn_black + 1;
				}
			}

			// � ������ ���� ����� ����� ������ ��� ��������� �����
			if ((from % 8) != A1)
			{
				if (!(rook_down_masks[from - 1] 
				& position.white_piece[N_PAWN]))
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
// ������ ������ �� ������������ 
inline void Evaluate::pawn_pst_endgame
(
	struct Position & position,// ������������� �����
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

	from_mask = position.white_piece[N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval + pozicion_pawn_white_end[from];
		//eval = eval + test[from];
		//cout << "from= "<< from << "\n";
		//cout << "test[from]= "<< test[from] << "\n";
		//cout << "from % 8= "<< from % 8 << "\n";

		// ���������� ���������
		// � ������ ���� ����� ������ ��� ��������� �����
		if (!(rook_up_masks[from] & position.black_piece[N_PAWN]))
		{ 
			eval = eval + PERSPECTIVE_PAWNN;// 10
			cnt_line_pawn_white = 0;

			// ��� ������ ������� ���������
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_white = 1;

			// � ������ ���� ������ ����� ������ ��� ��������� �����
			if ((from % 8) != H1)
			{
				if (!(rook_up_masks[from + 1] 
				& position.black_piece[N_PAWN]))
				{ 
					eval = eval + PERSPECTIVE_PAWNN;
					cnt_line_pawn_white = cnt_line_pawn_white + 1;
				}
			}

			// � ������ ���� ����� ����� ������ ��� ��������� �����
			if ((from % 8) != A1)
			{
				if (!(rook_up_masks[from - 1] 
				& position.black_piece[N_PAWN]))
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
	from_mask = position.black_piece[N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval - pozicion_pawn_black_end[from];

		// ���������� ���������
		// � ������ ���� ����� ������ ��� ��������� �����
		if (!(rook_down_masks[from] & position.white_piece[N_PAWN]))
		{ 
			eval = eval - PERSPECTIVE_PAWNN;
			cnt_line_pawn_black = 0;

			// ��� ������ ������� ���������
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_black = 1;

			// � ������ ���� ������ ����� ������ ��� ��������� �����
			if ((from % 8) != H1)
			{
				if (!(rook_down_masks[from + 1] 
				& position.white_piece[N_PAWN]))
				{
					eval = eval - PERSPECTIVE_PAWNN;
					cnt_line_pawn_black = cnt_line_pawn_black + 1;
				}
			}

			// � ������ ���� ����� ����� ������ ��� ��������� �����
			if ((from % 8) != A1)
			{
				if (!(rook_down_masks[from - 1] 
				& position.white_piece[N_PAWN]))
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
inline void Evaluate::PST_opening
(
	unsigned __int64 mask_mobility_from_coord[MAX_COORDINATE],
	struct Position & position,// ������������� �����
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

	at_white_king = 0; // ���������� ���������� ����� ������ ������ ������
	at_black_king = 0; // ���������� ���������� ����� ������ ������� ������

	// ������� ����� ���� ������
	unsigned __int64 white_king_move_mask = king_moves_masks[from_white_king];

	// ������� ����� ���� ������
	unsigned __int64 black_king_move_mask = king_moves_masks[from_black_king];

	// �����-----------------------------------------------------------------
	from_mask = position.white_piece[N_QUEEN];
	
	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		//dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
		//dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
		//distanse = kd/(dx + dy);
		//distanse = 0;

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		unsigned __int64 move_mask_b = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask | move_mask_b;
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval + pozicion_queen_white_opening[from]
		+ CONTROL_SQUARE_QUEEN_OPENING * popcnt(move_mask) // max r14 + b13 = q27  n8 k8
		////+ test[from] 
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_QUEEN;
			//at_black_king = at_black_king + 1 ;
		}

	}

	//�����----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval + pozicion_rook_white_opening[from] 
		+ CONTROL_SQUARE_ROOK_OPENING * popcnt(move_mask)
		//+ test[from] 
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_ROOK;
			//at_black_king = at_black_king + 1 ;
		}

	}

	//����---------------------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval + pozicion_bishop_white_opening[from] 
		+ CONTROL_SQUARE_BISHOP_OPENING * popcnt(move_mask)
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_BISHOP;
			//at_black_king = at_black_king + 1 ;
		}

	}

	//���� ----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		unsigned __int64 move_mask = knigt_moves_masks[from]; // ����� ����
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval + pozicion_knigh_white_opening[from] 
		+ CONTROL_SQUARE_KNIGHT_OPENING * popcnt(move_mask)
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_KNIGHT;
			//at_black_king = at_black_king + 1 ;
		}

	}

/////////////////////////////////////

	//�����-----------------------------------------------------------------
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		//dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
		//dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
		//distanse = kd/(dx + dy);
		//distanse = 0;

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		unsigned __int64 move_mask_b = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask | move_mask_b;
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval  - pozicion_queen_black_opening[from] 
		- CONTROL_SQUARE_QUEEN_OPENING * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_QUEEN;
			//at_white_king = at_white_king + 1 ;
		}

	}

	//�����----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval - pozicion_rook_black_opening[from]   
		- CONTROL_SQUARE_ROOK_OPENING * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_ROOK;
			//at_white_king = at_white_king + 1 ;
		}

	}

	//����---------------------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval - pozicion_bishop_black_opening[from] 
		- CONTROL_SQUARE_BISHOP_OPENING * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_BISHOP;
			//at_white_king = at_white_king + 1 ;
		}

	}

	//���� ----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = knigt_moves_masks[from]; // ����� ����
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval - pozicion_knigh_black_opening[from] 
		- CONTROL_SQUARE_KNIGHT_OPENING * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_KNIGHT;
			//at_white_king = at_white_king + 1 ;
		}

	}
}


//	===================================================================
// ������ ������ �� ������������ ������, �����, ������, �����
// ������ ��������������� ������������ ����� ��������
// ������ ����� ����� ������ ������� ����� ��������
inline void Evaluate::PST_endgame
(
	unsigned __int64 mask_mobility_from_coord[MAX_COORDINATE],
	struct Position & position,// ������������� �����
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

	at_white_king = 0; // ���������� ���������� ����� ������ ������ ������
	at_black_king = 0; // ���������� ���������� ����� ������ ������� ������

	// ������� ����� ���� ������
	unsigned __int64 white_king_move_mask = king_moves_masks[from_white_king];

	// ������� ����� ���� ������
	unsigned __int64 black_king_move_mask = king_moves_masks[from_black_king];

	// �����-----------------------------------------------------------------
	from_mask = position.white_piece[N_QUEEN];
	
	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		//dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
		//dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
		//distanse = kd/(dx + dy);
		//distanse = 0;

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		unsigned __int64 move_mask_b = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask | move_mask_b;
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval + pozicion_queen_white_endgame[from]
		+ CONTROL_SQUARE_QUEEN_ENDGAME * popcnt(move_mask) // max r14 + b13 = q27  n8 k8
		////+ test[from] 
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_QUEEN;
			//at_black_king = at_black_king + 1 ;
		}

	}

	//�����----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval //+ pozicion_rook_white_endgame[from] 
		+ CONTROL_SQUARE_ROOK_ENDGAME * popcnt(move_mask)
		//+ test[from] 
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_ROOK;
			//at_black_king = at_black_king + 1 ;
		}

	}

	//����---------------------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval + pozicion_bishop_white_endgame[from] 
		+ CONTROL_SQUARE_BISHOP_ENDGAME * popcnt(move_mask)
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_BISHOP;
			//at_black_king = at_black_king + 1 ;
		}

	}

	//���� ----------------------------------------------------------------------------------
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		unsigned __int64 move_mask = knigt_moves_masks[from]; // ����� ����
		move_mask = move_mask & (~position.all_white_piece);

		eval = eval + pozicion_knigh_white_endgame[from] 
		+ CONTROL_SQUARE_KNIGHT_ENDGAME * popcnt(move_mask)
		;

		if ( (move_mask & black_king_move_mask) != 0 )
		{
			eval = eval + ATAK_KING_KNIGHT;
			//at_black_king = at_black_king + 1 ;
		}

	}

/////////////////////////////////////

	//�����-----------------------------------------------------------------
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		//dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
		//dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
		//distanse = kd/(dx + dy);
		//distanse = 0;

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		unsigned __int64 move_mask_b = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask | move_mask_b;
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval  - pozicion_queen_black_endgame[from] 
		- CONTROL_SQUARE_QUEEN_ENDGAME * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_QUEEN;
			//at_white_king = at_white_king + 1 ;
		}

	}

	//�����----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = Generator_moves::get_rook_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval //- pozicion_rook_black_endgame[from]   
		- CONTROL_SQUARE_ROOK_ENDGAME * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_ROOK;
			//at_white_king = at_white_king + 1 ;
		}

	}

	//����---------------------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = Generator_moves::get_bishop_attacks_masks_magic(from,position.all_piece);
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval - pozicion_bishop_black_endgame[from] 
		- CONTROL_SQUARE_BISHOP_ENDGAME * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_BISHOP;
			//at_white_king = at_white_king + 1 ;
		}

	}

	//���� ----------------------------------------------------------------------------------
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from = Generator_moves::bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		unsigned __int64 move_mask = knigt_moves_masks[from]; // ����� ����
		move_mask = move_mask & (~position.all_black_piece);

		eval = eval - pozicion_knigh_black_endgame[from] 
		- CONTROL_SQUARE_KNIGHT_ENDGAME * popcnt(move_mask)
		;

		if ( (move_mask & white_king_move_mask) != 0 )
		{
			eval = eval - ATAK_KING_KNIGHT;
			//at_white_king = at_white_king + 1 ;
		}

	}
}

//	===================================================================
// ����� �� ���������� ���� ����� �������. 
// ���� ��� ��������� ������ �� ����������� ����� 
// (������ ���� ��������� ����� �������, �� ��� ����� ����� :))
inline __int32 Evaluate::king_shield
(
	struct Position & position,// ������������� �����
	unsigned __int8 king_from,// ��������� ������ (from)
	unsigned __int8 king_color,// ���� ������
	const unsigned __int8 SHIELD// ����� �� ��������� ������
)
{
	unsigned __int8 value_king_shield = 0;

	if (king_color == 1)
	{
		// ������� �� ������ ����� �� ������
		if (position.white_name_from_coord[king_from + 7] != N_PAWN)
		{
			if (king_from != 0)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}

		// ������� �� ������ ����� �����
		if (position.white_name_from_coord[king_from + 8] != N_PAWN) 
		{
			value_king_shield = value_king_shield + SHIELD;
		}

		// ������� �� ������ ������ �����
		if (position.white_name_from_coord[king_from + 9] != N_PAWN)
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
		if (position.black_name_from_coord[king_from - 7] != N_PAWN)
		{
			if (king_from != 63)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}

		// ������� �� ������  ����� ����
		if (position.black_name_from_coord[king_from - 8] != N_PAWN)
		{
			value_king_shield = value_king_shield + SHIELD;
		}

		// ������� �� ������ ������ ����
		if (position.black_name_from_coord[king_from - 9] != N_PAWN)
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
// ������� �������� ������� � ��������� ������� ���������. ����� ����� ������
__int32 Evaluate::eval_difference_material
(
	struct Position & position// ������������� �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	__int32 value_difference_material = 0;

	// ����� ���������

	// ������----------------------------------------------------------------
	from_mask = position.white_piece[N_KING];

	if (from_mask != 0) value_difference_material = value_difference_material + KING;

	// �����-----------------------------------------------------------------
	from_mask = position.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = position.white_piece[N_PAWN];

	while (from_mask != 0){
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + PAWN;
	}

	///////////////////////////////////

	// ������ ����������(����������)

	// ������----------------------------------------------------------------
	from_mask = position.black_piece[N_KING];

	if (from_mask != 0) value_difference_material = value_difference_material - KING;

	// �����-----------------------------------------------------------------
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = position.black_piece[N_PAWN];

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
	//	eval = eval + kod[position.white_name_from_coord[i]];
	//	eval = eval - kod[position.black_name_from_coord[i]];
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
__int32 Evaluate::eval_white_material
(
	struct Position & position// ������������� �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	__int32 value_white_material = 0;

	// ����� ���������

	// ������----------------------------------------------------------------
	from_mask = position.white_piece[N_KING];

	if (from_mask != 0) value_white_material = value_white_material + KING;

	// �����-----------------------------------------------------------------
	from_mask = position.white_piece[N_QUEEN];

	while (from_mask != 0)
	{
		// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = position.white_piece[N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = position.white_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = position.white_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = position.white_piece[N_PAWN];

	while (from_mask != 0){
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + PAWN;
	}

	return value_white_material;
}

//	===================================================================
// ������� �������� ������� � ��������� �������� ��� ������.
__int32 Evaluate::eval_black_material
(
	struct Position & position// ������������� �����
)
{
	unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
	__int32 value_black_material = 0;

	// ������ ��������� ����

	// ������----------------------------------------------------------------
	from_mask = position.black_piece[N_KING];

	if (from_mask != 0) value_black_material = value_black_material + KING;

	// �����-----------------------------------------------------------------
	from_mask = position.black_piece[N_QUEEN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + QUEEN;
	}

	// �����-----------------------------------------------------------------
	from_mask = position.black_piece[N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + ROOK;
	}

	// ����------------------------------------------------------------------- 
	from_mask = position.black_piece[N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + BISHOP;
	}

	// ���� -------------------------------------------------------------------
	from_mask = position.black_piece[N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + KNIGHT;
	}

	// �����--------------------------------------------------------------------
	from_mask = position.black_piece[N_PAWN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + PAWN;
	}

	return value_black_material;
}


//	===================================================================
// ������� ���������� ����
inline __int32 Evaluate::popcnt(unsigned __int64 analyzed_number)
{ 
	_asm
	{
		mov  ecx, dword ptr analyzed_number[4]
		xor  eax, eax      
		test  ecx, ecx
		jz  label_2

		label_1: 
			lea  ebx, [ecx-1]
			inc  eax
			and  ecx, ebx
			jnz  label_1

			label_2:
				mov  ecx, dword ptr analyzed_number[0]
				test  ecx, ecx
				jz  label_4

				label_3:
					lea  ebx, [ecx-1]
					inc  eax     
					and  ecx, ebx
					jnz  label_3
					label_4:
	}
}

	// TEST popcnt-----------------------------
	//unsigned __int64 move = 0;

	//for (int i = 0; i < 64; i++ )
	//{
	//	move = move | e_move_masks[i];
	//	//std::cout << "popcnt= " << popcnt(e_move_masks[i]) << "\n";
	//	std::cout << "popcnt= " << popcnt(move) << "\n";
	//}
    // TEST-------------------------------------


//// test-------------------------------------------------------
//std::cout <<"\n";
//std::cout << " from = " << (int)from <<"\n";
//std::cout << " knigt_moves_masks " <<"\n";
//Test_chess_bitboard::print_bitboard_map(move_mask);
//std::cout <<"\n";
//std::cout << " black_king_move_mask " <<"\n";
//Test_chess_bitboard::print_bitboard_map(black_king_move_mask);
//// test-------------------------------------------------------