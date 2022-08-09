#include <iostream>
#include <fstream>  // TEST

#include "j_Hash.h"
#include "k_Util.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  9.03.2009 20.1.2010
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 40%

 реализуем хеш-таблицу
 работаем с хеш-таблицей и памятью под нее
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 std::ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


// ТЕСТ ХЕШ - ТАБЛИЦЫ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TEST_WRITE_READ

//	===================================================================
// обнуляем счетчики
 void HASHM::test_hash_ini()
 {
	// тестируем запись в хеш таблицу--------------------------------------

	// общее количество записанных позиций
	hash_write_all = 0;

	// количество записанных позиций для сортировки ходов
	hash_move_write = 0;

	// количество записанных позиций для отсечки по оценке
	hash_score_write_low = 0;

	// количество записанных позиций для отсечки по оценке
	hash_score_write_high = 0;

	// тестируем использование записи---------------------------------------- 

	// используем для сортировки ходов
	hash_read_sorting = 0; 

	// используем для отсечки по оценке общее количество
	hash_read_score = 0;  

	hash_read_score_LOWER = 0;

	hash_read_score_UPPER = 0;

 }

//	===================================================================
// печатаем счетчики
void HASHM::test_hash_print()
{
	std::cout << " " << "\n";

	hash_write_all = hash_move_write + hash_score_write_low + hash_score_write_high;

	// печатаем количество всех записанных позиций
	std::cout << "hash_write_all = " << hash_write_all << "\n";

	if (hash_write_all != 0)
	{
		// количество записанных позиций для сортировки ходов
		std::cout << "hash_move_write = " << hash_move_write << "   100 * hash_move_write/hash_write_all = " << 100 * hash_move_write/hash_write_all << "\n";

		// количество записанных позиций для отсечки по оценке
		std::cout << "hash_score_write_low = " << hash_score_write_low << "   100 * hash_score_write_low/hash_write_all = " << 100 * hash_score_write_low/hash_write_all << "\n";

		// количество записанных позиций для отсечки по оценке
		std::cout << "hash_score_write_high = " << hash_score_write_high << "   100 * hash_score_write_high/hash_write_all = " << 100 * hash_score_write_high/hash_write_all << "\n";

		std::cout << " " << "\n";

		// печатаем количество использованных для сортировки позиций
		std::cout << "hash_read_sorting = " << hash_read_sorting << "   100 * hash_read_sorting/hash_write_all = " << 100 * hash_read_sorting/hash_write_all << "\n";
	}

	// дельта между записанными и использованными
	std::cout << "hash_write_all - hash_read_sorting = " << (hash_write_all - hash_read_sorting) << "\n";


	std::cout << " " << "\n";

	hash_read_score = hash_read_score_LOWER + hash_read_score_UPPER;

	if ( hash_read_score != 0)
	{
		if (hash_write_all != 0)
		{
			// печатаем количество всех считанных позиций
			std::cout << "hash_read_score = " << hash_read_score << "   100 * hash_read_score/hash_write_all = " << 100 * hash_read_score/hash_write_all << "\n";

			// печатаем количество всех считанных позиций
			std::cout << "hash_read_all = " << (hash_read_sorting + hash_read_score) << "   100 * hash_read_all/hash_write_all = " << 100 * (hash_read_sorting + hash_read_score)/hash_write_all << "\n";
		}

		// печатаем количество использованных для отсчеки по оценки позиций
		std::cout << "hash_read_score_LOWER = " << hash_read_score_LOWER <<  "   100 * hash_read_score_LOWER/hash_read_score = " << 100 * hash_read_score_LOWER/hash_read_score << "\n";

		// печатаем количество использованных для отсчеки по оценки позиций
		std::cout << "hash_read_score_UPPER = " << hash_read_score_UPPER <<  "   100 * hash_read_score_UPPER/hash_read_score = " << 100 * hash_read_score_UPPER/hash_read_score << "\n";
	}

	std::cout << " " << "\n";
 }

#endif // TEST_WRITE_READ
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// РАБОТА С ХЕШ - ТАБЛИЦЕЙ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//	===================================================================
// выделяем память под кэш. размер задается в мегабайтах
void Hash::hash_size_ini_default
(
	unsigned __int16 hash_size// размер хеш-таблицы в мегабайтах
)
{
	unsigned __int64 hash_n = 0;

	if (hash_size < 1) hash_size = 1;// память не меньше мегабайта
	if (hash_size > 1024) hash_size = 1024;// память не больше гигабайта

	// функция sizeof(struct T_hash_moves) выдает размер в байтах т.е. int это 4
	// в данный момент struct T_hash_moves - 16 байт
	// перегоняем размер в Мб в байты а потом в количество элементов
	max_index_hash = (unsigned __int64)(
		(hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));

	//std::cout << "max_index_hash in 1 mb = " << max_index_hash << "\n";
	//std::cout << "sizeof(T_hash_moves) = " << sizeof(struct T_hash_moves) << "\n";
	//std::cout << "sizeof(unsigned int) = " << sizeof(unsigned int) << "\n";

	// нам нужно получить размер (2 в степени n) -1 что бы получилось 
	// число вида 000..0111 и можно было вгонять хеш - индекс в пределы массива операцией &
	for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

	hash_n = hash_n / 2;

	if ((hash_n & (hash_n - 1)) != 0)
	{
		std::cout << "ЗАСАДА! размер не вида степени двух " << hash_n << "\n";
	}

	max_index_hash = hash_n - 1;

	// calloc
	// Allocates a block of memory for an array of num elements, 
	// each of them size bytes long, and initializes all its bits to zero.
	// резервируем память под таблицу и заполняем ее нулями
	p_hash_moves = (struct T_hash_moves*) calloc (
		(__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

	if (p_hash_moves == NULL) 
	{
		std::cout << "памяти нет " << "\n";
		exit(1); 
	}

	// TEST
	//std::cout << "sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
	//std::cout << "max_index_hash = " << max_index_hash << "\n";
	//std::cout << "hash_n = " << hash_n << "\n";

}

//	===================================================================
// освобождаем память из под хеш-таблицы 
// и по новой выделяем память под кэш. размер задается в мегабайтах
void Hash::hash_size_resize
(
	unsigned __int16 hash_size// размер хеш-таблицы в мегабайтах
)
{
	unsigned __int64 hash_n = 0;

	if (p_hash_moves == NULL) 
	{
		std::cout << "освобождать нечего " << "\n";
		exit(1); 
	}

	free(p_hash_moves);// освобождаем предыдущий объем памяти

	if (hash_size < 1) hash_size = 1;// память не меньше мегабайта
	if (hash_size > 1024) hash_size = 1024;// память не больше гигабайта

	// перегоняем размер в Мб в байты а потом в количество элементов
	max_index_hash = (unsigned __int64)(
		(hash_size * (1024 * 1024)) / sizeof(struct T_hash_moves));

    // TEST
	//std::cout << "предварительный max_index_hash = " << max_index_hash << "\n";
 

	// нам нужно получить размер (2 в степени n) -1 что бы получилось 
	// число вида 000..0111 и можно было вгонять хеш - индекс в пределы массива операцией &
	for (hash_n = 2; hash_n <= max_index_hash; hash_n = 2 * hash_n );

	hash_n = hash_n / 2;

	if ((hash_n & (hash_n - 1)) != 0)
	{
		std::cout << "ЗАСАДА! размер не вида степени двух " << hash_n << "\n";
	}

	max_index_hash = hash_n - 1;

	// резервируем память под таблицу и заполняем ее нулями
	p_hash_moves = (struct T_hash_moves*) calloc (
		(__int32)(max_index_hash + 3), sizeof(struct T_hash_moves));

	if (p_hash_moves == NULL) 
	{
		std::cout << "памяти нет " << "\n";
		exit(1); 
	}

// TEST
	//std::cout << " размер хеш-таблицы в мегабайтах hash_size = " << hash_size << "\n";
	//std::cout << " в итоге выделили памяти в мб = " << ((int)(max_index_hash + 3) * sizeof(T_hash_moves))/(1024 * 1024) << "\n";
	//std::cout << "размер элемента в байтах sizeof(T_hash_moves) = " << sizeof(T_hash_moves) << "\n";
	//std::cout << "число степени 2 hash_n = " << hash_n << "\n";
	//std::cout << "количество элементов в хеш-таблице max_index_hash = " << max_index_hash << "\n";

}

//	===================================================================
// освобождаем память из под хеш-таблицы
void Hash::hash_size_free()
{
	free(p_hash_moves);
}

//	===================================================================
// инициализируем все ключи хеш-таблицы 0
void Hash::ini_hash_moves()
{                                                      
	for (unsigned __int32 ihm = 0; ihm <= max_index_hash; ihm++)
	{
		p_hash_moves[ihm].key_lock_32_move = 0;
		p_hash_moves[ihm].key_lock_32_score = 0;
	}
}

//	===================================================================
// кэшируем лучший на данный момент ход или оценку (зависит от поиска)
void Hash::save_best_hash_score_or_move
(
	unsigned __int8 i,// номер хода по списку
	unsigned __int64 key,// хеш-ключ
	class List_surplus_moves * p_list_surplus_moves,// список ходов
	__int32 score,// оценка хода
	unsigned __int8 flag_hash_score,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 depth,// глубина записи хода
	unsigned __int8 depth_max// максимальная глубина расчета
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
	unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);
	unsigned __int8 delta_depth = depth_max - depth;

	if (delta_depth < 0) delta_depth = 0;

	/*
	запись лучшего хода и лучшей оценки разделены
	потому что при перезаписи в общем случае мы имеем противоречивые требования
	о которых написал в функциях записи хода и оценки

	на самом деле мы пишем две позиции в одну ячейку хеш-таблицы
	для уменьшения в двое коллизии перезаписи позиции
	когда разные позиции хотят залезть в одну и ту же ячейку
	*/

	// запись лучшего хода
	save_best_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	// запись лучшей оценки
	save_best_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
}

//	===================================================================
// смотрим не встречалась ли нам такая позиция если встречалась то применяем сохраненную оценку
// поиск за белых
__int32 Hash::look_hash_score_white
(
	const unsigned __int64 key,// хеш-ключ
	const unsigned __int8 depth,// глубина
	const unsigned __int8 depth_max,// максимальная глубина
	const __int32 alpha,//
	const __int32 beta,//
	unsigned __int8 & test_flag_hash_moves,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 & hash_depth,// максимальная глубина
	unsigned __int8 & hash_depth_max// максимальная глубина
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
	unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);


	/*
	(описание со стороны белых)

	при использовании оценки из хеш-таблицы возможно три случая:

	1)(в моей программе этот случай не работает)
	EXACT (проверяем score <  beta )
	при переборе получили точную оценку т.е. beta > score > alpha
	так как мы считаем с нулевым окном и кроме того а-б пределы могут менятся
	то точная оценка на самом деле не очень точна
	поэтому прежде чем ее использовать проверяем что она все еще как бы точна.
	проверяем только на score <  beta  
	потому что больше она alpha или меньше не важно
	так как в случае score < alpha мы просто получаем 3-ий вариант

	2)LOWER (проверяем score >= beta )
	при переборе получили оценку которая вызвала отсечку по beta. это минимум
	т.е. при дальнейшем счете оценка бы только повышалась.
	нас волнует только что бы она и теперь вызывала отсечку по бете так что
	проверяем только score >= beta

	3)UPPER (проверяем score < alpha )
	при переборе оценка не улучшила альфу и не вызвала отсечку по бете. это локальный
	максимум. реальной оценки мы не знаем. да нас и не волнует ее точность главное 
	что бы она по прежнему не улучшала альфу. 
	так что проверяем score < alpha
	конечно правильнее будет score <= alpha но при тестировании я не смог проверить этот случай
	*/

	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	if (p_hash_moves[ind].key_lock_32_score == key_lock_32)
	{
		// если позиция уже рассмотренна на длину варианта больше или равную той на которую
		// мы собираемся просмотреть то можно просто вернуть оценку варианта
		if (p_hash_moves[ind].score_delta_depth >= (depth_max - depth))
		{
			if (p_hash_moves[ind].flag_hash_score == LOWER)
			{
				// надо проверить отекается ли кешированной оценкой поиск и в данном случае
				// из хеш - таблицы имеем не точную оценку а ту что отсекла поиск. если бы поиск был 
				// продолжен то почти наверняка оценка бы усилилась. но нам от нее нужно только что бы
				// она отсекала поиск и в данном случае.
				if (p_hash_moves[ind].score >= beta)
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_LOWER = hash_read_score_LOWER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				} 

			}
			else
			{

				// проверяем что оценка все еще не улучшает альфу
				// тест показал что есть позиции где расходятся ходы и оценки
				// и даже если а б одинаковы и поиск не в нулевом окне и глубина одинакова 
				// все равно ходы как и оценки могут быть разными
				// оценка очень нестабильна и зависит от порядка ходов однако условие <= а соблюдается
				if (p_hash_moves[ind].score < alpha) 
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_UPPER = hash_read_score_UPPER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				}
			}
		}
	}

	return -1000000000;
}

//	===================================================================
// смотрим не встречалась ли нам такая позиция если встречалась то применяем сохраненную оценку
//поиск за черных
__int32 Hash::look_hash_score_black
(
	const unsigned __int64 key,// хеш-ключ
	const unsigned __int8 depth,// глубина
	const unsigned __int8 depth_max,// максимальная глубина
	const __int32 alpha,//
	const __int32 beta,//
	unsigned __int8 & test_flag_hash_moves,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 & hash_depth,// максимальная глубина
	unsigned __int8 & hash_depth_max// максимальная глубина
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);
	unsigned __int32 key_lock_32 = (unsigned __int32)(key >>32);

	if (p_hash_moves[ind].key_lock_32_score == key_lock_32)
	{
		if (p_hash_moves[ind].score_delta_depth >= (depth_max - depth))
		{
			if (p_hash_moves[ind].flag_hash_score == LOWER)
			{
				if (p_hash_moves[ind].score <= alpha) 
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_LOWER = hash_read_score_LOWER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				}
			}
			else
			{
				if (p_hash_moves[ind].score > beta) 
				{ 
#if TEST_WRITE_READ
					// 
					hash_read_score_UPPER = hash_read_score_UPPER + 1;
#endif // TEST_WRITE_READ

					return p_hash_moves[ind].score;
				}
			}
		}
	}
	return -1000000000;
}

//	===================================================================
// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
void Hash::look_hash_move
(
	class List_surplus_moves * p_list_surplus_moves,// список ходов
	const unsigned __int64 key,// хеш-ключ
	const unsigned __int8 depth,// глубина
	unsigned __int8  & flag_insert_hash_move//флаг того что удалось найти хеш-ход и поднять на первую позицию
)
{
	unsigned __int32 ind = (unsigned __int32)(key & max_index_hash);

	// смотрим не встречалась ли нам такая позиция если встречалась то ее ход смотрим первым
	if (p_hash_moves[ind].key_lock_32_move == (unsigned __int32)(key >> 32))
	{
#if TEST_WRITE_READ
		// количество считанных для сортировки позиций
		hash_read_sorting = hash_read_sorting + 1;
#endif // TEST_WRITE_READ

		// выводим ход на первую позицию в списке
		p_list_surplus_moves->insert_hash_moves(p_hash_moves[ind].move,flag_insert_hash_move);
	}
}

//	===================================================================
// обнуляем возраст позиции
void Hash::set_age_0()
{
	age = 0;
}

//	===================================================================
// увеличиваем возраст позиции
void Hash::aging()
{
	if (age > 250) age = 0;

	age = age + 1;
	//std::cout << "age= " << (__int32)age << "\n";
}


//////////////////////////////////////////////////////////////////////////////////////////
//private:


//	===================================================================
// кэшируем лучший на данный момент ход
inline void Hash::save_best_hash_move
(
	unsigned __int8 i,// номер хода по списку
	unsigned __int32 key_lock_32,//
	class List_surplus_moves * p_list_surplus_moves,// список ходов
	unsigned __int8 delta_depth,// глубина записи хода
	unsigned __int32 ind//
)
{
	/*

	при записи хода в таблицу возможно три случая:
	1) ячейка таблицы пустая. в пустой ячейке значение ключа 0
	в пустую ячейку пишем всегда 

	2) ячейка занята той же позицией тогда
	для текущего поиска
	перезаписываем в случае когда позиция изучена глубже т.е. больше длина варианта
	для более ранних поисков перезаписываем всегда

	тут есть не очевидный момент. казалось бы позицию следует перезаписывать только если
	мы ее лучше изучили для всех позиций. опыт показал что так следует делать только для 
	текущего поиска а предыдущие ходы следует перезаписывать всегда. иначе сбивается
	преемственность иттераций из-за застревания хода из предыдущего поиска.

	3) ячейка занята другой позицией тогда
	если записанная позиция из текущего поиска то
	перезаписываем в случае когда позиция изучена глубже т.е. больше длина варианта
	если же от более древних поисков то перезаписываем всегда. 

	-------------
	некоторые мысли:

	в текущем поиске позицию имеет смысл перезаписывать только если глубина варианта больше.
	это имеет значение и в случае перезаписи в ту же ячейку другой позиции.
	главный смысл в том что бы низкоуровневый муссор которого очень много не уничтожал лучше
	просчитанные и более ценные позиции.

	для той же позиции оставляем перезапись только если глубина варианта больше,
	а все более поздние всегда.
	тесты показывают что вроде так лучше

	для сортировки ходов
	мы не используем результаты поисков предыдущих ходов потому что для разных
	глубин лучшие ходы разные так что смысла в глубоком лучшем ходе во время новой иттерации
	нет. как правило пока не достигнута глубина хода он другой а значит не ускоряет а только
	тормозит поиск и только на равной глубине есть выигрыш. вопрос баланса по моему сильно 
	неоднозначен потому что поиск может идти по другому пути чем при поиске предыущего хода.

	другая причина в том что если мы будем оставлять ходы из предыдущих позиций то часто они 
	не дают записывать текущии и получается срыв поиска который я неоднократно наблюдал даже 
	в pv варианте

	главые причины все же в срыве поиска из-за влияния предыдущего и засорение таблицы.
	в итоге для сортировки ходов я использую кеш только текущего поиска т.е. иттераций
	в текущей позиции
	*/
  
	//1) ячейка пустая
	if (p_hash_moves[ind].key_lock_32_move == 0)
	{
		// записываем лучший ход в любом случае
		save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);

	}//2) та же позиция
	else if (p_hash_moves[ind].key_lock_32_move == key_lock_32) 
	{
		// имеем текущий поиск
		if (age == p_hash_moves[ind].move_age) 
		{
			//ход перезаписываем если глубина варианта больше или равна
			if (delta_depth >= p_hash_moves[ind].move_delta_depth)
			{
				save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
			}
		}
		else//нужно что-бы не сбивать иттерации текущего поиска (даже с учетом что это та же позиция)
		{
			// более поздние перезаписываем всегда
			save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
		}

	}//3) другая позиция
	else if (p_hash_moves[ind].key_lock_32_move != key_lock_32) 
	{
		// имеем текущий поиск
		if (age == p_hash_moves[ind].move_age) 
		{
			//ход перезаписываем если глубина варианта больше или равна
			if (delta_depth >= p_hash_moves[ind].move_delta_depth)
			{
				save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
			}
		}
		else
		{
			// более поздние перезаписываем всегда
			save_hash_move(i,key_lock_32,p_list_surplus_moves,delta_depth,ind);
		}
	}

}

//	===================================================================
// кэшируем лучший на данный момент ход 
inline void Hash::save_hash_move
(
	unsigned __int8 i,// номер хода по списку
	unsigned __int32 key_lock_32,//
	class List_surplus_moves * p_list_surplus_moves,// список ходов
	unsigned __int8 delta_depth,// глубина записи хода
	unsigned __int32 ind//   
)
{
	// запоминаем верхние 32 бита ключа
	p_hash_moves[ind].key_lock_32_move = key_lock_32;

	// запоминаем лучший ход
	p_hash_moves[ind].move = p_list_surplus_moves->get_move(i);

	// глубина варианта при которой получен данный ход
	p_hash_moves[ind].move_delta_depth = delta_depth;

	// запоминаем возраст хода
	p_hash_moves[ind].move_age = age;

#if TEST_WRITE_READ
	// количество записанных позиций
	hash_move_write = hash_move_write + 1;
#endif // TEST_WRITE_READ

}

//	===================================================================
// кэшируем лучшую на данный момент оценку
inline void Hash::save_best_hash_score
(
	unsigned __int32 key_lock_32,//
	__int32 score,// оценка хода
	unsigned __int8 flag_hash_score,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 delta_depth,// глубина записи хода
	unsigned __int32 ind//
)
{
	// кэшируем лучшую на данный момент оценку

	/*
	при записи оценки в таблицу возможно три случая:
	1) ячейка таблицы пустая. в пустой ячейке значение ключа 0
	в пустую ячейку пишем всегда 

	2) ячейка занята той же позицией
	перезаписываем если позиция изучена глубже т.е. больше длина варианта

	3) ячейка занята другой позицией
	если занята позицией из прошлых поисков, кроме предпоследнего, то перезаписываем
	если из текущего и предпоследнего то перезаписываем глубже изученными позициями.

	(это необходимо контролировать потому что иначе позиции вытесняются ничего не значащими
	позициями.)
	*/
  
	// 1) ячейка пустая
	if (p_hash_moves[ind].key_lock_32_score == 0)
	{
		// записываем лучший ход в любом случае
		save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);

	}// 2) ту же самую позицию перезаписываем если вариант глубже или равен
	else if (p_hash_moves[ind].key_lock_32_score == key_lock_32)
	{
		// текущий поиск и предыдущий заменяем если глубина варианта больше
		if (age <= p_hash_moves[ind].score_age + 1)
		{
			//ход перезаписываем если глубина варианта больше или равна
			if(delta_depth >= p_hash_moves[ind].score_delta_depth)
			{
				save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
			}

		}// более поздние перезаписываем всегда
		else
		{
			save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
		}

	}// 3) ячейка заполнена другой позицией
	else if (p_hash_moves[ind].key_lock_32_score != key_lock_32)
	{
		// текущий поиск и предыдущий заменяем если глубина варианта больше
		if (age <= p_hash_moves[ind].score_age + 1)
		{
			//ход перезаписываем если глубина варианта больше или равна
			if(delta_depth >= p_hash_moves[ind].score_delta_depth)
			{
				save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
			}

		}// более поздние перезаписываем всегда
		else
		{
			save_hash_score(key_lock_32,score,flag_hash_score,delta_depth,ind);
		}
	}
}

//	===================================================================
// кэшируем оценку т.е. записываем ее в хеш-таблицу
inline void Hash::save_hash_score
(
	unsigned __int32 key_lock_32,//
	__int32 score,// оценка хода
	unsigned __int8 flag_hash_score,// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	unsigned __int8 delta_depth,// глубина записи хода
	unsigned __int32 ind//
)
{
	// запоминаем верхние 32 бита ключа
	p_hash_moves[ind].key_lock_32_score = key_lock_32;

	// пишем оценку
	p_hash_moves[ind].score = score;

	// флаг хода (1- улучшили альфу(бету); 3-просто локальный максимум(минимум); 2-отсечка по бете(альфе))
	p_hash_moves[ind].flag_hash_score = flag_hash_score;

	// запоминаем текущую глубину
	p_hash_moves[ind].score_delta_depth = delta_depth;

	// запоминаем возраст
	p_hash_moves[ind].score_age = age;


#if TEST_WRITE_READ

	if (flag_hash_score == HASHM::LOWER)
	{
		// количество записанных позиций
       hash_score_write_low = hash_score_write_low + 1;
	}
	else
	{
		// количество записанных позиций
		hash_score_write_high = hash_score_write_high + 1;
	}

#endif // TEST_WRITE_READ

}

//	===================================================================
// тестируем хеш-таблицу. печатаем полное количество элементов сколько использовали а сколько пустых
void Hash::TEST_Hash_index_print()
{

	unsigned __int32 full_move = 0;
	unsigned __int32 full_score = 0;

	unsigned __int32 yes_move = 0;
	unsigned __int32 yes_score = 0;

	unsigned __int32 no_move = 0;
	unsigned __int32 no_score = 0;

	for (unsigned __int32 cnt = 0; cnt <= max_index_hash; cnt++)
	{
		if (p_hash_moves[cnt].key_lock_32_move != 0) yes_move = yes_move + 1 ;

		if (p_hash_moves[cnt].key_lock_32_score != 0) yes_score = yes_score + 1 ;

		if (p_hash_moves[cnt].key_lock_32_move == 0) no_move = no_move + 1 ;

		if (p_hash_moves[cnt].key_lock_32_score == 0) no_score = no_score + 1 ;
	}

	full_move = yes_move + no_move;

	full_score = yes_score + no_score;


	std::cout << "  "<< "\n";
	std::cout << " полное количество элементов " << max_index_hash << "\n";

	if (full_move != 0)
	{
		std::cout << "full_move = " << full_move << "\n";
	}

	if (full_score != 0)
	{
		std::cout << "full_score = " << full_score << "\n";
	}

	std::cout << "  "<< "\n";

	if (yes_move != 0)
	{
		std::cout << "yes_move = " << yes_move << "\n";
	}

	if (yes_score != 0) 
	{
		std::cout << "yes_score = " << yes_score << "\n";
	}

	std::cout << "  "<< "\n";

	if (no_move != 0)
	{
		std::cout << "no_move = " << no_move << "\n";
	}

	if (no_score != 0)
	{
		std::cout << "no_score = " << no_score << "\n";
	}

	std::cout << "  "<< "\n";

	if (no_move != 0)
	{
		std::cout << " 100 * yes_move/no_move = " << 100 * yes_move/no_move << "\n";
	}

	if (no_score != 0)
	{
		std::cout << " 100 * yes_score/no_score = " << 100 * yes_score/no_score << "\n";
	}

	std::cout << "  "<< "\n";
}
