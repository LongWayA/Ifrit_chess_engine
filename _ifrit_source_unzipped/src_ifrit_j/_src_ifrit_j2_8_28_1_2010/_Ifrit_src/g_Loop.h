#include "a_structure.h"
//-----------------------------------------------------------------------------------
/*
 * ������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 23.02.2009 20.12.2009
*/
//---------------------------------------------------------------------------

// ������ ��������� ������ ���������� ��������. ����� �� ����� �����
// �������� ��������� ����� �����
//switch
#define SAVE_LINE 1 // ��� ����������(0) �������� ������ ���������. ���� ������������� ������ ���������� ����� Ctrl + Alt + Delete
           // ������ ��� �������� ��������� �������� �� ��� � ������ c_Iterative_Deepening �.�. ��������� �������� && (flag_mate == 0)

#define FORCED 1 // ��������-���������(1-0) ����� ���������� �� �������

//----------------------------------
#define ALPHA_BETA_ON 1 // ��������-���������(1-0) ����� �����-���� ���������
// ������������� ��� ������ ���� ������ ������ ��� ���������� �����-����

// HASH -----------------------------------------------------------
#define HASH 1 // ��������-���������(1-0) ���-�������
#define HASH_SCORE 1 // ��������-���������(1-0) ������������� ������ �� ���-�������

#define HASH_TEST 0 // ��������-���������(1-0) ���������� ���������� ������ ������
                    // � ������ ������ ������������� ���� �� ��� �������
#define HASH_TEST_SCORE 0 // ��������-���������(1-0) ���������� ���������� ������ ������
                    // � ������ ������ ������������� ������ �� ��� �������.
                    // ����������� HASH_SCORE 1
#define HASH_TEST_KEY 0 // ��������-���������(1-0) �������� ���������� ���������
                        // � ��������� ��� - ����� (����� ����� ��� �������������� ��������)
// ----------------------------------------------------------------

#define KILLER 1 // ��������-���������(1-0) ��������

#define REPETITION 1 // ��������-���������(1-0) �������� �������� �������

#define EXTENSION_CHEK 1 // ��������-���������(1-0) ��������� �� �����

// ��������! ��� �������� ���� � futility ������ ���� EXTENSION_CHEK 1
#define NULL_MOVE 1 // ��������-���������(1-0) ����� �������� ����
#define LMR 1 // ��������-���������(1-0) ����� Late Move Reductions


// ������ ���� �������� ��� ��������.---------------------------------------------

// �� ����� �� ������ ��������� ���������� ������� ������ � ������� ��������
// ��������� ��������� ��� �������� ����� ���� �����.
 #define EXTENSION_CAPTURES 1 // ��������-���������(1-0) ��������� �� ��������




 #define FUTILITY_PRUNING 0 // ��������-���������(1-0) ����� futility pruning
                            // ������ � ������ ��������� �� ������������
                            // �������� ������ ������ ��� ���� ������������ �� ��������
//--------------------------------------------------------------------------------


#define TEST_L 0  // ��������-���������(1-0) ����� ������ � ���� �������� ����������
#define TEST_PRINT_ALL_LINE 0  // ��������-���������(1-0) ����� ������ ��� ���� �������� ����������
                      // � ����������� �������� �.�. ���� ������ ����� ���� ��� ����� ������� ������.


/*******************************************

 ����� FULLSEARCH
 �������� � ���� ������ � ������ ��������� ��������

 � ��� �����������:
 (��� �������� � ����� ������ �����)
 ����� � ��� �� ����� �� ������� ��� ���� ��� �� ������
 ������� �������� ������� ������� ��������� ������ ���� :)
 -------------------

 ALPHA - BETA
 �� ���� ��������� �������� ������� ��� �������� ��� ������ ������ :)
 ���� �������� �������� ���:

 ����� ���� � ������������ �������
 int AlphaBeta (pos, depth, alpha, beta) {
	if (depth == 0) return Evaluate(pos);
	best = -INFINITY;
	succ = Successors(pos);
	while (!Empty(succ)) {
		pos = RemoveOne(succ);
		value = -AlphaBeta(pos, depth - 1, -beta, -alpha);
		if (value > best) best = value;
	    if (value > alpha) alpha = value;
        if (alpha >= beta) return alpha;
	}
	return best;
 }
 --------------------------------------------------------------

 NULL MOVE
 ���� �� ��� �� ������ ���� ���� ����� ���� ��� �� ������ � �� ��� � ���� �� ������� �������
 � ���� �� �� � ������� ���� �.�. �� ��������� ����� �������� � ������� 
 �� 
 �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� ����� �������� ������
 ��������� � ������ �� ����� �� ��� ����� ����� ���������
 ?
 --------------------------------------------------------------

 LMR
 ��������� ��������� ���� �� ��� �� ������ ���� ���� ����� ���� �� ��� � ���� �� � ������� ��������
 ������ ������ ����: ���-���, ������ ������, ��� ������� �� ������������
 ��� ������ �� ������������
 ��� ��������� ���� �������� �� ���� ���� ���
 ?
 --------------------------------------------------------------

 FUTILITY PRUNING
 ���� �� ������ �� ��� � �� � ������� ����� � �� � ������� ������ ��
 �� ������������� ������� �.�. depth_max - depth == FUTILITY_DEPTH(����� 1)
 ���� 
 ������ - FUTILITY_MARGIN(����� 100) >= beta return beta
 �.�. ���� ������� �� ������� ���� ���� � ����� �� �� ������ ���������� ����
 ����� ������� ������� ��������� �� ���� ��� � �� �������� ������������� �����.
 ?
 
 --------------------------------------------------------------

 ---------------------
 ����� ����� ������� � ���������� 
 �� �������� �������� �� ��� �� ����������
 ��� ���� �������� � ��������� �� ���� �� ���� ��� ��� �������

********************************************/
class FULLSEARCH{

  unsigned __int16 audit;// �������� ������ ������� ����������� ������
  unsigned __int8 depth_max_rem;//
  unsigned __int8 lmr_reduction;//
  bool flag_stop_search;// ���������� ��� ������� �������� ��� ���������� ������
  __int64 nodes;// ���������� �����
  char string_in2[65536];//
  unsigned __int8 from;// ������ ����� ������
  unsigned __int8 to;// ���� ����� ������
  bool flag_stop_null_move;// ������������� ������������� �������� ����
  //int return_hash_score;
//////////////////////////////////////////////////////////////////////////////////////////
public:


double deep_time_terminate; // �������� �� ������� ����� ��������� ����� 

//---------------------------------------------------------------------------
// 
void ini_global_loop(unsigned __int8 depth_max//
					 );


//---------------------------------------------------------------------------
// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
// �� ��������� �� �������� �� ������
bool get_stop_search();

//---------------------------------------------------------------------------
// ���������� ������ �������
struct T_PV * get_pv_best();

//---------------------------------------------------------------------------
// ���������� ���������� ������������� �����
__int64  get_nodes();

//---------------------------------------------------------------------------
//�������� ���������� ������������� �����
 void set_nodes_in_0();

//---------------------------------------------------------------------------
// ���������� ������ �������� ���
unsigned __int8 get_from();

//---------------------------------------------------------------------------
// ���������� ���� �������� ���
unsigned __int8 get_to();

//---------------------------------------------------------------------------
// ����� �� ������� ������ �� �����
__int32 White_root(struct T_Bitboard * const p_bitboard,// ������������� �����
               struct T_list_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
			   __int32 alpha,//
			   const __int32 beta,// 
			   const unsigned __int8 depth_max// ������������ �������
			   );

//---------------------------------------------------------------------------
// ����� �� ������� ������ �� ������
__int32 Black_root(struct T_Bitboard * const p_bitboard,// ������������� �����
               struct T_list_surplus_moves & list_surplus_moves,// ������ ��������� ����� (������������ ���� �� ������� ����)
			   const __int32 alpha,//
			   __int32 beta,// 
			   const unsigned __int8 depth_max// ������������ �������
			   );

//////////////////////////////////////////////////////////////////////////////////////////
private:

//---------------------------------------------------------------------------
// ����� �� �����
__int32 White(struct T_Bitboard * const p_bitboard,// ������������� �����
		  __int32 alpha,//
		  const __int32 beta,//
		  const unsigned __int8 depth_max,// ������������ �������
		  const unsigned __int8 depth,// �������
		  const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		  const bool flag_pv_f,// �������� �� ��� ������� (pv)
		  bool flag_check,//���� ����
		  const bool flag_null_m// ���� �������� ����
		  );

//---------------------------------------------------------------------------
// ����� �� ������ 
__int32 Black(struct T_Bitboard * const p_bitboard,// ������������� �����
		  const __int32 alpha,//
		  __int32 beta,//
		  const unsigned __int8 depth_max,// ������������ �������
		  const unsigned __int8 depth,// �������
		  const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		  const bool flag_pv_f,// �������� �� ��� ������� (pv)
		  bool flag_check,//���� ����
		  const bool flag_null_m// ���� �������� ����
		  );

//---------------------------------------------------------------------------
// ���������� ������� �� ������������ �������. ��� �����
inline __int32 Max_depth_white(struct T_Bitboard * const p_bitboard,// ������������� �����
						   const __int32 alpha,//
						   const __int32 beta,//
		                   const bool flag_pv_f,// �������� �� ��� ������� (pv)
						   const unsigned __int8 depth// �������
						   );

//---------------------------------------------------------------------------
// ���������� ������� �� ������������ �������. ��� ������
inline __int32 Max_depth_black(struct T_Bitboard * const p_bitboard,// ������������� �����
						   const __int32 alpha,//
						   const __int32 beta,//
		                   const bool flag_pv_f,// �������� �� ��� ������� (pv)
						   const unsigned __int8 depth// �������
						   );

//---------------------------------------------------------------------------
// ��������� �� �����. ��� �����
inline void Extension_for_chek_white(const struct T_Bitboard * const p_bitboard,// ������������� �����
									 const unsigned __int8 depth_max,// ������������ �������
									 unsigned __int8 & depth_max_new,// ����� ������������ �������
									 bool & flag_check// ���� ����
									 );

//---------------------------------------------------------------------------
// ��������� �� �����. ��� ������
inline void Extension_for_chek_black(const struct T_Bitboard * const p_bitboard,// ������������� �����
									 const unsigned __int8 depth_max,// ������������ �������
									 unsigned __int8 & depth_max_new,// ����� ������������ �������
									 bool & flag_check// ���� ����
									 );

//---------------------------------------------------------------------------
// ��������� ��������� �� �������
inline void Extension_for_captures(const __int32 description_move,// ��� ����
								   const __int32 description_move_up,// ��� ���� �� ���������� �������
								   const unsigned __int8 depth_max,// ������������ �������
								   unsigned __int8 & depth_max_new// ����� ������������ �������
								   );

//---------------------------------------------------------------------------
// ������ ����������
inline void Sorting_moves(struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						  );

//---------------------------------------------------------------------------
// ������ ���������� ��������� ������
inline void Sorting_white_moves_root(struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						  );

//---------------------------------------------------------------------------
// ������ ���������� ��������� ������
inline void Sorting_black_moves_root(struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
						  );

//---------------------------------------------------------------------------
// ��������� ��� �����
void Print_name_from_coord(const struct T_Bitboard * const p_bitboard// ������������� �����
						   );

//---------------------------------------------------------------------------
// ������� ������� ������� ����������
void Print_currmove(const struct T_list_surplus_moves * const p_list_surplus_moves,//������ �����
				  const unsigned __int8 nodes_0,// ���������� ����� �� ������� �������
				  const unsigned __int8 i,// ����� ���� �� ������
				  const unsigned __int8 Depth_Max// ������������ �������
				  );

// ������� ������� ������� � ��������
void Print_pv_line(const bool colour_move,// ���� ���������� �������
				const struct T_list_surplus_moves * const p_list_surplus_moves,// ������ �����
				const struct T_PV * const p_PV_best_point,// ������ �������
				const __int64 print_nodes,// ���������� �����
				const unsigned __int8 i,// ����� ���� � ������
				const unsigned __int8 Depth_Max,// ������������ �������
				const __int32 value// ������ �������
				//,const double time_nodes// ����� ������� ��������
				);

//---------------------------------------------------------------------------
// ��������� ��� �����. 
inline void Save_test_bitboard(const struct T_Bitboard * const p_bitboard,// ������������� �����
                               struct T_Bitboard * p_test_bitboard// ������������� �����
						       );

//---------------------------------------------------------------------------
// ��������� ��� �����. ���������� �����
inline void Compare_test_bitboard(const struct T_Bitboard * const p_bitboard,// ������������� �����
                                  struct T_Bitboard * p_test_bitboard// ������������� �����
						          );

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������, ������
#if TEST_L
void List_Print(int Depth,// ������� �������
                struct T_list_surplus_moves * p_list_surplus_moves// ������ �����
				);
#endif
//---------------------------------------------------------------------------

};//class FULLSEARCH
