#include <iostream>
using namespace std;

#include "k_Timer.h"

//-----------------------------------------------------------------------------------
/* +
 * задаем функции по работе с временем
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 25.02.2009    5.12.2009
 *
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
*/

//---------------------------------------------------------------------------


//clock() - функция возвращающая количество процессорных тиков от начала запуска программы
//CLOCKS_PER_SEC - количество тиков в секунду (у меня дает 1000)

// global
  clock_t start_timer,finish_timer;// таймер для общего времени счета
  double out;// время выводим в секундах


//	===================================================================
// запускаем таймер	
void TIMER::Timer_start(){

    start_timer = clock();

}//void TIMER::Timer_start
//	*******************************************************************

//	===================================================================
// останавливаем таймер	
void TIMER::Timer_stop(){

    finish_timer = clock();

}//void TIMER::Timer_stop
//	*******************************************************************

//	===================================================================
// вычисляем прошедшее время (результат в секундах)
double TIMER::Timer_delta(){

    out = (double)(finish_timer - start_timer) / (double)CLOCKS_PER_SEC;

	if (out < 0.01) out = 0.01;

 // тестируем вывод времени
  //cout <<"start_timer = "<< start_timer <<"\n";
  //cout <<"finish_timer = "<< finish_timer <<"\n";
  //cout <<"CLOCKS_PER_SEC = "<< CLOCKS_PER_SEC <<"\n";
  //cout <<"((end_1-start_1) / CLOCKS_PER_SEC) = "<< out <<"\n";
  //cout <<"((double)(finish_timer - start_timer) / CLOCKS_PER_SEC) = "
	 // << (double)((finish_timer - start_timer) / CLOCKS_PER_SEC) <<"\n";

    return out;

}//double TIMER::Timer_delta
//	*******************************************************************
