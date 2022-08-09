#include <iostream>
#include <fstream>  // TEST

#include "c_Iterative_Deepening.h"
#include "k_Util.h"
#include "k_Timer.h"
#include "h_Evaluate.h"
#include "j_Hash.h"
#include "d_Bitboard_gen.h"

#include "j_ZObrist.h"
#include "g_Search_root.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.02.2008  11.11.2009 11.07.10
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%

перебор с циклическим увеличением глубины
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


namespace 
{
	__int64 nodes_y = 0; // количество узлов на данной глубине
	__int64 nodes_previous = 0; // запоминаем полное количество узлов
	double koff_deepening = 1.0;  // высчитываем насколько дольше по времени 
							      // считается следующая глубина
}

//	===================================================================
//
void GO::public_deepening
(
	struct T_Bitboard * p_bitboard,//представление доски
	const __int8 depth_max_go_command,//максимальная глубина перебора
	const double wtime_go_command,//оставшееся время на игру за белых
	const double btime_go_command,//оставшееся время на игру за черных
	const double winc_go_command,//добавление времени за ход для белых
	const double binc_go_command,//добавление времени за ход для черных
	const __int8 movestogo_go_command,//количество ходов на которое выделено время
	const double movetime_go_command,//время на ход
	const bool infinite_go_command//бесконечный анализ
)
{

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.open ("_TEST_DEEPENING.txt",ios::app);
	//Test_Deepening.open ("_TEST_DEEPENING.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT



	const __int32 Alpha = -9999999;
	const __int32 Beta  =  9999999;

	__int8 inv = 0;// флаг цвета хода
	__int32 value_root = 0;// оценка лучшего варианта и делаемого хода. 
	__int32 d_m = 0; // тип хода. нужен для определения хода пешкой с превращением
	__int32 test_eval = 0;// запоминаем оценку материала для контроля
	bool flag_mate = 0;// найден мат 
	unsigned __int64 key_control = 0;   // запоминаем хеш-ключ для контроля
	struct T_list_surplus_moves root_list_surplus_moves;// список возможных ходов (всевозможные ходы из данного узла)

	__int64 nodes = 0;// количество рассмотренных узлов(позиций)

	// время смотрим в секундах
	double max_time_move_search = -1.0;// время выделенное на ход
	double time_current = -1.0;  // сколько времени прошло при данной итерации
	unsigned __int16 cnt_move ;// количество уже сделанных в партии полуходов
	__int16 limit_cnt_move ;// сколько ходов осталось сделать
	unsigned __int8 depth_max;//максимальная глубина перебора


	double remember_time_current = 1.0;  // запоминаем

	/*
		это распределение времени тоже не очень удачно и кроме того все как то запутанно
		нужно все четко описать исходя из данных переменых:

	*/
 
		// TEST ---------
		//std::cout << "depth_max_go_command = " << (int)depth_max_go_command << "\n";
		//std::cout << "infinite_go_command = " << infinite_go_command << "\n";
		//std::cout << "movetime_go_command = " << movetime_go_command << "\n";
		//std::cout << "movestogo_go_command = " << (int)movestogo_go_command << "\n";

		//std::cout << "winc_go_command = " << winc_go_command << "\n";
		//std::cout << "binc_go_command = " << binc_go_command << "\n";
		//std::cout << "wtime_go_command = " << wtime_go_command << "\n";
		//std::cout << "btime_go_command = " << btime_go_command << "\n";
		// ---------------

		// TEST
		//std::cout << "koff_deepening = " << koff_deepening << "\n";

	if (depth_max_go_command != -1) // > go depth 5 
	{
		// TEST
		//std::cout << "depth_max_go_command " << "\n";

		// поиск на заданную глубину
		depth_max = depth_max_go_command;
	}
	else if (infinite_go_command != false) // > go infinite
	{
		// TEST
		//std::cout << "infinite_go_command " << "\n";

		// предел при бесконечном анализе. 50 полу ходов думаю достаточно
		depth_max = 50;
	}
	else if (movetime_go_command != -1)	// > go movetime 300000 
	{
		// TEST
		//std::cout << "movetime_go_command " << "\n";

		//задаем время на ход
		max_time_move_search = movetime_go_command;
		depth_max = 50;
	}
	else if (movestogo_go_command != -1) // > go movestogo 40 wtime 300000 btime 300000 
	{
		// TEST
		//std::cout << "movestogo_go_command " << "\n";

		// время на определенное количество ходов
		// оболочка(например Арена) сама считает сколько ходов 
		// осталось, т.е. уменьшает movestogo

		// количество полуходов сделанных в партии
		cnt_move = ZOBRIST::public_get_save_cnt_move();

		// это оставшееся количество ходов
		limit_cnt_move = movestogo_go_command;

		// TEST
		//std::cout << "cnt_move = " << cnt_move << "\n";
		//std::cout << "limit_cnt_move = " << limit_cnt_move << "\n";

		// оставляем некоторый запас
		if (limit_cnt_move < 4)
		{
			limit_cnt_move = 4;
		}

		// время оставшееся на партию делим на количество ходов 
		// нужных для достижения контроля. потом будет прибавка времени
		if (p_bitboard->colour_move == 1)// 1-ход белых 0- ход черных
		{

			max_time_move_search = winc_go_command +  wtime_go_command/limit_cnt_move;
		}
		else
		{
			max_time_move_search = binc_go_command +  btime_go_command/limit_cnt_move;
		}

 		depth_max = 50;
	}
	else if ( (wtime_go_command != -1.0) || (btime_go_command != -1.0)) // > go btime 300000 wtime 300000 
	{
		// TEST
		//std::cout << "wtime_go_command or btime_go_command " << "\n";
		//оставшееся время на игру

		// количество полуходов сделанных в партии
		cnt_move = ZOBRIST::public_get_save_cnt_move();

		// мы предполагаем что все станет ясно за определенное количество ходов (80)
		// это оставшееся количество ходов
		limit_cnt_move = 80 - cnt_move/2 ;

		// время на первые 60 ходов примерно одинаковое
		// после 60 го хода идет уменьшение времени на ход что бы 
		// оттянуть просрочку по времени
		if (limit_cnt_move < 20)
		{
			limit_cnt_move = 20;
		}

		//limit_cnt_move = 20;

		if (p_bitboard->colour_move == 1)// 1-ход белых 0- ход черных
		{

			max_time_move_search = winc_go_command + wtime_go_command/limit_cnt_move;
		}
		else
		{
			max_time_move_search = binc_go_command + btime_go_command/limit_cnt_move;
		}

 		depth_max = 50;
	}


		// TEST
		//std::cout << "max_time_move_search = " << max_time_move_search << "\n";

		if (max_time_move_search != -1)
		{
			// мы сравниваем с временем за который уже посчитали на данной глубине.
			// предполагаем что на следующем уровне время счета будет в несколько раз
			// больше. поэтому мы делим время на эту цифру что бы не было просрочки
			max_time_move_search = max_time_move_search/3;
		}

		// TEST
		//std::cout << "koff max_time_move_search = " << max_time_move_search << "\n";

	// на глубине обрубаем только при существенном превышении времени на расчет
	// сейчас это в ~17 раз
	ROOTSEARCH::public_ini_deep_time_terminate( (max_time_move_search * 50) );

	//-------------------------------------------------------------------------------


	// считаем материал. в дальнейшем только корректируем оценку при взятиях.
	p_bitboard->value_white_material = EVALUATE::public_eval_white_material(p_bitboard);
	p_bitboard->value_black_material = EVALUATE::public_eval_black_material(p_bitboard);

	// для проверки что после возвращения оценка вернулась к первоначальному положению
    test_eval =  p_bitboard->value_white_material - p_bitboard->value_black_material;

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << "\n";

	// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
	ZOBRIST::public_start_position_random_key(p_bitboard);

	// записываем ключ для контроля при выныривании с глубины
	key_control = p_bitboard->hash_key;

	//инициализируем хеш - массив повторов возрастающими числами 
	// что-бы исключить ложное срабатывание
	ZOBRIST::public_ini_hash_three(); 

	// записываем ключ для последующего анализа на повтор позиций
	ZOBRIST::public_save_hash_three(p_bitboard->hash_key,0);

	// TEST
	// здесь очищаем хеш-таблицу
	// инициализируем все ключи в хеш-таблице 0
	//   HASHM::Ini_hash_moves();

	// использую для тестирования подтверждением варианта
	// TEST
	//depth_max = depth_max - 1;

	// TEST
	//std::cout << "1111111 " << "\n";

	// инициализируем глобальные переменные модуля loop
	ROOTSEARCH::public_ini_global_loop(depth_max);

	if (p_bitboard->colour_move == 1)
	{  
		// белые 
		//Заполняем список избыточных ходов. Именно эти списки и составляют дерево перебора.
		BITBOARDGENERATOR::public_generator_captures_white(p_bitboard,&root_list_surplus_moves);// генерируем взятия и превращения
		BITBOARDGENERATOR::public_generator_move_white(p_bitboard,&root_list_surplus_moves);// генерируем остальные ходы
	}
	else
	{
		// черные
		//Заполняем список избыточных ходов. Именно эти списки и составляют дерево перебора.
		BITBOARDGENERATOR::public_generator_captures_black(p_bitboard,&root_list_surplus_moves);// генерируем взятия и превращения
		BITBOARDGENERATOR::public_generator_move_black(p_bitboard,&root_list_surplus_moves);// генерируем остальные ходы
	}

	// TEST
	//std::cout << "22222222222 " << "\n";
	//std::cout << " depth_max " << depth_max << "\n";//глубина

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
	// TEST
	// обнуляем счетчики
	HASHM::Test_hash_ini();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

	// если глубина меньше максимальной и не нашли мат и меньше 
	// 50 и не было команды на остановку то продолжаем счет
	for (unsigned __int8 depth = 1;

	// нет команды на конец счета
	( (depth <= depth_max) && // глубина меньше максимальной заданной на весь перебор
	(flag_mate == 0) && // нету мата
	(depth < 51) && // глубина меньше максимальной 
	(ROOTSEARCH::public_get_stop_search() == 0)// нет команды на остановку счета 
	);

	depth++
	)
	{

		// запоминаем полное количество узлов с предыдущей итерации т.е глубины
		nodes_previous = nodes;

		//сбрасываем счетчик позиций
		ROOTSEARCH::public_set_nodes_in_0();

		// запускаем таймер
		TIMER::public_time_start();

		if (p_bitboard->colour_move == 1)
		{
			value_root = ROOTSEARCH::public_white_root(p_bitboard,
				root_list_surplus_moves,Alpha,Beta,depth);
		}
		else
		{
			value_root = ROOTSEARCH::public_black_root(p_bitboard,
				root_list_surplus_moves,Alpha,Beta,depth);
		}

		// останавливаем таймер
		TIMER::public_time_stop();
		time_current = TIMER::public_time_delta();

        koff_deepening = time_current/remember_time_current;
		remember_time_current = time_current;

		nodes = ROOTSEARCH::public_get_nodes();

		// количество узлов на слое считаем как полное количество узлов
		// минус полное количество узлов на предыдущей итерации
		nodes_y = nodes - nodes_previous ;

		// проверка оценки материала
		if (test_eval != (p_bitboard->value_white_material - p_bitboard->value_black_material))
		{
			std::cout << "ERROR test_eval != bitboard->eval(in Iterative_Deepening.cpp)" << "\n";
		}
 
		//-----------------------------------------------
		// проверяем хеш-ключ на корректность т.е. что после возвращения 
		// с глубины он вернулся к 
		// состоянию которое он имел до погружения на глубину
		if (key_control != p_bitboard->hash_key)
		{
			std::cout <<"\n";
			std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
			std::cout <<"\n";
		}
		//-----------------------------------------------

		//---------------------------------------------------------------
		// печатаем информацию в поток
		std::cout << "info " ;
		std::cout << " nps " << (__int32)(nodes / time_current);// скорость из расчета по всем узлам на данной глубине
		std::cout << " depth " << (__int32)depth;//глубина

		// печатаем информацию о глубине форсированного варианта
		std::cout << " seldepth "<< (__int32)ROOTSEARCH::public_get_pv_best()->depth_max;

		p_bitboard->colour_move == 0 ? inv = -1 : inv = 1;

		// печатаем оценку. для мата специальное представление в виде дистанции
		if (inv * value_root > 900000)
		{ 
			flag_mate = 1;
			std::cout << " score mate "<< (__int32)ROOTSEARCH::public_get_pv_best()->depth_max;// дистанция до мата в полуходах
		}
		else if (inv * value_root < -900000)
		{
			std::cout << " score mate "<< -(__int32)ROOTSEARCH::public_get_pv_best()->depth_max;// дистанция до мата в полуходах
			flag_mate = 1;
		}
		else
		{
			std::cout << " score cp "<< inv * value_root;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
			flag_mate = 0;
		}
 
		// std::cout << " score cp "<< inv*value_root;// для теста матовой оценки

		// печатаем линию варианта
		//std::cout << " nodes " << (__int32)nodes; // полное количество узлов
		std::cout << " nodes "<< (__int32)nodes_y; // количество узлов на слое
		std::cout <<" pv ";

		for (unsigned __int8 n = 0; n < ROOTSEARCH::public_get_pv_best()->depth_max; n++)
		{
			std::cout << UTIL::public_x_to_notation(((ROOTSEARCH::public_get_pv_best()->move[n]>>12)& 63)%8);
			std::cout << (1 + ((ROOTSEARCH::public_get_pv_best()->move[n]>>12)& 63)/8);
			std::cout << UTIL::public_x_to_notation(((ROOTSEARCH::public_get_pv_best()->move[n]>>18)& 63)%8);
			std::cout << (1 + ((ROOTSEARCH::public_get_pv_best()->move[n]>>18)& 63)/8);

			// считываем тип хода
			d_m = ((ROOTSEARCH::public_get_pv_best()->move[n]>>6)& 63);

			// если d_m больше 5 то значит имеем превращение пешки
			if (d_m > 5)
			{
				if (d_m > 15)
				{
					std::cout << UTIL::public_int_to_char_board((d_m - 20),0);// превращение пешки с взятием
				}
				else
				{
					std::cout << UTIL::public_int_to_char_board((d_m - 10),0);// просто превращение пешки
				}
			}

			std::cout << " ";
		}

		std::cout <<"\n";

//---------------------------------------------------------------
#if TEST_WRITE_READ_WN
		// TEST

		// сколько элементов в таблице и сколько использованно т.е. во сколько мы записали
		HASHM::TEST_Hash_index_print();

		// печатаем инфу об эффективности кеширования
		HASHM::Test_hash_print();
#endif // #if HASHM::TEST_WRITE_READ_WN
//---------------------------------------------------------------

		// выходим по времени
		if ((max_time_move_search != -1) && (max_time_move_search < time_current) && (depth >= 4))
		{
			break;
		}

	}

	// TEST
	//std::cout << "cnt_move = " << cnt_move << "\n";
	//std::cout << "wtime = " << wtime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout <<"time_current = "<< time_current <<"\n";
	//std::cout <<"   " <<"\n";

	//-----------------------------------------------
	// проверяем хеш-ключ на корректность т.е. что после возвращения с глубины он вернулся к 
	// состоянию которое он имел до погружения на глубину
	if (key_control != p_bitboard->hash_key)
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}
	//-----------------------------------------------


	//---------------------------------------------------------------------
	// делаем ход вида < bestmove e2e4 
	// или при превращении в коня(к примеру)  < bestmove d7d8N 
	std::cout << "bestmove ";
	std::cout << UTIL::public_x_to_notation(((ROOTSEARCH::public_get_pv_best()->move[0]>>12)& 63)%8);
	std::cout << (1 + ((ROOTSEARCH::public_get_pv_best()->move[0]>>12)& 63)/8);
	std::cout << UTIL::public_x_to_notation(((ROOTSEARCH::public_get_pv_best()->move[0]>>18)& 63)%8);
	std::cout << (1 + ((ROOTSEARCH::public_get_pv_best()->move[0]>>18)& 63)/8);

	// считываем тип хода
	d_m = ((ROOTSEARCH::public_get_pv_best()->move[0]>>6)& 63);

	// если d_m больше 5 то значит имеем превращение пешки
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			// превращение пешки с взятием
			std::cout << UTIL::public_int_to_char_board((d_m - 20),0);
		}
		else
		{
			// превращение пешки
			std::cout << UTIL::public_int_to_char_board((d_m - 10),0);
		}
	}

	std::cout <<"\n";

	// контроль соответствия первого хода первому ходу линии варианта
	if (ROOTSEARCH::public_get_from() 
	!= ((ROOTSEARCH::public_get_pv_best()->move[0]>>12)& 63))
	{
		std::cout << "ERROR from(in Iterative_Deepening.cpp)" <<"\n";
	}

	if (ROOTSEARCH::public_get_to() 
	!= ((ROOTSEARCH::public_get_pv_best()->move[0]>>18)& 63))
	{
		std::cout << "ERROR to(in Iterative_Deepening.cpp)" <<"\n";
	}
//---------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}