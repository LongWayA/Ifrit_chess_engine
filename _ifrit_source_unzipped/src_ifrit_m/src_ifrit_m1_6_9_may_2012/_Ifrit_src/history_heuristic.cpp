
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  8.09.2011

#include <fstream>

#include "history_heuristic.h"


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_HISTORY
 std::ofstream Test_History;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

// 
const __int32 History_Maximum = 100000;

//
__int32 white_history[MAX_NAME][MAX_COORDINATE];  // [фигура][поле куда фигура ходит]
__int32 black_history[MAX_NAME][MAX_COORDINATE];


//	===================================================================
__int32 History_heuristic::get_white_history
(
	const unsigned __int8 n,///
	const unsigned __int8 c ///
)
{
	return white_history[n][c];
};

//	===================================================================
__int32 History_heuristic::get_black_history
(
	const unsigned __int8 n,///
	const unsigned __int8 c ///
)
{
	return black_history[n][c];
};


//	===================================================================
// инициализируем историю
void History_heuristic::ini_history()
{
	for (unsigned __int8  i = 0; i < MAX_NAME; i++)
	{
		for (unsigned __int8  j = 0; j < MAX_COORDINATE; j++)
		{
			white_history[i][j] = 0;
			black_history[i][j] = 0;
		}
	}
}

//	===================================================================
// пишем историю
void History_heuristic::white_history_good_save
(
	const unsigned __int8 i,/// номер хода в списке
	struct List & list,/// представление списка ходов
	const unsigned __int8 depth,// глубина записи хода
	const unsigned __int8 depth_max// максимальная глубина расчета
)
{

	const unsigned __int8 name = (list.move[i]>>24) & 7;// имя фигуры
	const unsigned __int8 to = (list.move[i]>>18) & 63;// финальная позиция фигуры
	const unsigned __int8 delta_depth = depth_max - depth;// максимальная глубина расчета


	//white_history_bad_save(i,list,depth,depth_max);

	//
	white_history[name][to] = white_history[name][to] + delta_depth * delta_depth;

	/// если достигнут предел то уменьшаем в два раза
	if( white_history[name][to] >= History_Maximum)
	{
		for (unsigned __int8  k = 0; k < MAX_NAME; k++)
		{
			for (unsigned __int8  j = 0; j < MAX_COORDINATE; j++)
			{
				white_history[k][j] = white_history[k][j]/2;
			}
		}
	}

}

//	===================================================================
// пишем историю
void History_heuristic::black_history_good_save
(
	const unsigned __int8 i,/// номер хода в списке
	struct List & list,/// представление списка ходов
	const unsigned __int8 depth,// глубина записи хода
	const unsigned __int8 depth_max// максимальная глубина расчета
)
{

	const unsigned __int8 name = (list.move[i]>>24) & 7;// имя фигуры
	const unsigned __int8 to = (list.move[i]>>18) & 63;// финальная позиция фигуры
	const unsigned __int8 delta_depth = depth_max - depth;// максимальная глубина расчета


	//black_history_bad_save(i,list,depth,depth_max);

	//
	black_history[name][to] = black_history[name][to] + delta_depth * delta_depth;

	/// если достигнут предел то уменьшаем в два раза
	if( black_history[name][to] >= History_Maximum)
	{
		for (unsigned __int8  k = 0; k < MAX_NAME; k++)
		{
			for (unsigned __int8  j = 0; j < MAX_COORDINATE; j++)
			{
				black_history[k][j] = black_history[k][j]/2;
			}
		}
	}
}

//	===================================================================
// пишем историю для белых плохих ходов
inline void History_heuristic::white_history_bad_save
(
	const unsigned __int8 e,/// номер хода в списке
	struct List & list,/// представление списка ходов
	const unsigned __int8 depth,// глубина записи хода
	const unsigned __int8 depth_max// максимальная глубина расчета
)
{
	unsigned __int8 name ;// имя фигуры
	unsigned __int8 to ;// финальная позиция фигуры
	const unsigned __int8 delta_depth = depth_max - depth;// максимальная глубина расчета

//Test_History.open ("_TEST_HISTORY.txt",std::ios::app);
//Test_History << "depth= "<< int(depth) << "\n";
//Test_History << "delta_depth= "<< int(delta_depth) << "\n";

	/// все тихие ходы в списке объявляем плохими
	for (unsigned __int8 c = list.end_captures; c < e; c++)
	{

		name = (list.move[c]>>24) & 7;
		to = (list.move[c]>>18) & 63;

		white_history[name][to] = white_history[name][to] - delta_depth * delta_depth;
		//white_history[name][to] = name; //white_history[name][to] - 1;

//Test_History << "white_history[" << int(name) << "][" << int(to) << "]= " << white_history[name][to] << "\n";

		/// если достигнут предел то уменьшаем в два раза
		if( white_history[name][to] <= -History_Maximum)
		{
			for (unsigned __int8  i = 0; i < MAX_NAME; i++)
			{
				for (unsigned __int8  j = 0; j < MAX_COORDINATE; j++)
				{
					white_history[i][j] = white_history[i][j]/2;
				}
			}
		}
	}

//Test_History << "    "<< "\n";
//Test_History << "--------------------------------"<< "\n";
//Test_History << "    "<< "\n";
//Test_History.close();

}


//	===================================================================
// пишем историю для черных плохих ходов
inline void History_heuristic::black_history_bad_save
(
	const unsigned __int8 e,/// номер хода в списке
	struct List & list,/// представление списка ходов
	const unsigned __int8 depth,// глубина записи хода
	const unsigned __int8 depth_max// максимальная глубина расчета
)
{
	unsigned __int8 name ;// имя фигуры
	unsigned __int8 to ;// финальная позиция фигуры
	const unsigned __int8 delta_depth = depth_max - depth;// максимальная глубина расчета

	/// все тихие ходы в списке объявляем плохими
	for (unsigned __int8 c = list.end_captures; c < e; c++)
	{

		name = (list.move[c]>>24) & 7;
		to = (list.move[c]>>18) & 63;

		black_history[name][to] = black_history[name][to] - delta_depth * delta_depth;
		//black_history[name][to] =  name; //  black_history[name][to] - 1;

		/// если достигнут предел то уменьшаем в два раза
		if( black_history[name][to] <= -History_Maximum)
		{
			for (unsigned __int8  i = 0; i < MAX_NAME; i++)
			{
				for (unsigned __int8  j = 0; j < MAX_COORDINATE; j++)
				{
					black_history[i][j] = black_history[i][j]/2;
				}
			}
		}
	}
}


//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем
// максимальная глубина, глубина перебора, доска,
#if TEST_HISTORY

void History_heuristic::test_print
(	
	const unsigned __int8 depth_max// максимальная глубина расчета
)
{
	Test_History.open ("_TEST_HISTORY.txt",std::ios::app);
	//Test_Loop.open ("_TEST_LOOP.txt");

	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG

	Test_History << "печатаем массивы"<< "\n";
	Test_History <<"depth_max= "<< int(depth_max) << "\n";

	for (unsigned __int8  i = 0; i < MAX_NAME; i++)
	{
		for (unsigned __int8  j = 0; j < MAX_COORDINATE; j++)
		{
			Test_History << "white_history[" << int(i) << "][" << int(j) << "]= " << white_history[i][j] << "\n";
			Test_History << "black_history[" << int(i) << "][" << int(j) << "]= " << black_history[i][j] << "\n";
		}
	}
	Test_History << "------------------------------------------------- " << "\n";

	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG

	Test_History.close();
}
#endif
