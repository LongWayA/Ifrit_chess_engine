
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 12.06.2007  11.11.2009 19.12.2010 1.1.2011

#include <fstream>
#include <iostream>

#include "b_Protocol_UCI.h"
#include "c_Go.h"
#include "i_ZObrist.h"
#include "j_Hash.h"
#include "l_Chess_bitboard.h"
#include "m_List_surplus_moves.h"


class Go go;
class ZObrist zobrist;
class Hash hash;
class Chess_bitboard bitboard;


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
std::ofstream Test_FEN;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// разбираем входящие UCI команды от графической шахматной оболочки
void Protocol_UCI::parse_protocol_UCI
(
	const std::string string_in//входящая строка
)
{
	unsigned __int16 hash_size_Mb ;// размер хеш-таблицы в мегабайтах 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_P
	Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
	Test_FEN << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n";
	Test_FEN << string_in.c_str() << "\n";
	Test_FEN << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< "\n"; 
	Test_FEN.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
  
	// // >1:ucinewgame
	if ( (int)string_in.compare(0,10,"ucinewgame") == 0 )
	{ 

		// TEST
		//std::cout << "string_in.compare(ucinewgame) == 0" << "\n";

		//должна быть инициализация
		// возраст партии в 0
		hash.set_age_0();

		// инициализируем все ключи в хеш-таблице 0  
		hash.ini_hash_moves();

	}
	// выдается один раз при первом запуске движка
	// в арене выглядит так: 
	// >1:uci
	else if ( (int)string_in.compare(0,3,"uci") == 0 )// точное совпадение
	{

		// TEST
		//std::cout << "string_in.compare(uci) == 0" << "\n";

		std::cout << NAME_OF_PROGRAM_AND_DATA_OF_RELEASE << "\n";
		std::cout << "id author Brenkman Andrey" << "\n";

		//  std::cout << "option name Ponder type check default false"<< "\n";

		// задаем диапазон размера кеша и размер по умолчанию  в Мб
		std::cout << "option name Hash type spin default 1 min 1 max 1024" << "\n";

		//std::cout << "option name NalimovCache type spin min 1 max 16 default 2"<< "\n";
		//std::cout << "option name NalimovPath type string default <empty>"<< "\n";
		//std::cout << "option name MultiPV type spin min 1 max 20 default 1"<< "\n";
		//std::cout << "option name OwnBook type check default true"<< "\n"; 


		// инициализируем массив хеш - ключей фигур
		/* делаем это один раз 
		вообще по уму нужно заранее посчитать его в таблицу и озаботиться тем
		что бы не было повторных значений ключа а пока мы его генерируем в данном месте
		*/
		zobrist.ini_random_piese_key();

		//--------------------------------------------------------------------------
		// на всякий случай выделаем память под хеш-таблицу(по умолчанию 1 Мб)
		//(если вдруг не пройдет команда setoption)
		// и ведь пригодилась! дело в том что когда задаешь 1 мб то арена не выдает команды setoption
		hash.hash_size_ini_default(1);

		// инициализируем все ключи в хеш-таблице 0
		hash.ini_hash_moves();
		//--------------------------------------------------------------------------

		// возраст партии в 0
		hash.set_age_0();

		// да нам известен  uci протокол :)
		std::cout << "uciok \n";

	}
	// видимо выдается при каждой смене настроек движка в том числе и размера хеша
	// в арене выглядит так: 
	// >1:setoption name Hash value 2 (если размер таблицы задали в 2 мб)
	else if ( (int)string_in.compare(0,9,"setoption") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(setoption ) == 0" << "\n";

		// TEST
		// std::cout << "Read_hash_size_Mb(string_in) = " << Read_hash_size_Mb(string_in) << "\n";

		/*
		если считанный из входящей строки размер хеш-таблицы 
		удалось перевести в число(Hash_size(string_in) != -1) 
		то запоминаем его и выделяем память под хеш-таблицу
		*/

		// считываем установленный размер хеш-таблицы в Mb
		hash_size_Mb = read_hash_size_Mb(string_in);

		if (hash_size_Mb != 0)
		{
			//выделяем память под хеш-таблицу
			hash.hash_size_resize(hash_size_Mb);

			// инициализируем все ключи в хеш-таблице 0  
			hash.ini_hash_moves();
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
	else if ( (int)string_in.compare(0,7,"isready") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(isready) == 0" << "\n";

		// передаем что к работе готовы
		std::cout << "readyok \n" << "\n";

	}
	// выдается перед каждым ходом. тут передается позиция
	// в арене выглядит так: 
	// >1:position startpos (для стартовой позиции)
	// >1:position startpos moves e2e4 e7e5 g1f3 g8f6 f3e5 (для текущей партии)
	// >1:position fen rnb1kb1r/ppppqppp/5n2/8/8/3P1N2/PPP1QPPP/RNB1KB1R w KQkq - 0 1
	// (для произвольной позиции)
	else if ( (int)string_in.compare(0,8,"position") == 0 )
	{
		// TEST
		//std::cout << "string_in.compare(position) == 0" << "\n";

		bitboard.bitboard_ini();

		//  разбираем входящую позицию инициализируем внутреннюю доску движка   
		parse_position_FEN(string_in); 
 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  выводим полученную после разбора фен строки позицию
#if TEST_P
		Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
		Test_FEN<< "position ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
		Test_FEN << string_in.c_str() << "\n";
		Test_FEN<< "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<< "\n";
		bitboard.bitboard_print();
		Test_FEN.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}
	// команда на старт обсчета позиции
	// в арене выглядит так: 
	// >1:go depth 5 (для счета на заданную глубину - в данном случае 5 полуходов )
	// >1:go wtime 300000 btime 300000 (блиц 5 мин - первый ход)
	// >1:go movetime 5000 ( 5 секунд на ход)
	// >1:go infinite (анализ позиции)
	else if ( (int)string_in.compare(0,2,"go") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(go ) == 0" << "\n";

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
//  выводим полученную после разбора фен строки позицию
#if TEST_P
		Test_FEN.open ("_TEST_FEN.txt",std::ios::app);
		Test_FEN << "go /////////////////////////////////////////////"<< "\n";
		Test_FEN << string_in.c_str() << "\n";
		Test_FEN << "////////////////////////////////////////////////"<< "\n";
    
		Test_FEN.close();
#endif 
//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

		//  запускаем движок на расчет позиции
		go.parse_go(string_in);

	}
	// команда на выход
	// в арене выглядит так: 
	// >1:quit
	else if ( (int)string_in.compare(0,4,"quit") == 0 )
	{

		// TEST
		//std::cout << "string_in.compare(quit) == 0" << "\n";

		// освобождаем память из под хеш-таблицы
		hash.hash_size_free();

		// команда завершения потока выполнения
		exit(1);
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
	//else if (util.string_equal(string_in,"stop"))
	//{
	//	// команда стоп реализована мною в другом месте  
	//}
	else
	{
		//если ничего не распознали
		std::cout << "WARNING ! Unknown command" << "\n";
	}

}

//	===================================================================
// считывает в подстроке вида "Hash value 32" размер хеш-таблицы (в данном случае 32)
unsigned __int16 Protocol_UCI::read_hash_size_Mb
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
		//std::cout << "hash_value = " << hash_value <<"\n";

		//------------------------
		// ТЕСТ печатаем входящую строку

		//std::cout << " ТЕСТ печатаем входящую строку---------------";
		//std::cout << "\n";

		//for (unsigned int i = 0; i < string_in.length(); i++)
		//{
		//	std::cout << string_in.at(i);
		//}
		//std::cout << "\n";


		//// ТЕСТ печатаем выделенную подстроку

		//std::cout << " ТЕСТ печатаем выделенную подстроку---------------";
		//std::cout << "\n";

		//for (unsigned int i = 0; i < string_hash_value.length(); i++)
		//{
		//	std::cout << string_hash_value.at(i);
		//}
		//std::cout << "\n";
		//------------------------

		return hash_value;//
	}

	return 0;
}

//	===================================================================
// разбираем все случаи фен и инициализируем доску
void Protocol_UCI::parse_position_FEN
(
	const std::string string_in//входящая строка
)
{
	// инициализирующая позицию строка может быть трех видов 
	//"position startpos" просто начальная позиция
	//"position startpos moves b1c3 b8c6 g1f3 g8f6" начальная позиция + ходы ведущие к текущей 
	//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " задание позиции через фен представление

	__int8 move_string[] = {' ',' ',' ',' ',' ',' '};// вспомогательный массив
	const std::string start_fen("position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");// стартовая фен-позиция

	unsigned __int8 cnt_move = 0;// счетчик ходов в строке партии нужно для кеширования строки

	if ( (int)string_in.compare(0,12,"position fen") == 0 )
	{
		//работаем с принятой фен-позицией
		bitboard.fen_to_board(string_in);

	}
	else
	{
		// а это инициализация посредством команды startpos
		// реализуем так же как предыдущую но только сами задаем стартовую позицию в виде
		// start_fen = "position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		bitboard.fen_to_board(start_fen);
	}

	// инициализируем массив ключей во избежании ложных срабатываний 
	// это массив ключей используемых для обнаружения трехкратного повторения позиций
	zobrist.ini_hash_string_moves(); 

	// по текущей позиции генерируем хеш - ключ
	zobrist.start_position_random_key();

	// записываем хеш-ключ в массив хеш - ключей по строке игры
	zobrist.save_hash_string_moves(bitboard.get_hash_key(),cnt_move); 


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
			zobrist.start_position_random_key();

			// записываем хеш-ключ в массив хеш -ключей по строке игры
			// этот массив ключей используемых для обнаружения трехкратного повторения позиций
			zobrist.save_hash_string_moves(bitboard.get_hash_key(),cnt_move); 
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
//загоняем в движок мувики(moves) т.е. ходы приводящие к текущей позиции из стартовой
void Protocol_UCI::move_to_board
(
	const __int8 move_string[]//строка содержащая ходы
)
{
	class List_surplus_moves list_surplus_moves_f;//список избыточных ходов
	const __int8 depth = 0;//

	// ходы передаются в виде
	//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
	// просчитываем массив разрешенных ходов
	unsigned __int8 n = 8 * bitboard.ch_koordinata_to_indeks(move_string[2]) 
		+ bitboard.koordinata_to_indeks(move_string[1]);// откуда фигура ходит

	unsigned __int8 n_2 = 8 * bitboard.ch_koordinata_to_indeks(move_string[4]) 
		+ bitboard.koordinata_to_indeks(move_string[3]);// куда фигура ходит

	// рассчитываем все возможные ходы фигуры
	if (bitboard.get_colour_move() == 1)
	{
		list_surplus_moves_f.generator_captures_white(bitboard);
		list_surplus_moves_f.generator_move_white(bitboard);
	}
	else
	{
		list_surplus_moves_f.generator_captures_black(bitboard);
		list_surplus_moves_f.generator_move_black(bitboard);
	}

	__int16 n_3 = -1; // номер хода по списку

	// бежим по списку избыточных ходов
	for (unsigned __int8 i = 0; i < list_surplus_moves_f.get_end_list(); i++)
	{

		// если ход в списке совпадает с ходом из мувика(moves) тогда..
		if (( n == ( (list_surplus_moves_f.get_move(i) >> 12) & 63 ) ) && // совпадение откуда фигура ходит
		( n_2 == ((list_surplus_moves_f.get_move(i) >> 18) & 63) ) )  // совпадение куда фигура ходит
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
		if (bitboard.get_colour_move() == 1)
		{
			bitboard.do_moves_white((unsigned __int8)n_3
				,depth,list_surplus_moves_f);
		}
		else
		{
			bitboard.do_moves_black((unsigned __int8)n_3
				,depth,list_surplus_moves_f);
		}
		//colour_move = !colour_move;

	}

}

