
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  8.09.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 0%



///!
#include "a_move_list_struct.h"


/// включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_HISTORY 0 

/*

History heuristic
http://chessprogramming.wikispaces.com/History+Heuristic

 History heuristic is a dynamic move ordering method based on 
 the number of cutoffs caused by a given move irrespectively 
 from the position in which the move has been made. The Heuristic 
 was invented by Jonathan Schaeffer in 1983 [1] and works as follows: 
 on a cutoff we increment a counter in a special table, addressed 
 either by [from][to] (the Butterfly Boards) or by [piece][to]. 
 The added value is typically depth * depth or 2 ^ depth, based 
 on the assumption that otherwise moves from the plies near the 
 leaves would have to much impact on the result. Values retrieved 
 from that table are used to order non-capturing moves. This simple 
 heuristics performs usually better than domain-dependent heuristics, 
 though it may be combined with them. For example, in Rebel only a 
 few non-captures are ordered by history heuristics, then a 
 piece-square approach is used [2] . In the literature, history 
 heuristic is often presented as depth-independent generalization 
 of the killer moves. It is also said to reflect long-term plans 
 in a position.

 However, all of those statements were made at the time when typical 
 search depth was much lower than today. Nowadays authors of the leading 
 programs say that given enough search depth, history heuristic produces 
 just a random noise. In Crafty it is not used anymore [3] , 
 whereas Ed Schröder, author of Rebel, advocated not taking into account 
 the cutoffs from the last couple of plies.

 This is how the history array may be updated, if a beta-cutoff occurs:

   if ( score >= beta ) { // cutoff
      if ( isNonCapture (move) )
         history[move.from][move.to] += depth*depth; // 1 << depth
      ...
      return score;
   }

end link(конец цитаты)---------------------
*/

/*
 реализуем историю

 если ход  что он улучшил альфу и вызвал отсечку по бете
 значит ему плюсуем а все остальные минусуем 
 конечно это для поиска за белых
 для черных улучшил бету и вызвал отсечку по альфе

*/

namespace History_heuristic
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	///
	__int32 get_white_history
	(
		const unsigned __int8 n,///
		const unsigned __int8 c ///
	);

	///---------------------------------------------------------------------------
	///
	__int32 get_black_history
	(
		const unsigned __int8 n,///
		const unsigned __int8 c ///
	);

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// инициализируем массив истории
	void ini_history();

	///---------------------------------------------------------------------------
	/// пишем историю
	void white_history_good_save
	(
		const unsigned __int8 i,/// номер хода в списке
		struct List & list,/// представление списка ходов
		const unsigned __int8 depth,// глубина записи хода
		const unsigned __int8 depth_max// максимальная глубина расчета
	);

	///---------------------------------------------------------------------------
	/// пишем историю
	void black_history_good_save
	(
		const unsigned __int8 i,/// номер хода в списке
		struct List & list,/// представление списка ходов
		const unsigned __int8 depth,// глубина записи хода
		const unsigned __int8 depth_max// максимальная глубина расчета
	);

	///---------------------------------------------------------------------------
	/// пишем историю
	inline void white_history_bad_save
	(
		const unsigned __int8 e,/// номер хода в списке
		struct List & list,/// представление списка ходов
		const unsigned __int8 depth,// глубина записи хода
		const unsigned __int8 depth_max// максимальная глубина расчета
	);

	///---------------------------------------------------------------------------
	/// пишем историю
	inline void black_history_bad_save
	(
		const unsigned __int8 e,/// номер хода в списке
		struct List & list,/// представление списка ходов
		const unsigned __int8 depth,// глубина записи хода
		const unsigned __int8 depth_max// максимальная глубина расчета
	);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем
// максимальная глубина
#if TEST_HISTORY

	void History_heuristic::test_print
	(	
		const unsigned __int8 depth_max// максимальная глубина расчета
	);

#endif

};
