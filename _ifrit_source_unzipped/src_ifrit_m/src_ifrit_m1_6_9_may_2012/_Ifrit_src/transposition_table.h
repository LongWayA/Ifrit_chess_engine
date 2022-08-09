
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  9.03.2009 20.1.2010
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 40%



///!
#include "move_list_struct.h"


/// включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define TEST_H 0 


/// включаем-выключаем(1-0) режим подсчета количества
/// записанных и считанных позиций
/// надо еще включить флаг TEST_WRITE_READ_WN в Go.h
#define TEST_WRITE_READ 0 
                          
                         
/*
Transposition Table
http://chessprogramming.wikispaces.com/Transposition+Table

 A Transposition Table,
 first used in Greenblatt's program Mac Hack [1] [2] , 
 is a database that stores results of previously performed 
 searches. It is a way to greatly reduce the search space 
 of a chess tree with little negative impact. Chess programs, 
 during their brute-force search, encounter the same positions 
 again and again, but from different sequences of moves, which 
 is called a transposition.

 Transposition (and refutation) tables are techniques derived from 
 dynamic programming [3] , a term coined by Richard E. Bellman in 
 the 1950s, when programming meant planning, and dynamic programming 
 was conceived to optimally plan multistage processes [4] .

How it works
 When the search encounters a transposition, it is beneficial to 
 'remember' what was determined last time the position was examined, 
 rather than redoing the entire search again. For this reason, chess 
 programs have a transposition table, which is a large hash table 
 storing information about positions previously searched, how deeply 
 they were searched, and what we concluded about them. Even if the 
 depth (draft) of the related transposition table entry is not big 
 enough, or does not contain the right bound for a cutoff, a best 
 (or good enough) move from a previous search can improve move ordering, 
 and save search time. This is especially true inside an iterative 
 deepening framework, where one gains valuable table hits from 
 previous iterations.

Hash functions
 Hash functions convert chess positions into an almost unique, 
 scalar signature, allowing fast index calculation as well as 
 space saving verification of stored positions.
  Zobrist Hashing


 реализуем хеш-таблицу
 работаем с хеш-таблицей и памятью под нее
*/


/// реализуем кэширование лучшего хода в данной позиции
struct Hash_moves 
{

	/// хеш ключ позиции (верхняя половина high 32 bits)
	/// используем верхние биты 64 битного хеш - ключа(high 32 bits)
	unsigned __int32 key_lock_32_move;

	/// описание хода смотреть в структуре T_list_surplus_moves
	/// в ходе много запаковано так что меньше чем 32 брать нельзя
	__int32 move; 

	/// глубина варианта при которой получен данный ход
	unsigned __int8 move_delta_depth;

	/// возраст записанного хода. нужен что бы перезаписывать элементы от прошлых позиций
	unsigned __int8 move_age;


	/// кешируем оценку отсечка по бете(альфе) и не улучшили альфу(бету)

	/// хеш ключ позиции (верхняя половина high 32 bits)
	/// используем верхние биты 64 битного хеш - ключа(high 32 bits)
	unsigned __int32 key_lock_32_score;

	/// оценка
	/// видимо тут можно будет ужаться до 16 но не сейчас
	__int32 score;

	/// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 flag_hash_score;

	/// глубина варианта при которой получена данная оценка
	unsigned __int8 score_delta_depth;

	/// возраст записанной оценки. нужен что бы перезаписывать элементы от прошлых позиций
	unsigned __int8 score_age;

};



namespace Transposition_table
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:

#if TEST_WRITE_READ

	/// тестируем запись в хеш таблицу

	/// общее количество записанных позиций
	unsigned __int32 hash_write_all;

	/// количество записанных позиций для сортировки ходов
	unsigned __int32 hash_move_write;

	/// количество записанных позиций для отсечки по оценке (LOWER // отсечка по бете(альфе))
	unsigned __int32 hash_score_write_low;

	/// количество записанных позиций для отсечки по оценке (UPPER 3 альфу(бету) улучшить не удалось)
	unsigned __int32 hash_score_write_high;


	/// тестируем использование кеша

	/// используем для сортировки ходов
	unsigned __int32 hash_read_sorting; 

	/// используем для отсечки по оценке. общее количество
	unsigned __int32 hash_read_score; 

	/// используем для отсечки по оценке. LOWER
	unsigned __int32 hash_read_score_LOWER; 

	/// используем для отсечки по оценке. UPPER 
	unsigned __int32 hash_read_score_UPPER; 

#endif // TEST_WRITE_READ


//////////////////////////////////////////////////////////////////////////////////////
//public:

/// ТЕСТ ХЕШ - ТАБЛИЦЫ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TEST_WRITE_READ

	///---------------------------------------------------------------------------
	/// обнуляем счетчики
	void test_hash_ini();

	///---------------------------------------------------------------------------
	/// печатаем счетчики
	void test_hash_print();

#endif // TEST_WRITE_READ
/// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/// РАБОТА С ХЕШ - ТАБЛИЦЕЙ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	///---------------------------------------------------------------------------
	/// выделяем память под хеш-таблицу. размер задается в мегабайтах
	void hash_size_ini_default
	(
		unsigned __int16 hash_size/// размер хеш-таблицы в мегабайтах
	);

	///---------------------------------------------------------------------------
	/// освобождаем память из под хеш-таблицы и по новой выделяем память под хеш-таблицу.
	/// размер задается в мегабайтах
	void hash_size_resize
	(
		unsigned __int16 hash_size/// размер хеш-таблицы в мегабайтах
	);

	///---------------------------------------------------------------------------
	/// освобождаем память из под хеш-таблицы
	void hash_size_free();

	///---------------------------------------------------------------------------
	/// инициализируем все ключи в хеш-таблице 0
	void ini_hash_moves();

	///---------------------------------------------------------------------------
	/// кэшируем лучший на данный момент ход или оценку (зависит от поиска)
	void save_best_hash_score_or_move
	(
		unsigned __int8 i,/// номер хода по списку
		unsigned __int64 key,/// хеш-ключ
		const struct List & list_surplus_moves,///список возможных ходов
		__int32 score,/// оценка хода
		unsigned __int8 flag_hash_score,/// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
		unsigned __int8 depth,/// глубина записи хода
		unsigned __int8 depth_max/// максимальная глубина расчета
	);

	///---------------------------------------------------------------------------
	/// смотрим не встречалась ли нам такая позиция. если встречалась 
	/// то применяем сохраненную оценку
	/// функцию используем для отсечки по оценке из хеш-таблицы
	__int32 look_hash_score_white
	(
		const unsigned __int64 key,/// хеш-ключ
		const unsigned __int8 depth,/// глубина
		const unsigned __int8 depth_max,/// максимальная глубина
		const __int32 alpha,///
		const __int32 beta,///
		unsigned __int8 & test_flag_hash_moves,/// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
		unsigned __int8 & hash_depth,/// максимальная глубина
		unsigned __int8 & hash_depth_max/// максимальная глубина
	);

	///---------------------------------------------------------------------------
	/// смотрим не встречалась ли нам такая позиция если встречалась 
	/// то применяем сохраненную оценку
	/// функцию используем для отсечки по оценке из хеш-таблицы
	__int32 look_hash_score_black
	(
		const unsigned __int64 key,/// хеш-ключ
		const unsigned __int8 depth,/// глубина
		const unsigned __int8 depth_max,/// максимальная глубина
		const __int32 alpha,///
		const __int32 beta,///
		unsigned __int8 & test_flag_hash_moves,/// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
		unsigned __int8 & hash_depth,/// максимальная глубина
		unsigned __int8 & hash_depth_max/// максимальная глубина
	);

	///---------------------------------------------------------------------------
	/// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	void look_hash_move
	(
		struct List & list_surplus_moves,///список возможных ходов
		const unsigned __int64 key,/// хеш - ключ
		const unsigned __int8 depth,/// глубина
		unsigned __int8  & flag_insert_hash_move///флаг того что удалось найти хеш-ход и поднять на первую позицию
	);

	///---------------------------------------------------------------------------
	/// обнуляем возраст позиции
	void set_age_0();

	///---------------------------------------------------------------------------
	/// увеличиваем возраст позиции
	void aging();


//////////////////////////////////////////////////////////////////////////////////////
//private:

	/// РАБОТА С КЕШ - ТАБЛИЦЕЙ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	///---------------------------------------------------------------------------
	/// кэшируем лучший на данный момент ход 
	inline void save_best_hash_move
	(
		unsigned __int8 i,/// номер хода по списку
		unsigned __int32 key_lock_32,///
		const struct List & list_surplus_moves,///список возможных ходов
		unsigned __int8 delta_depth,/// глубина записи хода
		unsigned __int32 ind///
	);

	///---------------------------------------------------------------------------
	/// кэшируем ход т.е. записываем ход в хеш-таблицу
	inline void save_hash_move
	(
		unsigned __int8 i,/// номер хода по списку
		unsigned __int32 key_lock_32,///
		const struct List & list_surplus_moves,///список возможных ходов
		unsigned __int8 delta_depth,/// глубина записи хода
		unsigned __int32 ind///  
	);

	///---------------------------------------------------------------------------
	/// кэшируем лучшую на данный момент оценку
	inline void save_best_hash_score
	(
		unsigned __int32 key_lock_32,///
		__int32 score,/// оценка хода
		unsigned __int8 flag_hash_score,/// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
		unsigned __int8 delta_depth,/// глубина записи хода
		unsigned __int32 ind///
	);

	///---------------------------------------------------------------------------
	/// кэшируем оценку т.е. записываем ее в хеш-таблицу
	inline void save_hash_score
	(
		unsigned __int32 key_lock_32,///
		__int32 score,/// оценка хода
		unsigned __int8 flag_hash_score,/// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
		unsigned __int8 delta_depth,/// глубина записи хода
		unsigned __int32 ind///  
	);

	///---------------------------------------------------------------------------
	/// тестируем хеш-таблицу. печатаем полное количество элементов 
	/// сколько использовали а сколько пустых
	void TEST_Hash_index_print();


//////////////////////////////////////////////////////////////////////////////////////
//public:

	///---------------------------------------------------------------------------
	/// поднимаем хеш-ход на первую позицию в списке ходов насчитанных для данной позиции
	void insert_hash_moves
	(
		struct List & list,/// представление списка ходов
		const __int32 hash_moves,//
		unsigned __int8  & flag_insert_hash_move///флаг того что удалось найти хеш-ход и поднять на первую позицию
	);


};
