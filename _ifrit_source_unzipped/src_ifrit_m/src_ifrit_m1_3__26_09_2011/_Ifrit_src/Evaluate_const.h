
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 11.12.2010 17.08.2011 19.09.2011 26.09.2011
/// ������� ����� ������� 70%

#ifndef EVALUATE_CONST_H
#define EVALUATE_CONST_H

namespace EVALUATE_CONSTANTS
{

/// System by Max Euwe and Hans Kramer
/// (http://chessprogramming.wikispaces.com/Material)

/// QUEEN =  1000
/// ROOK =   550
/// BISHOP = 350
/// KNIGHT = 350


/// Many strong chess programs including Fruit use a different generic scale
/// (http://chessprogramming.wikispaces.com/Material)

/// QUEEN =  1200
/// ROOK =   600
/// BISHOP = 400
/// KNIGHT = 400

/// Larry Kaufman proposed some modifications based on statistics
/// (http://chessprogramming.wikispaces.com/Material)

/// QUEEN =  975
/// ROOK =   500
/// BISHOP = 325
/// KNIGHT = 325

	/// �������� ��� ������� ���������(�� Larry Kaufman)
	const unsigned __int16 KING = 0;     /// ������  (���� 90000)
	const unsigned __int16 QUEEN = 975; /// �����
	const unsigned __int16 ROOK = 500;   /// ����� 
	const unsigned __int16 BISHOP = 325; /// ���� 
	const unsigned __int16 KNIGHT = 325; /// ���� 
	const unsigned __int16 PAWN = 100;   /// ����� 

	///// �������� ��� ������� ���������(�� Larry Kaufman) + �����
	//const unsigned __int16 KING = 0;     /// ������  (���� 90000)
	//const unsigned __int16 QUEEN = 1075; /// �����
	//const unsigned __int16 ROOK = 600;   /// ����� 
	//const unsigned __int16 BISHOP = 425; /// ���� 
	//const unsigned __int16 KNIGHT = 425; /// ���� 
	//const unsigned __int16 PAWN = 100;   /// ����� 

	/// ������ ����� �����: 0 - 0, 1 - �����, 2 - ����, 
	/// 3- ����, 4 - �����, 5 - �����, 6 - ������;
	const unsigned __int16 KOD[7] = 
	{
		0,PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING, 
	};



///                PST


	/*

	A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
	A7(48) B7(49) C7(50) D7(51) E7(52) F7(53) G7(54) H7(55)
	A6(40) B6(41) C6(42) D6(43) E6(44) F6(45) G6(46) H6(47)
	A5(32) B5(33) C5(34) D5(35) E5(36) F5(37) G5(38) H5(39)

	A4(24) B4(25) C4(26) D4(27) E4(28) F4(29) G4(30) H4(31)
	A3(16) B3(17) C3(18) D3(19) E3(20) F3(21) G3(22) H3(23)
	A2(8)  B2(9)  C2(10) D2(11) E2(12) F2(13) G2(14) H2(15)
	A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

	��� ������ 
	eval = eval + test[from];

	��� ������ �� �1 �.�. from = 7 test[from] = 7

	����� ������� ��� ��������� ���������� ������������ ��������� !

	��������������� ��� �������� ���:

	test[A8] = 56 test[B1] = 57 ... test[G1] = 62 test[H1] = 63 	
	......
	......
	test[A1] = 0  test[B1] = 1  ... test[G1] = 6  test[H1] = 7 

	�.�. ����� �� ������� �� ������ ��� �� �� ����� �� ����� � ����� �������
	��������� ���������� ������������ �������������� �����
	������� ��� �������� � ��� ���� ��������� ����� ������ ������� ���.

	*/

	/// ��������� ��� ������������ ���������� ������ pst
	const __int8 test[] =
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9,10,11,12,13,14,15,
		16,17,18,19,20,21,22,23,
		24,25,26,27,28,29,30,31,

		32,33,34,35,34,37,38,39,
		40,41,42,43,44,45,46,47,
		48,49,50,51,52,53,54,55,
		56,57,58,59,60,61,62,63,
	};


// ��������� ������������ ����� �� 
// Evaluation functions in Rybka 1.0 Beta and Rybka 2.3.2a (RybkaEvalCompare.pdf)

/// ������

	/// ������ ������ ���������� ����������
	/// � ������ � �������� ������

	const __int16 pozicion_king_white[] =
	{
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,

		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407
	};
// �����
	const __int16 pozicion_king_black[] =
	{
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,

		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407,
		1407,1876,938,0,0,938,1876,1407
	};

	/// � �������� ������ ������ ���������� � ������
	const __int16 pozicion_king_white_end[] =
	{
		-2406,-1604,-1203,-802,-802,-1203,-1604,-2406,
		-1604, -802, -401,   0,   0, -401, -802,-1604,
		-1203, -401,    0, 401, 401,    0, -401,-1203,
		 -802,    0,  401, 802, 802,  401,    0, -802,

		 -802,    0,  401, 802, 802,  401,    0, -802,
		-1203, -401,    0, 401, 401,    0, -401,-1203,
		-1604, -802, -401,   0,   0, -401, -802,-1604,
		-2406,-1604,-1203,-802,-802,-1203,-1604,-2406
	};
// �����
	const __int16 pozicion_king_black_end[] =
	{
		-2406,-1604,-1203,-802,-802,-1203,-1604,-2406,
		-1604, -802, -401,   0,   0, -401, -802,-1604,
		-1203, -401,    0, 401, 401,    0, -401,-1203,
		 -802,    0,  401, 802, 802,  401,    0, -802,

		 -802,    0,  401, 802, 802,  401,    0, -802,
		-1203, -401,    0, 401, 401,    0, -401,-1203,
		-1604, -802, -401,   0,   0, -401, -802,-1604,
		-2406,-1604,-1203,-802,-802,-1203,-1604,-2406
	};

/// �����

	/// ����� ����������� ����� �� ����� 
	const __int16 pozicion_queen_white[] =
	{
		-789,-593,-495,-397,-397,-495,-593,-789,
		-392,-196, -98,   0,   0, -98,-196,-392,
		-294, -98,   0,  98,  98,   0, -98,-294,
		-196,   0,  98, 196, 196,  98,   0,-196,

		-196,   0,  98, 196, 196,  98,   0,-196,
		-294, -98,   0,  98,  98,   0, -98,-294,
		-392,-196, -98,   0,   0, -98,-196,-392,
		-588,-392,-294,-196,-196,-294,-392,-588
	};
// �����
	const __int16 pozicion_queen_black[] =
	{
		-588,-392,-294,-196,-196,-294,-392,-588,
		-392,-196, -98,   0,   0, -98,-196,-392,
		-294, -98,   0,  98,  98,   0, -98,-294,
		-196,   0,  98, 196, 196,  98,   0,-196,

		-196,   0,  98, 196, 196,  98,   0,-196,
		-294, -98,   0,  98,  98,   0, -98,-294,
		-392,-196, -98,   0,   0, -98,-196,-392,
		-789,-593,-495,-397,-397,-495,-593,-789
	};

/// �����

	/// ����� ����������� ����� �� ����� 
	const __int16 pozicion_rook_white[] =
	{
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,

		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208
	};
// �����
	const __int16 pozicion_rook_black[] =
	{
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,

		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208,
		-208,-104,0,104,104,0,-104,-208
	};


/// ����

	/// ����� ����������� ����� �� ����� 
	const __int16 pozicion_bishop_white[] =
	{
		-755,-839,-692,-545,-545,-692,-839,-755,
		-588,  84,-147,   0,   0,-147,  84,-588,
		-441,-147, 378, 147, 147, 378,-147,-441,
		-294,   0, 147, 672, 672, 147,   0,-294,

		-294,   0, 147, 672, 672, 147,   0,-294,
		-441,-147, 378, 147, 147, 378,-147,-441,
		-588,  84,-147,   0,   0,-147,  84,-588,
		-504,-588,-441,-294,-294,-441,-588,-504
	};
// �����
	const __int16 pozicion_bishop_black[] =
	{
		-504,-588,-441,-294,-294,-441,-588,-504,
		-588,  84,-147,   0,   0,-147,  84,-588,
		-441,-147, 378, 147, 147, 378,-147,-441,
		-294,   0, 147, 672, 672, 147,   0,-294,

		-294,   0, 147, 672, 672, 147,   0,-294,
		-441,-147, 378, 147, 147, 378,-147,-441,
		-588,  84,-147,   0,   0,-147,  84,-588,
		-755,-839,-692,-545,-545,-692,-839,-755
	};


/// ����

	/// ����� ����������� ���� � ������ ����
	const __int16 pozicion_knigh_white[] =
	{
		-3492,-2798,-2104, -1757, -1757,-2104,-2798,-3492,
		-2440,-1746,-1052,  -705,  -705,-1052,-1746,-2440,
		-1388, -694,    0,   347,   347,    0, -694,-1388,
		 -683,   11,  705,  1052,  1052,  705,   11, -683,

		 -325,  369, 1063, 1410,  1410,  1063,  369, -325,
		 -314,  380, 1074, 1421,  1421,  1074,  380, -314,
		-1366, -672,   22,  369,   369,    22, -672,-1366,
		-5618,-1724,-1030, -683,  -683, -1030,-1724,-5618
	};
// �����
	const __int16 pozicion_knigh_black[] =
	{
		-5618,-1724,-1030, -683,  -683, -1030,-1724,-5618,
		-1366, -672,   22,  369,   369,    22, -672,-1366,
		 -314,  380, 1074, 1421,  1421,  1074,  380, -314,
		 -325,  369, 1063, 1410,  1410,  1063,  369, -325,

		 -683,   11,  705,  1052,  1052,  705,   11, -683,
		-1388, -694,    0,   347,   347,    0, -694,-1388,
		-2440,-1746,-1052,  -705,  -705,-1052,-1746,-2440,
		-3492,-2798,-2104, -1757, -1757,-2104,-2798,-3492
	};


/// �����
/// ������ ����� ������� ��� ����� ��� ���� ����� � ������ ������� :)

	/// ������ � ������ 
	const __int16 pozicion_pawn_king_in_center_white[] =
	{
		  0,0,0,0,0,0,0,0,
		  0,0,0,181,181,0,0,0,
		  -543,-181,0,181,181,0,-181,-543,
		  -543,-181,0,181,181,0,-181,-543,

		  -543,-181,0,255,255,0,-181,-543,
		  -543,-181,0,181,181,0,-181,-543,
		  -543,-181,0,181,181,0,-181,-543,
		  -543,-181,0,181,181,0,-181,-543
	}; 
// �����
	const __int16 pozicion_pawn_king_in_center_black[] =
	{
		  -543,-181,0,181,181,0,-181,-543,
		  -543,-181,0,181,181,0,-181,-543,
		  -543,-181,0,181,181,0,-181,-543,
		  -543,-181,0,255,255,0,-181,-543,

		  -543,-181,0,181,181,0,-181,-543,
		  -543,-181,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,0,0,
		  0,0,0,0,0,0,0,0
	};


	/// ����� �������� ��������� �������� � �������� ����� � ������� ������
	const __int16 pozicion_pawn_king_in_oo_white[] =
	{
		  0,0,0,0,0,0,0,0,
		  0,0,0,181,181,0,0,0,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,

		  0,0,0,255,255,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543
	};

	const __int16 pozicion_pawn_king_in_oo_black[] =
	{
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,

		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,0,0,
		  0,0,0,0,0,0,0,0
	};


	/// ����� ������� ��������� �������� � �������� ����� � ������� ������
	const __int16 pozicion_pawn_king_in_ooo_white[] =
	{
		  0,0,0,0,0,0,0,0,
		  0,0,0,181,181,0,0,0,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,

		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543
	};

	const __int16 pozicion_pawn_king_in_ooo_black[] =
	{
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,

		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,-181,-543,
		  0,0,0,181,181,0,0,0,
		  0,0,0,0,0,0,0,0
	};


	///����� ������ �� �����������. ���������� ��� ��������
	const __int16 pozicion_pawn_white_end[] =
	{
		  0,0,0,0,0,0,0,0,
		  0,0,0,181,181,0,0,0,
		  0,0,0,181,181,0,0,0,
		  0,0,0,181,181,0,0,0,

		  0,0,0,255,255,0,0,0,
		  0,0,0,255,255,0,0,0,
		  0,0,0,255,255,0,0,0,
		  0,0,0,255,255,0,0,0
	};

	const __int16 pozicion_pawn_black_end[] =
	{
		  0,0,0,255,255,0,0,0,
		  0,0,0,255,255,0,0,0,
		  0,0,0,255,255,0,0,0,
		  0,0,0,255,255,0,0,0,

		  0,0,0,181,181,0,0,0,
		  0,0,0,181,181,0,0,0,
		  0,0,0,181,181,0,0,0,
		  0,0,0,0,0,0,0,0
	};

	///����� ��������� ������ �� �����������
	const __int16 pozicion_perspective_pawn_white_end[] =
	{
		   0,  0,  0,  0,  0,  0,  0,  0,
		   0,  0,  0,  0,  0,  0,  0,  0,               
		  181, 181, 181, 181, 181, 181, 181, 181,
		  255, 255, 255, 255, 255, 255, 255, 255,

		 1600,1600,1600,1600,1600,1600,1600,1600,
		 3200,3200,3200,3200,3200,3200,3200,3200,
		 6400,6400,6400,6400,6400,6400,6400,6400,  
		   0,  0,  0,  0,  0,  0,  0,  0  
	};

	const __int16 pozicion_perspective_pawn_black_end[] =
	{
		    0,   0,   0,   0,   0,   0,   0,   0, 
		 6400,6400,6400,6400,6400,6400,6400,6400,   
		 3200,3200,3200,3200,3200,3200,3200,3200,  
		 1600,1600,1600,1600,1600,1600,1600,1600,

		  255, 255, 255, 255, 255, 255, 255, 255,
		  181, 181, 181, 181, 181, 181, 181, 181,
		   0,  0,  0,  0,  0,  0,  0,  0,         
		   0,  0,  0,  0,  0,  0,  0,  0          
	};

// ��������� ������������ ����� �� ������� ������� ���� ���������

	/// ������������ ������
	const unsigned __int8 PENALTY_CASTLING_O_O   = 30;/// ����� �� �������� ���������
	const unsigned __int8 PENALTY_CASTLING_O_O_O = 20;/// ����� �� ������� ���������
	//const unsigned __int8 PENALTY_SHIELD = 20;/// ����� �� ��������� ������ ������� ������
	const unsigned __int16 PENALTY_PAWNN = 158;/// ����� �� ��������� �����
	const unsigned __int16 PERSPECTIVE_PAWNN = 320;/// ��������� ����� ����� �� ������ ��������� (���� 3)

	//const unsigned __int8 CONTROL_SQUARE = 1;/// ����������� ������� ������������ ���������
	const unsigned __int16 CONTROL_SQUARE_QUEEN = 54;/// ����������� ������� ������������ ������
	const unsigned __int16 CONTROL_SQUARE_ROOK = 84;/// ����������� ������� ������������ ������
	const unsigned __int16 CONTROL_SQUARE_BISHOP = 149;/// ����������� ������� ������������ ������
	const unsigned __int16 CONTROL_SQUARE_KNIGHT = 14;/// ����������� ������� ������������ �����

	//const unsigned __int8 ATAK_KING = 20;/// ����������� ����� �� ������
	const unsigned __int16 ATAK_KING_QUEEN = 532;/// ����������� ����� �� ������
	const unsigned __int16 ATAK_KING_ROOK = 665;/// ����������� ����� �� ������
	const unsigned __int16 ATAK_KING_BISHOP = 418;/// ����������� ����� �� ������
	const unsigned __int16 ATAK_KING_KNIGHT = 941;/// ����������� ����� �� ������

	/// ��� ������ ����� ����� ������� ������ � �������� ����� ����� ������
	const unsigned __int16 RUBIKON = 
		//0; // test
		QUEEN + ROOK + KING;///����� �������� � ��������(��� ����� � ���� � �����)1600
						/// ��� ������ ��������� � ����� ������ �� ��������   

}

using namespace EVALUATE_CONSTANTS;

#endif // !defined EVALUATE_CONST_H