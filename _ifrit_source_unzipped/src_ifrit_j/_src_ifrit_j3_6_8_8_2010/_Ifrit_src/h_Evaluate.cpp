#include <iostream> // TEST
#include <fstream>  // TEST
//#include <string>


#include "h_Evaluate.h"
#include "d_Bitboard_const.h"

//---------------------------------------------------------------------------
/* +
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 24.02.2009 03.07.2010
*/
//---------------------------------------------------------------------------


/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 70%

оцениваем позицию

считаем 
материал (веса белых плюсуем а веса черных отнимаем)
позиционную оценку PST
мобильность фигур
атаку на окрестности короля
проходные

*/


//	===================================================================
// функция получает позицию и оценивает ее статически смотрим позиционные факторы
__int32 EVALUATE::public_eval_state
(
	const T_Bitboard * p_bitboard// представление доски
)
{
	__int32 eval = 0;//статическая оценка
	unsigned __int8 from_white_king = 0;// расположение белого короля
	unsigned __int8 from_black_king = 0;// расположение черного короля
	unsigned __int8 at_white_king = 0; // количество атакованных полей вокруг белого короля
	unsigned __int8 at_black_king = 0;// количество атакованных полей вокруг черного короля

	// нужно для вычисления стадии игры
	unsigned __int16 stadia_w = 0; // сумарный вес фигур у белых
	unsigned __int16 stadia_b = 0; // сумарный вес фигур у черных

	//в начальной позиции мы имеем 400 + 400 + 600 + 1200 = 2600 (для одной стороны)
	//                             2*400 + 2*400 + 2*600 + 1200 
	//                             800   + 800   + 1200  + 1200 = 4000 (сумма для двух сторон) 

	//stadia_w = 2600; // сумарный вес фигур у белых
	//stadia_b = 2600; // сумарный вес фигур у черных

	// TEST popcnt-----------------------------
	//unsigned __int64 move = 0;

	//for (int i = 0; i < 64; i++ )
	//{
	//	move = move | e_move_masks[i];
	//	//std::cout << "popcnt= " << popcnt(e_move_masks[i]) << "\n";
	//	std::cout << "popcnt= " << popcnt(move) << "\n";
	//}
    // TEST-------------------------------------

	stadia_w = p_bitboard->value_white_material;
	stadia_b = p_bitboard->value_black_material;

	// обсчитываем короля
	king_pst(p_bitboard, from_white_king, from_black_king, stadia_w, stadia_b, eval);

	// обсчитываем пешки
	pawn_pst(p_bitboard, from_white_king, from_black_king, stadia_w, stadia_b, eval);

	// обсчитываем ферзь, ладья, слон, конь
	// бонус расположения 
	PST(p_bitboard, from_white_king, from_black_king, at_white_king, at_black_king, eval);

	return eval;
}

//	===================================================================
//
inline void EVALUATE::king_pst
(
	const T_Bitboard * p_bitboard,// представление доски
	unsigned __int8 & from_white_king,// расположение белого короля
	unsigned __int8 & from_black_king,// расположение черного короля
	const unsigned __int16 stadia_w, // сумарный вес фигур у белых
	const unsigned __int16 stadia_b, // сумарный вес фигур у черных
	__int32 & eval //статическая оценка
)
{
	unsigned __int64 from_mask = 0; // маска расположения фигур одного типа
	unsigned __int8 from; // координата поля откуда фигура ходит

	/*
	здесь обсчитываем рокировку и прикрытие короля фигурами 
	по сути меры направлены на безопасность короля

	Рубикон это граница перехода в эндшпиль.
	есть две разные оценки - до рубикона и после. до рубикона действуют бонусы за прикрытие
	фигурами и рокировку а после рубикона уже нет.

	прикрытие пешками это три пешки перед королем. за прорехи в щите идет штраф

	*/

	// белый король
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_white_king = from;
		//cout << "короли " << "\n";
		//cout << "from= "<< from << "\n";
		//cout << "Pozicion_k_e[from]= "<< Pozicion_k_e[from] << "\n";


		if (stadia_b > rubikon)
		{
			//-----------------------------------------------------------
			// фрагмент доски у нас выглядит как
			// .........
			// A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

			// срыв рокировки 0-0 т.е. король побежал прятаться без рокировки :)
			// к сведению: 64 - 6 bit  128 - 7 bit 64 + 128 = 192 // включены 6 и 7 бит

			//std::cout << "from= "<< (int)from << "\n";

			if (((p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] & 192) != 0) //  ладья заперта на 6 или 7 клетках
			&& ((from == G1) || (from == F1)) ) //  белый король в зоне рокировки 0-0
			{
				eval = eval - PENALTY_CASTLING_O_O; // тогда штрафуем 
                //std::cout << "oo from= "<< (int)from << "\n";
				//std::cout << "eval= "<< eval << "\n";
			}

			// срыв рокировки 0-0-0 т.е. король побежал прятаться без рокировки :)
			// к сведению: 1 - 0 bit   2 - 1 bit
			// 1 + 2 = 3 // включены 0,1 биты
			if (((p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK] & 3) != 0) //  ладья заперта на 6 или 7 клетках
			&& ((from == B1) || (from == C1) || (from == D1))) //  белый король в зоне рокировки 0-0-0
			{
				eval = eval - PENALTY_CASTLING_O_O_O; // тогда штрафуем 
				//std::cout << "ooo from= "<< (int)from << "\n";
				//std::cout << "eval= "<< eval << "\n";
			}
			//-----------------------------------------------------------

			eval = eval 
				+ pozicion_king_white[from] 
				- king_shield(from,1,p_bitboard,PENALTY_SHIELD)
				;

			//std::cout << "king from= "<< (int)from << "\n";
		}
		else
		{
			// после рубикона король должен стремиться к центру
			eval = eval + pozicion_king_white_end[from];
		}
	}

	//черный король
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KING];
	//from_mask = 0;

	if (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_black_king = from;

		if (stadia_w > rubikon)
		{

			//-----------------------------------------------------------
			// фрагмент доски у нас выглядит как
			// A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
			// .........

			// срыв рокировки 0-0 т.е. король побежал прятаться без рокировки :)
			// к сведению: 4611686018427387904 - 62 bit  9223372036854775808 - 63 bit 
			// 4611686018427387904 + 9223372036854775808 = 13835058055282163712  включены 62 и 63 бит

			if (( (p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
			& 13835058055282163712) != 0 ) //  ладья заперта на 6 или 7 клетках
			&& ( (from == F8) || (from == G8))) //  черный король в зоне рокировки 0-0
			{
				eval = eval + PENALTY_CASTLING_O_O; // тогда штрафуем 
			}

			// срыв рокировки 0-0-0 т.е. король побежал прятаться без рокировки :)
			// к сведению: 72057594037927936 - 56 bit   144115188075855872 - 57 bit
			// 72057594037927936 + 144115188075855872 = 216172782113783808  включены 56,57 биты
			if (( (p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK] 
			& 216172782113783808) != 0 ) //  ладья заперта на 6 или 7 клетках
			&& ( (from == B8) || (from == C8) || (from == D8))) //  черный король в зоне рокировки 0-0-0
			{
				eval = eval + PENALTY_CASTLING_O_O_O; // тогда штрафуем 
			}
			//-----------------------------------------------------------

			eval = eval 
				- pozicion_king_black[from] 
				+ king_shield(from,0,p_bitboard,PENALTY_SHIELD)
				;

		}
		else
		{
			// после рубикона король должен стремиться к центру
			eval = eval - pozicion_king_black_end[from];
		}
	}
}

//	===================================================================
// обсчет бонуса за расположение 
inline void EVALUATE::pawn_pst
(
	const T_Bitboard * p_bitboard,// представление доски
	const unsigned __int8 from_white_king,// расположение белого короля
	const unsigned __int8 from_black_king,// расположение черного короля
	const unsigned __int16 stadia_w, // сумарный вес фигур у белых
	const unsigned __int16 stadia_b, // сумарный вес фигур у черных
	__int32 & eval //статическая оценка
)
{
	// нужно для определения сдвоенных пешек 
	unsigned __int8 Structure_pawn_w[]=
	{
		0,0,0,0,0,0,0,0
	};

	unsigned __int8 Structure_pawn_b[]=
	{
		0,0,0,0,0,0,0,0
	};

	unsigned __int64 from_mask = 0; // маска расположения фигур одного типа

	unsigned __int8 from; // координата поля откуда фигура ходит

	// количество открытых линий перед пешкой считаем только если путь открыт
	// т.е. по сути таких линий две - слева и справа от пешки
	unsigned __int8 cnt_line_pawn_white = 0;
	unsigned __int8 cnt_line_pawn_black = 0;

	/*

	Рубикон это граница перехода в эндшпиль.
	есть две разные оценки - до рубикона и после.

	за сдвоенные пешки штраф

	*/

	// белые-------------------------------------------------

	//пешки

	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		if (stadia_b > rubikon)
		{
			
			if ((from_white_king == G1) || (from_white_king == H1))
			{// если сделали короткую рокировку
				eval = eval + pozicion_pawn_king_in_oo_white[from];
			}
			else if ((from_white_king == A1) || (from_white_king == B1) 
			|| (from_white_king == C1))
			{// если сделали длинную рокировку
				eval = eval + pozicion_pawn_king_in_ooo_white[from];
			}
			else
			{// если рокировки не получилось
				eval = eval + pozicion_pawn_king_in_center_white[from];
			}
		}
		else
		{
			eval = eval + pozicion_pawn_white_end[from];
            //eval = eval + test[from];
			//cout << "from= "<< from << "\n";
			//cout << "test[from]= "<< test[from] << "\n";
			//cout << "from % 8= "<< from % 8 << "\n";
		}


		// определяем проходные
		// в случае если перед пешкой нет вражеской пешки
		if (!(e_rook_up_masks[from] & p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN]))
		{ 
			eval = eval + PERSPECTIVE_PAWNN;// 10
			cnt_line_pawn_white = 0;

			// для случая крайней проходной
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_white = 1;

			// в случае если справо перед пешкой нет вражеской пешки
			if ((from % 8) != H1)
			{
				if (!(e_rook_up_masks[from + 1] 
				& p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN]))
				{ 
					eval = eval + PERSPECTIVE_PAWNN;
					cnt_line_pawn_white = cnt_line_pawn_white + 1;
				}
			}

			// в случае если слево перед пешкой нет вражеской пешки
			if ((from % 8) != A1)
			{
				if (!(e_rook_up_masks[from - 1] 
				& p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN]))
				{
					eval = eval + PERSPECTIVE_PAWNN;
					cnt_line_pawn_white = cnt_line_pawn_white + 1;
				}
			}

			// все три линии открыты это чистая проходная.
			if (cnt_line_pawn_white == 2) eval = eval 
			+ pozicion_perspective_pawn_white_end[from]; 
		}

		//если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
		Structure_pawn_w[from % 8] = Structure_pawn_w[from % 8] + 1;
		//std::cout << "w from % 8= "<< from % 8 << "\n";
	}


// черные-------------------------------------------------

	//пешки
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN];
	//from_mask = 0;// test pawn :)
	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		if (stadia_w > rubikon)
		{
			
			if ((from_black_king == G8) || (from_black_king == H8))
			{// если сделали короткую рокировку
				eval = eval - pozicion_pawn_king_in_oo_black[from];
			}
			else if ((from_black_king == A8) || (from_black_king == B8) 
			|| (from_black_king == C8))
			{// если сделали длинную рокировку
				eval = eval - pozicion_pawn_king_in_ooo_black[from];
			}
			else
			{// если рокировки не получилось
				eval = eval - pozicion_pawn_king_in_center_black[from];
			}
		}
		else
		{
			eval = eval - pozicion_pawn_black_end[from];
		}


		// определяем проходные
		// в случае если перед пешкой нет вражеской пешки
		if (!(e_rook_down_masks[from] & p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN]))
		{ 
			eval = eval - PERSPECTIVE_PAWNN;
			cnt_line_pawn_black = 0;

			// для случая крайней проходной
			if (((from % 8) == H1) || ((from % 8) == A1)) cnt_line_pawn_black = 1;

			// в случае если справо перед пешкой нет вражеской пешки
			if ((from % 8) != H1)
			{
				if (!(e_rook_down_masks[from + 1] 
				& p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN]))
				{
					eval = eval - PERSPECTIVE_PAWNN;
					cnt_line_pawn_black = cnt_line_pawn_black + 1;
				}
			}

			// в случае если слево перед пешкой нет вражеской пешки
			if ((from % 8) != A1)
			{
				if (!(e_rook_down_masks[from - 1] 
				& p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN]))
				{
					eval = eval - PERSPECTIVE_PAWNN;
					cnt_line_pawn_black = cnt_line_pawn_black + 1;
				}
			}

			// все три линии открыты это чистая проходная.
			if (cnt_line_pawn_black == 2) 
				eval = eval - pozicion_perspective_pawn_black_end[from];
		}

		// если есть сдвоенные пешки то в массиве будут двойки если строенные то тройки за них штраф
		Structure_pawn_b[from % 8] = Structure_pawn_b[from % 8] + 1;
		//std::cout << "b from % 8= "<< from % 8 << "\n";
	}

	// штрафуем за сдвоенные пешки
	for (int ii = 0; ii < 8; ii++)
	{
		if (Structure_pawn_w[ii] > 1) eval = eval - PENALTY_PAWNN; // сейчас штраф 5 был 20

		if (Structure_pawn_b[ii] > 1) eval = eval + PENALTY_PAWNN;

		//std::cout << "ii = "<< ii << "\n";
		//std::cout << "Structure_pawn_w[ii]= "<< Structure_pawn_w[ii] << "\n";
		//std::cout << "Structure_pawn_b[ii]= "<< Structure_pawn_b[ii] << "\n";
	}

}

//	===================================================================
// обсчет бонуса за расположение ферзей, ладей, слонов, коней
// обсчет контролируемого пространства этими фигурами
// обсчет атаки полей вокруг королей этими фигурами
inline void EVALUATE::PST
(
	const T_Bitboard * p_bitboard,// представление доски
	const unsigned __int8 from_white_king,// расположение белого короля
	const unsigned __int8 from_black_king,// расположение черного короля
	unsigned __int8 & at_white_king, // количество атаковнных полей вокруг белого короля
	unsigned __int8 & at_black_king, // количество атаковнных полей вокруг черного короля
	__int32 & eval //статическая оценка
)
{
	unsigned __int8 from; // координата поля откуда фигура ходит
	unsigned __int64 from_mask = 0; // маска расположения фигур одного типа

	// для вычисления расстояния между фигурами
	//int dx;// дистанция по горизонтали
	//int dy;// дистанция по вертикали
	//int distanse;// вектор расстояния
	//const int kd = 7;// коэффициент дистанции

	/*
	ладьям и слонам бонус за захват пространства причем бонус несколько
	странный(зато быстрее считается :)) он включает и занятые поля

	кони стремятся к центру
	*/

	// ферзь-----------------------------------------------------------------
	//from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN];
	//
	//while (from_mask != 0)
	//{
	//	from = bit_scan_forward_win32(from_mask);
	//	from_mask = from_mask & (from_mask - 1);

	//	//dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
	//	//dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
	//	//distanse = kd/(dx + dy);
	//	//distanse = 0;

	//	eval = eval + CONTROL_SQUARE
	//	* queen_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//		from, from_black_king, at_black_king);
	//}

	//ладьи----------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		eval = eval + CONTROL_SQUARE 
		* rook_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, from, 
				from_black_king, at_black_king);

		// cout << "Rook_amoun_white(p_bitboard)= "<< Rook_amoun(p_bitboard, from) << "\n";
	}

	//слон---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval + pozicion_bishop_white[from] 
			//+ CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
				//from, from_black_king, at_black_king)
					;

		//cout << "Bishop_amoun(p_bitboard, from)= "<< Bishop_amoun(p_bitboard, from) << "\n";
	}

	//конь ----------------------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask - 1);

		eval = eval + pozicion_knigh_white[from] 
			//+ CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
				//from, from_black_king, at_black_king)
					;
	}

/////////////////////////////////////

	//ферзь-----------------------------------------------------------------
	//from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN];

	//while (from_mask != 0)
	//{
	//	from = bit_scan_forward_win32(from_mask);
	//	from_mask = from_mask & (from_mask-1);

	//	//dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
	//	//dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
	//	//distanse = kd/(dx + dy);
	//	//distanse = 0;

	//	eval = eval - CONTROL_SQUARE 
	//		* queen_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
	//			from_white_king, at_white_king);

	//}

	//ладьи----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval - CONTROL_SQUARE 
			* rook_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
				from_white_king, at_white_king);

	}

	//слон---------------------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval - pozicion_bishop_black[from] 
			//- CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece,p_bitboard->all_black_piece, 
				//from, from_white_king, at_white_king)
					;

	}

	//конь ----------------------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from = bit_scan_forward_win32(from_mask);
		from_mask = from_mask & (from_mask-1);

		eval = eval - pozicion_knigh_black[from] 
			//- CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, 
				//from, from_white_king, at_white_king)
					;

	}

	// бонус за атаку короля
	//eval = eval + ATAK_KING * (at_black_king - at_white_king);


}

//	===================================================================
// штраф за разрушение щита перед королем. 
// если нет прикрытия пешкой то назначается штраф 
// (раньше было прикрытие любой фигурой, но это очень глупо :))
inline __int32 EVALUATE::king_shield
(
	unsigned __int8 king_from,// положение короля (from)
	unsigned __int8 king_color,// цвет короля
	const T_Bitboard * p_bitboard,// представление доски
	const unsigned __int8 SHIELD// бонус за прикрытие короля
)
{
	unsigned __int8 value_king_shield = 0;

	if (king_color == 1)
	{
		// смотрим на клетку вверх от короля
		if (p_bitboard->white_name_from_coord[king_from + 7] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 0)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}

		// смотрим на клетку влево вверх
		if (p_bitboard->white_name_from_coord[king_from + 8] != BITBOARDGENERATOR::N_PAWN) 
		{
			value_king_shield = value_king_shield + SHIELD;
		}

		// смотрим на клетку вправо вверх
		if (p_bitboard->white_name_from_coord[king_from + 9] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 7)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}
	}
	else
	{
		// смотрим на клетку вниз от короля
		if (p_bitboard->black_name_from_coord[king_from - 7] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 63)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}

		// смотрим на клетку  влево вниз
		if (p_bitboard->black_name_from_coord[king_from - 8] != BITBOARDGENERATOR::N_PAWN)
		{
			value_king_shield = value_king_shield + SHIELD;
		}

		// смотрим на клетку вправо вниз
		if (p_bitboard->black_name_from_coord[king_from - 9] != BITBOARDGENERATOR::N_PAWN)
		{
			if (king_from != 56)
			{
				value_king_shield = value_king_shield + SHIELD;
			}
		}
	}

	return value_king_shield;
}

//	===================================================================
// функция получает позицию и оценивает разницу материала. белые - черные
__int32 EVALUATE::public_eval_difference_material
(
	const T_Bitboard * p_bitboard// представление доски
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	__int32 value_difference_material = 0;

	// белые плюсуются

	// король----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_difference_material = value_difference_material + KING;

	// ферзь-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + QUEEN;
	}

	// ладья-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + ROOK;
	}

	// слон------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + BISHOP;
	}

	// конь -------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + KNIGHT;
	}

	// пешка--------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0){
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material + PAWN;
	}

	///////////////////////////////////

	// черные минусуются(отнимаются)

	// король----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_difference_material = value_difference_material - KING;

	// ферзь-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - QUEEN;
	}

	// ладья-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - ROOK;
	}

	// слон------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - BISHOP;
	}

	// конь -------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - KNIGHT;
	}

	// пешка--------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_difference_material = value_difference_material - PAWN;
	}

	//--------------------------------------------------------------------------
	// это другой вариант простой оценки. он проще но все же медленнее
	//int eval = 0;
	//
	//for (int i = 0; i < 64; i++)
	//{
	//	eval = eval + kod[p_bitboard->white_name_from_coord[i]];
	//	eval = eval - kod[p_bitboard->black_name_from_coord[i]];
	//}
	//
	//if (eval != value_difference_material)
	//{
	//	cout << "KARAUUUUUUUUUUUUUUL"<< "\n";
	//}
	//--------------------------------------------------------------------------

	return value_difference_material;
}

//	===================================================================
// функция получает позицию и оценивает материал для белых
__int32 EVALUATE::public_eval_white_material
(
	const T_Bitboard * p_bitboard// представление доски
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	__int32 value_white_material = 0;

	// белые плюсуются

	// король----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_white_material = value_white_material + KING;

	// ферзь-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		// надо пояснить что конструкция типа b = b & (b-1)  сбрасывает самый правый единичный бит 
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + QUEEN;
	}

	// ладья-----------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + ROOK;
	}

	// слон------------------------------------------------------------------- 
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + BISHOP;
	}

	// конь -------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + KNIGHT;
	}

	// пешка--------------------------------------------------------------------
	from_mask = p_bitboard->white_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0){
		from_mask = from_mask & (from_mask - 1);
		value_white_material = value_white_material + PAWN;
	}

	return value_white_material;
}

//	===================================================================
// функция получает позицию и оценивает материал для черных.
__int32 EVALUATE::public_eval_black_material
(
	const T_Bitboard * p_bitboard// представление доски
)
{
	unsigned __int64 from_mask; // маска расположения фигур одного типа
	__int32 value_black_material = 0;

	// черные плюсуются тоже

	// король----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KING];

	if (from_mask != 0) value_black_material = value_black_material + KING;

	// ферзь-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_QUEEN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + QUEEN;
	}

	// ладья-----------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_ROOK];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + ROOK;
	}

	// слон------------------------------------------------------------------- 
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_BISHOP];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + BISHOP;
	}

	// конь -------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_KNIGHT];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + KNIGHT;
	}

	// пешка--------------------------------------------------------------------
	from_mask = p_bitboard->black_piece[BITBOARDGENERATOR::N_PAWN];

	while (from_mask != 0)
	{
		from_mask = from_mask & (from_mask - 1);
		value_black_material = value_black_material + PAWN;
	}

	return value_black_material;
}


//	===================================================================
// просчитываем все возможные ходы ладьи. возвращаем количество ходов
// и количество атакованных полей вокруг короля
inline unsigned __int8 EVALUATE::rook_amoun
(
	unsigned __int64  all_piece,// все фигуры
	unsigned __int64  piece,// свои фигуры
	unsigned __int8 from,// расположение ферзя
	unsigned __int8 from_king,// расположение короля
	unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
)
{
	unsigned __int64 blocking_square_mask = 0; // маска блокированных полей
	unsigned __int64 move_mask = 0; // маска ходов
	unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

	__int32 blocking_coordinate = 0; // координата блокированного поля 
	unsigned __int8 number = 0; // 
	unsigned __int8 number_n = 0; // 

	// ходы ладьи----------------------------------------------------------------------------------

	// смотрим в четырех различных направлениях

	// луч вверх----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_up_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------

	// луч влево----------------------------------------------------
	//blocking_square_mask = all_piece & e_rook_left_masks[from];
	//
	//if (blocking_square_mask != 0)
	//{ 
	//	blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
	//	move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
	//}
	//else
	//{
	//	move_mask = e_rook_left_masks[from];
	//}

	//move_mask_sum = move_mask_sum | move_mask;
	//--------------------------------------------------------------

	// луч вниз-----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_down_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;
	//--------------------------------------------------------------


	// луч вправо---------------------------------------------------
	//blocking_square_mask = all_piece & e_rook_right_masks[from];

	//if (blocking_square_mask != 0)
	//{ 
	//	blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
	//	move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
	//}
	//else
	//{
	//	move_mask = e_rook_right_masks[from];
	//}

	//move_mask_sum = move_mask_sum | move_mask;
	//--------------------------------------------------------------

	//if ((e_king_moves_masks[from_king] & move_mask_sum) != 0) at_cnt = at_cnt + 1 ;

	// убираем взятия своих фигур
	move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

	// Заполняем список ходов
	while (move_mask_sum != 0)
	{
		move_mask_sum = move_mask_sum & (move_mask_sum - 1);
		number_n = number_n + 1;
	}

	//	number_n = popcnt(move_mask_sum);

		//std::cout << "from= " << (int)from << "\n";
		//std::cout << "popcnt(move_mask_sum)= " << popcnt(move_mask_sum) << "\n";
		//std::cout << "number_n= " << (int)number_n << "\n";


	return number_n;

}

//	===================================================================
// просчитываем все возможные ходы слона. возвращаем количество ходов
// и количество атакованных полей вокруг короля
inline unsigned __int8 EVALUATE::bishop_amoun
(
	unsigned __int64  all_piece,// все фигуры
	unsigned __int64  piece,// свои фигуры
	unsigned __int8 from,// расположение ферзя
	unsigned __int8 from_king,// расположение короля
	unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
)
{
	unsigned __int64 blocking_square_mask=0; // маска блокированных полей
	unsigned __int64 move_mask = 0; // маска ходов
	unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

	__int32 blocking_coordinate = 0; // координата блокированного поля 
	unsigned __int8 number = 0; // координата поля откуда фигура ходит
	unsigned __int8 number_n = 0; // координата поля откуда фигура ходит

	// ходы слона---------------------------------------------------------------------------------- 

	// смотрим в четырех различных направлениях

	// луч вверх - вправо
	blocking_square_mask = all_piece & e_bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// луч вверх - влево
	blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// луч вниз - влево
	blocking_square_mask = all_piece & e_bishop_down_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// луч вниз - вправо
	blocking_square_mask = all_piece & e_bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	if ((e_king_moves_masks[from_king] & move_mask_sum) != 0) 
		at_cnt = at_cnt + 1 ;

	// убираем взятия своих фигур
	move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

	// Заполняем список ходов
	while (move_mask_sum != 0)
	{
		move_mask_sum = move_mask_sum & (move_mask_sum - 1);
		number_n = number_n + 1;
	}

	return number_n;

}

//	===================================================================
// просчитываем все возможные ходы ферзя. возвращаем количество ходов
// и количество атакованных полей вокруг короля
inline unsigned __int8 EVALUATE::queen_amoun
(
	unsigned __int64  all_piece,// все фигуры
	unsigned __int64  piece,// свои фигуры
	unsigned __int8 from,// расположение ферзя
	unsigned __int8 from_king,// расположение короля
	unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
)
{
	unsigned __int64 blocking_square_mask=0; // маска блокированных полей
	unsigned __int64 move_mask = 0; // маска ходов
	unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

	__int32 blocking_coordinate = 0; // координата блокированного поля 
	unsigned __int8 number = 0; // координата поля откуда фигура ходит
	unsigned __int8 number_n = 0; // координата поля откуда фигура ходит

	// ходы ладьи----------------------------------------------------------------------------------

	// смотрим в четырех различных направлениях

	// луч вверх----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_up_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_up_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// луч влево----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// луч вниз-----------------------------------------------------
	blocking_square_mask = all_piece & e_rook_down_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_down_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// луч вправо---------------------------------------------------
	blocking_square_mask = all_piece & e_rook_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_rook_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;


	// ходы слона---------------------------------------------------------------------------------- 

	// смотрим в четырех различных направлениях

	// луч вверх - вправо
	blocking_square_mask = all_piece & e_bishop_up_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_right_masks[from];
	}
 
	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// луч вверх - влево
	blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_forward_win32(blocking_square_mask);
		move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_up_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// луч вниз - влево
	blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_left_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

	//--------------------------------------------------------------
	// луч вниз - вправо
	blocking_square_mask = all_piece & e_bishop_down_right_masks[from];

	if (blocking_square_mask != 0)
	{ 
		blocking_coordinate = bit_scan_reverse_win32(blocking_square_mask);
		move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
	}
	else
	{
		move_mask = e_bishop_down_right_masks[from];
	}

	move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------
	if ((e_king_moves_masks[from_king] & move_mask_sum) != 0) at_cnt = at_cnt + 1 ;

	// убираем взятия своих фигур
	move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

	// Заполняем список ходов
	while (move_mask_sum != 0)
	{
		move_mask_sum = move_mask_sum & (move_mask_sum - 1);
		number_n = number_n + 1;
	}

	//if (number_n != number) cout << "ЗАСАДА!!!!!!!!!! " << "\n";

	return number_n;

}


//	===================================================================
// просчитываем все возможные ходы коня. возвращаем количество ходов
// и количество атакованных полей вокруг короля
inline unsigned __int8 EVALUATE::knigt_amoun
(
	unsigned __int64  all_piece,// все фигуры
	unsigned __int64  piece,// свои фигуры
	unsigned __int8 from,// расположение ферзя
	unsigned __int8 from_king,// расположение короля
	unsigned __int8 & at_cnt// количество атакованных полей вокруг короля(квадрат ходов короля)
)
{
	unsigned __int64 blocking_square_mask=0; // маска блокированных полей
	unsigned __int64 move_mask = 0; // маска ходов
	unsigned __int64 move_mask_sum = 0; // маска ходов всех направлений

	__int32 blocking_coordinate = 0; // координата блокированного поля 
	unsigned __int8 number = 0; // координата поля откуда фигура ходит
	unsigned __int8 number_n = 0; // координата поля откуда фигура ходит

	// находим ходы на свои фигуры
	// а потом убираем их из всех ходов
	move_mask = e_knigt_moves_masks[from] ^ (e_knigt_moves_masks[from] & piece) ;

	if ((e_king_moves_masks[from_king] & move_mask) != 0 ) at_cnt = at_cnt + 1 ;

	// Заполняем список ходов---------------------------------------
	while (move_mask != 0)
	{
		move_mask = move_mask & (move_mask-1);
		number = number + 1;
	}

	return number;
}


//	===================================================================
// ищем первый установленный бит(т.е. бежим справа)
__inline __int32 EVALUATE::bit_scan_forward_win32
(
	unsigned __int64 analyzed_number// число в котором ищем взведенный бит
)
{ 
	_asm
	{ 
		/*
		из учебника по ассемблеру(This document saved from http://mind.telenet.ru/samouch/7/31/index.html):
		Команда mov замещает первый операнд (приемник) вторым (источником). 
		При этом исходное значение первого операнда теряется. 
		Второй операнд не изменяется. В зависимости от описания операндов, 
		пересылается слово или байт. Если операнды описаны по-разному или 
		режим адресации не позволяет однозначно определить размер операнда, 
		для уточнения размера передаваемых данных в команду следует включить один 
		из атрибутных операторов byte ptr или word ptr. 
		Команда не воздействует на флаги процессора. 
		Часто возникает необходимость выполнить обратную операцию - 
		к паре байтов обратиться, как к слову. Для этого надо использовать оператор word ptr:
		*/
		mov  eax, dword ptr analyzed_number[0]

		/*
		Команда test выполняет операцию логического умножения 
		И над двумя операндами и, в зависимости от результата, 
		устанавливает флаги SF, ZF и PF. Флаги OF и CF сбрасываются, 
		a AF имеет неопределенное значение. Состояние флагов можно затем 
		проанализировать командами условных переходов. Команда test не изменяет 
		ни один из операндов. В качестве первого операнда команды test можно 
		указывать регистр (кроме сегментного) или ячейку памяти, в качестве 
		второго - регистр (кроме сегментного), ячейку памяти или непосредственное 
		значение, однако не допускается определять оба операнда одновременно как 
		ячейки памяти. Операнды могут быть байтами или словами и представлять числа 
		со знаком или без знака. 

		Правила побитового умножения:

		Первый операнд-бит 0101
		Второй операнд-бит 0011
		Бит результата 0001

		Флаг SF устанавливается в 1, если в результате выполнения команды 
		образовалось число с установленным знаковым битом.
		Флаг ZF устанавливается в 1, если в результате выполнения команды 
		образовалось число, состоящее из одних двоичных нулей.
		Флаг PF устанавливается в 1, если в результате выполнения команды 
		образовалось число с четным количеством двоичных единиц в его битах.
		*/
		test  eax, eax

		/*
		Команда Перейти, если Условие перехода
		jz нуль ZF= 1
		*/
		jz  grown_up

		/*
		Команда bsf сканирует слово или двойное слово в поисках бита, равного 1. 
		Сканирование выполняется от младшего бита (0) к старшему. Если в слове 
		не найдено установленных битов, то устанавливается флаг ZF. 
		Если установленные биты есть, то номер первого установленного бита 
		заносится в указанный в команде регистр. Номером бита считается его 
		позиция в слове, отсчитываемая от бита 0. В качестве первого операнда команды bsf 
		следует указывать регистр, куда будет помещен результат сканирования, в 
		качестве второго - регистр или ячейку памяти со сканируемым словом. 
		В команде bsf допустимо использование как 16-битовых, так и 32-битовых операндов 
		(но и первый, и второй операнды должны быть одного типа).
		*/
		bsf  eax, eax

		/*
		Команда jmp передает управление в указанную точку того же или другого программного 
		сегмента. Адрес возврата не сохраняется. Команда не воздействует на флаги процессора.
		*/
		jmp  jump_end
		grown_up:  bsf  eax, dword ptr analyzed_number[4]

		/*
		сложение
		*/
		add  eax, 20h
		jump_end:
	}
}

//	===================================================================
// ищем последний установленный бит(т.е. бежим слева)
__inline __int32 EVALUATE::bit_scan_reverse_win32
(
	unsigned __int64 analyzed_number// число в котором ищем взведенный бит
)
{ 
	_asm 
	{ 
		mov  eax, dword ptr analyzed_number[4]
		test  eax, eax
		jz  younger

		/*
		Команда bsr сканирует слою или двойное слово в поисках бита, равного 1. 
		Сканирование выполняется от старшего бита (15 или 31) к младшему. 
		Если в слове не найдено установленных битов, то устанавливается флаг ZF. 
		Если установленные биты есть, то номер первого установленного бита заносится 
		в указанный в команде регистр. Номером бита считается его позиция в слове, 
		отсчитываемая от бита 0. В качестве первого операнда команды bsr следует 
		указывать регистр, куда будет помещен результат сканирования, в качестве 
		второго - регистр или ячейку памяти со сканируемым словом. 
		В команде bsr допустимо использование как 16-битовых, так и 32-битовых операндов, 
		но и первый, и второй операнды должны быть одного типа (за исключением случая, 
		когда второй операнд - константа).
		*/
		bsr  eax, eax
		add  eax, 20h
		jmp  jump_end
		younger:  bsr  eax, dword ptr analyzed_number[0]
		jump_end:
	}
}


//	===================================================================
// считаем взведенные поля
//inline __int32 EVALUATE::popcnt(unsigned __int64 analyzed_number)
//{ 
//	_asm
//	{
//		mov  ecx, dword ptr analyzed_number[4]
//		xor  eax, eax      
//		test  ecx, ecx
//		jz  label_2
//
//		label_1: 
//			lea  ebx, [ecx-1]
//			inc  eax
//			and  ecx, ebx
//			jnz  label_1
//
//			label_2:
//				mov  ecx, dword ptr analyzed_number[0]
//				test  ecx, ecx
//				jz  label_4
//
//				label_3:
//					lea  ebx, [ecx-1]
//					inc  eax     
//					and  ecx, ebx
//					jnz  label_3
//					label_4:
//	}
//}

