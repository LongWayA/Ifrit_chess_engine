
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 20.11.2010
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%


///!
#include "a_move_list_struct.h"

/// включаем-выключаем(1-0) контроль списка ходов
#define KONTROL_B 0 

/// 
#define TEST_H 0 

/*

Move Ordering
http://chessprogramming.wikispaces.com/Move+Ordering

 For the Alpha-Beta algorithm to perform well, the best moves need 
 to be searched first. This is especially true for PV-nodes and 
 expected Cut-nodes. The goal is to become close to the minimal tree. 
 On the other hand - at Cut-nodes - the best move is not always the 
 cheapest refutation, see for instance enhanced transposition cutoff. 
 Most important inside an iterative deepening framework is to try the 
 principal variation of the previous iteration as the leftmost path 
 for the next iteration, which might be applied by an explicit triangular 
 PV-table or implicit by the transposition table.

Standard techniques
 Following techniques are common in finding a good first move
 PV-Move from the principal variation of the previous Iteration
 Hash Move - stored move from Transposition Table, if available
 Internal Iterative Deepening - if no hash move is available, likely only at PV-Nodes

Captures
 For captures (if any), a simple, but quite efficient heuristic is 
 (re)capturing the last moved piece with the least valuable attacker. 
 Otherwise following heuristics may used, concerning the order of captures:

 MVV-LVA - Most Valuable Victim - Least Valuable Aggressor

Non-Captures
 Killer Heuristic [1]
 History Heuristic [2]

end link(конец цитаты)---------------------
*/

/*
 сортировка списка ходов
 сортируем по предварительной оценке ходов
*/

namespace Move_ordering 
{


//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// полная сортировка
	void sorting_moves_captures
	(
		struct List & list/// представление списка ходов
	);

	///---------------------------------------------------------------------------
	/// полная сортировка корневого списка
	void sorting_moves_history
	(
		struct List & list/// представление списка ходов
	);

	///---------------------------------------------------------------------------
	/// полная сортировка корневого списка
	void sorting_white_moves_root
	(
		struct List & list/// представление списка ходов
	);

	///---------------------------------------------------------------------------
	/// полная сортировка корневого списка
	void sorting_black_moves_root
	(
		struct List & list/// представление списка ходов
	);

};