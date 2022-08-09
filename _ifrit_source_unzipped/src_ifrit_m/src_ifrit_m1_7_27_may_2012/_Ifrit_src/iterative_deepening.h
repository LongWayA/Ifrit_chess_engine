
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009 25.12.2010 23.5.2012
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%

#include "a_position_struct.h"

/*
Iterative deepening (ID)
http://chessprogramming.wikispaces.com/Iterative+Deepening

Iterative deepening (ID) has been invented as the basic time 
management strategy in depth-first searches, but proved 
surprisingly beneficial as far as the move ordering is concerned 
in alpha-beta and its enhancements. 
It has been noticed, that even if one is about to search to a 
given depth, the iterative deepening is faster than searching 
for the given depth immediately. It happens due to dynamic move 
ordering techniques, such as using PV-, hash- and refutation 
moves determined in previous iteration(s), as well the history 
heuristic.

How it Works
It works as follows: program starts with a one ply search, 
then increments the search depth and does another search. 
This process is repeated until the time allocated for the 
search is exhausted. In case of an unfinished search, program 
has always an option to fall back to the move from the less 
deep search. Yet if we make sure that this move is searched 
first in the next iteration, then overwriting the new move with 
the old one becomes unnecessary. This way, also the results from 
the partial search can be accepted - though in case of a severe 
drop of the score it is wise to allocate some more time, as the 
first alternative is often a bad capture, delaying the loss instead 
of preventing it.
Iterative deepening, using a TT, embed the depth-first algorithms 
like alpha-beta into a framework with best-first characteristics.

end link(конец цитаты)---------------------
*/

namespace Iterative_deepening
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

//////////////////////////////////////////////////////////////////////////////////////
//private:

/*
	перебор с циклическим увеличением глубины

	циклически перебираем увеличивая глубину до тех пор пока не сработает останов.
	т.е. в начале на глубине 1 потом 2 и т.д.
	это нужно во первых для заполнения хеш-таблицы
	во вторых чтобы чувствовать время перебора. например если мы на предыдущей глубине
	считали секунду то на следующем в среднем в двадцать раз дольше.

*/
	void loop
	(
		struct Position & position, /// представление доски
		unsigned __int8 & depth_max,
		double & max_time_move_search
	);

//////////////////////////////////////////////////////////////////////////////////////
//private:

/*
проверка оценки главной линии, проверка оценки материала
проверяем хеш-ключ на корректность, 
контроль соответствия первого хода первому ходу линии варианта
*/
	void test_loop
	(
		struct Position & position,// представление доски
		__int32 value_root,
 		const __int32 test_eval,// запоминаем оценку материала для контроля
		const unsigned __int64 key_control// запоминаем хеш-ключ для контроля
	);

};
