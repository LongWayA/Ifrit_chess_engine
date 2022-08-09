
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  8.09.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 0%



///!
#include "Header_constants.h"
#include "List.h"


/// включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_HISTORY 0 



/// реализуем историю


/// если ход  что он улучшил альфу и вызвал отсечку по бете
/// значит ему плюсуем а все остальные минусуем 
/// конечно это для поиска за белых
/// для черных улучшил бету и вызвал отсечку по альфе

namespace History
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///
	__int32 get_white_history
	(
		const unsigned __int8 n,///
		const unsigned __int8 c ///
	);

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

	void History::test_print
	(	
		const unsigned __int8 depth_max// максимальная глубина расчета
	);

#endif

};
