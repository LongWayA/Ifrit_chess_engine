#ifndef A_ALL_CONSTANTS_H
#define A_ALL_CONSTANTS_H

//--------------------------------------------------------------------------
/* +
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 27.11.2010
*/
//--------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 90%

  коллекция почти всех именованных констант используемых в программе

  кроме тех что заданы в файле a_List_surplus_moves_const.h,  h_Evaluate_const.h


*/


//------------------------------------------------------------------------------
// используем в g_Search.cpp g_Search_root.cpp

	// NULL MOVE
	const unsigned __int8 NULL_YES = 1; // мы внутри нулевого хода 
	const unsigned __int8 NULL_NO = 0; // мы снаружи нулевого хода 
	const unsigned __int8 NULL_DEPTH = 2; // дистанция до дна меньше которой NULL не вызывается
	const unsigned __int8 SWITCH_NULL_REDUCTION_1_2 = 5; // глубина на которой меняется редукция нулевого хода 
										// с NULL_REDUCTION_ONE на NULL_REDUCTION_TWO
	const unsigned __int8 NULL_REDUCTION = 4; // 4 сокращение глубины перебора для NULL
	const unsigned __int8 NULL_REDUCTION_ONE = 4; // 4 сокращение глубины перебора для динамического NULL
	const unsigned __int8 NULL_REDUCTION_TWO = 3; // 3 сокращение глубины перебора для динамического NULL
	const unsigned __int8 VERIFICATOIN_SEARCH_REDUCTION = 5; // сокращение глубины перебора 
											//для поиска по обнаружению цугцванга
	// LMR
	const unsigned __int8 LMR_DEPTH = 3; // дистанция до дна меньше которой LMR не вызывается
	const unsigned __int8 LMR_REDUCTION_D = 1; // сокращение глубины перебора для LMR
	const unsigned __int8 FULL_SEARCH = 2;  // было 2
											// число веток с полным поиском без сокращений 
											// ход из хеш-таблицы, лучшее взятие,
											// два киллера, все взятия не редуцируются
											// хеш-ход два киллера это 3 и минус один потому что с нуля 
											// надо еще помнить что условие редукции i > lmr_reduction

	// FUTILITY
	const __int32 FUTILITY_DEPTH = 5; //4
	const __int32 futility_margin[] = {130, 130, 300, 300, 300, 300, 300, 300};

	// константы
	const unsigned __int8 CHECK_YES = 1; // есть шах
	const unsigned __int8 CHECK_NO = 0; // нет шаха


	const unsigned __int8 MOVE_IS_CAPTURE = 2; // ход это взятие
	const unsigned __int8 CAPTURE_NO = 0; // нет взятий

	const unsigned __int8 PV_YES = 1; // основной вариант
	const unsigned __int8 PV_NO = 0; // 


//------------------------------------------------------------------------------
// используем в g_Search_quiescence.cpp g_Search.cpp g_Search_root.cpp
	// 0 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка

	const  __int32 VALUE_MATE = 1000000; // оценка мата
	const  __int16 DELTA_MATE = 1000; // дельта для нахождения скорейшего мата


//------------------------------------------------------------------------------
// используем в a_Chess_bitboard_delta_pruning.cpp

	// 0 - король 1200 - ферзь 600 - ладья 400 - слон  400 - конь 100 - пешка
	const unsigned __int16 ROOK_MARGIN = 700; // отсчка взятий ладьей
	const unsigned __int16 BISHOP_KNIGHT_MARGIN = 500; // отсчка взятий  слонов и коней
	const unsigned __int8 PAWN_MARGIN = 160; // отсчка взятий  пешек


//------------------------------------------------------------------------------
// используем в j_Hash.h
	// лучше наверное перечислением сделать
	const unsigned __int8 EXACT = 1; // точный ход
	const unsigned __int8 LOWER = 2; // отсечка по бете(альфе)
	const unsigned __int8 UPPER = 3; // альфу(бету) улучшить не удалось


//------------------------------------------------------------------------------
// используем в a_Structure.h

// unsigned __int8 : 0 to 255
//          __int8 : -128 to 127

// unsigned __int16 :  0 to 65535
//          __int16 : -32768 to 32767

// unsigned __int32 : 0 to 4294967295
//          __int32 : -2147483648 to 2147483647

// unsigned __int64 : 0 to 
//          __int64 : - to 

const unsigned __int16 LENGTH_STRING = 65534; // задаем длину буфера потока

// + 1 - потому что нулевой индекс массива тоже используем 
// в результате получаем: 
//количество индексов = максимально доступный индекс + 1

// максимум на имя фигур 6 + 1
const unsigned __int8 MAX_NAME = 7;

// максимум координаты фигуры 63 + 1
const unsigned __int8 MAX_COORDINATE = 64;

// максимально поддерживаемая длина варианта и она же глубина поиска 100 + 1
const unsigned __int8 MAX_DEPTH = 101;

// максимально поддерживаемое количество ходов из позиции
const unsigned __int16 MAX_MOVE = 256;

// доска
const unsigned __int8 A8=56, B8=57, C8=58, D8=59, E8=60, F8=61, G8=62, H8=63;
const unsigned __int8 A7=48, B7=49, C7=50, D7=51, E7=52, F7=53, G7=54, H7=55;
const unsigned __int8 A6=40, B6=41, C6=42, D6=43, E6=44, F6=45, G6=46, H6=47;
const unsigned __int8 A5=32, B5=33, C5=34, D5=35, E5=36, F5=37, G5=38, H5=39;
const unsigned __int8 A4=24, B4=25, C4=26, D4=27, E4=28, F4=29, G4=30, H4=31;
const unsigned __int8 A3=16, B3=17, C3=18, D3=19, E3=20, F3=21, G3=22, H3=23;
const unsigned __int8 A2=8,  B2=9,  C2=10, D2=11, E2=12, F2=13, G2=14, H2=15;
const unsigned __int8 A1=0,  B1=1,  C1=2,  D1=3,  E1=4,  F1=5,  G1=6,  H1=7;


#endif // !defined A_ALL_CONSTANTS_H






