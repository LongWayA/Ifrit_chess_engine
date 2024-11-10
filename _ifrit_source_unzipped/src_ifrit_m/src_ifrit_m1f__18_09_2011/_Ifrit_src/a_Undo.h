
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 5.09.2011
/// ������� ����� ������� 99%


#ifndef A_UNDO_H
#define A_UNDO_H

/// ��� ��������� ����� �����������(��������) ����� ��� 
/// �������������� ������� ����� ����
struct Undo
{
	/// ���� �������� ������ �� ������� 1/0
	bool return_taking_passage;

	/// � ���������� ������ ����(������������ �� �����) 
	/// ��� '�' - 0 , 'h' - 7 ����� ������� �� � ����������� ������
	unsigned __int8  taking_passage_x;

	/// � ���������� ������ ����
	/// ��� '1' - 0 , '8' - 7 ����� ������� ����������� ���� ������ ������ �������� �� 1
	unsigned __int8  taking_passage_y;

	/// ���� ��������� ����� � ������� �������
	bool return_castling_Q;

	/// ���� ��������� ����� � �������� �������
	bool return_castling_K;

	/// ���� ��������� ������ � ������� �������
	bool return_castling_q;

	/// ���� ��������� ������ � �������� �������
	bool return_castling_k;

};

#endif // !defined A_LIST_H