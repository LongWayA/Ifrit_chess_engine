
// АВТОР Бренкман Андрей (Brenkman Andrey)
// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 25.02.2009 5.12.2009

#include <iostream>

#include "time_management.h"

//clock() - функция возвращающая количество процессорных тиков от начала запуска программы
//CLOCKS_PER_SEC - количество тиков в секунду (у меня дает 1000)

	/// таймер для общего времени счета
	clock_t start_timer; 
	clock_t finish_timer;

//	===================================================================
// фиксируем начальное время	
void Time_management::time_start()
{
	start_timer = clock();
}

//	===================================================================
// фиксируем текущее время
void Time_management::time_stop()
{
	finish_timer = clock();
}

//	===================================================================
// вычисляем прошедшее время (результат в секундах)
double Time_management::time_delta()
{
	double out = (double)(finish_timer - start_timer) / (double)CLOCKS_PER_SEC;

	if (out < 0.01) out = 0.01;

	// тестируем вывод времени
	//cout <<"start_timer = "<< start_timer <<"\n";
	//cout <<"finish_timer = "<< finish_timer <<"\n";
	//cout <<"CLOCKS_PER_SEC = "<< CLOCKS_PER_SEC <<"\n";
	//cout <<"((end_1-start_1) / CLOCKS_PER_SEC) = "<< out <<"\n";
	//cout <<"((double)(finish_timer - start_timer) / CLOCKS_PER_SEC) = "
	// << (double)((finish_timer - start_timer) / CLOCKS_PER_SEC) <<"\n";

	return out;
}

