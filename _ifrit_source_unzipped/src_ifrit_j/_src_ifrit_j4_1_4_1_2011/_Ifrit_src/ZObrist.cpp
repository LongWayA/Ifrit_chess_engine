#include <iostream>
#include <fstream>  // TEST

#include "ZObrist.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ������ 2.05.10
 * ��������� ���������
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

 ��������� ZObrist-�����
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
	std::ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


// ������ � ��� - ������ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// ������ ��������� ������� ������ ��������� ������ 
// ����������� ��������� �����
void ZObrist::ini_random_piese_key()
{
	__int32 qq = 0;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
	Test_Hash.open ("_TEST_HASH.txt",std::ios::app);
	//Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//[���� ������][��� ������][���������� ������]
	for (unsigned __int8 color_piece = 0; color_piece < 2; color_piece++)
	{
		for (unsigned __int8 name_piece = 1; name_piece < 7; name_piece++)
		{
			for (unsigned __int8 coordinate_piece = 0; 
			coordinate_piece < 64; coordinate_piece++)
			{
				random_piese_key[color_piece][name_piece][coordinate_piece] = rand_64();
				//Test_Hash << random_piese_key[color_piece][name_piece][coordinate_piece] << "\n";
			}
		}
	}

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
	// ���� �������� � ����� ����������
	for (int c = 0; c < 2; c++)
	{
		for (int n = 1; n < 7; n++)
		{
			for (int q = 0; q < 64; q++)
			{
				for (int cv = 0; cv < 2; cv++)
				{
					for (int nv = 1; nv < 7; nv++)
					{
						for (int qv = 0; qv < 64; qv++)
						{
							if ( (c != cv) || (n != nv) || (q != qv) )
							{
								if ( random_piese_key[c][n][q] 
								== random_piese_key[cv][nv][qv])
								{
									qq = qq + 1;
									Test_Hash << "������ " << qq << "\n";
								}
							}
						}
					}
				}
			}
		}
	}

#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
	Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}

//	===================================================================
// �������������� ���� ��������� �������
void ZObrist::start_position_random_key
(
	class Chess_bitboard & bitboard// ������������� �����                                    
)
{
	unsigned __int8  colour = 0;// ���� ������
	unsigned __int8  name = 0;// ��� ������
	unsigned __int64 key = 0;// ������� �������� �����

	//std::cout << "--------------------------------------------- " << "\n";

	// ���������� �����
	//for (int i = 0; i < 64; i++)
	//{    
	//	name = bitboard.white_name_from_coord[i];
	//	if (name != 0) 
	//	{
	//		key = key ^ random_piese_key[1][name][i];
	//		std::cout << "white_name name = " << name << " i = " << i << "  random_piese_key[1][name][i] = " << random_piese_key[1][name][i] << "\n";
	//	}

	//	//name = bitboard.black_name_from_coord[i];
	//	//if (name != 0) 
	//	//{
	//	//	key = key ^ random_piese_key[0][name][i];
	//	//	std::cout << "black_name name = " << name << "\n";
	//	//}
	//}

	for (unsigned __int8 i = 0; i < 64; i++)
	{
		name = bitboard.get_white_name_from_coord(i);

		if (name != 0) key = key ^ random_piese_key[1][name][i];

		name = bitboard.get_black_name_from_coord(i);

		if (name != 0)
		{
			key = key ^ random_piese_key[0][name][i];
			//std::cout << "black_name name = " << name << " i = " << i << "  random_piese_key[1][name][i] = " << random_piese_key[1][name][i] << "\n";
		}
	}

	// ��������� ���� ����
	// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
	// ��������� ������� ���������� ������ ������ ����
	if (bitboard.get_colour_move()) key = key ^ 0x7C0228DD624EC390ui64;

	// ��������� ���������
	if (bitboard.get_castling_K()) key = key ^ 0xB3598055CE64A656ui64;
	if (bitboard.get_castling_Q()) key = key ^ 0x12A8F886AF9418C2ui64;
	if (bitboard.get_castling_k()) key = key ^ 0x1877B67E57A764D5ui64;
	if (bitboard.get_castling_q()) key = key ^ 0xD4490AD346F14431ui64;

	// ��������� ������ �� �������
	if (bitboard.get_taking_passage_yes())
	{
		key = key ^ 0xA3853B80F17F58EEui64;
		key = key ^ (bitboard.get_taking_passage_x() ^ 0x5B23E522E4B1B4EFui64);
		key = key ^ (bitboard.get_taking_passage_y() ^ 0x934E1DE72D36D310ui64);
	}//

	bitboard.set_hash_key(key);

	//std::cout << "bitboard.hash_key = " << bitboard.hash_key << "\n";
	//std::cout << "   "<< "\n";
}

//	===================================================================
// ������ ���� ��� ������ ����
void ZObrist::modification_random_key
(
	bool colour_figure,// ���� ������� �������
	unsigned __int8 i,// ����� ���� �� ������
	class List_surplus_moves & list_surplus_moves,// ������ �����
	class Chess_bitboard & bitboard,// ������������� �����
	struct Undo & undo//���������� ��� ������ ����
)
{
	unsigned __int64 key; // ����������������� ���-����
	unsigned __int8 name_figure;// ��� ������ ������� �����
	unsigned __int8 name_figure_capture;// ��� ������ ������� ������
	unsigned __int8  initial_position;// ��������� ���������� ����
	unsigned __int8  final_position;//   �������� ���������� ����
	unsigned __int8  type_move;//   ��� ����

	key = bitboard.get_hash_key();
	type_move = ((list_surplus_moves.get_move(i)>>6)& 63);
	name_figure_capture = (list_surplus_moves.get_move(i) & 63);

	name_figure = ((list_surplus_moves.get_move(i)>>24)& 7);
	initial_position = ((list_surplus_moves.get_move(i)>>12)& 63);
	final_position = ((list_surplus_moves.get_move(i)>>18)& 63);

	// ����
	//if(
	//	   (name_figure >= MAX_NAME) 
	//	&& (initial_position >= MAX_COORDINATE)
	//	&& (final_position >= MAX_COORDINATE)
	//  )
	//{
	//	std::cout << "Ifrit: ZObrist::modification_random_key bug. I am terminated." << "\n";
	//	exit(1);
	//}

	// ��� ���� ������ ��� ����
	switch (type_move)
	{
		// ��� �� ������ ����
		case SIMPLE_MOVE :

			//�� ������ ���-����� ������� �������� ���� ������ ��������� ����������
			//� ���������� ���� ������ �������� ����������
			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];

		break;

		// ������
		case CAPTURE_MOVE :

			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];
			key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

		break;

		//��������� 0-0-0
		case LONG_CASTLING :

			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];

			if (colour_figure)
			{
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][0];
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][3];
			}
			else
			{
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][56];
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][59];
			}

		break;

		//��������� 0-0-------------------------
		case SHORT_CASTLING :

			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];

			if (colour_figure)
			{
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][7];
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][5];
			}
			else
			{
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][63];
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][61];
			}

		break;

		// ������ �� �������
		case TAKING_PASSAGE :

			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];

			if (colour_figure)
			{
				key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][(final_position - 8)];
			}
			else
			{
				key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][(final_position + 8)];
			}

		break;

		// ���� ����� � ������������------------------------------
		case TRANSFORMATION_IN_KNIGHT : //����������� � ���� 
		case TRANSFORMATION_IN_BISHOP : //����������� � ����
		case TRANSFORMATION_IN_ROOK   : //����������� � ����� 
		case TRANSFORMATION_IN_QUEEN  : //����������� � ����� 

			//
			//
			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][(type_move - 10)][final_position];

		break;

		// ������ ������ � ������������---------------------------------------------------------------------
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT : //������ � ������������ � ���� 
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP : //������ � ������������ � ����
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK   : //������ � ������������ � ����� 
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN  : //������ � ������������ � ����� 

			//
			//
			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][(type_move - 20)][final_position];
			key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

		break;

		default:;//��� �� ���������
   
	}

	// ��������� ���� ���� (������ �� ������ ����)
	// ���� � ��� ��� � ����� ���� ���� ����� � � ������ ���. �������� ��� ������ ���
	// ��������� ������� ���������� ������ ������ ����
	key = key ^ 0x7C0228DD624EC390ui64;

	// ������������ ����� ���������
	// ���� ����� ��� ���� ���� �������� �� ����� ���������� ���-����
	if (bitboard.get_castling_K() != undo.return_castling_K) key = key ^ 0xB3598055CE64A656ui64;//?
	if (bitboard.get_castling_Q() != undo.return_castling_Q) key = key ^ 0x12A8F886AF9418C2ui64;
	if (bitboard.get_castling_k() != undo.return_castling_k) key = key ^ 0x1877B67E57A764D5ui64;
	if (bitboard.get_castling_q() != undo.return_castling_q) key = key ^ 0xD4490AD346F14431ui64;

	// TEST
	//if (bitboard.castling_K != undo.return_castling_K) std::cout << "bitboard.castling_K = " << bitboard.castling_K 
	//<< "undo.return_castling_K = " << undo.return_castling_K<< "\n";
	//if (bitboard.castling_Q != undo.return_castling_Q) std::cout << "bitboard.castling_Q = " << bitboard.castling_Q 
	//<< "undo.return_castling_Q = " << undo.return_castling_Q<< "\n";
	//if (bitboard.castling_k != undo.return_castling_k)  std::cout << "bitboard.castling_k = " << bitboard.castling_k 
	//<< "undo.return_castling_k = " << undo.return_castling_k<< "\n";
	//if (bitboard.castling_q != undo.return_castling_q)  std::cout << "bitboard.castling_q = " << bitboard.castling_q 
	//<< "undo.return_castling_q = " << undo.return_castling_q<< "\n";

	// ��������� ������ �� �������
	// ���� ��� ���� �� ���� ������ ��� ����� ������
	if (undo.return_taking_passage)
	{
		key = key ^ 0xA3853B80F17F58EEui64;
		key = key ^ (undo.taking_passage_x ^ 0x5B23E522E4B1B4EFui64);
		key = key ^ (undo.taking_passage_y ^ 0x934E1DE72D36D310ui64);
	}


	// ��������� ������ �� �������
	// ���� ��� ��������� ����� ���� ������ ��� ����� ��������
	if (bitboard.get_taking_passage_yes())
	{
		key = key ^ 0xA3853B80F17F58EEui64;
		key = key ^ (bitboard.get_taking_passage_x() ^ 0x5B23E522E4B1B4EFui64);
		key = key ^ (bitboard.get_taking_passage_y() ^ 0x934E1DE72D36D310ui64);
	}

bitboard.set_hash_key(key);

}



//	===================================================================
// ���������� ��������� 64 ������ ����
unsigned __int64 ZObrist::rand_64()
{
	//� ������ ��������� ��������� ����� ����� ����������� ������� ����������
	//<cstdlib>	������������ ������ ��������� ����� � ��������� 0 �� 32767

	/*
	int rand ( void ); 
	Returns a pseudo-random integral number in the range 0 to RAND_MAX.
	RAND_MAX is a constant defined in <cstdlib>. Its default value may vary between 
	implementations but it is granted to be at least 32767.
	*/

	return rand() ^ ( ((unsigned __int64)rand() << 15) ^ ((unsigned __int64)rand() << 31) ^ 
		((unsigned __int64)rand() << 47) ^ ((unsigned __int64)rand() << 59));
}



// ������� ������� +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// ���������� ���������� ����� � ������ ���� ���������� �� ��������
__int32 ZObrist::get_save_cnt_move()
{
	return save_cnt_move;
}

//	===================================================================
// �������������� ���-����� �������� ������������� ������� ��� �� ��������� ������ ������������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� � ������� ��������
void ZObrist::ini_hash_three()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH ; i++)
	{
		hash_three_moves[i] = i;
	}
}

//	===================================================================
// �������������� ���-����� �������� ������������� ������� ��� �� ��������� ������ ������������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� ���������� �� ��������
void ZObrist::ini_hash_string_moves()
{
	for (unsigned __int16 i = 0; i < 1000 ; i++)
	{
		hash_string_moves[i] = i;
	}
}

//	===================================================================
// ���������� ���-���� �� ������� �������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� � ������� ��������
void ZObrist::save_hash_three
(
	unsigned __int64 key,// ���-����(hash-key) 
	unsigned __int8 depth // �������
)
{
	hash_three_moves[depth] = key;
}

//	===================================================================
// ���������� ���-���� �� ������� �������
// (���������� ��� ������ �������� ������� ��� ����������� ������)
// ��� ������ ���� ���������� �� ��������
void ZObrist::save_hash_string_moves
(
	unsigned __int64 key,// ���-����
	unsigned __int16 cnt_move // ������� ����� � ������ ������
)
{
	hash_string_moves[cnt_move] = key;
	save_cnt_move = cnt_move;
}

//	===================================================================
// ����������� ���������� ����� ������. ���� ����� ���������� �� ���������� 1 ����� 0
// (���������� ��� ������ �������� ������� ��� ����������� ������)
bool ZObrist::analysis_hash_string_moves
(
	unsigned __int8 depth // �������
)
{

	// TEST--------------------------------------------------------
	//if (depth == 2)
	//{
	//	
	//	// TEST-------------
	//	std::cout << "depth= "
	//	     << depth
	//	     <<"\n";
	//	for (int i = 0; i <= depth; i++ )
	//	{
	//		std::cout << "i= "
	//		     << i
	//		     << " hash_three_moves[i]= "
	//		     << hash_three_moves[i]
	//		     <<"\n";
	//	}
	//	// TEST-------------
	//	
	//	
	//	// TEST-------------
	//	for (int i = 0; i <= save_cnt_move; i++ )
	//	{
	//		std::cout << "i= "
	//		     << i
	//		     << " hash_string_moves[i]= "
	//		     << hash_string_moves[i]
	//		     <<"\n";
	//	}
	//	// TEST-------------
	//	
	//}
	// TEST--------------------------------------------------------

	/*
	�������� � �������� ������ ��� ����������� ������� �������

	���� ������� ���������� 4 � ���� �� ���� ������ � ����� ��������
	���� ������ �� ���� ������ � ������ ���� ���������� ���������.

	���������� ������ ���������� ������� ��� �� ������ ����� ��������
	����� ������� ����������� ���� ��� ��� ����������� �������
	���� ������ ������� ���������� ��� ��� � ������ ������� :)))
	*/

	if (depth > 3)
	{
		for (unsigned __int8 j = 4; (depth - j) >= 0; j = j + 2 )
		{
			if (hash_three_moves[depth] == hash_three_moves[depth - j])
			{
       // // TEST--------------------------------------------------------
       //      std::cout << " DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD "
       //           << "\n";
       //      std::cout << "depth= "
       //           << depth
       //           <<"\n";
       //      
       //      std::cout << "j= "
       //           << j
       //           <<"\n";
       //      // TEST-------------
       //      for (int i = 0; i <= depth; i++ )
				//{
       //           std::cout << "i= "
       //                << i
       //                << " hash_three_moves[i]= "
       //                << hash_three_moves[i]
       //                <<"\n";
       //      }
       //      // TEST-------------
       //// TEST--------------------------------------------------------

				return 1;
			}
		}
	}
	else
	{
		for (__int32 k = 4; (depth - k + save_cnt_move) >= 0; k = k + 2)
		{
			if (hash_three_moves[depth] == hash_string_moves[ depth - k + save_cnt_move ])
			{
				//// TEST--------------------------------------------------------
				//std::cout << " CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC "
				//	<< "\n";
				//std::cout << "depth= "
				//	<< depth
				//	<<"\n";
				//// TEST-------------
				//for (int i = 0; i <= depth; i++ )
				//{
				//	std::cout << "i= "
				//		<< i
				//		<< " hash_three_moves[i]= "
				//		<< hash_three_moves[i]
				//		<<"\n";
				//}
				//std::cout << "  "
				//	<< "\n";

				//for (int i = 0; i <= save_cnt_move; i++ )
				//{
				//	std::cout << "i= "
				//		<< i
				//		<< " hash_string_moves[i]= "
				//		<< hash_string_moves[i]
				//		<<"\n";
				//}
				//// TEST-------------
				//// TEST--------------------------------------------------------

				return 1;
			}
		}	
	}
	return 0;
}