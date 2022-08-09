
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%

#include "a_position_struct.h"
#include "a_pv_line_struct.h"

/*

Search
http://chessprogramming.wikispaces.com/Search

 Because having a perfect evaluation is impossible, chess programs 
 must rely on some type of Search in order to play reasonably.

 Searching involves looking ahead at different move sequences and 
 evaluating the positions after making the moves. Formally, searching a 
 two-player zero-sum board game with perfect information implies traversing 
 and min-maxing a tree-like data-structure by various search algorithms.

Search Algorithms
 The majority of chess-programs use some variation of the alpha-beta 
 algorithm to search the tree in depth-first manner, with big up to 
 square-root savings for the same search result as the pure minimax algorithm. 
 Alpha-beta may be further enhanced by PVS similar to NegaScout and MTD(f). 
 While move ordering in pure minimax search don't cares - since all of branches 
 and nodes are searched anyway, it becomes crucial for the performance of alpha beta 
 and enhancements. Hans Berliner's chess-program HiTech and Ulf Lorenz's 
 program P.ConNerS used best-first approaches quite successfully.

Alpha-Beta Enhancements

 Obligatory
  Transposition Table
  Iterative Deepening

 Selectivity
  Quiescence Search
  Selectivity

 Scout and Friends
  Principal Variation Search

end link(конец цитаты)---------------------
*/

/*
 перебор позиций
 включает в себя данные и методы основного перебора

 так работает генератор позиций:
 1 в начале генерируются список избыточных ходов. избыточных потому что нет проверки на шах
   и в них есть такие ходы как ход короля под шах
 2 дальше мы сортируем этот список выводя взятия на верх и ход из хеш-таблицы если он есть и киллер
 3 дальше мы реализуем ход и проверяем позицию на шах если 
   под боем наш король то ход откатываем иначе идем дальше
 4 итерационное углубление
 5 когда вернулись с глубины откатываем ход
 6 если ходов не было то это случай мата или пата

 альфа бета с амортизацией отказов
 int AlphaBeta (pos, depth, alpha, beta) 
 {
	if (depth == 0) return Evaluate(pos);
	best = -INFINITY;
	succ = Successors(pos);
	while (!Empty(succ)) 
	{
		pos = RemoveOne(succ);
		value = -AlphaBeta(pos, depth - 1, -beta, -alpha);
		if (value > best) best = value;
	    if (value > alpha) alpha = value;
        if (alpha >= beta) return alpha;
	}
	return best;
 }
*/


namespace Search
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

/*
	поиск за белых
*/
	__int32 white_searching_max_value
	(
		struct Position & position,/// представление доски
		__int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth,/// глубина
		const unsigned __int8 depth_max,/// максимальная глубина
		const bool flag_pv_node,/// основной ли это вариант (pv)

		const __int32 old_description_move,/// тип хода который привел к данной позиции
		bool flag_old_check,///флаг шаха
		const bool flag_null_m/// флаг нулевого хода
	);

/*
	поиск за черных 
*/
	__int32 black_searching_min_value
	(
		struct Position & position,/// представление доски
		const __int32 alpha,///
		__int32 beta,///
		const unsigned __int8 depth,/// глубина
		const unsigned __int8 depth_max,/// максимальная глубина
		const bool flag_pv_node,/// основной ли это вариант (pv)

		const __int32 old_description_move,/// тип хода который привел к данной позиции
		bool flag_old_check,///флаг шаха
		const bool flag_null_m/// флаг нулевого хода
	);

/////////////////////////////////////////////////////////////////////////////////////
//private:

/*
	очень эффективная эвристика известная под именем нулевого хода
	я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
	суть его в том что
	мы пропускаем свой ход и если даже при такой форе противник не находит
	лучшего хода эту ветку можно подрезать
	за белых
*/
	inline __int32 white_null_move
	(
		struct Position & position,/// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,/// максимальная глубина
		const unsigned __int8 depth,/// глубина
		const __int32 old_description_move,/// тип хода который привел к данной позиции
		const bool flag_pv_node,/// основной ли это вариант (pv)
		const bool flag_old_check,///флаг шаха
		const bool flag_null_m,/// флаг нулевого хода
		const unsigned __int8 new_depth, /// следующая глубина
		const unsigned __int64 key_undo
	);

/*
	мы пропускаем свой ход и если даже при такой форе противник не находит
	лучшего хода эту ветку можно подрезать
	за черных
*/
	inline __int32 black_null_move
	(
		struct Position & position,/// представление доски
		const __int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth_max,/// максимальная глубина
		const unsigned __int8 depth,/// глубина
		const __int32 old_description_move,/// тип хода который привел к данной позиции
		const bool flag_pv_node,/// основной ли это вариант (pv)
		const bool flag_old_check,///флаг шаха
		const bool flag_null_m,/// флаг нулевого хода
		const unsigned __int8 new_depth, /// следующая глубина
		const unsigned __int64 key_undo
	);


/*
	производим расчеты на максимальной глубине. для белых
*/
	inline __int32 max_depth_white
	(
		struct Position & position,/// представление доски
		const __int32 alpha,///
		const __int32 beta,///
		const bool flag_pv_node,/// основной ли это вариант (pv)
		const unsigned __int8 depth/// глубина
	);

/*
	производим расчеты на максимальной глубине. для черных
*/
	inline __int32 max_depth_black
	(
		struct Position & position,/// представление доски
		const __int32 alpha,///
		const __int32 beta,///
		const bool flag_pv_node,/// основной ли это вариант (pv)
		const unsigned __int8 depth/// глубина
	);


/*

*/
	inline void quit_or_stop();

/*

*/
	void set_depth_max_rem
	(
		unsigned __int8 _depth_max_rem//
	);

/*

*/
	void ini_deep_time_terminate
	(
		double _deep_time_terminate//
	);


/*

*/ 
	void ini_global_loop();


/*
	возвращаем флаг остановки поиска. так если в глубине поиска получили команду
	на остановку то передаем ее наружу
*/
	bool get_stop_search();

/*
	возвращаем количество рассмотренных узлов
*/
	__int64  get_nodes();

/*
	обнуляем количество рассмотренных узлов
*/
	void set_nodes_in_0();


};
