#include <iostream> // TEST


#include "m_Line.h"
#include "k_Util.h"

//---------------------------------------------------------------------------
/*
 * ����� �������� ������ (Brenkman Andrey)
 * ��������� ��������� 27.02.2009
 *
*/
//---------------------------------------------------------------------------

/*
 ������� ����� ������� 70%

 ������ � ������ �������� ��������

 � ����� ������ �����.
 � ��� ���� ��� ���������� ����� �������� - ���� ����������� ��� ���������� �� �������, 
 � ������ ��� ������� �  �������. ����� ���� � ������ ���� ���� ����� ������� �������� 
 ��� ����.
 �� ���������� ����, ������� ������, ���������� �� ������� � ����� �������� ��������. 
 �� ������� �� �������������� �� � ����� ������� ��������. ���������� �� � ������ ���� 
 ���������� ������ ��� ������� ���� �������. ����� �� ����, �� ������ � ���� ������� 
 �������������� � ����� ������� ��������.

 ���������:

 � ��� ���� ����� �������� ����� ��� ����� ������ pv_current. 
 �� �� ���������, ����� ���������� �� ������� public_update_PV_current.  

 ���� ��� ������ ����� �������� pv_best. 
 �� �� ��������� ����� ��� ��� ����� �� ���� public_update_PV_best.
 ������ ������ �� ���������� �� ������������ ������� public_update_PV_best_max_depth.

 ��� �� � ������ ���� ���� ������ ����� �������� ��� ������� ���� p_PV_best_point.
 �� �� ��������� ������ ��� ����� � ���� ����� ������ ��� public_update_PV_best_point.

*/

namespace 
{
	struct T_PV pv_current;// ������� ������� ����� ��� ����� ������(��������� ��� ���������� �� �������)
	struct T_PV pv_best;// ������ ������� ����� ��� ����� ������(��������� ��� ��������)

	__int32 save_value = -999999999; // ���������� ������ ���������� ����������� ��������
	unsigned __int8 save_Depth_Max = 0; // ���������� ������� ���������� ����������� ��������
}

//	===================================================================
// ���������� ������ �������
struct T_PV * PVSAVE::public_get_pv_best()
{
	return & pv_best;
}

//	===================================================================
// ���������� ������� �������
struct T_PV * PVSAVE::public_get_pv_current()
{
	return & pv_current;
}

//	===================================================================
// ��������� ����� ������� �������� � ����
void PVSAVE::public_update_PV_best_point
(
	struct T_PV * const p_PV_best_point// ������ ������� ��������� � ����
)
{
	// ������ ������� ������� ������� �������� ������ ������ ���������� � ��������� PV_best
	// �� �� ������ ����������� ������ �� � ������� ��������������� ��
	// ���������� ���� ��������� ���������� ��� ������� ���� PV_best_point
	// ����� �� �������������� ������ ������� ��������������� ������ ������
	for (unsigned __int8 n = 0; n < pv_best.depth_max; n++)
	{
		p_PV_best_point->move[n] = pv_best.move[n];
	}

	p_PV_best_point->score_move = pv_best.score_move;
	p_PV_best_point->depth_max = pv_best.depth_max;
}

//	===================================================================
// ��������� ����� ������� ��������
void PVSAVE::public_update_PV_best
(
	const struct T_PV * const p_PV_best_point// ������ ������� ��������� � ����
)
{
	// �� ��������� ���� ������ � ������ � ��������� PV_best_point ���������� ������ �����
	// ������ �� �� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
	for (unsigned __int8 n = 0; n < p_PV_best_point->depth_max; n++)
	{
		pv_best.move[n] = p_PV_best_point->move[n];
	}

	pv_best.score_move = p_PV_best_point->score_move; 
	pv_best.depth_max = p_PV_best_point->depth_max;
}

//	===================================================================
// ��������� ����� ������� �������� �� ������������ �������
void PVSAVE::public_update_PV_best_max_depth
(
	const __int32 & score,// ������ ������� � ����� ��������
	const unsigned __int8 & depth// ������� �������
)
{
	// �� �������� ���������� ������� � ���������� ������� ������������ �������
	// ���������� �� ������� �� �� ������ ���� ���� �������� ���� �������� � ��������� PV_current
	// ������ �� ��������� ��� � ��������� PV_best � �������� ������ �������
	// � ��� ��������� ������� ���������� � ������ ������� � ���� ������ ���� �������
	// �� � ��� ������� ����� ��������
	// �������� ��� ���� ���� �� 0 �� Depth - 1
	// ������ ��� ���� � ��� ��������� ������� 0
	for (unsigned __int8 n = 0; n < depth; n++)
	{
		pv_best.move[n] = pv_current.move[n];
	}

	pv_best.score_move = score;

	// ���������� ������� ��������
	pv_best.depth_max = depth;
}

//	===================================================================
// ���������� ��� � ������� �������
void PVSAVE::public_update_PV_current
(
	const unsigned __int8 & i,// ����� ���� � ������ �����
	const unsigned __int8 & depth,// ������� �������
	const struct T_list_surplus_moves * const p_list_surplus_moves// ������ �����
)
{

	// �� ������ ��������� ����� �������� ������� ��� � ������� ������� �� ������� �������.
	// �� ����� ��� ������� ������� ������������� �� ������������ ������ ������� Alpha_beta �.�. 
	// �� ����������� ���� ���������� � ������� 
	// ������ � ��������� PV_current ����� ������ � �� ������� ������� 
	// �� ��� ����������� ���� ������� ������ �� ��������� ��� ��� ��������� � ����������� ����.
	pv_current.move[depth] = p_list_surplus_moves->move[i];
}

//	===================================================================
// ������� ������� ��� � ��������
void PVSAVE::public_print_currmove
(
	const struct T_list_surplus_moves * const p_list_surplus_moves,//������ �����
	const unsigned __int8 nodes_0,// ���������� ����� �� ������� �������
	const unsigned __int8 i,// ����� ���� �� ������
	const unsigned __int8 Depth_Max// ������������ �������
)
{
	// �������������� � ������ ������ ���
	std::cout << "info currmove " ;
	std::cout <<UTIL::public_x_to_notation(((p_list_surplus_moves->move[i]>>12)& 63)%8);
	std::cout <<(1 + ((p_list_surplus_moves->move[i]>>12)& 63)/8);
	std::cout <<UTIL::public_x_to_notation(((p_list_surplus_moves->move[i]>>18)& 63)%8);
	std::cout <<(1 + ((p_list_surplus_moves->move[i]>>18)& 63)/8);
	// ���������� ����� ��������������� ����
	std::cout << " currmovenumber " ;
	std::cout << (__int32)nodes_0;
	// �������� ���������� � ������� ���������
	std::cout << " depth " << (__int32)Depth_Max;
	std::cout << "\n";
	std::cout.flush();
}


//	===================================================================
// ������� ������� ������� � ��������
void PVSAVE::public_print_pv_line
(
	const bool colour_move,// ���� ���������� �������
	const struct T_list_surplus_moves * const p_list_surplus_moves,// ������ �����
	const struct T_PV * const p_PV_best_point,// ������ �������
	const __int64 print_nodes,// ���������� �����
	const unsigned __int8 i,// ����� ���� � ������
	const unsigned __int8 Depth_Max,// ������������ �������
	const __int32 value,// ������ �������
    const bool all_line// ����� ���� �����
		//,const double time_nodes// ����� ������� ��������
)
{
	__int32 inv = 0;// ��������� ���� ������� �������
	__int32 d_m = 0;// ������������ ������ ����������� �����

	/*
	// Here are examples of the possible info output: 
	< info currmove e8e7 (current calculated move) +
	< info currmovenumber 29 +
	< info nps 291506 (nodes per sec, Fritz calculates the nps itself)
	< info tbhits 830
	< ino depth 9
	< info score cp12 (in centipawns)
	< info score mate 5 (or mate -5)
	< info nodes 288973
	< info time 11192 (milliseconds)
	< info pv e2e4 e7e5 g1f3 b8c6 f1d3 f8c5 e1g1 g8e7 b1c3 
	*/

	if (save_value == value)
	{
		std::cout <<"\n";
	}

	// ������� ������� ������ � ��� ������ ���� ������ ������ � ������ � ������
	// ������� ���������� ��� �� �� ���� ������ ������ � ���� �� ��� ��� 

	// ���� ������ ���������� ��� ���������� ������� �� ������� �������
	if (all_line ||
		((save_value != value) // ���������� ������
		||(save_Depth_Max != Depth_Max)) // ���������� �������
		)
	{
		save_value = value;
		save_Depth_Max = Depth_Max;


		std::cout << "info " ;
		//std::cout << " nps "<<(int)(nodes/time_nodes);
		std::cout << " depth "<< (__int32)Depth_Max;
		//�������� ���������� � ������� �������������� ��������
		std::cout << " seldepth "<< (__int32)p_PV_best_point->depth_max;

		//�������� ���������� � ������ �������(��� ������ �� ������ ������ ������ �������� � ������ ���������)
		colour_move == 0 ? inv = -1 : inv = 1;

		if (inv * value > 900000)
		{
			std::cout << " score mate " << (__int32)p_PV_best_point->depth_max;// ��������� �� ���� � ���� �����
		} 
		else if(inv * value < -900000)
		{
			std::cout << " score mate " << -(__int32)p_PV_best_point->depth_max;// ��������� �� ���� � ���� �����
		}
		else
		{
			std::cout << " score cp " << inv * value;// ������� ������ (in centipawns)�.�. ���� ����� ����� 100 �������
		}

		//�������� ���������� � ���������� ������������� �����
		std::cout << " nodes "<<(__int32)print_nodes;

		//��������� �� ������ ������ ��� �� �� ��������� �� ����� ������� �� ������
		if ( (value * value) != (p_PV_best_point->score_move * p_PV_best_point->score_move) ) 
		//if(0)
		{
			std::cout << "\n";
			std::cout << "ERROR LINE " ;
			std::cout << "\n";
			std::cout << " value = " << value;
			std::cout << "\n";
			std::cout << " p_PV_best_point.score_move = " << p_PV_best_point->score_move;
			std::cout << "\n";
			std::cout << "**********************"<<"\n";
			std::cout << "  " << "\n";
		}
		else
		{
			//����� ����� ��������
			std::cout << " pv ";

			for (unsigned __int8 n = 0; n < p_PV_best_point->depth_max; n++)
			{
				std::cout << UTIL::public_x_to_notation(((p_PV_best_point->move[n]>>12)& 63)%8);
				std::cout << (1 + ((p_PV_best_point->move[n]>>12)& 63)/8);
				std::cout << UTIL::public_x_to_notation(((p_PV_best_point->move[n]>>18)& 63)%8);
				std::cout << (1 + ((p_PV_best_point->move[n]>>18)& 63)/8);

				d_m = ((p_PV_best_point->move[n]>>6)& 63);

				//���������� ������ ����������� �����
				if (d_m != 5)
				{
					if (d_m > 15)
					{
						std::cout <<UTIL::public_int_to_char_board((d_m - 20),0);
					}
					else
					{
						std::cout <<UTIL::public_int_to_char_board((d_m - 10),0);
					}
				}
				std::cout <<" ";
			}
		}
		std::cout <<"\n";
	}
	std::cout.flush();// ��� ��� �� ����� �� ��� ����� ������� :)
}






