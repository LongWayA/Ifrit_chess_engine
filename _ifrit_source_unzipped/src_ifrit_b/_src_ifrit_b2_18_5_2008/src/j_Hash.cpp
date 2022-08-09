#include <fstream.h>  // TEST
#include <stdlib.h>


#include "j_Hash.h"
#include "k_Util.h"

//-----------------------------------------------------------------------------------
/* ? ����� ����������� ��� ����������
 * ��������� ��� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 15.12.2007
 *
 *  ������������
 * ������ ���� ��� ��� ���� ����� ����� ��������
*/
//---------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
 ofstream Test_Hash;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


// ���������� ����������
// ������ ��� �����
 T_hash_moves hash_moves[1100000]={0};


// � ������ ��� ����� ��������� ��������������� ������ ��������� ����� 
// ��������� � ������������� ������ �� ����� � ��� ��� ������ �� ���� �����
// ��� ���������� ������ ���������� �������� ���� ��� ���������� ������
// ������ ���� ���������� � �������� ��������� �����
// � ���������� ��������� ��� ����� �� ������� ��������� ���� ��� �� �����
// ���������� ��������� � ������ ��������
// ���� ������, ��� ������, ���������� ������
// 3*7*120=2520 
int random_piese_key[2][7][64];


//	===================================================================
// ������ ��������� ������� ������ ��������� ������ 
// ����������� ��������� �����
void Ini_random_piese_key(){

//� ������ ��������� ��������� ����� ����� ����������� ������� ����������
//<cstdlib>	������������ ������ ��������� ����� � ��������� 0 �� 32767

//int rand ( void ); 
//Returns a pseudo-random integral number in the range 0 to RAND_MAX.
//RAND_MAX is a constant defined in <cstdlib>. Its default value may vary between 
//implementations but it is granted to be at least 32767.

 for(int c=0 ; c<2 ; c++){
	 for(int n=1 ; n<7 ; n++){
		for(int q=0 ; q<64 ; q++){
              random_piese_key[c][n][q]=rand();
//Test_Hash << "random_piese_key[" << c << "][" << n << "][" << q << "]= " << random_piese_key[c][n][q] << "\n";
	    }//for(int q=0 ; q<120 ; q++){
	 }//for(int n=1 ; n<7 ; n++){
 }//for(int c=1 ; c<3 ; c++){

}//Ini_random_piese_key(){
//	*******************************************************************

//	===================================================================
// �������������� ��� ����� -1
void Ini_hash_moves(){

for(int ihm=0 ; ihm<1100000 ; ihm++){
  hash_moves[ihm].key =-1;
}//for(int ihm=0 ; ihm<1100000 ; ihm++){


}//Ini_hash_moves(){
//	*******************************************************************

void Save_best_hash_key(int i,T_list_surplus_moves * list_surplus_moves,int key,int depth){
// �������� ������ �� ������ ������ ���
   if (hash_moves[key].key ==-1){
      Save_hash_key(i,list_surplus_moves,key,hash_moves,depth);
   }else{
// ������ �������� ���� ����� ������ ����� ��������
	   if(hash_moves[key].depth >= depth){ 
          Save_hash_key(i,list_surplus_moves,key,hash_moves,depth);
	   }//if(hash_moves[key].depth >= depth){   
   }//if (hash_moves[key].key ==-1){
}

//	===================================================================
//
void Save_hash_key(int i,T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,int depth){

hash_moves[key].key=key;
hash_moves[key].depth=depth;
hash_moves[key].initial_position =((list_surplus_moves->move[i]>>12)& 63);
hash_moves[key].final_position   =((list_surplus_moves->move[i]>>18)& 63);

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.open ("_TEST_HASH.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if TEST_H
 Test_Hash << "���������� ���"<< "\n";
 Test_Hash << "key= "<< key << "\n";
 Test_Hash << "depth= "<< depth << "\n";
 Test_Hash << "hash_moves[key].initial_position= "<< hash_moves[key].initial_position << "\n";
 Test_Hash << "hash_moves[key].final_position= "<< hash_moves[key].final_position << "\n";
 Test_Hash << "                "<< "\n";
 Test_Hash <<" "<<(x_to_notation(hash_moves[key].initial_position%16));
 Test_Hash <<""<<(8-hash_moves[key].initial_position/16);
 Test_Hash <<" "<<(x_to_notation(hash_moves[key].final_position%16));
 Test_Hash <<""<<(8-hash_moves[key].final_position/16) << "\n";
#endif 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//Save_hash_key(int i,T_list_surplus_moves * list_surplus_moves,T_board_list_816 * board_list_816,T_hash_moves * hash_moves){
//	*******************************************************************

//	===================================================================
void Look_hash(T_list_surplus_moves * list_surplus_moves,int key,int depth,int & insert_hash){
// ������� �� ����������� �� ��� ����� ������� ���� ����������� �� �� ��� ������� ������
   if (hash_moves[key].key == key ){
      Insert_hash_moves(list_surplus_moves,key,hash_moves,depth,insert_hash);
   }//if (hash_moves[board_list_816->hash_key].key == board_list_816->hash_key ){
  }// Look_hash(T_list_surplus_moves * list_surplus_moves,int key,int depth){
//	*******************************************************************

//	===================================================================
void Print_hash_moves(){
//ofstream Test_Loop;
//int key;
//  Test_Loop.open ("_TEST_LOOP.txt",ios::app);
//
////for(int hm=1 ; hm<1000 ; hm++){
////   Test_Loop << " hash_moves[ " << hm << "].key= " << hash_moves[hm].key  << "\n";
//////  Test_Loop << hash_moves[hm] << "\n";
////}
//Test_Loop << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< "\n";
//key=574582;
//Test_Loop << " hash_moves[ " << key  << "].key= " << hash_moves[key].key  << "\n";
//Test_Loop << "hash_moves[key].initial_position= "<< hash_moves[key].initial_position << "\n";
//Test_Loop << "hash_moves[key].final_position= "<< hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=574484;
//Test_Loop << " hash_moves[ " << key  << "].key= " << hash_moves[key].key  << "\n";
//Test_Loop << "hash_moves[key].initial_position= "<< hash_moves[key].initial_position << "\n";
//Test_Loop << "hash_moves[key].final_position= "<< hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=563472;
//Test_Loop << " hash_moves[ " << key  << "].key= " << hash_moves[key].key  << "\n";
//Test_Loop << "hash_moves[key].initial_position= "<< hash_moves[key].initial_position << "\n";
//Test_Loop << "hash_moves[key].final_position= "<< hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//key=585502;
//Test_Loop << " hash_moves[ " << key  << "].key= " << hash_moves[key].key  << "\n";
//Test_Loop << "hash_moves[key].initial_position= "<< hash_moves[key].initial_position << "\n";
//Test_Loop << "hash_moves[key].final_position= "<< hash_moves[key].final_position << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].initial_position%16));
//Test_Loop <<""<<(8-hash_moves[key].initial_position/16);
//Test_Loop <<" "<<(x_to_notation(hash_moves[key].final_position%16));
//Test_Loop <<""<<(8-hash_moves[key].final_position/16) << "\n";
//Test_Loop << "                "<< "\n";
//Test_Loop << "==================================="<< "\n";
//
//Test_Loop.close();

}//Print_hash_moves(){
//	*******************************************************************

//	===================================================================
// ������������� ���� ��������� �������
void Start_position_random_key(T_Bitboard * bitboard){
int  n_1 = 0;
int  colour=0;
int  name=0;

//Integer.         4bytes            signed: -2147483648 to 2 147 483 647
//32767*32=1 048 544 ��������� ������� ����� ������ ���� ����
int key=32767;

n_1 = 0;
while (n_1<64){
      
      name = bitboard->white_name_from_coord[n_1];
      key = key + random_piese_key[1][name][n_1];

      name = bitboard->black_name_from_coord[n_1];
      key = key + random_piese_key[0][name][n_1];

  n_1 = n_1 + 1;
}//while (n_1<64){
bitboard->hash_key=key;

}//Start_position_random_key(T_board_list_816 * board_list_816){
//	*******************************************************************

//	===================================================================
// ������ ���� ��� ������ ����
void Modification_up_random_key(int colour_figure,int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
int key=0; // ����������������� ��� ����
int key_mem=0;// ��������� ��� ����
int name_figure=0;// ��� ������ ������� �����
int  initial_position=0;// ��������� ���������� ����
int  final_position=0;//   �������� ���������� ����
//int  colour_figure=0;// ���� ������ ������� �����

key_mem = bitboard->hash_key;
name_figure = ((list_surplus_moves->move[i]>>24)& 7);
initial_position=((list_surplus_moves->move[i]>>12)& 63);
final_position=((list_surplus_moves->move[i]>>18)& 63);
//colour_figure=bitboard->colour_move;

//�� ������ ��� ����� ������� �������� ���� ������ ��������� ����������
//� ���������� ���� ������ �������� ����������
//��� ��� ������� ���������� ��� ����� ������������ ���� �� ���� ������� ������
//����� � ��������� ����������

key=key_mem-random_piese_key[colour_figure][name_figure][initial_position];
key=key+random_piese_key[colour_figure][name_figure][final_position];
bitboard->hash_key=key;


}//void Modification_up_random_key(int i,T_list_surplus_moves * list_surplus_moves,T_board_list_816 * board_list_816){
//	*******************************************************************

//	===================================================================
// ������ ���� ��� �������� ����
void Modification_down_random_key(int colour_figure,int i,T_list_surplus_moves * list_surplus_moves,T_Bitboard * bitboard){
int key=0; // ����������������� ��� ����
int key_mem=0;// ��������� ��� ����
int name_figure=0;// ��� ������ ������� �����
int  initial_position=0;// ��������� ���������� ����
int  final_position=0;//   �������� ���������� ����
//int  colour_figure=0;// ���� ������ ������� �����

key_mem = bitboard->hash_key;
name_figure = ((list_surplus_moves->move[i]>>24)& 7);
initial_position=((list_surplus_moves->move[i]>>12)& 63);
final_position=((list_surplus_moves->move[i]>>18)& 63);
//colour_figure=!bitboard->colour_move;

//�� ������ ��� ����� ������� �������� ���� ������ ��������� ����������
//� ���������� ���� ������ �������� ����������
//��� ��� ������� ���������� ��� ����� ������������ ���� �� ���� ������� ������
//����� � ��������� ����������

key = key_mem + random_piese_key[colour_figure][name_figure][initial_position];
key = key - random_piese_key[colour_figure][name_figure][final_position];
bitboard->hash_key = key;

}//void Modification_down_random_key(int i,T_list_surplus_moves * list_surplus_moves,T_board_list_816 * board_list_816){
//	*******************************************************************




//	===================================================================
// ��������� ��� ��� �� ������ ������� � ������
void Insert_hash_moves(T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,int depth ,int & insert_hash){
// 
int move_s=0;

insert_hash = 0;
int start = 0;

// 1 ���� ��� ��� � ������
//for (int j=start;(!((list_surplus_moves->initial_position[j]==hash_moves[key].initial_position)&&(list_surplus_moves->final_position[j]==hash_moves[key].final_position))&&(j<list_surplus_moves->end_list[depth]));j++);	
for (int j=start;(!((((list_surplus_moves->move[j]>>12)& 63)==hash_moves[key].initial_position)&&(((list_surplus_moves->move[j]>>18)& 63)==hash_moves[key].final_position))&&(j<list_surplus_moves->end_list));j++);	
// ���� ����� ��� ������� �� ���� ������
if (j<list_surplus_moves->end_list){	
//------------------------------------------------------------------------
// 2 ���������� ��� ��� 
  move_s  = list_surplus_moves->move[j];
//------------------------------------------------------------------------

// 3 �������� ��� ������� ����
 for (int i=j;i>start;i--){	
 	 list_surplus_moves->move[i] = list_surplus_moves->move[i-1];
}//for (int i=j;i>start;i--){	

//------------------------------------------------------------------------
// 4 �� ��������� ������� ���������� ��� ���
	 list_surplus_moves->move[start] = move_s;
//------------------------------------------------------------------------
 insert_hash = 1;
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.open ("_TEST_HASH.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if TEST_H
 Test_Hash << "���������� ���"<< "\n";
 Test_Hash << "key= "<< key << "\n";
 Test_Hash << "depth= "<< depth << "\n";
 Test_Hash << "hash_moves[key].initial_position= "<< hash_moves[key].initial_position << "\n";
 Test_Hash << "hash_moves[key].final_position= "<< hash_moves[key].final_position << "\n";
 Test_Hash << "                "<< "\n";
 Test_Hash <<" "<<(x_to_notation(hash_moves[key].initial_position%16));
 Test_Hash <<""<<(8-hash_moves[key].initial_position/16);
 Test_Hash <<" "<<(x_to_notation(hash_moves[key].final_position%16));
 Test_Hash <<""<<(8-hash_moves[key].final_position/16) << "\n";
#endif 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_H
    Test_Hash.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
}//if (j<list_surplus_moves->end_list[depth]){	


}//Insert_hash_moves(T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,int depth){
//	*******************************************************************


