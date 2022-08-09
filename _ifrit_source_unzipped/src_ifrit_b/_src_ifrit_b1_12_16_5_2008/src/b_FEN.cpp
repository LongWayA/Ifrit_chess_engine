#include <cstring>

#include "b_FEN.h"
#include "e_Move.h"
#include "d_Bitboard_gen.h"

//-----------------------------------------------------------------------------------
/* ++
 * ������ ������� �� ������ � fen ����������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 10.02.2008
 *
 *  ������������
 * ���������������� ������ ���
 * ������ ��������� � ������ b_Protokol_UCI
 * ����� ����� �������� �������� ������������ ������������� ���������� �����
 * ������ �� �������� UCI �������
 */
//---------------------------------------------------------------------------
T_list_surplus_moves list_surplus_moves_f={0};//������ ���������� �����

//	===================================================================
// ��������� ���������� ��������� ������� � �������������� ������
void parse_position(T_Bitboard * bitboard,const char string_in[]){
// ���������������� ������� ������ ����� ���� ���� ����� 
//"position startpos" ������ ��������� �������
//"position startpos moves b1c3 b8c6 g1f3 g8f6" ��������� ������� + ���� ������� � ������� 
//"position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 " ������� ������� ����� ��� �������������

 char * fen;  //
 char * moves;//
 char * ptr;  // ��������������� ���������
 char move_string[]={' ',' ',' ',' ',' ',' ',' '};// ��������������� ������
 // ��������� ���-�������
 char * StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";

// ������ ��������� �� fen
   fen = strstr(string_in,"fen ");
// ������ ��������� �� moves
   moves = strstr(string_in,"moves ");

   if (fen != NULL) { // "fen" ���� ������� ������������
// ����� �������� � �������� ���-��������
      board_from_fen(bitboard,fen);
   } else {
// � ��� ������������� ����������� ������� startpos
// ��������� ��� �� ��� ���������� �� ������ ���� ������ ��������� ������� � ����
// StartFen = "fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
     board_from_fen(bitboard,StartFen);
   }// if (fen != NULL)

// ���� ���������� � ����
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
   if (moves != NULL) { // ���� ������� "moves" ������������
      // ����������� moves 
      ptr = moves + 6;

      while (*ptr != '\0') { 
         move_string[1] = *ptr++;
         move_string[2] = *ptr++;
         move_string[3] = *ptr++;
         move_string[4] = *ptr++;

         if (*ptr == '\0' || *ptr == ' ') {
            move_string[5] = '\0';
         } else { // �����������
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
// ��������� ������ ��� � �������������� ������ ���������� ���������� Ch_Koordinata_To_Indeks
void board_from_fen(T_Bitboard * bitboard,/* const*/ char fen[]){
// �������� �� ������� ���� "position fen 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1 "
// �������� / ���������� ����������� ��������� �����
// ����� �������� ���������� ������ ������ �� �����������
// ����� ��� ������ ������ ������� ����� ������ ����� ���� � ��������� ������

// �������������� �������� ����� (������ �� � ��� ������� ��� ����� ������� 
// �� �� ����� ��� ���������)
bitboard->all_piece =0;
bitboard->all_white_piece =0;
bitboard->all_black_piece =0;

for(int k=0 ; k<7 ; k++){
  bitboard->white_piece[k]=0;
  bitboard->black_piece[k]=0;
}//for(int i=0 ; i<8 ; i++){

// ��������� �� ���������
for(int j=0 ; j<64 ; j++){
  bitboard->white_name_from_coord[j]=0;
  bitboard->black_name_from_coord[j]=0;
}//for(int j=0 ; j<64 ; j++){
//---------------------------------------------

  int i=0;
// ����������� fen
  while (fen[i]!=' '){
	  i=i+1;
  }//while (fen[i]!=' '){

// ����������� ������
  while (fen[i]==' '){
	  i=i+1;
  }//while (fen[i]!=' '){

//-----------------------------------------------------------------------------------	
// 1 ����  ������ ��� �������- ������� ������ � ������ �����
// ��� ��� �������� ��� �� ����� �� ��� ����� ���������� ���������� xy-������������� �����
int y=0;
int x=0;
int n=0;
int void_q;// ���������� ������ ������
// 1k1r4/8/8/8/8/8/8/3R2K1 w - - 0 1
// ���� �� � �.�. ������������ �����������
while(fen[i]!=' '){
	x=0;
//--------------------
// ���� �� � ����� �� �����������
	while((fen[i]!='/')&&(fen[i]!=' ')){
// 0-��� ������ 1-����� 2-����� 3-���� 4-���� 5-����� 6-������	
    n = 8 *(7 - y) + x;
	 switch(fen[i]){
// �����
		case 'R'://�����
//����� ������
         bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
	   // ����� � ������ (��� ����� 4)
         bitboard->white_piece[4]=bitboard->white_piece[4] | move_masks_f[n];
	   // ����� �� ����������
         bitboard->white_name_from_coord[n]=4;
		break;
		case 'N'://����
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // ���� � ������ (��� ���� 2)
       bitboard->white_piece[2]=bitboard->white_piece[2] | move_masks_f[n];
	   // ���� �� ����������
       bitboard->white_name_from_coord[n]=2;
		break;
		case 'B'://����
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // ����� � ������
       bitboard->white_piece[3]=bitboard->white_piece[3] | move_masks_f[n];
	   // ����� �� ����������
       bitboard->white_name_from_coord[n]=3;
		break;
		case 'Q'://�����
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // ����� � ������
       bitboard->white_piece[5]=bitboard->white_piece[5] | move_masks_f[n];
	   // ����� �� ����������
       bitboard->white_name_from_coord[n]=5;
		break;
		case 'K'://������
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
       // ������ � ������
       bitboard->white_piece[6]=bitboard->white_piece[6] | move_masks_f[n];
	   // ������ �� ����������
       bitboard->white_name_from_coord[n]=6;
		break;
		case 'P'://�����
       bitboard->all_white_piece =bitboard->all_white_piece | move_masks_f[n];
	   // ����� � ������
       bitboard->white_piece[1]=bitboard->white_piece[1] | move_masks_f[n];
	   // ����� �� ����������
       bitboard->white_name_from_coord[n]=1;
		break;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// 0-��� ������ 1-����� 2-����� 3-���� 4-���� 5-����� 6-������	
// ������
		case 'r':
//������ ������
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
	   // ����� � ������
       bitboard->black_piece[4]=bitboard->black_piece[4] | move_masks_f[n];
	   // ����� �� ����������
       bitboard->black_name_from_coord[n]=4;
		break;
		case 'n':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // ���� � ������
       bitboard->black_piece[2]=bitboard->black_piece[2] | move_masks_f[n];
	   // ���� �� ����������
       bitboard->black_name_from_coord[n]=2;
		break;
		case 'b':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // ����� � ������
       bitboard->black_piece[3]=bitboard->black_piece[3] | move_masks_f[n];
	   // ����� �� ����������
       bitboard->black_name_from_coord[n]=3;
		break;
		case 'q':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // ����� � ������
       bitboard->black_piece[5]=bitboard->black_piece[5] | move_masks_f[n];
	   // ����� �� ����������
       bitboard->black_name_from_coord[n]=5;
		break;
		case 'k':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
       // ������ � ������
       bitboard->black_piece[6]=bitboard->black_piece[6] | move_masks_f[n];
	   // ������ �� ����������
       bitboard->black_name_from_coord[n]=6;
		break;
		case 'p':
       bitboard->all_black_piece =bitboard->all_black_piece | move_masks_f[n];
	   // ����� � ������
       bitboard->black_piece[1]=bitboard->black_piece[1] | move_masks_f[n];
	   // ����� �� ����������
       bitboard->black_name_from_coord[n]=1;
		break;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			
			default:

//-----------------------------------------------------------------
// � ������������ ���������� ������ ����� � ��� 8 ������ ������
// ���� ����� '8' �� ������� Ch_Koordinata_To_Indeks ������ ����� 7 �� �� ����� ����� 8 
// ������� ���������� (1 +... 

		 void_q=(1 + Ch_Koordinata_To_Indeks(fen[i]));
// ���������� ������ ������ ������
// � ��������� 7 ( ����� ������� � ��� ���������� 1+7=8 ��� ������ ������ ) 
			for (int f=1;f<void_q;f++){
			 	x=x+1;
			}//for (int f=1;f<l;f++)

		}// switch(fen[i]){
//-----------------------------------------------------------------

		i=i+1;
		x=x+1;
	}//while((fen[i]!='/')&(fen[i]!=' ')){
//--------------------		
// ����� �� ���������� ������
	if(fen[i]!=' ')	{i=i+1;y=y+1;}
	  	
}//while(fen[i]!=' '){

// ������� ��� ������ �� �����
bitboard->all_piece = bitboard->all_white_piece | bitboard->all_black_piece;

//-----------------------------------------------------------------------------------	
//2 ���� �������� ������� ���� 	
// ����������� ������
  while (fen[i]==' '){
	  i=i+1;
  }//while (fen!='\0'){
// w - ��� �����  b-��� ������
	if (fen[i]=='w') bitboard->colour_move = 1;
	if (fen[i]=='b') bitboard->colour_move = 0;
//	-----------------------------------------------------------------------------------	
//	3 ���� �������� ���������
	// �������
	  i=i+2;
// ����� ����������� ������ �� ��������� ������� �������� ���� ���� ���� ������ ������� ���� �
    bitboard->castling_Q =0;
    bitboard->castling_K =0;
    bitboard->castling_q =0;
    bitboard->castling_k =0;

	while(fen[i]!=' '){	
		if (fen[i]=='K') bitboard->castling_K =1;// �������� ��������� �����
		if (fen[i]=='Q') bitboard->castling_Q =1;// ������� ��������� �����
		if (fen[i]=='k') bitboard->castling_k =1;// �������� ��������� ������
		if (fen[i]=='q') bitboard->castling_q =1;// ������� ��������� ������
		i=i+1;
	}//while(fen[i]!=' '){	
//	-----------------------------------------------------------------------------------
//	 4 ���� �������� ������ �� �������
/*
 �������� ��� ���������� ����� ����:
 And after the move 1. e4:
 rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
 ������� ������� ��� ������ �������� ���������� ������ ���� (��� ���� �4 ����� ���� �3)
*/
// ����������� ������
  while (fen[i]==' '){
	  i=i+1;
  }//while (fen!='\0'){
//		 �� ������ ��� ���������� ��� � ���� ������ �� � ���� �� ����
//		 � ���� ���������� ������� �� 1	 
// ��������� ������
		bitboard->taking_passage_yes =0;
        bitboard->taking_passage_x   =0;
        bitboard->taking_passage_y   =0;

		if (fen[i]!='-'){
			bitboard->taking_passage_yes =1;// ��������� ������
			bitboard->taking_passage_x = Koordinata_To_Indeks(fen[i]);// � ����������(�� �����)
			i=i+1;
			bitboard->taking_passage_y = Ch_Koordinata_To_Indeks(fen[i]);// � ���������� 	
		}//if (GUI_To_Engine_Text.charAt(i)!='-'){	
//---------------------------------------------------------------------------
}//board_from_fen(int chess_board[], const char fen[]){
//	*******************************************************************

//	===================================================================
// �������� � ������ ������ �.�. ���� ���������� � ������� ������� �� ���������
void board_from_move(T_Bitboard * bitboard,char move_string[],T_list_surplus_moves * list_surplus_moves_f){

// ���� ���������� � ����
//moves g1f3 d7d5 b2b3 c8g4 c1b2 b8c6 h2h3 
// ������������ ������ ����������� �����
list_surplus_moves_f->current_depth = 0;
list_surplus_moves_f->start_list[0]=1;
list_surplus_moves_f->end_list[0]=1;// ������� � 1 ��� � i � ����� ���� ���� ���� ������� �� 0 ��� �����?  
// ������ ������ �����
int n=8*Ch_Koordinata_To_Indeks(move_string[2])+Koordinata_To_Indeks(move_string[1]);
// ���� ������ �����
int n_2=8*Ch_Koordinata_To_Indeks(move_string[4])+Koordinata_To_Indeks(move_string[3]);
// ����������� ��� ��������� ���� ������
if(bitboard->colour_move == 1){
    Generator_captures_white(bitboard,list_surplus_moves_f);
    Generator_move_white(bitboard,list_surplus_moves_f);
}else{
    Generator_captures_black(bitboard,list_surplus_moves_f);
    Generator_move_black(bitboard,list_surplus_moves_f);
}//
//
int n_3=0; // ����� ���� �� ������
// ����� �� ������ ���������� �����
for (int i=list_surplus_moves_f->start_list[0];i<list_surplus_moves_f->end_list[0];i++){
// ���� ��� � ������ ��������� � ����� �� ������ �����..
	if ((n== ((list_surplus_moves_f->move[i]>>12)& 63))&&(n_2== ((list_surplus_moves_f->move[i]>>18)& 63))){
 //    n_3 =i;
//------------------
// � ����������� �� ���� ���� �� �����������
// ��� �� ������ ���� ������ ����� ����� ��� ���������� ����������� �����
// � ������ ���������� �����
	  switch (move_string[5]){
// ����������� ��� ������� ������ �������� ����� ���� � ������
   case '\0' :
   //������ ������
   n_3 =i;
   break;

   case 'q' :
   //�����
     n_3 =i;
   break;

   case 'r' :
   //�����
     n_3 =i+1;
   break;

   case 'b' :
   //����
     n_3 =i+2;
   break;

   case 'n' :
   //����
     n_3 =i+3;
   break;

   default:;//������ ������ �� ���������
   
 }// switch (name){
//------------------

	 break;
	}//if (n== list_surplus_moves->final_position[i]){
}//for (i=1,i<list_surplus_moves->amount_moves+1){

// ������ ���
if(bitboard->colour_move == 1){
    Onward_moves_white(n_3,list_surplus_moves_f,bitboard);
}else{
    Onward_moves_black(n_3,list_surplus_moves_f,bitboard);
}//
 bitboard->colour_move = !bitboard->colour_move;


}//void board_from_move(int chess_board[],char move_string){
//	*******************************************************************

//	===================================================================
//	�� ����� ������ ����� ������	
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
//	�� ������� ����� ������ �����	
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
