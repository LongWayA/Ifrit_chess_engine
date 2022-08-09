
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 27.11.2010 17.08.2011
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 99%


#ifndef HEADER_CONSTANTS_H
#define HEADER_CONSTANTS_H


/// Константы которые использую в заголовочных файлах.
namespace HEADER_CONSTANTS_CONSTANTS
{
	/// задаем длину буфера потока
	const unsigned __int16 LENGTH_STRING = 65534;

	/// + 1 - потому что нулевой индекс массива тоже используем 
	/// в результате получаем: 
	///количество индексов = максимально доступный индекс + 1

	/// максимум на имя фигур 6 + 1
	const unsigned __int8 MAX_NAME = 7;

	/// максимум координаты фигуры 63 + 1
	const unsigned __int8 MAX_COORDINATE = 64;

	/// максимально поддерживаемая длина варианта и она же глубина поиска 100 + 1
	const unsigned __int8 MAX_DEPTH = 101;

	/// максимально поддерживаемое количество ходов из позиции
	const unsigned __int16 MAX_MOVE = 256;
}

using namespace HEADER_CONSTANTS_CONSTANTS;

#endif // !defined HEADER_CONSTANTS_H

/// для справки

/// unsigned __int8 : 0 to 255
///          __int8 : -128 to 127

/// unsigned __int16 :  0 to 65535
///          __int16 : -32768 to 32767

/// unsigned __int32 : 0 to 4294967295
///          __int32 : -2147483648 to 2147483647

/// unsigned __int64 : 0 to 
///          __int64 : - to 




