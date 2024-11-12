/******************************************************************
  Parse.c - Функции обработки команд UCI
*******************************************************************/
#include <stdio.h>
#include <windows.h>
#include "strelka.h"

extern const int White;
extern const int Black;
extern const int FlagCastle;
extern const int FlagEnpassant;
extern const int FlagPromoQueen;
extern const int FlagPromoRook;
extern const int FlagPromoBishop;
extern const int FlagPromoKnight;

extern struct board_t Board[1];
extern int stop_search;
extern int best_move;
extern int best_score;
extern int best_depth;
extern int best_pv[];
extern char start_pos[];
extern __int64 true_nodes;
extern int start_time, search_time;
extern int testing;
extern int History[12][64];
extern struct pawn_info_t * pawn_entry;
extern struct entry_t * trans_entry;
extern struct pos_info_t Pos_info[];
extern struct pos_info_t * pos_info_entry;
extern struct list_t list_root[];

extern int Searching;
extern int Infinite;
extern int Delay;
extern int Post;
extern int MultiPV;

void parse_position(char []);
int  parse_move(char []);

void move_to_string(int move, char string[])
{ int pos = 0;

  string[pos++] = ((move >> 6) & 7) + 'a';
  string[pos++] = ((move >> 9) & 7) + '1';
  string[pos++] = (move & 7) + 'a';
  string[pos++] = ((move >> 3) & 7) + '1';
  if (MOVE_IS_PROMOTION(move)) {
    if ((move & 0x7000)      == FlagPromoQueen)  string[pos++] = 'q';
    else if ((move & 0x7000) == FlagPromoRook)   string[pos++] = 'r';
    else if ((move & 0x7000) == FlagPromoBishop) string[pos++] = 'b';
    else if ((move & 0x7000) == FlagPromoKnight) string[pos++] = 'n';
  }
  string[pos] = 0;
}

int move_from_string(char cmove[])
{ int from, to, move;

  from = ((cmove[1] - '1') * 8) + (cmove[0] - 'a');
  to   = ((cmove[3] - '1') * 8) + (cmove[2] - 'a');
  move = (from << 6) | to;
  if (Board->square[from] == 12 && ((from - to) == 2 || (to - from) == 2)) move |= FlagCastle;
  if (Board->square[from] == 13 && ((from - to) == 2 || (to - from) == 2)) move |= FlagCastle;
  if (Board->ep_square != 0 && to == Board->ep_square) move |= FlagEnpassant;
  if (cmove[4] != 0) {
    if (cmove[4]      == 'q') move |= FlagPromoQueen;
    else if (cmove[4] == 'r') move |= FlagPromoRook;
    else if (cmove[4] == 'b') move |= FlagPromoBishop;
    else if (cmove[4] == 'n') move |= FlagPromoKnight;
  }
  return move;
}

int input_available()
{ static int init = 0, is_pipe;
  static HANDLE stdin_h;
  DWORD val;

  if (stdin->_cnt > 0) return 1;
  if (!init) {
    init = 1;
    stdin_h = GetStdHandle(STD_INPUT_HANDLE);
    is_pipe = !GetConsoleMode(stdin_h,&val);
    if (!is_pipe) {
      SetConsoleMode(stdin_h,val&~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
      FlushConsoleInputBuffer(stdin_h);
    }
  }
  if (is_pipe) {
    if (!PeekNamedPipe(stdin_h,NULL,0,NULL,&val,NULL)) return 1;
    return val > 0;
  }
  else {
    GetNumberOfConsoleInputEvents(stdin_h,&val);
    return val > 1;
  }
  return 0;
}

void send_best_move()
{ char string[20], ponder[20];
  int time;
  __int64 speed = 0;

  if (!Post) return;
  time = GetTickCount() - start_time;
  if (time > 0) speed = (true_nodes * 1000) / time;
  fprintf(stdout,"info time %d nodes %I64d nps %I64d\n",time,true_nodes,speed);
  move_to_string(best_move, string);
  if (best_pv[0] == best_move && best_pv[1] != 0) {
    move_to_string(best_pv[1], ponder);
    fprintf(stdout,"bestmove %s ponder %s\n",string,ponder);
  }
  else fprintf(stdout,"bestmove %s\n",string);
}

#ifdef TEST_VER

void run_epd_test(char string[])
{ char fen[1000];
  char *epd_file;
  FILE * finp;
  FILE * ft;
  FILE * fst;
  int n = 0;
  int time = 0;
  __int64 cnt = 0;
  char *go_str = "go depth 10";
  int count_ok = 0;
  int move;
  char *ptr;
  char str[256];
  int count_key = 0, ok_key = 0;

  epd_file = strchr(string,' ');
  if (epd_file == NULL) return;
  epd_file++;
  if (strlen(epd_file) == 0) return;
  if ((finp = fopen(epd_file, "r")) == NULL) {
    fprintf(stdout,"Can't Open File %s\n", epd_file);
    return;
  }
  testing = 1; Post = 0;
  ft = fopen("test.txt", "w"); fclose(ft);
  fst = fopen("stat.txt", "w"); fclose(fst);
  while (!feof(finp)) {
    if (!fgets(fen, (int)sizeof(fen), finp)) continue;
    if (!memcmp(fen,"ucinewgame",10)) { trans_clear(); continue; }
    n++;
    fprintf(stdout,"P%4.4d ",n);
    ft = fopen("test.txt","a");
    fprintf(ft,"P%4.4d\n",n);
    fclose(ft);
    board_from_fen(fen);
    move = 0;
    if (ptr = strstr(fen,"bm ")) move = move_from_string(ptr + 3);
    if (strstr(fen,"; key")) count_key++;

//    trans_clear();
//    memset(History, 0, 12*64*sizeof(int));
//    memset(pawn_entry, 0, 0xC00000);

    strcpy(str,go_str);
    start_go(str);
    if (move != 0 && best_move == move) {
      count_ok++;
      if (strstr(fen,"; key")) ok_key++;
    }
    ft = fopen("test.txt","a");
    fprintf(ft,"Nodes : %I64d\n\n",true_nodes);
    fclose(ft);
    cnt += true_nodes;
    time += search_time;
    fprintf(stdout,"Nodes = %I64d Time = %d Ok = %d/%d Key = %d/%d\n",
      cnt,time,count_ok,n,ok_key,count_key);
    if ((n % 50) == 0) {
      fst = fopen("stat.txt","a");
      fprintf(fst,"P%4.4d Nodes = %I64d Time = %d Ok = %d/%d Key = %d/%d\n",
        n,cnt,time,count_ok,n,ok_key,count_key);
      fclose(fst);
    }
  }
  fst = fopen("stat.txt","a");
  fprintf(fst,"P%4.4d Nodes = %I64d Time = %d Ok = %d/%d Key = %d/%d\n",
    n,cnt,time,count_ok,n,ok_key,count_key);
  fclose(fst);
  fclose(finp);
  exit(0);
}

void gen_legal_moves()
{ int i, k, num_moves, legal;
  struct undo_t undo[1];

  pos_info_entry = Pos_info;
  memset(list_root, 0, 256 * sizeof(struct list_t));
  if (evaluate(pos_info_entry)) {
    gen_evasions(list_root, ~(pos_info_entry->mob[1]));
  }
  else {
    gen_captures(list_root, Board->mp[White], Board->mp[Black]);
    for (i = 0; list_root[i].move != 0; i++);
    gen_quiet_moves(list_root + i, pos_info_entry->mob[1], -1, -1, -1);
    for (k = i; list_root[k].move != 0; k++) list_root[k].score = 0;
  }
  num_moves = 0;
  for (i = 0; list_root[i].move != 0; i++) {
    move_do(list_root[i].move, undo);
    evaluate(pos_info_entry);
    legal = !ILLEGAL_POSITION;
    move_undo(list_root[i].move, undo);
    if (legal) { list_root[num_moves].move = list_root[i].move; num_moves++; }
  }
  list_root[num_moves].move = 0;
}

int parse_move(char cmove[])
{ int i, move, from, to, piece, pos, findmove;
  int left, right, match, matches;
  char stroka[20];
  char c, pt = 0, promotion = 0, file = 0, rank = 0;
  char ptypes[14] = {0,0,'P','P','N','N','B','B','R','R','Q','Q','K','K'};

  gen_legal_moves();
  if (strstr(cmove,"O-O-O") || strstr(cmove,"0-0-0") || strstr(cmove,"000")) {
    for (i = 0; (move = list_root[i].move) != 0; i++) {
      from = MOVE_FROM(move);
      to   = MOVE_TO(move);
      piece = Board->square[from];
      if ((piece & 0x0E) == 12 && (from - to) == 2) return move;
    }
    return 0;
  }
  if (strstr(cmove,"O-O") || strstr(cmove,"0-0") || strstr(cmove,"00")) {
    for (i = 0; (move = list_root[i].move) != 0; i++) {
      from = MOVE_FROM(move);
      to   = MOVE_TO(move);
      piece = Board->square[from];
      if ((piece & 0x0E) == 12 && (to - from) == 2) return move;
    }
    return 0;
  }
  // удаляем символы 'x' '=' '+' '#' и переводим в верхний регистр
  memset(stroka, 0, 20); pos = 0;
  for (i = 0; (c = cmove[i]) != 0; i++) if (!strchr("x=+#", c)) 
    stroka[pos++] = strchr("qrn", c) ? toupper(c) : c;
  left = 0; right = strlen(stroka) - 1;
  if (strstr(stroka, "/ep")) right -= 3;
  // смотрим превращение пешки
  if (strchr("QRBN", stroka[right])) promotion = stroka[right--];
  // тип ходящей фигуры - символ 'K' 'Q' 'R' 'B' 'N' 'P'
  if (left < right) {
    if (strchr("KQRBN", stroka[left])) pt = stroka[left++];
    else pt = 'P';   // если не указана фигура, то это пешка
  }
  // определяем позицию to
  if (left < right) {
    if (stroka[right] < '1' || stroka[right] > '8' ||
        stroka[right - 1] < 'a' || stroka[right - 1] > 'h') return 0;
    to = ((stroka[right] - '1') * 8) + (stroka[right - 1] - 'a');
    right -= 2;
  }
  else return 0;
  // определяем file, rank
  if (left <= right) {
    if (strchr("abcdefgh", stroka[left])) file = stroka[left++];
    if (strchr("12345678", stroka[left])) rank = stroka[left];
  }
  matches = 0;
  findmove = 0;
  for (i = 0; (move = list_root[i].move) != 0; i++) {
    from = MOVE_FROM(move);
    piece = ptypes[Board->square[from]];
    match = 1;
    if (piece != pt) match = 0;
    else if (MOVE_TO(move) != to) match = 0;
    else if (MOVE_IS_PROMOTION(move) && ptypes[PIECE_PROMO(move,Board->turn)] != promotion) match = 0;
    else if (file != 0 && (file - 'a') != (from & 7)) match = 0;
    else if (rank != 0 && (rank - '1') != (from >> 3)) match = 0;
    if (match) { findmove = move; matches++; }
  }
  if (matches == 1) return findmove;
  return 0;
}

struct pgn_t {
  char WhitePlayer[100];   // имя игрока, играющего белыми
  char BlackPlayer[100];   // имя игрока, играющего черными
  char Winner;             // кто выиграл: 0 - ничья, 1 - выигрыш белых, 2 - черных
  int  PlyCount;           // количество ходов
  int  Moves[1000];
  struct {
    int pv[100];
    int value;
    int depth;
    int time;
  } comment[1000];
} Pgn[1];

void parse_comment(char stroka[], struct pgn_t * pgn, int n)
{ char *ptr = stroka;
  char cmove[20];
  int i, move, num_moves = 0;
  struct undo_t undo[100];
  int znak, mate;

  for (i = 0; i < 100; i++) pgn->comment[n].pv[i] = 0;
  pgn->comment[n].value = 0;
  pgn->comment[n].depth = 0;
  pgn->comment[n].time = 0;
  i = 0;
  if (*ptr == '(') {
    while (num_moves < 100) {
      ptr++;
      if (*ptr == ' ' || *ptr == ')') {
        cmove[i] = 0;
        move = parse_move(cmove);
        if (!move) break;
        move_do(move, undo+num_moves);
        pgn->comment[n].pv[num_moves++] = move;
        if (*ptr == ')') break;
        i = 0;
      }
      else cmove[i++] = *ptr;
    }
    while (*ptr++ != ')');
    ptr++;
  }
  for (i = num_moves - 1; i >= 0; i--) move_undo(pgn->comment[n].pv[i], undo+i);
  memset(cmove, 0, 20);
  i = 0; znak = 0; mate = 0;
  while (*ptr != 0 && *ptr != '\n' && *ptr != '/') {
    if (*ptr == '+') znak = 0;
    else if (*ptr == '-') znak = 1;
    else if (*ptr == 'M') mate = 1;
    else if (*ptr != '.') cmove[i++] = *ptr;
    ptr++;
  }
  if (cmove[0] != 0) {
    if (mate) { if (znak) i = -32000; else i = 32000; }
    else {
      i = atol(cmove);
      if (znak) i = -i;
    }
    pgn->comment[n].value = i;
  }
  if (*ptr == '/') ptr++;
  memset(cmove, 0, 20);
  i = 0;
  while (*ptr != 0 && *ptr != '\n' && *ptr != ' ') cmove[i++] = *ptr++;
  if (cmove[0] != 0) pgn->comment[n].depth = atol(cmove);
  while (*ptr == ' ') ptr++;
  memset(cmove, 0, 20);
  i = 0;
  while (*ptr != 0 && *ptr != '\n' && *ptr != ' ') cmove[i++] = *ptr++;
  if (cmove[0] != 0) pgn->comment[n].time = atol(cmove);
}

int get_pgn_game(FILE * finp, struct pgn_t * Pgn)
{ char c, cc;
  char stroka[1000];
  char * ptr;
  int i, j, in_comment, in_move, comment_pos, move_pos;
  char cmove[20];
  int move;
  struct undo_t undo[1];

  board_from_fen(start_pos);
  Pgn->WhitePlayer[0] = 0;
  Pgn->BlackPlayer[0] = 0;
  Pgn->Winner = 0;
  Pgn->PlyCount = 0;
  do {
    fscanf(finp, "%c", &c);
    if (c != '1' && c != '\n' && !feof(finp)) {
      fgets(stroka, 1000, finp);
      if (!stroka) continue;
      for (i = 0; stroka[i] != 0 && stroka[i] != ' '; i++) stroka[i] = toupper(stroka[i]);
      if (strstr(stroka, "WHITE ")) {
        ptr = strchr(stroka, '"');
        strcpy(Pgn->WhitePlayer, ptr + 1);
        ptr = strchr(Pgn->WhitePlayer, '"');
        if (ptr) *ptr = 0;
      }
      if (strstr(stroka, "BLACK ")) {
        ptr = strchr(stroka, '"');
        strcpy(Pgn->BlackPlayer, ptr + 1);
        ptr = strchr(Pgn->BlackPlayer, '"');
        if (ptr) *ptr = 0;
      }
      if (strstr(stroka, "RESULT")) {
        ptr = strchr(stroka, '"');
        if (ptr[1] == '0') Pgn->Winner = 2;
        else if (ptr[1] == '1' && ptr[2] == '-') Pgn->Winner = 1;
      }
      if (strstr(stroka, "PLYCOUNT") != NULL) {
        ptr = strchr(stroka, '"');
        Pgn->PlyCount = strtol(ptr + 1, NULL, 10);
      }
    }
    if (c == '1') fscanf(finp, "%c", &cc);
  } while ((c != '1' || cc != '.') && !feof(finp));
  if (feof(finp)) return 0;
  for (j = 0; j < 100; j++) Pgn->comment[0].pv[j] = 0;
  Pgn->comment[0].value = 0;
  Pgn->comment[0].depth = 0;
  Pgn->comment[0].time = 0;
  i = 0; in_comment = 0; in_move = 0;
  do {
    fscanf(finp, "%c", &c);
    if (c == '[') break;
    if (c == '{') {
      in_comment = 1;
      comment_pos = 0;
      i--;
      memset(stroka, 0, 1000);
      continue;
    }
    if (c == '}') {
      if (Pgn->comment[i].pv[0] == 0) {
        move_undo(Pgn->Moves[i], undo);
        parse_comment(stroka, Pgn, i);
        move_do(Pgn->Moves[i], undo);
      }
      in_comment = 0;
      i++;
      memset(stroka, 0, 1000);
      continue;
    }
    if (c == '\n') c = ' ';
    if (!in_comment) {
      if (!in_move && isalpha(c)) {
        in_move = 1;
        move_pos = 1;
        memset(cmove, 0, 20);
        cmove[0] = c;
        continue;
      }
      if (in_move && c == ' ') {
        move = parse_move(cmove);
        if (!move) { printf("Bad move: %s\n", cmove); return 0; }
        Pgn->Moves[i] = move;
        move_do(move, undo);
        in_move = 0;
        i++;
        for (j = 0; j < 100; j++) Pgn->comment[i].pv[j] = 0;
        Pgn->comment[i].value = 0;
        Pgn->comment[i].depth = 0;
        Pgn->comment[i].time = 0;
        memset(stroka, 0, 1000);
        continue;
      }
    }
    if (in_comment) stroka[comment_pos++] = c;
    else if (in_move) cmove[move_pos++] = c;
  } while (!feof(finp));
  return 1;
}

void board_to_fen(char fen[])
{ int i, j, e, piece;
  char *fig[14] = {" "," ","P","p","N","n","B","b","R","r","Q","q","K","k"};
  char *pust[9] = {" ","1","2","3","4","5","6","7","8"};
  char *file[8] = {"a","b","c","d","e","f","g","h"};
  char *rank[8] = {"1","2","3","4","5","6","7","8"};

  fen[0] = 0;
  for (i = 56; i >= 0; i -= 8) {
    e = 0;
    for (j = 0; j <= 7; j++) {
      piece = Board->square[i + j];
      if (piece != 0) {
        if (e > 0) {
          strcat(fen,pust[e]);
          e = 0;
        }
        strcat(fen,fig[piece]);
      }
      else e++;
    }
    if (e > 0) strcat(fen,pust[e]);
    if (i > 0) strcat(fen,"/");
  }
  if (Board->turn) strcat(fen," b ");
  else strcat(fen," w ");
  if (Board->flags == 0) strcat(fen,"-");
  else {
    if (Board->flags & 1) strcat(fen,"K");
    if (Board->flags & 2) strcat(fen,"Q");
    if (Board->flags & 4) strcat(fen,"k");
    if (Board->flags & 8) strcat(fen,"q");
  }
  strcat(fen," ");
  if (Board->ep_square == 0) strcat(fen,"-");
  else {
    strcat(fen,file[Board->ep_square & 7]);
    strcat(fen,rank[Board->ep_square >> 3]);
  }
}

void run_pgn_test(char string[])
{ char * pgn_file;
  FILE * finp;
  FILE * ft;
  FILE * fepd;
  int i, j, move;
  char cmove[10];
  int side, opening;
  char position[65536];

  pgn_file = strchr(string,' ');
  if (pgn_file == NULL) return;
  pgn_file++;
  if (strlen(pgn_file) == 0) return;
  if ((finp = fopen(pgn_file, "r")) == NULL) {
    fprintf(stdout,"Can't Open File %s\n", pgn_file);
    return;
  }

  ft = fopen("test.txt","w");
  fepd = fopen("test.epd","w");
  while (!feof(finp)) {
    if (!get_pgn_game(finp, Pgn)) break;
    if (Pgn->PlyCount != 0) {
      fprintf(fepd,"ucinewgame\n");
      side = 0;
      if (!memcmp(Pgn->BlackPlayer,"Strelka",7)) side = 1;
      fprintf(ft,"White  : %s\nBlack  : %s\n", Pgn->WhitePlayer, Pgn->BlackPlayer);
      if (Pgn->Winner == 0) fprintf(ft,"Winner : Draw");
      else if (Pgn->Winner == 1) fprintf(ft,"Winner : White");
      else fprintf(ft,"Winner : Black");
      fprintf(ft,", PlyCount : %d\n\n",Pgn->PlyCount);
      for (i = 0; i < Pgn->PlyCount; i++) {
        opening = 0;
        if (Pgn->comment[i].pv[0] == 0 && Pgn->comment[i].depth == 0) opening = 1;
        if (!opening && (i & 1) == side && Pgn->comment[i].value > -200) {
          // не дебют, ход Стрелки и еще не проиграно - сохраняем позицию в EPD
          strcpy(position,"position startpos moves");
          for (j = 0; j < i; j++) {
            move_to_string(Pgn->Moves[j], cmove);
            strcat(position," ");
            strcat(position, cmove);
          }
          parse_position(position);
          // преобразуем позицию в FEN
          board_to_fen(position);
          // добавляем лучший ход, предложенный ранее соперником или наш текущий
          if (i > 0 && Pgn->comment[i-1].pv[1] != 0) move = Pgn->comment[i-1].pv[1];
          else move = Pgn->Moves[i];
          move_to_string(move, cmove);
          fprintf(fepd,"%s bm %s",position,cmove);
          if (i < Pgn->PlyCount - 2) {
            if (Pgn->comment[i].value > Pgn->comment[i+2].value + 20) {
              fprintf(fepd," ; key");
            }
          }
          fprintf(fepd,"\n");
        }
        if ((i & 1) == 0) fprintf(ft,"%3d. ",(i / 2) + 1);
        move_to_string(Pgn->Moves[i], cmove);
        if (cmove[4] == 0) { cmove[4] = ' '; cmove[5] = 0; }
        fprintf(ft,"%s ", cmove);
        if (Pgn->comment[i].pv[0] == 0 && Pgn->comment[i].depth == 0) {
          fprintf(ft,"opening                           ");
        }
        else {
          fprintf(ft,"( ");
          for (j = 0; j < 2; j++) {
            if (Pgn->comment[i].pv[j] != 0) {
              move_to_string(Pgn->comment[i].pv[j], cmove);
              if (cmove[4] == 0) { cmove[4] = ' '; cmove[5] = 0; }
              fprintf(ft,"%s", cmove);
            }
            else fprintf(ft,"     ");
            if (j == 0) fprintf(ft," ");
          }
          fprintf(ft,")");
          fprintf(ft," %6d/%2d %3d      ",
            Pgn->comment[i].value,Pgn->comment[i].depth,Pgn->comment[i].time);
        }
        if ((i & 1) || (i + 1) == Pgn->PlyCount) fprintf(ft,"\n");
      }
      fprintf(ft,"%c\n",12);
    }
  }
  fclose(ft);
  fclose(fepd);

  fclose(finp);
}

#endif

void parse_position(char string[])
{ const char * fen;
  char * moves;
  const char * ptr;
  char move_string[256];
  int move;
  struct undo_t undo[1];

  fen = strstr(string,"fen ");
  moves = strstr(string,"moves ");
  if (fen != NULL) {
    if (moves != NULL) moves[-1] = 0;   // Das ist UCI
    board_from_fen(fen+4);
  }
  else board_from_fen(start_pos);
  if (moves != NULL) {
    ptr = moves+6;
    while (*ptr != 0) {
      move_string[0] = *ptr++;
      move_string[1] = *ptr++;
      move_string[2] = *ptr++;
      move_string[3] = *ptr++;
      if (*ptr == 0 || *ptr == ' ') move_string[4] = 0;
      else { move_string[4] = *ptr++; move_string[5] = 0; }
      move = move_from_string(move_string);
      move_do(move, undo);
      while (*ptr == ' ') ptr++;
    }
  }
}

void parse_setoption(char string[])
{ char *name, *value;
  int size;

  name = strstr(string,"name ");
  value = strstr(string,"value ");
  if (name == NULL || value == NULL || name >= value) return;
  value[-1] = 0;
  name += 5;
  value += 6;
  if (!memcmp(name,"Hash",4)) {
    size = atoi(value);
    if (size >= 4 && size <= 1024) {
      free(trans_entry);
      trans_alloc(size);
    }
  }
  if (!memcmp(name,"MultiPV",7)) {
    size = atoi(value);
    if (size >= 1 && size <= 100) {
      MultiPV = size;
    }
  }
}

void get_uci_command()
{ char string[65536];
  char *ptr;

  (void)fgets(string, 65536, stdin);
  if (feof(stdin)) exit(0);
  ptr = strchr(string, '\n');
  if (ptr != NULL) *ptr = 0;
  // Parse
  if (!strcmp(string, "uci")) {
    fprintf(stdout,"id name Strelka 2.0 B\n");
    fprintf(stdout,"id author Jury Osipov\n");
    fprintf(stdout,"option name Hash type spin default 32 min 4 max 1024\n");
    fprintf(stdout,"option name MultiPV type spin default 1 min 1 max 100\n");
    fprintf(stdout,"uciok\n");
  }
  else if (!strcmp(string,"ucinewgame")) {
    if (!Searching && !Delay) trans_clear();
  }
  else if (!strcmp(string,"isready")) {
    fprintf(stdout,"readyok\n");
  }
  else if (!strcmp(string,"ponderhit")) {
    if (Searching) Infinite = 0;
    else if (Delay) { send_best_move(); Delay = 0; }
  }
  else if (!memcmp(string,"position",8)) {
    if (!Searching && !Delay) parse_position(string);
  }
  else if (!memcmp(string,"setoption",9)) {
    if (!Searching && !Delay) parse_setoption(string);
  }
  else if (!memcmp(string,"go",2)) {
    if (!Searching && !Delay) start_go(string);
  }
  else if (!strcmp(string,"stop")) {
    if (Searching) { stop_search = 1; Infinite = 0; }
    else if (Delay) { send_best_move(); Delay = 0; }
  }
  else if (!strcmp(string,"quit")) exit(0);
#ifdef TEST_VER
  else if (!memcmp(string,"epd",3)) run_epd_test(string);
  else if (!memcmp(string,"pgn",3)) run_pgn_test(string);
#endif
}
