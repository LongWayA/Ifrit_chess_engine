
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 12.06.2007  11.11.2009 19.12.2010 1.1.2011

#include <fstream>
#include <iostream>

#include "uci_gui_to_engine.h"
#include "uci_fen_parser.h"
#include "uci_go_parser.h"
#include "move_generation.h"
#include "make_move.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "test_chess_bitboard.h"
#include "uci_engine_to_gui.h"
#include "_version_engine.h"

/// включаем-выключаем(1-0)  режим вывода в 
/// файл(_TEST_UCI_PARSER.txt) тестовой информации
#define TEST_UCI_PARSER 0

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_UCI_PARSER
std::ofstream Test_uci_parser;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

// разбираем входящие UCI команды от графической шахматной оболочки(GUI)
__int32 Uci_gui_to_engine::parsing_command
(
	struct Position & position,// представление доски
	const std::string gui_string_command//входящая строка
)
{
	unsigned __int16 hash_size_Mb;// размер хеш-таблицы в мегабайтах 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_UCI_PARSER
	Test_uci_parser.open ("_TEST_UCI_PARSER.txt",std::ios::app);
	Test_uci_parser << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< std::endl;
	Test_uci_parser << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< std::endl;
	Test_uci_parser << string_in.c_str() << std::endl;
	Test_uci_parser << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< std::endl; 
	Test_uci_parser.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  
	// >1:ucinewgame
	if ( (int)gui_string_command.compare(0,10,"ucinewgame") == 0 )// точное совпадение
	{ 

		// TEST
		//std::cout << "string_in.compare(ucinewgame) == 0" << std::endl;

	}

	// выдается один раз при первом запуске движка
	// в арене выглядит так: 
	// >1:uci
	else if ( (int)gui_string_command.compare(0,3,"uci") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(uci) == 0" << std::endl;

		std::cout << NAME_AND_VERSION_ENGINE << std::endl;
		std::cout << "id author Brenkman Andrey" << std::endl;

		//  std::cout << "option name Ponder type check default false"<< std::endl;

		// задаем диапазон размера кеша и размер по умолчанию  в Мб
		std::cout << "option name Hash type spin default 1 min 1 max 1024" << std::endl;

		//std::cout << "option name NalimovCache type spin min 1 max 16 default 2"<< std::endl;
		//std::cout << "option name NalimovPath type string default <empty>"<< std::endl;
		//std::cout << "option name MultiPV type spin min 1 max 20 default 1"<< std::endl;
		//std::cout << "option name OwnBook type check default true"<< std::endl; 
		//option name Ponder type check default true
		// да нам известен  uci протокол :)
		std::cout << "uciok" << std::endl;

	}

	// видимо выдается при каждой смене настроек движка в том числе и размера хеша
	// в арене выглядит так: 
	// >1:setoption name Hash value 2 (если размер таблицы задали в 2 мб)
	else if ( (int)gui_string_command.compare(0,9,"setoption") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(setoption ) == 0" << std::endl;

		/*
		если считанный из входящей строки размер хеш-таблицы 
		удалось перевести в число то запоминаем его и 
		выделяем память под хеш-таблицу
		*/
		// считываем установленный размер хеш-таблицы в Mb
		hash_size_Mb = hash_value_Mb_string_to_int(gui_string_command);

		if (hash_size_Mb != 0)
		{
			//выделяем память под хеш-таблицу
			Transposition_table::hash_size_resize(hash_size_Mb);

			// инициализируем все ключи в хеш-таблице 0  
			Transposition_table::ini_hash_moves();
		}

		/*
		> setoption name Ponder value true
		> setoption name Hash value 16
		> setoption name NalimovCache value 4
		> setoption name NalimovPath value D:\tbs
		> setoption name Position Learning value true
		> setoption name BookFile value dbbook.bin
		> setoption name Book Size value Big
		> setoption name Selectivity value 5
		*/

	}

	// в арене выдается при каждой новой игре а вот во фрице похоже нет
	// в арене выглядит так: 
	// >1:isready
	else if ( (int)gui_string_command.compare(0,7,"isready") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(isready) == 0" << std::endl;

		// передаем что к работе готовы
		std::cout << "readyok " << std::endl;

	}

	// выдается перед каждым ходом. тут передается позиция
	// в арене выглядит так: 
	// >1:position startpos (для стартовой позиции)
	// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (для текущей партии)
	// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1
	// (для произвольной позиции)
	else if ( (int)gui_string_command.compare(0,8,"position") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(position) == 0" << std::endl;

		Uci_fen_parser::bitboard_ini(position);

		//  разбираем входящую позицию инициализируем внутреннюю доску движка   
		chess_position_string_to_internal_chess_board(position,gui_string_command); 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  выводим полученную после разбора фен строки позицию
#if TEST_UCI_PARSER
		Test_uci_parser.open ("_TEST_UCI_PARSER.txt",std::ios::app);
		Test_uci_parser << "position ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< std::endl;
		Test_uci_parser << string_in.c_str() << std::endl;
		Test_uci_parser << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< std::endl;
		Test_chess_bitboard::bitboard_print(position);
		Test_uci_parser.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}

	// команда на старт обсчета позиции
	// в арене выглядит так: 
	// >1:go depth 5 (для счета на заданную глубину - в данном случае 5 полуходов )
	// >1:go wtime 300000 btime 300000 (блиц 5 мин - первый ход)
	// >1:go movetime 5000 ( 5 секунд на ход)
	// >1:go infinite (анализ позиции)
	else if ( (int)gui_string_command.compare(0,2,"go") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(go ) == 0" << std::endl;
		return 1;

	}

	// команда на выход
	// в арене выглядит так: 
	// >1:quit
	else if ( (int)gui_string_command.compare(0,4,"quit") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(quit) == 0" << std::endl;

		// освобождаем память из под хеш-таблицы
		Transposition_table::hash_size_free();

		// команда завершения потока выполнения
		exit(1);
	}
	else if ( (int)gui_string_command.compare(0,4,"stop") == 0 )
	{ 
	}
	//------------------------------------------------------------------------------------
	// еще не реализовал
	//else if (util.string_start_with(string_in,"debug "))
	//{
	//	// режим отладки не знаю как использовать может и смогу 
	//	//нужен для лучшей отладки движка
	//	// dummy

	//}
	//else if (util.string_equal(string_in,"ponderhit"))
	//{
	//	// не знаю что это

	//	// в арене выглядит так: 
	//	//>1:stop
	//}

	else
	{
		//если ничего не распознали
		std::cout << "UCI WARNING! Unknown command" << std::endl;
	}

		return 0;

}

//	===================================================================
// считывает в подстроке вида "Hash value 32" размер хеш-таблицы (в данном случае 32)
unsigned __int16 Uci_gui_to_engine::hash_value_Mb_string_to_int
(
	const std::string string_in//входящая строка
)
{
	unsigned __int16 hash_value;

	// находим первое вхождение подстроки  Hash value
	__int16 find_hash_value = (int)string_in.find("Hash value");

	if (find_hash_value != -1)
	{
		// пропускаем надпись Hash value и выходим на число(размер хеш-таблицы)
		find_hash_value = find_hash_value + 11;

		// выделяем подстроку содержащую число
		std::string string_hash_value = string_in.substr(find_hash_value);

        // функция atoi переводит массив символов в целое число;
		hash_value = atoi(string_hash_value.c_str());

		//std::cout << "hash_value = " << hash_value <<std::endl;

		return hash_value;
	}

	return 0;
}

//	===================================================================
// разбираем шахматную позицию, закодированную в строке и инициализируем доску
void Uci_gui_to_engine::chess_position_string_to_internal_chess_board
(
	struct Position & position,// представление доски
	const std::string string_in//входящая строка
)
{
	// инициализирующая позицию строка может быть трех видов 
	// "position startpos" просто начальная позиция
	// "position startpos moves b1c3 b8c6 g1f3 g8f6" начальная позиция + ходы 
												//   ведущие к текущей 
	// "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " задание позиции 
													 //  через фен представление

	__int8 move_string[] = {' ',' ',' ',' ',' ',' '};// вспомогательный массив

	// стартовая фен-позиция
	const std::string start_fen(
		"position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
								);

	unsigned __int8 cnt_move = 0;// счетчик ходов в строке партии 
								 // нужно для кеширования строки

	if ( (int)string_in.compare(0,12,"position fen") == 0 )
	{
		//работаем с принятой фен-позицией
		Uci_fen_parser::fen_to_board(position,string_in);

	}
	else
	{
		// а это инициализация посредством команды startpos
		// реализуем так же как предыдущую но только сами задаем стартовую позицию в виде
		// start_fen = "position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		Uci_fen_parser::fen_to_board(position,start_fen);
	}

	// инициализируем массив ключей во избежании ложных срабатываний 
	// это массив ключей используемых для обнаружения трехкратного повторения позиций
	Zobrist_hashing::ini_hash_string_moves(); 

	// по текущей позиции генерируем хеш - ключ
	Zobrist_hashing::start_position_random_key(position);

	// записываем хеш-ключ в массив хеш - ключей по строке игры
	Zobrist_hashing::save_hash_string_moves(position.hash_key,cnt_move); 


	//---------------------------------------------

	// находим первое вхождение слова moves
    __int16 find = (int)string_in.find("moves");
	//std::cout << "find= " << find <<std::endl;

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
			// записываем букву поля с которого ходим
			// (в примере это g)
			move_string[1] = string_in.at(i);
			//std::cout << "move_string[1]= " << move_string[1] <<std::endl;

			// записываем цифру поля с которого ходим
			// (в примере это 2)
			move_string[2] = string_in.at(i + 1);
			//std::cout << "move_string[2]= " << move_string[2] <<std::endl;

			// записываем букву поля на которую ходим
			// (в примере это h)
			move_string[3] = string_in.at(i + 2);
			//std::cout << "move_string[3]= " << move_string[3] <<std::endl;

			//  записываем цифру поля на которую ходим
			// (в примере это 1)
			move_string[4] = string_in.at(i + 3);
			//std::cout << "move_string[4]= " << move_string[4] <<std::endl;

			// для справки скажу что
			// string_in.at(string_in.length()) - уже вызывает исключение 
			// выхода за пределы строки
			// и это очевидно ведь массив с нуля и индекс последнего элемента на 
			// единицу меньше чем количество элементов

			//  записываем фигуру превращения (в примере это q) обычно же поле пустое
			// (unsigned) - что бы компилятор не предупреждал
			if ( ( (unsigned)(i + 4) >= string_in.length() ) || string_in.at(i + 4) == ' ')
			{
				// нет фигуры превращения
				move_string[5] = '\0';
			}
			else
			{ 
				// записываем фигуру превращения (в примере это q)
				move_string[5] = string_in.at(i + 4);

				// учитываем что есть фигура превращения
				i = i + 1;
			}

			//std::cout << "move_string[5]= " << move_string[5] <<std::endl;

			// загоняем ход в позицию
			move_to_position(position,move_string);

			// по текущей позиции генерируем хеш - ключ
			Zobrist_hashing::start_position_random_key(position);

			// 
			cnt_move = cnt_move + 1;

			// записываем хеш-ключ в массив хеш -ключей по строке игры
			// этот массив ключей используемых для обнаружения 
			// трехкратного повторения позиций
			Zobrist_hashing::save_hash_string_moves(position.hash_key,cnt_move); 

			// для перехода к следующему ходу сдвигаемся на 5 полей 
			// потому что ход кодируется четырьмя или пятью в случае 
			// превращения пешки символами
			// а на символ фигуры превращения мы сдвинулись раньше
			// напомню что в примере мы разбираем g2h1q
			// т.е. в примере g2h1 + пробел это 5, 
			// а на символ фигуры превращения мы сдвинулись 
			// раньше  когда записывали q
            i = i + 5;

		}
	}
}

//	===================================================================
//загоняем в движок ходы приводящие к текущей позиции из стартовой
void Uci_gui_to_engine::move_to_position
(
	struct Position & position,// представление доски
	const __int8 move_string[]//строка содержащая ходы
)
{
	struct List list_surplus_moves; // список возможных ходов 
									// (всевозможные ходы из данного узла)
	const __int8 depth = 0;//

	// ходы передаются в виде
	// moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 

	// просчитываем массив разрешенных ходов
	// откуда фигура ходит
	unsigned __int8 move_from_square = 8 * Uci_fen_parser::ch_koordinata_to_indeks(move_string[2]) 
		+ Uci_fen_parser::koordinata_to_indeks(move_string[1]);

	// куда фигура ходит
	unsigned __int8 move_to_square = 8 * Uci_fen_parser::ch_koordinata_to_indeks(move_string[4]) 
		+ Uci_fen_parser::koordinata_to_indeks(move_string[3]);

	// не помню что тут делал
	//Move_generation::mobility_evaluation(position);

	// рассчитываем все возможные ходы фигуры
	if (position.colour_move == 1)
	{
		Move_generation::generator_captures_white(list_surplus_moves,position);
		Move_generation::generator_move_white(list_surplus_moves,position);
	}
	else
	{
		Move_generation::generator_captures_black(list_surplus_moves,position);
		Move_generation::generator_move_black(list_surplus_moves,position);
	}

	__int16 number_move_in_list = -1; // номер хода по списку

	// бежим по списку избыточных ходов
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_list; i++)
	{

		// если ход в списке совпадает с ходом из мувика(moves) тогда..
		if (( move_from_square == ( (list_surplus_moves.move[i] >> 12) & 63 ) ) && // совпадение откуда фигура ходит
		( move_to_square == ((list_surplus_moves.move[i] >> 18) & 63) ) )  // совпадение куда фигура ходит
		{
			/*
			в зависимости от того есть ли превращение мы корректируем индекс хода
			что бы понять этот момент нужно знать как передается превращение пешки
			в списке избыточных ходов
			ход пешкой на последнюю горизонталь один, но в генераторе 
			списка ходов он расписывается на все превращения
			так что под нормальным номером идет превращение в ферзь
			дальше идет превращения в ладью
			дальше идет превращения в слона
			дальше идет превращения в коня
			таким образом они и располагаются в списке
			*/
			switch (move_string[5])
			{
				// превращения нет поэтому просто передаем номер хода в списке
				case '\0' :
					// пустая клетка
					number_move_in_list = i;
				break;

				case 'q' :
					// превращение в ферзь
					number_move_in_list = i;
				break;

				case 'r' :
					// превращение в ладья
					number_move_in_list = i + 1;
				break;

				case 'b' :
					// превращение в слон
					number_move_in_list = i + 2;
				break;

				case 'n' :
					// превращение в конь
					number_move_in_list = i + 3;
				break;

				default:;//символ фигуры не распознан
   
			}

			// нужный ход нашли так что дальше бежать по циклу смысла нет
			// досрочно выходим из цикла
			break;
		}
	
	}

	// делаем найденный ход на внутренней доске
	if (number_move_in_list == -1)
	{
		std::cout << "WARNING ! moves error" << std::endl;
	}
	else
	{
		// делаем ход
		if (position.colour_move == 1)
		{
			Make_move::do_moves_white(position,(unsigned __int8)number_move_in_list
				,depth,list_surplus_moves);
		}
		else
		{
			Make_move::do_moves_black(position,(unsigned __int8)number_move_in_list
				,depth,list_surplus_moves);
		}

	}

}