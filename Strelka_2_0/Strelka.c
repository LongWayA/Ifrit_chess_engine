/////////////////////////////////////////////////////////////////////////
// Стрелка 2.0 B
// Версия от 09.01.2008
// Почти повторяет версию Стрелки 1.8 UCI от 26.06.2007
// Исправлены некоторые баги, переписана начальная инициализация таблиц.
// Главные модификации алгоритма, позволившие усилить игру на +60 пунктов
// сделаны на основе идей Игоря Коршунова, автора Wildcat.
// Варианты Коршунова отмечены в тексте как ссылки на различные версии
// движка "Белка", выпущенные в период с ноября по декабрь 2007 г.
//////////////////////////////////////////////////////////////////////////

#include "windows.h"
#include "stdio.h"
#include <csetjmp>

#include "strelka.h"

const int DateSize = 4;
const int ClusterSize = 4;

const int White = 0;
const int Black = 1;
const int WhitePawn = 2;
const int BlackPawn = 3;
const int WhiteKnight = 4;
const int BlackKnight = 5;
const int WhiteBishop = 6;
const int BlackBishop = 7;
const int WhiteRook = 8;
const int BlackRook = 9;
const int WhiteQueen = 10;
const int BlackQueen = 11;
const int WhiteKing = 12;
const int BlackKing = 13;

const int FlagCastle      = 0x2000;
const int FlagEnpassant   = 0x1000;
const int FlagPromoQueen  = 0x4000;
const int FlagPromoRook   = 0x5000;
const int FlagPromoBishop = 0x6000;
const int FlagPromoKnight = 0x7000;

struct mat_info_t *Material;

char LineIndex[64][64];
extern const int ValuePiece[14];
unsigned __int64 MaskPawnDoubled[64];
unsigned __int64 MaskPawnIsolated[64];
unsigned __int64 MaskPawnProtectedW[64];
unsigned __int64 MaskPawnProtectedB[64];
unsigned __int64 MaskPawnAttacksW1[64];
unsigned __int64 MaskPawnAttacksB1[64];
unsigned __int64 MaskPawnAttacksW2[64];
unsigned __int64 MaskPawnAttacksB2[64];
unsigned __int64 MaskPawnPassedW[64];
unsigned __int64 MaskPawnPassedB[64];
unsigned __int64 MaskPawnOpenFileW[64];
unsigned __int64 MaskPawnOpenFileB[64];
unsigned __int64 MaskPawnAttacks[2][64];
unsigned __int64 MaskClearSquare[64];
unsigned __int64 MaskKnightMoves[64];
unsigned __int64 MaskKingMoves[64];
unsigned __int64 MaskSetMD2[64];
unsigned __int64 MaskClearMD2[64];
unsigned __int64 MaskSetMD3[64];
unsigned __int64 MaskClearMD3[64];
unsigned __int64 MaskSetMD4[64];
unsigned __int64 MaskClearMD4[64];
unsigned __int64 WhitePawnMoves1[64];
unsigned __int64 BlackPawnMoves1[64];
unsigned __int64 WhitePawnMoves2[64];
unsigned __int64 BlackPawnMoves2[64];
unsigned __int64 WhitePawnCaptures1[64];
unsigned __int64 BlackPawnCaptures1[64];
unsigned __int64 WhitePawnCaptures2[64];
unsigned __int64 BlackPawnCaptures2[64];
unsigned __int64 MaskLines[64];
unsigned __int64 MaskDiags[64];
unsigned __int64 WhiteKingOpp[64];
unsigned __int64 BlackKingOpp[64];
unsigned __int64 QuadBlackKing[64][2];
unsigned __int64 QuadWhiteKing[64][2];
__int16 PawnStruScore0[4096];
__int16 PawnStruScore1[4096];
__int16 PieceSquareValue[12][64][2];

extern const unsigned __int64 RandomTurn;
extern const unsigned __int64 RandomCastle[16];
extern const unsigned __int64 RandomEP[8];
extern const unsigned __int64 Random[12][64];

extern const unsigned char PawnPassedFile[8];
extern const __int16 PawnPassedScoreOpening[8];
extern const __int16 PawnPassedScoreEndgame[8];
extern const char LineShift[4][64];
unsigned __int64 LineMask[4][64][64];
extern const int MatDiffValue[12];
extern const int MatSummValue[12];
extern const unsigned __int64 MaskEnPassant[2][8];
extern const unsigned int CastleFlags[64];
extern const unsigned __int64 MaskPieceIsPawn[14];
unsigned __int64 MaskEvasionMoves[64][64][2];
int PieceFromChar[256];
extern const unsigned __int64 MaskFile[8];
extern const int FileWing[64];
int BsfByte[256];
extern const int PassedPawnValue1[8];
extern const int PassedPawnValue2[8];
extern const int PassedPawnValue3[8];
extern const int PassedPawnValue4[8];
extern const int PassedPawnValue5[8];
extern const int PassedPawnValue6[8];
extern const int PassedPawnValue7[8];
char Distance[64][64];
extern const int KingAttackWeight[16];

struct board_t Board[1];
struct entry_t * trans_entry;
int trans_mask;
int trans_date;
int trans_size;
int trans_score[16];
struct pawn_info_t * pawn_entry;
unsigned int pawn_mask;

int Searching = 0;
int Infinite = 0;
int Delay = 0;
int Post = 1;
int MultiPV = 1;
struct pv_t {
  int pv[256];
  int score;
  int time;
  __int64 nodes;
  __int64 speed;
} multipv[100];
__int64 true_nodes;
int depth_is_limited, time_is_limited;
int depth_limit;
int time_limit_1, time_limit_2, time_max;
int can_stop;
int check_nb;
int best_score, best_move, best_depth, depth_score;
int best_pv[256];
int trans_move, best_trans_move;
int History[12][64];
struct pos_info_t Pos_info[256];
struct pos_info_t * pos_info_entry;
struct list_t list_root[256];
int start_time, search_time;
char pv_str[1000];
jmp_buf jmp_buffer;
int stop_search;
int bad_1, bad_2, change, easy, flag;
int testing = 0;

char start_pos[500] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";

#include "trans.c"
#include "board.c"
#include "search.c"
#include "eval.c"
#include "genmov.c"
#include "utils.c"
#include "emater.c"

//////////////////////////////////////////////////////////////////////////
void main()
{
  init_bitboards();
  init_evals();
 
  Material = malloc(MAX_MATERIAL * sizeof(struct mat_info_t));
  calc_material();

  // установка нулевых буферов ввода-вывода для UCI
  setbuf(stdout, NULL);
  setbuf(stdin, NULL);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stdin, NULL, _IONBF, 0);
  fflush(NULL);
  // инициализация таблицы транспозиций
  trans_alloc(32);
  // инициализация таблицы пешечной структуры
  pawn_entry = malloc(0xC00000);
  pawn_mask = (0xC00000 / sizeof(struct pawn_info_t)) - 1;
  memset(pawn_entry, 0, 0xC00000);
  // инициализация позиции
  board_from_fen(start_pos);
  // цикл команд UCI
  while (1) get_uci_command();
}
