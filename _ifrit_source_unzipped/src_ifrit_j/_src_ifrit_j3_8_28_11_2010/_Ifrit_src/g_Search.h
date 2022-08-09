#include "a_Structure.h"
#include "a_Chess_bitboard.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 2.11.2009 20.12.2009
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 50%

 ������� �������

 �������� � ���� ������ � ������ ��������� ��������
 
 --------------------------------------------------------------
 ������ ����� ������� � ���������� 

*/


namespace FULLSEARCH
{
	//---------------------------------------------------------------------------
	// 
	void public_set_depth_max_rem
	(
		unsigned __int8 depth_max_rem_v//
	);


	//---------------------------------------------------------------------------
	// 
	void public_ini_flag_stop_null_move
	(
		bool flag_stop_null_move_v//
	);

	//---------------------------------------------------------------------------
	// 
	void public_ini_deep_time_terminate
	(
		double deep_time_terminate_v//
	);


	//---------------------------------------------------------------------------
	// 
	void public_ini_global_loop();


	//---------------------------------------------------------------------------
	// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
	// �� ��������� �� �������� �� ������
	bool public_get_stop_search();

	//---------------------------------------------------------------------------
	// ���������� ������ �������
	struct T_PV * public_public_get_pv_best();

	//---------------------------------------------------------------------------
	// ���������� ���������� ������������� �����
	__int64  public_get_nodes();

	//---------------------------------------------------------------------------
	//�������� ���������� ������������� �����
	void public_set_nodes_in_0();

	//---------------------------------------------------------------------------
	// ����� �� �����
	__int32 public_white
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
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
	__int32 public_black
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		const __int32 alpha,//
		__int32 beta,//
		const unsigned __int8 depth_max,// ������������ �������
		const unsigned __int8 depth,// �������
		const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		bool flag_check,//���� ����
		const bool flag_null_m// ���� �������� ����
	);


	/////////////////////////////////////////////////////////////////////////////////////
	//private:

	//---------------------------------------------------------------------------
	// �� �����
	inline void PVS_white
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// ������������ �������
		const unsigned __int8 depth,// �������
		const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const bool old_flag_check,//���� ����
		const bool flag_null_m,// ���� �������� ����
		const unsigned __int8 new_depth,// ��������� �������
		const unsigned __int64 key_undo,
		const unsigned __int8 i,
		__int32 & value,// ������� ������ �������
		const unsigned __int8 depth_max_new,
		class List_surplus_moves list_surplus_moves,
		const bool flag_check
	);


	//---------------------------------------------------------------------------
	//  �� ������
	inline void PVS_black
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// ������������ �������
		const unsigned __int8 depth,// �������
		const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const bool old_flag_check,//���� ����
		const bool flag_null_m,// ���� �������� ����
		const unsigned __int8 new_depth,// ��������� �������
		const unsigned __int64 key_undo,
		const unsigned __int8 i,
		__int32 & value,// ������� ������ �������
		const unsigned __int8 depth_max_new,
		class List_surplus_moves list_surplus_moves,
		const bool flag_check
	);

	//---------------------------------------------------------------------------
	// ����� ����������� ��������� ��������� ��� ������ �������� ����
	// � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
	// ���� ��� � ��� ���
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	// �� �����
	inline __int32 null_move_white
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// ������������ �������
		const unsigned __int8 depth,// �������
		const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const bool old_flag_check,//���� ����
		const bool flag_null_m,// ���� �������� ����
		const unsigned __int8 new_depth, // ��������� �������
		const unsigned __int64 key_undo
	);

	//---------------------------------------------------------------------------
	// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	// ������� ���� ��� ����� ����� ���������
	// �� ������
	inline __int32 null_move_black
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,// ������������ �������
		const unsigned __int8 depth,// �������
		const __int32 description_move,// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const bool old_flag_check,//���� ����
		const bool flag_null_m,// ���� �������� ����
		const unsigned __int8 new_depth, // ��������� �������
		const unsigned __int64 key_undo
	);


	//---------------------------------------------------------------------------
	// ���������� ������� �� ������������ �������. ��� �����
	inline __int32 max_depth_white
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const unsigned __int8 depth// �������
	);

	//---------------------------------------------------------------------------
	// ���������� ������� �� ������������ �������. ��� ������
	inline __int32 max_depth_black
	(
		class Chess_bitboard * const p_bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const unsigned __int8 depth// �������
	);

	//---------------------------------------------------------------------------
	// ��������� �� �����. ��� �����
	inline void extension_for_chek_white
	(
		const class Chess_bitboard * const p_bitboard,// ������������� �����
		const unsigned __int8 depth_max,// ������������ �������
		unsigned __int8 & depth_max_new,// ����� ������������ �������
		bool & flag_check// ���� ����
	);

	//---------------------------------------------------------------------------
	// ��������� �� �����. ��� ������
	inline void extension_for_chek_black
	(
		const class Chess_bitboard * const p_bitboard,// ������������� �����
		const unsigned __int8 depth_max,// ������������ �������
		unsigned __int8 & depth_max_new,// ����� ������������ �������
		bool & flag_check// ���� ����
	);

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������, ������
#if TEST_L
	void list_Print
	(
		int Depth,// ������� �������
		class List_surplus_moves * p_list_surplus_moves// ������ �����
	);
#endif

}
