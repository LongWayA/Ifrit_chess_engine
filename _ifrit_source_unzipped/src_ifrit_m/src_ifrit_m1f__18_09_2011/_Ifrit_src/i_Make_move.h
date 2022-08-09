
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 20.11.2010
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 99%



///!
#include "a_Header_constants.h"
#include "a_Position.h"
#include "a_List.h"
#include "a_Undo.h"

/// включаем-выключаем(1-0) режим расчета материала
#define MATERIAL 1 


/// задаем функции по реализации ходов

/// собраны методы по реализации ходов из списка List_surplus_moves
/// в итоге работы обновляется доска Chess_bitboard

class i_Make_move
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
private:

	///информация для отката хода
	struct Undo d_undo[MAX_DEPTH];

/// доступ к полям класса
//////////////////////////////////////////////////////////////////////////////////////
public:

	/// 
	inline struct Undo get_d_undo(const unsigned __int8 depth)
	{
		return  d_undo[depth];
	};


//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	/// реализуем ходы вперед для белых
	void do_moves_white
	(
		struct Position & position, ///
		const unsigned __int8 & i,///номер хода по списку
		const unsigned __int8 & depth,///глубина поиска
		const struct List & list_surplus_moves///список возможных ходов
	);

	///---------------------------------------------------------------------------
	/// реализуем ходы вперед для черных
	void do_moves_black
	(
		struct Position & position, ///
		const unsigned __int8 & i,///номер хода по списку
		const unsigned __int8 & depth,///глубина поиска
		const struct List & list_surplus_moves///список возможных ходов
	);

	///---------------------------------------------------------------------------
	/// реализуем ходы назад для белых
	void undo_moves_white
	(
		struct Position & position, ///
		const unsigned __int8 & i,///номер хода по списку
		const unsigned __int8 & depth,///глубина поиска
		const struct List & list_surplus_moves///список возможных ходов
	);

	///---------------------------------------------------------------------------
	/// реализуем ходы назад для черных
	void undo_moves_black
	(
		struct Position & position, ///
		const unsigned __int8 & i,///номер хода по списку
		const unsigned __int8 & depth,///глубина поиска
		const struct List & list_surplus_moves///список возможных ходов
	);

};