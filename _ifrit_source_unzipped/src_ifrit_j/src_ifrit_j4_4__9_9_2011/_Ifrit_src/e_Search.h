
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 2.11.2009 20.12.2009
/// ������� ����� ������� 50%



/// ������� �������
/// �������� � ���� ������ � ������ ��������� ��������

class e_Search
{

/// ���� ������
//////////////////////////////////////////////////////////////////////////////////////
private:

	unsigned __int16 audit;/// �������� ������ ������� ����������� ������
	unsigned __int8 depth_max_rem;///
	bool flag_stop_search;/// ���������� ��� ������� �������� ��� ���������� ������
	__int64 nodes;/// ���������� �����
	__int8 string_in2[65536];///
	unsigned __int8 from;/// ������ ����� ������
	unsigned __int8 to;/// ���� ����� ������
	bool flag_stop_null_move;/// ������������� ������������� �������� ����
	//int return_hash_score;

	double deep_time_terminate; /// �������� �� ������� ����� ��������� ����� 

//////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	/// 
	void set_depth_max_rem
	(
		unsigned __int8 _depth_max_rem//
	);


	//---------------------------------------------------------------------------
	/// 
	void ini_flag_stop_null_move
	(
		bool _flag_stop_null_move//
	);

	//---------------------------------------------------------------------------
	/// 
	void ini_deep_time_terminate
	(
		double _deep_time_terminate//
	);


	//---------------------------------------------------------------------------
	/// 
	void ini_global_loop();


	///---------------------------------------------------------------------------
	/// ���������� ���� ��������� ������. ��� ���� � ������� ������ �������� �������
	/// �� ��������� �� �������� �� ������
	bool get_stop_search();

	///---------------------------------------------------------------------------
	/// ���������� ������ �������
	struct PV_line & get_pv_best();

	///---------------------------------------------------------------------------
	/// ���������� ���������� ������������� �����
	__int64  get_nodes();

	///---------------------------------------------------------------------------
	///�������� ���������� ������������� �����
	void set_nodes_in_0();

	///---------------------------------------------------------------------------
	/// ����� �� �����
	__int32 white
	(
		struct Position & position,/// ������������� �����
		__int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth_max,/// ������������ �������
		const unsigned __int8 depth,/// �������
		const __int32 description_move,/// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,/// �������� �� ��� ������� (pv)
		bool flag_check,///���� ����
		const bool flag_null_m/// ���� �������� ����
	);

	///---------------------------------------------------------------------------
	/// ����� �� ������ 
	__int32 black
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		__int32 beta,///
		const unsigned __int8 depth_max,/// ������������ �������
		const unsigned __int8 depth,/// �������
		const __int32 description_move,/// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,/// �������� �� ��� ������� (pv)
		bool flag_check,///���� ����
		const bool flag_null_m/// ���� �������� ����
	);

	///---------------------------------------------------------------------------
	/// ��������� �� �����. ��� �����
	inline void extension_for_chek_white
	(
		struct Position & position,/// ������������� �����
		const unsigned __int8 depth_max,/// ������������ �������
		unsigned __int8 & depth_max_new,/// ����� ������������ �������
		bool & flag_check/// ���� ����
	);

	///---------------------------------------------------------------------------
	/// ��������� �� �����. ��� ������
	inline void extension_for_chek_black
	(
		struct Position & position,/// ������������� �����
		const unsigned __int8 depth_max,/// ������������ �������
		unsigned __int8 & depth_max_new,/// ����� ������������ �������
		bool & flag_check/// ���� ����
	);

/////////////////////////////////////////////////////////////////////////////////////
private:

	///---------------------------------------------------------------------------
	/// ����� ����������� ��������� ��������� ��� ������ �������� ����
	/// � ����� �� ��� ������� ��� �� ����������� �� � ����� ������ ��� ��������� :)
	/// ���� ��� � ��� ���
	/// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	/// ������� ���� ��� ����� ����� ���������
	/// �� �����
	inline __int32 null_move_white
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,//
		const __int32 beta,//
		const unsigned __int8 depth_max,/// ������������ �������
		const unsigned __int8 depth,/// �������
		const __int32 description_move,/// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,/// �������� �� ��� ������� (pv)
		const bool old_flag_check,///���� ����
		const bool flag_null_m,/// ���� �������� ����
		const unsigned __int8 new_depth, /// ��������� �������
		const unsigned __int64 key_undo
	);

	///---------------------------------------------------------------------------
	/// �� ���������� ���� ��� � ���� ���� ��� ����� ���� ��������� �� �������
	/// ������� ���� ��� ����� ����� ���������
	/// �� ������
	inline __int32 null_move_black
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		const __int32 beta,///
		const unsigned __int8 depth_max,/// ������������ �������
		const unsigned __int8 depth,/// �������
		const __int32 description_move,/// ��� ���� ������� ������ � ������ �������
		const bool flag_pv_f,/// �������� �� ��� ������� (pv)
		const bool old_flag_check,///���� ����
		const bool flag_null_m,/// ���� �������� ����
		const unsigned __int8 new_depth, /// ��������� �������
		const unsigned __int64 key_undo
	);


	///---------------------------------------------------------------------------
	/// ���������� ������� �� ������������ �������. ��� �����
	inline __int32 max_depth_white
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		const __int32 beta,///
		const bool flag_pv_f,/// �������� �� ��� ������� (pv)
		const unsigned __int8 depth/// �������
	);

	///---------------------------------------------------------------------------
	/// ���������� ������� �� ������������ �������. ��� ������
	inline __int32 max_depth_black
	(
		struct Position & position,/// ������������� �����
		const __int32 alpha,///
		const __int32 beta,///
		const bool flag_pv_f,/// �������� �� ��� ������� (pv)
		const unsigned __int8 depth/// �������
	);


	///---------------------------------------------------------------------------
	///
	inline void quit_or_stop();

};
