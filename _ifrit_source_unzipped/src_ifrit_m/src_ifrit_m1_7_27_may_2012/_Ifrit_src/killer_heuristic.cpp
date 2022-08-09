
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 25.02.2009 11.12.2010

#include <fstream>

#include "killer_heuristic.h"


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 std::ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

__int32 killer_moves_1[MAX_DEPTH];
__int32 killer_moves_2[MAX_DEPTH];


//	===================================================================
//
__int32 Killer_heuristic::get_killer_moves_1(const unsigned __int16 n)
{
	return killer_moves_1[n];
};

//	===================================================================
//
__int32 Killer_heuristic::get_killer_moves_2(const unsigned __int16 n)
{
	return killer_moves_2[n];
};


//	===================================================================
// инициализируем киллера
void Killer_heuristic::ini_killer()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH; i++ )
	{
		killer_moves_2[i] = 0;
		killer_moves_1[i] = 0;
	}
}

//	===================================================================
// пишем киллера
void Killer_heuristic::save_killer
(
	const unsigned __int8 i,//номер хода в списке
	struct List & list,// представление списка ходов
	const unsigned __int8 depth// текущая глубина
)
{
	if (killer_moves_1[depth] != list.move[i])
	{
		killer_moves_2[depth] = killer_moves_1[depth];
		killer_moves_1[depth] = list.move[i];
	}
}


//	===================================================================
// вставляем киллера в начало списка(позиция start) 
void Killer_heuristic::insert_killer
(
	struct List & list,// представление списка ходов
	const unsigned __int8 depth,// текущая глубина
	unsigned __int8 start//позиция с которой ищем и на которую вставляем киллер
)
{
	insert_killer_in_list(list,killer_moves_1[depth],
							killer_moves_2[depth],
							start);
}


//	===================================================================
// вставляем киллера в начало списка(позиция start) 
void Killer_heuristic::insert_killer_in_list
(
	struct List & list,// представление списка ходов
	const __int32 killer_moves_1,// 
	const __int32 killer_moves_2,//
	const unsigned __int8 start//позиция с которой ищем и на которую вставляем киллер
)
{
	__int32 move_s = 0;
	__int32 score_s;// используем для запоминания оценки хода
	unsigned __int8 j;

	// 1) ищем киллер 1 ход в списке
	if( killer_moves_1 != 0 )
	{
		for ( j = start
		// крутим до тех пор пока не нашли первый киллер ход или не кончился список
		; ( (list.move[j] != killer_moves_1) 
		&& (j < list.end_list) )
		; j++  );

		// если такой ход нашелся то идем дальше
		if (j < list.end_list)
		{	
			//2) запоминаем киллер ход 
			move_s  = list.move[j];
			score_s = list.sorting_score[j];

			//3) сдвигаем все строчки вниз
			//от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
			for (unsigned __int8 i = j; i > start; i--)
			{	
				list.move[i] = list.move[i-1];
				list.sorting_score[i] = list.sorting_score[i-1];
			}

			//4) на стартовую позицию записываем киллера
			list.move[start] = move_s;
			list.sorting_score[start] = score_s;
		}
	}

	if( killer_moves_2 != 0 )
	{
		const unsigned __int8 start_2 = start + 1;

		// 1 ищем киллер 2 ход в списке
		for ( j = start_2
		// крутим до тех пор пока не нашли второй киллер ход или не кончился список
		; ( (list.move[j] != killer_moves_2) 
		&& (j < list.end_list) )
		;j++);

		// если такой ход нашелся то идем дальше
		if (j < list.end_list)
		{	
			//2) запоминаем киллер ход 
			move_s  = list.move[j];
			score_s = list.sorting_score[j];

			//3) сдвигаем все строчки вниз
			//от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
			for (unsigned __int8 i = j; i > start_2; i--)
			{	
				list.move[i] = list.move[i-1];
				list.sorting_score[i] = list.sorting_score[i-1];
			}

			//4) на стартовую позицию записываем киллера
			list.move[start_2] = move_s;
			list.sorting_score[start_2] = score_s;


			// TEST
			// я поразился насколько древний это тест. 
			// он еще со времен когда список был общий для всего поиска!
			// кое-как его запустил.
			// конечно же сейчас он работает неправильно. 
			// надо будет приводить его в порядок

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
			Test_Killer.open ("_TEST_KILLER.txt",std::ios::app);
			//Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if TEST_K
			Test_Killer << "записываем киллера"<< std::endl;
			Test_Killer << "start= "<< start << std::endl;
			//Test_Killer << "j= "<< j << std::endl;
			//Test_Killer << "i= "<< i << std::endl;
			//Test_Killer << "i_end= "<< i_end << std::endl;
			Test_Killer << "depth= "<< depth << std::endl;
			Test_Killer << "end_list[depth]= "<< end_list << std::endl;
			Test_Killer << "killer_moves[depth].initial_position= "<< killer_moves_1[depth] << std::endl;
			Test_Killer << "killer_moves[depth].final_position= "<< killer_moves_2[depth] << std::endl;
			Test_Killer << "                "<< std::endl;
			Test_Killer <<" "<<(UTIL::public_x_to_notation(killer_moves_1[depth]%16));
			Test_Killer <<""<<(8-killer_moves_2[depth]/16);
			Test_Killer <<" "<<(UTIL::public_x_to_notation(killer_moves_2[depth]%16));
			Test_Killer <<""<<(8-killer_moves_2[depth]/16) << std::endl;
#endif 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
			Test_Killer.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

		}
	}
}