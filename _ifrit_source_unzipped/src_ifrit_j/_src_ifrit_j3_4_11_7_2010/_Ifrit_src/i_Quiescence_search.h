#include "a_Structure.h"

#define TEST_F 0; // включаем-выключаем(1-0) режим вывода в файл тестовой информации

#define DELTA_PRUNING 1 // включаем-выключаем(1-0) режим
// нужно основательно тестировать

//---------------------------------------------------------------------------
/*+
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.12.2008 19.11.2009
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 80%

включает в себя данные и методы форсированного перебора:
смотрим только взятия 
используем delta pruning
*/

namespace  QUIESCENCESEARCH
{
	//---------------------------------------------------------------------------
	// просчитываем только взятия для белых
	__int32 public_forced_variant_white
	(
		struct T_Bitboard * const p_bitboard,// представление доски
		__int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth,// глубина
		const bool flag_pv_f// основной ли это вариант (pv)
	);

	//---------------------------------------------------------------------------
	// просчитываем только взятия для черных
	__int32 public_forced_variant_black
	(
		struct T_Bitboard * const p_bitboard,// представление доски
		const __int32 alpha,//
		__int32 beta,//
		const unsigned __int8 depth,// глубина
		const bool flag_pv_f// основной ли это вариант (pv)
	);

	//////////////////////////////////////////////////////////////////////////////////////////
	//private:

	//---------------------------------------------------------------------------
	// сортируем взятия. возвращаем количество взятий
	inline unsigned __int8 sorting_captures_moves
	(
		struct T_list_surplus_moves * const list_surplus_moves// список ходов
	);

	//---------------------------------------------------------------------------
	//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	// печатаем список
	// глубина перебора, список
#if TEST_F

	void list_Print_F
	(
		int depth,// текущая глубина
		const struct T_list_surplus_moves * list_surplus_moves//список ходов
	);
#endif

}
