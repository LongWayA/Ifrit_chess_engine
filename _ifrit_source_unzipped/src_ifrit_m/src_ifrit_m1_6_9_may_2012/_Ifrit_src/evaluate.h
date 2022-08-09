
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 24.02.2009 09.10.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%



#include "position_struct.h"

/*
evaluation
http://chessprogramming.wikispaces.com/evaluation

An evaluation function is used to heuristically determine 
the relative value of a position, i.e. the chances of winning. 
If we could see to the end of the game in every line, the evaluation 
would only have values of -1 (loss), 0 (draw), and 1 (win). 
In practice, however, we do not know the exact value of a position, 
so we must make an approximation. Beginning chess players learn to 
do this starting with the value of the pieces themselves. Computer 
evaluation functions also use the value of the material as the most 
significant aspect and then add other considerations.

*/

/// оцениваем позицию

/// считаем 
/// материал (веса белых плюсуем а веса черных отнимаем)
/// позиционную оценку PST
/// мобильность фигур (отключенно)
/// атаку на окрестности короля (отключенно)
/// проходные


namespace Evaluate 
{

//////////////////////////////////////////////////////////////////////////////////////
//public:

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
//private:

	///---------------------------------------------------------------------------
	/// считаем взведенные поля
	//inline __int32 popcnt(unsigned __int64 analyzed_number);

	///---------------------------------------------------------------------------
	///
	inline void king_pst_opening
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
	inline void king_pst_endgame
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
	inline void pawn_pst_opening
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
	inline void pawn_pst_endgame
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
	inline void PST_opening
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
	///
	inline void PST_endgame
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







