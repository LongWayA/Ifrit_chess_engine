#include <fstream>  // TEST

#include "j_Killer.h"
#include "k_Util.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ���������  25.02.2009
*/
//---------------------------------------------------------------------------

/*
 ��������� �������
*/

//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_K
 std::ofstream Test_Killer;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

namespace {
   __int32 killer_moves_1[MAX_DEPTH];
   __int32 killer_moves_2[MAX_DEPTH];
}//namespace {

//	===================================================================
// ����� �������
void SKILLER::Save_killer(const unsigned __int8 i,//����� ���� � ������
				          const struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
				          const unsigned __int8 depth// ������� �������
				          ){
	if (killer_moves_1[depth] != p_list_surplus_moves->move[i]){
         killer_moves_2[depth] = killer_moves_1[depth];
         killer_moves_1[depth] = p_list_surplus_moves->move[i];
	}//if (killer_moves_1[depth].move != p_list_surplus_moves->move[i])

}//void SKILLER::Save_killer
//	*******************************************************************

//	===================================================================
// ��������� ������� � ������ ������(������� start) 
void SKILLER::Insert_killer(struct T_list_surplus_moves * p_list_surplus_moves,// ������ �����
				            const unsigned __int8 depth,// ������� �������
				            unsigned __int8 start//������� � ������� ���� � �� ������� ��������� ������
				            ){

__int32 move_s = 0;
__int32 score_s;// ���������� ��� ����������� ������ ����
unsigned __int8 j;

// 1 ���� ������ 1 ��� � ������
for( j = start

    // ������ �� ��� ��� ���� �� ����� ������ ������ ��� ��� �� �������� ������
	;( (p_list_surplus_moves->move[j] != killer_moves_1[depth]) 
	&& (j < p_list_surplus_moves->end_list) )

	; j++  );

// ���� ����� ��� ������� �� ���� ������
if(j < p_list_surplus_moves->end_list){	
//   ------------------------------------------------------------------------
//   2 ���������� ������ ��� 
	 move_s  = p_list_surplus_moves->move[j];
     score_s = p_list_surplus_moves->sorting_score[j];
//   ------------------------------------------------------------------------

//   3 �������� ��� ������� ����
//   �� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
     for(unsigned __int8 i = j; i > start; i--){	
 	    p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
        p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
     }//for(int i = j; i > start; i--){

//   ------------------------------------------------------------------------
//   4 �� ��������� ������� ���������� �������
	 p_list_surplus_moves->move[start] = move_s;
     p_list_surplus_moves->sorting_score[start] = score_s;
//   ------------------------------------------------------------------------

}//if(j < p_list_surplus_moves->end_list){	

start = start + 1;

// 1 ���� ������ 2 ��� � ������
for( j = start

	// ������ �� ��� ��� ���� �� ����� ������ ������ ��� ��� �� �������� ������
	;( (p_list_surplus_moves->move[j] != killer_moves_2[depth]) 
	&& (j<p_list_surplus_moves->end_list) )

	;j++);

// ���� ����� ��� ������� �� ���� ������
if(j < p_list_surplus_moves->end_list){	
//   ------------------------------------------------------------------------
//   2 ���������� ������ ��� 
	 move_s  = p_list_surplus_moves->move[j];
     score_s = p_list_surplus_moves->sorting_score[j];
//   ------------------------------------------------------------------------

//   3 �������� ��� ������� ����
//   �� ������� ���������� ���� � ���� ��� ���� ��������� ��� ���������� ������� ����
     for(unsigned __int8 i = j; i > start; i--){	
 	    p_list_surplus_moves->move[i] = p_list_surplus_moves->move[i-1];
        p_list_surplus_moves->sorting_score[i] = p_list_surplus_moves->sorting_score[i-1];
     }//for(int i = j; i > start; i--)

//   ------------------------------------------------------------------------
//   4 �� ��������� ������� ���������� �������
	 p_list_surplus_moves->move[start] = move_s;
     p_list_surplus_moves->sorting_score[start] = score_s;
//   ------------------------------------------------------------------------


// TEST
// � ��������� ��������� ������� ��� ����. 
// �� ��� �� ������ ����� ������ ��� ����� ��� ����� ������!
// ���-��� ��� ��������.
// ������� �� ������ �� �������� �����������. 
// ���� ����� ��������� ��� � �������

//   TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
     #if TEST_K
	 Test_Killer.open ("_TEST_KILLER.txt",std::ios::app);
 //     Test_Hash.open ("_TEST_HASH.txt");
     #endif 
//   TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

     #if TEST_K
        Test_Killer << "���������� �������"<< "\n";
        Test_Killer << "start= "<< start << "\n";
 //       Test_Killer << "j= "<< j << "\n";
 //       Test_Killer << "i= "<< i << "\n";
 //       Test_Killer << "i_end= "<< i_end << "\n";
        Test_Killer << "depth= "<< depth << "\n";
        Test_Killer << "p_list_surplus_moves->end_list[depth]= "<< p_list_surplus_moves->end_list << "\n";
        Test_Killer << "killer_moves[depth].initial_position= "<< killer_moves_1[depth] << "\n";
        Test_Killer << "killer_moves[depth].final_position= "<< killer_moves_2[depth] << "\n";
        Test_Killer << "                "<< "\n";
		Test_Killer <<" "<<(UTIL::x_to_notation(killer_moves_1[depth]%16));
        Test_Killer <<""<<(8-killer_moves_2[depth]/16);
        Test_Killer <<" "<<(UTIL::x_to_notation(killer_moves_2[depth]%16));
        Test_Killer <<""<<(8-killer_moves_2[depth]/16) << "\n";
    #endif 

//  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
    #if TEST_K
       Test_Killer.close();
    #endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

}//if(j < p_list_surplus_moves->end_list){	

}//void SKILLER::Insert_killer
//	*******************************************************************

//	===================================================================
// �������������� �������
void SKILLER::Ini_killer(
				         ){
	for (unsigned __int8 i = 0; i < MAX_DEPTH; i++ ){
        killer_moves_2[i] = 0;
        killer_moves_1[i] = 0;
	}//for (i = 0; i < 100; i++ )

}//void SKILLER::Ini_killer
//	*******************************************************************