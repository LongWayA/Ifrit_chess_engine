#include "a_structure.h"
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
 класс BITBOARDGENERATOR

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
class BITBOARDGENERATOR{

// const 

// с помощью этой маски пишем и стираем фигуры из битового массива
static const unsigned __int64 move_masks[];

// маски ходов короля
static const unsigned __int64 king_moves_masks[];

// маски ходов коня
static const unsigned __int64 knigt_moves_masks[];

// маска ходы ладьи
// маска ладья ходы вверх
static const unsigned __int64 rook_up_masks[];
// маска ладья ходы вниз
static const unsigned __int64 rook_down_masks[];
// маска ладья ходы влево
static const unsigned __int64 rook_left_masks[];
// маска ладья ходы вправо
static const unsigned __int64 rook_right_masks[];

// маска ходы слона
// маска слон ходы вверх - вправо
static const unsigned __int64 bishop_up_right_masks[];
// маска слон ходы вверх - влево
static const unsigned __int64 bishop_up_left_masks[];
// маска слон ходы вниз - вправо
static const unsigned __int64 bishop_down_right_masks[];
// маска слон ходы вниз - влево
static const unsigned __int64 bishop_down_left_masks[];
// маска взятий белой пешки
static const unsigned __int64 pawn_capture_white[];
// маска взятий черной пешки
static const unsigned __int64 pawn_capture_black[];

// константы для белой стороны
// с помощью LINE_1 находим пешки стоящие на второй горизонтали
// они могут ходить на два поля вперед
// с помощью LINE_7 находим пешки дошедшие до превращения т.е. достигшие восьмой горизонтали
static const unsigned __int64 LINE_1 = 255<<8; // 2 горизонталь доски
static const unsigned __int64 LINE_7 = 18374686479671623680; // 8 горизонталь доски

static const unsigned __int64 CASTLING_Q = 14; // поля  B1 C1 D1 заняты
static const unsigned __int64 CASTLING_K = 96; // поля F1 G1 заняты

// константы для черной стороны
static const unsigned __int64 LINE_0 = 255; // 1 горизонталь доски (находим превращения)
static const unsigned __int64 LINE_6 = 71776119061217280; // 7 горизонталь доски

static const unsigned __int64 CASTLING_q = 1008806316530991104; // поля  B8 C8 D8 заняты
static const unsigned __int64 CASTLING_k = 6917529027641081856; // поля F8 G8 заняты


//////////////////////////////////////////////////////////////////////////////////////////
public:

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
private:

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
inline void list_capture_white(unsigned __int8 name,// имя ходящей фигуры
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
inline void list_capture_black(unsigned __int8 name,// имя ходящей фигуры
							   unsigned __int8 type_move,// тип хода
							   const unsigned __int8 & from,// откуда ходим
							   const unsigned __int8 & to,// куда ходим
							   unsigned __int8 & count,// номер хода в списке
							   const struct T_Bitboard * const p_bitboard,// представление доски
                               struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
						       );

//---------------------------------------------------------------------------
// заполняем список ходов
inline void list_record_move(unsigned __int8 name,// имя ходящей фигуры
							 unsigned __int8 type_move,// тип хода
							 const unsigned __int8 & from,// откуда ходим
							 const unsigned __int8 & to,// куда ходим
							 unsigned __int8 & count,// номер хода в списке
							 const struct T_Bitboard * const p_bitboard,// представление доски
                             struct T_list_surplus_moves * const p_list_surplus_moves// список ходов
	                         );

//---------------------------------------------------------------------------
// заполняем список ходов для особых случаев типа рокировки взятия на проходе и превращений пешек
inline void list_record_os(unsigned __int8 name,// имя ходящей фигуры
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
inline void BITBOARDGENERATOR::KONTROL_move(const unsigned __int8 * p_from,// откуда ходит фигура
						                    const unsigned __int8 * p_to,// куда ходит фигура
						                    const unsigned __int8 * p_p// номер хода в списке
						                    );

//---------------------------------------------------------------------------
// ищем первый установленный бит(т.е. бежим справа)
__inline __int32 first_one(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
					   );

//---------------------------------------------------------------------------
// ищем последний установленный бит(т.е. бежим слева)
__inline __int32 last_one(unsigned __int64 analyzed_number// число в котором ищем взведенный бит
					  );

};//class BITBOARDGENERATOR
