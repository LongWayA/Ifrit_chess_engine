#include <fstream.h>  // TEST
#include <stdlib.h>


#include "j_Killer.h"
#include "k_Util.h"

//-----------------------------------------------------------------------------------
/* ? ����� ����������� ��� ����������
 * ��������� �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 13.1.2008
 *
 *  ������������
 * ������ ���� ��� ��� ���� ����� ����� ��������
*/
//---------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


// ���������� ����������
 T_killer_moves killer_moves[100]={0};


//	===================================================================
//
void Save_killer(short int i,T_list_surplus_moves * list_surplus_moves,short int depth){

 killer_moves[depth].initial_position =list_surplus_moves->initial_position[i];
 killer_moves[depth].final_position   =list_surplus_moves->final_position[i];

}//void Save_killer(short int i,T_list_surplus_moves * list_surplus_moves,short int depth){
//	*******************************************************************

//	===================================================================
void Insert_killer(T_list_surplus_moves * list_surplus_moves,short int depth,short int start){
 // ��� ������ 1- ����� 2- ����� 3- ���� 4- ���� 5- ����� 6- ������ 
short int name_figure_s=0;
 // ��� ������ ������ 1- ����� 2- ����� 3- ���� 4- ���� 5- ����� 6- ������ 
short int name_taking_figure_s=0;
// ��������� ��������� ������(������ ������ �����)
short int initial_position_s=0; 
// �������� ��������� ������(���� ������ �����)
short int final_position_s=0;
// �������� �� ��� ����� 0, ������� 1, ������� �� ������� 2, ���������� 3,
// ������������ 4, ������������ � ������� 5
short int description_move_s=0;

// 1 ���� ������ ��� � ������
for (short int j=start;(!((list_surplus_moves->initial_position[j]==killer_moves[depth].initial_position)&&(list_surplus_moves->final_position[j]==killer_moves[depth].final_position))&&(j<list_surplus_moves->end_list[depth]));j++);	
// ���� ����� ��� ������� �� ���� ������
if (j<list_surplus_moves->end_list[depth]){	
//------------------------------------------------------------------------
// 2 ���������� ������ ��� 
	 name_figure_s        = list_surplus_moves->name_figure[j];
	 name_taking_figure_s = list_surplus_moves->name_taking_figure[j];
     initial_position_s   = list_surplus_moves->initial_position[j];
     final_position_s     = list_surplus_moves->final_position[j];
     description_move_s   = list_surplus_moves->description_move[j];
//------------------------------------------------------------------------

// 3 �������� ��� ������� ����
 for (short int i=j;i>start;i--){	
 	 list_surplus_moves->name_figure[i]        = list_surplus_moves->name_figure[i-1];
	 list_surplus_moves->name_taking_figure[i] = list_surplus_moves->name_taking_figure[i-1];
     list_surplus_moves->initial_position[i]   = list_surplus_moves->initial_position[i-1];
     list_surplus_moves->final_position[i]     = list_surplus_moves->final_position[i-1];
     list_surplus_moves->description_move[i]   = list_surplus_moves->description_move[i-1];
}//for (short int i=j;i>start;i--){	

//------------------------------------------------------------------------
// 4 �� ��������� ������� ���������� �������
	 list_surplus_moves->name_figure[start]        = name_figure_s;
	 list_surplus_moves->name_taking_figure[start] = name_taking_figure_s;
     list_surplus_moves->initial_position[start]   = initial_position_s;
     list_surplus_moves->final_position[start]     = final_position_s;
     list_surplus_moves->description_move[start]   = description_move_s;
//------------------------------------------------------------------------


//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
    Test_Killer.open ("_TEST_KILLER.txt",ios::app);
 //   Test_Hash.open ("_TEST_HASH.txt");
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

#if TEST_K
 Test_Killer << "���������� �������"<< "\n";
 Test_Killer << "start= "<< start << "\n";
 Test_Killer << "j= "<< j << "\n";
 Test_Killer << "i= "<< i << "\n";
 Test_Killer << "i_end= "<< i_end << "\n";
 Test_Killer << "depth= "<< depth << "\n";
 Test_Killer << "list_surplus_moves->end_list[depth]= "<< list_surplus_moves->end_list[depth] << "\n";
 Test_Killer << "killer_moves[depth].initial_position= "<< killer_moves[depth].initial_position << "\n";
 Test_Killer << "killer_moves[depth].final_position= "<< killer_moves[depth].final_position << "\n";
 Test_Killer << "                "<< "\n";
 Test_Killer <<" "<<(x_to_notation(killer_moves[depth].initial_position%16));
 Test_Killer <<""<<(8-killer_moves[depth].initial_position/16);
 Test_Killer <<" "<<(x_to_notation(killer_moves[depth].final_position%16));
 Test_Killer <<""<<(8-killer_moves[depth].final_position/16) << "\n";
#endif 

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
    Test_Killer.close();
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//if (j<list_surplus_moves->end_list[depth]){	

}//Insert_hash_moves(T_list_surplus_moves * list_surplus_moves,int key,T_hash_moves * hash_moves,short int depth){
//	*******************************************************************