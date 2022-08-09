
/// ����� �������� ������ (Brenkman Andrey)
/// ��������� ��������� 27.11.2010 17.08.2011
/// ������� ����� ������� 99%


#ifndef HEADER_CONSTANTS_H
#define HEADER_CONSTANTS_H


/// ��������� ������� ��������� � ������������ ������.
namespace HEADER_CONSTANTS_CONSTANTS
{
	/// ������ ����� ������ ������
	const unsigned __int16 LENGTH_STRING = 65534;

	/// + 1 - ������ ��� ������� ������ ������� ���� ���������� 
	/// � ���������� ��������: 
	///���������� �������� = ����������� ��������� ������ + 1

	/// �������� �� ��� ����� 6 + 1
	const unsigned __int8 MAX_NAME = 7;

	/// �������� ���������� ������ 63 + 1
	const unsigned __int8 MAX_COORDINATE = 64;

	/// ����������� �������������� ����� �������� � ��� �� ������� ������ 100 + 1
	const unsigned __int8 MAX_DEPTH = 101;

	/// ����������� �������������� ���������� ����� �� �������
	const unsigned __int16 MAX_MOVE = 256;
}

using namespace HEADER_CONSTANTS_CONSTANTS;

#endif // !defined HEADER_CONSTANTS_H

/// ��� �������

/// unsigned __int8 : 0 to 255
///          __int8 : -128 to 127

/// unsigned __int16 :  0 to 65535
///          __int16 : -32768 to 32767

/// unsigned __int32 : 0 to 4294967295
///          __int32 : -2147483648 to 2147483647

/// unsigned __int64 : 0 to 
///          __int64 : - to 




