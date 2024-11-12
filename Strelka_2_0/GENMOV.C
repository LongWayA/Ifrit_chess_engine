//////////////////////////////////////////////////////////////////////////
// Генерация ходов в случае шаха
struct list_t * gen_evasions(struct list_t * list, unsigned __int64 mask)
{ unsigned __int64 atk_bm, mask_em;
  int king, square, piece, atk_square;

  if (Board->turn == White) {           // ход белых
    king = first_one(Board->mp[WhiteKing]);    // позиция белого короля
    // формируем битовую строку позиций фигур, атакующих короля
    atk_bm = ((LINE3(king) | LINE4(king)) & ((Board->mp[BlackRook])   | (Board->mp[BlackQueen]))) |   // ладьи и ферзи
             ((LINE1(king) | LINE2(king)) & ((Board->mp[BlackBishop]) | (Board->mp[BlackQueen]))) |   // слоны и ферзи
             (MaskKnightMoves[king] & (Board->mp[BlackKnight])) |                                     // кони
             (MaskPawnAttacks[0][king] & (Board->mp[BlackPawn]));                                     // пешки
    atk_square = first_one(atk_bm);       // позиция первой атакующей фигуры
    piece = Board->square[atk_square];    // тип атакующей фигуры
    // создаем битовую маску позиций, безопасных для отхода короля;
    // исходная маска mask - все позиции, допустимые для ходов белых.
    mask = mask & (MaskPieceIsPawn[piece] | MaskEvasionMoves[king][atk_square][1]) &
           MaskKingMoves[king] & (~(Board->mp[White]));
    atk_bm &= (atk_bm - 1);   // удаляем из битовой строки атак первую атакующую фигуру
    if (atk_bm != 0) {        // остались атаки - это кратный шах; можно только уйти королем
      square = first_one(atk_bm);         // это позиция второй атакующей фигуры
      piece = Board->square[square];
      // корректируем маску безопасных позиций с учетом второй фигуры, и формируем отходы короля
      for (mask = mask & (MaskPieceIsPawn[piece] | MaskEvasionMoves[king][square][1]); mask != 0; mask &= (mask-1)) {
        square = first_one(mask);        // сюда король может безопасно уйти
        (list++)->move = (king << 6) | square;
      }
      list->move = list->score = 0;
      return (list - 1);
    }
    // теперь рассматриваем ситуацию одиночного шаха.
    // сначала смотрим безопасные отходы короля по маске ходов mask
    for (; mask != 0; mask &= (mask - 1)) {
      (list++)->move = (king << 6) | first_one(mask);
    }
    // смотрим взятия атакующей фигуры своей пешкой
    if (WhitePawnCaptures1[atk_square] & (Board->mp[WhitePawn])) {
      list->move = ((atk_square-9)<<6) | atk_square;
      if (atk_square >= 56) {
        (list++)->move |= FlagPromoQueen;
        (list++)->move = ((atk_square-9)<<6) | atk_square | FlagPromoRook;
        (list++)->move = ((atk_square-9)<<6) | atk_square | FlagPromoBishop;
        list->move     = ((atk_square-9)<<6) | atk_square | FlagPromoKnight;
      }
      list++;
    }
    if (WhitePawnCaptures2[atk_square] & (Board->mp[WhitePawn])) {
      list->move = ((atk_square - 7) << 6) | atk_square;
      if (atk_square >= 56) {
        (list++)->move |= FlagPromoQueen;
        (list++)->move = ((atk_square-7)<<6) | atk_square | FlagPromoRook;
        (list++)->move = ((atk_square-7)<<6) | atk_square | FlagPromoBishop;
        list->move     = ((atk_square-7)<<6) | atk_square | FlagPromoKnight;
      }
      list++;
    }
    // проверяем возможность взятия на проходе
    if (Board->ep_square != 0 && Board->ep_square == (atk_square + 8)) {
      if (WhitePawnCaptures1[atk_square + 8] & (Board->mp[WhitePawn])) {
        (list++)->move = ((atk_square - 1) << 6) | (atk_square + 8) | FlagEnpassant;
      }
      if (WhitePawnCaptures2[atk_square + 8] & (Board->mp[WhitePawn])) {
        (list++)->move = ((atk_square + 1) << 6) | (atk_square + 8) | FlagEnpassant;
      }
    }
    mask_em = MaskEvasionMoves[king][atk_square][0];
    // ходы пешки вперед на одно поле - пытаемся закрыть короля пешкой
    mask = (MaskClearSquare[atk_square] >> 8) & (mask_em >> 8) & (Board->mp[WhitePawn]);
    for (; mask != 0; mask &= (mask - 1)) {
      square = first_one(mask);
      list->move = (square<<6) | (square+8);
      if (square >= 48) {
        (list++)->move |= FlagPromoQueen;
        (list++)->move = (square<<6) | (square+8) | FlagPromoRook;
        (list++)->move = (square<<6) | (square+8) | FlagPromoBishop;
        list->move     = (square<<6) | (square+8) | FlagPromoKnight;
      }
      list++;
    }
    // проверяем ходы пешки на два поля вперед со второй горизонтали
    mask = (~((Board->md1) >> 8)) &
           (MaskClearSquare[atk_square] >> 16) &
           (mask_em >> 16) &
           (Board->mp[WhitePawn]);
    for (mask = mask & 0xFF00; mask != 0; mask &= (mask - 1)) {
      square = first_one(mask);
      (list++)->move = (square << 6) | (square + 16);
    }
    // защищающие ходы коня
    for (atk_bm = Board->mp[WhiteKnight]; atk_bm != 0; atk_bm &= (atk_bm - 1)) {
      square = first_one(atk_bm);
      for (mask = MaskKnightMoves[square] & mask_em; mask != 0; mask &= (mask - 1)) {
        (list++)->move = (square << 6) | first_one(mask);
      }
    }
    // защищающие ходы слона или ферзя - по диагоналям
    for (atk_bm = (Board->mp[WhiteBishop]) | (Board->mp[WhiteQueen]); atk_bm != 0; atk_bm &= (atk_bm - 1)) {
      square = first_one(atk_bm);
      for (mask = (LINE1(square) | LINE2(square)) & mask_em; mask != 0; mask &= (mask - 1)) {
        (list++)->move = (square << 6) | first_one(mask);
      }
    }
    // защищающие ходы ладьи или ферзя - по горизонталям, вертикалям
    for (atk_bm = (Board->mp[WhiteRook]) | (Board->mp[WhiteQueen]); atk_bm != 0; atk_bm &= (atk_bm - 1)) {
      square = first_one(atk_bm);
      for (mask = (LINE3(square) | LINE4(square)) & mask_em; mask != 0; mask &= (mask - 1)) {
        (list++)->move = (square << 6) | first_one(mask);
      }
    }
  }
  else {                // ход черных - все то же самое
    king = first_one(Board->mp[BlackKing]);
    atk_bm = ((LINE3(king) | LINE4(king)) & ((Board->mp[WhiteRook])   | (Board->mp[WhiteQueen]))) |
             ((LINE1(king) | LINE2(king)) & ((Board->mp[WhiteBishop]) | (Board->mp[WhiteQueen]))) |
             (MaskKnightMoves[king] & (Board->mp[WhiteKnight])) |
             (MaskPawnAttacks[1][king] & (Board->mp[WhitePawn]));
    atk_square = first_one(atk_bm);
    piece = Board->square[atk_square];
    mask = mask & (MaskPieceIsPawn[piece] | MaskEvasionMoves[king][atk_square][1]) &
           MaskKingMoves[king] & (~(Board->mp[Black]));
    atk_bm &= (atk_bm - 1);
    if (atk_bm != 0) {
      square = first_one(atk_bm);
      piece = Board->square[square];
      for (mask = mask & (MaskPieceIsPawn[piece] | MaskEvasionMoves[king][square][1]); mask != 0; mask &= (mask-1)) {
        square = first_one(mask);
        (list++)->move = (king << 6) | square;
      }
      list->move = list->score = 0;
      return (list - 1);
    }
    for (; mask != 0; mask &= (mask - 1)) {
      (list++)->move = (king << 6) | first_one(mask);
    }
    if (BlackPawnCaptures1[atk_square] & (Board->mp[BlackPawn])) {
      list->move = ((atk_square+7)<<6) | atk_square;
      if (atk_square <= 7) {
        (list++)->move |= FlagPromoQueen;
        (list++)->move = ((atk_square+7)<<6) | atk_square | FlagPromoRook;
        (list++)->move = ((atk_square+7)<<6) | atk_square | FlagPromoBishop;
        list->move     = ((atk_square+7)<<6) | atk_square | FlagPromoKnight;
      }
      list++;
    }
    if (BlackPawnCaptures2[atk_square] & (Board->mp[BlackPawn])) {
      list->move = ((atk_square+9)<<6) | atk_square;
      if (atk_square <= 7) {
        (list++)->move |= FlagPromoQueen;
        (list++)->move = ((atk_square+9)<<6) | atk_square | FlagPromoRook;
        (list++)->move = ((atk_square+9)<<6) | atk_square | FlagPromoBishop;
        list->move     = ((atk_square+9)<<6) | atk_square | FlagPromoKnight;
      }
      list++;
    }
    if (Board->ep_square != 0 && Board->ep_square == (atk_square - 8)) {
      if (BlackPawnCaptures1[atk_square - 8] & (Board->mp[BlackPawn])) {
        (list++)->move = ((atk_square - 1) << 6) | (atk_square - 8) | FlagEnpassant;
      }
      if (BlackPawnCaptures2[atk_square - 8] & (Board->mp[BlackPawn])) {
        (list++)->move = ((atk_square + 1) << 6) | (atk_square - 8) | FlagEnpassant;
      }
    }
    mask_em = MaskEvasionMoves[king][atk_square][0];
    mask = (MaskClearSquare[atk_square] << 8) & (mask_em << 8) & (Board->mp[BlackPawn]);
    for (; mask != 0; mask &= (mask - 1)) {
      square = first_one(mask);
      list->move = (square<<6) | (square-8);
      if (square <= 15) {
        (list++)->move |= FlagPromoQueen;
        (list++)->move = (square<<6) | (square-8) | FlagPromoRook;
        (list++)->move = (square<<6) | (square-8) | FlagPromoBishop;
        list->move     = (square<<6) | (square-8) | FlagPromoKnight;
      }
      list++;
    }
    mask = (((MaskClearSquare[atk_square] & mask_em) & 0xFF00000000) << 16) &
           (~((Board->md1) << 8)) &
           (Board->mp[BlackPawn]);
    for (; mask != 0; mask &= (mask - 1)) {
      square = first_one(mask);
      (list++)->move = (square << 6) | (square - 16);
    }
    for (atk_bm = Board->mp[BlackKnight]; atk_bm != 0; atk_bm &= (atk_bm - 1)) {
      square = first_one(atk_bm);
      for (mask = MaskKnightMoves[square] & mask_em; mask != 0; mask &= (mask - 1)) {
        (list++)->move = (square << 6) | first_one(mask);
      }
    }
    for (atk_bm = (Board->mp[BlackBishop]) | (Board->mp[BlackQueen]); atk_bm != 0; atk_bm &= (atk_bm - 1)) {
      square = first_one(atk_bm);
      for (mask = (LINE1(square) | LINE2(square)) & mask_em; mask != 0; mask &= (mask - 1)) {
        (list++)->move = (square << 6) | first_one(mask);
      }
    }
    for (atk_bm = (Board->mp[BlackRook]) | (Board->mp[BlackQueen]); atk_bm != 0; atk_bm &= (atk_bm - 1)) {
      square = first_one(atk_bm);
      for (mask = (LINE3(square) | LINE4(square)) & mask_em; mask != 0; mask &= (mask - 1)) {
        (list++)->move = (square << 6) | first_one(mask);
      }
    }
  }
  list->move = list->score = 0;
  return (list - 1);
}
//////////////////////////////////////////////////////////////////////////////////////////
// Генератор взятий и превращений пешки
void gen_captures(struct list_t * list, unsigned __int64 mask_w, unsigned __int64 mask_b)
{ unsigned __int64 mask_from, mask_to;
  int from, to;

  if (Board->turn == White) {      // в позиции ход белых
    // Здесь ищем взятия черных фигур белыми. Для этого на маски возможных ходов
    // накладываем маску позиций черных фигур mask_b
    // Ходы белого коня
    for (mask_from = Board->mp[WhiteKnight]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = MaskKnightMoves[from] & mask_b; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 12;  // оценка взятия = capture * 3 + 12;
      }
    }
    // Ходы белого слона
    for (mask_from = Board->mp[WhiteBishop]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = (LINE1(from) | LINE2(from)) & mask_b; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 11;
      }
    }
    // Ходы белой ладьи
    for (mask_from = Board->mp[WhiteRook]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = (LINE3(from) | LINE4(from)) & mask_b; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 10;
      }
    }
    // Ходы белого ферзя
    for (mask_from = Board->mp[WhiteQueen]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = (LINE1(from) | LINE2(from) | LINE3(from) | LINE4(from)) & mask_b; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 9;
      }
    }
    // Ходы белого короля
    from = first_one(Board->mp[WhiteKing]);
    for (mask_to = MaskKingMoves[from] & mask_b; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      list->move = (from << 6) | to;
      (list++)->score = (Board->square[to]) * 3 + 8;
    }
    // Превращения пешки - смотрим только предпоследнюю горизонталь
    for (mask_from = (Board->mp[WhitePawn]) & 0x00FF000000000000; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      to = from + 8;
      if ((Board->square[to]) == 0) {
        list->move = (from << 6) | to | FlagPromoQueen;
        (list++)->score = 15;
        list->move = (from << 6) | to | FlagPromoRook;
        (list++)->score = 14;
        list->move = (from << 6) | to | FlagPromoBishop;
        (list++)->score = 13;
        list->move = (from << 6) | to | FlagPromoKnight;
        (list++)->score = 12;
      }
      to = from + 7;
      if (from != 48 && (((unsigned __int64)1 << to) & Board->mp[Black]) != 0) {
        list->move = (from << 6) | to | FlagPromoQueen;
        (list++)->score = (Board->square[to]) * 3 + 13;
        list->move = (from << 6) | to | FlagPromoRook;
        (list++)->score = (Board->square[to]) * 3 + 12;
        list->move = (from << 6) | to | FlagPromoBishop;
        (list++)->score = (Board->square[to]) * 3 + 11;
        list->move = (from << 6) | to | FlagPromoKnight;
        (list++)->score = (Board->square[to]) * 3 + 10;
      }
      to = from + 9;
      if (from != 55 && (((unsigned __int64)1 << to) & Board->mp[Black]) != 0) {
        list->move = (from << 6) | to | FlagPromoQueen;
        (list++)->score = (Board->square[to]) * 3 + 13;
        list->move = (from << 6) | to | FlagPromoRook;
        (list++)->score = (Board->square[to]) * 3 + 12;
        list->move = (from << 6) | to | FlagPromoBishop;
        (list++)->score = (Board->square[to]) * 3 + 11;
        list->move = (from << 6) | to | FlagPromoKnight;
        (list++)->score = (Board->square[to]) * 3 + 10;
      }
    }
    // Простые взятия пешками - без превращения
    for (mask_to = (((Board->mp[WhitePawn]) & 0x0000FEFEFEFEFE00) << 7) & mask_b; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      from = to - 7;
      list->move = (from << 6) | to;
      (list++)->score = (Board->square[to]) * 3 + 13;
    }
    for (mask_to = (((Board->mp[WhitePawn]) & 0x00007F7F7F7F7F00) << 9) & mask_b; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      from = to - 9;
      list->move = (from << 6) | to;
      (list++)->score = (Board->square[to]) * 3 + 13;
    }
    // Взятие на проходе
    if ((to = Board->ep_square) != 0) {
      mask_to = (((Board->mp[WhitePawn]) & 0x00FEFEFEFEFEFEFE) << 7) & ((unsigned __int64)1 << to);
      if (mask_to != 0) {
        from = to - 7;
        list->move = (from << 6) | to | FlagEnpassant;
        (list++)->score = 22;
      }
      mask_to = (((Board->mp[WhitePawn]) & 0xFFFF7F7F7F7F7F7F) << 9) & ((unsigned __int64)1 << to);
      if (mask_to != 0) {
        from = to - 9;
        list->move = (from << 6) | to | FlagEnpassant;
        (list++)->score = 22;
      }
    }
  }
  else {                // Для позиции с ходом черных - то же самое
    for (mask_from = Board->mp[BlackKnight]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = MaskKnightMoves[from] & mask_w; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 15;
      }
    }
    for (mask_from = Board->mp[BlackBishop]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = (LINE1(from) | LINE2(from)) & mask_w; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 14;
      }
    }
    for (mask_from = Board->mp[BlackRook]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = (LINE3(from) | LINE4(from)) & mask_w; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 13;
      }
    }
    for (mask_from = Board->mp[BlackQueen]; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      for (mask_to = (LINE1(from) | LINE2(from) | LINE3(from) | LINE4(from)) & mask_w; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = (Board->square[to]) * 3 + 12;
      }
    }
    from = first_one(Board->mp[BlackKing]);
    for (mask_to = MaskKingMoves[from] & mask_w; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      list->move = (from << 6) | to;
      (list++)->score = (Board->square[to]) * 3 + 11;
    }
    for (mask_from = (Board->mp[BlackPawn]) & 0x000000000000FF00; mask_from != 0; mask_from &= (mask_from-1)) {
      from = first_one(mask_from);
      to = from - 8;
      if ((Board->square[to]) == 0) {
        list->move = (from << 6) | to | FlagPromoQueen;
        (list++)->score = 15;
        list->move = (from << 6) | to | FlagPromoRook;
        (list++)->score = 14;
        list->move = (from << 6) | to | FlagPromoBishop;
        (list++)->score = 13;
        list->move = (from << 6) | to | FlagPromoKnight;
        (list++)->score = 12;
      }
      to = from - 9;
      if (from != 8 && (((unsigned __int64)1 << to) & Board->mp[White]) != 0) {
        list->move = (from << 6) | to | FlagPromoQueen;
        (list++)->score = (Board->square[to]) * 3 + 16;
        list->move = (from << 6) | to | FlagPromoRook;
        (list++)->score = (Board->square[to]) * 3 + 15;
        list->move = (from << 6) | to | FlagPromoBishop;
        (list++)->score = (Board->square[to]) * 3 + 14;
        list->move = (from << 6) | to | FlagPromoKnight;
        (list++)->score = (Board->square[to]) * 3 + 13;
      }
      to = from - 7;
      if (from != 15 && (((unsigned __int64)1 << to) & Board->mp[White]) != 0) {
        list->move = (from << 6) | to | FlagPromoQueen;
        (list++)->score = (Board->square[to]) * 3 + 16;
        list->move = (from << 6) | to | FlagPromoRook;
        (list++)->score = (Board->square[to]) * 3 + 15;
        list->move = (from << 6) | to | FlagPromoBishop;
        (list++)->score = (Board->square[to]) * 3 + 14;
        list->move = (from << 6) | to | FlagPromoKnight;
        (list++)->score = (Board->square[to]) * 3 + 13;
      }
    }
    for (mask_to = (((Board->mp[BlackPawn]) >> 9) & mask_w) & 0x00007F7F7F7F7F00; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      from = to + 9;
      list->move = (from << 6) | to;
      (list++)->score = (Board->square[to]) * 3 + 16;
    }
    for (mask_to = (((Board->mp[BlackPawn]) >> 7) & mask_w) & 0x0000FEFEFEFEFE00; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      from = to + 7;
      list->move = (from << 6) | to;
      (list++)->score = (Board->square[to]) * 3 + 16;
    }
    if ((to = Board->ep_square) != 0) {
      mask_to = (((Board->mp[BlackPawn]) >> 9) & 0x007F7F7F7F7F7F7F) & ((unsigned __int64)1 << to);
      if (mask_to != 0) {
        from = to + 9;
        list->move = (from << 6) | to | FlagEnpassant;
        (list++)->score = 22;
      }
      mask_to = (((Board->mp[BlackPawn]) >> 7) & 0x00FEFEFEFEFEFEFE) & ((unsigned __int64)1 << to);
      if (mask_to != 0) {
        from = to + 7;
        list->move = (from << 6) | to | FlagEnpassant;
        (list++)->score = 22;
      }
    }
  }
  list->move = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Генерация тихих ходов для функций типа full_search
void gen_quiet_moves(struct list_t * list, unsigned __int64 mask, int trans_move, int killer1, int killer2)
{ int move, score, from, to;
  unsigned __int64 bme, mask_to;
  struct list_t * list_head, * pi, * pj;

  list_head = list;       // запоминаем начало списка
  bme = ~(Board->md1);    // маска пустых полей - для генерации ходов без взятий
  if (Board->turn == White) {
    // рокировки
    if (((Board->flags) & 1) && (((Board->md1) | mask) & 0x60) == 0) {
      list->move = 0x106 | FlagCastle;
      (list++)->score = History[WhiteKing-2][6];    // оценку тихих ходов берем из History
    }
    if (((Board->flags) & 2) && ((Board->md1) & 0x0E) == 0 && (mask & 0x0C) == 0) {
      list->move = 0x102 | FlagCastle;
      (list++)->score = History[WhiteKing-2][2];
    }
    // ходы пешек вперед
    for (mask = (((Board->mp[WhitePawn]) & 0x0000FFFFFFFFFF00) << 8) & bme; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      if (((from & 0xFFFFFFF8) == 16) && Board->square[from + 8] == 0) {
        list->move = ((from - 8) << 6) | (from + 8);
        (list++)->score = History[WhitePawn-2][from + 8];
      }
      list->move = ((from - 8) << 6) | from;
      (list++)->score = History[WhitePawn-2][from];
    }
    // ходы коня
    for (mask = Board->mp[WhiteKnight]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      for (mask_to = MaskKnightMoves[from] & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[WhiteKnight-2][to];
      }
    }
    // ходы ферзя
    for (mask = Board->mp[WhiteQueen]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      for (mask_to = (LINE1(from) | LINE2(from) | LINE3(from) | LINE4(from)) & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[WhiteQueen-2][to];
      }
    }
    // ходы слона
    for (mask = Board->mp[WhiteBishop]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      for (mask_to = (LINE1(from) | LINE2(from)) & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[WhiteBishop-2][to];
      }
    }
    // ходы ладьи
    for (mask = Board->mp[WhiteRook]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      for (mask_to = (LINE3(from) | LINE4(from)) & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[WhiteRook-2][to];
      }
    }
    // ходы короля
    from = first_one(Board->mp[WhiteKing]);
    for (mask_to = MaskKingMoves[from] & bme; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      list->move = (from << 6) | to;
      (list++)->score = History[WhiteKing-2][to];
    }
  }
  else {              // ход черных - аналогично
    if (((Board->flags) & 4) && (((Board->md1) | mask) & 0x6000000000000000) == 0) {
      list->move = 0xF3E | FlagCastle;
      (list++)->score = History[BlackKing-2][62];
    }
    if (((Board->flags) & 8) && ((Board->md1) & 0x0E00000000000000) == 0 && (mask & 0x0C00000000000000) == 0) {
      list->move = 0xF3A | FlagCastle;
      (list++)->score = History[BlackKing-2][58];
    }
    for (mask = (((Board->mp[BlackPawn]) >> 8) & 0x0000FFFFFFFFFF00) & bme; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      if (((from & 0xFFFFFFF8) == 40) && Board->square[from - 8] == 0) {
        list->move = ((from + 8) << 6) | (from - 8);
        (list++)->score = History[BlackPawn-2][from-8];
      }
      list->move = ((from + 8) << 6) | from;
      (list++)->score = History[BlackPawn-2][from];
    }
    for (mask = Board->mp[BlackKnight]; mask != 0; mask &= (mask - 1)) {
      from = first_one(mask);
      for (mask_to = MaskKnightMoves[from] & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[BlackKnight-2][to];
      }
    }
    for (mask = Board->mp[BlackQueen]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      for (mask_to = (LINE1(from) | LINE2(from) | LINE3(from) | LINE4(from)) & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[BlackQueen-2][to];
      }
    }
    for (mask = Board->mp[BlackBishop]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      for (mask_to = (LINE1(from) | LINE2(from)) & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[BlackBishop-2][to];
      }
    }
    for (mask = Board->mp[BlackRook]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      for (mask_to = (LINE3(from) | LINE4(from)) & bme; mask_to != 0; mask_to &= (mask_to-1)) {
        to = first_one(mask_to);
        list->move = (from << 6) | to;
        (list++)->score = History[BlackRook-2][to];
      }
    }
    from = first_one(Board->mp[BlackKing]);
    for (mask_to = MaskKingMoves[from] & bme; mask_to != 0; mask_to &= (mask_to-1)) {
      to = first_one(mask_to);
      list->move = (from << 6) | to;
      (list++)->score = History[BlackKing-2][to];
    }
  }
  list->move = 0;
  list->score = 0;
  if (list == list_head) return;    // нет ходов
  // сортируем список по score и исключаем из него trans_move и killer
  for (pi = list - 1; pi >= list_head; pi--) {
    if (pi->move != trans_move && pi->move != killer1 && pi->move != killer2) break;
    pi->move = 0;
    pi->score = 0;
  }
  while (pi > list_head) {
    pi--;
    move = pi->move;
    if (move != trans_move && move != killer1 && move != killer2) {
      score = pi->score;
      for (pj = pi; score < (pj+1)->score; pj++) {
        pj->move  = (pj+1)->move;
        pj->score = (pj+1)->score;
      }
      pj->move  = move;
      pj->score = score;
    }
    else {
      list--;
      if (pi < list) memcpy(pi, pi + 1, (list - pi) * sizeof(struct list_t));
      list->move = 0;
      list->score = 0;
    }
  }
}
///////////////////////////////////////////////////////////////////////////////
// Генерация шахов для qu_search
void gen_checks(struct list_t * list, unsigned __int64 mask_w, unsigned __int64 mask_b)
{ unsigned __int64 mask, mask_to;
  int wking, bking, from, to, piece;

  if (Board->turn == White) {     // ход белых
    bking  = first_one(Board->mp[BlackKing]);    // позиция черного короля
    // шахи белым конем
    for (mask = Board->mp[WhiteKnight]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      mask_to = MaskKnightMoves[from] & MaskKnightMoves[bking] & ((Board->mp[White]) ^ mask_b);
      for (; mask_to != 0; mask_to &= (mask_to-1))
        (list++)->move = (from << 6) | first_one(mask_to);
    }
    // ходы дальнобойных фигур по диагоналям - слона и ферзя
    // они могут сами поставить шах, или открыть линию для шаха от другой фигуры
    for (mask = (Board->mp[WhiteBishop]) | (Board->mp[WhiteQueen]); mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      mask_to = (LINE1(from) & LINE1(bking) & mask_b) |
                (LINE2(from) & LINE2(bking) & mask_b);
      if (mask_to != 0) {
        to = first_one(mask_to);
        piece = Board->square[to];
        if (piece == BlackPawn) (list++)->move = (from << 6) | to;
        else if (piece == WhitePawn) {
          if (to <= 47 && Board->square[to + 8] == 0) {
            (list++)->move = (to << 6) | (to + 8);
            if ((to & 0xFFFFFFF8) == 8 && Board->square[to + 16] == 0)
              (list++)->move = (to << 6) | (to + 16);
          }
          if (((mask_to & 0xFFFF7F7F7F7F7F7F) << 9) & (Board->mp[Black]) & mask_b)
            (list++)->move = (to << 6) | (to + 9);
          if (((mask_to & 0x00FEFEFEFEFEFEFE) << 7) & (Board->mp[Black]) & mask_b)
            (list++)->move = (to << 6) | (to + 7);
        }
        else if (piece == WhiteKnight) {
          mask_to = MaskKnightMoves[to] & ((Board->mp[White]) ^ mask_b);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == WhiteRook) {
          mask_to = (LINE3(to) | LINE4(to)) & ((Board->mp[White]) ^ mask_b);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == WhiteKing) {
          mask_to = MaskKingMoves[to] & MaskDiags[bking] & ((Board->mp[White]) ^ mask_b);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
      }
      else {
        mask_to = (LINE1(from) & LINE2(bking) & mask_b) |
                  (LINE2(from) & LINE1(bking) & mask_b);
        if (Board->square[from] == WhiteQueen) {
          mask_to |= ((LINE3(from) | LINE4(from)) &
                     ((LINE2(bking) & mask_b) | (LINE1(bking) & mask_b))) |
                     ((LINE1(from) | LINE2(from)) &
                     ((LINE3(bking) & mask_b) | (LINE4(bking) & mask_b)));
        }
        mask_to &= (Board->mp[White]) ^ mask_b;
        for (; mask_to != 0; mask_to &= (mask_to-1))
          (list++)->move = (from << 6) | first_one(mask_to);
      }
    }
    // точно так же для вертикалей, горизонталей - ходы ладьи или ферзя
    for (mask = (Board->mp[WhiteRook]) | (Board->mp[WhiteQueen]); mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      mask_to = (LINE3(from) & LINE3(bking) & mask_b) |
                (LINE4(from) & LINE4(bking) & mask_b);
      if (mask_to != 0) {
        to = first_one(mask_to);
        piece = Board->square[to];
        if (piece == BlackPawn) (list++)->move = (from << 6) | to;
        else if (piece == WhitePawn) {
          if ((mask_to & LINE3(from)) && to <= 47 && Board->square[to + 8] == 0) {
            (list++)->move = (to << 6) | (to + 8);
            if ((to & 0xFFFFFFF8) == 8 && Board->square[to + 16] == 0)
              (list++)->move = (to << 6) | (to + 16);
          }
          if (((mask_to & 0xFFFF7F7F7F7F7F7F) << 9) & (Board->mp[Black]) & mask_b)
            (list++)->move = (to << 6) | (to + 9);
          if (((mask_to & 0x00FEFEFEFEFEFEFE) << 7) & (Board->mp[Black]) & mask_b)
            (list++)->move = (to << 6) | (to + 7);
        }
        else if (piece == WhiteKnight) {
          mask_to = MaskKnightMoves[to] & ((Board->mp[White]) ^ mask_b);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == WhiteBishop) {
          mask_to = (LINE1(to) | LINE2(to)) & ((Board->mp[White]) ^ mask_b);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == WhiteKing) {
          mask_to = MaskKingMoves[to] & MaskLines[bking] & ((Board->mp[White]) ^ mask_b);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
      }
      else {
        mask_to = ((LINE3(from) & LINE4(bking) & mask_b) |
                   (LINE4(from) & LINE3(bking) & mask_b)) &
                  ((Board->mp[White]) ^ mask_b);
        for (; mask_to != 0; mask_to &= (mask_to-1))
          (list++)->move = (from << 6) | first_one(mask_to);
      }
    }
    // ходы белой пешки
    mask = (((Board->mp[WhitePawn]) & 0x0000FEFEFEFEFEFE) << 15) & (Board->mp[BlackKing]);
    if (mask && Board->square[bking - 7] == 0) (list++)->move = ((bking - 15) << 6) | (bking - 7);
    mask = (((Board->mp[WhitePawn]) & 0xFFFFFF7F7F7F7F7F) << 17) & (Board->mp[BlackKing]);
    if (mask && Board->square[bking - 9] == 0) (list++)->move = ((bking - 17) << 6) | (bking - 9);
    mask = ((Board->mp[BlackKing]) >> 7) &
           (((Board->mp[WhitePawn]) & 0x00FCFCFCFCFCFCFC) << 7) &
           (Board->mp[Black]) & mask_b;
    if (mask) (list++)->move = ((bking - 14) << 6) | (bking - 7);
    mask = ((Board->mp[BlackKing]) >> 9) &
           (((Board->mp[WhitePawn]) & 0x00FEFEFEFEFEFEFE) << 7) &
           (Board->mp[Black]) & mask_b;
    if (mask) (list++)->move = ((bking - 16) << 6) | (bking - 9);
    mask = ((Board->mp[BlackKing]) >> 7) &
           (((Board->mp[WhitePawn]) & 0xFFFF7F7F7F7F7F7F) << 9) &
           (Board->mp[Black]) & mask_b;
    if (mask) (list++)->move = ((bking - 16) << 6) | (bking - 7);
    mask = ((Board->mp[BlackKing]) >> 9) &
           (((Board->mp[WhitePawn]) & 0x003F3F3F3F3F3F3F) << 9) &
           (Board->mp[Black]) & mask_b;
    if (mask) (list++)->move = ((bking - 18) << 6) | (bking - 9);
    mask = (((Board->mp[WhitePawn]) & 0x000000000000FE00) << 23) & (Board->mp[BlackKing]);
    if (mask && Board->square[bking - 7] == 0 && Board->square[bking - 15] == 0)
      (list++)->move = ((bking - 23) << 6) | (bking - 7);
    mask = (((Board->mp[WhitePawn]) & 0x0000000000007F00) << 25) & (Board->mp[BlackKing]);
    if (mask && Board->square[bking - 9] == 0 && Board->square[bking - 17] == 0)
      (list++)->move = ((bking - 25) << 6) | (bking - 9);
  }
  else {            // ход черных - аналогично
    wking  = first_one(Board->mp[WhiteKing]);
    for (mask = Board->mp[BlackKnight]; mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      mask_to = MaskKnightMoves[from] & MaskKnightMoves[wking] & ((Board->mp[Black]) ^ mask_w);
      for (; mask_to != 0; mask_to &= (mask_to-1))
        (list++)->move = (from << 6) | first_one(mask_to);
    }
    for (mask = (Board->mp[BlackBishop]) | (Board->mp[BlackQueen]); mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      mask_to = (LINE1(from) & LINE1(wking) & mask_w) |
                (LINE2(from) & LINE2(wking) & mask_w);
      if (mask_to != 0) {
        to = first_one(mask_to);
        piece = Board->square[to];
        if (piece == WhitePawn) (list++)->move = (from << 6) | to;
        else if (piece == BlackPawn) {
          if (to >= 16 && Board->square[to - 8] == 0) {
            (list++)->move = (to << 6) | (to - 8);
            if ((to & 0xFFFFFFF8) == 48 && Board->square[to - 16] == 0)
              (list++)->move = (to << 6) | (to - 16);
          }
          if (((mask_to >> 7) & (Board->mp[White]) & mask_w) & 0x00FEFEFEFEFEFEFE)
            (list++)->move = (to << 6) | (to - 7);
          if (((mask_to >> 9) & (Board->mp[White]) & mask_w) & 0x007F7F7F7F7F7F7F)
            (list++)->move = (to << 6) | (to - 9);
        }
        else if (piece == BlackKnight) {
          mask_to = MaskKnightMoves[to] & ((Board->mp[Black]) ^ mask_w);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == BlackRook) {
          mask_to = (LINE3(to) | LINE4(to)) & ((Board->mp[Black]) ^ mask_w);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == BlackKing) {
          mask_to = MaskKingMoves[to] & MaskDiags[wking] & ((Board->mp[Black]) ^ mask_w);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
      }
      else {
        mask_to = (LINE1(from) & LINE2(wking) & mask_w) |
                  (LINE2(from) & LINE1(wking) & mask_w);
        if (Board->square[from] == BlackQueen) {
          mask_to |= ((LINE3(from) | LINE4(from)) &
                     ((LINE2(wking) & mask_w) | (LINE1(wking) & mask_w))) |
                     ((LINE1(from) | LINE2(from)) &
                     ((LINE3(wking) & mask_w) | (LINE4(wking) & mask_w)));
        }
        mask_to &= (Board->mp[Black]) ^ mask_w;
        for (; mask_to != 0; mask_to &= (mask_to-1))
          (list++)->move = (from << 6) | first_one(mask_to);
      }
    }
    for (mask = (Board->mp[BlackRook]) | (Board->mp[BlackQueen]); mask != 0; mask &= (mask-1)) {
      from = first_one(mask);
      mask_to = (LINE3(from) & (LINE3(wking) & mask_w)) |
                (LINE4(from) & (LINE4(wking) & mask_w));
      if (mask_to != 0) {
        to = first_one(mask_to);
        piece = Board->square[to];
        if (piece == WhitePawn) (list++)->move = (from << 6) | to;
        else if (piece == BlackPawn) {
          if ((mask_to & LINE3(from)) && to >= 16 && Board->square[to - 8] == 0) {
            (list++)->move = (to << 6) | (to - 8);
            if ((to & 0xFFFFFFF8) == 48 && Board->square[to - 16] == 0)
              (list++)->move = (to << 6) | (to - 16);
          }
          if (((mask_to >> 7) & (Board->mp[White]) & mask_w) & 0x00FEFEFEFEFEFEFE)
            (list++)->move = (to << 6) | (to - 7);
          if (((mask_to >> 9) & (Board->mp[White]) & mask_w) & 0x007F7F7F7F7F7F7F)
            (list++)->move = (to << 6) | (to - 9);
        }
        else if (piece == BlackKnight) {
          mask_to = MaskKnightMoves[to] & ((Board->mp[Black]) ^ mask_w);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == BlackBishop) {
          mask_to = (LINE1(to) | LINE2(to)) & ((Board->mp[Black]) ^ mask_w);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
        else if (piece == BlackKing) {
          mask_to = MaskKingMoves[to] & MaskLines[wking] & ((Board->mp[Black]) ^ mask_w);
          for (; mask_to != 0; mask_to &= (mask_to-1))
            (list++)->move = (to << 6) | first_one(mask_to);
        }
      }
      else {
        mask_to = ((LINE3(from) & LINE4(wking) & mask_w) |
                   (LINE4(from) & LINE3(wking) & mask_w)) &
                  ((Board->mp[Black]) ^ mask_w);
        for (; mask_to != 0; mask_to &= (mask_to - 1))
          (list++)->move = (from << 6) | first_one(mask_to);
      }
    }
    mask = ((Board->mp[BlackPawn]) >> 17) & (Board->mp[WhiteKing]) & 0x00007F7F7F7F7F7F;
    if (mask && Board->square[wking + 9] == 0) (list++)->move = ((wking + 17) << 6) | (wking + 9);
    mask = ((Board->mp[BlackPawn]) >> 15) & (Board->mp[WhiteKing]) & 0x0000FEFEFEFEFEFE;
    if (mask && Board->square[wking + 7] == 0) (list++)->move = ((wking + 15) << 6) | (wking + 7);
    mask = (((Board->mp[WhiteKing]) & 0x00003F3F3F3F3F3F) << 9) &
           ((Board->mp[BlackPawn]) >> 9) &
           (Board->mp[White]) & mask_w;
    if (mask) (list++)->move = ((wking + 18) << 6) | (wking + 9);
    mask = (((Board->mp[WhiteKing]) & 0x0000FEFEFEFEFEFE) << 7) &
           ((Board->mp[BlackPawn]) >> 9) &
           (Board->mp[White]) & mask_w;
    if (mask) (list++)->move = ((wking + 16) << 6) | (wking + 7);
    mask = (((Board->mp[WhiteKing]) & 0x00007F7F7F7F7F7F) << 9) &
           ((Board->mp[BlackPawn]) >> 7) &
           (Board->mp[White]) & mask_w;
    if (mask) (list++)->move = ((wking + 16) << 6) | (wking + 9);
    mask = (((Board->mp[WhiteKing]) & 0x0000FCFCFCFCFCFC) << 7) &
           ((Board->mp[BlackPawn]) >> 7) &
           (Board->mp[White]) & mask_w;
    if (mask) (list++)->move = ((wking + 14) << 6) | (wking + 7);
    mask = (((Board->mp[BlackPawn]) >> 25) & 0x000000007F000000) & (Board->mp[WhiteKing]);
    if (mask && Board->square[wking + 9] == 0 && Board->square[wking + 17] == 0)
      (list++)->move = ((wking + 25) << 6) | (wking + 9);
    mask = (((Board->mp[BlackPawn]) >> 23) & 0x00000000FE000000) & (Board->mp[WhiteKing]);
    if (mask && Board->square[wking + 7] == 0 && Board->square[wking + 15] == 0)
      (list++)->move = ((wking + 23) << 6) | (wking + 7);
  }
  list->move = 0;
}
