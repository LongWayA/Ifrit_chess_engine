#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * перебор позиций
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 12.4.2008
*/
//---------------------------------------------------------------------------
// Задаем различные режимы компилаяции полного перебора. Здесь мы можем легко
// включить отключить любой метод
#define COMPILING_1 0;// включаем-выключаем(1-0) режим чистого полного перебора (режим 1)
#define FORCED 1;// включаем-выключаем(1-0) режим форсировки на взятиях
#define EXTENSION_CHEK 1;// включаем-выключаем(1-0) продление на шахах
#define EXTENSION_CAPTURES 0;// включаем-выключаем(1-0) продление на взятиях
//----------------------------------
#define ALPHA_BETA_ON 1; // включаем-выключаем(1-0) режим альфа бета отсечения
// нижеследующие три метода дают эффект только при включенном альфа-бета
#define KILLER 1;// включаем-выключаем(1-0) киллеров
#define HASH 1;// включаем-выключаем(1-0) хеш таблицу
//----------------------------------

#define NEGA_SCOUT 1;// включаем-выключаем(1-0) режим негаскаута
#define NULL_MOVE 1;// включаем-выключаем(1-0) режим нулевого хода

#define TEST_L 0; // включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_PRINT 0; // включаем-выключаем(1-0) режим вывода при игре тестовой информации
                      // в графическую оболочку т.е. либо лучшая линия либо все линии первого ур.

//---------------------------------------------------------------------------
// максимальная глубина,глубина перебора,доска,
int White_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
// максимальная глубина,глубина перебора,доска,
int Black_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
//        доска,                список избыточных ходов,                  альфа,    бета,    максимальная глубина, глубина перебора, счетчик узлов,цепочка ходов при спуске в глубину, лучший вариант, тип хода, ничья повтором, флаг выхода, флаг нулевого хода
int White(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
//        доска,                список избыточных ходов,                  альфа,    бета,    максимальная глубина, глубина перебора, счетчик узлов,цепочка ходов при спуске в глубину, лучший вариант, тип хода, ничья повтором, флаг выхода, флаг нулевого хода
int Black(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br,int null_m);

//---------------------------------------------------------------------------
// если нет легальных ходов то это мат или пат
inline int No_moves_white(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current);

//---------------------------------------------------------------------------
// если нет легальных ходов то это мат или пат
inline int No_moves_black(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current);

//---------------------------------------------------------------------------
// производим расчеты на максимальной глубине
inline int Max_depth_white(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int description_move,int & br);

//---------------------------------------------------------------------------
// производим расчеты на максимальной глубине
inline int Max_depth_black(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int description_move,int & br);

//---------------------------------------------------------------------------
// выводим вариант который обдумываем
void Print_cout(int colour_move,T_Bitboard * bitboard,const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value,double  time_nodes);

void Print_cout_v(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// печатаем список
// глубина перебора,список
#if TEST_L
void List_816_Print(int Depth,const T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// продление на шахах. для белых
void Extension_for_chek_white(T_Bitboard * bitboard,int depth_max,int & depth_max_new);

//---------------------------------------------------------------------------
// продление на шахах. для черных
void Extension_for_chek_black(T_Bitboard * bitboard,int depth_max,int & depth_max_new);

//---------------------------------------------------------------------------
// реализуем продление на взятиях
void Extension_for_captures(int i,int start,int description_move,int description_move_up,int depth_max,int & depth_max_new,int depth);
