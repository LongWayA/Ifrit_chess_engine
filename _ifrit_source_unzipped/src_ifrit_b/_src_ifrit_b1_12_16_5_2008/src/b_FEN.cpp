#include <cstring>

#include "b_FEN.h"
#include "e_Move.h"
#include "d_Bitboard_gen.h"

//-----------------------------------------------------------------------------------
/* ++
 * задаем функции по работе с fen протоколом
 *
 * АВТОР ChessTerminator75
 * ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 10.02.2008
 *
 *  ТЕСТИРОВАНИЕ
 * непосредственных тестов нет
 * модуль тестируем в модуле b_Protokol_UCI
 * целью теста является контроль правильности инициализации внутренней доски
 * движка по входящей UCI позиции
 */
//---------------------------------------------------------------------------
T_list_surplus_moves list_surplus_moves_f={0};//список избыточных ходов

//	===================================================================
// разбираем переданную оболочкой позицию и инициализируем движок
void parse_position(T_Bitboard * bitboard,const char string_in[]){
// инициализирующая позицию строка может быть трех видов 
//"position startpos" просто начальная позиция
//"position startpos moves b1c3 b8c6 g1f3 g8f6" начальная позиция + ходы ведущие к текущей 
//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " задание позиции через фен представление

 char * fen;  //
 char * moves;//
 char * ptr;  // вспомогательный указатель
 char move_string[]={' ',' ',' ',' ',' ',' ',' '};// вспомогательный массив
 // стартовая фен-позиция
 char * StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";

// выдает указатель на fen
   fen = strstr(string_in,"fen ");
// выдает указатель на moves
   moves = strstr(string_in,"moves ");

   if (fen != NULL) { // "fen" если команда присутствует
// тогда работаем с принятой фен-позицией
      board_from_fen(bitboard,fen);
   } else {
// а это инициализация посредством команды startpos
// реализуем так же как предыдущую но только сами задаем стартовую позицию в виде
// StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
     board_from_fen(bitboard,StartFen);
   }// if (fen != NULL)

// ходы передаются в виде
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
   if (moves != NULL) { // если команда "moves" присутствует
      // проматываем moves 
      ptr = moves + 6;

      while (*ptr != '\0') { 
         move_string[1] = *ptr++;
         move_string[2] = *ptr++;
         move_string[3] = *ptr++;
         move_string[4] = *ptr++;

         if (*ptr == '\0' || *ptr == ' ') {
            move_string[5] = '\0';
         } else { // проматываем
            move_string[5] = *ptr++;
            move_string[6] = '\0';
         }//if (*ptr == '\0' || *ptr == ' ') {
         board_from_move(bitboard,move_string,&list_surplus_moves_f);
         while (*ptr == ' ') ptr++;
      }//while (*ptr != '\0') {
    }//if (moves != NULL)

fen   = NULL;
moves = NULL;
ptr = NULL;
StartFen = NULL;

}//parse_position(char string_in[]){
//	*******************************************************************

//	===================================================================
// разбираем строку фен и инициализируем движок используем подфункцию Ch_Koordinata_To_Indeks
void board_from_fen(T_Bitboard * bitboard,/* const*/ char fen[]){
// работаем со строкой вида "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
// символом / отделяются горизонтали шахматной доски
// цифры означают количество пустых клеток на горизонтали
// буквы это фигуры причем большие буквы значат белый цвет а маленькие черный

// инициализируем битбоард доску (вообще то я уже написал для этого функцию 
// но по моему так нагляднее)
bitboard->all_piece =0;
bitboard->all_white_piece =0;
bitboard->all_black_piece =0;

for(int k=0 ; k<7 ; k++){
  bitboard->white_piece[k]=0;
  bitboard->black_piece[k]=0;
}//for(int i=0 ; i<8 ; i++){

// медленней но очевидней
for(int j=0 ; j<64 ; j++){
  bitboard->white_name_from_coord[j]=0;
  bitboard->black_name_from_coord[j]=0;
}//for(int j=0 ; j<64 ; j++){
//---------------------------------------------

  int i=0;
// проматываем fen
  while (fen[i]!=' '){
	  i=i+1;
  }//while (fen[i]!=' '){

// проматываем пробел
  while (fen[i]==' '){
	  i=i+1;
  }//while (fen[i]!=' '){

//-----------------------------------------------------------------------------------	
// 1 этап  разбор фен позиции- перевод строки в массив доски
// так как скорость тут не нужна то для ввода используем двухмерное xy-представление доски
int y=0;
int x=0;
int n=0;
int void_q;// количество пустых клеток
// 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1
// цикл по у т.е. пролистываем горизонтали
while(fen[i]!=' '){
	x=0;
//--------------------
// цикл по х бежим по горизонтали
	while((fen[i]!='/')&&(fen[i]!=' ')){
// 0-нет фигуры 1-пешка 2-ладья 3-конь 4-слон 5-ферзь 6-король	
    n = 8 *(7 - y) + x;
	 switch(fen[i]){
// белые
		case 'R'://ладья
//белые фигуры
         bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
	   // ладьи в списке (имя ладьи 4)
         bitboard->white_piece[4]=bitboard->white_piece[4] | move_masks_f[n];
	   // ладьи от координаты
         bitboard->white_name_from_coord[n]=4;
		break;
		case 'N'://конь
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // кони в списке (имя коня 2)
       bitboard->white_piece[2]=bitboard->white_piece[2] | move_masks_f[n];
	   // кони от координаты
       bitboard->white_name_from_coord[n]=2;
		break;
		case 'B'://слон
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // слоны в списке
       bitboard->white_piece[3]=bitboard->white_piece[3] | move_masks_f[n];
	   // слоны от координаты
       bitboard->white_name_from_coord[n]=3;
		break;
		case 'Q'://ферзь
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // ферзи в списке
       bitboard->white_piece[5]=bitboard->white_piece[5] | move_masks_f[n];
	   // ферзи от координаты
       bitboard->white_name_from_coord[n]=5;
		break;
		case 'K'://король
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // король в списке
       bitboard->white_piece[6]=bitboard->white_piece[6] | move_masks_f[n];
	   // король от координаты
       bitboard->white_name_from_coord[n]=6;
		break;
		case 'P'://пешка
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
	   // пешки в списке
       bitboard->white_piece[1]=bitboard->white_piece[1] | move_masks_f[n];
	   // пешка от координаты
       bitboard->white_name_from_coord[n]=1;
		break;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// 0-нет фигуры 1-пешка 2-ладья 3-конь 4-слон 5-ферзь 6-король	
// черные
		case 'r':
//черные фигуры
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
	   // ладьи в списке
       bitboard->black_piece[4]=bitboard->black_piece[4] | move_masks_f[n];
	   // ладьи от координаты
       bitboard->black_name_from_coord[n]=4;
		break;
		case 'n':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // кони в списке
       bitboard->black_piece[2]=bitboard->black_piece[2] | move_masks_f[n];
	   // кони от координаты
       bitboard->black_name_from_coord[n]=2;
		break;
		case 'b':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // слоны в списке
       bitboard->black_piece[3]=bitboard->black_piece[3] | move_masks_f[n];
	   // слоны от координаты
       bitboard->black_name_from_coord[n]=3;
		break;
		case 'q':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // ферзи в списке
       bitboard->black_piece[5]=bitboard->black_piece[5] | move_masks_f[n];
	   // ферзи от координаты
       bitboard->black_name_from_coord[n]=5;
		break;
		case 'k':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // король в списке
       bitboard->black_piece[6]=bitboard->black_piece[6] | move_masks_f[n];
	   // король от координаты
       bitboard->black_name_from_coord[n]=6;
		break;
		case 'p':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
	   // пешки в списке
       bitboard->black_piece[1]=bitboard->black_piece[1] | move_masks_f[n];
	   // пешка от координаты
       bitboard->black_name_from_coord[n]=1;
		break;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			
			default:

//-----------------------------------------------------------------
// в комментариях рассмотрим случай когда у нас 8 пустых клеток
// если зашло '8' то функция Ch_Koordinata_To_Indeks выдает цифру 7 на же нужна цифра 8 
// поэтому используем (1 +... 

		 void_q=(1 + Ch_Koordinata_To_Indeks(fen[i]));
// обработали первую пустую клетку
// и следующие 7 ( таким образом у нас обработаны 1+7=8 все пустые клетки ) 
			for (int f=1;f<void_q;f++){
			 	x=x+1;
			}//for (int f=1;f<l;f++)

		}// switch(fen[i]){
//-----------------------------------------------------------------

		i=i+1;
		x=x+1;
	}//while((fen[i]!='/')&(fen[i]!=' ')){
//--------------------		
// важно не проскочить пробел
	if(fen[i]!=' ')	{i=i+1;y=y+1;}
	  	
}//while(fen[i]!=' '){

// находим все фигуры на доске
bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

//-----------------------------------------------------------------------------------	
//2 этап загоняем очередь хода 	
// проматываем пробел
  while (fen[i]==' '){
	  i=i+1;
  }//while (fen!='\0'){
// w - ход белых  b-ход черных
	if (fen[i]=='w') bitboard->colour_move = 1;
	if (fen[i]=='b') bitboard->colour_move = 0;
//	-----------------------------------------------------------------------------------	
//	3 этап загоняем рокировку
	// пробелы
	  i=i+2;
// сразу присваиваем запрет на рокировку который отменяем ниже если есть нужные символы типа К
    bitboard->castling_Q =0;
    bitboard->castling_K =0;
    bitboard->castling_q =0;
    bitboard->castling_k =0;

	while(fen[i]!=' '){	
		if (fen[i]=='K') bitboard->castling_K =1;// короткая рокировка белых
		if (fen[i]=='Q') bitboard->castling_Q =1;// длинная рокировка белых
		if (fen[i]=='k') bitboard->castling_k =1;// короткая рокировка черных
		if (fen[i]=='q') bitboard->castling_q =1;// длинная рокировка черных
		i=i+1;
	}//while(fen[i]!=' '){	
//	-----------------------------------------------------------------------------------
//	 4 этап загоняем взятие на проходе
/*
 напомним как кодируется битое поле:
 And after the move 1. e4:
 rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
 другими словами нам просто передают координаты битого поля (при ходе е4 битое поле е3)
*/
// проматываем пробел
  while (fen[i]==' '){
	  i=i+1;
  }//while (fen!='\0'){
//		 не забыть при дешифровке что у меня массив по у идет от нуля
//		 а ведь нормальная нотация от 1	 
// запретили взятие
		bitboard->taking_passage_yes =0;
        bitboard->taking_passage_x   =0;
        bitboard->taking_passage_y   =0;

		if (fen[i]!='-'){
			bitboard->taking_passage_yes =1;// разрешаем взятие
			bitboard->taking_passage_x = Koordinata_To_Indeks(fen[i]);// х координата(из буквы)
			i=i+1;
			bitboard->taking_passage_y = Ch_Koordinata_To_Indeks(fen[i]);// у координата 	
		}//if (GUI_To_Engine_Text.charAt(i)!='-'){	
//---------------------------------------------------------------------------
}//board_from_fen(int chess_board[], const char fen[]){
//	*******************************************************************

//	===================================================================
// загоняем в движок мувики т.е. ходы приводящие к текущей позиции из стартовой
void board_from_move(T_Bitboard * bitboard,char move_string[],T_list_surplus_moves * list_surplus_moves_f){

// ходы передаются в виде
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
// просчитываем массив разрешенных ходов
list_surplus_moves_f->current_depth = 0;
list_surplus_moves_f->start_list[0]=1;
list_surplus_moves_f->end_list[0]=1;// оставил с 1 как и i в цикле ниже хотя може перейти на 0 как везде?  
// откуда фигура ходит
int n=8*Ch_Koordinata_To_Indeks(move_string[2])+Koordinata_To_Indeks(move_string[1]);
// куда фигура ходит
int n_2=8*Ch_Koordinata_To_Indeks(move_string[4])+Koordinata_To_Indeks(move_string[3]);
// расчитываем все возможные ходы фигуры
if(bitboard->colour_move == 1){
    Generator_captures_white(bitboard,list_surplus_moves_f);
    Generator_move_white(bitboard,list_surplus_moves_f);
}else{
    Generator_captures_black(bitboard,list_surplus_moves_f);
    Generator_move_black(bitboard,list_surplus_moves_f);
}//
//
int n_3=0; // номер хода по списку
// бежим по списку избыточных ходов
for (int i=list_surplus_moves_f->start_list[0];i<list_surplus_moves_f->end_list[0];i++){
// если ход в списке совпадает с ходом из мувика тогда..
	if ((n== ((list_surplus_moves_f->move[i]>>12)& 63))&&(n_2== ((list_surplus_moves_f->move[i]>>18)& 63))){
 //    n_3 =i;
//------------------
// в зависимости от того есть ли превращение
// что бы понять этот момент нужно знать как передается превращение пешки
// в списке избыточных ходов
	  switch (move_string[5]){
// превращения нет поэтому просто передаем номер хода в списке
   case '\0' :
   //Пустая клетка
   n_3 =i;
   break;

   case 'q' :
   //Ферзь
     n_3 =i;
   break;

   case 'r' :
   //Ладья
     n_3 =i+1;
   break;

   case 'b' :
   //Слон
     n_3 =i+2;
   break;

   case 'n' :
   //Конь
     n_3 =i+3;
   break;

   default:;//Символ фигуры не распознан
   
 }// switch (name){
//------------------

	 break;
	}//if (n== list_surplus_moves->final_position[i]){
}//for (i=1,i<list_surplus_moves->amount_moves+1){

// делаем ход
if(bitboard->colour_move == 1){
    Onward_moves_white(n_3,list_surplus_moves_f,bitboard);
}else{
    Onward_moves_black(n_3,list_surplus_moves_f,bitboard);
}//
 bitboard->colour_move = !bitboard->colour_move;


}//void board_from_move(int chess_board[],char move_string){
//	*******************************************************************

//	===================================================================
//	по букве выдает идекс клетки	
	int Koordinata_To_Indeks(char ch){
			   if (ch =='a') return 0;
			   if (ch =='b') return 1;
			   if (ch =='c') return 2;
			   if (ch =='d') return 3;
			   if (ch =='e') return 4;
			   if (ch =='f') return 5;
			   if (ch =='g') return 6;
			   if (ch =='h') return 7;
			   return 777;
			  }
//	*******************************************************************

//	===================================================================
//	по символу цифры выдает цифру	
	int Ch_Koordinata_To_Indeks(int ch){
			   if (ch =='1') return 0;
			   if (ch =='2') return 1;
			   if (ch =='3') return 2;
			   if (ch =='4') return 3;
			   if (ch =='5') return 4;
			   if (ch =='6') return 5;
			   if (ch =='7') return 6;
			   if (ch =='8') return 7;
			   return 777;
			  }
//	*******************************************************************
