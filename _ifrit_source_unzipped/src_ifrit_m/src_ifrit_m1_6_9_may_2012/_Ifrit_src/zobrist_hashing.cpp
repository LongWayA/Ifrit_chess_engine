
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.05.10

#include <fstream>
#include <iostream>

#include "constants.h"
#include "move_generation_const.h"
#include "zobrist_hashing.h"

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
	std::ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	// в начале нам нужно заполнить вспомогательный массив случайных чисел 
	// связанных с расположением фигуры на доске в нем все фигуры на всех полях
	// это трехмерный массив координаты которого цвет, имя, координата фигуры
	// массив этот глобальный и содержит случайные числа
	// в дальнейшем складывая эти числа мы получим случайный ключ тем не менее
	// однозначно связанный с данной позицией
	// [цвет фигуры][имя фигуры][координата фигуры]
	// 2*6*64= 768 
	unsigned __int64 random_piese_key[2][MAX_NAME][MAX_COORDINATE];

	// массив для запоминания хеш-ключей позиций при погружении на глубину
	// т.е. у нас записываются не только ходы в pv_current но и ключи позиций
	// другими словами запоминаем ветку ключей позиций
	unsigned __int64 hash_three_moves[MAX_DEPTH];

	// массив для запоминания хеш-ключей позиций
	// при анализе входящей строки от оболочки
	// другими словами запоминаем ветку ключей позиций расчитанных по входящей строке от оболочки
	unsigned __int64 hash_string_moves[1000];

	// запоминаем количество ходов в строке
	unsigned __int16 save_cnt_move;



// РАБОТА С ХЕШ - КЛЮЧОМ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// каждой возможной позиции каждой возможной фигуры 
// присваиваем случайное число
void Zobrist_hashing::ini_random_piese_key()
{
	__int32 qq = 0;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
	Test_Hash.open ("_TEST_HASH.txt",std::ios::app);
	//Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	//[цвет фигуры][имя фигуры][координата фигуры]
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
	// ищем коллизии в самом генераторе
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
									Test_Hash << "Повтор " << qq << "\n";
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
// инициализируем ключ начальной позиции
void Zobrist_hashing::start_position_random_key
(
	struct Position & position// представление доски
)
{
	unsigned __int8  colour = 0;// цвет фигуры
	unsigned __int8  name = 0;// имя фигуры
	unsigned __int64 key = 0;// нулевое значение ключа

	//std::cout << "--------------------------------------------- " << "\n";

	// тестировал ключи
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
		name = position.white_name_from_coord[i];

		if (name != 0) key = key ^ random_piese_key[1][name][i];

		name = position.black_name_from_coord[i];

		if (name != 0)
		{
			key = key ^ random_piese_key[0][name][i];
			//std::cout << "black_name name = " << name << " i = " << i << "  random_piese_key[1][name][i] = " << random_piese_key[1][name][i] << "\n";
		}
	}

	// учитываем цвет хода
	// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
	// некоторые позиции отличаются только цветом хода
	if (position.colour_move) key = key ^ 8935749541958697872;

	// учитываем рокировки
	if (position.castling_K) key = key ^ 12923501711739627094;
	if (position.castling_Q) key = key ^ 1344597746125117634;
	if (position.castling_k) key = key ^ 1763078432891561173;
	if (position.castling_q) key = key ^ 15296769511885194289;

	// учитываем взятие на проходе
	if (position.taking_passage_yes)
	{
		key = key ^ 11782889425078278382;
		key = key ^ (position.taking_passage_x ^ 6567344619664815343);
		key = key ^ (position.taking_passage_y ^ 10614454250492056336);
	}//

	position.hash_key = key;

	//std::cout << "bitboard.hash_key = " << bitboard.hash_key << "\n";
	//std::cout << "   "<< "\n";
}

//	===================================================================
// меняем ключ при прямом ходе
void Zobrist_hashing::modification_random_key
(
	struct Position & position,// представление доски
	bool colour_figure,// цвет ходящей стороны
	unsigned __int8 i,// номер хода по списку
	struct List & list_surplus_moves,// представление списка ходов
	struct Undo & undo//информация для отката хода
)
{

// надо будет обдумать этот момент.
// warning C6385: Invalid data: accessing 'random_piese_key', 
// the readable size is '7168' bytes, but '-86016' bytes might be read: 
// Lines: 178, 179, 180, 181, 182, 183, 185, 186, 187, 189, 190, 191
//  , 205,    227, 229, 230, 232, 234

	unsigned __int64 key; // скорректированный хеш-ключ        warning Lines begin: 178 - 191
	unsigned __int8 name_figure;// имя фигуры которой ходим
	unsigned __int8 name_figure_capture;// имя фигуры которую побили
	unsigned __int8  initial_position;// начальная координата хода
	unsigned __int8  final_position;//   конечная координата хода
	unsigned __int8  type_move;//   тип хода

	key = position.hash_key;
	type_move = ((list_surplus_moves.move[i]>>6)& 63);
	name_figure_capture = (list_surplus_moves.move[i] & 63);

	name_figure = ((list_surplus_moves.move[i]>>24)& 7);
	initial_position = ((list_surplus_moves.move[i]>>12)& 63);
	final_position = ((list_surplus_moves.move[i]>>18)& 63); //warning Lines end: 191

	// ТЕСТ
	//if(
	//	   (name_figure >= MAX_NAME) 
	//	&& (initial_position >= MAX_COORDINATE)
	//	&& (final_position >= MAX_COORDINATE)
	//  )
	//{
	//	std::cout << "Ifrit: Zobrist_hashing::modification_random_key bug. I am terminated." << "\n";
	//	exit(1);
	//}

	// тут надо учесть тип хода
	switch (type_move)                                     // warning Lines: 205
	{
		// ход на пустое поле
		case SIMPLE_MOVE :

			//от общего хеш-ключа позиции отнимаем ключ фигуры начальной координаты
			//и прибавляем ключ фигуры конечной координаты
			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];

		break;

		// взятие
		case CAPTURE_MOVE :

			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];
			key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

		break;

		//рокировка 0-0-0
		case LONG_CASTLING :                                // warning Lines begin: 227 - 234

			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][name_figure][final_position];

			if (colour_figure)
			{
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][0]; // warning Lines end: 234
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][3];
			}
			else
			{
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][56];
				key = key ^ random_piese_key[(colour_figure)][N_ROOK][59];
			}

		break;

		//рокировка 0-0-------------------------
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

		// взятие на проходе
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

		// ходы пешки с превращением------------------------------
		case TRANSFORMATION_IN_KNIGHT : //превращение в конь 
		case TRANSFORMATION_IN_BISHOP : //превращение в слон
		case TRANSFORMATION_IN_ROOK   : //превращение в ладья 
		case TRANSFORMATION_IN_QUEEN  : //превращение в ферзь 

			//
			//
			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][(type_move - 10)][final_position];

		break;

		// взятие пешкой с превращением---------------------------------------------------------------------
		case CAPTURE_WITH_TRANSFORMATION_IN_KNIGHT : //взятие с превращением в конь 
		case CAPTURE_WITH_TRANSFORMATION_IN_BISHOP : //взятие с превращением в слон
		case CAPTURE_WITH_TRANSFORMATION_IN_ROOK   : //взятие с превращением в ладья 
		case CAPTURE_WITH_TRANSFORMATION_IN_QUEEN  : //взятие с превращением в ферзь 

			//
			//
			key = key ^ random_piese_key[colour_figure][name_figure][initial_position];
			key = key ^ random_piese_key[colour_figure][(type_move - 20)][final_position];
			key = key ^ random_piese_key[(!colour_figure)][(name_figure_capture)][final_position];

		break;

		default:;//ход не распознан
   
	}

	// учитываем цвет хода (меняем на каждом ходе)
	// дело в том что у белых есть этот бонус а у черных нет. сделанно это потому что
	// некоторые позиции отличаются только цветом хода
	key = key ^ 8935749541958697872;

	// корректируем флаги рокировки
	// идея такая что если флаг меняется то нужно подправить хеш-ключ
	if (position.castling_K != undo.return_castling_K) key = key ^ 12923501711739627094;//?
	if (position.castling_Q != undo.return_castling_Q) key = key ^ 1344597746125117634;
	if (position.castling_k != undo.return_castling_k) key = key ^ 1763078432891561173;
	if (position.castling_q != undo.return_castling_q) key = key ^ 15296769511885194289;

	// TEST
	//if (bitboard.castling_K != undo.return_castling_K) std::cout << "bitboard.castling_K = " << bitboard.castling_K 
	//<< "undo.return_castling_K = " << undo.return_castling_K<< "\n";
	//if (bitboard.castling_Q != undo.return_castling_Q) std::cout << "bitboard.castling_Q = " << bitboard.castling_Q 
	//<< "undo.return_castling_Q = " << undo.return_castling_Q<< "\n";
	//if (bitboard.castling_k != undo.return_castling_k)  std::cout << "bitboard.castling_k = " << bitboard.castling_k 
	//<< "undo.return_castling_k = " << undo.return_castling_k<< "\n";
	//if (bitboard.castling_q != undo.return_castling_q)  std::cout << "bitboard.castling_q = " << bitboard.castling_q 
	//<< "undo.return_castling_q = " << undo.return_castling_q<< "\n";

	// учитываем взятие на проходе
	// если оно было до хода значит его нужно убрать
	if (undo.return_taking_passage)
	{
		key = key ^ 11782889425078278382;
		key = key ^ (undo.taking_passage_x ^ 6567344619664815343);
		key = key ^ (undo.taking_passage_y ^ 10614454250492056336);
	}


	// учитываем взятие на проходе
	// если оно появилось после хода значит его нужно добавить
	if (position.taking_passage_yes)
	{
		key = key ^ 11782889425078278382;
		key = key ^ (position.taking_passage_x ^ 6567344619664815343);
		key = key ^ (position.taking_passage_y ^ 10614454250492056336);
	}

position.hash_key = key;

}



//	===================================================================
// генерируем случайный 64 битный ключ
unsigned __int64 Zobrist_hashing::rand_64()
{
	//в основе генерации случайных чисел лежит стандартная функция библиотеки
	//<cstdlib>	возвращающая псевдо случайное число в диапазоне 0 до 32767

	/*
	int rand ( void ); 
	Returns a pseudo-random integral number in the range 0 to RAND_MAX.
	RAND_MAX is a constant defined in <cstdlib>. Its default value may vary between 
	implementations but it is granted to be at least 32767.
	*/

	return rand() ^ ( ((unsigned __int64)rand() << 15) ^ ((unsigned __int64)rand() << 31) ^ 
		((unsigned __int64)rand() << 47) ^ ((unsigned __int64)rand() << 59));
}



// ПОВТОРЫ ПОЗИЦИЙ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// возвращаем количество ходов в строке игры переданной из оболочки
__int32 Zobrist_hashing::get_save_cnt_move()
{
	return save_cnt_move;
}

//	===================================================================
// инициализируем хеш-ключи повторов возрастающими числами что бы исключить ложное срабатывание
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры в текущем варианте
void Zobrist_hashing::ini_hash_three()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH ; i++)
	{
		hash_three_moves[i] = i;
	}
}

//	===================================================================
// инициализируем хеш-ключи повторов возрастающими числами что бы исключить ложное срабатывание
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры переданной из оболочки
void Zobrist_hashing::ini_hash_string_moves()
{
	for (unsigned __int16 i = 0; i < 1000 ; i++)
	{
		hash_string_moves[i] = i;
	}
}

//	===================================================================
// запоминаем хеш-ключ на текущей глубине
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры в текущем варианте
void Zobrist_hashing::save_hash_three
(
	unsigned __int64 key,// хеш-ключ(hash-key) 
	unsigned __int8 depth // глубина
)
{
	hash_three_moves[depth] = key;
}

//	===================================================================
// запоминаем хеш-ключ на текущей глубине
// (используем для поиска повторов позиций для определения ничьей)
// для строки игры переданной из оболочки
void Zobrist_hashing::save_hash_string_moves
(
	unsigned __int64 key,// хеш-ключ
	unsigned __int16 cnt_move // счетчик ходов в строке партии
)
{
	hash_string_moves[cnt_move] = key;
	save_cnt_move = cnt_move;
}

//	===================================================================
// анализируем нисходящую ветку ключей. если видим совпадение то возвращаем 1 иначе 0
// (используем для поиска повторов позиций для определения ничьей)
bool Zobrist_hashing::analysis_hash_string_moves
(
	unsigned __int8 depth // глубина
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
	работаем с правилом ничьей при трехкратном повторе позиции

	если глубина погружения 4 и выше то ищем повтор в ветке варианта
	если меньше то ищем повтор в строке игры переданной оболочкой.

	достаточно одного совпадения позиции что бы оценка стала ничейной
	таким образом присваиваем ноль уже при двухкратном повторе
	меня мучают смутные подозрения что тут я сильно упрощаю :)))
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