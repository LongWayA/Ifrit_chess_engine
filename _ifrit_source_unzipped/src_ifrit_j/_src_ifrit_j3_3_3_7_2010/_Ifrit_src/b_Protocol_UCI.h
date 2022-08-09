#include "a_Structure.h"

#define TEST_P 0 // ��������-���������(1-0)  ����� ������ � ���� �������� ����������
// �� �������� ��� ���� �������� � ������ ���������(bit board) TEST_B � ����� d_Bitboard_gen.h TEST_B

// ��� � ���� ������� ������
//#define NAME_OF_PROGRAM_AND_DATA_OF_RELEASE "id name Ifrit_j3_3full_Beta_29_6_2010"
#define NAME_OF_PROGRAM_AND_DATA_OF_RELEASE "id name Ifrit_j3_3_Beta_3_7_2010"

//-----------------------------------------------------------------------------------
/* ++
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 12.06.2007  11.11.2009
*/
//-----------------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

 ������������ ������ � ������ ��������� UCI

 ��������� �������� UCI ������� �� ����������� ��������� ��������
 ��������� ��� �������� ��� �����������
 ��� ���� � ������ �������� ������ ����� ������������� ����� bitboard(bit board) 
 �.�. ���������� ���������� ���� �� �����
 � ������ �� �������������� ������ �������� �������� �������
 // ��������� �������� ������� �������������� ���������� ����� ������   
    parse_position(bitboard,string_in); 
 ����������� �������� ������ �����  bitboard(bit board) 

 � �� ������ �� ��������� ������ �� �����������
 // ��������� ������ �� ������ �������
    parse_go(bitboard,string_in,three_position);
 � ���������� � ����� ������� ��� � �������

 ������� ������������� ����� ������ ���� ������������ 
 ��� ������� � ����� a_Structure.h
*/

namespace PROTOCOLUCI
{
	// ��������� �������� UCI ������� �� ����������� ��������� ��������
	void public_parse_protokol_UCI
	(
		struct T_Bitboard * p_bitboard,//����� 
		char string_in[]//�������� ������
	);

	//////////////////////////////////////////////////////////////////////////////////////
	//private:

	//-----------------------------------------------------------------------------------
	// ��������� � ������ � ������ ������ ���� � ����������
	unsigned __int16 read_hash_size_Mb
	(
		const char string_in[]
	);

	//-----------------------------------------------------------------------------------
	// ���� �� � ������ ������� ���������
	bool string_equal
	(
		const char s1[], 
		const char s2[]
	);

	//-----------------------------------------------------------------------------------
	// ���� �� � ������ ������ ������� ���������
	bool string_start_with
	(
		const char s1[],
		const char s2[]
	);

}


