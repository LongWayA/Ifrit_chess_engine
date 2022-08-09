
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 20.11.2010 18.09.2011
/// ������� ����� ������� 99%




///!
#include "a_move_list_struct.h"
#include "a_position_struct.h"

/// ��������-���������(1-0) �������� ������ ����� 
#define KONTROL_B 0 

/// 
#define TEST_H 0 

/*
Move Generation
http://chessprogramming.wikispaces.com/Move+Generation

 Generation of moves is a basic part of a chess engine with many variations. 
 The implementation heavily depends on the board representation, and it 
 can be generalized into two types:

Pseudo-legal
 In Pseudo-legal move generation pieces obey their normal rules of movement,
 but they're not checked beforehand to see if they'll leave the king in check.
 It is left up to the move-making function to test the move, or it is even 
 possible to let the king remain in check and only test for the capture of the 
 king on the next move.


Magic Bitboards 
http://chessprogramming.wikispaces.com/Magic+Bitboards

 Magic bitboards uses a multiply-right-shift perfect hashing algorithm 
 to index a attack bitboard database - which leaves both line-attacks 
 of a bishop or rook in one run.

 Thanks to the fast 64-bit multiplication and fast and huge caches of 
 recent processors, Magic Bitboards has become a de facto standard of 
 modern bitboard engines, as used for instance in Crafty, Arasan, Stockfish 
 and Houdini. While Robert Hyatt reported no immediate speed gain over 
 Rotated Bitboards in Crafty [1], Jon Dart mentioned a 20-25% speedup [2] 
 in Arasan over rotated.

end link(����� ������)---------------------
*/

/*
 ��������� ��������(bit board) ���������
 ��������� �� Magic Bitboards

 �������� �� ��������� ������:
 "Hyatt, Robert. Rotated bitmaps, a new twist on an old idea"
 "Magic Move-Bitboard Generation in Computer Chess Pradyumna Kannan April 30, 2007"

 ���������� ��������� ���� � Pradyumna Kannan �� magicmoves.�
 �������� ����� ���������

 �� ������� �������� � ��������� Position ���������� ������ ����� (��������� List)
 ������ ����� ��������� ������ ��� �������� ���� ��� ��� � ����������� ���
 + ���������� �������� �����

 �������� �� ��� � ���� ������ �� ������� ������ ��� ��� ������� �������� �����
 ������� �� ��� ������� �� ��������� � ������ ����� ����� ���������� ����

 ������������
 ��������� � ������ �������� �� ����������� �������� ��������
 �� ������ ������(7.09.11) �����(bugs) �� ��������
*/

///---------------------------------------------------------------------------
/// ������ ��������� ����� 
namespace Move_generation 
{

/// ������� ��� �����


//////////////////////////////////////////////////////////////////////////////////////
//private:




//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	///
	void ini_attacks_magic_masks_database();

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


//////////////////////////////////////////////////////////////////////////////////////
//private:

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


/// ��������� �����

//////////////////////////////////////////////////////////////////////////////////////
//public:

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
//private:


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
//public:

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

};


