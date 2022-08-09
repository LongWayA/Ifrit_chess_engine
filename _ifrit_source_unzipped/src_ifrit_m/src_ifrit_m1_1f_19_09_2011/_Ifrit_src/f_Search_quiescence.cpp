
// ����� �������� ������ (Brenkman Andrey)
// ��������� ��������� 5.12.2008 19.11.2009 22.06.10 22.11.10

#include <fstream>
#include <iostream>

#include "a_List.h"
#include "d_Search_constants.h"
#include "f_Search_quiescence.h"
#include "g_Evaluate.h"
#include "h_Generator_moves.h"
#include "i_Make_move.h"
#include "j_Sorting_moves.h"
#include "m_PV_save.h"
#include "n_Delta_pruning.h"
#include "n_Delta_pruning_const.h"



class n_Delta_pruning delta_pruning;


extern class g_Evaluate evaluate;
extern class h_Generator_moves generator_moves;//
extern class i_Make_move make_move;//
extern class j_Sorting_moves sorting_moves;//
extern class m_PV_save pv_save;




//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
#if TEST_F
	std::ofstream Test_Loop_f;
#endif 
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT

/*

������������� ������� ������� 

 ����� �������� ������� ������ ��� �����. ���� ��� ��� ������ ����������.

 ������� ������� ������� ��������� �� ������� �����-���� ����������� ��� �������������
 ����������� ������ ��� ��������� ����� � ��������� �� ����. ���� ��� �������� ��� 
 ���������� �� ������ ������.

 ��� �� ������ ������ �������� ����� ��������� ��� ����� �����-���� �����������
 � ����� ����������� ��������� ������ �� ����������� �������.

 ����� ���� �������� ����� ��������� ������� ����� ������. ����� ������� ������ �������
 ����� ��� ����� � ���������� ������ � ���� �����. ����� ������� ����� ��������� ��������� 
 �����. ������ ���� �������. � ����� ��������� ������ ������� ��� ��� ����� � ���� ����.
 ����� ���������� ������ ������� ������ ����� � ��� ��������� ��� �����������. �������� 
 ����� ������� � ������ ��� ������ � ��������� ������ ������ �����. ���� ��� ��������� 
 ������ ���� ������� � ���������� ������ ����� ������ �����������. �� ������ ������ 
 ���������� ��� ��� ��� ���� ��� ����� ��� ������ ��������.
   
 � ������������� ������ �� ������� ������ ������ ��� ��� ��������� �� ����� ��� �����
 �� ����������� � ����� ������ ����������. ��� ���� ��� �� ��������� �� ������ 
 ������������� ������ �� ����������� ����� ����������� �������. � ����� ���� ���������
 ������ ������ ������� �� ��� ������� ��� � ���� ��� �� ��������� �� ���������� ������.

 ����� �� ���������� ����������� ������� �� ���� �� ������� �� ������������� ��� ���� 
 ������� ��� � ���� ������ � ��� ��� ���� ���� ������ �� �� ���� ������� ������ ���� 
 ������� ��. ��� ��� �� ������� ������ ������ � �� ����� ����� ����� ������������� ��
 ��������� �������� �������������� ��������.

1) ������� ����������� ������ �������.

  1.1 ���� ����������� ������ ������ ����� �� ����������� �� �����. ����� ������ 
   �������� ����� ����������� �������.

  1.2 ���� ����� �������� �� �������� ����� ������ �� ����. 

  1.3 �������� ���� �� ������ 1:

	if (value_stat > alpha)
	{ 
		if (value_stat >= beta)
		{
			return value_stat;
		}

		alpha = value_stat;
		save_best = 1;
	}

---------------------------------------
2) ����������� � ��������� �� mvv/lva ������ ��������� ������. ���������� �����
   �������������� �������� ��� ��������� ������.

  2.1  ������ ������� �����-����. 

  2.2 �������� ���� �� ������ 2:
	value_max_din = value_stat;

	if (value_din > value_max_din)
	{
		value_max_din = value_din;

		if (value_din > alpha)
		{
			if (value_din >= beta)
			{
				return value_din;
			}

			alpha = value_din;

			//����� �� �������������� ������ ������� ��������������� ������ ������
			PVSAVE::public_update_PV_best_point(&pv_best_point);

			save_best = 1;
		}
	}

  �������������� ������ ����������� � ��� ��� 
  �� ����� ������ �� ������� ������ ��� ����������� ������ ������� �������� �����
	if (save_best == 1)
	{
		if (value_max_din == value_stat)
		{
			// ����������� ������ �������� �� �������

			// ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
			// � ������ ������� ����� �������� ������
			PVSAVE::public_update_PV_best_max_depth(value_stat,depth); 
		}
		else
		{
			// ������ �� ������� ������ ������� ��������� � ��������� PV_best ��� �� �������� �� ����� ������� �������
			PVSAVE::public_update_PV_best(&pv_best_point);
		}
	}

*/

//	===================================================================
// ������������ ������ ������ ��� �����
__int32 f_Search_quiescence::forced_variant_white
(
	struct Position & position,// ������������� �����
	__int32 alpha,//
	const __int32 beta,//
	const unsigned __int8 depth,// �������
	const bool flag_pv_f// �������� �� ��� ������� (pv)
)
{
	bool save_best = 0;// ����� �� ������ �������
	__int32 value_din; // ������������ ������
	__int32 value_stat; // ����������� ������
	__int32 value_max_din; // �������� � ����
	const unsigned __int8 new_depth = depth + 1; // ��������� �������

	struct List list_surplus_moves;// ������ ��������� ����� (������������ ���� �� ������� ����)

	struct PV_line pv_best_point;// ������ ��� ������� ���� �������


	// ������� ����������� ������ �������
	value_stat = position.value_white_material
		- position.value_black_material
		+ evaluate.eval_state(position);

	// ������ ��������
	//value_stat = (bitboard.value_white_material - bitboard.value_black_material);


	//------------------------------------------------
	// TEST
	// ��������� �� ������������ ��������� ������ ���������

	//if (EVALUATE::eval_white_material(bitboard) != bitboard.value_white_material)
	//{
	//	std::cout << "WHITE eval_white_material != bitboard.value_white_material" <<"\n";
	//	std::cout << "eval_white_material(bitboard)= "<< EVALUATE::eval_white_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_white_material= "<< bitboard.value_white_material<<"\n";
	//}

	//if (EVALUATE::eval_black_material(bitboard) != bitboard.value_black_material)
	//{
	//	std::cout << "WHITE eval_black_material != bitboard.value_black_material" <<"\n";
	//	std::cout << "eval_black_material(bitboard)= "<< EVALUATE::eval_black_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_black_material= "<< bitboard.value_black_material<<"\n";
	//}

	//if (EVALUATE::eval_difference_material(bitboard) 
	//	!= (bitboard.value_white_material - bitboard.value_black_material) )
	//{
	//	std::cout << "WHITE eval_difference_material != (bitboard.value_white_material - bitboard.value_black_material)" <<"\n";
	//	std::cout << "eval_difference_material(bitboard)= "<< EVALUATE::eval_difference_material(bitboard)<<"\n";
	//	std::cout << "(bitboard.value_white_material - bitboard.value_black_material)= " << (bitboard.value_white_material - bitboard.value_black_material)<<"\n";
	//}
	//------------------------------------------------


	// �������� ����� ��� �� �� ������ ������������� ������.
	if (value_stat > alpha)
	{ 
		// �������� ����� ������ �� ���� 
		// ������ ������ �� ����?
		// ���� � ��� ��� � ������������� ������ �� ���������� ������� ����� ����� 
		// ������ � ��� ���� ����� ��������
		// ������ ����
		// �� ������� �� ���� ��� ���� � ��� ������� ��� �� ��������� ������
		// �� ������ ��� �� ������ �� �������
		if (value_stat >= beta)
		{
			return value_stat;
		}

		alpha = value_stat;
		save_best = 1;
	}

	/*
	�� ���� ��������� ��������� � ���������� delta pruning.
	���� �����, ��� ���� ������ ������ ����� �� ������� �����, �� ������� ����� 
	�������� �� ��������� � ����� ������ ����� ���������.  �� �� � ��� ������ �����.
	*/
#if DELTA_PRUNING

	// ���������� ������ �� �����
	delta_pruning.delta_pruning_remember_piese_white(position);

	if (value_stat < alpha - PAWN_MARGIN )
	{
		delta_pruning.delta_pruning_white(position,alpha,value_stat);
	}
#endif //#if DELTA_PRUNING

	//����������� ������ ��������� ������ � ��������� ���
	generator_moves.generator_captures_white(list_surplus_moves,position);
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);
	sorting_moves.sorting_moves_captures(list_surplus_moves);
	//if (depth == 2) List_Print_F(depth, &list_surplus_moves);

#if DELTA_PRUNING
	// ���������� �� ����� ������
    delta_pruning.delta_pruning_return_piese_white(position);
#endif //#if DELTA_PRUNING

    //
	value_max_din = value_stat;

	// ����� �� ������ ��������� �����
	// ����� � �����-���� ����������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++)
	{
		//TEST
		//cout << "white depth= " << depth<<"\n";
		//cout << "i = " << i <<"\n";
		//cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

		// ��������� ���
		make_move.do_moves_white(position,i,depth,list_surplus_moves);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� 
		// �� ������� �������.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		// ���� ������ �� �����
		if ((list_surplus_moves.move[i] & 63) != 6)
		{
			// ����������� ����� �������
			value_din = forced_variant_black(position,alpha,beta,new_depth
				,flag_pv_f);

			// ��������������� �������
			make_move.undo_moves_white(position,i,depth,list_surplus_moves);


			if (value_din > value_max_din)
			{

				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
				if (value_din > alpha) 
				{
					if (value_din >= beta)
					{
						return value_din;
					}

					alpha = value_din; 

					// ����� �� �������������� ������ ������� ��������������� 
					// ������ ������
					if (flag_pv_f)
					{
						pv_save.update_PV_best_point(pv_best_point);
					}

					save_best = 1;
				}
				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

				value_max_din = value_din;
			}

		}// ������������ ������ ������� ������
		else
		{
			// ������ ������� !
			value_stat = (VALUE_MATE - depth * DELTA_MATE);

			make_move.undo_moves_white(position,i,depth,list_surplus_moves);

			// ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
			// � ������ ������� ����� �������� ������
			if (flag_pv_f)
			{
				pv_save.update_PV_best_max_depth(value_stat,depth);
			}

			return value_stat;

		}
	}

	/*
	�������������� ������ ����������� � ��� ��� �� ����� ������ �� ������� ������
	��� ����������� ������ ������� �������� �����
	*/
	if (save_best == 1)
	{
		if (value_max_din == value_stat)
		{// ����������� ������ �������� �� �������

			// ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
			// � ������ ������� ����� �������� ������
			if (flag_pv_f)
			{
				pv_save.update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// ������ �� ������� ������ ������� ��������� � ��������� 
			// PV_best ��� �� �������� �� ����� ������� �������
			if (flag_pv_f)
			{
				pv_save.update_PV_best(pv_best_point);
			}
		}
	}
	return value_max_din;
}

//	===================================================================
// ������������ ������ ������ ��� ������
__int32 f_Search_quiescence::forced_variant_black
(
	struct Position & position,// ������������� �����
	const __int32 alpha,//
	__int32 beta,//
	const unsigned __int8 depth,// �������
	const bool flag_pv_f// �������� �� ��� ������� (pv)
)
{
	bool save_best = 0;// ����� �� ������ �������
	__int32 value_din; // ������������ ������
	__int32 value_stat; // ����������� ������
	__int32 value_min_din; //������� � ����
	const unsigned __int8 new_depth = depth + 1; // ��������� �������

	struct List list_surplus_moves; // ������ ��������� ����� (������������ ���� �� ������� ����)

	struct PV_line pv_best_point;// ������ ��� ������� ���� �������

	value_stat = position.value_white_material
		- position.value_black_material
		+ evaluate.eval_state(position);
 
	// ������ ��������
	//value_stat = bitboard.value_white_material - bitboard.value_black_material;

	//------------------------------------------------
	// TEST
	// ��������� �� ������������ ��������� ������ ���������

	//if (EVALUATE::eval_white_material(bitboard) != bitboard.value_white_material)
	//{
	//	std::cout << "BLACK eval_white_material != bitboard.value_white_material" <<"\n";
	//	std::cout << "eval_white_material(bitboard)= "<< EVALUATE::eval_white_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_white_material= "<< bitboard.value_white_material<<"\n";
	//}

	//if (EVALUATE::eval_black_material(bitboard) != bitboard.value_black_material)
	//{
	//	std::cout << "BLACK value_stat != bitboard.value_black_material" <<"\n";
	//	std::cout << "eval_black_material(bitboard)= "<< EVALUATE::eval_black_material(bitboard)<<"\n";
	//	std::cout << "bitboard.value_black_material= "<< bitboard.value_black_material<<"\n";
	//}

	//if (EVALUATE::eval_difference_material(bitboard) 
	//	!= (bitboard.value_white_material - bitboard.value_black_material) )
	//{
	//	std::cout << "BLACK eval_difference_material != (bitboard.value_white_material - bitboard.value_black_material)" <<"\n";
	//	std::cout << "eval_difference_material(bitboard)= "<< EVALUATE::eval_difference_material(bitboard)<<"\n";
	//	std::cout << "(bitboard.value_white_material - bitboard.value_black_material)= " << (bitboard.value_white_material - bitboard.value_black_material)<<"\n";
	//}
	//------------------------------------------------


	if (value_stat < beta)
	{
		if (value_stat <= alpha)
		{
			return value_stat;
		}

		beta = value_stat;
		save_best = 1;
	}


#if DELTA_PRUNING

	// ���������� ������ �� �����
	delta_pruning.delta_pruning_remember_piese_black(position);


	if (value_stat > beta + PAWN_MARGIN)
	{
		delta_pruning.delta_pruning_black(position,beta,value_stat);
	}
#endif //#if DELTA_PRUNING

	//����������� ������ ��������� ������
	generator_moves.generator_captures_black(list_surplus_moves,position);
	sorting_moves.sorting_moves_captures(list_surplus_moves);

#if DELTA_PRUNING
	// ���������� �� ����� ������
	delta_pruning.delta_pruning_return_piese_black(position);
#endif //#if DELTA_PRUNING


	//
	value_min_din = value_stat;

	//���� �� ��� ��� ������� �� ���� ������ ����������� ������ �� �����

	//����� �� ������ ��������� �����
	// ����� � ����� ���� ����������
	for (unsigned __int8 i = 0; i < list_surplus_moves.end_captures; i++)
	{
		//TEST
		//cout << "black depth= " << depth<<"\n";
		//cout << "i = " << i <<"\n";
		//cout << "list_surplus_moves.move[i] & 63 = " << (list_surplus_moves.move[i] & 63) <<"\n";

		// ��������� ���
		make_move.do_moves_black(position,i,depth,list_surplus_moves);

		// �� ������ ��������� ����� �������� ������� ��� � ������� ������� 
		// �� ������� �������.
		if (flag_pv_f)
		{
			pv_save.update_PV_current(i,depth,list_surplus_moves);
		}

		// ���� ������ �� �����
		if ((list_surplus_moves.move[i] & 63) != 6)
		{
			// ����������� ����� �������
			value_din = forced_variant_white(position,alpha,beta,new_depth
				,flag_pv_f);

			// ��������������� �������
			make_move.undo_moves_black(position,i,depth,list_surplus_moves);


			if (value_din < value_min_din)
			{
				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
				if (value_din < beta)
				{
					if (value_din <= alpha)
					{
						return value_din;
					}

					beta = value_din; 

					// ����� �� �������������� ������ ������� ��������������� 
					// ������ ������
					if (flag_pv_f)
					{
						pv_save.update_PV_best_point(pv_best_point);
					}

					save_best = 1;
				}
				//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

				value_min_din = value_din;
			}

		
		}// ������������ ������ ������� ������
		else
		{
			// ������ �������!
			value_stat = -(VALUE_MATE - depth * DELTA_MATE);
			make_move.undo_moves_black(position,i,depth,list_surplus_moves);

			// ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
			// � ������ ������� ����� �������� ������
			if (flag_pv_f)
			{
				pv_save.update_PV_best_max_depth(value_stat,depth);
			}

			return value_stat;

		}
	}

	if (save_best == 1)
	{
		if (value_min_din == value_stat)
		{
			// ����������� ������ �������� �� �������

			// ����� �� �������� ������ �������� ������� ������������ �������� �� ��� �� 
			// � ������ ������� ����� �������� ������
			if (flag_pv_f)
			{
				pv_save.update_PV_best_max_depth(value_stat,depth);
			}
		}
		else
		{
			// ������ �� ������� ������ ������� ��������� � ��������� PV_best 
			// ��� �� �������� �� ����� ������� �������
			if (flag_pv_f)
			{
				pv_save.update_PV_best(pv_best_point);
			}
		}
	}
	return value_min_din;
}

//	===================================================================
//TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// �������� ������
#if TEST_F
void f_Search_quiescence::list_Print_F
(
	int depth,// ������� �������
	const class List_surplus_moves & list_surplus_moves//������ �����
)
{

	Test_Loop_f.open ("_TEST_LOOP_FORCED.txt",std::ios::app);	
	//Test_Loop_f.open ("_TEST_LOOP_FORCED.txt");	
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop_f << "\n";
	Test_Loop_f << "------------------------------------------------- "<<"\n";
	Test_Loop_f <<"depth= "<< depth << "\n";
	Test_Loop_f << "\n";
	Test_Loop_f << "������ ��������� ����� ������ "<<"\n";

	for (int i = 0 ; i < list_surplus_moves->end_list ; i++)
	{	
		Test_Loop_f << "����� ���� �� ������ i= " << i << "\n";  
		Test_Loop_f << "������� ������ " << UTIL::public_int_to_char_board(((list_surplus_moves->move[i]>>24)& 7),1)<< "\n";
		Test_Loop_f << "������ ���� " << list_surplus_moves->sorting_score[i] << "\n";

		// ����� ���� ���� e2-e4
		//------------------------------
		Test_Loop_f << " " << (UTIL::public_x_to_notation(((list_surplus_moves->move[i]>>12)& 63)%8));
		Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>12)& 63)/8);//<< "\n";
		//Test_Loop_f << "\n";
		Test_Loop_f << " " << (UTIL::public_x_to_notation(((list_surplus_moves->move[i]>>18)& 63)%8));
		Test_Loop_f << "" << (1 + ((list_surplus_moves->move[i]>>18)& 63)/8)<< "\n";   
		//------------------------------

		Test_Loop_f << "��� ���� " << ((list_surplus_moves->move[i]>>6)& 63)  << "\n";
		Test_Loop_f << "������ ������ " << UTIL::public_int_to_char_board(( list_surplus_moves->move[i] & 63),1)<< "\n";
		Test_Loop_f << "��������� ��������� " << ((list_surplus_moves->move[i]>>12)& 63)  << "\n";
		Test_Loop_f << "�������� ��������� " << ((list_surplus_moves->move[i]>>18)& 63)  << "\n";
      
		Test_Loop_f << "\n";
	}

	Test_Loop_f << "list_surplus_moves->end_captures= " <<list_surplus_moves->end_captures << "\n";
	Test_Loop_f << "list_surplus_moves->end_list= " <<list_surplus_moves->end_list << "\n";
	Test_Loop_f << "------------------------------------------------- " << "\n";
	//GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	Test_Loop_f.close();
}
#endif
