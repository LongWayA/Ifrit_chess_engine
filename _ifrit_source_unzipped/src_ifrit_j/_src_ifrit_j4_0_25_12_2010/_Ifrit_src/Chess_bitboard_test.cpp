//#include <cstring>
#include <iostream> // TEST

#include "Chess_bitboard.h"

//-----------------------------------------------------------------------------------
/* +
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 23.11.2010
*/
//-----------------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 

*/

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
