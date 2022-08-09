//#include <cstring>
#include <iostream> // TEST

#include "b_FEN.h"
#include "e_Move.h"
#include "d_Bitboard_gen.h"
#include "j_Hash.h"
#include "j_ZObrist.h"

//-----------------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009   11.11.2009
*/
//-----------------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

 ������ ������� �� ������ � fen ����������
*/

//	===================================================================
// ��������� ��� ������ ��� � �������������� �����
void FEN::public_parse_position
(
	struct T_Bitboard * p_bitboard,//�����
	char string_in[]//�������� ������(65536)
)
{
	// ���������������� ������� ������ ����� ���� ���� ����� 
	//"position startpos" ������ ��������� �������
	//"position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� ������� � ������� 
	//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� ����� ��� �������������

	char * fen;  //
	char * moves;//
	char * ptr;  // ��������������� ���������
	char move_string[]={' ',' ',' ',' ',' ',' ',' '};// ��������������� ������
	char * StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";// ��������� ���-�������
	unsigned __int8 cnt_move = 0;// ������� ����� � ������ ������ ����� ��� ����������� ������


	//�������� ��������� �� fen
	fen = strstr(string_in,"fen ");

	//�������� ��������� �� moves
	moves = strstr(string_in,"moves ");

	if (fen != NULL)
	{
		//�������� � �������� ���-��������
		fen_to_board(p_bitboard,fen);
	}
	else
	{
		// � ��� ������������� ����������� ������� startpos
		// ��������� ��� �� ��� ���������� �� ������ ���� ������ ��������� ������� � ����
		// StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		fen_to_board(p_bitboard,StartFen);
	}

	//---------------------------------------------
	// ������� �����. ��������� ��� ����������� ����� �� ����� �������
	cnt_move = 0;

	// �������������� ������ ������ �� ��������� ������ ������������ 
	// ��� ������ ������ ������������ ��� ����������� ������������ ���������� �������
	ZOBRIST::public_ini_hash_string_moves(); 

	// �� ������� ������� ���������� ��� - ����
	ZOBRIST::public_start_position_random_key(p_bitboard);

	// ���������� ���-���� � ������ ��� - ������ �� ������ ����
	ZOBRIST::public_save_hash_string_moves(p_bitboard->hash_key,cnt_move); 
	//---------------------------------------------

	// ���� ���������� � ����
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	if (moves != NULL)
	{
		// ����������� moves 
		ptr = moves + 6;

		while (*ptr != '\0')
		{ 
			move_string[1] = *ptr++;
			move_string[2] = *ptr++;
			move_string[3] = *ptr++;
			move_string[4] = *ptr++;

			if (*ptr == '\0' || *ptr == ' ')
			{
				move_string[5] = '\0';
			}
			else
			{ // �����������
				move_string[5] = *ptr++;
				move_string[6] = '\0';
			}

			move_to_board(p_bitboard,move_string);

			//---------------------------------------------
			cnt_move = cnt_move + 1;
			// �� ������� ������� ���������� ��� - ����
			ZOBRIST::public_start_position_random_key(p_bitboard);

			// ���������� ���-���� � ������ ��� -������ �� ������ ����
			// ���� ������ ������ ������������ ��� ����������� ������������ ���������� �������
			ZOBRIST::public_save_hash_string_moves(p_bitboard->hash_key,cnt_move); 
			//---------------------------------------------

			while (*ptr == ' ')
			{
				ptr++;
			}
		}
	}

	fen   = NULL;
	moves = NULL;
	ptr = NULL;
	StartFen = NULL;

}

//	===================================================================
//��������� ��� - ������� � �������������� ������ ���������� ���������� ch_koordinata_to_indeks
void FEN::fen_to_board
(
	struct T_Bitboard * p_bitboard,//�����
	const char fen[]//������ ���������� ��� �������(������ ����������)
)
{
	// �������� �� ������� ���� "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	// �������� / ���������� ����������� ��������� �����
	// ����� �������� ���������� ������ ������ �� �����������
	// ����� ��� ������ ������ ������� ����� ������ ����� ���� � ��������� ������

	//---------------------------------------------
	// �������������� ��������(bit board) ����� 
	// (������ �� � ��� ������� ��� ����� ������� �� �� ����� ��� ���������)
	p_bitboard->all_piece = 0;
	p_bitboard->all_white_piece = 0;
	p_bitboard->all_black_piece = 0;

	for (unsigned __int8 name_piece = 0; name_piece < 7; name_piece++)
	{
		p_bitboard->white_piece[name_piece] = 0;
		p_bitboard->black_piece[name_piece] = 0;
	}

	for (unsigned __int8 coordinate = 0; coordinate < 64; coordinate++)
	{
		p_bitboard->white_name_from_coord[coordinate] = 0;
		p_bitboard->black_name_from_coord[coordinate] = 0;
	}
	//---------------------------------------------

	unsigned __int8 i = 0;// ������� ������� ��� ��������� ��� - ������

	// ����������� ����� fen
	while (fen[i] != ' ')
	{
		i = i + 1;
	}


	// ����������� ������
	while (fen[i] == ' ')
	{
		i = i + 1;
	}

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
	while (fen[i] != ' ')
	{
		x = 0;

		//  --------------------
		//  ���� �� � ����� �� �����������
		while ((fen[i] != '/') && (fen[i] != ' '))
		{
			//  ��������� ���������� ���������� �� � ���������� n
			n = 8 * (7 - y) + x;

			////////////////////////////////////////////////////////////////
			// ����������� ����� � ������ ������
			switch (fen[i])
			{
				case 'K':// ����� ������

					// ��������� ������ � ������ ���� ����� �����
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// ��������� ������ � ������ ����� �������
					p_bitboard->white_piece[BITBOARDGENERATOR::N_KING] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_KING] 
						| move_masks_f[n];

					// ������ ���������� ������ �� �����
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_KING;

				break;

				case 'Q':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// ��������� ����� � ������ ����� ������
					p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN] 
						| move_masks_f[n];

					// ������ ���������� ����� �� �����
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_QUEEN;

				break;

				case 'R':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// ��������� ����� � ������ ����� �����
					p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] 
						| move_masks_f[n];

					// ������ ���������� ����� �� �����
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_ROOK;

				break;

				case 'B':// ����� ����

					// ��������� ����� � ������ ���� ����� �����
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// ��������� ����� � ������ ����� ������
					p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP] 
						| move_masks_f[n];

					// ������ ���������� ����� �� �����
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_BISHOP;

				break;

				case 'N':// ����� ����

					// ��������� ���� � ������ ���� ����� �����
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// ��������� ���� � ������ ����� �����
					p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT] 
						| move_masks_f[n];

					// ������ ���������� ���� �� �����
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_KNIGHT;

				break;

				case 'P':// ����� �����

					// ��������� ����� � ������ ���� ����� �����
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// ��������� ����� � ������ ����� �����
					p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN] 
						| move_masks_f[n];

					// ������ ���������� ����� �� �����
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_PAWN;

				break;

				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				// ������

				case 'k'://������ ������

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_KING] 
						= p_bitboard->black_piece[BITBOARDGENERATOR::N_KING] 
						| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_KING;

				break;

				case 'q'://������ �����

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_QUEEN;

				break;

				case 'r'://������ �����

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_ROOK;

				break;

				case 'b'://������ ����

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP] 
						= p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP] 
						| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_BISHOP;

				break;

				case 'n'://������ ����

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_KNIGHT;

				break;

				case 'p'://������ �����

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_PAWN;

				break;
			
				default:

					//-----------------------------------------------------------------
					// � ������������ ���������� ������ ����� � ��� 8 ������ ������
					// ���� ����� '8' �� ������� ch_koordinata_to_indeks ������ ����� 7 �� �� ����� ����� 8 
					// ������� ���������� (1 +... 
					void_q = (1 + ch_koordinata_to_indeks(fen[i]));

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
		if (fen[i] != ' ')
		{
			i = i + 1;
			y = y + 1;
		}
	  	
	}

	// ������� ��� ������ �� �����
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

	// ������ ��� �������� ���������� ��������� ����
	// w - - 0 1 ��� b KQkq e3 0 1 ���� ���� ��������� � ������ �� �������
	//-----------------------------------------------------------------------------------	
	//2 ���� �������� ������� ���� 	
	// ����������� ������
	while (fen[i] == ' ')
	{
		i = i + 1;
	}

	// w - ��� �����  b-��� ������
	if (fen[i] == 'w')
	{
		p_bitboard->colour_move = 1;
	}

	if (fen[i] == 'b')
	{
		p_bitboard->colour_move = 0;
	}

	//	-----------------------------------------------------------------------------------	
	//	3 ���� �������� ���������
	// �������
	i = i + 2;

	// ����� ����������� ������ �� ��������� ������� �������� ���� ���� ���� ������ ������� ���� �
	p_bitboard->castling_Q = 0;
	p_bitboard->castling_K = 0;
	p_bitboard->castling_q = 0;
	p_bitboard->castling_k = 0;

	while (fen[i] != ' ')
	{	
		if (fen[i] == 'K')
		{
			p_bitboard->castling_K = 1;// �������� ��������� �����
		}

		if (fen[i] == 'Q')
		{
			p_bitboard->castling_Q = 1;// ������� ��������� �����
		}

		if (fen[i] == 'k')
		{
			p_bitboard->castling_k = 1;// �������� ��������� ������
		}

		if (fen[i] == 'q')
		{
			p_bitboard->castling_q = 1;// ������� ��������� ������
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
	while(fen[i] == ' ')
	{
		i = i + 1;
	}

	// �� ������ ��� ���������� ��� � ���� ������ �� � ���� �� ����
	// � ���� ���������� ������� �� 1	 
	// ��������� ������
	p_bitboard->taking_passage_yes = 0;
	p_bitboard->taking_passage_x   = 0;
	p_bitboard->taking_passage_y   = 0;

	if (fen[i] != '-')
	{
		p_bitboard->taking_passage_yes = 1;// ��������� ������
		p_bitboard->taking_passage_x = koordinata_to_indeks(fen[i]);// � ����������(�� �����)
		i = i + 1;
		p_bitboard->taking_passage_y = ch_koordinata_to_indeks(fen[i]);// � ���������� 	
	}
	//---------------------------------------------------------------------------

}

//	===================================================================
//�������� � ������ ������(moves) �.�. ���� ���������� � ������� ������� �� ���������
void FEN::move_to_board
(
	struct T_Bitboard * p_bitboard,//�����
	char move_string[]//������ ���������� ����(7)
)
{
	struct T_undo undo[1];//
	struct T_list_surplus_moves list_surplus_moves_f;//������ ���������� �����

	// ���� ���������� � ����
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	// ������������ ������ ����������� �����
	unsigned __int8 n = 8 * ch_koordinata_to_indeks(move_string[2]) 
		+ koordinata_to_indeks(move_string[1]);// ������ ������ �����

	unsigned __int8 n_2 = 8 * ch_koordinata_to_indeks(move_string[4]) 
		+ koordinata_to_indeks(move_string[3]);// ���� ������ �����

	// ������������ ��� ��������� ���� ������
	if (p_bitboard->colour_move == 1)
	{
		BITBOARDGENERATOR::public_generator_captures_white(p_bitboard,&list_surplus_moves_f);
		BITBOARDGENERATOR::public_generator_move_white(p_bitboard,&list_surplus_moves_f);
	}
	else
	{
		BITBOARDGENERATOR::public_generator_captures_black(p_bitboard,&list_surplus_moves_f);
		BITBOARDGENERATOR::public_generator_move_black(p_bitboard,&list_surplus_moves_f);
	}

	__int16 n_3 = -1; // ����� ���� �� ������

	// ����� �� ������ ���������� �����
	for (unsigned __int8 i = 0; i < list_surplus_moves_f.end_list; i++)
	{

		// ���� ��� � ������ ��������� � ����� �� ������(moves) �����..
		if (( n == ( (list_surplus_moves_f.move[i] >> 12) & 63 ) ) && // ���������� ������ ������ �����
		( n_2 == ((list_surplus_moves_f.move[i] >> 18) & 63) ) )  // ���������� ���� ������ �����
		{
			/*
			� ����������� �� ���� ���� �� ����������� �� ������������ ������ ����
			��� �� ������ ���� ������ ����� ����� ��� ���������� ����������� �����
			� ������ ���������� �����
			��� ������ �� ��������� ����������� ���� �� � ���������� 
			������ ����� �� �������������
			��� ��� ��� ���������� ������� ���� ����������� � �����
			����� ���������� ���� ����������� � ����� ����� � ����
			����� ������� ��� � ������������� � ������
			*/
			switch (move_string[5])
			{
				// ����������� ��� ������� ������ �������� ����� ���� � ������
				case '\0' :
					// ������ ������
					n_3 = i;
				break;

				case 'q' :
					// ����������� � �����
					n_3 = i;
				break;

				case 'r' :
					// ����������� � �����
					n_3 = i + 1;
				break;

				case 'b' :
					// ����������� � ����
					n_3 = i + 2;
				break;

				case 'n' :
					// ����������� � ����
					n_3 = i + 3;
				break;

				default:;//������ ������ �� ���������
   
			}

			// ������ ��� ����� ��� ��� ������ ������ �� ����� ������ ���
			// �������� ������� �� ����� for(int i = 0; 
			// i < list_surplus_moves_f.end_list; i++)
			break;
		}
	

	}

	//----------------------------------------------------------
	if (n_3 == -1)
	{
		std::cout << "WARNING ! moves error" << "\n";
	}
	else
	{
		// ������ ���
		if (p_bitboard->colour_move == 1)
		{
			MOVES::public_onward_moves_white((unsigned __int8)n_3
				,undo,&list_surplus_moves_f,p_bitboard);
		}
		else
		{
			MOVES::public_onward_moves_black((unsigned __int8)n_3
				,undo,&list_surplus_moves_f,p_bitboard);
		}
		//p_bitboard->colour_move = !p_bitboard->colour_move;

	}

}

//	===================================================================
//	�� ����� ������ ������ ������(���� �� ����� ������ ���� ������� :))	
int FEN::koordinata_to_indeks
(
	char ch
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
int FEN::ch_koordinata_to_indeks
(
 int ch
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