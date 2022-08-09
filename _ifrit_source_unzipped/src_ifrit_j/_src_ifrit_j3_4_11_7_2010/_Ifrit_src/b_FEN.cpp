//#include <cstring>
#include <iostream> // TEST

#include "b_FEN.h"
#include "e_Move.h"
#include "d_Bitboard_gen.h"
#include "j_Hash.h"
#include "j_ZObrist.h"

//-----------------------------------------------------------------------------------
/* +
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009   11.11.2009
*/
//-----------------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%

 задаем функции по работе с fen протоколом
*/

//	===================================================================
// разбираем все случаи фен и инициализируем доску
void FEN::public_parse_position
(
	struct T_Bitboard * p_bitboard,//доска
	char string_in[]//входящая строка(65536)
)
{
	// инициализирующая позицию строка может быть трех видов 
	//"position startpos" просто начальная позиция
	//"position startpos moves b1c3 b8c6 g1f3 g8f6" начальная позиция + ходы ведущие к текущей 
	//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " задание позиции через фен представление

	char * fen;  //
	char * moves;//
	char * ptr;  // вспомогательный указатель
	char move_string[]={' ',' ',' ',' ',' ',' ',' '};// вспомогательный массив
	char * StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";// стартовая фен-позиция
	unsigned __int8 cnt_move = 0;// счетчик ходов в строке партии нужно для кеширования строки


	//получаем указатель на fen
	fen = strstr(string_in,"fen ");

	//получаем указатель на moves
	moves = strstr(string_in,"moves ");

	if (fen != NULL)
	{
		//работаем с принятой фен-позицией
		fen_to_board(p_bitboard,fen);
	}
	else
	{
		// а это инициализация посредством команды startpos
		// реализуем так же как предыдущую но только сами задаем стартовую позицию в виде
		// StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		fen_to_board(p_bitboard,StartFen);
	}

	//---------------------------------------------
	// счетчик ходов. использую для отображения ходов на ключи позиций
	cnt_move = 0;

	// инициализируем массив ключей во избежании ложных срабатываний 
	// это массив ключей используемых для обнаружения трехкратного повторения позиций
	ZOBRIST::public_ini_hash_string_moves(); 

	// по текущей позиции генерируем хеш - ключ
	ZOBRIST::public_start_position_random_key(p_bitboard);

	// записываем хеш-ключ в массив хеш - ключей по строке игры
	ZOBRIST::public_save_hash_string_moves(p_bitboard->hash_key,cnt_move); 
	//---------------------------------------------

	// ходы передаются в виде
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	if (moves != NULL)
	{
		// проматываем moves 
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
			{ // проматываем
				move_string[5] = *ptr++;
				move_string[6] = '\0';
			}

			move_to_board(p_bitboard,move_string);

			//---------------------------------------------
			cnt_move = cnt_move + 1;
			// по текущей позиции генерируем хеш - ключ
			ZOBRIST::public_start_position_random_key(p_bitboard);

			// записываем хеш-ключ в массив хеш -ключей по строке игры
			// этот массив ключей используемых для обнаружения трехкратного повторения позиций
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
//разбираем фен - позицию и инициализируем движок используем подфункцию ch_koordinata_to_indeks
void FEN::fen_to_board
(
	struct T_Bitboard * p_bitboard,//доска
	const char fen[]//строка содержащая фен позицию(длинна переменная)
)
{
	// работаем со строкой вида "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	// символом / отделяются горизонтали шахматной доски
	// цифры означают количество пустых клеток на горизонтали
	// буквы это фигуры причем большие буквы значат белый цвет а маленькие черный

	//---------------------------------------------
	// инициализируем битбоард(bit board) доску 
	// (вообще то я уже написал для этого функцию но по моему так нагляднее)
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

	unsigned __int8 i = 0;// позиция курсора при просмотре фен - строки

	// проматываем слово fen
	while (fen[i] != ' ')
	{
		i = i + 1;
	}


	// проматываем пробел
	while (fen[i] == ' ')
	{
		i = i + 1;
	}

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
	while (fen[i] != ' ')
	{
		x = 0;

		//  --------------------
		//  цикл по х бежим по горизонтали
		while ((fen[i] != '/') && (fen[i] != ' '))
		{
			//  переводим двухмерные координаты ху в одномерную n
			n = 8 * (7 - y) + x;

			////////////////////////////////////////////////////////////////
			// расставляем белые и черные фигуры
			switch (fen[i])
			{
				case 'K':// белый король

					// добавляем короля в список всех белых фигур
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// добавляем короля в список белых королей
					p_bitboard->white_piece[BITBOARDGENERATOR::N_KING] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_KING] 
						| move_masks_f[n];

					// задаем координату короля на доске
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_KING;

				break;

				case 'Q':// белый ферзь

					// добавляем ферзя в список всех белых фигур
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// добавляем ферзя в список белых ферзей
					p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN] 
						| move_masks_f[n];

					// задаем координату ферзя на доске
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_QUEEN;

				break;

				case 'R':// белая ладья

					// добавляем ладью в список всех белых фигур
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// добавляем ладью в список белых ладей
					p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] 
						| move_masks_f[n];

					// задаем координату ладьи на доске
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_ROOK;

				break;

				case 'B':// белый слон

					// добавляем слона в список всех белых фигур
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// добавляем слона в список белых слонов
					p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP] 
						| move_masks_f[n];

					// задаем координату слона на доске
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_BISHOP;

				break;

				case 'N':// белый конь

					// добавляем коня в список всех белых фигур
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// добавляем коня в список белых коней
					p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT] 
						| move_masks_f[n];

					// задаем координату коня на доске
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_KNIGHT;

				break;

				case 'P':// белая пешка

					// добавляем пешку в список всех белых фигур
					p_bitboard->all_white_piece = p_bitboard->all_white_piece 
						| move_masks_f[n];

					// добавляем пешку в список белых пешек
					p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN] 
						= p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN] 
						| move_masks_f[n];

					// задаем координату пешки на доске
					p_bitboard->white_name_from_coord[n] = BITBOARDGENERATOR::N_PAWN;

				break;

				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				// черные

				case 'k'://черный король

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_KING] 
						= p_bitboard->black_piece[BITBOARDGENERATOR::N_KING] 
						| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_KING;

				break;

				case 'q'://черный ферзь

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_QUEEN;

				break;

				case 'r'://черная ладья

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_ROOK;

				break;

				case 'b'://черный слон

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP] 
						= p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP] 
						| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_BISHOP;

				break;

				case 'n'://черный конь

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_KNIGHT;

				break;

				case 'p'://черная пешка

					p_bitboard->all_black_piece = p_bitboard->all_black_piece 
						| move_masks_f[n];

					p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN] 
					= p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN] 
					| move_masks_f[n];

					p_bitboard->black_name_from_coord[n] = BITBOARDGENERATOR::N_PAWN;

				break;
			
				default:

					//-----------------------------------------------------------------
					// в комментариях рассмотрим случай когда у нас 8 пустых клеток
					// если зашло '8' то функция ch_koordinata_to_indeks выдает цифру 7 на же нужна цифра 8 
					// поэтому используем (1 +... 
					void_q = (1 + ch_koordinata_to_indeks(fen[i]));

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
		if (fen[i] != ' ')
		{
			i = i + 1;
			y = y + 1;
		}
	  	
	}

	// находим все фигуры на доске
	p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

	// теперь нам осталось обработать подстроку вида
	// w - - 0 1 или b KQkq e3 0 1 если есть рокировки и взятие на проходе
	//-----------------------------------------------------------------------------------	
	//2 этап загоняем очередь хода 	
	// проматываем пробел
	while (fen[i] == ' ')
	{
		i = i + 1;
	}

	// w - ход белых  b-ход черных
	if (fen[i] == 'w')
	{
		p_bitboard->colour_move = 1;
	}

	if (fen[i] == 'b')
	{
		p_bitboard->colour_move = 0;
	}

	//	-----------------------------------------------------------------------------------	
	//	3 этап загоняем рокировку
	// пробелы
	i = i + 2;

	// сразу присваиваем запрет на рокировку который отменяем ниже если есть нужные символы типа К
	p_bitboard->castling_Q = 0;
	p_bitboard->castling_K = 0;
	p_bitboard->castling_q = 0;
	p_bitboard->castling_k = 0;

	while (fen[i] != ' ')
	{	
		if (fen[i] == 'K')
		{
			p_bitboard->castling_K = 1;// короткая рокировка белых
		}

		if (fen[i] == 'Q')
		{
			p_bitboard->castling_Q = 1;// длинная рокировка белых
		}

		if (fen[i] == 'k')
		{
			p_bitboard->castling_k = 1;// короткая рокировка черных
		}

		if (fen[i] == 'q')
		{
			p_bitboard->castling_q = 1;// длинная рокировка черных
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
	while(fen[i] == ' ')
	{
		i = i + 1;
	}

	// не забыть при дешифровке что у меня массив по у идет от нуля
	// а ведь нормальная нотация от 1	 
	// запретили взятие
	p_bitboard->taking_passage_yes = 0;
	p_bitboard->taking_passage_x   = 0;
	p_bitboard->taking_passage_y   = 0;

	if (fen[i] != '-')
	{
		p_bitboard->taking_passage_yes = 1;// разрешаем взятие
		p_bitboard->taking_passage_x = koordinata_to_indeks(fen[i]);// х координата(из буквы)
		i = i + 1;
		p_bitboard->taking_passage_y = ch_koordinata_to_indeks(fen[i]);// у координата 	
	}
	//---------------------------------------------------------------------------

}

//	===================================================================
//загоняем в движок мувики(moves) т.е. ходы приводящие к текущей позиции из стартовой
void FEN::move_to_board
(
	struct T_Bitboard * p_bitboard,//доска
	char move_string[]//строка содержащая ходы(7)
)
{
	struct T_undo undo[1];//
	struct T_list_surplus_moves list_surplus_moves_f;//список избыточных ходов

	// ходы передаются в виде
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	// просчитываем массив разрешенных ходов
	unsigned __int8 n = 8 * ch_koordinata_to_indeks(move_string[2]) 
		+ koordinata_to_indeks(move_string[1]);// откуда фигура ходит

	unsigned __int8 n_2 = 8 * ch_koordinata_to_indeks(move_string[4]) 
		+ koordinata_to_indeks(move_string[3]);// куда фигура ходит

	// рассчитываем все возможные ходы фигуры
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

	__int16 n_3 = -1; // номер хода по списку

	// бежим по списку избыточных ходов
	for (unsigned __int8 i = 0; i < list_surplus_moves_f.end_list; i++)
	{

		// если ход в списке совпадает с ходом из мувика(moves) тогда..
		if (( n == ( (list_surplus_moves_f.move[i] >> 12) & 63 ) ) && // совпадение откуда фигура ходит
		( n_2 == ((list_surplus_moves_f.move[i] >> 18) & 63) ) )  // совпадение куда фигура ходит
		{
			/*
			в зависимости от того есть ли превращение мы корректируем индекс хода
			что бы понять этот момент нужно знать как передается превращение пешки
			в списке избыточных ходов
			ход пешкой на последнюю горизонталь один но в генераторе 
			списка ходов он расписывается
			так что под нормальным номером идет превращение в ферзь
			потом следующими идут превращения в ладью слона и коня
			таким образом они и располагаются в списке
			*/
			switch (move_string[5])
			{
				// превращения нет поэтому просто передаем номер хода в списке
				case '\0' :
					// пустая клетка
					n_3 = i;
				break;

				case 'q' :
					// превращение в ферзь
					n_3 = i;
				break;

				case 'r' :
					// превращение в ладья
					n_3 = i + 1;
				break;

				case 'b' :
					// превращение в слон
					n_3 = i + 2;
				break;

				case 'n' :
					// превращение в конь
					n_3 = i + 3;
				break;

				default:;//символ фигуры не распознан
   
			}

			// нужный ход нашли так что дальше бежать по циклу смысла нет
			// досрочно выходим из цикла for(int i = 0; 
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
		// делаем ход
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
//	по букве выдает индекс клетки(одна из самых первых моих функций :))	
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
//по символу цифры выдает цифру
//перешла еще из моего Джина на джаве(java) :)
//подобная функция встречалось в коде Анд чесс от 9.01.06 
//сохраню как историческую ценность :)) 
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