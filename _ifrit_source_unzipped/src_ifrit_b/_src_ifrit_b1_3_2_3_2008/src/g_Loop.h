#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * перебор позиций
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 30.12.2007
*/
//---------------------------------------------------------------------------
// Задаем различные режимы компилаяции полного перебора. Здесь мы можем легко
// включить отключить любой метод
#define COMPILING_1 0;// включаем-выключаем(1-0) режим чистого полного перебора (режим 1)
#define FORCED 1;// включаем-выключаем(1-0) режим форсировки на взятиях
#define EXTENSION_CHEK 1;// включаем-выключаем(1-0) продление на шахах
//----------------------------------
#define ALPHA_BETA_ON 1; // включаем-выключаем(1-0) режим альфа бета отсечения
// нижеследующие три метода дают эффект только при включенном альфа-бета
#define KILLER 1;// включаем-выключаем(1-0) киллеров
#define HASH 1;// включаем-выключаем(1-0) хеш таблицу
//----------------------------------

#define NEGA_SCOUT 0;// включаем-выключаем(1-0) режим негаскаута
#define NULL_MOVE 1;// включаем-выключаем(1-0) режим нулевого хода
// нулевой ход работает только при включенном продление на шахах

#define TEST_L 0; // включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_PRINT 0; // включаем-выключаем(1-0) режим вывода при игре тестовой информации
                      // в графическую оболочку т.е. либо лучшая линия либо все линии первого ур.

//---------------------------------------------------------------------------
// максимальная глубина,глубина перебора,доска,
int Alpha_beta_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,int description_move,T_PV * three_moves,int & br,int for_chek,int null_m);

//---------------------------------------------------------------------------
// максимальная глубина,глубина перебора,доска,
int Alpha_beta_816(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,int description_move,T_PV * three_moves,int & br,int for_chek,int null_m);

//---------------------------------------------------------------------------
// если нет легальных ходов то это мат или пат
int No_moves(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current);

//---------------------------------------------------------------------------
// производим расчеты на максимальной глубине
int Max_depth(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int description_move,int & br,int for_chek);

//---------------------------------------------------------------------------
// выводим вариант который обдумываем
void Print_cout(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value,double  time_nodes);

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
 int Null_move(T_Bitboard * bitboard,int depth,int depth_max,int null_m,int description_move,int for_chek,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int & br);

//---------------------------------------------------------------------------
void Extension_for_chek(T_Bitboard * bitboard,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z);
