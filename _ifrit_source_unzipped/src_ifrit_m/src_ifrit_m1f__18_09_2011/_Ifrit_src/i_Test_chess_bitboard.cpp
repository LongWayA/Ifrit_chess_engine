
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.11.2010

#include <fstream>
#include <iostream>

#include "h_Generator_moves_const.h"
#include "i_Test_chess_bitboard.h"


//	===================================================================
//  печатаем 64 битное число как доску. использую для теста 
void i_Test_chess_bitboard::print_bitboard_map
(
	const unsigned __int64 mask //
)
{

unsigned __int64 print_m = mask ; //

int print[64];

	for (int i= 0; i < 64; i++)
	{

		if( print_m & 1)
		{
			//std::cout << 1 <<" ";
			print[i] = 1;
		}
		else
		{
			//std::cout << 0 <<" ";
			print[i] = 0;
		}

		//if( i%8 == 7)
		//{
		//	std::cout << "\n";
		//}

		print_m = print_m >> 1;
	}

std::cout << " --------------- "<< "\n";
std::cout << "\n";

	for ( int i = 56; i >= 0; i = i - 8)
	{
		for ( int j = i; j < i + 8; j++)
		{
			std::cout << print[j] <<" ";
		}

		std::cout << "\n";
		//std::cout << i << "\n";
	}

//std::cout << " ===================== "<< "\n";
//std::cout << "\n";

}


//	===================================================================
// использую для теста. сейчас уже не помню для чего он мне был нужен
void i_Test_chess_bitboard::print_name_from_coord
(
	const struct Position & position// представление доски
)
{

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << position.white_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	std::cout << "\n";

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << position.black_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	//for (int i= 0; i < 64; i++)
	//{
	//	std::cout << position.bitboard->white_name_from_coord[i];
	//	if( i%8 == 7)
	//{
	//		std::cout << "\n";
	//}
	//}
}

//	===================================================================
// использую для теста. запоминаем доску
void i_Test_chess_bitboard::save_test_bitboard
(
	struct Position & position_save,// представление доски
	const struct Position & position// представление доски
)
{

	position_save.all_piece = position.all_piece;// все фигуры
	position_save.all_white_piece = position.all_white_piece;// все белые фигуры
	position_save.all_black_piece = position.all_black_piece;// все черные фигуры
	position_save.colour_move = position.colour_move;
	position_save.taking_passage_yes = position.taking_passage_yes;
	position_save.taking_passage_x = position.taking_passage_x;
	position_save.taking_passage_y = position.taking_passage_y;
	position_save.castling_Q = position.castling_Q;
	position_save.castling_K = position.castling_K;
	position_save.castling_q = position.castling_q;
	position_save.castling_k = position.castling_k;
	position_save.hash_key =  position.hash_key;
	position_save.value_white_material = position.value_white_material;
	position_save.value_black_material = position.value_black_material;

	for (unsigned __int8 n =0; n < 7; n++ )
	{
		position_save.white_piece[n] = position.white_piece[n];
		position_save.black_piece[n] = position.black_piece[n];
	}

	for (unsigned __int8 k =0; k < 64; k++ )
	{
		position_save.white_name_from_coord[k] = position.white_name_from_coord[k];
		position_save.black_name_from_coord[k] = position.black_name_from_coord[k];
	}
}

//	===================================================================
// использую для теста. сравниваем доски
void i_Test_chess_bitboard::compare_test_bitboard
(
	const struct Position & position_1,// представление доски
	const struct Position & position_2// представление доски
)
{
	// проверяем востановилась ли доска после прохождения глубины
	if (position_1.all_piece != position_2.all_piece)
	{
		std::cout << "white test_all_piece != bitboard->all_piece" << "\n";// все фигуры
	}

	if (position_1.all_white_piece != position_2.all_white_piece)
	{
		std::cout << "white test_all_white_piece != bitboard->all_white_piece" << "\n";// все белые фигуры
	}

	if (position_1.all_black_piece != position_2.all_black_piece) 
	{
		std::cout << "white test_all_black_piece != bitboard->all_black_piece" << "\n";// все черные фигуры
	}

	if (position_1.colour_move != position_2.colour_move)
	{
		std::cout << "white test_colour_move != bitboard->colour_move" << "\n";
	}

	if (position_1.taking_passage_yes != position_2.taking_passage_yes)
	{
		std::cout << "white test_taking_passage_yes != bitboard->taking_passage_yes" << "\n";
	}

	if (position_1.taking_passage_x != position_2.taking_passage_x)
	{
		std::cout << "white test_taking_passage_x != bitboard->taking_passage_x" << "\n";
	}

	if (position_1.taking_passage_y != position_2.taking_passage_y)
	{
		std::cout << "white test_taking_passage_y != bitboard->taking_passage_y" << "\n";
	}

	if (position_1.castling_Q != position_2.castling_Q)
	{
		std::cout << "white test_castling_Q != bitboard->castling_Q" << "\n";
	}

	if (position_1.castling_K != position_2.castling_K)
	{
		std::cout << "white test_castling_K != bitboard->castling_K" << "\n";
	}

	if (position_1.castling_q != position_2.castling_q)
	{
		std::cout << "white test_castling_q != bitboard->castling_q" << "\n";
	}

	if (position_1.castling_k != position_2.castling_k)
	{
		std::cout << "white test_castling_k != bitboard->castling_k" << "\n";
	}

	if (position_1.hash_key !=  position_2.hash_key)
	{
		std::cout << "white test_hash_key !=  bitboard->hash_key" << "\n";
	}

	if (position_1.value_white_material != position_2.value_white_material)
	{
		std::cout << "white test_value_white_material != bitboard->value_white_material" << "\n";
	}

	if (position_1.value_black_material != position_2.value_black_material)
	{
		std::cout << "white test_value_black_material != bitboard->value_black_material" << "\n";
	}



	for (unsigned __int8 n2 = 0; n2 < 7; n2++ )
	{
		if (position_1.white_piece[n2] != position_2.white_piece[n2])
		{
			std::cout << "white test_white_piece[n] = bitboard->white_piece[n]" << n2 << "\n";
		}

		if (position_1.black_piece[n2] != position_2.black_piece[n2])
		{
			std::cout << "white test_black_piece[n] = bitboard->black_piece[n]" << n2 << "\n";
		}
	}

	for (unsigned __int8 k2 = 0; k2 < 64; k2++ )
	{
		if (position_1.white_name_from_coord[k2] != position_2.white_name_from_coord[k2])
		{
			std::cout << "white test_white_name_from_coord[k] = bitboard->white_name_from_coord[k]" << k2 << "\n";
		}

		if (position_1.black_name_from_coord[k2] != position_2.black_name_from_coord[k2])
		{
			std::cout << "white test_black_name_from_coord[k] = bitboard->black_name_from_coord[k]" << k2 << "\n";
		}
	}

}


//	===================================================================
// печатаем битбоард доску
void i_Test_chess_bitboard::bitboard_print
(
	const struct Position & position// представление доски
)
{
	std::ofstream Test_Bitboard;

	unsigned __int64 z = 0;

	Test_Bitboard.open ("_TEST_BITBOARD.txt",std::ios::app);

	Test_Bitboard << "bitboard->colour_move = "<< position.colour_move <<"\n";
	Test_Bitboard << "\n";

	Test_Bitboard << "taking_passage_yes= " << position.taking_passage_yes << "\n";
	Test_Bitboard << "bitboard->taking_passage_x= " << (int)position.taking_passage_x << "\n";
	Test_Bitboard << "taking_passage_y= " << (int)position.taking_passage_y << "\n";

	Test_Bitboard << "\n";
	Test_Bitboard << "castling_K= " << position.castling_K << "\n";
	Test_Bitboard << "castling_Q= " << position.castling_Q << "\n";
	Test_Bitboard << "castling_k= " << position.castling_k << "\n";
	Test_Bitboard << "castling_q= " << position.castling_q << "\n";

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
			z = position.all_white_piece & move_masks[j];

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
			z = position.all_black_piece & move_masks[j];

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

		if (p == 1)
		{
			Test_Bitboard << "PAWN" << "\n";
		}
		else if (p == 2)
		{
			Test_Bitboard << "KNIGHT" << "\n";
		}
		else if (p == 3)
		{
			Test_Bitboard << "BISHOP" << "\n";
		}
		else if (p == 4)
		{
			Test_Bitboard << "ROOK" << "\n";
		}
		else if (p == 5)
		{
			Test_Bitboard << "QUEEN" << "\n";
		}
		else if (p == 6)
		{
			Test_Bitboard << "KING" << "\n";
		}

		for (int i = 63; i > -1; i = i - 8)
		{
			for (int j = i - 7; j < i + 1; j++)
			{
				z =  position.white_piece[p] & move_masks[j];

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

		if (p == 1)
		{
			Test_Bitboard << "PAWN" << "\n";
		}
		else if (p == 2)
		{
			Test_Bitboard << "KNIGHT" << "\n";
		}
		else if (p == 3)
		{
			Test_Bitboard << "BISHOP" << "\n";
		}
		else if (p == 4)
		{
			Test_Bitboard << "ROOK" << "\n";
		}
		else if (p == 5)
		{
			Test_Bitboard << "QUEEN" << "\n";
		}
		else if (p == 6)
		{
			Test_Bitboard << "KING" << "\n";
		}

		for (int i = 63; i > -1; i = i - 8)
		{
			for (int j = i - 7; j < i + 1; j++)
			{
				z =  position.black_piece[p] & move_masks[j];

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