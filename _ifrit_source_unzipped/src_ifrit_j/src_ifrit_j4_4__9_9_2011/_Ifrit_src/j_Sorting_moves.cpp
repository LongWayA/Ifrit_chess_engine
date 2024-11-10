
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 20.11.2010

#include <fstream>
#include <iostream>

#include "j_Sorting_moves.h"

//	===================================================================
// ��������� ������
void j_Sorting_moves::sorting_moves_captures
(
	struct List & list// ������������� ������ �����
)
{
	__int32 move_s;// ���������� ��� ����������� ����
	__int32 score_s;// ���������� ��� ����������� ������ ����
	//unsigned __int8 end_captures = end_captures;//���������� ������
	//unsigned __int8 end_list = end_list;//���������� �����

	/*
	������ � ��� ���������� � 0 � ����� ���
	0 ������
	1 ������
	2 ������
	3 ���          end_captures
	�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
	4 ���
	5 ���
	6 ���
	7              end_list
	���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

	������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����

	*/

	// 1 ��������� ������----------------------------------
	/*
	���� � ��� ���� ������ ����� �� �� ��������� �� ������ ����������� ��� � ���������� ������
	���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
	������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
	���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
	����� ������� ����� �� ������� �� ����� ��� ������ � ��� ������������� �� ���������.
	*/
	for (unsigned __int8 k = 0; k < list.end_captures; k++)
	{	
		for (unsigned __int8 i = k + 1; i < list.end_captures; i++)
		{
			if (list.sorting_score[k] < list.sorting_score[i])
			{ 
				//��������� ������   
				move_s = list.move[k];
				score_s = list.sorting_score[k];

				//�������� �� i � k
				list.move[k] = list.move[i];
				list.sorting_score[k] = list.sorting_score[i];

				//��������������� ������
				list.move[i] = move_s;
				list.sorting_score[i] = score_s;
			}
		}
	}

}

//	===================================================================
// ������ ���������� ��������� ������
void j_Sorting_moves::sorting_moves_history
(
	struct List & list// ������������� ������ �����
)
{
	__int32 move_s;// ���������� ��� ����������� ����
	__int32 score_s;// ���������� ��� ����������� ������ ����
	//unsigned __int8 end_captures = end_captures;//���������� ������
	//unsigned __int8 end_list = end_list;//���������� �����


	/*
	������ � ��� ���������� � 0 � ����� ���
	0 ������
	1 ������
	2 ������
	3 ���          end_captures
	�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
	4 ���
	5 ���
	6 ���
	7              end_list
	���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

	������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����
	*/

	// 1 ��������� �������� ����----------------------------------
	/*
	�� �� ��������� �� ������ ����������� �� ���������� ��������
	���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
	������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
	���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
	����� ������� ����� �� ������� �� ����� ��� ���� � ��� ������������� �� ���������.
	*/
	for (unsigned __int8 k = list.end_captures; k < list.end_list; k++)
	{	
		for (unsigned __int8 i = k + 1; i < list.end_list; i++)
		{
			if (list.sorting_score[k] < list.sorting_score[i])
			{ 
				// ��������� ������   
				move_s = list.move[k];
				score_s = list.sorting_score[k];

				// �������� �� i � k
				list.move[k] = list.move[i];
				list.sorting_score[k] = list.sorting_score[i];

				// ��������������� ������
				list.move[i] = move_s;
				list.sorting_score[i] = score_s;
			}
		}
	}

}


//	===================================================================
// ������ ���������� ��������� ������
void j_Sorting_moves::sorting_white_moves_root
(
	struct List & list// ������������� ������ �����
)
{
	__int32 move_s;// ���������� ��� ����������� ����
	__int32 score_s;// ���������� ��� ����������� ������ ����
	//unsigned __int8 end_captures = end_captures;//���������� ������
	//unsigned __int8 end_list = end_list;//���������� �����


	/*
	������ � ��� ���������� � 0 � ����� ���
	0 ������
	1 ������
	2 ������
	3 ���          end_captures
	�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
	4 ���
	5 ���
	6 ���
	7              end_list
	���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

	������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����
	*/

	// 1 ��������� �������� ����----------------------------------
	/*
	�� �� ��������� �� ������ ����������� �� ���������� ��������
	���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
	������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
	���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
	����� ������� ����� �� ������� �� ����� ��� ���� � ��� ������������� �� ���������.
	*/
	for (unsigned __int8 k = 0; k < list.end_list; k++)
	{	
		for (unsigned __int8 i = k + 1; i < list.end_list; i++)
		{
			if (list.sorting_score[k] < list.sorting_score[i])
			{ 
				// ��������� ������   
				move_s = list.move[k];
				score_s = list.sorting_score[k];

				// �������� �� i � k
				list.move[k] = list.move[i];
				list.sorting_score[k] = list.sorting_score[i];

				// ��������������� ������
				list.move[i] = move_s;
				list.sorting_score[i] = score_s;
			}
		}
	}

}

//	===================================================================
// ������ ���������� ��������� ������
void j_Sorting_moves::sorting_black_moves_root
(
	struct List & list// ������������� ������ �����
)
{
	__int32 move_s;// ���������� ��� ����������� ����
	__int32 score_s;// ���������� ��� ����������� ������ ����
	//unsigned __int8 end_captures = end_captures;//���������� ������
	//unsigned __int8 end_list = end_list;//���������� �����


	/*
	������ � ��� ���������� � 0 � ����� ���
	0 ������
	1 ������
	2 ������
	3 ���          end_captures
	�.�. ���������� ������ �� 1 ��������� �������� ������ ���������� ������
	4 ���
	5 ���
	6 ���
	7              end_list
	���������� ��� � ��� ��������� ���������� ������ � ����� �.�. ���� ������� � 1

	������� ������ � ����� � ������ ��������� ������ ���������� ������ � ����
	*/

	// 1 ��������� �������� ����----------------------------------
	/*
	�� �� ��������� �� ������ ����������� �� ���������� ��������
	���� �� ������ � ����������(������ �� ���) ������� ������ �� ����� ������������
	������� ��� ���� � ��� ������� ������ k �� ��������� ������ ����� k + 1
	���� �����(�.�. ������ �������) ������ ������� �� �� ������ ������� �� � ������� ������
	����� ������� ����� �� ������� �� ����� ��� ���� � ��� ������������� �� ���������.
	*/
	for (unsigned __int8 k = 0; k < list.end_list; k++)
	{	
		for (unsigned __int8 i = k + 1; i < list.end_list; i++)
		{
			if (list.sorting_score[k] > list.sorting_score[i])
			{ 
				// ��������� ������   
				move_s = list.move[k];
				score_s = list.sorting_score[k];

				// �������� �� i � k
				list.move[k] = list.move[i];
				list.sorting_score[k] = list.sorting_score[i];

				// ��������������� ������
				list.move[i] = move_s;
				list.sorting_score[i] = score_s;
			}
		}
	}
}