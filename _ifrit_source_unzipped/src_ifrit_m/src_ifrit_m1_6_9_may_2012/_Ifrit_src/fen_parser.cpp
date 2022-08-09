
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 22.02.2009 11.11.2009 21.11.2010 20.12.2010 1.1.2011

#include <cstring>
#include <iostream>

#include "constants.h"
#include "fen_parser.h"
#include "move_generation_const.h"


//	===================================================================
//��������� ��� - ������� � �������������� ������ ���������� ���������� ch_koordinata_to_indeks
void Fen_parser::fen_to_board
(
	struct Position & position,// ������������� �����
	const std::string position_fen//������ ���������� ��� �������(������ ����������)
)
{
	// �������� �� ������� ���� "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	// �������� / ���������� ����������� ��������� �����
	// ����� �������� ���������� ������ ������ �� �����������
	// ����� ��� ������ ������ ������� ����� ������ ����� ���� � ��������� ������

	//---------------------------------------------
	// �������������� ��������(bit board) ����� 
	bitboard_ini(position);

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
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// ��������� ������ � ������ ����� �������
					position.white_piece[N_KING] 
						= position.white_piece[N_KING] 
						| move_masks[n];

					// ������ ���������� ������ �� �����
					position.white_name_from_coord[n] = N_KING;

				break;

				case 'Q':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� ������
					position.white_piece[N_QUEEN] 
						= position.white_piece[N_QUEEN] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					position.white_name_from_coord[n] = N_QUEEN;

				break;

				case 'R':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� �����
					position.white_piece[N_ROOK] 
						= position.white_piece[N_ROOK] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					position.white_name_from_coord[n] = N_ROOK;

				break;

				case 'B':// ����� ����

					// ��������� ����� � ������ ���� ����� �����
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� ������
					position.white_piece[N_BISHOP] 
						= position.white_piece[N_BISHOP] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					position.white_name_from_coord[n] = N_BISHOP;

				break;

				case 'N':// ����� ����

					// ��������� ���� � ������ ���� ����� �����
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// ��������� ���� � ������ ����� �����
					position.white_piece[N_KNIGHT] 
						= position.white_piece[N_KNIGHT] 
						| move_masks[n];

					// ������ ���������� ���� �� �����
					position.white_name_from_coord[n] = N_KNIGHT;

				break;

				case 'P':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// ��������� ����� � ������ ����� �����
					position.white_piece[N_PAWN] 
						= position.white_piece[N_PAWN] 
						| move_masks[n];

					// ������ ���������� ����� �� �����
					position.white_name_from_coord[n] = N_PAWN;

				break;

				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				// ������

				case 'k'://������ ������

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_KING] 
						= position.black_piece[N_KING] 
						| move_masks[n];

					position.black_name_from_coord[n] = N_KING;

				break;

				case 'q'://������ �����

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_QUEEN] 
					= position.black_piece[N_QUEEN] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_QUEEN;

				break;

				case 'r'://������ �����

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_ROOK] 
					= position.black_piece[N_ROOK] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_ROOK;

				break;

				case 'b'://������ ����

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_BISHOP] 
						= position.black_piece[N_BISHOP] 
						| move_masks[n];

					position.black_name_from_coord[n] = N_BISHOP;

				break;

				case 'n'://������ ����

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_KNIGHT] 
					= position.black_piece[N_KNIGHT] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_KNIGHT;

				break;

				case 'p'://������ �����

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_PAWN] 
					= position.black_piece[N_PAWN] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_PAWN;

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
	position.all_piece = position.all_white_piece | position.all_black_piece;

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
		position.colour_move = 1;
	}

	if (position_fen.at(i) == 'b')
	{
		position.colour_move = 0;
	}

	//	-----------------------------------------------------------------------------------	
	//	3 ���� �������� ���������
	// �������
	i = i + 2;

	// ����� ����������� ������ �� ��������� ������� �������� ���� ���� ���� ������ ������� ���� �
	position.castling_Q = 0;
	position.castling_K = 0;
	position.castling_q = 0;
	position.castling_k = 0;

	while (position_fen.at(i) != ' ')
	{	
		if (position_fen.at(i) == 'K')
		{
			position.castling_K = 1;// �������� ��������� �����
		}

		if (position_fen.at(i) == 'Q')
		{
			position.castling_Q = 1;// ������� ��������� �����
		}

		if (position_fen.at(i) == 'k')
		{
			position.castling_k = 1;// �������� ��������� ������
		}

		if (position_fen.at(i) == 'q')
		{
			position.castling_q = 1;// ������� ��������� ������
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
	position.taking_passage_yes = 0;
	position.taking_passage_x   = 0;
	position.taking_passage_y   = 0;

	if (position_fen.at(i) != '-')
	{
		position.taking_passage_yes = 1;// ��������� ������
		position.taking_passage_x = koordinata_to_indeks(position_fen.at(i));// � ����������(�� �����)
		i = i + 1;
		position.taking_passage_y = ch_koordinata_to_indeks(position_fen.at(i));// � ���������� 	
	}
	//---------------------------------------------------------------------------

}

//	===================================================================
//	�� ����� ������ ������ ������(���� �� ����� ������ ���� ������� :))	
int Fen_parser::koordinata_to_indeks
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
int Fen_parser::ch_koordinata_to_indeks
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
void Fen_parser::bitboard_ini
(
	struct Position & position// ������������� �����
)
{

	position.all_piece = 0;
	position.all_white_piece = 0;
	position.all_black_piece = 0;

	for (unsigned __int8 i = 0; i < 7; i++)
	{
		position.white_piece[i]= 0;
		position.black_piece[i]= 0;
	}

	for (unsigned __int8 j = 0; j < 64; j++)
	{
		position.white_name_from_coord[j]= 0;
		position.black_name_from_coord[j]= 0;
	}

	position.colour_move = 0;

	position.taking_passage_yes = 0;

	position.taking_passage_x = 0;

	position.taking_passage_y = 0;

	position.castling_Q = 0;

	position.castling_K = 0;

	position.castling_q = 0;

	position.castling_k = 0;

	position.hash_key = 0;

	position.value_white_material = 0;

	position.value_black_material = 0;

	//all_black_or_white_piece = 0;

}
