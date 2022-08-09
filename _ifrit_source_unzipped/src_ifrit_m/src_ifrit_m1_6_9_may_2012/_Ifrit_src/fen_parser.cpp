
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009 11.11.2009 21.11.2010 20.12.2010 1.1.2011

#include <cstring>
#include <iostream>

#include "constants.h"
#include "fen_parser.h"
#include "move_generation_const.h"


//	===================================================================
//разбираем фен - позицию и инициализируем движок используем подфункцию ch_koordinata_to_indeks
void Fen_parser::fen_to_board
(
	struct Position & position,// представление доски
	const std::string position_fen//строка содержащая фен позицию(длинна переменная)
)
{
	// работаем со строкой вида "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	// символом / отделяются горизонтали шахматной доски
	// цифры означают количество пустых клеток на горизонтали
	// буквы это фигуры причем большие буквы значат белый цвет а маленькие черный

	//---------------------------------------------
	// инициализируем битбоард(bit board) доску 
	bitboard_ini(position);

	unsigned __int8 i = 13;// позиция курсора при просмотре фен - строки

	// TEST
	//std::cout << "--------------------" << "\n";
	//std::cout << position_fen.at(i-1) << "\n";
	//std::cout << position_fen.at(i) << "\n";
	//std::cout << position_fen.at(i+1) << "\n";
	//std::cout << "--------------------" << "\n";

	//----------------------------------------------------------------------------------	
	// 1 этап  разбор фен позиции - перевод строки в массив доски
	// так как скорость тут не нужна то для ввода используем двухмерное 
	// xy-представление доски
	// (на самом деле это осталось еще со времен двухмерной доски в движке :))
	unsigned __int8 y = 0;// вертикальная координата доски
	unsigned __int8 x = 0;// горизонтальная координата доски
	unsigned __int8 n = 0;// одномерная координата доски (когда доску отображаем на прямую)
	unsigned __int8 void_q;// количество пустых клеток

	// после прокручивания слова и пробела у нас осталась строка вида
	// 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1

	// в цикле мы обрабатываем подстроку вида  1k1r4/8/8/8/8/8/8/3R2K1
	// концом обработки считаем пробел
	// цикл по у т.е. пролистываем горизонтали
	while (position_fen.at(i) != ' ')
	{
		x = 0;

		//  --------------------
		//  цикл по х бежим по горизонтали
		while ((position_fen.at(i) != '/') && (position_fen.at(i) != ' '))
		{
			//  переводим двухмерные координаты ху в одномерную n
			n = 8 * (7 - y) + x;

			////////////////////////////////////////////////////////////////
			// расставляем белые и черные фигуры
			switch (position_fen.at(i))
			{
				case 'K':// белый король

					// добавляем короля в список всех белых фигур
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// добавляем короля в список белых королей
					position.white_piece[N_KING] 
						= position.white_piece[N_KING] 
						| move_masks[n];

					// задаем координату короля на доске
					position.white_name_from_coord[n] = N_KING;

				break;

				case 'Q':// белый ферзь

					// добавляем ферзя в список всех белых фигур
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// добавляем ферзя в список белых ферзей
					position.white_piece[N_QUEEN] 
						= position.white_piece[N_QUEEN] 
						| move_masks[n];

					// задаем координату ферзя на доске
					position.white_name_from_coord[n] = N_QUEEN;

				break;

				case 'R':// белая ладья

					// добавляем ладью в список всех белых фигур
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// добавляем ладью в список белых ладей
					position.white_piece[N_ROOK] 
						= position.white_piece[N_ROOK] 
						| move_masks[n];

					// задаем координату ладьи на доске
					position.white_name_from_coord[n] = N_ROOK;

				break;

				case 'B':// белый слон

					// добавляем слона в список всех белых фигур
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// добавляем слона в список белых слонов
					position.white_piece[N_BISHOP] 
						= position.white_piece[N_BISHOP] 
						| move_masks[n];

					// задаем координату слона на доске
					position.white_name_from_coord[n] = N_BISHOP;

				break;

				case 'N':// белый конь

					// добавляем коня в список всех белых фигур
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// добавляем коня в список белых коней
					position.white_piece[N_KNIGHT] 
						= position.white_piece[N_KNIGHT] 
						| move_masks[n];

					// задаем координату коня на доске
					position.white_name_from_coord[n] = N_KNIGHT;

				break;

				case 'P':// белая пешка

					// добавляем пешку в список всех белых фигур
					position.all_white_piece = position.all_white_piece 
						| move_masks[n];

					// добавляем пешку в список белых пешек
					position.white_piece[N_PAWN] 
						= position.white_piece[N_PAWN] 
						| move_masks[n];

					// задаем координату пешки на доске
					position.white_name_from_coord[n] = N_PAWN;

				break;

				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				// черные

				case 'k'://черный король

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_KING] 
						= position.black_piece[N_KING] 
						| move_masks[n];

					position.black_name_from_coord[n] = N_KING;

				break;

				case 'q'://черный ферзь

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_QUEEN] 
					= position.black_piece[N_QUEEN] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_QUEEN;

				break;

				case 'r'://черная ладья

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_ROOK] 
					= position.black_piece[N_ROOK] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_ROOK;

				break;

				case 'b'://черный слон

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_BISHOP] 
						= position.black_piece[N_BISHOP] 
						| move_masks[n];

					position.black_name_from_coord[n] = N_BISHOP;

				break;

				case 'n'://черный конь

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_KNIGHT] 
					= position.black_piece[N_KNIGHT] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_KNIGHT;

				break;

				case 'p'://черная пешка

					position.all_black_piece = position.all_black_piece 
						| move_masks[n];

					position.black_piece[N_PAWN] 
					= position.black_piece[N_PAWN] 
					| move_masks[n];

					position.black_name_from_coord[n] = N_PAWN;

				break;
			
				default:

					//-----------------------------------------------------------------
					// в комментариях рассмотрим случай когда у нас 8 пустых клеток
					// если зашло '8' то функция ch_koordinata_to_indeks выдает цифру 7 на же нужна цифра 8 
					// поэтому используем (1 +... 
					void_q = (1 + ch_koordinata_to_indeks(position_fen.at(i)));

					// обработали первую пустую клетку
					// и следующие 7 ( таким образом у нас обработаны 1 + 7 = 8 все пустые клетки ) 
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
		// важно не проскочить пробел
		if (position_fen.at(i) != ' ')
		{
			i = i + 1;
			y = y + 1;
		}
	  	
	}

	// находим все фигуры на доске
	position.all_piece = position.all_white_piece | position.all_black_piece;

	// теперь нам осталось обработать подстроку вида
	// w - - 0 1 или b KQkq e3 0 1 если есть рокировки и взятие на проходе
	//-----------------------------------------------------------------------------------	
	//2 этап загоняем очередь хода 	
	// проматываем пробел
	while (position_fen.at(i) == ' ')
	{
		i = i + 1;
	}

	// w - ход белых  b-ход черных
	if (position_fen.at(i) == 'w')
	{
		position.colour_move = 1;
	}

	if (position_fen.at(i) == 'b')
	{
		position.colour_move = 0;
	}

	//	-----------------------------------------------------------------------------------	
	//	3 этап загоняем рокировку
	// пробелы
	i = i + 2;

	// сразу присваиваем запрет на рокировку который отменяем ниже если есть нужные символы типа К
	position.castling_Q = 0;
	position.castling_K = 0;
	position.castling_q = 0;
	position.castling_k = 0;

	while (position_fen.at(i) != ' ')
	{	
		if (position_fen.at(i) == 'K')
		{
			position.castling_K = 1;// короткая рокировка белых
		}

		if (position_fen.at(i) == 'Q')
		{
			position.castling_Q = 1;// длинная рокировка белых
		}

		if (position_fen.at(i) == 'k')
		{
			position.castling_k = 1;// короткая рокировка черных
		}

		if (position_fen.at(i) == 'q')
		{
			position.castling_q = 1;// длинная рокировка черных
		}

		i = i + 1;
	}

	//	-----------------------------------------------------------------------------------
	//	 4 этап загоняем взятие на проходе
	/*
	напомним как кодируется битое поле:
	And after the move 1. e4:
	rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
	другими словами нам просто передают координаты битого поля (при ходе е4 битое поле е3)
	*/
	// проматываем пробел
	while(position_fen.at(i) == ' ')
	{
		i = i + 1;
	}

	// не забыть при дешифровке что у меня массив по у идет от нуля
	// а ведь нормальная нотация от 1	 
	// запретили взятие
	position.taking_passage_yes = 0;
	position.taking_passage_x   = 0;
	position.taking_passage_y   = 0;

	if (position_fen.at(i) != '-')
	{
		position.taking_passage_yes = 1;// разрешаем взятие
		position.taking_passage_x = koordinata_to_indeks(position_fen.at(i));// х координата(из буквы)
		i = i + 1;
		position.taking_passage_y = ch_koordinata_to_indeks(position_fen.at(i));// у координата 	
	}
	//---------------------------------------------------------------------------

}

//	===================================================================
//	по букве выдает индекс клетки(одна из самых первых моих функций :))	
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
//по символу цифры выдает цифру
//перешла еще из моего Джина на джаве(java) :)
//подобная функция встречалось в коде Анд чесс от 9.01.06 
//сохраню как историческую ценность :)) 
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
// начальная инициализация доски
void Fen_parser::bitboard_ini
(
	struct Position & position// представление доски
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
