
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 5.09.2011
/// ������� ����� ������� 99%

#ifndef MOVE_LIST_H
#define MOVE_LIST_H

//!
#include "b_constants_header.h"

/*
Move List

*/


/// ������ ���������� �����. �.�. ����� ����� ���� ������ ������ � ���� ��� ���.
struct List
{
	/// � ����� ����� ������� move �� ��������� ��������� ���������� (���������� 27 ���)
	/// --3 ���--      --6 ���--   --6 ���--     --6 ���--  --6 ���-- 
	/// ������� ������  ���� �����  ������ �����  ��� ����   ������ ������

	///-----------------------------------------------
	/// ���� ������ �.�. � �������� ����
	/// --6 ���-- 
	/// ������ ������

	/// 0 - ��� ������
	/// 1 - ����� 
	/// 2 - ���� 
	/// 3 - ����
	/// 4 - ����� 
	/// 5 - ����� 
	/// 6 - ������

	/// ������ move = ��� ������
	/// ������ move & 63 ( 63 ��� ...000111111)

	///-------------------------------------------------
	/// --6 ���--
	/// ��� ����

	/// 0 - ��� ����
	/// 1 � ������� ���
	/// 2 � ������
	/// 3 � ������� ���������
	/// 4 � �������� ���������
	/// 5 � ������ �� �������

	/// 12 - ����������� � ���� 
	/// 13 - ����������� � ����
	/// 14 - ����������� � ����� 
	/// 15 - ����������� � ����� 

	/// 22 - ������ � ������������ � ���� 
	/// 23 - ������ � ������������ � ����
	/// 24 - ������ � ������������ � ����� 
	/// 25 - ������ � ������������ � ����� 
 
	/// ������ move = move | (��� ���� << 6)
	/// ������ (move>>6) & 63

	///-------------------------------------------------
	/// --6 ���--
	/// ������ ����� (0 -63)
	/// ������ move = move | (������ ����� << 12)
	/// ������ (move>>12) & 63

	///-------------------------------------------------
	/// --6 ���--
	/// ���� ����� (0 -63)
	/// ������ move = move | (���� ����� << 18)
	/// ������ (move>>18) & 63

	///-------------------------------------------------
	/// --3 ���-- 
	/// ������� ������
	/// 0 - ��� ������
	/// 1 - ����� 
	/// 2 - ���� 
	/// 3 - ����
	/// 4 - ����� 
	/// 5 - ����� 
	/// 6 - ������
	/// ������ move = move | (���� ����� << 24)
	/// ������ (move>>24) & 7 ( 7 ��� ...000111)

	__int32 move[MAX_MOVE];

	/// ������ ������������ ��� ���������� �����
	__int32 sorting_score[MAX_MOVE];

	/// ����� ������ � ������ ������ ������ �� ������ �.� ���������� ������ 
	/// (������ ��� ������ � ��� ���������� � ����)
	unsigned __int8 end_captures;

	/// ����� ������ ������ ������ ������ ������� �.�. ���������� �����
	unsigned __int8 end_list;

};

#endif // !defined MOVE_LIST_H






