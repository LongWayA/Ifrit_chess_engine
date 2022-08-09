#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 12.4.2008
*/
//---------------------------------------------------------------------------
// ������ ��������� ������ ����������� ������� ��������. ����� �� ����� �����
// �������� ��������� ����� �����
#define SAVE_LINE 1;// ��������-���������(1-0) ����� ������ ��������
#define FORCED 1;// ��������-���������(1-0) ����� ���������� �� �������
#define EXTENSION_CHEK 1;// ��������-���������(1-0) ��������� �� �����
    #define EXTENSION_CAPTURES 0;// ��������-���������(1-0) ��������� �� �������
//----------------------------------
#define ALPHA_BETA_ON 1; // ��������-���������(1-0) ����� ����� ���� ���������
// ������������� ��� ������ ���� ������ ������ ��� ���������� �����-����
#define KILLER 1;// ��������-���������(1-0) ��������
#define HASH 1;// ��������-���������(1-0) ��� �������
//----------------------------------

#define NEGA_SCOUT 1;// ��������-���������(1-0) ����� ����������
#define NULL_MOVE 1;// ��������-���������(1-0) ����� �������� ����

#define TEST_L 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_PRINT 0; // ��������-���������(1-0) ����� ������ ��� ���� �������� ����������
                      // � ����������� �������� �.�. ���� ������ ����� ���� ��� ����� ������� ��.

//---------------------------------------------------------------------------
// ������������ �������,������� ��������,�����,
int White_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
// ������������ �������,������� ��������,�����,
int Black_root(int & from,int & to,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
//                  �����,                ������ ���������� �����,                  �����,    ����,    ������������ �������, ������� ��������, ������� �����,������� ����� ��� ������ � �������, ������ �������, ��� ����, ����� ��������, ���� ������, ���� �������� ����
int White(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br, int null_m);

//---------------------------------------------------------------------------
//                  �����,                ������ ���������� �����,                  �����,    ����,    ������������ �������, ������� ��������, ������� �����,������� ����� ��� ������ � �������, ������ �������, ��� ����, ����� ��������, ���� ������, ���� �������� ����
int Black(int check,struct T_Bitboard * bitboard,int alpha,int beta, int depth_max, int depth,int & nodes,struct T_PV * pv_current,T_PV * pv_best, int description_move,struct T_PV * three_moves, int & br,int null_m);

//---------------------------------------------------------------------------
// ���� ��� ��������� ����� �� ��� ��� ��� ���
inline int No_moves_white(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current);

//---------------------------------------------------------------------------
// ���� ��� ��������� ����� �� ��� ��� ��� ���
inline int No_moves_black(struct T_Bitboard * bitboard,int depth,struct T_PV * pv_best,struct T_PV * pv_current);

//---------------------------------------------------------------------------
// ���������� ������� �� ������������ �������
inline int Max_depth_white(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_PV * three_moves,int description_move,int & br);

//---------------------------------------------------------------------------
// ���������� ������� �� ������������ �������
inline int Max_depth_black(struct T_Bitboard * bitboard,int alpha,int beta,int depth_max,int depth,int & nodes,struct T_PV * pv_current,struct T_PV * pv_best,struct T_PV * three_moves,int description_move,int & br);

//---------------------------------------------------------------------------
// ������� ������� ������� ����������
void Print_cout(int colour_move,struct T_Bitboard * bitboard,const struct T_list_surplus_moves * list_surplus_moves,struct T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value,double  time_nodes);

void Print_cout_v(const struct T_list_surplus_moves * list_surplus_moves,struct T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������,������
#if TEST_L
void List_816_Print(int Depth,const T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ��������� �� �����. ��� �����
inline void Extension_for_chek_white(T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check);

//---------------------------------------------------------------------------
// ��������� �� �����. ��� ������
inline void Extension_for_chek_black(T_Bitboard * bitboard,int depth_max,int & depth_max_new,int & check);

//---------------------------------------------------------------------------
// ��������� ��������� �� �������
inline void Extension_for_captures(int i,int start,int description_move,int depth_max,int & depth_max_new,int depth);
