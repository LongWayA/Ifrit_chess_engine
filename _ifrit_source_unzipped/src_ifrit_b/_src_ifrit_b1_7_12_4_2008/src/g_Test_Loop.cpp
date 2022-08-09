
#include <fstream.h>  // TEST
#include <iostream.h>


#include "b_Protokol_UCI.h"
#include "g_Test_Loop.h"
#include "k_Timer.h"
//-----------------------------------------------------------------------------------
/*
 * ������������ �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 13.10.2007
 *
 �������� ������� ���� � ����� ������ Roman Hartmann
http://mypage.bluewin.ch/romanhartmann/perft.html
Intro
When writing a new move generator or chess engine, there is often the need to check if 
there are some hidden bugs in the move generation process. And believe me, usually there 
are some well hidden bugs. Playing a lot of games is one way to figure out that everything 
is fine but that is a very timeconsuming approach. Another and much better solution is to 
implement 'perft' into the chess engine. Perft counts by definition all the legal nodes 
(positions) from depth-1 to depth. So if perft 1 is called with the startposition on the 
board the answer should be 20. 'perft 2' should give the answer 400 because 420-20=400. 
Keep in mind that for some of the testpositions a 64-bit counter is needed as the numbers 
getting big quickly. That would be long long on UNIX/Linux systems or __int64 for the windows 
world. If unsure about that, just check your compiler/OS documentation. 

*/
//---------------------------------------------------------------------------


//	===================================================================
// ��������� ������ ������� �.�. ��������� ������ �������
void Test_Loop_Perft(){
 ofstream Test_Loop;
// char string_in[65536];// ����� ���� �������� ������ �� ������
 char three_position[65536];// �������� ������ ��� ������� ������������ ���������� �������
 T_Bitboard bitboard;//���������� ����� ������ 
 int w=0;
 int Nodes=0;
 int Nodes_True[4][100];

 const char string_in_go[][20]=
 {" ",
  "go depth 1\n",
  "go depth 2\n",
  "go depth 3\n",
  "go depth 4\n",
  "go depth 5\n",
  "go depth 6\n",
  "go depth 7\n",
  "go depth 8\n",
  "go depth 9\n",
  "go depth 10\n"};


char string_coment[][100]=
{" ",
"��������� �������==========================================================",
"�������� �������� �������==================================================",
//Good testposition
//The startposition isn't a very good testposition as it doesn't uncover flaws that might 
//still be hidden regarding promotion or castling. Therefore the following position should 
//be a better test. Still, even if this position is no problem for an engine, it doesn't mean 
//there are no bugs left. The link below leads to a textfile with a lot of positions with 
//precalculated perft values. The positions were collected/calculated by Andrew 
//Wagner and published in 2004 on CCC. 

"������ �����������=========================================================",
//Discover promotion bugs
//The following position doesn't look very realistic but nevertheless helps to 
//uncover promotion bugs

};


char string_in[][100]=
{" ",
"position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
"position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
"position fen n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",
};

// ��� ��������� �������
Nodes_True[1][0]=0;
Nodes_True[1][1]=20;
Nodes_True[1][2]=420;
Nodes_True[1][3]=9322;
Nodes_True[1][4]=206603;
Nodes_True[1][5]=5072212;
Nodes_True[1][6]=124132536;
//Nodes_True[1][7]=3320034396;

// ��� ������� �������
Nodes_True[2][0]=0;
Nodes_True[2][1]=48;
Nodes_True[2][2]=2087;
Nodes_True[2][3]=99949;
Nodes_True[2][4]=4185552;
Nodes_True[2][5]=197876242;
//Nodes_True[2][6]=8229523927;

// ��� �����������
Nodes_True[3][0]=0;
Nodes_True[3][1]=24;
Nodes_True[3][2]=520;
Nodes_True[3][3]=10003;
Nodes_True[3][4]=192841;
Nodes_True[3][5]=3797944;
Nodes_True[3][6]=74977083;

double time=0;

 Test_Loop.open ("TEST_LOOP_PERFT.txt");
 Test_Loop << "������ ������"<<"\n";

for (int j=1;j<4;j++){//����� �� ������ ������������� �������
 Test_Loop << string_coment[j]<<"\n";
 for (int i=1;i<5;i++){ // ����� �� ������� �������
Nodes=i;
////////////////////////////////////////////////////////////////////////////////////////////////
//  Parse_Protokol_UCI(string_in[j],three_position,&board_list_816,Nodes);
  Parse_Protokol_UCI(&bitboard,string_in[j],three_position,Nodes);
Timer_start();
  Parse_Protokol_UCI(&bitboard,string_in_go[i],three_position,Nodes);
Timer_stop();
time=Timer_delta();
// Parse_Protokol_UCI(ff,three_position,&board_list_816,Nodes);

  Test_Loop << "-----------------------------------------------"<<"\n";
  Test_Loop << "������� "<<i<<"\n";
  Test_Loop << "����� ������� "<<time<<"\n";
  Test_Loop << "�������� ������� "<<(Nodes/time)<<"\n";
  Test_Loop << "���������� ������������� ������� ������ ���� "<<Nodes_True[j][i]<<"\n";
  Test_Loop <<"�������� ���������� =                        "<<Nodes<<"\n";
  if (Nodes_True[j][i]==Nodes){
    Test_Loop << "OK"<<"\n";
  }else{
	Test_Loop << "Attention result false #######################################"<<"\n";
  }// if (Nodes_True=Nodes){
  Test_Loop << "-----------------------------------------------"<<"\n";
  Test_Loop << " "<<"\n";
/////////////////////////////////////////////////////////////////////////////////////////////////
 }//for (int i=0;i<5;i++){ 
}//for (int j=1;j<4;j++){
 Test_Loop << "������ ����������"<<"\n";
 Test_Loop.close();

//cin>>w;

}//void Test_Loop_Perft(){
//	*******************************************************************




