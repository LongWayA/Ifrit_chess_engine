
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  25.02.2009
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 80%



///!
#include "constants_header.h"
#include "move_list_struct.h"


/// включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_K 0 

/*

Killer Heuristic
http://chessprogramming.wikispaces.com/Killer+Heuristic

 The Killer Heuristic is a dynamic, path-dependent move ordering 
 technique. It considers moves that caused a beta-cutoff in a 
 sibling node as killer moves and orders them high on the list. 
 When a node fails high, a non-capturing move that caused a 
 cutoff is stored in a table indexed by depth, typically 
 containing two or three moves per ply. The replacement 
 scheme ought to ensure that all the available slots 
 contain different moves.

 In move ordering, killer moves usually come right after after 
 the hash move and (good) captures. The logic behind this 
 heuristic is as follows. In many positions there is only a 
 small set of moves creating a threat or defending against it, 
 and those that cannot do it might be refuted ("killed") by the 
 same move by the opponent. Apart from the killer moves from the 
 same depth, some programs use killers from two plies ago. Also 
 the mate killers are often separated and treated differently.

 As far as relative position of captures and killer moves 
 is concerned, there are different schemes. Sometimes killer 
 moves are sorted below all the captures, sometimes - between 
 equal and losing captures. One rare idea was to place them 
 even before winning captures of a pawn.


 реализуем киллеры

 киллер ход это лучший ход найденный до этого на данной глубине
 он не должен быть взятием т.к. с взятиями разговор особый

 что значит лучший? 
 это значит  
 что он улучшил альфу и вызвал отсечку по бете
 конечно это для поиска за белых
 для черных улучшил бету и вызвал отсечку по альфе

*/

namespace Killer_heuristic
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:


//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	///
	__int32 get_killer_moves_1(const unsigned __int16 n);

	///---------------------------------------------------------------------------
	/// 
	__int32 get_killer_moves_2(const unsigned __int16 n);

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// инициализируем массив киллеров
	void ini_killer();

	///---------------------------------------------------------------------------
	/// пишем киллера
	void save_killer
	(
		const unsigned __int8 i,/// номер хода в списке
		struct List & list,/// представление списка ходов
		const unsigned __int8 depth/// текущая глубина
	);

	///---------------------------------------------------------------------------
	/// вставляем киллера в начало списка(позиция start) 
	void insert_killer
	(
		struct List & list,/// представление списка ходов
		const unsigned __int8 depth,/// текущая глубина
		unsigned __int8 start///позиция с которой ищем и на которую вставляем киллер
	);

	///---------------------------------------------------------------------------
	/// вставляем киллера в начало списка(позиция start) 
	void insert_killer_in_list
	(
		struct List & list,/// представление списка ходов
		const __int32 killer_moves_1,/// 
		const __int32 killer_moves_2,///
		const unsigned __int8 start///позиция с которой ищем и на которую вставляем киллер
	);


};
