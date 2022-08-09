#include <iostream> // TEST
#include <fstream>  // TEST
#include <string>

using namespace std;

#include "h_Estimation.h"
#include "k_Util.h"


//-----------------------------------------------------------------------------------
/* +
 * ������ �������
 *
 * ����� ChessTerminator75
 * ��������� ��������� 16.09.2008
 *  ������������
 * ���������������� ������ ���
 * ��� ��� ����� �������� ����� ��������� � ������ d_Loop
*/

/*
 ���� ������� ��� ��� ����� ������ �� ����� �� �������
 ��� �� ��������� ������������ ������������ ����������� �������������� �����

 � ������������� ����� � ��� �������� ���
  A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
  A7(48) B7(49) C7(50) D7(51) E7(52) F7(53) G7(54) H7(55)
  A6(40) B6(41) C6(42) D6(43) E6(44) F6(45) G6(46) H6(47)
  A5(32) B5(33) C5(34) D5(35) E5(36) F5(37) G5(38) H5(39)
  A4(24) B4(25) C4(26) D4(27) E4(28) F4(29) G4(30) H4(31)
  A3(16) B3(17) C3(18) D3(19) E3(20) F3(21) G3(22) H3(23)
  A2(8)  B2(9)  C2(10) D2(11) E2(12) F2(13) G2(14) H2(15)
  A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

  � ������ �������� ����� ������� � ������ ����

  ������� ������� ���������� ������������ �������������� ����� � ����� � ������ ������
  ��������� ���� test[i]

*/

// ��������� ��� ������������ ���������� ������ pst
const __int8 EVAL::c_test[] ={
				         0, 1, 2, 3, 4, 5, 6, 7,
 		                 8, 9,10,11,12,13,14,15,
 		                16,17,18,19,20,21,22,23,
 		                24,25,26,27,28,29,30,31,
 		                32,33,34,35,34,37,38,39,
 		                40,41,42,43,44,45,46,47,
 		                48,49,50,51,52,53,54,55,
 		                56,57,58,59,60,61,62,63,
 		                };

// ������ ������ ���������� ����������(������� ������������ ���������)
const __int8 EVAL::c_Pozicion_k[] ={
 		                   30, 30, 30,-10, 0, -10, 30, 30,
 		                   -5, -5, -5,-10,-10,-10, -5, -5,
 		                  -10,-10,-10,-10,-10,-10,-10,-10,
 		                  -10,-10,-10,-10,-10,-10,-10,-10,
 		                  -10,-10,-10,-10,-10,-10,-10,-10,
 		                  -10,-10,-10,-10,-10,-10,-10,-10,
 		                   -5, -5, -5,-10,-10,-10, -5, -5,
 		                   30, 30, 30,-10, 0, -10 ,30, 30,
 		                  };

// ���� ������ ���������� � ������(������� ������������ ���������)
const __int8 EVAL::c_Pozicion_n[] ={
				        0,0, 0, 0, 0, 0,0,0,
 		                0,5, 5, 5, 5, 5,5,0,
 		                0,5,10, 0, 0,10,5,0,
 		                0,5,10,15,15,10,5,0,
 		                0,5,10,15,15,10,5,0,
 		                0,5,10, 0, 0,10,5,0,
 		                0,5, 5, 5, 5, 5,5,0,
 		                0,0, 0, 0, 0, 0,0,0,
 		                };

// ����� ������ ���������� �������������� ������� ���������(������� ������������ ���������)
const __int8 EVAL::c_Pozicion_b[] ={
 		          0, 0, 0, 0, 0, 0, 0, 0,
 		         10,15,10,10,10,10,15,10,
 		          0,10,15, 0, 0,15,10, 0,
 		          0,20,20,15,15,20,20, 0,
 		          0,20,20,15,15,20,20, 0,
 		          0,10,15, 0, 0,15,10, 0,
 		         10,15,10,10,10,10,15,10,
 		          0, 0, 0, 0, 0, 0, 0, 0,
 		         };

//����� ������ �� �����������(������� ������������ ����������� ���������)
//������ ����������� �������� �������
const __int8 EVAL::c_Pawn[] ={
	              0, 0, 0, 0, 0,  0, 0, 0,
		          0, 0, 0, 0, 0,  0, 0, 0,
		          0, 0, 0, 21,21, 0, 0, 0,
		          0, 0, 0, 25,25, 0, 0, 0,
		          0, 0, 5, 25,25, 5, 0, 0,
		          0, 0, 5, 30,30, 5, 0, 0,
	              30,30,30,35,35,30, 30,30,
		          0, 0, 0, 0, 0,  0, 0, 0
		          };

//����� ������ �� ����������� ���������� ��� ��������
// ����� ������� ������
const __int8 EVAL::c_Pawn_e[] ={
	                  0,  0,  0,  0,  0,  0,  0,  0,
		              0,  0,  0,  0,  0,  0,  0,  0,	           
		             10, 10, 10, 10, 10, 10, 10, 10,           
		             20, 20, 20, 20, 20, 20, 20, 20,
		             30, 30, 30, 30, 30, 30, 30, 30,
	                 40, 40, 40, 40, 40, 40, 40, 40,
	                 50, 50, 50, 50, 50, 50, 50, 50,
		              0,  0,  0,  0,  0,  0,  0,  0,
		            };

//����� ��������� ������ �� �����������
// ����� ������� ������
const __int16 EVAL::c_Perspective_pawn_e[] ={
	                  0,  0,  0,  0,  0,  0,  0,  0,
		              0,  0,  0,  0,  0,  0,  0,  0,	           
		             20, 20, 20, 20, 20, 20, 20, 20,
		             30, 30, 30, 30, 30, 30, 30, 30,
 	                 50, 50, 50, 50, 50, 50, 50, 50,
                     70, 70, 70, 70, 70, 70, 70, 70,
                    150,150,150,150,150,150,150,150,
		              0,  0,  0,  0,  0,  0,  0,  0,
		            };


// � �������� ������ ������ ���������� � ������(������� ������������ ���������)
// ��� ������� ������� �� ������������� ������
const __int8 EVAL::c_Pozicion_k_e[] ={
				          0,0, 0, 0, 0, 0,0,0,
 		                  0,5, 5, 5, 5, 5,5,0,
 		                  0,5,10,10,10,10,5,0,
 		                  0,5,10,15,15,10,5,0,
 		                  0,5,10,15,15,10,5,0,
 		                  0,5,10,10,10,10,5,0,
 		                  0,5, 5, 5, 5, 5,5,0,
 		                  0,0, 0, 0, 0, 0,0,0,
 		                  };

//	===================================================================
// ��������� ������ �� ���������. ���������� ����� �� ������ ������� ����.
void EVAL::Default_parameters_eval(
						          ){

// ������ ��� �����
KING = 90000; // ������
QUEEN =  1200; // ����� (������ 900) ���� 1000 ���� 1100
ROOK =   600; // ����� ���� 500
BISHOP = 400; // ���� ����  350 400
KNIGHT = 400; // ���� ����  350
PAWN =   100; // �����  ����  

// ������ ����� �����: 0 - 0, 1 - �����, 2 - ����, 3- ����, 4 - �����, 5 - �����, 6 - ������;
kod[0]= 0;
kod[1]= PAWN;
kod[2]= KNIGHT;
kod[3]= BISHOP;
kod[4]= ROOK;
kod[5]= QUEEN;
kod[6]= KING ;

// ��� ������ 0 - ��� ������ 1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 6 - ������
// ������ ������������� ��������� (�����(1) � ������(6) �� �����������)
//                0 1 2      3      4    5     6
rubikon_kod[0]= 0;
rubikon_kod[1]= 0;
rubikon_kod[2]= KNIGHT;
rubikon_kod[3]= BISHOP;
rubikon_kod[4]= ROOK;
rubikon_kod[5]= QUEEN;
rubikon_kod[6]= 0;


// ������������ ������
BONUS_CASTLING_O_O   = 30;// ����� �� �������� ���������
BONUS_CASTLING_O_O_O = 30;// ����� �� ������� ���������
BONUS_SHIELD = 20;// ����� �� ��������� ������ ������� ������
BONUS_PAWNN = 20;// ����� �� ��������� �����
PERSPECTIVE_PAWNN = 10;// ��������� ����� ����� �� ������ ��������� (���� 3)
CONTROL_SQUARE = 1;// ����������� ������� ������������ ������� � �������
ATAK_KING = 10;// ����������� ����� �� ������

// ��� ������ ����� ����� ������� ������ � �������� ����� ����� ������
rubikon = QUEEN + ROOK;//����� �������� � ��������(��� ����� � ���� � �����)1600
                                 // ��� ������ ��������� � ����� ������ �� ��������  

// �������������� ������� PST �.�. ����� ������� �� ���������
	for (unsigned __int8 i = 0; i < 64; i++){

        test[i] = c_test[i]; //TEST

        Pozicion_n[i] = c_Pozicion_n[i];
        Pozicion_b[i] = c_Pozicion_b[i];

        Pawn[i] = c_Pawn[i];
        Pawn_e[i] = c_Pawn_e[i];
        Perspective_pawn_e[i] = c_Perspective_pawn_e[i];
        Pozicion_k[i] = c_Pozicion_k[i];
        Pozicion_k_e[i] = c_Pozicion_k_e[i];

	}//for (int i = 0; i < 64; i++){

//cout << "Default_parameters_eval"<< "\n";

}//void EVAL::Ini_parameters_eval
//	*******************************************************************


//	===================================================================
// ��������� ��������� ������ �� �����. ��� ������ ��������� 1 ����� 0
bool EVAL::Read_parameters_eval(
					          ){

ifstream Ini_Eval("Parameters_Eval.txt");// ����� ������������ �����
ofstream Control;// ����� ������������� �����
bool err = 0;// ������� �� ������� ����(1-�� 0-���)
string line;// ����� ��� �����
//char line[256];//
char l[10];// ����� ��� �����
char * ptr;// ��������������� ���������
char line_pst[150];// ����� ��� ����� ������ pst

Control.open ("Control_Parameters_Eval.txt");
    Control << "�������� ������ ��������� � ����� Parameters_Eval.txt." << "\n";
    Control << "�������� ��� ��� �������� ����� ������ �� �����."<< "\n";  
    Control << " "<< "\n"; 

if (Ini_Eval.is_open()){

// ��������� ������-----------------------------------
getline (Ini_Eval,line);// ���������� ������
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

//                �������������� ��� �����
// ������-----------------------------------
getline (Ini_Eval,line);
getline (Ini_Eval,line);
Ini_Eval.get (l,9);
KING = atoi(l);// ������� atoi ��������� ������ �������� � ����� �����
//cout << KING << "\n";
Control << "KING(������) " << KING << "\n";

// �����-----------------------------------
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
Ini_Eval.get (l,9);
QUEEN = atoi(l);// 
//cout << QUEEN << "\n";
Control << "QUEEN(�����) " << QUEEN << "\n";

// �����-----------------------------------
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
Ini_Eval.get (l,9);
ROOK = atoi(l);// 
//cout << ROOK << "\n";
Control << "ROOK(�����) " << ROOK << "\n";

// ����-----------------------------------
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
Ini_Eval.get (l,9);
BISHOP = atoi(l);// 
//cout << BISHOP << "\n";
Control << "BISHOP(����) " << BISHOP << "\n";

// ����-----------------------------------
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
Ini_Eval.get (l,9);
KNIGHT = atoi(l);// 
//cout << KNIGHT << "\n";
Control << "KNIGHT(����) " << KNIGHT << "\n";

// �����-----------------------------------
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
Ini_Eval.get (l,9);
PAWN = atoi(l);// 
//cout << PAWN << "\n";
Control << "PAWN(�����) " << PAWN << "\n";
Control <<" "<< "\n"; 

// ������ ����� �����: 0 - 0, 1 - �����, 2 - ����, 3- ����, 4 - �����, 5 - �����, 6 - ������;
kod[0]= 0;
kod[1]= PAWN;
kod[2]= KNIGHT;
kod[3]= BISHOP;
kod[4]= ROOK;
kod[5]= QUEEN;
kod[6]= KING ;

// ��� ������ 0 - ��� ������ 1 - ����� 2 - ���� 3 - ���� 4 - ����� 5 - ����� 6 - ������
// ������ ������������� ��������� (�����(1) � ������(6) �� �����������)
//                0 1 2      3      4    5     6
rubikon_kod[0]= 0;
rubikon_kod[1]= 0;
rubikon_kod[2]= KNIGHT;
rubikon_kod[3]= BISHOP;
rubikon_kod[4]= ROOK;
rubikon_kod[5]= QUEEN;
rubikon_kod[6]= 0;

//                                        ������������ ������


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ����� �� �������� ���������-----------------------------------
Ini_Eval.get (l,9);
BONUS_CASTLING_O_O = atoi(l);// ������� atoi ��������� ������ �������� � ����� �����
//cout << BONUS_CASTLING_O_O << "\n";
Control << "BONUS_CASTLING_O_O(����� �� �������� ���������) " << BONUS_CASTLING_O_O << "\n";


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ����� �� ������� ���������-----------------------------------
Ini_Eval.get (l,9);
BONUS_CASTLING_O_O_O = atoi(l);//
//cout << BONUS_CASTLING_O_O_O << "\n";
Control << "BONUS_CASTLING_O_O_O(����� �� ������� ���������) " << BONUS_CASTLING_O_O_O << "\n";


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ����� �� ��������� ������ ������� ������-----------------------------------
Ini_Eval.get (l,9);
BONUS_SHIELD = atoi(l);//
//cout << BONUS_SHIELD << "\n";
Control << "BONUS_SHIELD(����� �� ��������� ������ ������� ������) " << BONUS_SHIELD << "\n";


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ����� �� ��������� �����-----------------------------------
Ini_Eval.get (l,9);
BONUS_PAWNN = atoi(l);//
//cout << BONUS_PAWNN << "\n";
Control << "BONUS_PAWNN(����� �� ��������� �����) " << BONUS_PAWNN << "\n";


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ��������� ����� ����� �� ������ ��������� (���� 3)-----------------------------------
Ini_Eval.get (l,9);
PERSPECTIVE_PAWNN = atoi(l);//
//cout << PERSPECTIVE_PAWNN << "\n";
Control << "PERSPECTIVE_PAWNN(��������� ����� ����� �� ������ ��������� (���� 3)) " << PERSPECTIVE_PAWNN << "\n";


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ����������� ������� ������������-----------------------------------
Ini_Eval.get (l,9);
CONTROL_SQUARE = atoi(l);//
//cout << Z << "\n";
Control << "CONTROL_SQUARE(����������� ������� ������������) " << CONTROL_SQUARE << "\n";


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ����������� ����� �� ������-----------------------------------
Ini_Eval.get (l,9);
ATAK_KING = atoi(l);//
//cout << ATAK_KING << "\n";
Control << "ATAK_KING(����������� ����� �� ������) " << ATAK_KING << "\n";


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

// ����� �������� � ��������-----------------------------------
Ini_Eval.get (l,9);
rubikon = atoi(l);//
//cout << rubikon << "\n";
Control << "rubikon(����� �������� � ��������) " << rubikon << "\n";


// �������������� ������� PST �.�. ����� ������� �� ���������-----------------------------------

// ������� ��� ����� � ���������� ��������� �.�. ��� �� ������� �����
// 56 57 58 59 60 61 62 63 |8
// 48 49 50 51 52 53 54 55 |7
// 40 41 42 43 44 45 46 47 |6
// 32 33 34 35 36 37 38 39 |5
// 24 25 26 27 28 29 30 31 |4
// 16 17 18 19 20 21 22 23 |3
// 8  9  10 11 12 13 14 15 |2
// 0  1  2  3  4  5  6  7  |1
// ------------------------
// a  b  c  d  e  f  g  h

getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

//���� ������ ���������� � ������
//---------------------------------------------
for (__int8 y = 56; y > -1; y = y - 8){
    
    Ini_Eval.get (line_pst,100);
//    cout << y << "= " << line_pst << "\n";
    // ������� strtok �������� ��� �����������(����������� �� ���������) ������ ������ ��� ���� ��� ��������� ������
    // ��� ������ ������ ������ ������ � ����� ��� ��� ���������� ���� 
    ptr = strtok(line_pst,","); // 
    Pozicion_n[y] = atoi(ptr);// 
//    cout << "Pozicion_n[" << y << "]= " << Pozicion_n[y] << "\n";

    for(__int8 p = y + 1; p < y + 8; p++){
        ptr = strtok(NULL,","); // 
        Pozicion_n[p] = atoi(ptr);// ������� atoi ��������� ������ � ����� �����
 //       cout << "Pozicion_n[" << p << "]= " << Pozicion_n[p] << "\n";
    }// for(int p = 57; p < 64; p++){

    getline (Ini_Eval,line);
//    cout  << line << "\n";

}//for (int y = 56; y > 0; y = y - 8)
//---------------------------------------------

//-------------------------------------------------------------------------------------
Control << " " << "\n";
Control << "���� ������ ���������� � ������ " << "\n";
	for (__int8 i = 63; i > -1 ; i = i - 8){
       for (__int8 j = i - 7; j < i + 1 ; j++){
          Control << Pozicion_n[j] << ",";
	   }//for ( j=i-7; i>j+1 ; j++){
       Control << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------

getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);


// ����� ������ �� ����������� ������ ����������� �������� ������� �������� �.�. hgfedcba
//---------------------------------------------
for (__int8 y = 56; y > -1; y = y - 8){
    
    Ini_Eval.get (line_pst,100);
//    cout << y << "= " << line_pst << "\n";
    // ������� strtok �������� ��� �����������(����������� �� ���������) ������ ������ ��� ���� ��� ��������� ������
    // ��� ������ ������ ������ ������ � ����� ��� ��� ���������� ���� 
    ptr = strtok(line_pst,","); // 
    Pawn[y] = atoi(ptr);// 
 //   cout << "Pawn[" << y << "]= " << Pawn[y] << "\n";

    for(__int8 p = y + 1; p < y + 8; p++){
        ptr = strtok(NULL,","); // 
        Pawn[p] = atoi(ptr);// ������� atoi ��������� ������ � ����� �����
 //       cout << "Pawn" << p << "]= " << Pawn[p] << "\n";
    }// for(int p = 57; p < 64; p++){

    getline (Ini_Eval,line);
 //   cout  << line << "\n";

}//for (int y = 56; y > 0; y = y - 8)

//-------------------------------------------------------------------------------------
Control << " " << "\n";
Control << "����� ������ �� ����������� ������ ����������� �������� ������� �������� " << "\n";
	for (__int8 i = 63; i > -1; i = i - 8){
       for (__int8 j = i - 7; j < i + 1; j++){
          Control << Pawn[j] << ",";
	   }//for ( j = i - 7; i > j + 1; j++){
       Control << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);

//����� ������ �� ����������� ���������� ��� �������� ����� ������� ������
//---------------------------------------------
for (__int8 y = 56; y > -1; y = y - 8){
    
    Ini_Eval.get (line_pst,100);
//    cout << y << "= " << line_pst << "\n";
    // ������� strtok �������� ��� �����������(����������� �� ���������) ������ ������ ��� ���� ��� ��������� ������
    // ��� ������ ������ ������ ������ � ����� ��� ��� ���������� ���� 
    ptr = strtok(line_pst,","); // 
    Pawn_e[y] = atoi(ptr);// 
//    cout << "Pawn_e[" << y << "]= " << Pawn_e[y] << "\n";

    for(__int8 p = y + 1; p < y + 8; p++){
        ptr = strtok(NULL,","); // 
        Pawn_e[p] = atoi(ptr);// ������� atoi ��������� ������ � ����� �����
 //       cout << "Pawn_e[" << p << "]= " << Pawn_e[p] << "\n";
    }// for(int p = 57; p < 64; p++){

    getline (Ini_Eval,line);
//    cout  << line << "\n";

}//for (int y = 56; y > 0; y = y - 8)

//-------------------------------------------------------------------------------------
Control << " " << "\n";
Control << "����� ������ �� ����������� ���������� ��� �������� ����� ������� ������ " << "\n";
	for (__int8 i = 63; i > -1 ; i = i - 8){
       for (__int8 j = i - 7; j < i + 1 ; j++){
          Control << Pawn_e[j] << ",";
	   }//for ( j=i-7; i>j+1 ; j++){
       Control << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------


getline (Ini_Eval,line);
getline (Ini_Eval,line);
getline (Ini_Eval,line);


//---------------------------------------------
for (__int8 y = 56; y > -1; y = y - 8){
    
    Ini_Eval.get (line_pst,100);
 //   cout << y << "= " << line_pst << "\n";
    // ������� strtok �������� ��� �����������(����������� �� ���������) ������ ������ ��� ���� ��� ��������� ������
    // ��� ������ ������ ������ ������ � ����� ��� ��� ���������� ���� 
    ptr = strtok(line_pst,","); // 
    Pozicion_k_e[y] = atoi(ptr);// 
 //   cout << "Pozicion_k_e[" << y << "]= " << Pozicion_k_e[y] << "\n";

    for(__int8 p = y + 1; p < y + 8; p++){
        ptr = strtok(NULL,","); // 
        Pozicion_k_e[p] = atoi(ptr);// ������� atoi ��������� ������ � ����� �����
//		cout << "Pozicion_k_e[" << p << "]= " << Pozicion_k_e[p] << "\n";
    }// for(int p = 57; p < 64; p++){

    getline (Ini_Eval,line);
//    cout  << line << "\n";

}//for (int y = 56; y > 0; y = y - 8)

//-------------------------------------------------------------------------------------
Control << " " << "\n";
Control << "� �������� ������ ������ ���������� � ������ ��� ������� ������� �� ������������� ������ " << "\n";
	for (__int8 i = 63; i > -1 ; i = i - 8){
       for (__int8 j = i - 7; j < i + 1 ; j++){
          Control << Pozicion_k_e[j] << ",";
	   }//for ( j=i-7; i>j+1 ; j++){
       Control << "\n";
   }//for (int i=63; i>-1 ; i--){
//-------------------------------------------------------------------------------------
//---------------------------------------------

//getline (Ini_Eval,line);
//cout << "1= " << line << "\n";
//getline (Ini_Eval,line);
//cout << "2= " << line << "\n";
//getline (Ini_Eval,line);
//cout << "3= " << line << "\n";
//getline (Ini_Eval,line);
//cout << "4= " << line << "\n";
//getline (Ini_Eval,line);
//cout << "5= " << line << "\n";
//getline (Ini_Eval,line);
//cout << "6= " << line << "\n";

//-------------------------------------------------------------------------------
// �������� ������ PST(����� ������� �� ���������) �� ����������
	//for (int i = 0; i < 64; i++){

 // if (Pozicion_n[i] != c_Pozicion_n[i]) cout << "Pozicion_n  AAAAAAAAAAAA " << "\n";
 // if (Pawn[i] != c_Pawn[i]) cout << "Pawn  AAAAAAAAAAAA " << "\n";
 // if (Pawn_e[i] != c_Pawn_e[i]) cout << "Pawn_e  AAAAAAAAAAAA " << "\n";
 // if (Pozicion_k_e[i] != c_Pozicion_k_e[i]) cout << "Pozicion_k_e  AAAAAAAAAAAA " << "\n";

	//}//for (int i = 0; i < 64; i++){
//-------------------------------------------------------------------------------


	//while (!Ini_Eval.eof()){
 //        getline (Ini_Eval,line);
 //        cout<<line<< "\n";
 //   }//while (!Ini_Eval.eof())

    err = 1;

    // ��������� ���� ���������� ������    
    Ini_Eval.close();

//cout << "Read_parameters_eval"<< "\n";

}//if (Ini_Eval.is_open()){

// ��������� ���� ������ ������    
Control.close();

return err;
}//void Read_parameters_eval
//	*******************************************************************

//	===================================================================
// �������������� ��������� ������ ���� �� �������� �����
// ���� �� ��������� � ������ ���� �� �� ���������� ������� ����
void EVAL::Ini_parameters_eval(
						       ){

    //if (!Read_parameters_eval()) Default_parameters_eval();
    Default_parameters_eval();

}//void EVAL::Ini_parameters_eval
//	*******************************************************************

//	===================================================================
// ������� �������� ������� � ��������� �� ���������� ������� ����������� �������
__int32 EVAL::Eval_State(const T_Bitboard * p_bitboard// ������������� �����
			         ){
__int32 eval = 0;//����������� ������
unsigned __int8 from_white_king = 0;// ������������ ������ ������
unsigned __int8 from_black_king = 0;// ������������ ������� ������
unsigned __int8 at_white_king = 0; // ���������� ����������� ����� ������ ������ ������
unsigned __int8 at_black_king = 0;// ���������� ����������� ����� ������ ������� ������

// ����� ��� ���������� ������ ����
unsigned __int16 stadia_w = 0; // �������� ��� ����� � �����
unsigned __int16 stadia_b = 0; // �������� ��� ����� � ������

//� ��������� ������� �� ����� 400 + 400 + 600 + 1200 = 2600 (��� ����� �������)
//                             2*400 + 2*400 + 2*600 + 1200 
//                             800   + 800   + 1200  + 1200 = 4000 (����� ��� ���� ������) 
for(unsigned __int8 i = 0; i < 64; i++) {
	stadia_w = stadia_w + rubikon_kod[p_bitboard->white_name_from_coord[i]];
    stadia_b = stadia_b + rubikon_kod[p_bitboard->black_name_from_coord[i]];
}//for(int i=0 ;i<64;i++) {

//stadia_w = 2600; // �������� ��� ����� � �����
//stadia_b = 2600; // �������� ��� ����� � ������

// ����������� ������
King_pst( p_bitboard,// ������������� �����
          from_white_king,// ������������ ������ ������
          from_black_king,// ������������ ������� ������
          stadia_w, // �������� ��� ����� � �����
          stadia_b, // �������� ��� ����� � ������
          eval //����������� ������
         );

// ����������� �����
Pawn_pst( p_bitboard,// ������������� �����
          from_white_king,// ������������ ������ ������
          from_black_king,// ������������ ������� ������
          stadia_w, // �������� ��� ����� � �����
          stadia_b, // �������� ��� ����� � ������
          eval //����������� ������
         );

// ����������� �����, �����, ����, ����
// ����� ������������ 
PST( p_bitboard,// ������������� �����
     from_white_king,// ������������ ������ ������
     from_black_king,// ������������ ������� ������
     at_white_king, // ���������� ����������� ����� ������ ������ ������
     at_black_king, // ���������� ����������� ����� ������ ������� ������
     eval //����������� ������
    );


return eval;

}//int EVAL::Eval_State
//	*******************************************************************

//	===================================================================
//
inline void EVAL::King_pst(const T_Bitboard * p_bitboard,// ������������� �����
                           unsigned __int8 & from_white_king,// ������������ ������ ������
                           unsigned __int8 & from_black_king,// ������������ ������� ������
                           unsigned __int16 stadia_w, // �������� ��� ����� � �����
                           unsigned __int16 stadia_b, // �������� ��� ����� � ������
                           __int32 & eval //����������� ������
                           ){

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

unsigned __int8 from; // ���������� ���� ������ ������ �����

/*
����� ����������� ��������� � ��������� ������ �������� 
�� ���� ���� ���������� �� ������������ ������

������� ��� ������� �������� � ��������.
���� ��� ������ ������ - �� �������� � �����. �� �������� ��������� ������ �� ���������
�������� � ��������� � ����� �������� ��� ���.

��������� ��������(�� ����������� �������) ��� ��� ������ ����� �������. �� ������� � ���� ���� �����

*/
// ����� ������
from_mask = p_bitboard->white_piece[6];

if(from_mask != 0) {
   from = first_one(from_mask);
   from_white_king = from;
//cout << "������ " << "\n";
//cout << "from= "<< from << "\n";
//cout << "Pozicion_k_e[from]= "<< Pozicion_k_e[from] << "\n";

   if(stadia_b > rubikon){

//-----------------------------------------------------------
 // �������� ����� � ��� �������� ���
 // .........
 // A1(0)  B1(1)  C1(2)  D1(3)  E1(4)  F1(5)  G1(6)  H1(7)

// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
// � ��������: 64 - 6 bit  128 - 7 bit 64 + 128 = 192 // �������� 6 � 7 ���
//	  if ( 
//          ( (p_bitboard->white_piece[4] & 192) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 6) || (from == 5) ) //  ����� ������ � ���� ��������� 0-0
//		  ) eval = eval - BONUS_CASTLING_O_O; // ����� �������� 
//
//// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
//// � ��������: 1 - 0 bit   2 - 1 bit
//// 1 + 2 = 3 // �������� 0,1 ����
//	  if ( 
//          ( (p_bitboard->white_piece[4] & 3) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 1) || (from == 2) || (from == 3) ) //  ����� ������ � ���� ��������� 0-0-0
//		  ) eval = eval - BONUS_CASTLING_O_O_O; // ����� �������� 
//-----------------------------------------------------------

         eval = eval + Pozicion_k[from] - King_shield(from,1,p_bitboard,BONUS_SHIELD);

     }else{// if(stadia_b > rubikon){

         // ����� �������� ������ ������ ���������� � ������
         eval = eval + Pozicion_k_e[from];

     }// if(stadia_b > rubikon){
}//if(from_mask != 0) {


//������ ������
from_mask = p_bitboard->black_piece[6];

//from_mask = 0;
if(from_mask != 0) {
   from = first_one(from_mask);
   from_black_king = from;

   if(stadia_w > rubikon){

//-----------------------------------------------------------
 // �������� ����� � ��� �������� ���
// A8(56) B8(57) C8(58) D8(59) E8(60) F8(61) G8(62) H8(63)
 // .........

// ���� ��������� 0-0 �.�. ������ ������� ��������� ��� ��������� :)
// � ��������: 4611686018427387904 - 62 bit  9223372036854775808 - 63 bit 
// 4611686018427387904 + 9223372036854775808 = 13835058055282163712  �������� 62 � 63 ���
//	  if ( 
//          ( (p_bitboard->black_piece[4] & 13835058055282163712) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 61) || (from == 62) ) //  ������ ������ � ���� ��������� 0-0
//		  ) eval = eval + BONUS_CASTLING_O_O; // ����� �������� 
//
//// ���� ��������� 0-0-0 �.�. ������ ������� ��������� ��� ��������� :)
//// � ��������: 72057594037927936 - 56 bit   144115188075855872 - 57 bit
//// 72057594037927936 + 144115188075855872 = 216172782113783808  �������� 56,57 ����
//	  if ( 
//          ( (p_bitboard->black_piece[4] & 216172782113783808) != 0 ) //  ����� ������� �� 6 ��� 7 �������
//		  && ( (from == 57) || (from == 58) || (from == 59) ) //  ������ ������ � ���� ��������� 0-0-0
//		  ) eval = eval + BONUS_CASTLING_O_O_O; // ����� �������� 
//-----------------------------------------------------------

         eval = eval - Pozicion_k[from] + King_shield(from,0,p_bitboard,BONUS_SHIELD);

    }else{

       // ����� �������� ������ ������ ���������� � ������
         eval = eval - Pozicion_k_e[from];

    }// if(stadia_w > rubikon){

}//if(from_mask != 0) {

}//inline void EVAL::King
//	*******************************************************************

//	===================================================================
// ������ ������ �� ������������ 
inline void EVAL::Pawn_pst(const T_Bitboard * p_bitboard,// ������������� �����
                           unsigned __int8 & from_white_king,// ������������ ������ ������
                           unsigned __int8 & from_black_king,// ������������ ������� ������
                           unsigned __int16 stadia_w, // �������� ��� ����� � �����
                           unsigned __int16 stadia_b, // �������� ��� ����� � ������
                           __int32 & eval //����������� ������
                           ){


 // ����� ��� ����������� ��������� ����� 
  unsigned __int8 Structure_pawn_w[]={
		     0,0,0,0,0,0,0,0
            };
  unsigned __int8 Structure_pawn_b[]={
	         0,0,0,0,0,0,0,0
            };

unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

unsigned __int8 from; // ���������� ���� ������ ������ �����

bool end; // �������� ����� �������� ��� ���������

// ���������� �������� ����� ����� ������ ������� ������ ���� ���� ������
// �.�. �� ���� ����� ����� ��� - ����� � ������ �� �����
unsigned __int8 cnt_line_pawn_white = 0;
unsigned __int8 cnt_line_pawn_black = 0;

/*

������� ��� ������� �������� � ��������.
���� ��� ������ ������ - �� �������� � �����.

�� ��������� ����� �����

*/

// �����

//�����----------------------------------------------------------------------------------
end = 0;
from_mask = p_bitboard->white_piece[1];
while(from_mask != 0){

   from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

   if(stadia_b > rubikon){
      eval = eval + Pawn[from];
      //eval = eval + test[from];
//cout << "from= "<< from << "\n";
//cout << "test[from]= "<< test[from] << "\n";
   }else{
      end = 1;
      eval = eval + Pawn_e[from];

//cout << "from= "<< from << "\n";
//cout << "from % 8= "<< from % 8 << "\n";

   }// if(stadia_b > rubikon){

	// ���������� ���������
    // � ������ ���� ����� ������ ��� ��������� �����
	if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){ 

		eval = eval + 2 * PERSPECTIVE_PAWNN;// 10
        cnt_line_pawn_white = 0;

		// ��� ������ ������� ���������
        if( ((from % 8) == 7) || ((from % 8) == 0) ) cnt_line_pawn_white = 1;

        // � ������ ���� ������ ����� ������ ��� ��������� �����
		if((from % 8) != 7) if(!(e_rook_up_masks[from + 1] & p_bitboard->black_piece[1])){ 
			eval = eval + PERSPECTIVE_PAWNN;
			cnt_line_pawn_white = cnt_line_pawn_white + 1;
		}//if((from % 8) != 7) if(!(e_rook_up_masks[from + 1] & p_bitboard->black_piece[1])){

        // � ������ ���� ����� ����� ������ ��� ��������� �����
		if((from % 8) != 0) if(!(e_rook_up_masks[from - 1] & p_bitboard->black_piece[1])){
			eval = eval + PERSPECTIVE_PAWNN;
            cnt_line_pawn_white = cnt_line_pawn_white + 1;
		}//if((from % 8) != 0) if(!(e_rook_up_masks[from - 1] & p_bitboard->black_piece[1])){

        // ��� ��� ����� ������� ��� ������ ���������.
        if(cnt_line_pawn_white == 2) eval = eval + end * Perspective_pawn_e[from]; 
		                                        // + 26 * PERSPECTIVE_PAWNN;

	}//if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){ 


 //���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_w[from % 8] = Structure_pawn_w[from % 8] + 1;

}//while(from_mask != 0){


// ������

//�����----------------------------------------------------------------------------------
end = 0;
from_mask = p_bitboard->black_piece[1];
while(from_mask != 0){
   from = first_one(from_mask);
   from_mask = from_mask & (from_mask-1);

   if(stadia_w > rubikon){
     eval = eval - Pawn[63 - from];//
     //eval = eval - test[from];//
   }else{
     end = 1;
     eval = eval - Pawn_e[63 - from];

   }//if(stadia_w > rubikon){

	// ���������� ���������
    // � ������ ���� ����� ������ ��� ��������� �����
	if(!(e_rook_down_masks[from] & p_bitboard->white_piece[1])){ 

		eval = eval - 2 * PERSPECTIVE_PAWNN;
        cnt_line_pawn_black = 0;

		// ��� ������ ������� ���������
        if( ((from % 8) == 7) || ((from % 8) == 0) ) cnt_line_pawn_black = 1;

        // � ������ ���� ������ ����� ������ ��� ��������� �����
		if((from % 8) != 7) if(!(e_rook_down_masks[from + 1] & p_bitboard->white_piece[1])){
			eval = eval - PERSPECTIVE_PAWNN;
            cnt_line_pawn_black = cnt_line_pawn_black + 1;
		}//if((from % 8) != 7) if(!(e_rook_down_masks[from + 1] & p_bitboard->white_piece[1])){

        // � ������ ���� ����� ����� ������ ��� ��������� �����
		if((from % 8) != 0) if(!(e_rook_down_masks[from - 1] & p_bitboard->white_piece[1])){
			eval = eval - PERSPECTIVE_PAWNN;
            cnt_line_pawn_black = cnt_line_pawn_black + 1;
		}//if((from % 8) != 0) if(!(e_rook_down_masks[from - 1] & p_bitboard->white_piece[1])){

        // ��� ��� ����� ������� ��� ������ ���������.
        if(cnt_line_pawn_black == 2) eval = eval - end * Perspective_pawn_e[63 - from]; 
		                                         //- 26 * PERSPECTIVE_PAWNN;

	}//if(!(e_rook_up_masks[from] & p_bitboard->black_piece[1])){


// ���� ���� ��������� ����� �� � ������� ����� ������ ���� ��������� �� ������ �� ��� �����
	Structure_pawn_b[from % 8] = Structure_pawn_b[from % 8] + 1;
}//while(from_mask != 0){

// �������� �� ��������� �����
  for(int ii = 0; ii < 8; ii++){
    if(Structure_pawn_w[ii] > 1) eval = eval - BONUS_PAWNN; // ������ ����� 20
    if(Structure_pawn_b[ii] > 1) eval = eval + BONUS_PAWNN;
    //cout << "ii = "<< ii << "\n";
    //cout << "Structure_pawn_w[ii]= "<< Structure_pawn_w[ii] << "\n";
  }//for(int ii = 0; ii < 8; ii++){

}//inline void EVAL::Pawn
//	*******************************************************************



//	===================================================================
// ������ ������ �� ������������ ������, �����, ������, �����
// ������ ��������������� ������������ ����� ��������
// ������ ����� ����� ������ ������� ����� ��������
inline void EVAL::PST(const T_Bitboard * p_bitboard,// ������������� �����
                      unsigned __int8 from_white_king,// ������������ ������ ������
                      unsigned __int8 from_black_king,// ������������ ������� ������
                      unsigned __int8 & at_white_king, // ���������� ���������� ����� ������ ������ ������
                      unsigned __int8 & at_black_king, // ���������� ���������� ����� ������ ������� ������
                      __int32 & eval //����������� ������
                      ){

unsigned __int8 from; // ���������� ���� ������ ������ �����
unsigned __int64 from_mask = 0; // ����� ������������ ����� ������ ����

// ��� ���������� ���������� ����� ��������
//int dx;// ��������� �� �����������
//int dy;// ��������� �� ���������
//int distanse;// ������ ����������
//const int kd = 7;// ����������� ���������

/*
������ � ������ ����� �� ������ ������������ ������ ����� ���������
��������(���� ������� ��������� :)) �� �������� � ������� ����

���� ��������� � ������
*/

// �����-----------------------------------------------------------------
//from_mask = p_bitboard->white_piece[5];
//while(from_mask != 0){
//    from = first_one(from_mask);
//    from_mask = from_mask & (from_mask - 1);
//
////dx = (from_black_king%8 - from%8)>=0 ? (from_black_king%8 - from%8) :  -(from_black_king%8 - from%8);
////dy = (from_black_king/8 - from/8)>=0 ? (from_black_king/8 - from/8) :  -(from_black_king/8 - from/8);
////distanse = kd/(dx + dy);
////distanse = 0;
//
//	eval = eval + CONTROL_SQUARE
//	* Queen_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
//	              from, from_black_king, at_black_king);
//
//  }//while(from_mask != 0)

//�����----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[4];
while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask - 1);

    eval = eval + CONTROL_SQUARE 
	* Rook_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	             from, from_black_king, at_black_king);

//    cout << "Rook_amoun_white(p_bitboard)= "<< Rook_amoun(p_bitboard, from) << "\n";
}//while(from_mask != 0)

//����---------------------------------------------------------------------------------- 
from_mask = p_bitboard->white_piece[3];
while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval + Pozicion_b[from] 
	//+ CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//	                            from, from_black_king, at_black_king)
	;

//cout << "Bishop_amoun(p_bitboard, from)= "<< Bishop_amoun(p_bitboard, from) << "\n";
}//while(from_mask != 0)

//���� ----------------------------------------------------------------------------------
from_mask = p_bitboard->white_piece[2];
while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask - 1);

    eval = eval + Pozicion_n[from] 
	//+ CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_white_piece, 
	//	                           from, from_black_king, at_black_king)
    ;

 
}// while(from_mask != 0)


/////////////////////////////////////


//�����-----------------------------------------------------------------
//from_mask = p_bitboard->black_piece[5];
//while(from_mask != 0){
//    from = first_one(from_mask);
//    from_mask = from_mask & (from_mask-1);
//
////dx = (from_white_king%8 - from%8)>=0 ? (from_white_king%8 - from%8) :  -(from_white_king%8 - from%8);
////dy = (from_white_king/8 - from/8)>=0 ? (from_white_king/8 - from/8) :  -(from_white_king/8 - from/8);
////distanse = kd/(dx + dy);
////distanse = 0;
//
//    eval = eval - CONTROL_SQUARE 
//	* Queen_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
//	              from_white_king, at_white_king);
//
//}//while(from_mask != 0)

//�����----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[4];
while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - CONTROL_SQUARE 
	* Rook_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, from, 
	             from_white_king, at_white_king);

}//while(from_mask != 0){

//����---------------------------------------------------------------------------------- 
from_mask = p_bitboard->black_piece[3];
while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - Pozicion_b[from] 
	//- CONTROL_SQUARE * Bishop_amoun(p_bitboard->all_piece,p_bitboard->all_black_piece, 
	//	                            from, from_white_king, at_white_king)
	;


}//while(from_mask != 0){

//���� ----------------------------------------------------------------------------------
from_mask = p_bitboard->black_piece[2];
while(from_mask != 0){
    from = first_one(from_mask);
    from_mask = from_mask & (from_mask-1);

    eval = eval - Pozicion_n[from] 
	//- CONTROL_SQUARE * Knigt_amoun(p_bitboard->all_piece, p_bitboard->all_black_piece, 
	//	                           from, from_white_king, at_white_king)
	;


 }// while(from_mask != 0){
// ���� ----------------------------------------------------------------------------------

// ����� �� ����� ������
//eval = eval + ATAK_KING * (at_black_king - at_white_king);


}// inline int EVAL::PST(const T_Bitboard * p_bitboard// ������������� �����
//	*******************************************************************


//	*******************************************************************


//	===================================================================
// ����� �� ���������� ���� ����� �������. ���� ��� ��������� �� ����������� ����� 
inline __int32 EVAL::King_shield(unsigned __int8 n,// ��������� ������ (from)
					         unsigned __int8 z,// ���� ������
					         const T_Bitboard * p_bitboard,// ������������� �����
					         const unsigned __int8 SHIELD// ����� �� ��������� ������
					         ){

unsigned __int8 st = 0;

if(z == 1){
// ������� �� ������ ����� �� ������
   if(p_bitboard->white_name_from_coord[n + 7] == 0) st = st + SHIELD;
// ������� �� ������ ����� �����
   if(p_bitboard->white_name_from_coord[n + 8] == 0) st = st + SHIELD;
// ������� �� ������ ������ �����
   if(p_bitboard->white_name_from_coord[n + 9] == 0) st = st + SHIELD;
}else{
// ������� �� ������ ���� �� ������
   if(p_bitboard->black_name_from_coord[n - 7] == 0) st = st + SHIELD;
// ������� �� ������  ����� ����
   if(p_bitboard->black_name_from_coord[n - 8] == 0) st = st + SHIELD;
// ������� �� ������ ������ ����
   if(p_bitboard->black_name_from_coord[n - 9] == 0) st = st + SHIELD;
}//if(z==1){
return st;
}//inline int EVAL::King_shield
//	*******************************************************************

//	===================================================================
// ������� �������� ������� � ��������� �� ���������� ������� �������� � ������
__int32 EVAL::Eval_forced(const T_Bitboard * p_bitboard// ������������� �����
				      ){

unsigned __int64 from_mask; // ����� ������������ ����� ������ ����
__int32 ocenka_f = 0;

// ����� ���������

// ������----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[6];
  if(from_mask != 0) ocenka_f = ocenka_f + KING;

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[5];
  while(from_mask != 0){
// ���� �������� ��� ����������� ���� b = b & (b-1)  ���������� ����� ������ ��������� ��� 
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + QUEEN;
  }//while(from_mask != 0){

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->white_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + ROOK;
  }//while(from_mask != 0){

// ����------------------------------------------------------------------- 
  from_mask = p_bitboard->white_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + BISHOP;
 }//while(from_mask != 0){

// ���� -------------------------------------------------------------------
  from_mask = p_bitboard->white_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + KNIGHT;
 }// while(from_mask != 0){

// �����--------------------------------------------------------------------
  from_mask = p_bitboard->white_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f + PAWN;
}//while(from_mask != 0){

///////////////////////////////////

// ������ ����������(����������)

// ������----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[6];
  if(from_mask != 0) ocenka_f = ocenka_f - KING;

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[5];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - QUEEN;
  }//while(from_mask != 0){

// �����-----------------------------------------------------------------
  from_mask = p_bitboard->black_piece[4];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - ROOK;
  }//while(from_mask != 0){

// ����------------------------------------------------------------------- 
  from_mask = p_bitboard->black_piece[3];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - BISHOP;
 }//while(from_mask != 0){

// ���� -------------------------------------------------------------------
  from_mask = p_bitboard->black_piece[2];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - KNIGHT;
 }// while(from_mask != 0){

// �����--------------------------------------------------------------------
  from_mask = p_bitboard->black_piece[1];
  while(from_mask != 0){
    from_mask = from_mask & (from_mask - 1);
    ocenka_f = ocenka_f - PAWN;
}//while(from_mask != 0){

//--------------------------------------------------------------------------
// ��� ������ ������� ������� ������. �� ����� �� ��� �� ���������
//int eval = 0;
//
//for(int i = 0; i < 64; i++){
//	 eval = eval + kod[p_bitboard->white_name_from_coord[i]];
//   eval = eval - kod[p_bitboard->black_name_from_coord[i]];
//}//for(int i = 0; i < 64; i++) {
//
//if (eval != ocenka_f) cout << "KARAUUUUUUUUUUUUUUL"<< "\n";
//--------------------------------------------------------------------------

return ocenka_f;
}//int EVAL::Eval_forced
//	*******************************************************************


//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVAL::Rook_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					        ){

unsigned __int64 blocking_square_mask = 0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����
// ���� �����----------------------------------------------------------------------------------

// ������� � ������� ��������� ������������

// ��� �����----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� �����----------------------------------------------------
// blocking_square_mask = all_piece & e_rook_left_masks[from];
// if(blocking_square_mask !=0){ 
//   blocking_coordinate =  last_one(blocking_square_mask);
//   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
// }else{
//   move_mask = e_rook_left_masks[from];
// }//if(blocking_square_mask !=0){
//move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����-----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------


// ��� ������---------------------------------------------------
// blocking_square_mask = all_piece & e_rook_right_masks[from];
// if(blocking_square_mask !=0){ 
//   blocking_coordinate =  first_one(blocking_square_mask);
//   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
// }else{
//   move_mask = e_rook_right_masks[from];
// }//if(blocking_square_mask !=0){ 
//move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// ������� ������ ����� �����
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// ��������� ������ �����
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

  return number_n;
}//inline int EVAL::Rook_amoun
//	*******************************************************************

//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVAL::Bishop_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
						      ){

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

// ���� �����---------------------------------------------------------------------------------- 

// ������� � ������� ��������� ������������

// ��� ����� - ������
 blocking_square_mask = all_piece & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����� - �����
   blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - �����
 blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - ������
 blocking_square_mask = all_piece & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
  }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// ������� ������ ����� �����
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// ��������� ������ �����
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

  return number_n;
}//inline int EVAL::Bishop_amoun
//	*******************************************************************


//	===================================================================
// ������������ ��� ��������� ���� �����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVAL::Queen_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					         ){

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

// ���� �����----------------------------------------------------------------------------------

// ������� � ������� ��������� ������������

// ��� �����----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_up_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_up_masks[from] ^ e_rook_up_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_up_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� �����----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_left_masks[from] ^ e_rook_left_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_left_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����-----------------------------------------------------
 blocking_square_mask = all_piece & e_rook_down_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_rook_down_masks[from] ^ e_rook_down_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_down_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------


// ��� ������---------------------------------------------------
 blocking_square_mask = all_piece & e_rook_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_rook_right_masks[from] ^ e_rook_right_masks[blocking_coordinate];
 }else{
   move_mask = e_rook_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ���� �����---------------------------------------------------------------------------------- 

// ������� � ������� ��������� ������������

// ��� ����� - ������
 blocking_square_mask = all_piece & e_bishop_up_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_right_masks[from] ^ e_bishop_up_right_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_right_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ����� - �����
   blocking_square_mask = all_piece & e_bishop_up_left_masks[from];

 if(blocking_square_mask !=0){ 
   blocking_coordinate =  first_one(blocking_square_mask);
   move_mask = e_bishop_up_left_masks[from] ^ e_bishop_up_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_up_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - �����
 blocking_square_mask = all_piece & e_bishop_down_left_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_left_masks[from] ^ e_bishop_down_left_masks[blocking_coordinate];
 }else{
   move_mask = e_bishop_down_left_masks[from];
 }//if(blocking_square_mask !=0){ 
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

// ��� ���� - ������
 blocking_square_mask = all_piece & e_bishop_down_right_masks[from];
 if(blocking_square_mask !=0){ 
   blocking_coordinate =  last_one(blocking_square_mask);
   move_mask = e_bishop_down_right_masks[from] ^ e_bishop_down_right_masks[blocking_coordinate];
  }else{
   move_mask = e_bishop_down_right_masks[from];
 }//if(blocking_square_mask !=0){
move_mask_sum = move_mask_sum | move_mask;

//--------------------------------------------------------------

if ((e_king_moves_masks[from_king] & move_mask_sum) != 0 ) at_cnt = at_cnt + 1 ;

// ������� ������ ����� �����
move_mask_sum = move_mask_sum ^ (move_mask_sum & piece);

// ��������� ������ �����
   while (move_mask_sum !=0){
     move_mask_sum = move_mask_sum & (move_mask_sum - 1);
     number_n = number_n + 1;
   }//while (move !=0){

//if (number_n != number) cout << "������!!!!!!!!!! " << "\n";

  return number_n;
}//inline int EVAL::Queen_amoun
//	*******************************************************************


//	===================================================================
// ������������ ��� ��������� ���� ����. ���������� ���������� �����
// � ���������� ����������� ����� ������ ������
inline unsigned __int8 EVAL::Knigt_amoun(unsigned __int64  all_piece,// ��� ������
                       unsigned __int64  piece,// ���� ������
					   unsigned __int8 from,// ������������ �����
					   unsigned __int8 from_king,// ������������ ������
					   unsigned __int8 & at_cnt// ���������� ����������� ����� ������ ������(������� ����� ������)
					         ){

unsigned __int64 blocking_square_mask=0; // ����� ������������� �����
unsigned __int64 move_mask = 0; // ����� �����
unsigned __int64 move_mask_sum = 0; // ����� ����� ���� �����������

__int32 blocking_coordinate = 0; // ���������� �������������� ���� 
unsigned __int8 number = 0; // ���������� ���� ������ ������ �����
unsigned __int8 number_n = 0; // ���������� ���� ������ ������ �����

// ������� ���� �� ���� ������
// � ����� ������� �� �� ���� �����
move_mask = e_knigt_moves_masks[from] ^ (e_knigt_moves_masks[from] & piece) ;

if ((e_king_moves_masks[from_king] & move_mask) != 0 ) at_cnt = at_cnt + 1 ;

// ��������� ������ �����---------------------------------------
   while (move_mask !=0){
     move_mask = move_mask & (move_mask-1);
     number = number + 1;
   }//while (move !=0){
//--------------------------------------------------------------

  return number;
}//inline int EVAL::Knigt_amoun
//	*******************************************************************












//	===================================================================
// ���� ������ ������������� ���(�.�. ����� ������)
__inline __int32 EVAL::first_one(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
							 ){ 

   _asm { 
/*
�� �������� �� ����������(This document saved from http://mind.telenet.ru/samouch/7/31/index.html):
������� mov �������� ������ ������� (��������) ������ (����������). 
��� ���� �������� �������� ������� �������� ��������. 
������ ������� �� ����������. � ����������� �� �������� ���������, 
������������ ����� ��� ����. ���� �������� ������� ��-������� ��� 
����� ��������� �� ��������� ���������� ���������� ������ ��������, 
��� ��������� ������� ������������ ������ � ������� ������� �������� ���� 
�� ���������� ���������� byte ptr ��� word ptr. 
������� �� ������������ �� ����� ����������. 
����� ��������� ������������� ��������� �������� �������� - 
� ���� ������ ����������, ��� � �����. ��� ����� ���� ������������ �������� word ptr:
*/
           mov  eax, dword ptr analyzed_number[0]
/*
������� test ��������� �������� ����������� ��������� 
� ��� ����� ���������� �, � ����������� �� ����������, 
������������� ����� SF, ZF � PF. ����� OF � CF ������������, 
a AF ����� �������������� ��������. ��������� ������ ����� ����� 
���������������� ��������� �������� ���������. ������� test �� �������� 
�� ���� �� ���������. � �������� ������� �������� ������� test ����� 
��������� ������� (����� �����������) ��� ������ ������, � �������� 
������� - ������� (����� �����������), ������ ������ ��� ���������������� 
��������, ������ �� ����������� ���������� ��� �������� ������������ ��� 
������ ������. �������� ����� ���� ������� ��� ������� � ������������ ����� 
�� ������ ��� ��� �����. 

������� ���������� ���������:

������ �������-��� 0101
������ �������-��� 0011
��� ���������� 0001

���� SF ��������������� � 1, ���� � ���������� ���������� ������� 
������������ ����� � ������������� �������� �����.
���� ZF ��������������� � 1, ���� � ���������� ���������� ������� 
������������ �����, ��������� �� ����� �������� �����.
���� PF ��������������� � 1, ���� � ���������� ���������� ������� 
������������ ����� � ������ ����������� �������� ������ � ��� �����.
*/
           test eax, eax
/*
������� �������, ���� ������� ��������
jz ���� ZF= 1
*/
           jz   grown_up
/*
������� bsf ��������� ����� ��� ������� ����� � ������� ����, ������� 1. 
������������ ����������� �� �������� ���� (0) � ��������. ���� � ����� 
�� ������� ������������� �����, �� ��������������� ���� ZF. 
���� ������������� ���� ����, �� ����� ������� �������������� ���� 
��������� � ��������� � ������� �������. ������� ���� ��������� ��� 
������� � �����, ������������� �� ���� 0. � �������� ������� �������� ������� bsf 
������� ��������� �������, ���� ����� ������� ��������� ������������, � 
�������� ������� - ������� ��� ������ ������ �� ����������� ������. 
� ������� bsf ��������� ������������� ��� 16-�������, ��� � 32-������� ��������� 
(�� � ������, � ������ �������� ������ ���� ������ ����).
*/
           bsf  eax, eax
/*
������� jmp �������� ���������� � ��������� ����� ���� �� ��� ������� ������������ 
��������. ����� �������� �� �����������. ������� �� ������������ �� ����� ����������.
*/
           jmp jump_end
           grown_up:    bsf  eax, dword ptr analyzed_number[4]
/*
��������
*/
           add  eax, 20h
           jump_end:
       }//_asm {
}//__inline int EVAL::first_one
//	*******************************************************************

//	===================================================================
// ���� ��������� ������������� ���(�.�. ����� �����)
__inline __int32 EVAL::last_one(unsigned __int64 analyzed_number// ����� � ������� ���� ���������� ���
							){ 

  _asm { 
           mov  eax, dword ptr analyzed_number[4]
           test eax, eax
           jz   younger
/*
������� bsr ��������� ���� ��� ������� ����� � ������� ����, ������� 1. 
������������ ����������� �� �������� ���� (15 ��� 31) � ��������. 
���� � ����� �� ������� ������������� �����, �� ��������������� ���� ZF. 
���� ������������� ���� ����, �� ����� ������� �������������� ���� ��������� 
� ��������� � ������� �������. ������� ���� ��������� ��� ������� � �����, 
������������� �� ���� 0. � �������� ������� �������� ������� bsr ������� 
��������� �������, ���� ����� ������� ��������� ������������, � �������� 
������� - ������� ��� ������ ������ �� ����������� ������. 
� ������� bsr ��������� ������������� ��� 16-�������, ��� � 32-������� ���������, 
�� � ������, � ������ �������� ������ ���� ������ ���� (�� ����������� ������, 
����� ������ ������� - ���������).
*/
           bsr  eax, eax
           add  eax, 20h
           jmp  jump_end
           younger:    bsr  eax, dword ptr analyzed_number[0]
           jump_end:
       }//_asm { 
}//__inline int EVAL::last_one
//	*******************************************************************

// � ������� ���� ����� ����� � ������� ������ �� �������� �������
const unsigned __int64 EVAL::e_move_masks[] = {
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
};//unsigned __int64 move_masks[]
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

// ����� ����� ���� �����
const unsigned __int64 EVAL::e_rook_up_masks[] = {
//======================
72340172838076672, // from0
//======================
144680345676153344, // from1
//======================
289360691352306688, // from2
//======================
578721382704613376, // from3
//======================
1157442765409226752, // from4
//======================
2314885530818453504, // from5
//======================
4629771061636907008, // from6
//======================
9259542123273814016, // from7
//======================
72340172838076416, // from8
//======================
144680345676152832, // from9
//======================
289360691352305664, // from10
//======================
578721382704611328, // from11
//======================
1157442765409222656, // from12
//======================
2314885530818445312, // from13
//======================
4629771061636890624, // from14
//======================
9259542123273781248, // from15
//======================
72340172838010880, // from16
//======================
144680345676021760, // from17
//======================
289360691352043520, // from18
//======================
578721382704087040, // from19
//======================
1157442765408174080, // from20
//======================
2314885530816348160, // from21
//======================
4629771061632696320, // from22
//======================
9259542123265392640, // from23
//======================
72340172821233664, // from24
//======================
144680345642467328, // from25
//======================
289360691284934656, // from26
//======================
578721382569869312, // from27
//======================
1157442765139738624, // from28
//======================
2314885530279477248, // from29
//======================
4629771060558954496, // from30
//======================
9259542121117908992, // from31
//======================
72340168526266368, // from32
//======================
144680337052532736, // from33
//======================
289360674105065472, // from34
//======================
578721348210130944, // from35
//======================
1157442696420261888, // from36
//======================
2314885392840523776, // from37
//======================
4629770785681047552, // from38
//======================
9259541571362095104, // from39
//======================
72339069014638592, // from40
//======================
144678138029277184, // from41
//======================
289356276058554368, // from42
//======================
578712552117108736, // from43
//======================
1157425104234217472, // from44
//======================
2314850208468434944, // from45
//======================
4629700416936869888, // from46
//======================
9259400833873739776, // from47
//======================
72057594037927936, // from48
//======================
144115188075855872, // from49
//======================
288230376151711744, // from50
//======================
576460752303423488, // from51
//======================
1152921504606846976, // from52
//======================
2305843009213693952, // from53
//======================
4611686018427387904, // from54
//======================
9223372036854775808, // from55
//======================
0, // from56
//======================
0, // from57
//======================
0, // from58
//======================
0, // from59
//======================
0, // from60
//======================
0, // from61
//======================
0, // from62
//======================
0, // from63
};//unsigned __int64 rook_up_masks[]
////-----------------------------------------------------------------------------------
//
// ����� ����� ���� ����
const unsigned __int64 EVAL::e_rook_down_masks[] = {
//======================
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
//======================
1, // from8
//======================
2, // from9
//======================
4, // from10
//======================
8, // from11
//======================
16, // from12
//======================
32, // from13
//======================
64, // from14
//======================
128, // from15
//======================
257, // from16
//======================
514, // from17
//======================
1028, // from18
//======================
2056, // from19
//======================
4112, // from20
//======================
8224, // from21
//======================
16448, // from22
//======================
32896, // from23
//======================
65793, // from24
//======================
131586, // from25
//======================
263172, // from26
//======================
526344, // from27
//======================
1052688, // from28
//======================
2105376, // from29
//======================
4210752, // from30
//======================
8421504, // from31
//======================
16843009, // from32
//======================
33686018, // from33
//======================
67372036, // from34
//======================
134744072, // from35
//======================
269488144, // from36
//======================
538976288, // from37
//======================
1077952576, // from38
//======================
2155905152, // from39
//======================
4311810305, // from40
//======================
8623620610, // from41
//======================
17247241220, // from42
//======================
34494482440, // from43
//======================
68988964880, // from44
//======================
137977929760, // from45
//======================
275955859520, // from46
//======================
551911719040, // from47
//======================
1103823438081, // from48
//======================
2207646876162, // from49
//======================
4415293752324, // from50
//======================
8830587504648, // from51
//======================
17661175009296, // from52
//======================
35322350018592, // from53
//======================
70644700037184, // from54
//======================
141289400074368, // from55
//======================
282578800148737, // from56
//======================
565157600297474, // from57
//======================
1130315200594948, // from58
//======================
2260630401189896, // from59
//======================
4521260802379792, // from60
//======================
9042521604759584, // from61
//======================
18085043209519168, // from62
//======================
36170086419038336, // from63


};//unsigned __int64 rook_down_masks[]
////-----------------------------------------------------------------------------------
//
// ����� ����� ���� �����
const unsigned __int64 EVAL::e_rook_left_masks[] = {
//======================
0, // from0
//======================
1, // from1
//======================
3, // from2
//======================
7, // from3
//======================
15, // from4
//======================
31, // from5
//======================
63, // from6
//======================
127, // from7
//======================
0, // from8
//======================
256, // from9
//======================
768, // from10
//======================
1792, // from11
//======================
3840, // from12
//======================
7936, // from13
//======================
16128, // from14
//======================
32512, // from15
//======================
0, // from16
//======================
65536, // from17
//======================
196608, // from18
//======================
458752, // from19
//======================
983040, // from20
//======================
2031616, // from21
//======================
4128768, // from22
//======================
8323072, // from23
//======================
0, // from24
//======================
16777216, // from25
//======================
50331648, // from26
//======================
117440512, // from27
//======================
251658240, // from28
//======================
520093696, // from29
//======================
1056964608, // from30
//======================
2130706432, // from31
//======================
0, // from32
//======================
4294967296, // from33
//======================
12884901888, // from34
//======================
30064771072, // from35
//======================
64424509440, // from36
//======================
133143986176, // from37
//======================
270582939648, // from38
//======================
545460846592, // from39
//======================
0, // from40
//======================
1099511627776, // from41
//======================
3298534883328, // from42
//======================
7696581394432, // from43
//======================
16492674416640, // from44
//======================
34084860461056, // from45
//======================
69269232549888, // from46
//======================
139637976727552, // from47
//======================
0, // from48
//======================
281474976710656, // from49
//======================
844424930131968, // from50
//======================
1970324836974592, // from51
//======================
4222124650659840, // from52
//======================
8725724278030336, // from53
//======================
17732923532771328, // from54
//======================
35747322042253312, // from55
//======================
0, // from56
//======================
72057594037927936, // from57
//======================
216172782113783808, // from58
//======================
504403158265495552, // from59
//======================
1080863910568919040, // from60
//======================
2233785415175766016, // from61
//======================
4539628424389459968, // from62
//======================
9151314442816847872, // from63

};//unsigned __int64 rook_left_masks[]
//-----------------------------------------------------------------------------------

// ����� ����� ���� ������
const unsigned __int64 EVAL::e_rook_right_masks[] = {
//======================
254, // from0
//======================
252, // from1
//======================
248, // from2
//======================
240, // from3
//======================
224, // from4
//======================
192, // from5
//======================
128, // from6
//======================
0, // from7
//======================
65024, // from8
//======================
64512, // from9
//======================
63488, // from10
//======================
61440, // from11
//======================
57344, // from12
//======================
49152, // from13
//======================
32768, // from14
//======================
0, // from15
//======================
16646144, // from16
//======================
16515072, // from17
//======================
16252928, // from18
//======================
15728640, // from19
//======================
14680064, // from20
//======================
12582912, // from21
//======================
8388608, // from22
//======================
0, // from23
//======================
4261412864, // from24
//======================
4227858432, // from25
//======================
4160749568, // from26
//======================
4026531840, // from27
//======================
3758096384, // from28
//======================
3221225472, // from29
//======================
2147483648, // from30
//======================
0, // from31
//======================
1090921693184, // from32
//======================
1082331758592, // from33
//======================
1065151889408, // from34
//======================
1030792151040, // from35
//======================
962072674304, // from36
//======================
824633720832, // from37
//======================
549755813888, // from38
//======================
0, // from39
//======================
279275953455104, // from40
//======================
277076930199552, // from41
//======================
272678883688448, // from42
//======================
263882790666240, // from43
//======================
246290604621824, // from44
//======================
211106232532992, // from45
//======================
140737488355328, // from46
//======================
0, // from47
//======================
71494644084506624, // from48
//======================
70931694131085312, // from49
//======================
69805794224242688, // from50
//======================
67553994410557440, // from51
//======================
63050394783186944, // from52
//======================
54043195528445952, // from53
//======================
36028797018963968, // from54
//======================
0, // from55
//======================
//9079256848778919936, // from56
18302628885633695744, // from56
//======================
//8935141660703064064, // from57
18158513697557839872, // from57
//======================
//8646911284551352320, // from58
17870283321406128128, // from58
//======================
//8070450532247928832, // from59
17293822569102704640, // from59
//======================
//6917529027641081856, // from60
16140901064495857664, // from60
//======================
//4611686018427387904, // from61
13835058055282163712, // from61
//======================
//0, // from62
9223372036854775808, // from62
//======================
0 // from63

};//unsigned __int64 rook_right_masks[]
//-----------------------------------------------------------------------------------




// ����� ���� ���� ����� - ������
const unsigned __int64  EVAL::e_bishop_up_right_masks[] = {
//======================
//18049651735527936, // from0
9241421688590303744, // from0
//======================
36099303471055872, // from1
//======================
141012904183808, // from2
//======================
550831656960, // from3
//======================
2151686144, // from4
//======================
8404992, // from5
//======================
32768, // from6
//======================
0, // from7
//======================
4620710844295151616, // from8
//======================
//18049651735527424, // from9
9241421688590303232, // from9
//======================
36099303471054848, // from10
//======================
141012904181760, // from11
//======================
550831652864, // from12
//======================
2151677952, // from13
//======================
8388608, // from14
//======================
0, // from15
//======================
2310355422147510272, // from16
//======================
4620710844295020544, // from17
//======================
//18049651735265280, // from18
9241421688590041088, // from18
//======================
36099303470530560, // from19
//======================
141012903133184, // from20
//======================
550829555712, // from21
//======================
2147483648, // from22
//======================
0, // from23
//======================
1155177711056977920, // from24
//======================
2310355422113955840, // from25
//======================
4620710844227911680, // from26
//======================
//18049651601047552, // from27
9241421688455823360, // from27
//======================
36099303202095104, // from28
//======================
141012366262272, // from29
//======================
549755813888, // from30
//======================
0, // from31
//======================
577588851233521664, // from32
//======================
1155177702467043328, // from33
//======================
2310355404934086656, // from34
//======================
4620710809868173312, // from35
//======================
//18049582881570816, // from36
9241421619736346624, // from36
//======================
36099165763141632, // from37
//======================
140737488355328, // from38
//======================
0, // from39
//======================
288793326105133056, // from40
//======================
577586652210266112, // from41
//======================
1155173304420532224, // from42
//======================
2310346608841064448, // from43
//======================
4620693217682128896, // from44
//======================
//18014398509481984, // from45
9241386435364257792, // from45
//======================
36028797018963968, // from46
//======================
0, // from47
//======================
144115188075855872, // from48
//======================
288230376151711744, // from49
//======================
576460752303423488, // from50
//======================
1152921504606846976, // from51
//======================
2305843009213693952, // from52
//======================
4611686018427387904, // from53
//======================
9223372036854775808, // from54
//======================
0, // from55
//======================
0, // from56
//======================
0, // from57
//======================
0, // from58
//======================
0, // from59
//======================
0, // from60
//======================
0, // from61
//======================
0, // from62
//======================
0 // from63

};//unsigned __int64 bishop_up_right_masks[]


// ����� ���� ���� ����� - �����
const unsigned __int64  EVAL::e_bishop_up_left_masks[] = {
//======================
0, // from0
//======================
256, // from1
//======================
66048, // from2
//======================
16909312, // from3
//======================
4328785920, // from4
//======================
1108169199616, // from5
//======================
283691315109888, // from6
//======================
72624976668147712, // from7
//======================
0, // from8
//======================
65536, // from9
//======================
16908288, // from10
//======================
4328783872, // from11
//======================
1108169195520, // from12
//======================
283691315101696, // from13
//======================
72624976668131328, // from14
//======================
145249953336262656, // from15
//======================
0, // from16
//======================
16777216, // from17
//======================
4328521728, // from18
//======================
1108168671232, // from19
//======================
283691314053120, // from20
//======================
72624976666034176, // from21
//======================
145249953332068352, // from22
//======================
290499906664136704, // from23
//======================
0, // from24
//======================
4294967296, // from25
//======================
1108101562368, // from26
//======================
283691179835392, // from27
//======================
72624976397598720, // from28
//======================
145249952795197440, // from29
//======================
290499905590394880, // from30
//======================
580999811180789760, // from31
//======================
0, // from32
//======================
1099511627776, // from33
//======================
283673999966208, // from34
//======================
72624942037860352, // from35
//======================
145249884075720704, // from36
//======================
290499768151441408, // from37
//======================
580999536302882816, // from38
//======================
1161999072605765632, // from39
//======================
0, // from40
//======================
281474976710656, // from41
//======================
72620543991349248, // from42
//======================
145241087982698496, // from43
//======================
290482175965396992, // from44
//======================
580964351930793984, // from45
//======================
1161928703861587968, // from46
//======================
2323857407723175936, // from47
//======================
0, // from48
//======================
72057594037927936, // from49
//======================
144115188075855872, // from50
//======================
288230376151711744, // from51
//======================
576460752303423488, // from52
//======================
1152921504606846976, // from53
//======================
2305843009213693952, // from54
//======================
4611686018427387904, // from55
//======================
0, // from56
//======================
0, // from57
//======================
0, // from58
//======================
0, // from59
//======================
0, // from60
//======================
0, // from61
//======================
0, // from62
//======================
0, // from63

};//unsigned __int64 bishop_up_left_masks[]

// ����� ���� ���� ���� - ������
const unsigned __int64  EVAL::e_bishop_down_right_masks[] = {
//======================
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
//======================
2, // from8
//======================
4, // from9
//======================
8, // from10
//======================
16, // from11
//======================
32, // from12
//======================
64, // from13
//======================
128, // from14
//======================
0, // from15
//======================
516, // from16
//======================
1032, // from17
//======================
2064, // from18
//======================
4128, // from19
//======================
8256, // from20
//======================
16512, // from21
//======================
32768, // from22
//======================
0, // from23
//======================
132104, // from24
//======================
264208, // from25
//======================
528416, // from26
//======================
1056832, // from27
//======================
2113664, // from28
//======================
4227072, // from29
//======================
8388608, // from30
//======================
0, // from31
//======================
33818640, // from32
//======================
67637280, // from33
//======================
135274560, // from34
//======================
270549120, // from35
//======================
541097984, // from36
//======================
1082130432, // from37
//======================
2147483648, // from38
//======================
0, // from39
//======================
8657571872, // from40
//======================
17315143744, // from41
//======================
34630287488, // from42
//======================
69260574720, // from43
//======================
138521083904, // from44
//======================
277025390592, // from45
//======================
549755813888, // from46
//======================
0, // from47
//======================
2216338399296, // from48
//======================
4432676798592, // from49
//======================
8865353596928, // from50
//======================
17730707128320, // from51
//======================
35461397479424, // from52
//======================
70918499991552, // from53
//======================
140737488355328, // from54
//======================
0, // from55
//======================
567382630219904, // from56
//======================
1134765260439552, // from57
//======================
2269530520813568, // from58
//======================
4539061024849920, // from59
//======================
9078117754732544, // from60
//======================
18155135997837312, // from61
//======================
36028797018963968, // from62
//======================
0, // from63


};//unsigned __int64 bishop_down_right_masks[]

// ����� ���� ���� ���� - �����
const unsigned __int64  EVAL::e_bishop_down_left_masks[] = {
//======================
0, // from0
//======================
0, // from1
//======================
0, // from2
//======================
0, // from3
//======================
0, // from4
//======================
0, // from5
//======================
0, // from6
//======================
0, // from7
//======================
0, // from8
//======================
1, // from9
//======================
2, // from10
//======================
4, // from11
//======================
8, // from12
//======================
16, // from13
//======================
32, // from14
//======================
64, // from15
//======================
0, // from16
//======================
256, // from17
//======================
513, // from18
//======================
1026, // from19
//======================
2052, // from20
//======================
4104, // from21
//======================
8208, // from22
//======================
16416, // from23
//======================
0, // from24
//======================
65536, // from25
//======================
131328, // from26
//======================
262657, // from27
//======================
525314, // from28
//======================
1050628, // from29
//======================
2101256, // from30
//======================
4202512, // from31
//======================
0, // from32
//======================
16777216, // from33
//======================
33619968, // from34
//======================
67240192, // from35
//======================
134480385, // from36
//======================
268960770, // from37
//======================
537921540, // from38
//======================
1075843080, // from39
//======================
0, // from40
//======================
4294967296, // from41
//======================
8606711808, // from42
//======================
17213489152, // from43
//======================
34426978560, // from44
//======================
68853957121, // from45
//======================
137707914242, // from46
//======================
275415828484, // from47
//======================
0, // from48
//======================
1099511627776, // from49
//======================
2203318222848, // from50
//======================
4406653222912, // from51
//======================
8813306511360, // from52
//======================
17626613022976, // from53
//======================
35253226045953, // from54
//======================
70506452091906, // from55
//======================
0, // from56
//======================
281474976710656, // from57
//======================
564049465049088, // from58
//======================
1128103225065472, // from59
//======================
2256206466908160, // from60
//======================
4512412933881856, // from61
//======================
9024825867763968, // from62
//======================
18049651735527937, // from63

};//unsigned __int64 bishop_down_left_masks[]



//-----------------------------------------------------------------------------------

// ����� ����� ������
const unsigned __int64  EVAL::e_king_moves_masks[] = {
// � ���������� ����� ��� ��������� ������ �������� from0 - ������ � A1(0)
//======================
770, // from0
//======================
1797, // from1
//======================
3594, // from2
//======================
7188, // from3
//======================
14376, // from4
//======================
28752, // from5
//======================
57504, // from6
//======================
49216, // from7
//======================
197123, // from8
//======================
460039, // from9
//======================
920078, // from10
//======================
1840156, // from11
//======================
3680312, // from12
//======================
7360624, // from13
//======================
14721248, // from14
//======================
12599488, // from15
//======================
50463488, // from16
//======================
117769984, // from17
//======================
235539968, // from18
//======================
471079936, // from19
//======================
942159872, // from20
//======================
1884319744, // from21
//======================
3768639488, // from22
//======================
3225468928, // from23
//======================
12918652928, // from24
//======================
30149115904, // from25
//======================
60298231808, // from26
//======================
120596463616, // from27
//======================
241192927232, // from28
//======================
482385854464, // from29
//======================
964771708928, // from30
//======================
825720045568, // from31
//======================
3307175149568, // from32
//======================
7718173671424, // from33
//======================
15436347342848, // from34
//======================
30872694685696, // from35
//======================
61745389371392, // from36
//======================
123490778742784, // from37
//======================
246981557485568, // from38
//======================
211384331665408, // from39
//======================
846636838289408, // from40
//======================
1975852459884544, // from41
//======================
3951704919769088, // from42
//======================
7903409839538176, // from43
//======================
15806819679076352, // from44
//======================
31613639358152704, // from45
//======================
63227278716305408, // from46
//======================
54114388906344448, // from47
//======================
216739030602088448, // from48
//======================
505818229730443264, // from49
//======================
1011636459460886528, // from50
//======================
2023272918921773056, // from51
//======================
4046545837843546112, // from52
//======================
8093091675687092224, // from53
//======================
//6962811314519408640, // from54
16186183351374184448, // from54
//======================
//4629911523169402880, // from55
13853283560024178688, // from55
//======================
144959613005987840, // from56
//======================
362258295026614272, // from57
//======================
724516590053228544, // from58
//======================
1449033180106457088, // from59
//======================
2898066360212914176, // from60
//======================
5796132720425828352, // from61
//======================
//2368893403996880896, // from62
11592265440851656704, // from62
//======================
4665729213955833856, // from63
};//unsigned __int64 king_moves_masks[]
//-----------------------------------------------------------------------------------



// ����� ����� ����
const unsigned __int64  EVAL::e_knigt_moves_masks[] = {
//======================
132096, // from0
//======================
329728, // from1
//======================
659712, // from2
//======================
1319424, // from3
//======================
2638848, // from4
//======================
5277696, // from5
//======================
10489856, // from6
//======================
4202496, // from7
//======================
33816580, // from8
//======================
84410376, // from9
//======================
168886289, // from10
//======================
337772578, // from11
//======================
675545156, // from12
//======================
1351090312, // from13
//======================
2685403152, // from14
//======================
1075839008, // from15
//======================
8657044482, // from16
//======================
21609056261, // from17
//======================
43234889994, // from18
//======================
86469779988, // from19
//======================
172939559976, // from20
//======================
345879119952, // from21
//======================
687463207072, // from22
//======================
275414786112, // from23
//======================
2216203387392, // from24
//======================
5531918402816, // from25
//======================
11068131838464, // from26
//======================
22136263676928, // from27
//======================
44272527353856, // from28
//======================
88545054707712, // from29
//======================
175990581010432, // from30
//======================
70506185244672, // from31
//======================
567348067172352, // from32
//======================
1416171111120896, // from33
//======================
2833441750646784, // from34
//======================
5666883501293568, // from35
//======================
11333767002587136, // from36
//======================
22667534005174272, // from37
//======================
45053588738670592, // from38
//======================
18049583422636032, // from39
//======================
145241105196122112, // from40
//======================
362539804446949376, // from41
//======================
725361088165576704, // from42
//======================
1450722176331153408, // from43
//======================
2901444352662306816, // from44
//======================
5802888705324613632, // from45
//======================
11533718717099671552, // from46
//======================
4620693356194824192, // from47
//======================
288234782788157440, // from48
//======================
576469569871282176, // from49
//======================
1224997833292120064, // from50
//======================
2449995666584240128, // from51
//======================
4899991333168480256, // from52
//======================
9799982666336960512, // from53
//======================
1152939783987658752, // from54
//======================
2305878468463689728, // from55
//======================
1128098930098176, // from56
//======================
2257297371824128, // from57
//======================
4796069720358912, // from58
//======================
9592139440717824, // from59
//======================
19184278881435648, // from60
//======================
38368557762871296, // from61
//======================
4679521487814656, // from62
//======================
9077567998918656 // from63
};//unsigned __int64 knigt_moves_masks[]
//-----------------------------------------------------------------------------------


