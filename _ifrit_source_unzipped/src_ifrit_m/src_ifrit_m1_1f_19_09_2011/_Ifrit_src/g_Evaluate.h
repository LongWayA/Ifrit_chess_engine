
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 24.02.2009
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%



#include "a_Position.h"


/// оцениваем позицию

/// считаем 
/// материал (веса белых плюсуем а веса черных отнимаем)
/// позиционную оценку PST
/// мобильность фигур (отключенно)
/// атаку на окрестности короля (отключенно)
/// проходные


class g_Evaluate 
{

//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	///  функция получает позицию и оценивает разницу материала. белые - черные
	__int32 eval_difference_material
	(
		struct Position & position/// представление доски
	);

	///---------------------------------------------------------------------------
	/// функция получает позицию и оценивает материал для белых.
	__int32 eval_white_material
	(
		struct Position & position/// представление доски
	);

	///---------------------------------------------------------------------------
	/// функция получает позицию и оценивает материал для черных.
	__int32 eval_black_material
	(
		struct Position & position/// представление доски
	);

	///---------------------------------------------------------------------------
	/// функция получает позицию и оценивает ее статически
	__int32 eval_state
	(
		struct Position & position/// представление доски
	);


//////////////////////////////////////////////////////////////////////////////////////
private:

	///---------------------------------------------------------------------------
	/// считаем взведенные поля
	//inline __int32 popcnt(unsigned __int64 analyzed_number);

	///---------------------------------------------------------------------------
	///
	inline void king_pst
	(
		struct Position & position,/// представление доски
		unsigned __int8 & from_white_king,/// расположение белого короля
		unsigned __int8 & from_black_king,/// расположение черного короля
		const unsigned __int16 stadia_w, /// сумарный вес фигур у белых
		const unsigned __int16 stadia_b, /// сумарный вес фигур у черных
		__int32 & eval ///статическая оценка
	);

	///---------------------------------------------------------------------------
	///
	inline void pawn_pst
	(
		struct Position & position,/// представление доски
		const unsigned __int8 from_white_king,/// расположение белого короля
		const unsigned __int8 from_black_king,/// расположение черного короля
		const unsigned __int16 stadia_w, /// сумарный вес фигур у белых
		const unsigned __int16 stadia_b, /// сумарный вес фигур у черных
		__int32 & eval ///статическая оценка
	);

	///---------------------------------------------------------------------------
	///
	inline void PST
	(
		unsigned __int64 mask_mobility_from_coord[MAX_COORDINATE],
		struct Position & position,/// представление доски
		const unsigned __int8 from_white_king,/// расположение белого короля
		const unsigned __int8 from_black_king,/// расположение черного короля
		unsigned __int8 & at_white_king, /// количество атаковнных полей вокруг белого короля
		unsigned __int8 & at_black_king, /// количество атаковнных полей вокруг черного короля
		__int32 & eval ///статическая оценка
	);

	///---------------------------------------------------------------------------
	/// штраф за разрушение щита перед королем
	/// если нет прикрытия пешкой то назначается штраф 
	/// (раньше было прикрытие любой фигурой, но это очень глупо :))
	inline __int32 king_shield
	(
		struct Position & position,/// представление доски
		unsigned __int8 n,/// положение короля (from)
		unsigned __int8 z,/// цвет короля
		const unsigned __int8 SHIELD/// бонус за прикрытие короля
	);

	///---------------------------------------------------------------------------
	/// просчитываем все возможные ходы ладьи возвращаем количество ходов
	/// и количество атакованных полей вокруг короля
	inline unsigned __int8 rook_amoun
	(
		unsigned __int64 all_piece,/// все фигуры
		unsigned __int64 piece,/// свои фигуры
		unsigned __int8 from,/// расположение ладьи
		unsigned __int8 from_king,/// расположение короля
		unsigned __int8 & at_cnt/// количество атакованных полей вокруг короля(квадрат ходов короля)
	);

	///---------------------------------------------------------------------------
	/// просчитываем все возможные ходы слона возвращаем количество ходов
	/// и количество атакованных полей вокруг короля
	inline unsigned __int8 bishop_amoun
	(
		unsigned __int64 all_piece,/// все фигуры
		unsigned __int64 piece,/// свои фигуры
		unsigned __int8 from,/// расположение слона
		unsigned __int8 from_king,/// расположение короля
		unsigned __int8 & at_cnt/// количество атакованных полей вокруг короля(квадрат ходов короля)
	);

	///---------------------------------------------------------------------------
	/// просчитываем все возможные ходы ферзя. возвращаем количество ходов
	/// и количество атакованных полей вокруг короля
	inline unsigned __int8 queen_amoun
	(
		unsigned __int64 all_piece,/// все фигуры
		unsigned __int64 piece,/// свои фигуры
		unsigned __int8 from,/// расположение ферзя
		unsigned __int8 from_king,/// расположение короля
		unsigned __int8 & at_cnt/// количество атакованных полей вокруг короля(квадрат ходов короля)
	);

	///---------------------------------------------------------------------------
	/// просчитываем все возможные ходы коня. возвращаем количество ходов
	/// и количество атакованных полей вокруг короля
	inline unsigned __int8 knigt_amoun
	(
		unsigned __int64 all_piece,/// все фигуры
		unsigned __int64 piece,/// свои фигуры
		unsigned __int8 from,/// расположение коня
		unsigned __int8 from_king,/// расположение короля
		unsigned __int8 & at_cnt/// количество атакованных полей вокруг короля(квадрат ходов короля)
	);

	///---------------------------------------------------------------------------
	///
	inline __int32 popcnt(unsigned __int64 analyzed_number);

};



/*

Other factors that affect material evaluation might be:

Bonus for the bishop pair (bishops complement each other, controlling squares of different color)
Penalty for the rook pair (Larry Kaufman called it "principle of redundancy")
Penalty for the knight pair (as two knights are less successful against the rook than any 
other pair of minor pieces)
decreasing the value of the rook pawns and increasing the value of the central pawns (though this 
can be done in the piece-square tables as well)
Trade down bonus that encourages the winning side to trade pieces but no pawns
Penalty for having no pawns, as it makes it more difficult to win the endgame
Bad trade penalty as proposed by Robert Hyatt, that is penalizing the material imbalances that 
are disadvantageous like having three pawns for a piece or a rook for two minors.


*/







