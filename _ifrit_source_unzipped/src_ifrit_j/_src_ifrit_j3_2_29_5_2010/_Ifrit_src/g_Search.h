#include "a_Structure.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%

 перебор позиций

 включает в себя данные и методы основного перебора
 
 --------------------------------------------------------------
 модуль самый сложный и запутанный 
 не вызывает сомнений то что он перегружен
 его надо упрощать и разбивать но пока не ясно как это сделать

*/


namespace FULLSEARCH
{
	//---------------------------------------------------------------------------
	// 
	void public_set_depth_max_rem
	(
		unsigned __int8 depth_max_rem_v//
	);


	//---------------------------------------------------------------------------
	// 
	void public_ini_flag_stop_null_move
	(
		bool flag_stop_null_move_v//
	);

	//---------------------------------------------------------------------------
	// 
	void public_ini_deep_time_terminate
	(
		double deep_time_terminate_v//
	);


	//---------------------------------------------------------------------------
	// 
	void public_ini_global_loop
	(
		unsigned __int8 depth_max//
	);


	//---------------------------------------------------------------------------
	// возвращаем флаг остановки поиска. так если в глубине поиска получили команду
	// на остановку то передаем ее наружу
	bool public_get_stop_search();

	//---------------------------------------------------------------------------
	// возвращаем лучший вариант
	struct T_PV * public_public_get_pv_best();

	//---------------------------------------------------------------------------
	// возвращаем количество рассмотренных узлов
	__int64  public_get_nodes();

	//---------------------------------------------------------------------------
	//обнуляем количество рассмотренных узлов
	void public_set_nodes_in_0();

	//---------------------------------------------------------------------------
	// поиск за белых
	__int32 public_white
	(
		struct T_Bitboard * const p_bitboard,// представление доски
		__int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// максимальная глубина
		const unsigned __int8 depth,// глубина
		const __int32 description_move,// тип хода который привел к данной позиции
		const bool flag_pv_f,// основной ли это вариант (pv)
		bool flag_check,//флаг шаха
		const bool flag_null_m// флаг нулевого хода
	);

	//---------------------------------------------------------------------------
	// поиск за черных 
	__int32 public_black
	(
		struct T_Bitboard * const p_bitboard,// представление доски
		const __int32 alpha,//
		__int32 beta,//
		const unsigned __int8 depth_max,// максимальная глубина
		const unsigned __int8 depth,// глубина
		const __int32 description_move,// тип хода который привел к данной позиции
		const bool flag_pv_f,// основной ли это вариант (pv)
		bool flag_check,//флаг шаха
		const bool flag_null_m// флаг нулевого хода
	);


	/////////////////////////////////////////////////////////////////////////////////////
	//private:

	//---------------------------------------------------------------------------
	// производим расчеты на максимальной глубине. для белых
	inline __int32 max_depth_white
	(
		struct T_Bitboard * const p_bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// основной ли это вариант (pv)
		const unsigned __int8 depth// глубина
	);

	//---------------------------------------------------------------------------
	// производим расчеты на максимальной глубине. для черных
	inline __int32 max_depth_black
	(
		struct T_Bitboard * const p_bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// основной ли это вариант (pv)
		const unsigned __int8 depth// глубина
	);

	//---------------------------------------------------------------------------
	// продление на шахах. для белых
	inline void extension_for_chek_white
	(
		const struct T_Bitboard * const p_bitboard,// представление доски
		const unsigned __int8 depth_max,// максимальная глубина
		unsigned __int8 & depth_max_new,// новая максимальная глубина
		bool & flag_check// флаг шаха
	);

	//---------------------------------------------------------------------------
	// продление на шахах. для черных
	inline void extension_for_chek_black
	(
		const struct T_Bitboard * const p_bitboard,// представление доски
		const unsigned __int8 depth_max,// максимальная глубина
		unsigned __int8 & depth_max_new,// новая максимальная глубина
		bool & flag_check// флаг шаха
	);

	//---------------------------------------------------------------------------
	// реализуем продление на взятиях
	inline void extension_for_captures
	(
		const __int32 description_move,// тип хода
		const __int32 description_move_up,// тип хода на предыдущей глубине
		const unsigned __int8 depth_max,// максимальная глубина
		unsigned __int8 & depth_max_new// новая максимальная глубина
	);

	//---------------------------------------------------------------------------
	// полная сортировка
	inline void sorting_moves
	(
		struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
	);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// глубина перебора, список
#if TEST_L
	void list_Print
	(
		int Depth,// текущая глубина
		struct T_list_surplus_moves * p_list_surplus_moves// список ходов
	);
#endif

}
