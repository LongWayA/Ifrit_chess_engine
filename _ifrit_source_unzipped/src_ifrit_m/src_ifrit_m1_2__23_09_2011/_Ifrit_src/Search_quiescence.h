
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 5.12.2008 19.11.2009
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 80%

#include "Position.h"

/// включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_F 0;

/// включаем-выключаем(1-0) режим
#define DELTA_PRUNING 1 

///!
/// отключил предупреждения которые выдаются из за использования условия 
/// для препроцессора #if #endif
/// unexpected tokens following preprocessor directive - expected a newline
#pragma warning(disable : 4067) 




/// форсированный перебор позиций 
/// включает в себя данные и методы форсированного перебора:
/// смотрим только взятия 
/// используем delta pruning

namespace Search_quiescence
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// просчитываем только взятия для белых
	__int32 forced_variant_white
	(
		struct Position & position,/// представление доски
		__int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth,/// глубина
		const bool flag_pv_f/// основной ли это вариант (pv)
	);

	///---------------------------------------------------------------------------
	/// просчитываем только взятия для черных
	__int32 forced_variant_black
	(
		struct Position & position,/// представление доски
		const __int32 alpha,///
		__int32 beta,///
		const unsigned __int8 depth,/// глубина
		const bool flag_pv_f/// основной ли это вариант (pv)
	);

	///---------------------------------------------------------------------------
	///TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
	/// печатаем список
	/// глубина перебора, список
#if TEST_F

	void list_Print_F
	(
		int depth,/// текущая глубина
		const namespace List_surplus_moves & list_surplus_moves///список ходов
	);
#endif

};
