#include "Chess_bitboard.h"

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


*/


class Search
{

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////////
private:

	unsigned __int16 audit;// �������� ������ ������� ����������� ������
	unsigned __int8 depth_max_rem;//
	bool flag_stop_search;// ���������� ��� ������� �������� ��� ���������� ������
	__int64 nodes;// ���������� �����
	__int8 string_in2[65536];//
	unsigned __int8 from;// ������ ����� ������
	unsigned __int8 to;// ���� ����� ������
	bool flag_stop_null_move;// ������������� ������������� �������� ����
	//int return_hash_score;

	double deep_time_terminate; // �������� �� ������� ����� ��������� ����� 

//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// 
	void set_depth_max_rem
	(
		unsigned __int8 depth_max_rem_v//
	);


	//---------------------------------------------------------------------------
	// 
	void ini_flag_stop_null_move
	(
		bool flag_stop_null_move_v//
	);

	//---------------------------------------------------------------------------
	// 
	void ini_deep_time_terminate
	(
		double deep_time_terminate_v//
	);


	//---------------------------------------------------------------------------
	// 
	void ini_global_loop();


	//---------------------------------------------------------------------------
	// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
	// �� ��������� �� �������� �� ������
	bool get_stop_search();

	//---------------------------------------------------------------------------
	// ���������� ������ �������
	struct PV_line & get_pv_best();

	//---------------------------------------------------------------------------
	// ���������� ���������� ������������� �����
	__int64  get_nodes();

	//---------------------------------------------------------------------------
	//�������� ���������� ������������� �����
	void set_nodes_in_0();

	//---------------------------------------------------------------------------
	// ����� �� �����
	__int32 white
	(
		class Chess_bitboard & bitboard,// ������������� �����
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
	__int32 black
	(
		class Chess_bitboard & bitboard,// ������������� �����
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
	// ��������� �� �����. ��� �����
	inline void extension_for_chek_white
	(
		const class Chess_bitboard & bitboard,// ������������� �����
		const unsigned __int8 depth_max,// ������������ �������
		unsigned __int8 & depth_max_new,// ����� ������������ �������
		bool & flag_check,// ���� ����
		class List_surplus_moves & list_surplus_moves// ������ ��������� �����
	);

	//---------------------------------------------------------------------------
	// ��������� �� �����. ��� ������
	inline void extension_for_chek_black
	(
		const class Chess_bitboard & bitboard,// ������������� �����
		const unsigned __int8 depth_max,// ������������ �������
		unsigned __int8 & depth_max_new,// ����� ������������ �������
		bool & flag_check,// ���� ����
		class List_surplus_moves & list_surplus_moves// ������ ��������� �����
	);

/////////////////////////////////////////////////////////////////////////////////////
private:

	//---------------------------------------------------------------------------
	// �� �����
	inline void PVS_white
	(
		class Chess_bitboard & bitboard,// ������������� �����
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
		class List_surplus_moves & list_surplus_moves,
		const bool flag_check
	);


	//---------------------------------------------------------------------------
	//  �� ������
	inline void PVS_black
	(
		class Chess_bitboard & bitboard,// ������������� �����
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
		class List_surplus_moves & list_surplus_moves,
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
		class Chess_bitboard & bitboard,// ������������� �����
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
		class Chess_bitboard & bitboard,// ������������� �����
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
		class Chess_bitboard & bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const unsigned __int8 depth// �������
	);

	//---------------------------------------------------------------------------
	// ���������� ������� �� ������������ �������. ��� ������
	inline __int32 max_depth_black
	(
		class Chess_bitboard & bitboard,// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const bool flag_pv_f,// �������� �� ��� ������� (pv)
		const unsigned __int8 depth// �������
	);

//////////////////////////////////////////////////////////////////////////////////////////
public:

//---------------------------------------------------------------------------
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
// ������� ��������, ������
#if TEST_L
	void list_print
	(
		int Depth,// ������� �������
		class List_surplus_moves & list_surplus_moves// ������ �����
	);
#endif

};
