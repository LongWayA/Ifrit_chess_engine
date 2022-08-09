#include "k_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 30.12.2007
*/
//---------------------------------------------------------------------------

/* COMPILING
   1 ������ �������
   2 ������ ������� + ������ + ����� ��������
   3 ������ ������� + ������ + ����� �������� + ������������� �������
   4 ������ ������� + ������ + ����� �������� + ������������� ������� + ���
   5 ������ ������� + ������ + ����� �������� + ������������� ������� + ��� + ������� ���
*/
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

#define NEGA_SCOUT 1;// ��������-���������(1-0) ����� ����������
#define NULL_MOVE 1;// ��������-���������(1-0) ����� �������� ����
// ������� ��� �������� ������ ��� ���������� ��������� �� �����

#define TEST_L 0; // ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_PRINT 0; // ��������-���������(1-0) ����� ������ ��� ���� �������� ����������
                      // � ����������� �������� �.�. ���� ������ ����� ���� ��� ����� ������� ��.

//---------------------------------------------------------------------------
// ������������ �������,������� ��������,�����,
int Alpha_beta_root(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,short int depth_max,short int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,short int description_move,T_PV * three_moves,short int & br,short int for_chek,short int null_m);

//---------------------------------------------------------------------------
// ������������ �������,������� ��������,�����,
int Alpha_beta_816(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,short int depth_max,short int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,short int description_move,T_PV * three_moves,short int & br,short int for_chek,short int null_m);

//---------------------------------------------------------------------------
// ���� ��� ��������� ����� �� ��� ��� ��� ���
int No_moves(T_Bitboard * bitboard,int depth,T_PV * pv_best,T_PV * pv_current);

//---------------------------------------------------------------------------
// ���������� ������� �� ������������ �������
int Max_depth(T_Bitboard * bitboard,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,short int depth_max,short int depth,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,short int description_move,short int & br,short int for_chek);

//---------------------------------------------------------------------------
// ������� ������� ������� ����������
void Print_cout(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,short int Depth_Max,int value,double  time_nodes);

void Print_cout_v(const T_list_surplus_moves * list_surplus_moves,T_PV PV_best_point,int Nodes_0, int Nodes,int i,short int Depth_Max,int value);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������,������
#if TEST_L
void List_816_Print(short int Depth,const T_list_surplus_moves * list_surplus_moves);
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
short int Null_move(T_Bitboard * bitboard,short int depth,short int depth_max,short int null_m,short int description_move,short int for_chek,T_list_surplus_moves * list_surplus_moves,int alpha,int beta,int & nodes,T_PV * pv_current,T_PV * pv_best,T_PV * three_moves,short int & br);

//---------------------------------------------------------------------------
void Extension_for_chek(T_Bitboard * bitboard,short int for_chek,short int depth_max,short int & for_chek_l,short int & depth_max_z);
