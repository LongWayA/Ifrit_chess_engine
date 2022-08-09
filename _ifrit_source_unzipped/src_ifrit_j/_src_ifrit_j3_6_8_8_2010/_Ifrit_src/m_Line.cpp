#include <iostream> // TEST


#include "m_Line.h"
#include "k_Util.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 27.02.2009
 *
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%

 Работа с линией главного варианта

 В целом логика такая.
 У нас есть две глобальные линии варианта - одна заполняется при погружении на глубину, 
 а другая при подъеме с  глубины. Кроме того в каждом узле есть линия лучшего варианта 
 для узла.
 Мы записываем ходы, которые делаем, погружаясь на глубину в линию текущего варианта. 
 На глубине мы перезаписываем их в линию лучшего варианта. Поднимаясь мы в каждом узле 
 запоминаем лучший для данного узла вариант. Уходя из узла, мы лучший в узле вариант 
 перезаписываем в линию лучшего варианта.

 конретнее:

 У нас есть линия варианта общая для всего поиска pv_current. 
 Ее мы заполняем, когда спускаемся на глубину public_update_PV_current.  

 Есть еще лучшая линия варианта pv_best. 
 Ее мы заполняем перед тем как выйти из узла public_update_PV_best.
 Особый случай ее обновления на максимальной глубине public_update_PV_best_max_depth.

 Так же в каждом узле есть лучшая линия варианта для данного узла p_PV_best_point.
 Ее мы обновляем каждый раз когда в узле нашли лучший ход public_update_PV_best_point.

*/

namespace 
{
	struct T_PV pv_current;// текущий вариант общий для всего поиска(заполняем при погружении на глубину)
	struct T_PV pv_best;// лучший вариант общий для всего поиска(заполняем при всплытии)

	__int32 save_value = -999999999; // запоминаем оценку последнего выведенного варианта
	unsigned __int8 save_Depth_Max = 0; // запоминаем глубину последнего выведенного варианта
}

//	===================================================================
// возвращаем лучший вариант
struct T_PV * PVSAVE::public_get_pv_best()
{
	return & pv_best;
}

//	===================================================================
// возвращаем текущий вариант
struct T_PV * PVSAVE::public_get_pv_current()
{
	return & pv_current;
}

//	===================================================================
// обновляем линию лучшего варианта в узле
void PVSAVE::public_update_PV_best_point
(
	struct T_PV * const p_PV_best_point// лучший вариант найденный в узле
)
{
	// лучший вариант который функция перебора выдает наверх содержится в структуре PV_best
	// мы не только присваиваем оценку но и вариант соответствующий ей
	// присвоение идет структуре уникальной для каждого узла PV_best_point
	// здесь мы перезаписываем лучший вариант соответствующий лучшей оценке
	for (unsigned __int8 n = 0; n < pv_best.depth_max; n++)
	{
		p_PV_best_point->move[n] = pv_best.move[n];
	}

	p_PV_best_point->score_move = pv_best.score_move;
	p_PV_best_point->depth_max = pv_best.depth_max;
}

//	===================================================================
// обновляем линию лучшего варианта
void PVSAVE::public_update_PV_best
(
	const struct T_PV * const p_PV_best_point// лучший вариант найденный в узле
)
{
	// мы пробежали весь список и теперь в структуре PV_best_point содержится лучшая линия
	// теперь мы ее скопируем в структуру PV_best что бы передать на более высокий уровень
	for (unsigned __int8 n = 0; n < p_PV_best_point->depth_max; n++)
	{
		pv_best.move[n] = p_PV_best_point->move[n];
	}

	pv_best.score_move = p_PV_best_point->score_move; 
	pv_best.depth_max = p_PV_best_point->depth_max;
}

//	===================================================================
// обновляем линию лучшего варианта на максимальной глубине
void PVSAVE::public_update_PV_best_max_depth
(
	const __int32 & score,// оценка позиции в конце варианта
	const unsigned __int8 & depth// текущая глубина
)
{
	// мы достигли предельной глубины и статически оценили получившуюся позицию
	// погружаясь на глубину мы на каждом полу ходе заносили ходы варианта в структуру PV_current
	// теперь мы перепишем его в структуру PV_best и прицепим оценку позиции
	// у нас получился вариант приводящий к данной позиции и плюс оценка этой позиции
	// ну и еще конечно длина варианта
	// внимание тут цикл идет от 0 до Depth - 1
	// именно так ведь у нас начальная глубина 0
	for (unsigned __int8 n = 0; n < depth; n++)
	{
		pv_best.move[n] = pv_current.move[n];
	}

	pv_best.score_move = score;

	// запоминаем глубину варианта
	pv_best.depth_max = depth;
}

//	===================================================================
// записываем ход в текущий вариант
void PVSAVE::public_update_PV_current
(
	const unsigned __int8 & i,// номер хода в списке ходов
	const unsigned __int8 & depth,// текущая глубина
	const struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
)
{

	// из списка возможных ходов копируем текущий ход в текущий вариант на текущей глубине.
	// мы видим что текущий вариант прописывается до рекурсивного вызова функции Alpha_beta т.е. 
	// мы разматываем нить погружаясь в глубину 
	// запись в структуре PV_current верна сверху и до текущей глубины 
	// то что расположено ниже данного уровня не правильно так как относится к предыдущему узлу.
	pv_current.move[depth] = p_list_surplus_moves->move[i];
}

//	===================================================================
// выводим текущий ход в оболочку
void PVSAVE::public_print_currmove
(
	const struct T_list_surplus_moves * const p_list_surplus_moves,//список ходов
	const unsigned __int8 nodes_0,// количество ходов на нулевой глубине
	const unsigned __int8 i,// номер хода по списку
	const unsigned __int8 Depth_Max// максимальная глубина
)
{
	// рассчитываемый в момент печати ход
	std::cout << "info currmove " ;
	std::cout <<UTIL::public_x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8);
	std::cout <<(1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);
	std::cout <<UTIL::public_x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8);
	std::cout <<(1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8);
	// порядковый номер рассчитываемого хода
	std::cout << " currmovenumber " ;
	std::cout << (__int32)nodes_0;
	// печатаем информацию о глубине просмотра
	std::cout << " depth " << (__int32)Depth_Max;
	std::cout << "\n";
	std::cout.flush();
}


//	===================================================================
// выводим текущий вариант в оболочку
void PVSAVE::public_print_pv_line
(
	const bool colour_move,// цвет печатающей стороны
	const struct T_list_surplus_moves * const p_list_surplus_moves,// список ходов
	const struct T_PV * const p_PV_best_point,// лучший вариант
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
		std::cout <<"\n";
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
		std::cout << " seldepth "<< (__int32)p_PV_best_point->depth_max;

		//печатаем информацию о оценке позиции(это лучшая на данный момент оценка связанна с лучшим вариантом)
		colour_move == 0 ? inv = -1 : inv = 1;

		if (inv * value > 900000)
		{
			std::cout << " score mate " << (__int32)p_PV_best_point->depth_max;// дистанция до мата в полу ходах
		} 
		else if(inv * value < -900000)
		{
			std::cout << " score mate " << -(__int32)p_PV_best_point->depth_max;// дистанция до мата в полу ходах
		}
		else
		{
			std::cout << " score cp " << inv * value;// обычная оценка (in centipawns)т.е. одна пешка стоит 100 пунктов
		}

		//печатаем информацию о количестве просмотренных узлов
		std::cout << " nodes "<<(__int32)print_nodes;

		//сравниваю по модулю потому что из за продлений на шахах запарки со знаком
		if ( (value * value) != (p_PV_best_point->score_move * p_PV_best_point->score_move) ) 
		//if(0)
		{
			std::cout << "\n";
			std::cout << "ERROR LINE " ;
			std::cout << "\n";
			std::cout << " value = " << value;
			std::cout << "\n";
			std::cout << " p_PV_best_point.score_move = " << p_PV_best_point->score_move;
			std::cout << "\n";
			std::cout << "**********************"<<"\n";
			std::cout << "  " << "\n";
		}
		else
		{
			//вывод линии варианта
			std::cout << " pv ";

			for (unsigned __int8 n = 0; n < p_PV_best_point->depth_max; n++)
			{
				std::cout << UTIL::public_x_to_notation(((p_PV_best_point->move[n]>>12)& 63)%8);
				std::cout << (1 + ((p_PV_best_point->move[n]>>12)& 63)/8);
				std::cout << UTIL::public_x_to_notation(((p_PV_best_point->move[n]>>18)& 63)%8);
				std::cout << (1 + ((p_PV_best_point->move[n]>>18)& 63)/8);

				d_m = ((p_PV_best_point->move[n]>>6)& 63);

				//записываем фигуру превращения пешки
				if (d_m != 5)
				{
					if (d_m > 15)
					{
						std::cout <<UTIL::public_int_to_char_board((d_m - 20),0);
					}
					else
					{
						std::cout <<UTIL::public_int_to_char_board((d_m - 10),0);
					}
				}
				std::cout <<" ";
			}
		}
		std::cout <<"\n";
	}
	std::cout.flush();// это уже не нужно но все равно оставлю :)
}






