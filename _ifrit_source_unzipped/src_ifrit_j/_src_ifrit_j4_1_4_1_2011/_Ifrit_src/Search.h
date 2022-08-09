#include "Chess_bitboard.h"

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


*/


class Search
{

// поля класса
//////////////////////////////////////////////////////////////////////////////////////////
private:

	unsigned __int16 audit;// задержка опроса команды экстренного выхода
	unsigned __int8 depth_max_rem;//
	bool flag_stop_search;// переменная для отсечки перебора при экстренном выходе
	__int64 nodes;// количество узлов
	__int8 string_in2[65536];//
	unsigned __int8 from;// откуда ходит фигура
	unsigned __int8 to;// куда ходит фигура
	bool flag_stop_null_move;// останавливаем использование нулевого хода
	//int return_hash_score;

	double deep_time_terminate; // передаем на глубину время остановки счета 

//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// 
	void set_depth_max_rem
	(
		unsigned __int8 depth_max_rem_v//
	);


	//---------------------------------------------------------------------------
	// 
	void ini_flag_stop_null_move
	(
		bool flag_stop_null_move_v//
	);

	//---------------------------------------------------------------------------
	// 
	void ini_deep_time_terminate
	(
		double deep_time_terminate_v//
	);


	//---------------------------------------------------------------------------
	// 
	void ini_global_loop();


	//---------------------------------------------------------------------------
	// возвращаем флаг остановки поиска. так если в глубине поиска получили команду
	// на остановку то передаем ее наружу
	bool get_stop_search();

	//---------------------------------------------------------------------------
	// возвращаем лучший вариант
	struct PV_line & get_pv_best();

	//---------------------------------------------------------------------------
	// возвращаем количество рассмотренных узлов
	__int64  get_nodes();

	//---------------------------------------------------------------------------
	//обнуляем количество рассмотренных узлов
	void set_nodes_in_0();

	//---------------------------------------------------------------------------
	// поиск за белых
	__int32 white
	(
		class Chess_bitboard & bitboard,// представление доски
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
	__int32 black
	(
		class Chess_bitboard & bitboard,// представление доски
		const __int32 alpha,//
		__int32 beta,//
		const unsigned __int8 depth_max,// максимальная глубина
		const unsigned __int8 depth,// глубина
		const __int32 description_move,// тип хода который привел к данной позиции
		const bool flag_pv_f,// основной ли это вариант (pv)
		bool flag_check,//флаг шаха
		const bool flag_null_m// флаг нулевого хода
	);

	//---------------------------------------------------------------------------
	// продление на шахах. для белых
	inline void extension_for_chek_white
	(
		const class Chess_bitboard & bitboard,// представление доски
		const unsigned __int8 depth_max,// максимальная глубина
		unsigned __int8 & depth_max_new,// новая максимальная глубина
		bool & flag_check,// флаг шаха
		class List_surplus_moves & list_surplus_moves// список возможных ходов
	);

	//---------------------------------------------------------------------------
	// продление на шахах. для черных
	inline void extension_for_chek_black
	(
		const class Chess_bitboard & bitboard,// представление доски
		const unsigned __int8 depth_max,// максимальная глубина
		unsigned __int8 & depth_max_new,// новая максимальная глубина
		bool & flag_check,// флаг шаха
		class List_surplus_moves & list_surplus_moves// список возможных ходов
	);

/////////////////////////////////////////////////////////////////////////////////////
private:

	//---------------------------------------------------------------------------
	// за белых
	inline void PVS_white
	(
		class Chess_bitboard & bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// максимальная глубина
		const unsigned __int8 depth,// глубина
		const __int32 description_move,// тип хода который привел к данной позиции
		const bool flag_pv_f,// основной ли это вариант (pv)
		const bool old_flag_check,//флаг шаха
		const bool flag_null_m,// флаг нулевого хода
		const unsigned __int8 new_depth,// следующая глубина
		const unsigned __int64 key_undo,
		const unsigned __int8 i,
		__int32 & value,// текущая оценка позиции
		const unsigned __int8 depth_max_new,
		class List_surplus_moves & list_surplus_moves,
		const bool flag_check
	);


	//---------------------------------------------------------------------------
	//  за черных
	inline void PVS_black
	(
		class Chess_bitboard & bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// максимальная глубина
		const unsigned __int8 depth,// глубина
		const __int32 description_move,// тип хода который привел к данной позиции
		const bool flag_pv_f,// основной ли это вариант (pv)
		const bool old_flag_check,//флаг шаха
		const bool flag_null_m,// флаг нулевого хода
		const unsigned __int8 new_depth,// следующая глубина
		const unsigned __int64 key_undo,
		const unsigned __int8 i,
		__int32 & value,// текущая оценка позиции
		const unsigned __int8 depth_max_new,
		class List_surplus_moves & list_surplus_moves,
		const bool flag_check
	);

	//---------------------------------------------------------------------------
	// очень эффективная эвристика известная под именем нулевого хода
	// я долго не мог оценить его по достоинству но в итоге истина мне открылась :)
	// суть его в том что
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	// за белых
	inline __int32 null_move_white
	(
		class Chess_bitboard & bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// максимальная глубина
		const unsigned __int8 depth,// глубина
		const __int32 description_move,// тип хода который привел к данной позиции
		const bool flag_pv_f,// основной ли это вариант (pv)
		const bool old_flag_check,//флаг шаха
		const bool flag_null_m,// флаг нулевого хода
		const unsigned __int8 new_depth, // следующая глубина
		const unsigned __int64 key_undo
	);

	//---------------------------------------------------------------------------
	// мы пропускаем свой ход и если даже при такой форе противник не находит
	// лучшего хода эту ветку можно подрезать
	// за черных
	inline __int32 null_move_black
	(
		class Chess_bitboard & bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// максимальная глубина
		const unsigned __int8 depth,// глубина
		const __int32 description_move,// тип хода который привел к данной позиции
		const bool flag_pv_f,// основной ли это вариант (pv)
		const bool old_flag_check,//флаг шаха
		const bool flag_null_m,// флаг нулевого хода
		const unsigned __int8 new_depth, // следующая глубина
		const unsigned __int64 key_undo
	);


	//---------------------------------------------------------------------------
	// производим расчеты на максимальной глубине. для белых
	inline __int32 max_depth_white
	(
		class Chess_bitboard & bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// основной ли это вариант (pv)
		const unsigned __int8 depth// глубина
	);

	//---------------------------------------------------------------------------
	// производим расчеты на максимальной глубине. для черных
	inline __int32 max_depth_black
	(
		class Chess_bitboard & bitboard,// представление доски
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// основной ли это вариант (pv)
		const unsigned __int8 depth// глубина
	);

//////////////////////////////////////////////////////////////////////////////////////////
public:

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// глубина перебора, список
#if TEST_L
	void list_print
	(
		int Depth,// текущая глубина
		class List_surplus_moves & list_surplus_moves// список ходов
	);
#endif

};
