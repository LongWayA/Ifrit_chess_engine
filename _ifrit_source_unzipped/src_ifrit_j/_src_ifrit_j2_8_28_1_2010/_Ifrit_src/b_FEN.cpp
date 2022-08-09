#include <cstring>
#include <iostream> // TEST
using namespace std;

#include "b_FEN.h"
#include "e_Move.h"
#include "d_Bitboard_gen.h"
#include "j_Hash.h"

//-----------------------------------------------------------------------------------
/* ++
 * задаем функции по работе с fen протоколом
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 22.02.2009  11.11.2009
 *
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
 * модуль тестируем в модуле b_Protokol_UCI
 * целью теста является контроль правильности инициализации внутренней доски
 * движка по входящей UCI позиции
 */
//---------------------------------------------------------------------------

// задаем имя фигур(обязательно должно быть согласованно с модулем d_Bitboard_gen.cpp  и i_Forced.cpp)
#define N_KING    6 // король
#define N_QUEEN   5 // ферзь
#define N_ROOK    4 // ладья
#define N_BISHOP  3 // слон
#define N_KNIGHT  2 // конь
#define N_PAWN    1 // пешка

// global
extern class BITBOARDGENERATOR bitboard_generator_o;// генератор ходов из позиции
extern class MOVES moves_o;// реализатор(делатель) ходов сгенерированных генератором
extern class  HASHM hash_o;// кэширование


//	===================================================================
// разбираем все случаи фен и инициализируем доску
void FEN::parse_position(struct T_Bitboard * p_bitboard,//доска
					     char string_in[]//входящая строка(65536)
                         ){

// инициализирующая позицию строка может быть трех видов 
//"position startpos" просто начальная позиция
//"position startpos moves b1c3 b8c6 g1f3 g8f6" начальная позиция + ходы ведущие к текущей 
//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " задание позиции через фен представление

char * fen;  //
char * moves;//
char * ptr;  // вспомогательный указатель
char move_string[]={' ',' ',' ',' ',' ',' ',' '};// вспомогательный массив
char * StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";// стартовая фен-позиция
unsigned __int8 cnt_move = 0;// счетчик ходов в строке партии нужно для кеширования строки


//получаем указатель на fen
fen = strstr(string_in,"fen ");

//получаем указатель на moves
moves = strstr(string_in,"moves ");

if(fen != NULL) {

    //работаем с принятой фен-позицией
    fen_to_board(p_bitboard,fen);

} else {

    // а это инициализация посредством команды startpos
    // реализуем так же как предыдущую но только сами задаем стартовую позицию в виде
    // StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    fen_to_board(p_bitboard,StartFen);

}// if (fen != NULL)

//---------------------------------------------
// счетчик ходов. использую для отображения ходов на ключи позиций
cnt_move = 0;

// инициализируем массив ключей во избежании ложных срабатываний 
// это массив ключей используемых для обнаружения трехкратного повторения позиций
hash_o.Ini_hash_string_moves(); 

// по текущей позиции генерируем хеш - ключ
hash_o.Start_position_random_key(p_bitboard// представление доски
						  );

// записываем хеш-ключ в массив хеш - ключей по строке игры
hash_o.Save_hash_string_moves(p_bitboard->hash_key,cnt_move); 
//---------------------------------------------

// ходы передаются в виде
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
if(moves != NULL) {

  // проматываем moves 
  ptr = moves + 6;

  while(*ptr != '\0') { 

        move_string[1] = *ptr++;
        move_string[2] = *ptr++;
        move_string[3] = *ptr++;
        move_string[4] = *ptr++;

        if(*ptr == '\0' || *ptr == ' ') {

           move_string[5] = '\0';

        } else { // проматываем

           move_string[5] = *ptr++;
           move_string[6] = '\0';

        }//if (*ptr == '\0' || *ptr == ' ') {

        move_to_board(p_bitboard,move_string);

//---------------------------------------------
cnt_move = cnt_move + 1;
// по текущей позиции генерируем хеш - ключ
hash_o.Start_position_random_key(p_bitboard// представление доски
						  );

// записываем хеш-ключ в массив хеш -ключей по строке игры
// этот массив ключей используемых для обнаружения трехкратного повторения позиций
hash_o.Save_hash_string_moves(p_bitboard->hash_key,cnt_move); 
//---------------------------------------------

        while (*ptr == ' ') ptr++;

   }//while (*ptr != '\0') {

}//if (moves != NULL)

fen   = NULL;
moves = NULL;
ptr = NULL;
StartFen = NULL;

}//void FEN::parse_position
//	*******************************************************************

//	===================================================================
//разбираем фен - позицию и инициализируем движок используем подфункцию Ch_Koordinata_To_Indeks
void FEN::fen_to_board(struct T_Bitboard * p_bitboard,//доска
					   const char fen[]//строка содержащая фен позицию(длинна переменная)
                       ){

// работаем со строкой вида "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
// символом / отделяются горизонтали шахматной доски
// цифры означают количество пустых клеток на горизонтали
// буквы это фигуры причем большие буквы значат белый цвет а маленькие черный

//---------------------------------------------
// инициализируем битбоард(bit board) доску 
// (вообще то я уже написал для этого функцию но по моему так нагляднее)
p_bitboard->all_piece = 0;
p_bitboard->all_white_piece = 0;
p_bitboard->all_black_piece = 0;

for(unsigned __int8 name_piece = 0; name_piece < 7; name_piece++){
   p_bitboard->white_piece[name_piece] = 0;
   p_bitboard->black_piece[name_piece] = 0;
}//for(int name_piece = 0; name_piece < 7; name_piece++)

for(unsigned __int8 coordinate = 0; coordinate < 64; coordinate++){
   p_bitboard->white_name_from_coord[coordinate] = 0;
   p_bitboard->black_name_from_coord[coordinate] = 0;
}//for(int coordinate = 0; coordinate < 64; coordinate++)
//---------------------------------------------

unsigned __int8 i = 0;// позиция курсора при просмотре фен - строки

// проматываем слово fen
while(fen[i] != ' '){
	i = i + 1;
}//while (fen[i] != ' '){

// проматываем пробел
while(fen[i] == ' '){
	i = i + 1;
}//while(fen[i] == ' '){

//-----------------------------------------------------------------------------------	
// 1 этап  разбор фен позиции - перевод строки в массив доски
// так как скорость тут не нужна то для ввода используем двухмерное xy-представление доски
// (на самом деле это осталось еще со времен двухмерной доски в движке :))
unsigned __int8 y = 0;// вертикальная координата доски
unsigned __int8 x = 0;// горизонтальная координата доски
unsigned __int8 n = 0;// одномерная координата доски (когда доску отображаем на прямую)
unsigned __int8 void_q;// количество пустых клеток

// после прокручивания слова и пробела у нас осталась строка вида
// 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1

// в цикле мы обрабатываем подстроку вида  1k1r4/8/8/8/8/8/8/3R2K1
// концом обработки считаем пробел
// цикл по у т.е. пролистываем горизонтали
while(fen[i] != ' '){

	x = 0;
    //  --------------------
    //  цикл по х бежим по горизонтали
	while( (fen[i] != '/') && (fen[i] != ' ') ){

      //  переводим двухмерные координаты ху в одномерную n
      n = 8 * (7 - y) + x;

      ////////////////////////////////////////////////////////////////
	  // расставляем белые и черные фигуры
	  switch(fen[i]){
     
		case 'K':// белый король

          // добавляем короля в список всех белых фигур
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // добавляем короля в список белых королей
          p_bitboard->white_piece[N_KING] = p_bitboard->white_piece[N_KING] | move_masks_f[n];

	      // задаем координату короля на доске
          p_bitboard->white_name_from_coord[n] = N_KING;

		  break;

		case 'Q':// белый ферзь

          // добавляем ферзя в список всех белых фигур
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // добавляем ферзя в список белых ферзей
          p_bitboard->white_piece[N_QUEEN] = p_bitboard->white_piece[N_QUEEN] | move_masks_f[n];

	      // задаем координату ферзя на доске
          p_bitboard->white_name_from_coord[n] = N_QUEEN;

		  break;

		case 'R':// белая ладья

          // добавляем ладью в список всех белых фигур
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

	      // добавляем ладью в список белых ладей
          p_bitboard->white_piece[N_ROOK] = p_bitboard->white_piece[N_ROOK] | move_masks_f[n];

	      // задаем координату ладьи на доске
          p_bitboard->white_name_from_coord[n] = N_ROOK;

		  break;

		case 'B':// белый слон

          // добавляем слона в список всех белых фигур
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // добавляем слона в список белых слонов
          p_bitboard->white_piece[N_BISHOP] = p_bitboard->white_piece[N_BISHOP] | move_masks_f[n];

	      // задаем координату слона на доске
          p_bitboard->white_name_from_coord[n] = N_BISHOP;

		  break;

		case 'N':// белый конь

          // добавляем коня в список всех белых фигур
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // добавляем коня в список белых коней
          p_bitboard->white_piece[N_KNIGHT] = p_bitboard->white_piece[N_KNIGHT] | move_masks_f[n];

	      // задаем координату коня на доске
          p_bitboard->white_name_from_coord[n] = N_KNIGHT;

		  break;

		case 'P':// белая пешка

          // добавляем пешку в список всех белых фигур
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

	      // добавляем пешку в список белых пешек
          p_bitboard->white_piece[N_PAWN] = p_bitboard->white_piece[N_PAWN] | move_masks_f[n];

	      // задаем координату пешки на доске
          p_bitboard->white_name_from_coord[n] = N_PAWN;

		  break;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//      черные

		case 'k'://черный король

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_KING] = p_bitboard->black_piece[N_KING] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_KING;

		  break;

		case 'q'://черный ферзь

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_QUEEN] = p_bitboard->black_piece[N_QUEEN] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_QUEEN;

		  break;

		case 'r'://черная ладья

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_ROOK] = p_bitboard->black_piece[N_ROOK] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_ROOK;

		  break;

		case 'b'://черный слон

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_BISHOP] = p_bitboard->black_piece[N_BISHOP] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_BISHOP;

		  break;

		case 'n'://черный конь

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_KNIGHT] = p_bitboard->black_piece[N_KNIGHT] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_KNIGHT;

		  break;

		case 'p'://черная пешка

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_PAWN] = p_bitboard->black_piece[N_PAWN] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_PAWN;

		  break;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			
		default:

       //-----------------------------------------------------------------
       // в комментариях рассмотрим случай когда у нас 8 пустых клеток
       // если зашло '8' то функция Ch_Koordinata_To_Indeks выдает цифру 7 на же нужна цифра 8 
       // поэтому используем (1 +... 

	    void_q = (1 + Ch_Koordinata_To_Indeks(fen[i]));

        // обработали первую пустую клетку
        // и следующие 7 ( таким образом у нас обработаны 1 + 7 = 8 все пустые клетки ) 
	    for(unsigned __int8 f = 1; f < void_q; f++){
		   x = x + 1;
	    }//for (int f=1;f<l;f++)

	 }// switch(fen[i]){
     ////////////////////////////////////////////////////////////////

	 i = i + 1;
	 x = x + 1;

   }//while( (fen[i] != '/') && (fen[i] != ' ') ){

   //--------------------		
   // важно не проскочить пробел
   if(fen[i] != ' '){
	   i = i + 1;
	   y = y + 1;
   }//if(fen[i] != ' ')
	  	
}//while(fen[i] != ' '){

// находим все фигуры на доске
p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

// теперь нам осталось обработать подстроку вида
// w - - 0 1 или b KQkq e3 0 1 если есть рокировки и взятие на проходе
//-----------------------------------------------------------------------------------	
//2 этап загоняем очередь хода 	
// проматываем пробел
while(fen[i] == ' '){
	i = i + 1;
}//while(fen[i] == ' ')

// w - ход белых  b-ход черных
if(fen[i] == 'w') p_bitboard->colour_move = 1;
if(fen[i] == 'b') p_bitboard->colour_move = 0;

//	-----------------------------------------------------------------------------------	
//	3 этап загоняем рокировку
// пробелы
i = i + 2;

// сразу присваиваем запрет на рокировку который отменяем ниже если есть нужные символы типа К
p_bitboard->castling_Q = 0;
p_bitboard->castling_K = 0;
p_bitboard->castling_q = 0;
p_bitboard->castling_k = 0;

while(fen[i] != ' '){	
	if(fen[i] == 'K') p_bitboard->castling_K = 1;// короткая рокировка белых
	if(fen[i] == 'Q') p_bitboard->castling_Q = 1;// длинная рокировка белых
	if(fen[i] == 'k') p_bitboard->castling_k = 1;// короткая рокировка черных
	if(fen[i] == 'q') p_bitboard->castling_q = 1;// длинная рокировка черных
	i = i + 1;
}//while(fen[i] != ' '){	

//	-----------------------------------------------------------------------------------
//	 4 этап загоняем взятие на проходе
/*
 напомним как кодируется битое поле:
 And after the move 1. e4:
 rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
 другими словами нам просто передают координаты битого поля (при ходе е4 битое поле е3)
*/
// проматываем пробел
while(fen[i] == ' '){
	i = i + 1;
}//while(fen[i] == ' ')

//		 не забыть при дешифровке что у меня массив по у идет от нуля
//		 а ведь нормальная нотация от 1	 
// запретили взятие
p_bitboard->taking_passage_yes = 0;
p_bitboard->taking_passage_x   = 0;
p_bitboard->taking_passage_y   = 0;

if(fen[i] != '-'){
	p_bitboard->taking_passage_yes = 1;// разрешаем взятие
	p_bitboard->taking_passage_x = Koordinata_To_Indeks(fen[i]);// х координата(из буквы)
	i = i + 1;
	p_bitboard->taking_passage_y = Ch_Koordinata_To_Indeks(fen[i]);// у координата 	
}//if(fen[i] != '-'){
//---------------------------------------------------------------------------

}//void FEN::fen_to_board
//	*******************************************************************

//	===================================================================
//загоняем в движок мувики(moves) т.е. ходы приводящие к текущей позиции из стартовой
void FEN::move_to_board(struct T_Bitboard * p_bitboard,//доска
					    char move_string[]//строка содержащая ходы(7)
                        ){

struct T_undo undo[1];//
struct T_list_surplus_moves list_surplus_moves_f;//список избыточных ходов

// ходы передаются в виде
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
// просчитываем массив разрешенных ходов
unsigned __int8 n = 8 * Ch_Koordinata_To_Indeks(move_string[2]) 
                    + Koordinata_To_Indeks(move_string[1]);// откуда фигура ходит

unsigned __int8 n_2 = 8 * Ch_Koordinata_To_Indeks(move_string[4]) 
                      + Koordinata_To_Indeks(move_string[3]);// куда фигура ходит

// рассчитываем все возможные ходы фигуры
if(p_bitboard->colour_move == 1){
    bitboard_generator_o.Generator_captures_white(p_bitboard,&list_surplus_moves_f);
    bitboard_generator_o.Generator_move_white(p_bitboard,&list_surplus_moves_f);
}else{
    bitboard_generator_o.Generator_captures_black(p_bitboard,&list_surplus_moves_f);
    bitboard_generator_o.Generator_move_black(p_bitboard,&list_surplus_moves_f);
}//

__int16 n_3 = -1; // номер хода по списку

// бежим по списку избыточных ходов
for(unsigned __int8 i = 0; i < list_surplus_moves_f.end_list; i++){

// если ход в списке совпадает с ходом из мувика(moves) тогда..
	if (( n == ( (list_surplus_moves_f.move[i] >> 12) & 63 ) ) && // совпадение откуда фигура ходит
		( n_2 == ((list_surplus_moves_f.move[i] >> 18) & 63) )    // совпадение куда фигура ходит
		){
 
               //------------------
			/*
                в зависимости от того есть ли превращение мы корректируем индекс хода
                что бы понять этот момент нужно знать как передается превращение пешки
                в списке избыточных ходов
                ход пешкой на последнюю горизонталь один но в генераторе 
				списка ходов он расписывается
                так что под нормальным номером идет превращение в ферзь
                потом следующими идут превращения в ладью слона и коня
                таким образом они и располагаются в списке
			*/
	           switch(move_string[5]){

                  // превращения нет поэтому просто передаем номер хода в списке
                  case '\0' :
                     // пустая клетка
                     n_3 = i;
                     break;

                  case 'q' :
                     // превращение в ферзь
                     n_3 = i;
                     break;

                  case 'r' :
                     // превращение в ладья
                     n_3 = i + 1;
                     break;

                 case 'b' :
                     // превращение в слон
                     n_3 = i + 2;
                     break;

                 case 'n' :
                     // превращение в конь
                     n_3 = i + 3;
                     break;

                 default:;//символ фигуры не распознан
   
               }// switch(move_string[5]){
               //------------------

	 break;// нужный ход нашли так что дальше бежать по циклу смысла нет
	       // досрочно выходим из цикла for(int i = 0; i < list_surplus_moves_f.end_list; i++)

	}//	if (( n == ( (list_surplus_moves_f.move[i] >> 12) & 63 ) ) &&
	

}//for(int i = 0; i < list_surplus_moves_f.end_list; i++)

//----------------------------------------------------------
if(n_3 == -1){
    cout << "WARNING ! moves error" << "\n";
}else{

    // делаем ход
    if(p_bitboard->colour_move == 1){
       moves_o.Onward_moves_white((unsigned __int8)n_3,undo,&list_surplus_moves_f,p_bitboard);
    }else{
       moves_o.Onward_moves_black((unsigned __int8)n_3,undo,&list_surplus_moves_f,p_bitboard);
    }//if(p_bitboard->colour_move == 1){

 //   p_bitboard->colour_move = !p_bitboard->colour_move;

}//if(n_3 == -1){

}//void FEN::move_to_board
//	*******************************************************************

//	===================================================================
//	по букве выдает индекс клетки(одна из самых первых моих функций :))	
int FEN::Koordinata_To_Indeks(char ch){
	if(ch == 'a') return 0;
    if(ch == 'b') return 1;
	if(ch == 'c') return 2;
	if(ch == 'd') return 3;
	if(ch == 'e') return 4;
	if(ch == 'f') return 5;
	if(ch == 'g') return 6;
	if(ch == 'h') return 7;
	return -1;
}//int FEN::Koordinata_To_Indeks
//	*******************************************************************

//	===================================================================
//	по символу цифры выдает цифру
//перешла еще из моего Джина на джаве(java) :)
//подобная функция встречалось в коде Анд чесс от 9.01.06 
//сохраню как историческую ценность :)) 
int FEN::Ch_Koordinata_To_Indeks(int ch){
	if(ch == '1') return 0;
	if(ch == '2') return 1;
    if(ch == '3') return 2;
	if(ch == '4') return 3;
	if(ch == '5') return 4;
	if(ch == '6') return 5;
	if(ch == '7') return 6;
	if(ch == '8') return 7;
	return -1;
}//int FEN::Ch_Koordinata_To_Indeks
//	*******************************************************************


//---------------------------------------------------------------------------
// с помощью этой маски пишем и стираем фигуры из битового массива
const unsigned __int64 FEN::move_masks_f[] ={

// в примечании пишем какой бит установлен в 1
1,           // 0 bit т.е. ....0001
2,           // 1 bit т.е. ....0010
4,           // 2 bit т.е. ....0100 
8,           // 3 bit и т.д.
16,          // 4 bit
32,          // 5 bit
64,          // 6 bit
128,         // 7 bit
256,         // 8 bit
512,         // 9 bit
1024,        // 10 bit
2048,        // 11 bit
4096,        // 12 bit
8192,        // 13 bit
16384,       // 14 bit
32768,       // 15 bit
65536,       // 16 bit
131072,      // 17 bit
262144,      // 18 bit
524288,      // 19 bit
1048576,     // 20 bit
2097152,     // 21 bit
4194304,     // 22 bit
8388608,     // 23 bit
16777216,    // 24 bit
33554432,    // 25 bit
67108864,    // 26 bit
134217728,   // 27 bit
268435456,   // 28 bit
536870912,   // 29 bit
1073741824,  // 30 bit
2147483648,  // 31 bit
4294967296,         // 32 bit
8589934592,         // 33 bit
17179869184,        // 34 bit
34359738368,        // 35 bit
68719476736,        // 36 bit
137438953472,       // 37 bit
274877906944,       // 38 bit
549755813888,       // 39 bit
1099511627776,      // 40 bit
2199023255552,      // 41 bit
4398046511104,      // 42 bit
8796093022208,      // 43 bit
17592186044416,     // 44 bit
35184372088832,     // 45 bit
70368744177664,     // 46 bit
140737488355328,    // 47 bit
281474976710656,    // 48 bit
562949953421312,    // 49 bit
1125899906842624,   // 50 bit
2251799813685248,   // 51 bit
4503599627370496,   // 52 bit
9007199254740992,   // 53 bit
18014398509481984,  // 54 bit
36028797018963968,  // 55 bit
72057594037927936,  // 56 bit
144115188075855872, // 57 bit
288230376151711744, // 58 bit
576460752303423488, // 59 bit
1152921504606846976,// 60 bit
2305843009213693952,// 61 bit
4611686018427387904,// 62 bit
9223372036854775808 // 63 bit
};//unsigned __int64 move_masks[] ={
//-----------------------------------------------------------------------------------
