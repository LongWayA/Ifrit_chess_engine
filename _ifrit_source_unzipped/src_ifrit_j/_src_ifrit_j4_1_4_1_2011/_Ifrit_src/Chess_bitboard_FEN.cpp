#include <cstring>
#include <iostream> // TEST

#include "Chess_bitboard.h"

//-----------------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009   11.11.2009 21.11.2010 20.12.2010  1.1.2011
*/
//-----------------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

 ��������� ��� - ������ � �������������� ���������� ����� 

*/

//	===================================================================
//��������� ��� - ������� � �������������� ������ ���������� ���������� ch_koordinata_to_indeks
void Chess_bitboard::fen_to_board
(
	const std::string position_fen//������ ���������� ��� �������(������ ����������)
)
{
	// �������� �� ������� ���� "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	// �������� / ���������� ����������� ��������� �����
	// ����� �������� ���������� ������ ������ �� �����������
	// ����� ��� ������ ������ ������� ����� ������ ����� ���� � ��������� ������

	//---------------------------------------------
	// �������������� ��������(bit board) ����� 
	bitboard_ini();

	unsigned __int8 i = 13;// ������� ������� ��� ��������� ��� - ������

	// TEST
	//std::cout << "--------------------" << "\n";
	//std::cout << position_fen.at(i-1) << "\n";
	//std::cout << position_fen.at(i) << "\n";
	//std::cout << position_fen.at(i+1) << "\n";
	//std::cout << "--------------------" << "\n";

	//----------------------------------------------------------------------------------	
	// 1 ����  ������ ��� ������� - ������� ������ � ������ �����
	// ��� ��� �������� ��� �� ����� �� ��� ����� ���������� ���������� 
	// xy-������������� �����
	// (�� ����� ���� ��� �������� ��� �� ������ ���������� ����� � ������ :))
	unsigned __int8 y = 0;// ������������ ���������� �����
	unsigned __int8 x = 0;// �������������� ���������� �����
	unsigned __int8 n = 0;// ���������� ���������� ����� (����� ����� ���������� �� ������)
	unsigned __int8 void_q;// ���������� ������ ������

	// ����� ������������� ����� � ������� � ��� �������� ������ ����
	// 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1

	// � ����� �� ������������ ��������� ����  1k1r4/8/8/8/8/8/8/3R2K1
	// ������ ��������� ������� ������
	// ���� �� � �.�. ������������ �����������
	while (position_fen.at(i) != ' ')
	{
		x = 0;

		//  --------------------
		//  ���� �� � ����� �� �����������
		while ((position_fen.at(i) != '/') && (position_fen.at(i) != ' '))
		{
			//  ��������� ���������� ���������� �� � ���������� n
			n = 8 * (7 - y) + x;

			////////////////////////////////////////////////////////////////
			// ����������� ����� � ������ ������
			switch (position_fen.at(i))
			{
				case 'K':// ����� ������

					// ��������� ������ � ������ ���� ����� �����
					all_white_piece = all_white_piece 
						| move_masks[n];

					// ��������� ������ � ������ ����� �������
					white_piece[N_KING] 
						= white_piece[N_KING] 
						| move_masks[n];

					// ������ ���������� ������ �� �����
					white_name_from_coord[n] = N_KING;

				break;

				case 'Q':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					all_white_piece = all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� ������
					white_piece[N_QUEEN] 
						= white_piece[N_QUEEN] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					white_name_from_coord[n] = N_QUEEN;

				break;

				case 'R':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					all_white_piece = all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� �����
					white_piece[N_ROOK] 
						= white_piece[N_ROOK] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					white_name_from_coord[n] = N_ROOK;

				break;

				case 'B':// ����� ����

					// ��������� ����� � ������ ���� ����� �����
					all_white_piece = all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� ������
					white_piece[N_BISHOP] 
						= white_piece[N_BISHOP] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					white_name_from_coord[n] = N_BISHOP;

				break;

				case 'N':// ����� ����

					// ��������� ���� � ������ ���� ����� �����
					all_white_piece = all_white_piece 
						| move_masks[n];

					// ��������� ���� � ������ ����� �����
					white_piece[N_KNIGHT] 
						= white_piece[N_KNIGHT] 
						| move_masks[n];

					// ������ ���������� ���� �� �����
					white_name_from_coord[n] = N_KNIGHT;

				break;

				case 'P':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					all_white_piece = all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� �����
					white_piece[N_PAWN] 
						= white_piece[N_PAWN] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					white_name_from_coord[n] = N_PAWN;

				break;

				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				// ������

				case 'k'://������ ������

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_KING] 
						= black_piece[N_KING] 
						| move_masks[n];

					black_name_from_coord[n] = N_KING;

				break;

				case 'q'://������ �����

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_QUEEN] 
					= black_piece[N_QUEEN] 
					| move_masks[n];

					black_name_from_coord[n] = N_QUEEN;

				break;

				case 'r'://������ �����

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_ROOK] 
					= black_piece[N_ROOK] 
					| move_masks[n];

					black_name_from_coord[n] = N_ROOK;

				break;

				case 'b'://������ ����

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_BISHOP] 
						= black_piece[N_BISHOP] 
						| move_masks[n];

					black_name_from_coord[n] = N_BISHOP;

				break;

				case 'n'://������ ����

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_KNIGHT] 
					= black_piece[N_KNIGHT] 
					| move_masks[n];

					black_name_from_coord[n] = N_KNIGHT;

				break;

				case 'p'://������ �����

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_PAWN] 
					= black_piece[N_PAWN] 
					| move_masks[n];

					black_name_from_coord[n] = N_PAWN;

				break;
			
				default:

					//-----------------------------------------------------------------
					// � ������������ ���������� ������ ����� � ��� 8 ������ ������
					// ���� ����� '8' �� ������� ch_koordinata_to_indeks ������ ����� 7 �� �� ����� ����� 8 
					// ������� ���������� (1 +... 
					void_q = (1 + ch_koordinata_to_indeks(position_fen.at(i)));

					// ���������� ������ ������ ������
					// � ��������� 7 ( ����� ������� � ��� ���������� 1 + 7 = 8 ��� ������ ������ ) 
					for (unsigned __int8 f = 1; f < void_q; f++)
					{
						x = x + 1;
					}
			}
			////////////////////////////////////////////////////////////////

			i = i + 1;
			x = x + 1;

		}

		//--------------------		
		// ����� �� ���������� ������
		if (position_fen.at(i) != ' ')
		{
			i = i + 1;
			y = y + 1;
		}
	  	
	}

	// ������� ��� ������ �� �����
	all_piece = all_white_piece | all_black_piece;

	// ������ ��� �������� ���������� ��������� ����
	// w - - 0 1 ��� b KQkq e3 0 1 ���� ���� ��������� � ������ �� �������
	//-----------------------------------------------------------------------------------	
	//2 ���� �������� ������� ���� 	
	// ����������� ������
	while (position_fen.at(i) == ' ')
	{
		i = i + 1;
	}

	// w - ��� �����  b-��� ������
	if (position_fen.at(i) == 'w')
	{
		colour_move = 1;
	}

	if (position_fen.at(i) == 'b')
	{
		colour_move = 0;
	}

	//	-----------------------------------------------------------------------------------	
	//	3 ���� �������� ���������
	// �������
	i = i + 2;

	// ����� ����������� ������ �� ��������� ������� �������� ���� ���� ���� ������ ������� ���� �
	castling_Q = 0;
	castling_K = 0;
	castling_q = 0;
	castling_k = 0;

	while (position_fen.at(i) != ' ')
	{	
		if (position_fen.at(i) == 'K')
		{
			castling_K = 1;// �������� ��������� �����
		}

		if (position_fen.at(i) == 'Q')
		{
			castling_Q = 1;// ������� ��������� �����
		}

		if (position_fen.at(i) == 'k')
		{
			castling_k = 1;// �������� ��������� ������
		}

		if (position_fen.at(i) == 'q')
		{
			castling_q = 1;// ������� ��������� ������
		}

		i = i + 1;
	}

	//	-----------------------------------------------------------------------------------
	//	 4 ���� �������� ������ �� �������
	/*
	�������� ��� ���������� ����� ����:
	And after the move 1. e4:
	rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
	������� ������� ��� ������ �������� ���������� ������ ���� (��� ���� �4 ����� ���� �3)
	*/
	// ����������� ������
	while(position_fen.at(i) == ' ')
	{
		i = i + 1;
	}

	// �� ������ ��� ���������� ��� � ���� ������ �� � ���� �� ����
	// � ���� ���������� ������� �� 1	 
	// ��������� ������
	taking_passage_yes = 0;
	taking_passage_x   = 0;
	taking_passage_y   = 0;

	if (position_fen.at(i) != '-')
	{
		taking_passage_yes = 1;// ��������� ������
		taking_passage_x = koordinata_to_indeks(position_fen.at(i));// � ����������(�� �����)
		i = i + 1;
		taking_passage_y = ch_koordinata_to_indeks(position_fen.at(i));// � ���������� 	
	}
	//---------------------------------------------------------------------------

}

//	===================================================================
//	�� ����� ������ ������ ������(���� �� ����� ������ ���� ������� :))	
int Chess_bitboard::koordinata_to_indeks
(
	const __int8 ch
 )
{
	if (ch == 'a') return 0;
	if (ch == 'b') return 1;
	if (ch == 'c') return 2;
	if (ch == 'd') return 3;
	if (ch == 'e') return 4;
	if (ch == 'f') return 5;
	if (ch == 'g') return 6;
	if (ch == 'h') return 7;

	return -1;
}

//	===================================================================
//�� ������� ����� ������ �����
//������� ��� �� ����� ����� �� �����(java) :)
//�������� ������� ����������� � ���� ��� ���� �� 9.01.06 
//������� ��� ������������ �������� :)) 
int Chess_bitboard::ch_koordinata_to_indeks
(
 const int ch
 )
{
	if (ch == '1') return 0;
	if (ch == '2') return 1;
	if (ch == '3') return 2;
	if (ch == '4') return 3;
	if (ch == '5') return 4;
	if (ch == '6') return 5;
	if (ch == '7') return 6;
	if (ch == '8') return 7;

	return -1;
}

//	===================================================================
// ��������� ������������� �����
void Chess_bitboard::bitboard_ini()
{

	all_piece = 0;
	all_white_piece = 0;
	all_black_piece = 0;

	for (unsigned __int8 i = 0; i < 7; i++)
	{
		white_piece[i]= 0;
		black_piece[i]= 0;
	}

	for (unsigned __int8 j = 0; j < 64; j++)
	{
		white_name_from_coord[j]= 0;
		black_name_from_coord[j]= 0;
	}

	colour_move = 0;

	taking_passage_yes = 0;

	taking_passage_x = 0;

	taking_passage_y = 0;

	castling_Q = 0;

	castling_K = 0;

	castling_q = 0;

	castling_k = 0;

	hash_key = 0;

	value_white_material = 0;

	value_black_material = 0;

	all_black_or_white_piece = 0;

}
