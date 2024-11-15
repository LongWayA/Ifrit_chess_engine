//#include <cstring>
#include <iostream> // TEST

#include "j_ZObrist.h"

//-----------------------------------------------------------------------------------
/* +
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 22.02.2009   11.11.2009 21.11.2010
*/
//-----------------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

 ������ ������� �� ������ � fen ����������


 ��������� ��� - ������ � �������������� ���������� ����� 

 ���������������� ������� ������ ����� ���� ���� ����� 
 "position startpos" ������ ��������� �������
 "position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� ������� � ������� 
 "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� ����� ��� �������������

 � ���������� ������ ������ �� �������� ���������� ������� ��������������� �������
 �������������� � ��� - �������������

*/

extern class ZObrist zobrist;

//	===================================================================
// ��������� ��� ������ ��� � �������������� �����
void Chess_bitboard::parse_position
(
	__int8 string_in[]//�������� ������(65536)
)
{
	// ���������������� ������� ������ ����� ���� ���� ����� 
	//"position startpos" ������ ��������� �������
	//"position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� ������� � ������� 
	//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� ����� ��� �������������

	__int8 * fen;  //
	__int8 * moves;//
	__int8 * ptr;  // ��������������� ���������
	__int8 move_string[]={' ',' ',' ',' ',' ',' ',' '};// ��������������� ������
	__int8 * StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";// ��������� ���-�������
	unsigned __int8 cnt_move = 0;// ������� ����� � ������ ������ ����� ��� ����������� ������


	//�������� ��������� �� fen
	fen = strstr(string_in,"fen ");

	//�������� ��������� �� moves
	moves = strstr(string_in,"moves ");

	if (fen != NULL)
	{
		//�������� � �������� ���-��������
		fen_to_board(fen);
	}
	else
	{
		// � ��� ������������� ����������� ������� startpos
		// ��������� ��� �� ��� ���������� �� ������ ���� ������ ��������� ������� � ����
		// StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		fen_to_board(StartFen);
	}

	//---------------------------------------------
	// ������� �����. ��������� ��� ����������� ����� �� ����� �������
	cnt_move = 0;

	// �������������� ������ ������ �� ��������� ������ ������������ 
	// ��� ������ ������ ������������ ��� ����������� ������������ ���������� �������
	zobrist.ini_hash_string_moves(); 

	// �� ������� ������� ���������� ��� - ����
	zobrist.start_position_random_key(this);

	// ���������� ���-���� � ������ ��� - ������ �� ������ ����
	zobrist.save_hash_string_moves(hash_key,cnt_move); 
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

			move_to_board(move_string);

			//---------------------------------------------
			cnt_move = cnt_move + 1;
			// �� ������� ������� ���������� ��� - ����
			zobrist.start_position_random_key(this);

			// ���������� ���-���� � ������ ��� -������ �� ������ ����
			// ���� ������ ������ ������������ ��� ����������� ������������ ���������� �������
			zobrist.save_hash_string_moves(hash_key,cnt_move); 
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
void Chess_bitboard::fen_to_board
(
	const __int8 fen[]//������ ���������� ��� �������(������ ����������)
)
{
	// �������� �� ������� ���� "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	// �������� / ���������� ����������� ��������� �����
	// ����� �������� ���������� ������ ������ �� �����������
	// ����� ��� ������ ������ ������� ����� ������ ����� ���� � ��������� ������

	//---------------------------------------------
	// �������������� ��������(bit board) ����� 
	// (������ �� � ��� ������� ��� ����� ������� �� �� ����� ��� ���������)
	all_piece = 0;
	all_white_piece = 0;
	all_black_piece = 0;

	for (unsigned __int8 name_piece = 0; name_piece < 7; name_piece++)
	{
		white_piece[name_piece] = 0;
		black_piece[name_piece] = 0;
	}

	for (unsigned __int8 coordinate = 0; coordinate < 64; coordinate++)
	{
		white_name_from_coord[coordinate] = 0;
		black_name_from_coord[coordinate] = 0;
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
	all_piece = all_white_piece | all_black_piece;

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
		colour_move = 1;
	}

	if (fen[i] == 'b')
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

	while (fen[i] != ' ')
	{	
		if (fen[i] == 'K')
		{
			castling_K = 1;// �������� ��������� �����
		}

		if (fen[i] == 'Q')
		{
			castling_Q = 1;// ������� ��������� �����
		}

		if (fen[i] == 'k')
		{
			castling_k = 1;// �������� ��������� ������
		}

		if (fen[i] == 'q')
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
	while(fen[i] == ' ')
	{
		i = i + 1;
	}

	// �� ������ ��� ���������� ��� � ���� ������ �� � ���� �� ����
	// � ���� ���������� ������� �� 1	 
	// ��������� ������
	taking_passage_yes = 0;
	taking_passage_x   = 0;
	taking_passage_y   = 0;

	if (fen[i] != '-')
	{
		taking_passage_yes = 1;// ��������� ������
		taking_passage_x = koordinata_to_indeks(fen[i]);// � ����������(�� �����)
		i = i + 1;
		taking_passage_y = ch_koordinata_to_indeks(fen[i]);// � ���������� 	
	}
	//---------------------------------------------------------------------------

}

//	===================================================================
//�������� � ������ ������(moves) �.�. ���� ���������� � ������� ������� �� ���������
void Chess_bitboard::move_to_board
(
	__int8 move_string[]//������ ���������� ����(7)
)
{
	struct Undo undo[1];//
	class List_surplus_moves list_surplus_moves_f;//������ ���������� �����

	// ���� ���������� � ����
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	// ������������ ������ ����������� �����
	unsigned __int8 n = 8 * ch_koordinata_to_indeks(move_string[2]) 
		+ koordinata_to_indeks(move_string[1]);// ������ ������ �����

	unsigned __int8 n_2 = 8 * ch_koordinata_to_indeks(move_string[4]) 
		+ koordinata_to_indeks(move_string[3]);// ���� ������ �����

	// ������������ ��� ��������� ���� ������
	if (colour_move == 1)
	{
		list_surplus_moves_f.generator_captures_white(this);
		list_surplus_moves_f.generator_move_white(this);
	}
	else
	{
		list_surplus_moves_f.generator_captures_black(this);
		list_surplus_moves_f.generator_move_black(this);
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
		if (colour_move == 1)
		{
			onward_moves_white((unsigned __int8)n_3
				,undo,&list_surplus_moves_f);
		}
		else
		{
			onward_moves_black((unsigned __int8)n_3
				,undo,&list_surplus_moves_f);
		}
		//colour_move = !colour_move;

	}

}

//	===================================================================
//	�� ����� ������ ������ ������(���� �� ����� ������ ���� ������� :))	
int Chess_bitboard::koordinata_to_indeks
(
	__int8 ch
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

//	===================================================================
// ��������� ������������� �����
void Chess_bitboard::bitboard_ini_t()
{
	all_piece = 0;
	all_white_piece = 0;
	all_black_piece = 0;

	for (unsigned __int8 i = 0; i < 7; i++)
	{
		white_piece[i]= 0;
		black_piece[i]= 0;
	}

	// ��������� �� ���������
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
