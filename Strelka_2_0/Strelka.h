#define KEY_INDEX               (unsigned int)(Board->key)
#define KEY_LOCK                (unsigned int)((Board->key)>>32)
#define LINE1(square)           (LineMask[0][square][((Board->md3)>>LineShift[0][square])&0x3F])
#define LINE2(square)           (LineMask[1][square][((Board->md4)>>LineShift[1][square])&0x3F])
#define LINE3(square)           (LineMask[2][square][((Board->md1)>>LineShift[2][square])&0x3F])
#define LINE4(square)           (LineMask[3][square][((Board->md2)>>LineShift[3][square])&0x3F])
#define ILLEGAL_POSITION        ((Board->mp[13 - Board->turn]&(pos_info_entry->mob[0])))
#define MOVE_FROM(move)         ((move>>6)&0x3F)
#define MOVE_TO(move)           (move&0x3F)
#define PIECE_IS_PAWN(piece)    ((piece&0x0E)==2)
#define MAX_MATERIAL            2*2*3*3*3*3*3*3*9*9
#define MOVE_IS_PROMOTION(move) (move&0x4000)
#define MOVE_IS_CASTLING(move)  ((move&0x7000)==0x2000)
#define MOVE_IS_ENPASSANT(move) ((move&0x7000)==0x1000)
#define PIECE_PROMO(move,side)  (10+side-((move&0x3000)>>11))

struct mat_info_t {
  unsigned __int16 flags;
  __int16 value;
};

struct entry_t {
  unsigned int lock;
  unsigned __int16 move;
  char depth;
  unsigned char date;
  __int16 min_value;
  __int16 max_value;
  char move_depth;
  unsigned char flags;
  char min_depth;
  char max_depth;
};

struct board_t {
  int square[64];
  unsigned __int64 mp[14];
  unsigned __int64 md1;
  unsigned __int64 md2;
  unsigned __int64 md3;
  unsigned __int64 md4;
  int turn;
  int flags;
  int ep_square;
  int ply_nb;
  int sp;
  int opening;
  int endgame;
  unsigned __int64 key;
  unsigned __int64 pawn_key;
  int mat_summ;
  int mat_diff;
  unsigned __int64 stack[1024];
};

struct undo_t {
  int flags;
  int ep_square;
  int ply_nb;
  int sp;
  int opening;
  int endgame;
  unsigned __int64 key;
  unsigned __int64 pawn_key;
  int mat_summ;
  int mat_diff;
  int capture;
};

struct list_t {
  int move;
  int score;
};

struct moves_info_t {
  struct list_t list[128];
  struct list_t * next;
  int bad_captures[128];
  int bad_cnt;
  int bad_index;
  int entry;
  int trans_move;
};

struct undo_null_t {
  int ep_square;
  int ply_nb;
  unsigned __int64 key;
};

struct pos_info_t {
  unsigned __int64 mob[2];
  int killer1;
  int killer2;
  int value;
  int move;
};

struct pawn_info_t {
  unsigned int lock;
  __int16 opening;
  __int16 endgame;
  __int16 sheet_white_king[3];
  __int16 sheet_black_king[3];
  unsigned char pass_file[2];
  __int16 dummy;
};

void get_uci_command(void);
void send_best_move(void);
int  input_available(void);
void move_to_string(int, char []);

void board_from_fen(const char []);
void board_init(struct board_t *);
int  popcnt(unsigned __int64);
int  line_attack(int, int);
void init_bitboards(void);
void init_evals(void);

void trans_alloc(int);
void trans_set_date(int);
void trans_clear(void);
void trans_store(unsigned __int16 move, char depth, __int16 value);
void trans_move_store(unsigned __int16 move, char depth, __int16 value);
void trans_min_store(char depth, __int16 value);
void trans_max_store(char depth, __int16 value);

int  first_one(unsigned __int64);
int  last_one(unsigned __int64);
int  evaluate(struct pos_info_t *);
int  eval_mob(struct pos_info_t *);
struct pawn_info_t * pawn_get_info(void);

void start_go(char []);
void start_search(void);
int  full_root(int);
int  full_search(int, int, int, int *);
int  full_check_search(int, int, int, int *);
int  full_pv_search(int, int, int, int, int *);
int  qu_search(int, int, int, int *);
int  qu_check_search(int, int, int, int *);
int  next_move(struct moves_info_t *, struct pos_info_t *);
void search_check(void);

void move_do(int, struct undo_t *);
void move_undo(int, struct undo_t *);
void move_do_null(struct undo_null_t *);
void move_do_castle(int);
void move_undo_castle(int);

struct list_t * gen_evasions(struct list_t *, unsigned __int64);
void gen_captures(struct list_t *, unsigned __int64, unsigned __int64);
void gen_quiet_moves(struct list_t *, unsigned __int64, int, int, int);
void gen_checks(struct list_t *, unsigned __int64, unsigned __int64);

int  see_move(int);
int  move_is_legal(int, struct pos_info_t *);
void history_store(struct pos_info_t *, int, int);
void note_moves(struct list_t *, struct list_t *, struct pos_info_t *, int);
int  move_order(int);
