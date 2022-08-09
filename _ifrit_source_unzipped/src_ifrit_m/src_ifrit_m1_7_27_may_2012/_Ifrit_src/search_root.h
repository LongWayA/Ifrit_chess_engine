
/// АВТОР Бренкман Андрей (Brenkman Andrey)
/// ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ 2.11.2009 20.12.2009
/// УРОВЕНЬ МОЕГО ДОВЕРИЯ 50%


///!
#include "a_move_list_struct.h"
#include "a_position_struct.h"
#include "a_pv_line_struct.h"

/*
Root
http://chessprogramming.wikispaces.com/Root

The Root of the Search Tree is the initial position of the search. 
Many programmers dedicate a separate function for doing a root search, 
since using a different move ordering scheme may be beneficial. Also root 
search must return not only the value, but also the move which will be 
played. This in turn may require special care in case of a fail-low when 
using aspiration window. This is typically where the time management 
decisions are beneficial.

end link(конец цитаты)---------------------
*/

/*
Late Move Reductions (LMR)
http://chessprogramming.wikispaces.com/Late+Move+Reductions

Late Move Reductions (LMR), or its version known as History Pruning 
and History Reductions [1] , save search by reducing moves that are 
ordered closer to the end of likely fail-low nodes. Typically, most 
schemes search the first few moves (say 3-4) at full depth, then if 
no move fails high, many of the remaining moves are reduced in search 
depth. The technique has been used for many years in various forms, 
but it became very popular in 2005 after Fruit and Glaurung [2] used 
open source implementations based on the History Heuristic.
LMR can often reduce the effective branching factor to less than 2, 
depending on the reduction conditions.

Common Conditions
 Most programs do not reduce these types of moves:
 Tactical moves (captures and promotions)
 Moves while in check
 Moves which give check
 Moves that cause a search extension
 Anytime in a PV-Node in a PVS search
 Depth<3 (sometimes depth<2)

Less Common Conditions
 Less common conditions on moves not to reduce:
 Passed Pawn Moves
 Killer Moves
 Moves Threatening the King area
 Tactically threatening moves
 Moves with good past relative history [4]
 Any Pawn Moves
 Allowing reductions of "bad" captures
 Moves of a threatened piece to safety (often detected via a Null Move search)

Deeper Reductions
 Some programs allow reductions of more 
 than one ply in even "later" moves. 
 Useful conditions are expected node type, 
 depth and move number.

Re-searches
 Many programs do a re-search at full depth 
 if the reduced depth search returns a score above alpha.

end link(конец цитаты)---------------------
*/

/*
Principal Variation Search (PVS)
http://chessprogramming.wikispaces.com/Principal+Variation+Search

Principal Variation Search (PVS) is an enhancement to 
Alpha-Beta, based on null- or zero window searches of 
none PV-nodes, to prove a move is worse or not than an 
already safe score from the principal variation.

The Idea
 In most of the nodes we need just a bound, proving that 
 a move is unacceptable for us or for the opponent, and 
 not the exact score. This is needed only in so-called principal 
 variation - a sequence of moves acceptable for both players 
 (i.e. not causing a beta-cutoff anywhere in the path) which 
 is expected to propagate down to the root. If a lower-depth 
 search has already established such a sequence, finding a 
 series of moves whose value is greater than alpha but lower 
 than beta throughout the entire branch, the chances are that 
 deviating from it will do us no good. So in a PV-node only 
 the first move (the one which is deemed best by the previous 
 iteration of an iterative deepening framework) is searched 
 in the full window in order to establish the expected node value.

 When we already have a PV-move (defined as the move that 
 raised alpha in a PV-node) we assume we are going to stick 
 with it. To confirm our belief, a null- or zero window 
 search centered around alpha is conducted to test if a 
 new move can be better. If so, with respect to the null 
 window but not with respect to the full window, we have 
 to do a re-search with the full normal window. Since null 
 window searches are cheaper, with a good move ordering we 
 expect to save about 10% of a search effort.

Bruce Moreland's PVS implementation waits until a move is 
found that improves alpha, and then searches every move 
after that with a zero window around alpha [1] . The alpha 
improvement usually occurs at the first move, and always at 
the leftmost nodes (assuming from left to right traversal) 
with a most open alpha-beta window of +-oo. In re-searches 
or with aspiration-windows the first moves may rarely not 
improve alpha. As pointed out by Edmund Moshammer, 
Gian-Carlo Pascutto, Robert Hyatt and Vincent Diepeveen [2] , 
it is recommend to only search the first move with an open 
window, and then every other move after that with a zero window. 
A further improvement (similar to that known from the 
NegaScout algorithm) is possible. Since there is not much 
to be gained in the last two plies of the normal search, one 
might disable PVS there, but programs respond differently to 
that change.

PVS and NegaScout

Most PVS implementations are similar to Reinefeld's NegaScout [10] [11] 
, and are used by most todays chess programs. It is based on the 
accuracy of the move ordering. Typically, modern chess programs 
find fail-highs on the first move around 90% of the time. This 
observation can be used to narrow the window on searches of moves 
after the first, because there is a high probability that they 
will be lower than the score of the first move.

 Reinefeld's original implementation introduces one additional 
 variable on the stack (only b, since after a = alpha, alpha is 
 not needed any longer), for a slightly simpler control structure 
 than PVS. It has therefor set a new null window at the end of the 
 loop (b = a + 1), but has to consider the move count for the 
 re-search condition though. His implementation trusts the 
 null-window score, even if the re-search doesn't confirm the 
 alpha increase, eventually due to search instability. While 
 re-searching, it uses the narrow window of {score, beta}, 
 while other implementations dealing with search instability, 
 re-search with {alpha, beta}. Practically, due to 
 Quiescence Search, and fail-soft implementations of PVS, 
 the two algorithms are essentially equivalent to each other - 
 they expand the same search tree [12] [13] .

end link(конец цитаты)---------------------
*/

/*
Alpha-Beta algorithm
http://chessprogramming.wikispaces.com/Alpha-Beta

The Alpha-Beta algorithm (Alpha-Beta Pruning, 
Alpha-Beta Heuristic [1]) is a significant enhancement 
to the minimax search algorithm that eliminates the need 
to search large portions of the game tree applying a 
branch-and-bound technique. Remarkably, it does this 
without any potential of overlooking a better move. 
If one already has found a quite good move and search 
for alternatives, one refutation is enough to avoid it. 
No need to look for even stronger refutations. The algorithm 
maintains two values, alpha and beta. They represent the 
minimum score that the maximizing player is assured of and 
the maximum score that the minimizing player is assured of 
respectively. Consider the following example...

How it works
 Say it is White's turn to move, and we are searching to a 
 depth of 2 (that is, we are consider all of White's moves, 
 and all of Black's responses to each of those moves.) First 
 we pick one of White's possible moves - let's call this 
 Possible Move #1. We consider this move and every possible 
 response to this move by black. After this analysis, we determine 
 that the result of making Possible Move #1 is an even position. 
 Then, we move on and consider another of White's possible moves 
 (Possible Move #2.) When we consider the first possible counter-move 
 by black, we discover that playing this results in black winning a 
 Rook! In this situation, we can safely ignore all of Black's other 
 possible responses to Possible Move #2 because we already know that 
 Possible Move #1 is better. We really don't care exactly how much 
 worse Possible Move #2 is. Maybe another possible response wins a 
 Queen, but it doesn't matter because we know that we can achieve at 
 least an even game by playing Possible Move #1. The full analysis of 
 Possible Move #1 gave us a lower bound. We know that we can achieve 
 at least that, so anything that is clearly worse can be ignored.

 The situation becomes even more complicated, however, when we go 
 to a search depth of 3 or greater, because now both players can 
 make choices affecting the game tree. Now we have to maintain both 
 a lower bound and an upper bound (called Alpha and Beta.) We maintain 
 a lower bound because if a move is too bad we don't consider it. But 
 we also have to maintain an upper bound because if a move at depth 3 
 or higher leads to a continuation that is too good, the other player 
 won't allow it, because there was a better move higher up on the game 
 tree that he could have played to avoid this situation. One player's lower
 bound is the other player's upper bound.

Savings
 The savings of alpha beta can be considerable. If a standard minimax 
 search tree has x nodes, an alpha beta tree in a well-written program 
 can have a node count close to the square-root of x. How many nodes 
 you can actually cut, however, depends on how well ordered your game 
 tree is. If you always search the best possible move first, you eliminate 
 the most of the nodes. Of course, we don't always know what the best move 
 is, or we wouldn't have to search in the first place. Conversely, if we 
 always searched worse moves before the better moves, we wouldn't be able 
 to cut any part of the tree at all! For this reason, good move ordering 
 is very important, and is the focus of a lot of the effort of writing a 
 good chess program. As pointed out by Levin in 1961, assuming constantly b 
 moves for each node visited and search depth n, the maximal number of 
 leaves in alpha-beta is equivalent to minimax, b ^ n. Considering always 
 the best move first, it is b ^ ceil(n/2) plus b ^ floor(n/2) minus one.

end link(конец цитаты)---------------------
*/

/*
Minimax
http://chessprogramming.wikispaces.com/Minimax

Minimax is an algorithm used to determine the score in a zero-sum game 
after a certain number of moves, with best play according to an 
evaluation function.

 The algorithm can be explained like this: In a one-ply search, 
 where only move sequences with length one are examined, the side 
 to move (max player) can simply look at the evaluation after playing 
 all possible moves. The move with the best evaluation is chosen. 
 But for a two-ply search, when the opponent also moves, things 
 become more complicated. The opponent (min player) also chooses 
 the move that gets the best score. Therefore, the score of each 
 move is now the score of the worst that the opponent can do.

end link(конец цитаты)---------------------
*/

/*
 перебор позиций
 включает в себя данные и методы  перебора в корне т.е. на нулевой глубине
*/

namespace Search_root
{

/// поля класса
//////////////////////////////////////////////////////////////////////////////////////
//private:


//////////////////////////////////////////////////////////////////////////////////////
//public:

/*
	Поиск на нулевом уровне за белых
*/
	__int32 white_root
	(
		struct Position & position,/// представление доски
		__int32 alpha,///
		const __int32 beta,/// 
		const unsigned __int8 depth_max/// максимальная глубина
	);

/*
	Поиск на нулевом уровне за черных
*/
	__int32 black_root
	(
		struct Position & position,/// представление доски
		const __int32 alpha,///
		__int32 beta,/// 
		const unsigned __int8 depth_max/// максимальная глубина
	);

/*
	возвращаем количество рассмотренных узлов
*/
	__int64  get_nodes();

/*
	обнуляем количество рассмотренных узлов
*/
	void set_nodes_in_0();

/*
	возвращаем откуда делается ход
*/
	unsigned __int8 get_from();

/*
	возвращаем куда делается ход
*/
	unsigned __int8 get_to();

//////////////////////////////////////////////////////////////////////////////////////////
//public:

/*
	печатаем список
	глубина перебора, список
*/
	void list_print
	(
		int Depth,// текущая глубина
		struct List & list_surplus_moves// список возможных ходов
	);


};