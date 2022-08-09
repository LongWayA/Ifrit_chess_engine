#include <cstring>
#include <iostream> // TEST

#include "ZObrist.h"

//-----------------------------------------------------------------------------------
/* +
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009   11.11.2009 21.11.2010 20.12.2010
*/
//-----------------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%

 задаем функции по работе с fen протоколом


 разбираем фен - строку и инициализируем внутреннюю доску 

 инициализирующая позицию строка может быть трех видов 
 "position startpos" просто начальная позиция
 "position startpos moves b1c3 b8c6 g1f3 g8f6" начальная позиция + ходы ведущие к текущей 
 "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " задание позиции через фен представление

 в результате работы класса мы получаем внутреннюю позицию соответствующую позиции
 закодированной в фен - представлении

*/

extern class ZObrist zobrist;

//	===================================================================
// разбираем все случаи фен и инициализируем доску
void Chess_bitboard::parse_position
(
	std::string string_in//входящая строка
)
{
	// инициализирующая позицию строка может быть трех видов 
	//"position startpos" просто начальная позиция
	//"position startpos moves b1c3 b8c6 g1f3 g8f6" начальная позиция + ходы ведущие к текущей 
	//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " задание позиции через фен представление

	__int8 move_string[]={' ',' ',' ',' ',' ',' '};// вспомогательный массив
	const std::string start_fen("position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");// стартовая фен-позиция

	unsigned __int8 cnt_move = 0;// счетчик ходов в строке партии нужно для кеширования строки

	if ( (int)string_in.compare(0,12,"position fen") == 0 )
	{
		//работаем с принятой фен-позицией
		fen_to_board(string_in);

	}
	else
	{
		// а это инициализация посредством команды startpos
		// реализуем так же как предыдущую но только сами задаем стартовую позицию в виде
		// start_fen = "position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		fen_to_board(start_fen);
	}

	// инициализируем массив ключей во избежании ложных срабатываний 
	// это массив ключей используемых для обнаружения трехкратного повторения позиций
	zobrist.ini_hash_string_moves(); 

	// по текущей позиции генерируем хеш - ключ
	zobrist.start_position_random_key(*this);

	// записываем хеш-ключ в массив хеш - ключей по строке игры
	zobrist.save_hash_string_moves(hash_key,cnt_move); 


	//---------------------------------------------

	// находим первое вхождение слова moves
    __int16 find = (int)string_in.find("moves");
	//std::cout << "find= " << find <<"\n";

	// ходы передаются в виде
	//position startpos moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
    //position fen n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1 moves g2h1q b7b8q h1e4 e2f2
	// g2h1q - превращение в ферзь , r- ладья b - слон n- конь   

    unsigned __int16 i = find;

	if (find != -1)
	{
		// проматываем moves и пробел 
		i = i + 6;

		// для примера возьмем g2h1q
		// до тех пор пока длины строки хватает для считывания хода
		while (i < string_in.length() - 3)	
		{ 
			// записываем букву поля с которого ходим в примере это g
			move_string[1] = string_in.at(i);
			//std::cout << "move_string[1]= " << move_string[1] <<"\n";

			//  записываем цифру поля с которого ходим в примере это 2
			move_string[2] = string_in.at(i + 1);
			//std::cout << "move_string[2]= " << move_string[2] <<"\n";

			//  записываем букву поля на которую ходим в примере это h
			move_string[3] = string_in.at(i + 2);
			//std::cout << "move_string[3]= " << move_string[3] <<"\n";

			//  записываем цифру поля на которую ходим в примере это 1
			move_string[4] = string_in.at(i + 3);
			//std::cout << "move_string[4]= " << move_string[4] <<"\n";

			// для справки скажу что
			// string_in.at(string_in.length()) - уже вызывает исключение выхода за пределы строки
			// и это очевидно ведь массив с нуля и индекс последнего элемента на 
			// единицу меньше чем количество элементов

			//  записываем фигуру превращения в примере это q обычно же поле пустое
			// (unsigned) - что бы компилятор не предупреждал
			if ( ( (unsigned)(i + 4) >= string_in.length() ) || string_in.at(i + 4) == ' ')
			{
				move_string[5] = '\0';
			}
			else
			{ 
				move_string[5] = string_in.at(i + 4);

				// если фигура превращения есть то прибавлять нужно на один символ больше
				i = i + 1;
			}

			//std::cout << "move_string[5]= " << move_string[5] <<"\n";

			//
			move_to_board(move_string);

			//---------------------------------------------
			cnt_move = cnt_move + 1;

			// по текущей позиции генерируем хеш - ключ
			zobrist.start_position_random_key(*this);

			// записываем хеш-ключ в массив хеш -ключей по строке игры
			// этот массив ключей используемых для обнаружения трехкратного повторения позиций
			zobrist.save_hash_string_moves(hash_key,cnt_move); 
			//---------------------------------------------

			// в примере g2h1q
			// сдвигаемся на 5 полей т.е. в примере g2h1 + пробел, 
			// а на символ фигуры превращения мы сдвинулись 
			// раньше  когда записывали q
            i = i + 5;

		}
	}

}

//	===================================================================
//разбираем фен - позицию и инициализируем движок используем подфункцию ch_koordinata_to_indeks
void Chess_bitboard::fen_to_board
(
	const std::string position_fen//строка содержащая фен позицию(длинна переменная)
)
{
	// работаем со строкой вида "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
	// символом / отделяются горизонтали шахматной доски
	// цифры означают количество пустых клеток на горизонтали
	// буквы это фигуры причем большие буквы значат белый цвет а маленькие черный

	//---------------------------------------------
	// инициализируем битбоард(bit board) доску 
	// (вообще то я уже написал для этого функцию но по моему так нагляднее)
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
					all_white_piece = all_white_piece 
						| move_masks[n];

					// добавляем короля в список белых королей
					white_piece[N_KING] 
						= white_piece[N_KING] 
						| move_masks[n];

					// задаем координату короля на доске
					white_name_from_coord[n] = N_KING;

				break;

				case 'Q':// белый ферзь

					// добавляем ферзя в список всех белых фигур
					all_white_piece = all_white_piece 
						| move_masks[n];

					// добавляем ферзя в список белых ферзей
					white_piece[N_QUEEN] 
						= white_piece[N_QUEEN] 
						| move_masks[n];

					// задаем координату ферзя на доске
					white_name_from_coord[n] = N_QUEEN;

				break;

				case 'R':// белая ладья

					// добавляем ладью в список всех белых фигур
					all_white_piece = all_white_piece 
						| move_masks[n];

					// добавляем ладью в список белых ладей
					white_piece[N_ROOK] 
						= white_piece[N_ROOK] 
						| move_masks[n];

					// задаем координату ладьи на доске
					white_name_from_coord[n] = N_ROOK;

				break;

				case 'B':// белый слон

					// добавляем слона в список всех белых фигур
					all_white_piece = all_white_piece 
						| move_masks[n];

					// добавляем слона в список белых слонов
					white_piece[N_BISHOP] 
						= white_piece[N_BISHOP] 
						| move_masks[n];

					// задаем координату слона на доске
					white_name_from_coord[n] = N_BISHOP;

				break;

				case 'N':// белый конь

					// добавляем коня в список всех белых фигур
					all_white_piece = all_white_piece 
						| move_masks[n];

					// добавляем коня в список белых коней
					white_piece[N_KNIGHT] 
						= white_piece[N_KNIGHT] 
						| move_masks[n];

					// задаем координату коня на доске
					white_name_from_coord[n] = N_KNIGHT;

				break;

				case 'P':// белая пешка

					// добавляем пешку в список всех белых фигур
					all_white_piece = all_white_piece 
						| move_masks[n];

					// добавляем пешку в список белых пешек
					white_piece[N_PAWN] 
						= white_piece[N_PAWN] 
						| move_masks[n];

					// задаем координату пешки на доске
					white_name_from_coord[n] = N_PAWN;

				break;

				//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				// черные

				case 'k'://черный король

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_KING] 
						= black_piece[N_KING] 
						| move_masks[n];

					black_name_from_coord[n] = N_KING;

				break;

				case 'q'://черный ферзь

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_QUEEN] 
					= black_piece[N_QUEEN] 
					| move_masks[n];

					black_name_from_coord[n] = N_QUEEN;

				break;

				case 'r'://черная ладья

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_ROOK] 
					= black_piece[N_ROOK] 
					| move_masks[n];

					black_name_from_coord[n] = N_ROOK;

				break;

				case 'b'://черный слон

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_BISHOP] 
						= black_piece[N_BISHOP] 
						| move_masks[n];

					black_name_from_coord[n] = N_BISHOP;

				break;

				case 'n'://черный конь

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_KNIGHT] 
					= black_piece[N_KNIGHT] 
					| move_masks[n];

					black_name_from_coord[n] = N_KNIGHT;

				break;

				case 'p'://черная пешка

					all_black_piece = all_black_piece 
						| move_masks[n];

					black_piece[N_PAWN] 
					= black_piece[N_PAWN] 
					| move_masks[n];

					black_name_from_coord[n] = N_PAWN;

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
	all_piece = all_white_piece | all_black_piece;

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
		colour_move = 1;
	}

	if (position_fen.at(i) == 'b')
	{
		colour_move = 0;
	}

	//	-----------------------------------------------------------------------------------	
	//	3 этап загоняем рокировку
	// пробелы
	i = i + 2;

	// сразу присваиваем запрет на рокировку который отменяем ниже если есть нужные символы типа К
	castling_Q = 0;
	castling_K = 0;
	castling_q = 0;
	castling_k = 0;

	while (position_fen.at(i) != ' ')
	{	
		if (position_fen.at(i) == 'K')
		{
			castling_K = 1;// короткая рокировка белых
		}

		if (position_fen.at(i) == 'Q')
		{
			castling_Q = 1;// длинная рокировка белых
		}

		if (position_fen.at(i) == 'k')
		{
			castling_k = 1;// короткая рокировка черных
		}

		if (position_fen.at(i) == 'q')
		{
			castling_q = 1;// длинная рокировка черных
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
	taking_passage_yes = 0;
	taking_passage_x   = 0;
	taking_passage_y   = 0;

	if (position_fen.at(i) != '-')
	{
		taking_passage_yes = 1;// разрешаем взятие
		taking_passage_x = koordinata_to_indeks(position_fen.at(i));// х координата(из буквы)
		i = i + 1;
		taking_passage_y = ch_koordinata_to_indeks(position_fen.at(i));// у координата 	
	}
	//---------------------------------------------------------------------------

}

//	===================================================================
//загоняем в движок мувики(moves) т.е. ходы приводящие к текущей позиции из стартовой
void Chess_bitboard::move_to_board
(
	__int8 move_string[]//строка содержащая ходы(7)
)
{
	struct Undo undo[1];//
	class List_surplus_moves list_surplus_moves_f;//список избыточных ходов

	// ходы передаются в виде
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	// просчитываем массив разрешенных ходов
	unsigned __int8 n = 8 * ch_koordinata_to_indeks(move_string[2]) 
		+ koordinata_to_indeks(move_string[1]);// откуда фигура ходит

	unsigned __int8 n_2 = 8 * ch_koordinata_to_indeks(move_string[4]) 
		+ koordinata_to_indeks(move_string[3]);// куда фигура ходит

	// рассчитываем все возможные ходы фигуры
	if (colour_move == 1)
	{
		list_surplus_moves_f.generator_captures_white(*this);
		list_surplus_moves_f.generator_move_white(*this);
	}
	else
	{
		list_surplus_moves_f.generator_captures_black(*this);
		list_surplus_moves_f.generator_move_black(*this);
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
			// досрочно выходим из цикла
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
		if (colour_move == 1)
		{
			onward_moves_white((unsigned __int8)n_3
				,undo,list_surplus_moves_f);
		}
		else
		{
			onward_moves_black((unsigned __int8)n_3
				,undo,list_surplus_moves_f);
		}
		//colour_move = !colour_move;

	}

}

//	===================================================================
//	по букве выдает индекс клетки(одна из самых первых моих функций :))	
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
//по символу цифры выдает цифру
//перешла еще из моего Джина на джаве(java) :)
//подобная функция встречалось в коде Анд чесс от 9.01.06 
//сохраню как историческую ценность :)) 
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
// начальная инициализация доски
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

	// медленней но очевидней
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
