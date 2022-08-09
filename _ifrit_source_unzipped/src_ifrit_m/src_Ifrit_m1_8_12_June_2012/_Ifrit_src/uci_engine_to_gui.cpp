
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.5.2012

#include <fstream>
#include <iostream>

#include "uci_engine_to_gui.h"
#include "uci_gui_to_engine.h"
#include "uci_fen_parser.h"
#include "uci_go_parser.h"
#include "move_generation.h"
#include "make_move.h"
#include "zobrist_hashing.h"
#include "transposition_table.h"
#include "test_chess_bitboard.h"
#include "pv_save.h"
#include "time_management.h"

#define TEST_UCI_INFO 0 // включаем-выключаем(1-0) режим 
						// тестирования вывода информации о счете

#if TEST_UCI_INFO
// TEST
std::ofstream Test_info;
#endif//#if TEST_UCI_INFO


__int32 save_value; /// запоминаем оценку последнего выведенного варианта
unsigned __int8 save_Depth_Max; /// запоминаем глубину последнего выведенного варианта

/*
 делаем ход 
 он передается в виде <  <1:bestmove e2e4
 или при превращении в коня <1:bestmove d7d8N

В переменной move упаковано, откуда ходим, 
куда ходим, тип хода, есть ли превращение.
В этой функции мы это распаковываем и печатаем в выходной поток.
Подробно о способе упаковки см. a_move_list_struct.h
*/
void Uci_engine_to_gui::do_bestmove(__int32 bestmove)
{
	std::cout << "bestmove ";
	std::cout << x_to_notation(((bestmove >> 12) & 63) % 8);
	std::cout << (1 + ((bestmove >> 12) & 63) / 8 );
	std::cout << x_to_notation(((bestmove >> 18) & 63) % 8);
	std::cout << (1 + ((bestmove >> 18) & 63) / 8);

	// считываем тип хода
	__int32 d_m = ((bestmove >> 6) & 63);

	// если d_m больше 5 то значит имеем превращение пешки
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			// превращение пешки с взятием
			std::cout << int_to_char_board((d_m - 20),0);
		}
		else
		{
			// превращение пешки
			std::cout << int_to_char_board((d_m - 10),0);
		}
	}

	//std::cout << std::endl;
	std::cout << std::endl;

	//if (PV_save::get_pv_best().depth_max > 1)
	//{
	//	// печатаем ponder
	//	// второй ход в линии варианта
	//	__int32 bestmove = PV_save::get_pv_best().move[1];

	//	std::cout << " ponder ";
	//	std::cout << x_to_notation(((bestmove >> 12) & 63) % 8);
	//	std::cout << (1 + ((bestmove >> 12) & 63) / 8 );
	//	std::cout << x_to_notation(((bestmove >> 18) & 63) % 8);
	//	std::cout << (1 + ((bestmove >> 18) & 63) / 8);

	//	// считываем тип хода
	//	__int32 d_m = ((bestmove >> 6) & 63);

	//	// если d_m больше 5 то значит имеем превращение пешки
	//	if (d_m > 5)
	//	{
	//		if (d_m > 15)
	//		{
	//			// превращение пешки с взятием
	//			std::cout << int_to_char_board((d_m - 20),0);
	//		}
	//		else
	//		{
	//			// превращение пешки
	//			std::cout << int_to_char_board((d_m - 10),0);
	//		}
	//	}
	//}
	//	std::cout << std::endl;
//std::endl
}

/*
 печатаем информацию в поток
 в виде

 <1:info depth 2 seldepth 2 score cp 12 nodes 160 nps 5161 time 31 pv g1f3 g8f6
         глубина продления оценка       узлы      скорость время   линия варианта
*/
void Uci_engine_to_gui::print_pv_line
(
	struct Position & position,// представление доски
	const __int64 nodes,// количество рассмотренных узлов(позиций)
	const double time_current,  // сколько времени прошло при данной итерации
	const unsigned __int8 depth,
	bool & flag_mate,// найден мат 
	const __int64 nodes_y
 )
{

	__int8 inv;// флаг цвета хода
	__int32 move_n;

	__int32 value_root = PV_save::get_pv_best().score_move;

// TEST
#if TEST_UCI_INFO
	Test_info.open ("_TEST_UCI_INFO.txt",std::ios::app);

	Test_info << " " << std::endl;
	Test_info << "-------------------------------------" << std::endl;

	Test_info << " depth " << (__int32)depth << std::endl;
	Test_info << " seldepth " << (__int32)PV_save::get_pv_best().depth_max << std::endl;

	position.colour_move == 0 ? inv = -1 : inv = 1;

	// печатаем оценку. для мата специальное представление в виде дистанции
	if (inv * value_root > 900000)
	{ 
		Test_info  << " score mate "<< (__int32)PV_save::get_pv_best().depth_max;// дистанция до мата в полуходах
	}
	else if (inv * value_root < -900000)
	{
		Test_info  << " score mate "<< -(__int32)PV_save::get_pv_best().depth_max;// дистанция до мата в полуходах
	}
	else
	{
		// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
		Test_info  << " score cp "<< inv * value_root << std::endl;
	}

	Test_info << " nodes_y "<< (__int32)nodes_y << std::endl;
	Test_info << " nodes "<< (__int32)nodes << std::endl;
	Test_info << " nps " << (__int32)(nodes / time_current) << std::endl;
	Test_info << " time ms " << 
		(__int32) (time_current * Time_management::get_clocks_per_sec()) << std::endl;
	Test_info << " time double " << time_current << std::endl;

	// печатаем линию варианта
	Test_info << " pv ";

	for (unsigned __int8 n = 0; n < PV_save::get_pv_best().depth_max; n++)
	{
		move_n = PV_save::get_pv_best().move[n];
		Test_info << x_to_notation(((move_n >> 12) & 63) % 8);
		Test_info << (1 + ((move_n >> 12) & 63) / 8);
		Test_info << x_to_notation((( move_n >> 18) & 63) % 8);
		Test_info << (1 + ((move_n >> 18) & 63) / 8);

		// считываем тип хода
		__int32 d_m = ((move_n >> 6) & 63);

		// если d_m больше 5 то значит имеем превращение пешки
		if (d_m > 5)
		{
			if (d_m > 15)
			{
				Test_info << int_to_char_board((d_m - 20),0);// превращение пешки с взятием
			}
			else
			{
				Test_info << int_to_char_board((d_m - 10),0);// просто превращение пешки
			}
		}
		Test_info << " ";
	}
	Test_info << std::endl;

	Test_info.close();
#endif//#if TEST_UCI_INFO


	std::cout << "info " ;

	//глубина
	std::cout << " depth " << (__int32)depth;

	// глубина форсированного варианта
	std::cout << " seldepth " << (__int32)PV_save::get_pv_best().depth_max;

	// у меня оценка абсолютная(всегда со стороны белых) 
	// а UCI требует со стороны ходящего
	position.colour_move == 0 ? inv = -1 : inv = 1;

	// печатаем оценку. для мата специальное представление в виде дистанции
	if (inv * value_root > 900000)
	{ 
		flag_mate = 1;

		// дистанция до мата в полуходах
		std::cout << " score mate " << 
			(__int32)(PV_save::get_pv_best().depth_max / 2);
	}
	else if (inv * value_root < -900000)
	{
		flag_mate = 1;

		// дистанция до мата в полуходах
		std::cout << " score mate " << 
			-(__int32)(PV_save::get_pv_best().depth_max / 2);
	}
	else
	{
		flag_mate = 0;

		// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
		std::cout << " score cp " << inv * value_root;
	}
 
	// для теста матовой оценки
	// std::cout << " score cp " << inv * value_root;

	// полное количество узлов
	std::cout << " nodes " << (__int32)nodes; 

	// количество узлов на слое
	//std::cout << " nodes "<< (__int32)nodes_y;

	// скорость из расчета по всем узлам на данной глубине
	std::cout << " nps " << (__int32)(nodes / time_current);

	// время счета по всем узлам на данной глубине в ms
	std::cout << " time " << 
		(__int32) (time_current * Time_management::get_clocks_per_sec());

	// печатаем линию варианта
	std::cout << " pv ";

	for (unsigned __int8 n = 0; n < PV_save::get_pv_best().depth_max; n++)
	{
		move_n = PV_save::get_pv_best().move[n];

		std::cout << x_to_notation(((move_n >> 12) & 63) % 8);
		std::cout << (1 + ((move_n >> 12) & 63) / 8);
		std::cout << x_to_notation((( move_n >> 18) & 63) % 8);
		std::cout << (1 + ((move_n >> 18) & 63) / 8);

		// считываем тип хода
		__int32 d_m = ((move_n >> 6) & 63);

		// если d_m больше 5 то значит имеем превращение пешки
		if (d_m > 5)
		{
			if (d_m > 15)
			{
				// превращение пешки с взятием
				std::cout << int_to_char_board((d_m - 20),0);
			}
			else
			{
				// просто превращение пешки
				std::cout << int_to_char_board((d_m - 10),0);
			}
		}
		std::cout << " ";
	}

	//std::cout << std::endl;
	std::cout << std::endl;
}


/*
 выводим текущий ход в оболочку
*/
void Uci_engine_to_gui::print_currmove
(
	const struct List & list_surplus_moves,//список возможных ходов
	const unsigned __int8 nodes_0,// количество ходов на нулевой глубине
	const unsigned __int8 i,// номер хода по списку
	const unsigned __int8 Depth_Max// максимальная глубина
)
{
	// рассчитываемый в момент печати ход
	std::cout << "info currmove " ;
	std::cout <<x_to_notation(((list_surplus_moves.move[i]>>12)& 63)%8);
	std::cout <<(1 + ((list_surplus_moves.move[i]>>12)& 63)/8);
	std::cout <<x_to_notation(((list_surplus_moves.move[i]>>18)& 63)%8);
	std::cout <<(1 + ((list_surplus_moves.move[i]>>18)& 63)/8);
	// порядковый номер рассчитываемого хода
	std::cout << " currmovenumber " ;
	std::cout << (__int32)nodes_0;
	// печатаем информацию о глубине просмотра
	std::cout << " depth " << (__int32)Depth_Max;
	std::cout << std::endl;
	std::cout.flush();
}


/*
 выводим текущий вариант в оболочку
*/
void Uci_engine_to_gui::print_pv_line_s
(
	const bool colour_move,// цвет печатающей стороны
	const struct List & list_surplus_moves,//список возможных ходов
	const struct PV_line & PV_best_point,// лучший вариант
	const __int64 print_nodes,// количество узлов
	const unsigned __int8 i,// номер хода в списке
	const unsigned __int8 Depth_Max,// максимальная глубина
	const __int32 value,// оценка позиции
    const bool all_line// вывод всех линий
		//,const double time_nodes// время расчета варианта
)
{
	__int32 inv = 0;// учитываем цвет ходящей стороны
	__int32 d_m = 0;// обрабатываем случай превращения пешки

	/*
	// Here are examples of the possible info output: 
	< info currmove e8e7 (current calculated move) +
	< info currmovenumber 29 +
	< info nps 291506 (nodes per sec, Fritz calculates the nps itself)
	< info tbhits 830
	< ino depth 9
	< info score cp12 (in centipawns)
	< info score mate 5 (or mate -5)
	< info nodes 288973
	< info time 11192 (milliseconds)
	< info pv e2e4 e7e5 g1f3 b8c6 f1d3 f8c5 e1g1 g8e7 b1c3 
	*/

	if (save_value == value)
	{
		std::cout <<std::endl;
	}

	// выводим вариант только в том случае если лучшая оценка а значит и лучший
	// вариант изменились что бы не было вывода одного и того же сто раз 

	// если оценка изменилась или изменилась глубина то выводим вариант
	if (all_line ||
		((save_value != value) // изменилась оценка
		||(save_Depth_Max != Depth_Max)) // изменилась глубина
		)
	{
		save_value = value;
		save_Depth_Max = Depth_Max;


		std::cout << "info " ;
		//std::cout << " nps "<<(int)(nodes/time_nodes);
		std::cout << " depth "<< (__int32)Depth_Max;
		//печатаем информацию о глубине форсированного варианта
		std::cout << " seldepth "<< (__int32)PV_best_point.depth_max;

		//печатаем информацию о оценке позиции(это лучшая на данный момент оценка связанна с лучшим вариантом)
		colour_move == 0 ? inv = -1 : inv = 1;

		if (inv * value > 900000)
		{
			std::cout << " score mate " << (__int32)PV_best_point.depth_max;// дистанция до мата в полу ходах
		} 
		else if(inv * value < -900000)
		{
			std::cout << " score mate " << -(__int32)PV_best_point.depth_max;// дистанция до мата в полу ходах
		}
		else
		{
			std::cout << " score cp " << inv * value;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
		}

		//печатаем информацию о количестве просмотренных узлов
		std::cout << " nodes "<<(__int32)print_nodes;

		//сравниваю по модулю потому что из за продлений на шахах запарки со знаком
		if ( (value * value) != (PV_best_point.score_move * PV_best_point.score_move) ) 
		//if(0)
		{
			std::cout << std::endl;
			std::cout << "ERROR LINE " ;
			std::cout << std::endl;
			std::cout << " value = " << value;
			std::cout << std::endl;
			std::cout << " PV_best_point.score_move = " << PV_best_point.score_move;
			std::cout << std::endl;
			std::cout << "**********************"<<std::endl;
			std::cout << "  " << std::endl;
		}
		else
		{
			//вывод линии варианта
			std::cout << " pv ";

			for (unsigned __int8 n = 0; n < PV_best_point.depth_max; n++)
			{
				std::cout << x_to_notation(((PV_best_point.move[n]>>12)& 63)%8);
				std::cout << (1 + ((PV_best_point.move[n]>>12)& 63)/8);
				std::cout << x_to_notation(((PV_best_point.move[n]>>18)& 63)%8);
				std::cout << (1 + ((PV_best_point.move[n]>>18)& 63)/8);

				d_m = ((PV_best_point.move[n]>>6)& 63);

				//записываем фигуру превращения пешки
				if (d_m != 5)
				{
					if (d_m > 15)
					{
						std::cout <<int_to_char_board((d_m - 20),0);
					}
					else
					{
						std::cout <<int_to_char_board((d_m - 10),0);
					}
				}
				std::cout <<" ";
			}
		}
		std::cout <<std::endl;
	}
	std::cout.flush();// это уже не нужно но все равно оставлю :)
}


/*
 функция встречалось в коде Анд чесс(Andchess) от 9.01.06 
 сохраню как историческую ценность :)) 
 конвертируем число фигуры в букву
 0-нет фигуры, 1-пешка, 2-конь, 3-слон, 4-ладья, 5-ферзь, 6-король	
*/
__int8 Uci_engine_to_gui::int_to_char_board
(
	const int i,// код фигуры
	const int z// цвет фигуры
)
{
	__int8 ch = ' ';
	if (z == 0)
	{
		if (i == 0) ch = 'x'; 
		if (i == 1) ch = 'p'; 
		if (i == 2) ch = 'n'; 
		if (i == 3) ch = 'b'; 
		if (i == 4) ch = 'r'; 
		if (i == 5) ch = 'q'; 
		if (i == 6) ch = 'k'; 
	}
	else
	{
		if (i == 0) ch = 'x'; 
		if (i == 1) ch = 'P'; 
		if (i == 2) ch = 'N'; 
		if (i == 3) ch = 'B'; 
		if (i == 4) ch = 'R'; 
		if (i == 5) ch = 'Q'; 
		if (i == 6) ch = 'K'; 
	}
	return ch;
}

/*
 по координате х доски выдаем букву нотации например 0 - а	
*/
__int8 Uci_engine_to_gui::x_to_notation
(
	const int x//горизонтальная координата доски (0-7)
)
{
	if (x == 0) return 'a';
	if (x == 1) return 'b';
	if (x == 2) return 'c';
	if (x == 3) return 'd';
	if (x == 4) return 'e';
	if (x == 5) return 'f';
	if (x == 6) return 'g';
	if (x == 7) return 'h';
	return '-';
}
