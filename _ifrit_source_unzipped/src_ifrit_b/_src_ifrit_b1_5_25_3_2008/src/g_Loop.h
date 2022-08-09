#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 30.12.2007
*/
//---------------------------------------------------------------------------
// ������ ��������� ������ ����������� ������� ��������. ����� �� ����� �����
// �������� ��������� ����� �����
#define COMPILING_1 0;// ��������-���������(1-0) ����� ������� ������� �������� (����� 1)
#define FORCED 1;// ��������-���������(1-0) ����� ���������� �� �������
#define EXTENSION_CHEK 1;// ��������-���������(1-0) ��������� �� �����
//----------------------------------
#define ALPHA_BETA_ON 1; // ��������-���������(1-0) ����� ����� ���� ���������
// ������������� ��� ������ ���� ������ ������ ��� ���������� �����-����
#define KILLER 1;// ��������-���������(1-0) ��������
#define HASH 1;// ��������-���������(1-0) ��� �������
//----------------------------------

#define NEGA_SCOUT 0;// ��������-���������(1-0) ����� ����������(�� ��������)

#define TEST_L 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_PRINT 0; // ��������-���������(1-0) ����� ������ ��� ���� �������� ����������
                      // � ����������� �������� �.�. ���� ������ ����� ���� ��� ����� ������� ��.

//---------------------------------------------------------------------------
// ������������ �������,������� ��������,�����,
int White_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int for_chek, int null_m);

//---------------------------------------------------------------------------
// ������������ �������,������� ��������,�����,
int Black_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int for_chek, int null_m);

//---------------------------------------------------------------------------
//                 �����,                ������ ���������� �����,                  �����,    ����,    ������������ �������, ������� ��������, ������� �����,������� ����� ��� ������ � �������, ������ �������, ��� ����, ����� ��������, ���� ������, ���� ����, ���� �������� ����
int White(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int for_chek, int null_m);

//---------------------------------------------------------------------------
//                 �����,                ������ ���������� �����,                  �����,    ����,    ������������ �������, ������� ��������, ������� �����,������� ����� ��� ������ � �������, ������ �������, ��� ����, ����� ��������, ���� ������, ���� ����, ���� �������� ����
int Black(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta, int depth_max, int depth,int & nodes,T_PV * pv_current,T_PV * pv_best, int description_move,T_PV * three_moves, int & br, int for_chek, int null_m);

//---------------------------------------------------------------------------
// ���� ��� ��������� ����� �� ��� ��� ��� ���
inline int No_moves_white(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current);

//---------------------------------------------------------------------------
// ���� ��� ��������� ����� �� ��� ��� ��� ���
inline int No_moves_black(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current);

//---------------------------------------------------------------------------
// ���������� ������� �� ������������ �������
inline int Max_depth_white(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int description_move,int & br,int for_chek);

//---------------------------------------------------------------------------
// ���������� ������� �� ������������ �������
inline int Max_depth_black(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int depth_max,int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,int description_move,int & br,int for_chek);

//---------------------------------------------------------------------------
// ������� ������� ������� ����������
void Print_cout(T_Bitboard * bitboard,const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value,double  time_nodes);

void Print_cout_v(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,int Depth_Max,int value);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������,������
#if TEST_L
void List_816_Print(int Depth,const T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void Extension_for_chek_white(T_Bitboard * bitboard,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z);

//---------------------------------------------------------------------------
void Extension_for_chek_black(T_Bitboard * bitboard,int for_chek,int depth_max,int & for_chek_l,int & depth_max_z);
