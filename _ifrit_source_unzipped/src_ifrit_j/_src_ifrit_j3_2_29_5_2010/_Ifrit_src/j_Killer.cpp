#include <fstream>  // TEST

#include "j_Killer.h"
#include "k_Util.h"

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  25.02.2009
*/
//---------------------------------------------------------------------------

/*
 УРОВЕНЬ МОЕГО ДОВЕРИЯ 80%

 реализуем киллеры
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 std::ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

namespace 
{
	__int32 killer_moves_1[MAX_DEPTH];
	__int32 killer_moves_2[MAX_DEPTH];
}

//	===================================================================
// инициализируем киллера
void SKILLER::public_ini_killer()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH; i++ )
	{
		killer_moves_2[i] = 0;
		killer_moves_1[i] = 0;
	}
}

//	===================================================================
// пишем киллера
void SKILLER::public_save_killer
(
	const unsigned __int8 i,//номер хода в списке
	const struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
	const unsigned __int8 depth// текущая глубина
)
{
	if (killer_moves_1[depth] != p_list_surplus_moves->move[i])
	{
		killer_moves_2[depth] = killer_moves_1[depth];
		killer_moves_1[depth] = p_list_surplus_moves->move[i];
	}
}

//	===================================================================
// вставляем киллера в начало списка(позиция start) 
void SKILLER::public_insert_killer
(
	struct T_list_surplus_moves * p_list_surplus_moves,// список ходов
	const unsigned __int8 depth,// текущая глубина
	unsigned __int8 start//позиция с которой ищем и на которую вставляем киллер
)
{
	__int32 move_s = 0;
	__int32 score_s;// используем для запоминания оценки хода
	unsigned __int8 j;

	// 1) ищем киллер 1 ход в списке
	for ( j = start
	// крутим до тех пор пока не нашли первый киллер ход или не кончился список
	; ( (p_list_surplus_moves->move[j] != killer_moves_1[depth]) 
	&& (j < p_list_surplus_moves->end_list) )
	; j++  );

	// если такой ход нашелся то идем дальше
	if (j < p_list_surplus_moves->end_list)
	{	
		//2) запоминаем киллер ход 
		move_s  = p_list_surplus_moves->move[j];
		score_s = p_list_surplus_moves->sorting_score[j];

		//3) сдвигаем все строчки вниз
		//от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
		for (unsigned __int8 i = j; i > start; i--)
		{	
			p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
			p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
		}

		//4) на стартовую позицию записываем киллера
		p_list_surplus_moves->move[start] = move_s;
		p_list_surplus_moves->sorting_score[start] = score_s;
	}

	start = start + 1;

	// 1 ищем киллер 2 ход в списке
	for ( j = start
	// крутим до тех пор пока не нашли второй киллер ход или не кончился список
	; ( (p_list_surplus_moves->move[j] != killer_moves_2[depth]) 
	&& (j < p_list_surplus_moves->end_list) )
	;j++);

	// если такой ход нашелся то идем дальше
	if (j < p_list_surplus_moves->end_list)
	{	
		//2) запоминаем киллер ход 
		move_s  = p_list_surplus_moves->move[j];
		score_s = p_list_surplus_moves->sorting_score[j];

		//3) сдвигаем все строчки вниз
		//от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
		for (unsigned __int8 i = j; i > start; i--)
		{	
			p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
			p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
		}

		//4) на стартовую позицию записываем киллера
		p_list_surplus_moves->move[start] = move_s;
		p_list_surplus_moves->sorting_score[start] = score_s;


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
		Test_Killer << "записываем киллера"<< "\n";
		Test_Killer << "start= "<< start << "\n";
		//Test_Killer << "j= "<< j << "\n";
		//Test_Killer << "i= "<< i << "\n";
		//Test_Killer << "i_end= "<< i_end << "\n";
		Test_Killer << "depth= "<< depth << "\n";
		Test_Killer << "p_list_surplus_moves->end_list[depth]= "<< p_list_surplus_moves->end_list << "\n";
		Test_Killer << "killer_moves[depth].initial_position= "<< killer_moves_1[depth] << "\n";
		Test_Killer << "killer_moves[depth].final_position= "<< killer_moves_2[depth] << "\n";
		Test_Killer << "                "<< "\n";
		Test_Killer <<" "<<(UTIL::public_x_to_notation(killer_moves_1[depth]%16));
		Test_Killer <<""<<(8-killer_moves_2[depth]/16);
		Test_Killer <<" "<<(UTIL::public_x_to_notation(killer_moves_2[depth]%16));
		Test_Killer <<""<<(8-killer_moves_2[depth]/16) << "\n";
#endif 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
		Test_Killer.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}
}
