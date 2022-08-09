
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ  26.05.2012
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 40%

#ifndef GO_PARAMETERS_H
#define GO_PARAMETERS_H

struct Go_parameters 
{
	//> go btime 300000 wtime 300000 
	double wtime;     /// оставшееся время на игру за белых
	double btime;     /// оставшееся время на игру за черных

	//> go btime 302000 wtime 302000 winc 2000 binc 2000 
	double winc;      ///  добавление времени за ход для белых
	double binc;      ///  добавление времени за ход для черных

	//> go wtime 300000 btime 300000  movestogo 40
	__int8 movestogo; /// количество ходов за определенное время

	//> go depth 5 
	__int8 depth; /// глубина перебора

	//> go movetime 300000 
	double movetime;  /// время на ход

	//> go infinite 
	bool infinite;    /// флаг бесконечного анализа
};

#endif // !defined GO_PARAMETERS_H