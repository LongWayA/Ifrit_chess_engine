#include <iostream>
#include <fstream>
#include <cstring>      // используем для работы со строкой (сравнение строк)
#include <stdlib.h>

#include "c_Go.h"
#include "g_Search.h"
#include "b_FEN.h"
#include "c_Iterative_Deepening.h"
#include "j_Hash.h"

//-----------------------------------------------------------------------------------
/*+
 * АВТОР Бренкман Андрей (Brenkman Andrey)
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009
*/
//-----------------------------------------------------------------------------------

/*

* обрабатываем команду на старт

// разбираем команду go и устанавливаем режим игры

 команды для блица типа 5 мин либо 5 мин + добавление за ход в секундах
 > go btime 300000 wtime 300000 
 for game in 5min. 

 > go btime 302000 wtime 302000 winc 2000 binc 2000 
 with incremental clock 5 min + 2 sec. 

 время на определенное количество ходов
 > go movestogo 40 wtime 300000 btime 300000 
 for 40 moves in 5 min. 
 причем арена выдает в виде
 > go wtime 300000 btime 300000  movestogo 40

 это понятно что для анализа
 > go infinite 
 for analysing. 

 время на ход говорят что фриц не поддерживает а по моему все он держит надо проверять
 > go movetime 300000 
 Exactly 5min for the next move, not supported by Fritz. 

поиск на заданную глубину
> go depth 5 
*/

namespace {
  char string_in_l[LENGTH_STRING];//используем в функции parse_go
}//namespace {

//	===================================================================
void GO::parse_go(struct T_Bitboard * const p_bitboard,//доска
			      const char string_in[]//строка команд
			      ){

const char * ptr;// вспомогательный указатель

//int mate = -1 ;
unsigned __int8 depth_max = 0;// глубина перебора
unsigned __int8 movestogo = 0;//количество ходов за определенное время

double wtime = -1.0;// оставшееся время на игру за белых
double btime = -1.0;// оставшееся время на игру за черных
double winc = -1.0;//  добавление времени за ход для белых
double binc = -1.0;//  добавление времени за ход для черных
double movetime = -1.0;// время на ход

bool infinite = false;// флаг бесконечного анализа


// копируем строку string_in в строку string_in_l это нужно потому что в дальнейшем
// строка будет разрушена при анализе
strcpy (string_in_l,string_in);

// функция strtok работает как разбиватель(разделитель на фрагменты) строки причем при этом она разрушает строку
// при первом вызове задаем строку а потом она уже продолжает сама 
ptr = strtok(string_in_l," "); // пропускаем "go"
 
// тут мы бежим по строке выделяя фрагменты отделенные пробелами
for(ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {

   if (string_equal(ptr,"depth")) {//глубина перебора
        ptr = strtok(NULL," ");// следующий за depth фрагмент отделенный пробелом(здесь число)
        depth_max = atoi(ptr);// функция atoi переводит строку в целое число

   }else if (string_equal(ptr,"wtime")) {// время которым располагают белые
		ptr = strtok(NULL," ");
        wtime = double(atoi(ptr)) / 1000.0;// миллисекунды переводим в секунды (/ 1000.0)

   }else if (string_equal(ptr,"btime")) {// время которым располагают черные
        ptr = strtok(NULL," ");
        btime = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"winc")) {// добавочное время для белых
        ptr = strtok(NULL," ");
        winc = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"binc")) {// добавочное время для черных
        ptr = strtok(NULL," ");
        binc = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"movestogo")) {//количество ходов на которое выделено время
        ptr = strtok(NULL," ");
        movestogo = atoi(ptr);

   }else if (string_equal(ptr,"movetime")) {// время на ход
        ptr = strtok(NULL," ");
        movetime = double(atoi(ptr)) / 1000.0;

   }else if (string_equal(ptr,"infinite")) {// бесконечный анализ
        infinite = true;

// пока делать не буду ----------------------------------------------
//  }else if (string_equal(ptr,"mate")) {
//         ptr = strtok(NULL," ");
//         mate = atoi(ptr);
//  }else if (string_equal(ptr,"nodes")) {

//         ptr = strtok(NULL," ");
//         nodes = my_atoll(ptr);

//  }else if (string_equal(ptr,"ponder")) {
//         ponder = true;

//  }else if (string_equal(ptr,"searchmoves")) {
//-------------------------------------------------------------------

    }//if (string_equal(ptr,"depth")) {

}//for (ptr = strtok(NULL," "); ptr != NULL; ptr = strtok(NULL," ")) {

//------------------------------------------------------------------------------

ptr = NULL;

// увеличиваем возраст позиции
HASHM::Aging();
//////////////////////////////////////////////////////////////////////////////////////////

// время уже в секундах
// получаем позицию и глубину перебора chess_board, Depth_Max
//depth_max = depth_max - 1;
Deepening(p_bitboard,depth_max,wtime,btime,winc,binc,movestogo,movetime,infinite);

}//parse_go(char string_in[],int chess_board[]){
//	*******************************************************************

//	===================================================================
// есть ли в строке искомая подстрока
bool GO::string_equal(const char s1[], const char s2[]) {
//функция показывает равны ли строки видимо уровень соответствия?
   return strcmp(s1,s2) == 0;
}
//	*******************************************************************

//	===================================================================
// есть ли в начале строки искомая подстрока
bool GO::string_start_with(const char s1[], const char s2[]) {
// выдает указатель на первое вхождение второй строки в первую
// ну и если они совпадают значит мы имеем то что нужно в самом начале
   return strstr(s1,s2) == s1;
}
//	*******************************************************************
