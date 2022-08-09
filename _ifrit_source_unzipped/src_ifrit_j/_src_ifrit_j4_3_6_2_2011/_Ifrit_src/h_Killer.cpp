
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 25.02.2009 11.12.2010

#include <fstream>

#include "h_Killer.h"
#include "m_List_surplus_moves.h"

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 std::ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

//	===================================================================
// инициализируем киллера
void Killer::ini_killer()
{
	for (unsigned __int8 i = 0; i < MAX_DEPTH; i++ )
	{
		killer_moves_2[i] = 0;
		killer_moves_1[i] = 0;
	}
}

//	===================================================================
// пишем киллера
void Killer::save_killer
(
	const unsigned __int8 i,//номер хода в списке
	class List_surplus_moves & list_surplus_moves,// список ходов
	const unsigned __int8 depth// текущая глубина
)
{
	if (killer_moves_1[depth] != list_surplus_moves.get_move(i))
	{
		killer_moves_2[depth] = killer_moves_1[depth];
		killer_moves_1[depth] = list_surplus_moves.get_move(i);
	}
}


//	===================================================================
// вставляем киллера в начало списка(позиция start) 
void Killer::insert_killer
(
	class List_surplus_moves & list_surplus_moves,// список ходов
	const unsigned __int8 depth,// текущая глубина
	unsigned __int8 start//позиция с которой ищем и на которую вставляем киллер
)
{
	list_surplus_moves.insert_killer_in_list(killer_moves_1[depth],
											 killer_moves_2[depth],
											 start);
}
