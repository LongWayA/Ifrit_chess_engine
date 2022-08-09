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
#define SAVE_LINE 1;// включаем-выключаем(1-0) режим записи варианта
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
int White_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
// максимальная глубина,глубина перебора,доска,
int Black_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
//                  доска,                список избыточных ходов,                  альфа,    бета,    максимальная глубина, глубина перебора, счетчик узлов,цепочка ходов при спуске в глубину, лучший вариант, тип хода, ничья повтором, флаг выхода, флаг нулевого хода
int White(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
//                  доска,                список избыточных ходов,                  альфа,    бета,    максимальная глубина, глубина перебора, счетчик узлов,цепочка ходов при спуске в глубину, лучший вариант, тип хода, ничья повтором, флаг выхода, флаг нулевого хода
int Black(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br,int null_m);

//---------------------------------------------------------------------------
// если нет легальных ходов то это мат или пат
inline int No_moves_white(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current);

//---------------------------------------------------------------------------
// если нет легальных ходов то это мат или пат
inline int No_moves_black(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current);

//---------------------------------------------------------------------------
// производим расчеты на максимальной глубине
inline int Max_depth_white(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_PV * three_moves,int description_move,int & br);

//---------------------------------------------------------------------------
// производим расчеты на максимальной глубине
inline int Max_depth_black(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_PV * three_moves,int description_move,int & br);

//---------------------------------------------------------------------------
// выводим вариант который обдумываем
void Print_cout(int colour_move,struct T_Bitboard * bitboard,const struct T_list_surplus_moves * list_surplus_moves,struct T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value,double  time_nodes);

void Print_cout_v(const struct T_list_surplus_moves * list_surplus_moves,struct T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value);

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
inline void Extension_for_chek_white(T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check);

//---------------------------------------------------------------------------
// продление на шахах. для черных
inline void Extension_for_chek_black(T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check);

//---------------------------------------------------------------------------
// реализуем продление на взятиях
inline void Extension_for_captures(int i,int start,int description_move,int depth_max,int & depth_max_new,int depth);
