
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 18.08.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%


/// константы
namespace HASH_CONSTANTS
{

	const unsigned __int8 EXACT = 1; /// точный ход
	const unsigned __int8 LOWER = 2; /// отсечка по бете(альфе)
	const unsigned __int8 UPPER = 3; /// альфу(бету) улучшить не удалось

}

using namespace HASH_CONSTANTS;


/// задаем имя фигур
namespace LIST_NAME_PIECE_CONSTANTS
{
	const unsigned __int8 N_KING = 6;   /// король
	const unsigned __int8 N_QUEEN = 5;  /// ферзь
	const unsigned __int8 N_ROOK = 4;   /// ладья
	const unsigned __int8 N_BISHOP = 3; /// слон
	const unsigned __int8 N_KNIGHT = 2; /// конь
	const unsigned __int8 N_PAWN = 1;   /// пешка
}

using namespace LIST_NAME_PIECE_CONSTANTS;

/// поля шахматной доски
namespace POSITION_CONSTANTS_BOARD_SQUARES
{
	const unsigned __int8 A8=56, B8=57, C8=58, D8=59, E8=60, F8=61, G8=62, H8=63;
	const unsigned __int8 A7=48, B7=49, C7=50, D7=51, E7=52, F7=53, G7=54, H7=55;
	const unsigned __int8 A6=40, B6=41, C6=42, D6=43, E6=44, F6=45, G6=46, H6=47;
	const unsigned __int8 A5=32, B5=33, C5=34, D5=35, E5=36, F5=37, G5=38, H5=39;
	const unsigned __int8 A4=24, B4=25, C4=26, D4=27, E4=28, F4=29, G4=30, H4=31;
	const unsigned __int8 A3=16, B3=17, C3=18, D3=19, E3=20, F3=21, G3=22, H3=23;
	const unsigned __int8 A2=8,  B2=9,  C2=10, D2=11, E2=12, F2=13, G2=14, H2=15;
	const unsigned __int8 A1=0,  B1=1,  C1=2,  D1=3,  E1=4,  F1=5,  G1=6,  H1=7;
}

using namespace POSITION_CONSTANTS_BOARD_SQUARES;

