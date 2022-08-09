#include <cstring>
#include <iostream> // TEST
using namespace std;

#include "b_FEN.h"
#include "e_Move.h"
#include "d_Bitboard_gen.h"
#include "j_Hash.h"

//-----------------------------------------------------------------------------------
/* ++
 * ������ ������� �� ������ � fen ����������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 22.02.2009  11.11.2009
 *
 *  ������������
 * ���������������� ������ ���
 * ������ ��������� � ������ b_Protokol_UCI
 * ����� ����� �������� �������� ������������ ������������� ���������� �����
 * ������ �� �������� UCI �������
 */
//---------------------------------------------------------------------------

// ������ ��� �����(����������� ������ ���� ������������ � ������� d_Bitboard_gen.cpp  � i_Forced.cpp)
#define N_KING    6 // ������
#define N_QUEEN   5 // �����
#define N_ROOK    4 // �����
#define N_BISHOP  3 // ����
#define N_KNIGHT  2 // ����
#define N_PAWN    1 // �����

// global
extern class BITBOARDGENERATOR bitboard_generator_o;// ��������� ����� �� �������
extern class MOVES moves_o;// ����������(��������) ����� ��������������� �����������
extern class  HASHM hash_o;// �����������


//	===================================================================
// ��������� ��� ������ ��� � �������������� �����
void FEN::parse_position(struct T_Bitboard * p_bitboard,//�����
					     char string_in[]//�������� ������(65536)
                         ){

// ���������������� ������� ������ ����� ���� ���� ����� 
//"position startpos" ������ ��������� �������
//"position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� ������� � ������� 
//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� ����� ��� �������������

char * fen;  //
char * moves;//
char * ptr;  // ��������������� ���������
char move_string[]={' ',' ',' ',' ',' ',' ',' '};// ��������������� ������
char * StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";// ��������� ���-�������
unsigned __int8 cnt_move = 0;// ������� ����� � ������ ������ ����� ��� ����������� ������


//�������� ��������� �� fen
fen = strstr(string_in,"fen ");

//�������� ��������� �� moves
moves = strstr(string_in,"moves ");

if(fen != NULL) {

    //�������� � �������� ���-��������
    fen_to_board(p_bitboard,fen);

} else {

    // � ��� ������������� ����������� ������� startpos
    // ��������� ��� �� ��� ���������� �� ������ ���� ������ ��������� ������� � ����
    // StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    fen_to_board(p_bitboard,StartFen);

}// if (fen != NULL)

//---------------------------------------------
// ������� �����. ��������� ��� ����������� ����� �� ����� �������
cnt_move = 0;

// �������������� ������ ������ �� ��������� ������ ������������ 
// ��� ������ ������ ������������ ��� ����������� ������������ ���������� �������
hash_o.Ini_hash_string_moves(); 

// �� ������� ������� ���������� ��� - ����
hash_o.Start_position_random_key(p_bitboard// ������������� �����
						  );

// ���������� ���-���� � ������ ��� - ������ �� ������ ����
hash_o.Save_hash_string_moves(p_bitboard->hash_key,cnt_move); 
//---------------------------------------------

// ���� ���������� � ����
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
if(moves != NULL) {

  // ����������� moves 
  ptr = moves + 6;

  while(*ptr != '\0') { 

        move_string[1] = *ptr++;
        move_string[2] = *ptr++;
        move_string[3] = *ptr++;
        move_string[4] = *ptr++;

        if(*ptr == '\0' || *ptr == ' ') {

           move_string[5] = '\0';

        } else { // �����������

           move_string[5] = *ptr++;
           move_string[6] = '\0';

        }//if (*ptr == '\0' || *ptr == ' ') {

        move_to_board(p_bitboard,move_string);

//---------------------------------------------
cnt_move = cnt_move + 1;
// �� ������� ������� ���������� ��� - ����
hash_o.Start_position_random_key(p_bitboard// ������������� �����
						  );

// ���������� ���-���� � ������ ��� -������ �� ������ ����
// ���� ������ ������ ������������ ��� ����������� ������������ ���������� �������
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
//��������� ��� - ������� � �������������� ������ ���������� ���������� Ch_Koordinata_To_Indeks
void FEN::fen_to_board(struct T_Bitboard * p_bitboard,//�����
					   const char fen[]//������ ���������� ��� �������(������ ����������)
                       ){

// �������� �� ������� ���� "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
// �������� / ���������� ����������� ��������� �����
// ����� �������� ���������� ������ ������ �� �����������
// ����� ��� ������ ������ ������� ����� ������ ����� ���� � ��������� ������

//---------------------------------------------
// �������������� ��������(bit board) ����� 
// (������ �� � ��� ������� ��� ����� ������� �� �� ����� ��� ���������)
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

unsigned __int8 i = 0;// ������� ������� ��� ��������� ��� - ������

// ����������� ����� fen
while(fen[i] != ' '){
	i = i + 1;
}//while (fen[i] != ' '){

// ����������� ������
while(fen[i] == ' '){
	i = i + 1;
}//while(fen[i] == ' '){

//-----------------------------------------------------------------------------------	
// 1 ����  ������ ��� ������� - ������� ������ � ������ �����
// ��� ��� �������� ��� �� ����� �� ��� ����� ���������� ���������� xy-������������� �����
// (�� ����� ���� ��� �������� ��� �� ������ ���������� ����� � ������ :))
unsigned __int8 y = 0;// ������������ ���������� �����
unsigned __int8 x = 0;// �������������� ���������� �����
unsigned __int8 n = 0;// ���������� ���������� ����� (����� ����� ���������� �� ������)
unsigned __int8 void_q;// ���������� ������ ������

// ����� ������������� ����� � ������� � ��� �������� ������ ����
// 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1

// � ����� �� ������������ ��������� ����  1k1r4/8/8/8/8/8/8/3R2K1
// ������ ��������� ������� ������
// ���� �� � �.�. ������������ �����������
while(fen[i] != ' '){

	x = 0;
    //  --------------------
    //  ���� �� � ����� �� �����������
	while( (fen[i] != '/') && (fen[i] != ' ') ){

      //  ��������� ���������� ���������� �� � ���������� n
      n = 8 * (7 - y) + x;

      ////////////////////////////////////////////////////////////////
	  // ����������� ����� � ������ ������
	  switch(fen[i]){
     
		case 'K':// ����� ������

          // ��������� ������ � ������ ���� ����� �����
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // ��������� ������ � ������ ����� �������
          p_bitboard->white_piece[N_KING] = p_bitboard->white_piece[N_KING] | move_masks_f[n];

	      // ������ ���������� ������ �� �����
          p_bitboard->white_name_from_coord[n] = N_KING;

		  break;

		case 'Q':// ����� �����

          // ��������� ����� � ������ ���� ����� �����
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // ��������� ����� � ������ ����� ������
          p_bitboard->white_piece[N_QUEEN] = p_bitboard->white_piece[N_QUEEN] | move_masks_f[n];

	      // ������ ���������� ����� �� �����
          p_bitboard->white_name_from_coord[n] = N_QUEEN;

		  break;

		case 'R':// ����� �����

          // ��������� ����� � ������ ���� ����� �����
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

	      // ��������� ����� � ������ ����� �����
          p_bitboard->white_piece[N_ROOK] = p_bitboard->white_piece[N_ROOK] | move_masks_f[n];

	      // ������ ���������� ����� �� �����
          p_bitboard->white_name_from_coord[n] = N_ROOK;

		  break;

		case 'B':// ����� ����

          // ��������� ����� � ������ ���� ����� �����
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // ��������� ����� � ������ ����� ������
          p_bitboard->white_piece[N_BISHOP] = p_bitboard->white_piece[N_BISHOP] | move_masks_f[n];

	      // ������ ���������� ����� �� �����
          p_bitboard->white_name_from_coord[n] = N_BISHOP;

		  break;

		case 'N':// ����� ����

          // ��������� ���� � ������ ���� ����� �����
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

          // ��������� ���� � ������ ����� �����
          p_bitboard->white_piece[N_KNIGHT] = p_bitboard->white_piece[N_KNIGHT] | move_masks_f[n];

	      // ������ ���������� ���� �� �����
          p_bitboard->white_name_from_coord[n] = N_KNIGHT;

		  break;

		case 'P':// ����� �����

          // ��������� ����� � ������ ���� ����� �����
          p_bitboard->all_white_piece = p_bitboard->all_white_piece | move_masks_f[n];

	      // ��������� ����� � ������ ����� �����
          p_bitboard->white_piece[N_PAWN] = p_bitboard->white_piece[N_PAWN] | move_masks_f[n];

	      // ������ ���������� ����� �� �����
          p_bitboard->white_name_from_coord[n] = N_PAWN;

		  break;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//      ������

		case 'k'://������ ������

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_KING] = p_bitboard->black_piece[N_KING] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_KING;

		  break;

		case 'q'://������ �����

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_QUEEN] = p_bitboard->black_piece[N_QUEEN] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_QUEEN;

		  break;

		case 'r'://������ �����

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_ROOK] = p_bitboard->black_piece[N_ROOK] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_ROOK;

		  break;

		case 'b'://������ ����

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_BISHOP] = p_bitboard->black_piece[N_BISHOP] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_BISHOP;

		  break;

		case 'n'://������ ����

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_KNIGHT] = p_bitboard->black_piece[N_KNIGHT] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_KNIGHT;

		  break;

		case 'p'://������ �����

          p_bitboard->all_black_piece = p_bitboard->all_black_piece | move_masks_f[n];

          p_bitboard->black_piece[N_PAWN] = p_bitboard->black_piece[N_PAWN] | move_masks_f[n];

          p_bitboard->black_name_from_coord[n] = N_PAWN;

		  break;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			
		default:

       //-----------------------------------------------------------------
       // � ������������ ���������� ������ ����� � ��� 8 ������ ������
       // ���� ����� '8' �� ������� Ch_Koordinata_To_Indeks ������ ����� 7 �� �� ����� ����� 8 
       // ������� ���������� (1 +... 

	    void_q = (1 + Ch_Koordinata_To_Indeks(fen[i]));

        // ���������� ������ ������ ������
        // � ��������� 7 ( ����� ������� � ��� ���������� 1 + 7 = 8 ��� ������ ������ ) 
	    for(unsigned __int8 f = 1; f < void_q; f++){
		   x = x + 1;
	    }//for (int f=1;f<l;f++)

	 }// switch(fen[i]){
     ////////////////////////////////////////////////////////////////

	 i = i + 1;
	 x = x + 1;

   }//while( (fen[i] != '/') && (fen[i] != ' ') ){

   //--------------------		
   // ����� �� ���������� ������
   if(fen[i] != ' '){
	   i = i + 1;
	   y = y + 1;
   }//if(fen[i] != ' ')
	  	
}//while(fen[i] != ' '){

// ������� ��� ������ �� �����
p_bitboard->all_piece = p_bitboard->all_white_piece | p_bitboard->all_black_piece;

// ������ ��� �������� ���������� ��������� ����
// w - - 0 1 ��� b KQkq e3 0 1 ���� ���� ��������� � ������ �� �������
//-----------------------------------------------------------------------------------	
//2 ���� �������� ������� ���� 	
// ����������� ������
while(fen[i] == ' '){
	i = i + 1;
}//while(fen[i] == ' ')

// w - ��� �����  b-��� ������
if(fen[i] == 'w') p_bitboard->colour_move = 1;
if(fen[i] == 'b') p_bitboard->colour_move = 0;

//	-----------------------------------------------------------------------------------	
//	3 ���� �������� ���������
// �������
i = i + 2;

// ����� ����������� ������ �� ��������� ������� �������� ���� ���� ���� ������ ������� ���� �
p_bitboard->castling_Q = 0;
p_bitboard->castling_K = 0;
p_bitboard->castling_q = 0;
p_bitboard->castling_k = 0;

while(fen[i] != ' '){	
	if(fen[i] == 'K') p_bitboard->castling_K = 1;// �������� ��������� �����
	if(fen[i] == 'Q') p_bitboard->castling_Q = 1;// ������� ��������� �����
	if(fen[i] == 'k') p_bitboard->castling_k = 1;// �������� ��������� ������
	if(fen[i] == 'q') p_bitboard->castling_q = 1;// ������� ��������� ������
	i = i + 1;
}//while(fen[i] != ' '){	

//	-----------------------------------------------------------------------------------
//	 4 ���� �������� ������ �� �������
/*
 �������� ��� ���������� ����� ����:
 And after the move 1. e4:
 rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
 ������� ������� ��� ������ �������� ���������� ������ ���� (��� ���� �4 ����� ���� �3)
*/
// ����������� ������
while(fen[i] == ' '){
	i = i + 1;
}//while(fen[i] == ' ')

//		 �� ������ ��� ���������� ��� � ���� ������ �� � ���� �� ����
//		 � ���� ���������� ������� �� 1	 
// ��������� ������
p_bitboard->taking_passage_yes = 0;
p_bitboard->taking_passage_x   = 0;
p_bitboard->taking_passage_y   = 0;

if(fen[i] != '-'){
	p_bitboard->taking_passage_yes = 1;// ��������� ������
	p_bitboard->taking_passage_x = Koordinata_To_Indeks(fen[i]);// � ����������(�� �����)
	i = i + 1;
	p_bitboard->taking_passage_y = Ch_Koordinata_To_Indeks(fen[i]);// � ���������� 	
}//if(fen[i] != '-'){
//---------------------------------------------------------------------------

}//void FEN::fen_to_board
//	*******************************************************************

//	===================================================================
//�������� � ������ ������(moves) �.�. ���� ���������� � ������� ������� �� ���������
void FEN::move_to_board(struct T_Bitboard * p_bitboard,//�����
					    char move_string[]//������ ���������� ����(7)
                        ){

struct T_undo undo[1];//
struct T_list_surplus_moves list_surplus_moves_f;//������ ���������� �����

// ���� ���������� � ����
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
// ������������ ������ ����������� �����
unsigned __int8 n = 8 * Ch_Koordinata_To_Indeks(move_string[2]) 
                    + Koordinata_To_Indeks(move_string[1]);// ������ ������ �����

unsigned __int8 n_2 = 8 * Ch_Koordinata_To_Indeks(move_string[4]) 
                      + Koordinata_To_Indeks(move_string[3]);// ���� ������ �����

// ������������ ��� ��������� ���� ������
if(p_bitboard->colour_move == 1){
    bitboard_generator_o.Generator_captures_white(p_bitboard,&list_surplus_moves_f);
    bitboard_generator_o.Generator_move_white(p_bitboard,&list_surplus_moves_f);
}else{
    bitboard_generator_o.Generator_captures_black(p_bitboard,&list_surplus_moves_f);
    bitboard_generator_o.Generator_move_black(p_bitboard,&list_surplus_moves_f);
}//

__int16 n_3 = -1; // ����� ���� �� ������

// ����� �� ������ ���������� �����
for(unsigned __int8 i = 0; i < list_surplus_moves_f.end_list; i++){

// ���� ��� � ������ ��������� � ����� �� ������(moves) �����..
	if (( n == ( (list_surplus_moves_f.move[i] >> 12) & 63 ) ) && // ���������� ������ ������ �����
		( n_2 == ((list_surplus_moves_f.move[i] >> 18) & 63) )    // ���������� ���� ������ �����
		){
 
               //------------------
			/*
                � ����������� �� ���� ���� �� ����������� �� ������������ ������ ����
                ��� �� ������ ���� ������ ����� ����� ��� ���������� ����������� �����
                � ������ ���������� �����
                ��� ������ �� ��������� ����������� ���� �� � ���������� 
				������ ����� �� �������������
                ��� ��� ��� ���������� ������� ���� ����������� � �����
                ����� ���������� ���� ����������� � ����� ����� � ����
                ����� ������� ��� � ������������� � ������
			*/
	           switch(move_string[5]){

                  // ����������� ��� ������� ������ �������� ����� ���� � ������
                  case '\0' :
                     // ������ ������
                     n_3 = i;
                     break;

                  case 'q' :
                     // ����������� � �����
                     n_3 = i;
                     break;

                  case 'r' :
                     // ����������� � �����
                     n_3 = i + 1;
                     break;

                 case 'b' :
                     // ����������� � ����
                     n_3 = i + 2;
                     break;

                 case 'n' :
                     // ����������� � ����
                     n_3 = i + 3;
                     break;

                 default:;//������ ������ �� ���������
   
               }// switch(move_string[5]){
               //------------------

	 break;// ������ ��� ����� ��� ��� ������ ������ �� ����� ������ ���
	       // �������� ������� �� ����� for(int i = 0; i < list_surplus_moves_f.end_list; i++)

	}//	if (( n == ( (list_surplus_moves_f.move[i] >> 12) & 63 ) ) &&
	

}//for(int i = 0; i < list_surplus_moves_f.end_list; i++)

//----------------------------------------------------------
if(n_3 == -1){
    cout << "WARNING ! moves error" << "\n";
}else{

    // ������ ���
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
//	�� ����� ������ ������ ������(���� �� ����� ������ ���� ������� :))	
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
//	�� ������� ����� ������ �����
//������� ��� �� ����� ����� �� �����(java) :)
//�������� ������� ����������� � ���� ��� ���� �� 9.01.06 
//������� ��� ������������ �������� :)) 
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
// � ������� ���� ����� ����� � ������� ������ �� �������� �������
const unsigned __int64 FEN::move_masks_f[] ={

// � ���������� ����� ����� ��� ���������� � 1
1,           // 0 bit �.�. ....0001
2,           // 1 bit �.�. ....0010
4,           // 2 bit �.�. ....0100 
8,           // 3 bit � �.�.
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
