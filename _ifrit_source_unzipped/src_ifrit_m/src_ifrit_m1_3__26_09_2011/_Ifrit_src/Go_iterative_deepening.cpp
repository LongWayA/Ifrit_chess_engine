
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.02.2008  11.11.2009 11.07.10

//!
//--------------------------------------
// отключил предупреждения которые выдаются из за использования условия 
// для препроцессора #if #endif
// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 

#include <fstream>
#include <iostream>

#include "Search_switch.h"
#include "List.h"
#include "Go.h"
#include "Search_root.h"
#include "Search.h"
#include "Evaluate.h"
#include "Generator_moves.h"
#include "Killer.h"
#include "History.h"
#include "ZObrist.h"
#include "PV_save.h"
#include "Timer.h"


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
 ofstream Test_Deepening;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


	__int64 nodes_y;             /// количество узлов на данной глубине
	__int64 nodes_previous;      /// запоминаем полное количество узлов
	double koff_deepening;       /// высчитываем насколько дольше по времени 
							     /// считается следующая глубина

//	===================================================================
//
void Go::deepening
(
	struct Position & position,// представление доски
	unsigned __int8 & depth_max,
	double & max_time_move_search
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

	__int32 value_root = 0;// оценка лучшего варианта и делаемого хода. 
	bool flag_mate = 0;// найден мат 
	__int64 nodes = 0;// количество рассмотренных узлов(позиций)

	__int32 test_eval = 0;// запоминаем оценку материала для контроля
	unsigned __int64 key_control = 0;   // запоминаем хеш-ключ для контроля

	// время смотрим в секундах
	double time_current = -1.0;  // сколько времени прошло при данной итерации
	double remember_time_current = 1.0;  // запоминаем


	// на глубине обрубаем только при существенном превышении времени на расчет
	// сейчас это в ~ 5 раз
	Search::ini_deep_time_terminate( (max_time_move_search * 15) );

	// считаем материал. в дальнейшем только корректируем оценку при взятиях.
	position.value_white_material = Evaluate::eval_white_material(position);
	position.value_black_material = Evaluate::eval_black_material(position);

	//TEST
	// для проверки что после возвращения оценка вернулась к первоначальному положению
    test_eval =  position.value_white_material 
		- position.value_black_material;

	// инициализируем ключ начальной позиции(мы пишем его в структуру доски)
	ZObrist::start_position_random_key(position);

	//инициализируем хеш - массив повторов возрастающими числами 
	// что-бы исключить ложное срабатывание
	ZObrist::ini_hash_three(); 

	// записываем ключ для последующего анализа на повтор позиций
	ZObrist::save_hash_three(position.hash_key,0);

	// TEST
	//std::cout << " N bitboard->eval = " << bitboard->eval << "\n";

	//TEST
	// записываем ключ для контроля при выныривании с глубины
	key_control = position.hash_key;

	// TEST
	// здесь очищаем хеш-таблицу
	// инициализируем все ключи в хеш-таблице 0
	//   HASHM::Ini_hash_moves();

	// использую для тестирования подтверждением варианта
	// TEST
	//depth_max = depth_max - 1;

	// TEST
	//std::cout << "1111111 " << "\n";

	// инициализируем глобальные переменные
	Search::ini_global_loop();

#if SW_HISTORY
	// обнуляем историю
	History::ini_history();
#endif // #if SW_HISTORY

	// инициализируем массив киллеров
	Killer::ini_killer(); 

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
		(Search::get_stop_search() == 0)// нет команды на остановку счета 
		);
	depth++
	)
	{
		// запоминаем полное количество узлов с предыдущей итерации т.е глубины
		nodes_previous = nodes;

		//сбрасываем счетчик позиций
		Search_root::set_nodes_in_0();

		// запускаем таймер
		Timer::time_start();

		if (position.colour_move == 1)
		{
			value_root = Search_root::white_root(position,Alpha,Beta,depth);
		}
		else
		{
			value_root = Search_root::black_root(position,Alpha,Beta,depth);
		}

		// останавливаем таймер
		Timer::time_stop();

		// считаем промежуток времени
		time_current = Timer::time_delta();

        koff_deepening = time_current/remember_time_current;
		remember_time_current = time_current;

		nodes = Search_root::get_nodes();

		// количество узлов на слое считаем как полное количество узлов
		// минус полное количество узлов на предыдущей итерации
		nodes_y = nodes - nodes_previous ;

		//
		test_loop(position,test_eval,key_control);

		// печатаем информацию в поток
		print_deepening_line(position,nodes,time_current,depth,value_root,flag_mate);

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
		if ((max_time_move_search != -1) 
			&& (max_time_move_search < time_current) 
			&& (depth >= 4))
		{
			break;
		}

	}

	// делаем ход вида < bestmove e2e4 
	do_bestmove();

	// проверяем хеш-ключ на корректность и 
	// контроль соответствия первого хода первому ходу линии варианта
	test_end_deepening(position,key_control);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_D
	Test_Deepening.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}

//	===================================================================
// делаем ход вида < bestmove e2e4 
// или при превращении в коня(к примеру) < bestmove d7d8N 
void Go::do_bestmove()
{
	std::cout << "bestmove ";
	std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[0]>>12)& 63)%8);
	std::cout << (1 + ((Search_root::get_pv_best().move[0]>>12)& 63)/8);
	std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[0]>>18)& 63)%8);
	std::cout << (1 + ((Search_root::get_pv_best().move[0]>>18)& 63)/8);

	// считываем тип хода
	__int32 d_m = ((Search_root::get_pv_best().move[0]>>6)& 63);

	// если d_m больше 5 то значит имеем превращение пешки
	if (d_m > 5)
	{
		if (d_m > 15)
		{
			// превращение пешки с взятием
			std::cout << PV_save::int_to_char_board((d_m - 20),0);
		}
		else
		{
			// превращение пешки
			std::cout << PV_save::int_to_char_board((d_m - 10),0);
		}
	}

	std::cout <<"\n";
}

//	===================================================================
// печатаем информацию в поток
void Go::print_deepening_line
(
	struct Position & position,// представление доски
	const __int64 nodes,// количество рассмотренных узлов(позиций)
	const double time_current,  // сколько времени прошло при данной итерации
	const unsigned __int8 depth,
	const __int32 value_root,// оценка лучшего варианта и делаемого хода.
	bool & flag_mate// найден мат 
 )
{

	__int8 inv;// флаг цвета хода

		std::cout << "info " ;
		std::cout << " nps " << (__int32)(nodes / time_current);// скорость из расчета по всем узлам на данной глубине
		std::cout << " depth " << (__int32)depth;//глубина

		// печатаем информацию о глубине форсированного варианта
		std::cout << " seldepth "<< (__int32)Search_root::get_pv_best().depth_max;

		position.colour_move == 0 ? inv = -1 : inv = 1;

		// печатаем оценку. для мата специальное представление в виде дистанции
		if (inv * value_root > 900000)
		{ 
			flag_mate = 1;
			std::cout << " score mate "<< (__int32)Search_root::get_pv_best().depth_max;// дистанция до мата в полуходах
		}
		else if (inv * value_root < -900000)
		{
			std::cout << " score mate "<< -(__int32)Search_root::get_pv_best().depth_max;// дистанция до мата в полуходах
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

		for (unsigned __int8 n = 0; n < Search_root::get_pv_best().depth_max; n++)
		{
			std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[n]>>12)& 63)%8);
			std::cout << (1 + ((Search_root::get_pv_best().move[n]>>12)& 63)/8);
			std::cout << PV_save::x_to_notation(((Search_root::get_pv_best().move[n]>>18)& 63)%8);
			std::cout << (1 + ((Search_root::get_pv_best().move[n]>>18)& 63)/8);

			// считываем тип хода
			__int32 d_m = ((Search_root::get_pv_best().move[n]>>6)& 63);

			// если d_m больше 5 то значит имеем превращение пешки
			if (d_m > 5)
			{
				if (d_m > 15)
				{
					std::cout << PV_save::int_to_char_board((d_m - 20),0);// превращение пешки с взятием
				}
				else
				{
					std::cout << PV_save::int_to_char_board((d_m - 10),0);// просто превращение пешки
				}
			}

			std::cout << " ";
		}

		std::cout <<"\n";
}

//	===================================================================
// 
void Go::test_end_deepening
(
		struct Position & position,// представление доски
		const unsigned __int64 key_control
)
{
	// TEST
	//std::cout << "cnt_move = " << cnt_move << "\n";
	//std::cout << "wtime = " << wtime << "\n";
	//std::cout << "time_terminate = " << time_terminate << "\n";
	//std::cout <<"time_current = "<< time_current <<"\n";
	//std::cout <<"   " <<"\n";

	//TEST
	// проверяем хеш-ключ на корректность т.е. что после возвращения с глубины он вернулся к 
	// состоянию которое он имел до погружения на глубину
	if (key_control != position.hash_key)
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}

	//TEST
	// контроль соответствия первого хода первому ходу линии варианта
	if (Search_root::get_from() 
	!= ((Search_root::get_pv_best().move[0]>>12)& 63))
	{
		std::cout << "ERROR from(in Iterative_Deepening.cpp)" <<"\n";
	}

	if (Search_root::get_to() 
	!= ((Search_root::get_pv_best().move[0]>>18)& 63))
	{
		std::cout << "ERROR to(in Iterative_Deepening.cpp)" <<"\n";
	}
}


//	===================================================================
// 
void Go::test_loop
(
	struct Position & position,// представление доски
 	const __int32 test_eval,// запоминаем оценку материала для контроля
	const unsigned __int64 key_control// запоминаем хеш-ключ для контроля
)
{
	//TEST
	// проверка оценки материала
	if (test_eval != (position.value_white_material - position.value_black_material))
	{
		std::cout << "ERROR test_eval != bitboard->eval(in Iterative_Deepening.cpp)" << "\n";
	}
 
	//TEST
	// проверяем хеш-ключ на корректность т.е. что после возвращения 
	// с глубины он вернулся к 
	// состоянию которое он имел до погружения на глубину
	if (key_control != position.hash_key)
	{
		std::cout <<"\n";
		std::cout << "ERROR HESH Test_Deepening(in Iterative_Deepening.cpp) " ;
		std::cout <<"\n";
	}
}


