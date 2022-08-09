#include <fstream>  // TEST
#include <iostream> // TEST
//#include <stdlib.h>

#include "a_List_surplus_moves.h"

#define TEST_H 0 

//---------------------------------------------------------------------------
/*
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  28.11.2010 12.12.2010
*/
//---------------------------------------------------------------------------

/*
   УРОВЕНЬ МОЕГО ДОВЕРИЯ 

*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 std::ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// поднимаем кэш ход на первую позицию в списке
// если ход нашелся то flag_insert_hash_move 2 иначе флаг 1
void List_surplus_moves::insert_hash_moves
(
	__int32 hash_moves,//
	unsigned __int8  & flag_insert_hash_move//флаг того что удалось найти хеш-ход и поднять на первую позицию
)
{
	__int32 move_s = 0;
	__int32 score_s;// используем для запоминания оценки хода
	unsigned __int8 j;

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    
	static int kolizii = 0; 
	static int enter = 0; 
	int pozition = 0;

	int move_print;

	enter = enter + 1;

#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	flag_insert_hash_move = 1;// в функцию зашли

	// 1 ищем хеш-ход в списке
	for (j = 0; 
	( (move[j] != hash_moves) 
	&& (j < end_list) ); 
	j++);


// тест старый и неадекватный. нужно будет переписать
#if TEST_H

	//--------------------
	if (j == end_list)
	//if (1)
	{	
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
		Test_Hash.open ("_TEST_HASH.txt",std::ios::app);
		//Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

		for(unsigned int ihm = 0; ihm <= max_index_hash; ihm++)
		{
			if(p_hash_moves[ihm].key_lock_32_move != -1) pozition = pozition + 1;
		}

		Test_Hash << "Всего позиций "<< max_index_hash <<"\n";
		Test_Hash << "Для записей использовали позиций "<< pozition <<"\n";

		move_print = p_hash_moves[ind].move;

		kolizii = kolizii + 1;

		Test_Hash << "Вхождения "<< enter <<"\n";
		Test_Hash << "Коллизия "<< kolizii <<"\n";
		Test_Hash << "depth "<< depth <<"\n";
		Test_Hash << "p_hash_moves[].move_delta_depth " << p_hash_moves[ind].move_delta_depth << "\n";
		//Test_Hash << "key "<< key <<"\n";
		Test_Hash << "p_hash_moves[].key " << p_hash_moves[ind].key_lock_32_move << "\n";
		Test_Hash << "p_hash_moves[].move "<< move_print <<"\n";

		//------------------------------------------
		Test_Hash << "Ходящая фигура " << UTIL::public_int_to_char_board(((move_print>>24)& 7),1)<< "\n";
		Test_Hash << "p_hash_moves[].score_move " << p_hash_moves[ind].score << "\n";

		// вывод хода вида e2-e4
		//------------------------------
		Test_Hash << " " << (UTIL::public_x_to_notation(((move_print>>12)& 63)%8));
		Test_Hash << "" << (1 + ((move_print>>12)& 63)/8);//<< "\n";
		//Test_Hash<< "\n";
		Test_Hash << " " << (UTIL::public_x_to_notation(((move_print>>18)& 63)%8));
		Test_Hash << "" << (1 + ((move_print>>18)& 63)/8)<< "\n";   
		//------------------------------
		//тип хода 
		//0 - нет хода 1 – простой ход 2 – взятие 3 – длинная рокировка 4 – короткая рокировка 5 – взятие на проходе
		//12 - превращение в конь 13 - превращение в слон 14 - превращение в ладья 15 - превращение в ферзь 
		//22 - взятие с превращением в конь 23 - взятие с превращением в слон 24 - взятие с превращением в ладья 
		//25 - взятие с превращением в ферзь 
		Test_Hash << "вид хода " << ((move_print>>6)& 63)  << "\n";
		Test_Hash << "Взятая фигура " << UTIL::public_int_to_char_board(( move_print & 63),1)<< "\n";
		//Test_Hash << "начальная положение " << ((move_print>>12)& 63)  << "\n";
		//Test_Hash << "конечное положение " << ((move_print>>18)& 63)  << "\n";
     
		Test_Hash << "\n";
		//------------------------------------------

		Test_Hash << "end_list= "<< end_list <<"\n";
		Test_Hash << "j= "<< j <<"\n";
		Test_Hash << "-----------------------------------"<<"\n";

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
		Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

	}	
#endif 


	// если такой ход нашелся то идем дальше
	if (j < end_list)
	{	
		//------------------------------------------------------------------------
		// 2 запоминаем хеш-ход 
		move_s  = move[j];
		score_s = sorting_score[j];
		//------------------------------------------------------------------------

		// 3 сдвигаем все строчки вниз
		// от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
		for (unsigned __int8 i = j; i > 0; i--)
		{	
			move[i] = move[i-1];
			sorting_score[i] = sorting_score[i-1];
		}	

		//------------------------------------------------------------------------
		// 4 на стартовую позицию записываем хеш-ход
		move[0] = move_s;
		sorting_score[0] = score_s;
		//------------------------------------------------------------------------

		flag_insert_hash_move = 2;// ход нашелся
	}
}


//	===================================================================
// вставляем киллера в начало списка(позиция start) 
void List_surplus_moves::insert_killer_in_list
(
	__int32 killer_moves_1,// 
	__int32 killer_moves_2,//
	unsigned __int8 start//позиция с которой ищем и на которую вставляем киллер
)
{
	__int32 move_s = 0;
	__int32 score_s;// используем для запоминания оценки хода
	unsigned __int8 j;

	// 1) ищем киллер 1 ход в списке
	for ( j = start
	// крутим до тех пор пока не нашли первый киллер ход или не кончился список
	; ( (move[j] != killer_moves_1) 
	&& (j < end_list) )
	; j++  );

	// если такой ход нашелся то идем дальше
	if (j < end_list)
	{	
		//2) запоминаем киллер ход 
		move_s  = move[j];
		score_s = sorting_score[j];

		//3) сдвигаем все строчки вниз
		//от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
		for (unsigned __int8 i = j; i > start; i--)
		{	
			move[i] = move[i-1];
			sorting_score[i] = sorting_score[i-1];
		}

		//4) на стартовую позицию записываем киллера
		move[start] = move_s;
		sorting_score[start] = score_s;
	}

	start = start + 1;

	// 1 ищем киллер 2 ход в списке
	for ( j = start
	// крутим до тех пор пока не нашли второй киллер ход или не кончился список
	; ( (move[j] != killer_moves_2) 
	&& (j < end_list) )
	;j++);

	// если такой ход нашелся то идем дальше
	if (j < end_list)
	{	
		//2) запоминаем киллер ход 
		move_s  = move[j];
		score_s = sorting_score[j];

		//3) сдвигаем все строчки вниз
		//от позиции найденного хода и выше при этом найденный ход заменяется стоящим выше
		for (unsigned __int8 i = j; i > start; i--)
		{	
			move[i] = move[i-1];
			sorting_score[i] = sorting_score[i-1];
		}

		//4) на стартовую позицию записываем киллера
		move[start] = move_s;
		sorting_score[start] = score_s;


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
		Test_Killer << "end_list[depth]= "<< end_list << "\n";
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
