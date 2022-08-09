#include "a_Structure.h"
//-----------------------------------------------------------------------------------
/*
 * реализуем битбоард(bit board) генератор
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 21.02.2009
 *
*/
//---------------------------------------------------------------------------

#define TEST_B 0   // включаем-выключаем(1-0) режим вывода в файл тестовой информации
#define KONTROL_B 0 // включаем-выключаем(1-0) контроль списка ходов

/*******************************************
 BITBOARDGENERATOR

 реализуем битбоард(bit board) генератор
 это простой генератор без ротации. в его написании
 мне помогла статья:
 "Hyatt, Robert. Rotated bitmaps, a new twist on an old idea"

 по позиции заданной в структуре T_Bitboard генерирует список ходов T_list_surplus_moves
 список ходов избыточен потому что включает ходы под шах и открывающие шах
 + реализован детектор шахов

 проверка на шах в этом классе не сделана потому что это требует движения фигур
 поэтому на шах позицию мы проверяем в другом месте после реализации хода

********************************************/
namespace BITBOARDGENERATOR{

//////////////////////////////////////////////////////////////////////////////////////////
//public:

//---------------------------------------------------------------------------
// генерируем список взятий + превращения пешки (для белых)
void Generator_captures_white(const struct T_Bitboard * const p_bitboard,// представление доски
                              struct T_list_surplus_moves * const p_list_surplus_moves// список ходов                
							  );

//---------------------------------------------------------------------------
// генерируем список взятий  + превращения пешки (для черных)
void Generator_captures_black(const struct T_Bitboard * const p_bitboard,// представление доски
                              struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
							  );


//---------------------------------------------------------------------------
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)(для белых)
void Generator_move_white(const struct T_Bitboard * const p_bitboard,// представление доски
                          struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						  );

//---------------------------------------------------------------------------
// генерируем список остальных ходов (т.е. всех ходов кроме взятий + превращения пешки)(для черных)
void Generator_move_black(const struct T_Bitboard * const p_bitboard,// представление доски
                          struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						  );

//---------------------------------------------------------------------------
// под боем ли король белых если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 King_white_check(const struct T_Bitboard * const p_bitboard// представление доски
					 );

//---------------------------------------------------------------------------
// под боем ли король черных если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
unsigned __int8 King_black_check(const struct T_Bitboard * const p_bitboard// представление доски
					 );


//---------------------------------------------------------------------------
// печатаем битбоард(bit board) доску
#if TEST_B
 void Bitboard_print(const struct T_Bitboard * p_bitboard// представление доски
	                 );
#endif//TEST_B

//////////////////////////////////////////////////////////////////////////////////////////
//private:

//---------------------------------------------------------------------------
// под боем ли белые поля если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
inline unsigned __int8 White_check_square(const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
							  const struct T_Bitboard * const p_bitboard// представление доски
							  );

//---------------------------------------------------------------------------
// под боем ли черные поля если шаха нет то функция возвращает 0 иначе цифру шахующей фигуры
inline unsigned __int8 Black_check_square(const unsigned __int8 & sq,//индекс проверяемого поля(0-63) 
							  const struct T_Bitboard * const p_bitboard// представление доски
							  );

//---------------------------------------------------------------------------
// начальная инициализация доски
 inline void Bitboard_ini_t(struct T_Bitboard * p_bitboard// представление доски
					        );

//---------------------------------------------------------------------------
// заполняем список ходов - взятия за белых
// две функции только из за разного контроля списков(строки взятия)
inline void List_capture_white(unsigned __int8 name,// имя ходящей фигуры
							   unsigned __int8 type_move,// тип хода
							   const unsigned __int8 & from,// откуда ходим
							   const unsigned __int8 & to,// куда ходим
							   unsigned __int8 & count,// номер хода в списке
							   const struct T_Bitboard * const p_bitboard,// представление доски
                               struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
							   );

//---------------------------------------------------------------------------
// заполняем список ходов - взятия за черных
// две функции только из за разного контроля списков(строки взятия)
inline void List_capture_black(unsigned __int8 name,// имя ходящей фигуры
							   unsigned __int8 type_move,// тип хода
							   const unsigned __int8 & from,// откуда ходим
							   const unsigned __int8 & to,// куда ходим
							   unsigned __int8 & count,// номер хода в списке
							   const struct T_Bitboard * const p_bitboard,// представление доски
                               struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						       );

//---------------------------------------------------------------------------
// заполняем список ходов
inline void List_record_move(unsigned __int8 name,// имя ходящей фигуры
							 unsigned __int8 type_move,// тип хода
							 const unsigned __int8 & from,// откуда ходим
							 const unsigned __int8 & to,// куда ходим
							 unsigned __int8 & count,// номер хода в списке
							 const struct T_Bitboard * const p_bitboard,// представление доски
                             struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
	                         );

//---------------------------------------------------------------------------
// заполняем список ходов для особых случаев типа рокировки взятия на проходе и превращений пешек
// sp - special
inline void List_record_sp(unsigned __int8 name,// имя ходящей фигуры
						   unsigned __int8 d_m,// тип хода
						   unsigned __int8 & p,// номер хода в списке
						   const unsigned __int8 & from,// откуда ходим
						   unsigned __int8 to,// куда ходим
						   unsigned __int8 capture,// имя взятой фигуры
                           struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						   );

//---------------------------------------------------------------------------
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для белых фигур генератора взятий
inline void KONTROL_captures_W(const struct T_Bitboard * p_bitboard,// представление доски
							   const unsigned __int8 * p_from,// откуда ходит фигура
							   const unsigned __int8 * p_to,// куда ходит фигура
							   const unsigned __int8 * p_p,// номер хода в списке
							   unsigned __int8 name_figure// имя ходящей фигуры
							   );

//---------------------------------------------------------------------------
// контролируем выход за пределы доски, за пределы списка а также пустые взятия  
// все это для черных фигур генератора взятий
inline void KONTROL_captures_B(const struct T_Bitboard * p_bitboard,// представление доски
							   const unsigned __int8 * p_from,// откуда ходит фигура
							   const unsigned __int8 * p_to,// куда ходит фигура
							   const unsigned __int8 * p_p,// номер хода в списке
							   unsigned __int8 name_figure// имя ходящей фигуры
							   );

//---------------------------------------------------------------------------
// контролируем выход за пределы доски, за пределы списка 
// для генератора ходов 
inline void KONTROL_move(const unsigned __int8 * p_from,// откуда ходит фигура
						 const unsigned __int8 * p_to,// куда ходит фигура
						 const unsigned __int8 * p_p// номер хода в списке
						 );

//---------------------------------------------------------------------------
// ищем первый установленный бит(т.е. бежим справа)
__inline __int32 Bit_scan_forward_win32(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
					                    );

//---------------------------------------------------------------------------
// ищем последний установленный бит(т.е. бежим слева)
__inline __int32 Bit_scan_reverse_win32(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
					                   );

};//namespace BITBOARDGENERATOR
