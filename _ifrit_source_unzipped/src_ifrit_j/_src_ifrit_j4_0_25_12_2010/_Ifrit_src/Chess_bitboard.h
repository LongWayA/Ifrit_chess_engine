#ifndef A_CHESS_BITBOARD_H
#define A_CHESS_BITBOARD_H

#include <cstring>

#include "Structure.h"
#include "List_surplus_moves.h"

// a_Chess_bitboard_make_move.cpp
#define MATERIAL 1 // ��������-���������(1-0) ����� ������� ���������

//---------------------------------------------------------------------------
/*++
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 20.11.2010
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 99%

����� Chess_bitboard �������� � ���� ���������� ������������� 
��������� ����� � ������ ��� ������ � ���.
����� ���:
���������� �����
������������� ����� �� ��� �������
��������� ���� ���������� ������������� �����

*/


// ������� �����
class Chess_bitboard
{
	// ������ �� ������� �������� ������ ������ � �������� �����
	friend class List_surplus_moves;
	friend class Evaluate;

// ���� ������
//////////////////////////////////////////////////////////////////////////////////////////
private:
//public:
	//-------------------------------------------------
	// ����� ������������ ��������� �������
	//A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
	//A7(48) B7(49) C7(50) D7(51) E7(52) F7(53) G7(54) H7(55)
	//A6(40) B6(41) C6(42) D6(43) E6(44) F6(45) G6(46) H6(47)
	//A5(32) B5(33) C5(34) D5(35) E5(36) F5(37) G5(38) H5(39)
	//A4(24) B4(25) C4(26) D4(27) E4(28) F4(29) G4(30) H4(31)
	//A3(16) B3(17) C3(18) D3(19) E3(20) F3(21) G3(22) H3(23)
	//A2(8)  B2(9)  C2(10) D2(11) E2(12) F2(13) G2(14) H2(15)
	//A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)
	//

	//A1(LSB)- ��� ������� ��� H8(MSB)-��� 63 ���
	// �.�. ������� ��������� �������� ��������� �������
	// H8(63) G8(62) F8(61) . . . . . .C1(2) B1(1) A1(0)
	//-------------------------------------------------
	// �����
	// � ���� 64 ������ ����������
	// ������� ������ ���� ������� ��� ���������� ������ � ����� �� ����
	// ������ ����������  ��� ��� �� ������ �������� ������ ���� �� ��������������
	// � ��� ��� ���� ����������
	unsigned __int64  all_piece;// ��� ������
	unsigned __int64  all_white_piece;// ��� ����� ������
	unsigned __int64  all_black_piece;// ��� ������ ������

	//-------------------------------------------------
	// ������ �����
	/*
	��� ������
	0 - ��� ������
	1 - ����� 
	2 - ���� 
	3 - ����
	4 - ����� 
	5 - ����� 
	6 - ������
	*/
	// ������� ������ ����� � ������ �����
	unsigned __int64 white_piece[MAX_NAME];
	unsigned __int64 black_piece[MAX_NAME];

	// ��� ������ �� ���������� 
	unsigned __int8 white_name_from_coord[MAX_COORDINATE];
	unsigned __int8 black_name_from_coord[MAX_COORDINATE];

	//-------------------------------------------------
	// ��������������� ����������
	// ���� ���� 0 - ������ 1 - �����
	bool colour_move;

	// ���������� ������ �� ������� 1/0
	bool taking_passage_yes;

	// � ���������� ������ ����(������������ �� �����)
	// ��� � - 0 , h - 7 ����� ������� �� � ����������� ������
	unsigned __int8 taking_passage_x;

	// � ���������� ������ ����
	// ��� '1' - 0 , '8' - 7 ����� ������� ����������� ���� ������ ������ �������� �� 1
	unsigned __int8 taking_passage_y;

	// ��������� ����� � ������� �������   1/0
	bool castling_Q;

	// ��������� ����� � �������� �������  1/0
	bool castling_K;

	// ��������� ������ � ������� �������  1/0
	bool castling_q;

	// ��������� ������ � �������� ������� 1/0
	bool castling_k;

	// 64 ������ ���-���� �������
	unsigned __int64 hash_key ;

	// ������ ������� �������� �� �����	
	unsigned __int16 value_white_material ;

	// ������ ������� �������� �� ������
	unsigned __int16 value_black_material ;

	// ���������� ��� delta pruning
	// ���������� ������ �� �����
	unsigned __int64 all_black_or_white_piece;


// ������ � ����� ������
//////////////////////////////////////////////////////////////////////////////////////////
public:

	// ��� ������
	inline unsigned __int64 get_all_piece()
	{
		return all_piece;
	};

	// ��� ����� ������
	inline unsigned __int64 get_all_white_piece()
	{
		return all_white_piece;
	};

	// ��� ������ ������
	inline unsigned __int64 get_all_black_piece()
	{
		return all_black_piece;
	};

	// ������� ������ ����� �����
	inline unsigned __int64 get_white_piece(unsigned __int8 name_piece)
	{
		return white_piece[name_piece];
	};

	// ������� ������ ������ �����
	inline unsigned __int64 get_black_piece(unsigned __int8 name_piece)
	{
		return  black_piece[name_piece];
	};


	//  ��� ������ �� ���������� 
	inline unsigned __int8 get_white_name_from_coord(unsigned __int8 coordinate_piece)
	{
		return white_name_from_coord[coordinate_piece];
	};

	// ��� ������ �� ���������� 
	inline unsigned __int8 get_black_name_from_coord(unsigned __int8 coordinate_piece)
	{
		return  black_name_from_coord[coordinate_piece];
	};

	// ���� ���� 0 - ������ 1 - �����
	inline bool get_colour_move()
	{
		return colour_move;
	};

	// ���������� ������ �� ������� 1/0
	inline bool get_taking_passage_yes()
	{
		return taking_passage_yes;
	};

	// � ���������� ������ ����(������������ �� �����)
	// ��� � - 0 , h - 7 ����� ������� �� � ����������� ������
	inline unsigned __int8 get_taking_passage_x()
	{
		return taking_passage_x;
	};

	// � ���������� ������ ����
	// ��� '1' - 0 , '8' - 7 ����� ������� ����������� ���� ������ ������ �������� �� 1
	inline unsigned __int8 get_taking_passage_y()
	{
		return taking_passage_y;
	};

	// ��������� ����� � ������� �������   1/0
	// ��� ������ ������
	inline bool get_castling_Q()
	{
		return castling_Q;
	};

	// ��������� ����� � �������� �������  1/0
	inline bool get_castling_K()
	{
		return castling_K;
	};

	// ��������� ������ � ������� �������  1/0
	inline bool get_castling_q()
	{
		return castling_q;
	};

	// ��������� ������ � �������� ������� 1/0
	inline bool get_castling_k()
	{
		return castling_k;
	};

	// 64 ������ ���-���� �������
	inline unsigned __int64 get_hash_key()
	{
		return hash_key;
	};

	// ������ ������� �������� �� �����
	inline unsigned __int16 get_value_white_material()
	{
		return value_white_material;
	};

	// ������ ������� �������� �� ������
	inline unsigned __int16 get_value_black_material()
	{
		return value_black_material;
	};

	// ���������� ��� delta pruning
	inline unsigned __int64 get_all_black_or_white_piece()
	{
		return all_black_or_white_piece;
	};


// �������� ���� ������ 
//////////////////////////////////////////////////////////////////////////////////////////
public:

	// 64 ������ ���-���� �������
	inline void set_hash_key(unsigned __int64 _hash_key)
	{
		hash_key = _hash_key;
	};

	// ���� ���� 0 - ������ 1 - �����
	inline void set_colour_move(bool _colour_move)
	{
		colour_move = _colour_move;
	};

	// ���������� ������ �� ������� 1/0
	inline void set_taking_passage_yes(bool _taking_passage_yes)
	{
		taking_passage_yes = _taking_passage_yes;
	};

	// � ���������� ������ ����(������������ �� �����)
	// ��� � - 0 , h - 7 ����� ������� �� � ����������� ������
	inline void set_taking_passage_x(unsigned __int8 _taking_passage_x)
	{
		taking_passage_x = _taking_passage_x;
	};

	// � ���������� ������ ����
	// ��� '1' - 0 , '8' - 7 ����� ������� ����������� ���� ������ ������ �������� �� 1
	inline void set_taking_passage_y(unsigned __int8 _taking_passage_y)
	{
		taking_passage_y = _taking_passage_y;
	};

	// ������ ������� �������� �� �����
	inline void set_value_white_material(unsigned __int16 _value_white_material)
	{
		value_white_material = _value_white_material;
	};

	// ������ ������� �������� �� ������
	inline void set_value_black_material(unsigned __int16 _value_black_material)
	{
		value_black_material = _value_black_material;
	};


// ���������� � �����
// a_Chess_bitboard_make_move.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// ��������� ���� ������ ��� �����
	void onward_moves_white
	(
		const unsigned __int8 & i,//����� ���� �� ������
		struct Undo undo[],//���������� ��� ������ ����
		const class List_surplus_moves & list_surplus_moves//������ ��������� �����
	);

	//---------------------------------------------------------------------------
	// ��������� ���� ������ ��� ������
	void onward_moves_black
	(
		const unsigned __int8 & i,//����� ���� �� ������
		struct Undo undo[],//���������� ��� ������ ����
		const class List_surplus_moves & list_surplus_moves//������ ��������� �����
	);

	//---------------------------------------------------------------------------
	// ��������� ���� ����� ��� �����
	void backs_moves_white
	(
		const unsigned __int8 & i,//����� ���� �� ������
		const struct Undo undo[],//���������� ��� ������ ����
		const class List_surplus_moves & list_surplus_moves//������ ��������� �����
	);

	//---------------------------------------------------------------------------
	// ��������� ���� ����� ��� ������
	void backs_moves_black
	(
		const unsigned __int8 & i,//����� ���� �� ������
		const struct Undo undo[],//���������� ��� ������ ����
		const class List_surplus_moves & list_surplus_moves//������ ��������� �����
	);


// ���������� � �����
// a_Chess_bitboard_FEN.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// ��������� �������� ��� - �������
	void parse_position
	(
		std::string string_in//�������� ������
	);


	//---------------------------------------------------------------------------
	// ��������� ������������� �����
	void bitboard_ini_t();


// ���������� � �����
// a_Chess_bitboard_FEN.cpp
/////////////////////////////////////////////////////////////////////////////////////
private:

	//---------------------------------------------------------------------------
	// �� ����� ������ ������ ������	
	// a-0 b-1 c-2 d-3 e-4 f-5 g-6 h-7 ����� 777 :)
	int koordinata_to_indeks
	(
		__int8 ch
	);

	//---------------------------------------------------------------------------
	//	�� ������� ����� ������ �����
	// ���������� ������ ������� � ���� ��� ����� �������� ��� ���� ������� :)
	// '1'-0 '2'-1 '3'-2 '4'-3 '5'-4 '6'-5 '7'-6 '8'-7 ����� 777 :)
	int ch_koordinata_to_indeks
	(
		int ch
	);

	//---------------------------------------------------------------------------
	// ��������� ������ ��� � �������������� �����
	void fen_to_board
	(
		std::string position_fen//������ ���������� ��� �������
	);

	//---------------------------------------------------------------------------
	// �������� � ������ ������ �.�. ���� ���������� � ������� ������� �� ���������
	void move_to_board
	(
		__int8 move_string[]// ������ ���������� ����(7)
	);


// ���������� � �����
// a_Chess_bitboard_delta_pruning.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	//
	void delta_pruning_remember_piese_white();

	//---------------------------------------------------------------------------
	//
	void delta_pruning_white
	(
		const __int32 alpha,//
		__int32 & value_stat // ����������� ������
	);

	//---------------------------------------------------------------------------
	//
	void delta_pruning_return_piese_white();


	//---------------------------------------------------------------------------
	//
	void delta_pruning_remember_piese_black();


	//---------------------------------------------------------------------------
	//
	void delta_pruning_black
	(
		const __int32 beta,//
		__int32 & value_stat // ����������� ������
	);

	//---------------------------------------------------------------------------
	//
	void delta_pruning_return_piese_black();


// ���������� � �����
// a_Chess_bitboard_test.cpp
//////////////////////////////////////////////////////////////////////////////////////////
public:

	//---------------------------------------------------------------------------
	// ��������� ��� �����
	void print_name_from_coord();

	//---------------------------------------------------------------------------
	// ��������� ��� �����. ���������� �����
	void save_test_bitboard
	(
		const class Chess_bitboard & bitboard// ������������� �����
	);

	//---------------------------------------------------------------------------
	// ��������� ��� �����. ���������� �����
	void compare_test_bitboard
	(
		const class Chess_bitboard & bitboard// ������������� �����
	);

};

#endif // !defined A_CHESS_BITBOARD_H