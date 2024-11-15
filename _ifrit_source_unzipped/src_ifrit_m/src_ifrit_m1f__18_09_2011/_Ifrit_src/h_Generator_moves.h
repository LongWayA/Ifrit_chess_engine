
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010 18.09.2011
/// ������� ����� ������� 99%




///!
#include "a_Header_constants.h"
#include "a_List.h"
#include "a_Position.h"

/// ��������-���������(1-0) �������� ������ ����� 
#define KONTROL_B 0 

/// 
#define TEST_H 0 



/// ��������� ��������(bit board) ���������
/// ��������� �� Magic Bitboards

/// �������� �� ��������� ������:
/// "Hyatt, Robert. Rotated bitmaps, a new twist on an old idea"
/// "Magic Move-Bitboard Generation in Computer Chess Pradyumna Kannan April 30, 2007"
/// chessprogramming - Magic Bitboards 
/// http://chessprogramming.wikispaces.com/Magic+Bitboards

/// ���������� ��������� ���� � Pradyumna Kannan �� magicmoves.�
/// �������� ����� ���������

/// �� ������� �������� � ��������� Position ���������� ������ ����� (��������� List)
/// ������ ����� ��������� ������ ��� �������� ���� ��� ��� � ����������� ���
/// + ���������� �������� �����

/// �������� �� ��� � ���� ������ �� ������� ������ ��� ��� ������� �������� �����
/// ������� �� ��� ������� �� ��������� � ������ ����� ����� ���������� ����

/// ������������
/// ��������� � ������ �������� �� ����������� �������� ��������
/// �� ������ ������(7.09.11) �����(bugs) �� ��������


///---------------------------------------------------------------------------
/// ������ ��������� ����� 
class h_Generator_moves 
{

/// ������� ��� �����


//////////////////////////////////////////////////////////////////////////////////////
private:

	unsigned __int64 bishop_attacks_masks_database[64][512]; /// 1<<9
	unsigned __int64 rook_attacks_masks_database[64][4096]; /// 1<<12


//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	///
	void ini_attacks_magic_masks_database();

//////////////////////////////////////////////////////////////////////////////////////
private:

	///---------------------------------------------------------------------------
	///
	unsigned __int64 ini_deg_masks
	(
		const int number_on_bit[], 
		const int count_attack_squares, 
		const unsigned __int64 deg
	);

	///---------------------------------------------------------------------------
	///
	unsigned __int64 ini_rook_attacks_masks
	(
		const int square, 
		const unsigned __int64 deg_masks
	);

	///---------------------------------------------------------------------------
	///
	unsigned __int64 ini_bishop_attacks_masks
	(
		const int square, 
		const unsigned __int64 deg_masks
	);

	///---------------------------------------------------------------------------
	/// 
	inline unsigned __int64 get_bishop_attacks_masks_magic
	(
		const unsigned __int8 & square,/// ������������ ������ 
		const unsigned __int64 & position_all_piece ///
	);

	///---------------------------------------------------------------------------
	/// 
	inline unsigned __int64 get_rook_attacks_masks_magic
	(
		const unsigned __int8 & square,/// ������������ ������ 
		const unsigned __int64 & position_all_piece ///
	);


/// ��������� �����

//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	/// ���������� ������ ������ + ����������� ����� (��� �����)
	void generator_captures_white
	(
		struct List & list,/// ������������� ������ �����
		const struct Position & position /// ������������� �����            
	);

	///---------------------------------------------------------------------------
	/// ���������� ������ ������  + ����������� ����� (��� ������)
	void generator_captures_black
	(
		struct List & list,/// ������������� ������ �����
		const struct Position & position /// ������������� �����    
	);

	///---------------------------------------------------------------------------
	/// ���������� ������ ��������� ����� 
	/// (�.�. ���� ����� ����� ������ + ����������� �����)(��� �����)
	void generator_move_white
	(
		struct List & list,/// ������������� ������ �����
		const struct Position & position /// ������������� �����    
	);

	///---------------------------------------------------------------------------
	/// ���������� ������ ��������� ����� 
	/// (�.�. ���� ����� ����� ������ + ����������� �����)(��� ������)
	void generator_move_black
	(
		struct List & list,/// ������������� ������ �����
		const struct Position & position /// ������������� �����    
	);

	///---------------------------------------------------------------------------
	/// ��� ���� �� ������ ����� ���� ���� ��� �� ������� 
	/// ���������� 0 ����� ����� �������� ������
	unsigned __int8 king_white_check
	(
		const struct Position & position /// ������������� �����   
	);

	///---------------------------------------------------------------------------
	/// ��� ���� �� ������ ������ ���� ���� ��� �� ������� 
	/// ���������� 0 ����� ����� �������� ������
	unsigned __int8 king_black_check
	(
		const struct Position & position /// ������������� �����   
	);




/////////////////////////////////////////////////////////
///       ����������� �����


//////////////////////////////////////////////////////////////////////////////////////
private:


	///---------------------------------------------------------------------------
	/// ��� ���� �� ����� ���� ���� ���� ��� �� ������� 
	/// ���������� 0 ����� ����� �������� ������
	inline unsigned __int8 white_check_square
	(
		const unsigned __int8 & sq,///������ ������������ ����(0-63) 
		const struct Position & position /// ������������� �����   
	);

	///---------------------------------------------------------------------------
	/// ��� ���� �� ������ ���� ���� ���� ��� �� ������� 
	/// ���������� 0 ����� ����� �������� ������
	inline unsigned __int8 black_check_square
	(
		const unsigned __int8 & sq,///������ ������������ ����(0-63) 
		const struct Position & position /// ������������� �����   
	);

	///---------------------------------------------------------------------------
	/// ��������� ������ ����� - ������ �� �����
	/// ��� ������� ������ �� �� ������� �������� �������(������ ������)
	inline void list_capture_white
	(
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 name,/// ��� ������� ������
		const unsigned __int8 type_move,/// ��� ����
		const unsigned __int8 & from,/// ������ �����
		const unsigned __int8 & to,/// ���� �����
		unsigned __int8 & count,/// ����� ���� � ������
		const struct Position & position /// ������������� �����    
	);

	///---------------------------------------------------------------------------
	/// ��������� ������ ����� - ������ �� ������
	/// ��� ������� ������ �� �� ������� �������� �������(������ ������)
	inline void list_capture_black
	(
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 name,/// ��� ������� ������
		const unsigned __int8 type_move,/// ��� ����
		const unsigned __int8 & from,/// ������ �����
		const unsigned __int8 & to,/// ���� �����
		unsigned __int8 & count,/// ����� ���� � ������
		const struct Position & position /// ������������� �����    
	);

	///---------------------------------------------------------------------------
	/// ��������� ������ �����
	inline void list_record_move_white
	(
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 name,/// ��� ������� ������
		const unsigned __int8 type_move,/// ��� ����
		const unsigned __int8 & from,/// ������ �����
		const unsigned __int8 & to,/// ���� �����
		unsigned __int8 & count,/// ����� ���� � ������
		const struct Position & position /// ������������� �����    
	);

	///---------------------------------------------------------------------------
	/// ��������� ������ �����
	inline void list_record_move_black
	(
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 name,/// ��� ������� ������
		const unsigned __int8 type_move,/// ��� ����
		const unsigned __int8 & from,/// ������ �����
		const unsigned __int8 & to,/// ���� �����
		unsigned __int8 & count,/// ����� ���� � ������
		const struct Position & position /// ������������� �����    
	);

	///---------------------------------------------------------------------------
	/// ��������� ������ ����� ��� ������ ������� ���� ��������� 
	/// ������ �� ������� � ����������� �����
	inline void list_record_special
	(
		struct List & list,/// ������������� ������ �����
		const unsigned __int8 name,/// ��� ������� ������
		const unsigned __int8 d_m,/// ��� ����
		unsigned __int8 & count,/// ����� ���� � ������
		const unsigned __int8 & from,/// ������ �����
		const unsigned __int8 to,/// ���� �����
		const unsigned __int8 capture/// ��� ������ ������
	);

	///---------------------------------------------------------------------------
	/// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
	/// ��� ��� ��� ����� ����� ���������� ������
	inline void KONTROL_captures_W
	(
		const struct Position & position, /// ������������� �����    
		const unsigned __int8 & from,/// ������ ����� ������
		const unsigned __int8 & to,/// ���� ����� ������
		const unsigned __int8 & p,/// ����� ���� � ������
		const unsigned __int8 name_figure/// ��� ������� ������
	);

	///---------------------------------------------------------------------------
	/// ������������ ����� �� ������� �����, �� ������� ������ � ����� ������ ������  
	/// ��� ��� ��� ������ ����� ���������� ������
	inline void KONTROL_captures_B
	(
		const struct Position & position, /// ������������� �����    
		const unsigned __int8 & from,/// ������ ����� ������
		const unsigned __int8 & to,/// ���� ����� ������
		const unsigned __int8 & p,/// ����� ���� � ������
		const unsigned __int8 name_figure/// ��� ������� ������
	);

	///---------------------------------------------------------------------------
	/// ������������ ����� �� ������� �����, �� ������� ������ 
	/// ��� ���������� ����� 
	inline void KONTROL_move
	(
		const unsigned __int8 & from,// ������ ����� ������
		const unsigned __int8 & to,// ���� ����� ������
		const unsigned __int8 & p// ����� ���� � ������
	);


//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	/// ���� ������ ������������� ���(�.�. ����� ������)
	__inline __int32 bit_scan_forward_win32
	(
		const unsigned __int64 analyzed_number/// ����� � ������� ���� ���������� ���
	);

	///---------------------------------------------------------------------------
	/// ���� ��������� ������������� ���(�.�. ����� �����)
	__inline __int32 bit_scan_reverse_win32
	(
		const unsigned __int64 analyzed_number/// ����� � ������� ���� ���������� ���
	);


//////////////////////////////////////////////////////////////////////////////////////
private:

	///---------------------------------------------------------------------------
	///
	__inline void mobility_king
	(
		unsigned __int64 & mobility_mask, // ����� �����������
		const struct Position & position, /// ������������� �����    
		const unsigned __int8 & square /// ������������ ������     
	);


	///---------------------------------------------------------------------------
	///
	__inline void mobility_queen
	(
		unsigned __int64 & mobility_mask, // ����� �����������
		const struct Position & position, /// ������������� �����    
		const unsigned __int8 & square/// ������������ ������     
	);

	///---------------------------------------------------------------------------
	///
	__inline void mobility_rook
	(
		unsigned __int64 & mobility_mask, // ����� �����������
		const struct Position & position, /// ������������� �����    
		const unsigned __int8 & square/// ������������ ������     
	);

	///---------------------------------------------------------------------------
	///
	__inline void mobility_bishop
	(
		unsigned __int64 & mobility_mask, // ����� �����������
		const struct Position & position, /// ������������� �����    
		const unsigned __int8 & square/// ������������ ������     
	);

	///---------------------------------------------------------------------------
	///
	__inline void mobility_knigt
	(
		unsigned __int64 & mobility_mask, // ����� �����������
		const struct Position & position, /// ������������� �����    
		const unsigned __int8 & square/// ������������ ������     
	);

//////////////////////////////////////////////////////////////////////////////////////
public:

	///---------------------------------------------------------------------------
	/// ���������� 
	void mobility_evaluation
	(
		unsigned __int64 mask_mobility_from_coord[MAX_COORDINATE],
		const struct Position & position /// ������������� �����      
	);
};


