
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.11.2010

#include <fstream>
#include <iostream>

#include "l_Chess_bitboard.h"
#include "m_List_surplus_moves_const.h"

//	===================================================================
// использую для теста. сейчас уже не помню для чего он мне был нужен
void Chess_bitboard::print_name_from_coord()
{

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << white_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	std::cout << "\n";

	for (unsigned __int8 i = 0; i < 8; i++)
	{
		for (unsigned __int8 j = 0; j < 8; j++)
		{
			std::cout << black_name_from_coord[(7 - i) * 8 + j];
		}

		std::cout << "\n";
	}

	//for (int i= 0; i < 64; i++)
	//{
	//	std::cout << bitboard->white_name_from_coord[i];
	//	if( i%8 == 7)
	//{
	//		std::cout << "\n";
	//}
	//}
}

//	===================================================================
// использую для теста. запоминаем доску
void Chess_bitboard::save_test_bitboard
(
	const class Chess_bitboard & bitboard// представление доски
)
{

	all_piece = bitboard.all_piece;// все фигуры
	all_white_piece = bitboard.all_white_piece;// все белые фигуры
	all_black_piece = bitboard.all_black_piece;// все черные фигуры
	colour_move = bitboard.colour_move;
	taking_passage_yes = bitboard.taking_passage_yes;
	taking_passage_x = bitboard.taking_passage_x;
	taking_passage_y = bitboard.taking_passage_y;
	castling_Q = bitboard.castling_Q;
	castling_K = bitboard.castling_K;
	castling_q = bitboard.castling_q;
	castling_k = bitboard.castling_k;
	hash_key =  bitboard.hash_key;
	value_white_material = bitboard.value_white_material;
	value_black_material = bitboard.value_black_material;

	for (unsigned __int8 n =0; n < 7; n++ )
	{
		white_piece[n] = bitboard.white_piece[n];
		black_piece[n] = bitboard.black_piece[n];
	}

	for (unsigned __int8 k =0; k < 64; k++ )
	{
		white_name_from_coord[k] = bitboard.white_name_from_coord[k];
		black_name_from_coord[k] = bitboard.black_name_from_coord[k];
	}
}

//	===================================================================
// использую для теста. сравниваем доски
void Chess_bitboard::compare_test_bitboard
(
	const class Chess_bitboard & bitboard// представление доски
)
{
	// проверяем востановилась ли доска после прохождения глубины
	if (all_piece != bitboard.all_piece)
	{
		std::cout << "white test_all_piece != bitboard->all_piece" << "\n";// все фигуры
	}

	if (all_white_piece != bitboard.all_white_piece)
	{
		std::cout << "white test_all_white_piece != bitboard->all_white_piece" << "\n";// все белые фигуры
	}

	if (all_black_piece != bitboard.all_black_piece) 
	{
		std::cout << "white test_all_black_piece != bitboard->all_black_piece" << "\n";// все черные фигуры
	}

	if (colour_move != bitboard.colour_move)
	{
		std::cout << "white test_colour_move != bitboard->colour_move" << "\n";
	}

	if (taking_passage_yes != bitboard.taking_passage_yes)
	{
		std::cout << "white test_taking_passage_yes != bitboard->taking_passage_yes" << "\n";
	}

	if (taking_passage_x != bitboard.taking_passage_x)
	{
		std::cout << "white test_taking_passage_x != bitboard->taking_passage_x" << "\n";
	}

	if (taking_passage_y != bitboard.taking_passage_y)
	{
		std::cout << "white test_taking_passage_y != bitboard->taking_passage_y" << "\n";
	}

	if (castling_Q != bitboard.castling_Q)
	{
		std::cout << "white test_castling_Q != bitboard->castling_Q" << "\n";
	}

	if (castling_K != bitboard.castling_K)
	{
		std::cout << "white test_castling_K != bitboard->castling_K" << "\n";
	}

	if (castling_q != bitboard.castling_q)
	{
		std::cout << "white test_castling_q != bitboard->castling_q" << "\n";
	}

	if (castling_k != bitboard.castling_k)
	{
		std::cout << "white test_castling_k != bitboard->castling_k" << "\n";
	}

	if (hash_key !=  bitboard.hash_key)
	{
		std::cout << "white test_hash_key !=  bitboard->hash_key" << "\n";
	}

	if (value_white_material != bitboard.value_white_material)
	{
		std::cout << "white test_value_white_material != bitboard->value_white_material" << "\n";
	}

	if (value_black_material != bitboard.value_black_material)
	{
		std::cout << "white test_value_black_material != bitboard->value_black_material" << "\n";
	}



	for (unsigned __int8 n2 = 0; n2 < 7; n2++ )
	{
		if (white_piece[n2] != bitboard.white_piece[n2])
		{
			std::cout << "white test_white_piece[n] = bitboard->white_piece[n]" << n2 << "\n";
		}

		if (black_piece[n2] != bitboard.black_piece[n2])
		{
			std::cout << "white test_black_piece[n] = bitboard->black_piece[n]" << n2 << "\n";
		}
	}

	for (unsigned __int8 k2 = 0; k2 < 64; k2++ )
	{
		if (white_name_from_coord[k2] != bitboard.white_name_from_coord[k2])
		{
			std::cout << "white test_white_name_from_coord[k] = bitboard->white_name_from_coord[k]" << k2 << "\n";
		}

		if (black_name_from_coord[k2] != bitboard.black_name_from_coord[k2])
		{
			std::cout << "white test_black_name_from_coord[k] = bitboard->black_name_from_coord[k]" << k2 << "\n";
		}
	}

}


//	===================================================================
// печатаем битбоард доску
void Chess_bitboard::bitboard_print()
{
	std::ofstream Test_Bitboard;

	unsigned __int64 z = 0;

	Test_Bitboard.open ("_TEST_BITBOARD.txt",std::ios::app);

	Test_Bitboard << "bitboard->colour_move = "<< colour_move <<"\n";
	Test_Bitboard << "\n";

	Test_Bitboard << "taking_passage_yes= " << taking_passage_yes << "\n";
	Test_Bitboard << "bitboard->taking_passage_x= " << (int)taking_passage_x << "\n";
	Test_Bitboard << "taking_passage_y= " << (int)taking_passage_y << "\n";

	Test_Bitboard << "\n";
	Test_Bitboard << "castling_K= " << castling_K << "\n";
	Test_Bitboard << "castling_Q= " << castling_Q << "\n";
	Test_Bitboard << "castling_k= " << castling_k << "\n";
	Test_Bitboard << "castling_q= " << castling_q << "\n";

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
			z = all_white_piece & move_masks[j];

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
			z = all_black_piece & move_masks[j];

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
				z =  white_piece[p] & move_masks[j];

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
				z =  black_piece[p] & move_masks[j];

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