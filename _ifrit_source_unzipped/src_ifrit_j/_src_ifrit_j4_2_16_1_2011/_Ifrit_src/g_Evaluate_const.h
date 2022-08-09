
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 11.12.2010

/*
 ������� ����� ������� 70%

*/


	// �������� ��� ������� ���������
	const unsigned __int16 KING = 0; // ������  (���� 90000)
	const unsigned __int16 QUEEN = 1200;// �����
	const unsigned __int16 ROOK = 600;// ����� 
	const unsigned __int16 BISHOP = 400;// ���� 
	const unsigned __int16 KNIGHT = 400;// ���� 
	const unsigned __int16 PAWN = 100;// ����� 

	// ������ ����� �����: 0 - 0, 1 - �����, 2 - ����, 
	//3- ����, 4 - �����, 5 - �����, 6 - ������;
	const unsigned __int16 KOD[7] = 
	{
		0,PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING, 
	};


	// ������������ ������
	const unsigned __int8 PENALTY_CASTLING_O_O   = 50;// ����� �� �������� ���������
	const unsigned __int8 PENALTY_CASTLING_O_O_O = 50;// ����� �� ������� ���������
	const unsigned __int8 PENALTY_SHIELD = 20;// ����� �� ��������� ������ ������� ������
	const unsigned __int8 PENALTY_PAWNN = 10;// ����� �� ��������� �����
	const unsigned __int8 PERSPECTIVE_PAWNN = 10;// ��������� ����� ����� �� ������ ��������� (���� 3)

	const unsigned __int8 CONTROL_SQUARE = 1;// ����������� ������� ������������ ������� � �������
	const unsigned __int8 ATAK_KING = 10;// ����������� ����� �� ������(���� �� ���������)

	// ��� ������ ����� ����� ������� ������ � �������� ����� ����� ������
	const unsigned __int16 RUBIKON = QUEEN + ROOK
		+ KING;//����� �������� � ��������(��� ����� � ���� � �����)1600
						// ��� ������ ��������� � ����� ������ �� ��������   
	//const unsigned __int16 rubikon = 0;// test

	// ��������� ��� ������������ ���������� ������ pst
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
	����� � ��������� ������ ��������� ��� ��� ������ �� �1 �.�. from = 7
	test[from] = 7

	�.�. ���� ������ � ��������� ��������� ������������ ���� ��� �� ����� �����
	��� �� ����� ��� ���������� ��������� ������ �� ����� � �������� ������� ���������
	����� ������� ��� ����� ������� ��������� �� �����.

	���� ������ ������� ��� ������� �������, ��� �� ������ ����� ������� ������ 
	� ������ ����� :)
 
	*/


// ������

	// ������ ������ ���������� ����������
	const __int8 pozicion_king_white[] =
	{
		 30, 30, 30,  0,  0,  0, 30, 30,
		-15,-15,-15,-10,-10,-10,-15,-15,
		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,

		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,
	};

	const __int8 pozicion_king_black[] =
	{
		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,

		-20,-20,-20,-20,-20,-20,-20,-20,
		-20,-20,-20,-20,-20,-20,-20,-20,
		-15,-15,-15,-10,-10,-10,-15,-15,
		 30, 30, 30,  0,  0,  0, 30, 30
	};

	// � �������� ������ ������ ���������� � ������
	const __int8 pozicion_king_white_end[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 2, 2, 2, 2, 1, 0,
		0, 1, 2, 3, 3, 2, 1, 0,

		0, 1, 2, 3, 3, 2, 1, 0,
		0, 1, 2, 2, 2, 2, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	};

	const __int8 pozicion_king_black_end[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 2, 2, 2, 2, 1, 0,
		0, 1, 2, 3, 3, 2, 1, 0,

		0, 1, 2, 3, 3, 2, 1, 0,
		0, 1, 2, 2, 2, 2, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	};

// �����
// �����
// ��� ����� � ����� ������ ������� �� �����

// ����

	// �������� ������
	// + ����� ������ ���������� �������������� ������� ���������
	// �������� ��������� ����� ���� ���� ��������������� �����
	const __int8 pozicion_bishop_white[] =
	{
		11,  0,  0,  0,  0,  0,  0, 11,
		 0, 13, 11, 11, 11, 11, 13,  0,
		 0, 11, 12, 12, 12, 12, 11,  0,
		 0, 12, 13, 14, 14, 13, 12,  0,

		 0, 12, 13, 14, 14, 13, 12, 10,
		 0, 11, 12, 12, 12, 12, 11, 10,
		 0, 13, 11, 11, 11, 11, 13, 10,
		11,  0,  0,  0,  0,  0,  0, 11
	};

	const __int8 pozicion_bishop_black[] =
	{
		11,  0,  0,  0,  0,  0,  0, 11,
		 0, 13, 11, 11, 11, 11, 13,  0,
		 0, 11, 12, 12, 12, 12, 11,  0,
		 0, 12, 13, 14, 14, 13, 12,  0,

		 0, 12, 13, 14, 14, 13, 12,  0,
		 0, 11, 12, 12, 12, 12, 11,  0,
		 0, 13, 11, 11, 11, 11, 13,  0,
		11,  0,  0,  0,  0,  0,  0, 11
	};

// ����

	// ��� ����� � ������ ��� ����� ����� ����
	// �� ������ ���������� �� �� ���� ����� �� ������� ������
	const __int8 pozicion_knigh_white[] =
	{
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0, 11, 11, 11, 11, 11, 11,  0,
		 0, 11, 12, 12, 12, 12, 11,  0,
		 0, 11, 12, 13, 13, 12, 11,  0,

		11, 12, 13, 14, 14, 13, 12, 11,
		11, 12, 13, 13, 13, 13, 12, 11,
		11, 12, 12, 12, 12, 12, 12, 11,
		11, 11, 11, 11, 11, 11, 11, 11
	};

	const __int8 pozicion_knigh_black[] =
	{
		11, 11, 11, 11, 11, 11, 11, 11,
		11, 12, 12, 12, 12, 12, 12, 11,
		11, 12, 13, 13, 13, 13, 12, 11,
		11, 12, 13, 14, 14, 13, 12, 11,

		 0, 11, 12, 13, 13, 12, 11,  0,
		 0, 11, 12, 12, 12, 12, 11,  0,
		 0, 11, 11, 11, 11, 11, 11,  0,
		 0,  0,  0,  0,  0,  0,  0,  0
	};


// �����
// ������ ����� ������� ��� ����� ��� ���� ����� � ������ ������� :)

	// ������ � ������ ��� ��� ������ ����� ������ ������������ �������
	const __int8 pozicion_pawn_king_in_center_white[] =
	{
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 15,15, 0, 0, 0,
		0, 0, 0, 16,16, 0, 0, 0,

		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0
	};

	const __int8 pozicion_pawn_king_in_center_black[] =
	{
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0,

		0, 0, 0, 16,16, 0, 0, 0,
		0, 0, 0, 15,15, 0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0,
		0, 0, 0, 0, 0,  0, 0, 0
	};

	// ����� �������� ��������� �������� � �������� ����� � ������� ������
	const __int8 pozicion_pawn_king_in_oo_white[] =
	{
		0,  0,  0,  0,  0, 0, 0, 0,
		0,  0,  0,  0,  0, 0, 0, 0,
		1,  1,  1, 15, 15, 0, 0, 0,
		2,  2,  2, 16, 16, 0, 0, 0,

		3,  3,  3, 17, 17, 0, 0, 0,
		4,  4,  4, 18, 18, 0, 0, 0,
		5,  5,  5, 19, 19, 0, 0, 0,
		0,  0,  0,  0,  0, 0, 0, 0
	};

	const __int8 pozicion_pawn_king_in_oo_black[] =
	{
		 0,  0,  0,  0,  0, 0, 0, 0,
		 5,  5,  5, 19, 19, 0, 0, 0,
		 4,  4,  4, 18, 18, 0, 0, 0,
		 3,  3,  3, 17, 17, 0, 0, 0,

		 2,  2,  2, 16, 16, 0, 0, 0,
		 1,  1,  1, 15, 15, 0, 0, 0,
		 0,  0,  0,  0,  0, 0, 0, 0,
		 0,  0,  0,  0,  0, 0, 0, 0
	};


	// ����� ������� ��������� �������� � �������� ����� � ������� ������
	const __int8 pozicion_pawn_king_in_ooo_white[] =
	{
		0, 0, 0,  0,  0,  0,  0,  0,
		0, 0, 0,  0,  0,  0,  0,  0,
		0, 0, 0, 15, 15,  1,  1,  1,
		0, 0, 0, 16, 16,  2,  2,  2,

		0, 0, 0, 17, 17,  3,  3,  3,
		0, 0, 0, 18, 18,  4,  4,  4,
		0, 0, 0, 19, 19,  5,  5,  5,
		0, 0, 0,  0,  0,  0,  0,  0
	};

	const __int8 pozicion_pawn_king_in_ooo_black[] =
	{
		0, 0, 0,  0,  0,  0,  0,  0,
		0, 0, 0, 19, 19,  5,  5,  5,
		0, 0, 0, 18, 18,  4,  4,  4,
		0, 0, 0, 17, 17,  3,  3,  3,

		0, 0, 0, 16, 16,  2,  2,  2,
		0, 0, 0, 15, 15,  1,  1,  1,
		0, 0, 0,  0,  0,  0,  0,  0,
		0, 0, 0,  0,  0,  0,  0,  0
	};


	//����� ������ �� �����������. ���������� ��� ��������
	const __int8 pozicion_pawn_white_end[] =
	{
		0,   0,  0,  0,  0,  0,  0,  0,
		0,   0,  0,  0,  0,  0,  0,  0,	           
		10, 10, 10, 10, 10, 10, 10, 10,           
		15, 15, 15, 15, 15, 15, 15, 15,

		20, 20, 20, 20, 20, 20, 20, 20,
		25, 25, 25, 25, 25, 25, 25, 25,
		50, 50, 50, 50, 50, 50, 50, 50,
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	const __int8 pozicion_pawn_black_end[] =
	{
		0,  0,  0,  0,  0,  0,  0,  0,
		50, 50, 50, 50, 50, 50, 50, 50,     
		25, 25, 25, 25, 25, 25, 25, 25,
		20, 20, 20, 20, 20, 20, 20, 20,

		15, 15, 15, 15, 15, 15, 15, 15,
		10, 10, 10, 10, 10, 10, 10, 10,  
		 0,  0,  0,  0,  0,  0,  0,  0,	
		 0,  0,  0,  0,  0,  0,  0,  0
	};

	//����� ��������� ������ �� �����������
	const __int16 pozicion_perspective_pawn_white_end[] =
	{
		 0,    0,   0,   0,   0,   0,   0,   0,
		 0,    0,   0,   0,   0,   0,   0,   0,	           
		 20,  20,  20,  20,  20,  20,  20,  20,
		 30,  30,  30,  30,  30,  30,  30,  30,

		 40,  40,  40,  40,  40,  40,  40,  40,
		 50,  50,  50,  50,  50,  50,  50,  50,
		100, 100, 100, 100, 100, 100, 100, 100,
		  0,   0,   0,   0,   0,   0,   0,   0
	};

	const __int16 pozicion_perspective_pawn_black_end[] =
	{
		  0,   0,   0,   0,   0,   0,   0,   0,
		100, 100, 100, 100, 100, 100, 100, 100,     
		 50,  50,  50,  50,  50,  50,  50,  50,
		 40,  40,  40,  40,  40,  40,  40,  40,

		 30,  30,  30,  30,  30,  30,  30,  30,
		 20,  20,  20,  20,  20,  20,  20,  20,
		 0,    0,   0,   0,   0,   0,   0,   0,	          
		 0,    0,   0,   0,   0,   0,   0,   0          
	};
