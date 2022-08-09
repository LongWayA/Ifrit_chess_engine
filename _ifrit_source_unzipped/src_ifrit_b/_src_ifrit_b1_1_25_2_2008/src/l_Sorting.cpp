#include <fstream.h>  // TEST
#include <stdlib.h>   // ������������ div

#include "l_Sorting.h"
#include "e_Move.h"
#include "h_Estimation.h"
#include "k_Util.h"
//-----------------------------------------------------------------------------------
/* ? ����� ����������� ��� ����������
 * ���������� �����
 *
 * ����� ChessTerminator75
 * ��������� ��������� 5.1.2008
 *
 *  ������������
 * ������ ���� ��� ��� ���� ����� ����� ��������
*/
//---------------------------------------------------------------------------

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_Sor
 ofstream Test_Loop_s;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT


//	===================================================================
// ��������� ������ ���������� ���������� ������
int Sorting_captures_moves(T_list_surplus_moves * list_surplus_moves,short int m,short int depth){
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

 for (short int n=list_surplus_moves->start_list[depth];n<m;n++){	
   for (short int i=n+1;i<m;i++){
     if((list_surplus_moves->name_taking_figure[n] - list_surplus_moves->name_figure[n])<(list_surplus_moves->name_taking_figure[i] - list_surplus_moves->name_figure[i])){ 
//     if(material[list_surplus_moves->name_figure[n]]>material[list_surplus_moves->name_figure[i]]){ 
// ��������� ������ �� n     
	 name_figure_s        = list_surplus_moves->name_figure[n];
	 name_taking_figure_s = list_surplus_moves->name_taking_figure[n];
     initial_position_s   = list_surplus_moves->initial_position[n];
     final_position_s     = list_surplus_moves->final_position[n];
     description_move_s   = list_surplus_moves->description_move[n];

// �������� �� i � n
	 list_surplus_moves->name_figure[n]        = list_surplus_moves->name_figure[i];
	 list_surplus_moves->name_taking_figure[n] = list_surplus_moves->name_taking_figure[i];
     list_surplus_moves->initial_position[n]   = list_surplus_moves->initial_position[i];
     list_surplus_moves->final_position[n]     = list_surplus_moves->final_position[i];
     list_surplus_moves->description_move[n]   = list_surplus_moves->description_move[i];


// ��������������� ������
	 list_surplus_moves->name_figure[i]        = name_figure_s;
	 list_surplus_moves->name_taking_figure[i] = name_taking_figure_s;
     list_surplus_moves->initial_position[i]   = initial_position_s;
     list_surplus_moves->final_position[i]     = final_position_s;
     list_surplus_moves->description_move[i]   = description_move_s;
	 }//     if(material[list_surplus_moves->name_figure[n]]>material[list_surplus_moves->name_figure[i]]){ 
   }//for (short int i=0;i<m;i++){
 }//for (short int n=0;n<m;n++){	
return n;
}//void Sorting_surplus_moves(T_list_surplus_moves list_surplus_moves,T_list_surplus_moves * sort_list_surplus_moves){
//	*******************************************************************













