
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ���������  26.05.2012
/// ������� ����� ������� 40%

#ifndef GO_PARAMETERS_H
#define GO_PARAMETERS_H

struct Go_parameters 
{
	//> go btime 300000 wtime 300000 
	double wtime;     /// ���������� ����� �� ���� �� �����
	double btime;     /// ���������� ����� �� ���� �� ������

	//> go btime 302000 wtime 302000 winc 2000 binc 2000 
	double winc;      ///  ���������� ������� �� ��� ��� �����
	double binc;      ///  ���������� ������� �� ��� ��� ������

	//> go wtime 300000 btime 300000  movestogo 40
	__int8 movestogo; /// ���������� ����� �� ������������ �����

	//> go depth 5 
	__int8 depth; /// ������� ��������

	//> go movetime 300000 
	double movetime;  /// ����� �� ���

	//> go infinite 
	bool infinite;    /// ���� ������������ �������
};

#endif // !defined GO_PARAMETERS_H