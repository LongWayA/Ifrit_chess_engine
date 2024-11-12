///////////////////////////////////////////////////////////////////////////
static __inline int first_one(unsigned __int64 bm)
{ _asm { mov  eax, dword ptr bm[0]
         test eax, eax
         jz   f_hi
         bsf  eax, eax
         jmp  f_ret
f_hi:    bsf  eax, dword ptr bm[4]
         add  eax, 20h
f_ret:
  }
}
///////////////////////////////////////////////////////////////////////////
static __inline int last_one(unsigned __int64 bm)
{ _asm { mov  eax, dword ptr bm[4]
         test eax, eax
         jz   l_lo
         bsr  eax, eax
         add  eax, 20h
         jmp  l_ret
l_lo:    bsr  eax, dword ptr bm[0]
l_ret:
  }
}
///////////////////////////////////////////////////////////////////////////
// Вычисление статической оценки позиции
int evaluate(struct pos_info_t * pos_info)
{ int wking_square, bking_square, king_attack_nb, king_safety_value;
  int diff, summ, value, cnt, square, i, k, opening, endgame, phase;
  unsigned __int64 mask, mob, mob_w, mob_b;
  unsigned __int64 wking_moves, bking_moves;
  struct pawn_info_t * entry;
  unsigned __int16 flags;
  unsigned char files;

  diff = Board->mat_diff;     // материальная разница
  summ = (Board->mat_summ) & 0x3FFFF;       // материальная сумма
  flags = 0;
  value = diff * 3399;
  if (summ < MAX_MATERIAL) {
    value += Material[summ].value;
    flags = Material[summ].flags;
  }
  // Здесь была ленивая оценка. В одной из Белок была отключена,
  // и вроде получилось лучше, но данных об этом нет.
  // Зато такое отключение очень упростило программу,
  // а на скорости сказалось не очень заметно.
  opening = value + Board->opening;  // материал + оценки позиций фигур
  endgame = value + Board->endgame;
  wking_square = first_one(Board->mp[WhiteKing]);
  wking_moves = MaskKingMoves[wking_square];
  bking_square = first_one(Board->mp[BlackKing]);
  bking_moves = MaskKingMoves[bking_square];
  king_attack_nb = 0;      // здесь будем накапливать оценку безопасности короля
  king_safety_value = 0;
  // Вычисляем по фигурам - их подвижность и атаки на чужого короля
  // Белые пешки
  mob_w = Board->mp[WhitePawn];
  mob_w = (((mob_w & 0xFFFF7F7F7F7F7F7F) << 2) | (mob_w & 0x00FEFEFEFEFEFEFE)) << 7;
  if (mob_w & bking_moves) king_attack_nb++;
  // Белые кони
  for (mask = Board->mp[WhiteKnight]; mask != 0; mask &= (mask-1)) {  // Идем по списку коней
    mob = MaskKnightMoves[first_one(mask)];    // маска возможных ходов коня
    mob_w |= mob;         // добавляем к общей маске мобильности белых фигур
    if (mob & bking_moves) {    // есть атака окрестности чужого короля
      king_attack_nb++; king_safety_value += 941;
    }
    mob &= ~(Board->mp[White]);   // из маски подвижности убираем позиции своих фигур
    // и определяем количество полей, доступных для хода коня
    cnt = popcnt(mob);
    // оценку подвижности добавляем к общей позиционной оценке
    endgame += cnt * 121;
    opening += cnt * 14;
  }
  // Белые слоны - аналогично
  for (mask = Board->mp[WhiteBishop]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob = LINE1(square) | LINE2(square);
    mob_w |= mob;
    if (mob & bking_moves) { king_attack_nb++; king_safety_value += 418; }
    mob &= ~(Board->mp[White]);
    cnt = popcnt(mob);
    endgame += cnt * 116;
    opening += cnt * 149;
  }
  // Белые ладьи
  for (mask = Board->mp[WhiteRook]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob = LINE3(square) | LINE4(square);
    mob_w |= mob;
    if (mob & bking_moves) { king_attack_nb++; king_safety_value += 666; }
    mob &= ~(Board->mp[White]);
    cnt = popcnt(mob);
    endgame += cnt * 79;
    opening += cnt * 84;
    mob = MaskPawnOpenFileW[square];
    if ((Board->mp[WhitePawn] & mob) == 0) {    // Открытая вертикаль - нет своих пешек
      endgame += 256;
      opening += 64;
      if ((Board->mp[BlackPawn] & mob) == 0) {   // На вертикали нет и чужих пешек
        endgame += 172; opening += 971;
      }
      // атака на чужого короля по открытой линии
      if ((flags & 4) && (bking_moves & mob)) {
        opening += 121;
        if (Board->mp[BlackKing] & mob) opening += 853;
      }
    }
    if ((square & 0xFFFFFFF8) == 0x30) {    // Предпоследняя горизонталь
      // если есть чужие пешки на предпоследней горизонтали или король на последней
      if ((Board->mp[BlackPawn] & 0x00FF000000000000) || (Board->mp[BlackKing] & 0xFF00000000000000)) {
        endgame += 1026; opening += 246;
      }
    }
  }
  // Белый ферзь
  for (mask = Board->mp[WhiteQueen]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob = LINE1(square) | LINE2(square) | LINE3(square) | LINE4(square);
    mob_w |= mob;
    if (mob & bking_moves) { king_attack_nb++; king_safety_value += 532; }
    mob &= ~(Board->mp[White]);
    cnt = popcnt(mob);
    endgame += cnt * 37;
    opening += cnt * 54;
    if ((square & 0xFFFFFFF8) == 0x30) {    // Предпоследняя горизонталь
      if ((Board->mp[BlackPawn] & 0x00FF000000000000) || (Board->mp[BlackKing] & 0xFF00000000000000)) {
        endgame += 1420;
      }
    }
  }
  mob_w |= wking_moves;     // Добавляем к оценке подвижность белого короля
  // Определяем оценку пешечной структуры
  entry = pawn_get_info();
  opening += entry->opening;
  endgame += entry->endgame;
  if (flags & 4) {    // Особый тип позиции (определенный суммарный материал)
    // Только для таких особых позиций определяем безопасность короля
    opening += (KingAttackWeight[king_attack_nb] * king_safety_value) / 32;
    i = FileWing[bking_square];
    k = entry->sheet_black_king[i];
    if (((Board->flags) & 4) && (entry->sheet_black_king[2] < k)) k = entry->sheet_black_king[2];
    if (((Board->flags) & 8) && (entry->sheet_black_king[0] < k)) k = entry->sheet_black_king[0];
    opening += (k + entry->sheet_black_king[i]) / 2;
  }
  // Все то же самое для черных фигур
  king_attack_nb = 0;
  king_safety_value = 0;
  mob_b = Board->mp[BlackPawn];
  mob_b = (((mob_b >> 2) & 0x3FBFBFBFBFBFBF80) | (mob_b & 0x7F7F7F7F7F7F7F00)) >> 7;
  if (mob_b & wking_moves) king_attack_nb++;
  for (mask = Board->mp[BlackKnight]; mask != 0; mask &= (mask-1)) {
    mob = MaskKnightMoves[first_one(mask)];
    mob_b |= mob;
    if (mob & wking_moves) { king_attack_nb++; king_safety_value += 941; }
    mob &= ~(Board->mp[Black]);
    cnt = popcnt(mob);
    endgame -= cnt * 121;
    opening -= cnt * 14;
  }
  for (mask = Board->mp[BlackBishop]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob = LINE1(square) | LINE2(square);
    mob_b |= mob;
    if (mob & wking_moves) { king_attack_nb++; king_safety_value += 418; }
    mob &= ~(Board->mp[Black]);
    cnt = popcnt(mob);
    endgame -= cnt * 116;
    opening -= cnt * 149;
  }
  for (mask = Board->mp[BlackRook]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob = LINE3(square) | LINE4(square);
    mob_b |= mob;
    if (mob & wking_moves) { king_attack_nb++; king_safety_value += 666; }
    mob &= ~(Board->mp[Black]);
    cnt = popcnt(mob);
    endgame -= cnt * 79;
    opening -= cnt * 84;
    mob = MaskPawnOpenFileB[square];
    if ((Board->mp[BlackPawn] & mob) == 0) {
      endgame -= 256;
      opening -= 64;
      if ((Board->mp[WhitePawn] & mob) == 0) { endgame -= 172; opening -= 971; }
      if ((flags & 2) && (wking_moves & mob)) {
        opening -= 121;
        if (Board->mp[WhiteKing] & mob) opening -= 853;
      }
    }
    if ((square & 0xFFFFFFF8) == 8) {
      if ((Board->mp[WhitePawn] & 0x000000000000FF00) || (Board->mp[WhiteKing] & 0x00000000000000FF)) {
        endgame -= 1026; opening -= 246;
      }
    }
  }
  for (mask = Board->mp[BlackQueen]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob = LINE1(square) | LINE2(square) | LINE3(square) | LINE4(square);
    mob_b |= mob;
    if (mob & wking_moves) { king_attack_nb++; king_safety_value += 532; }
    mob &= ~(Board->mp[Black]);
    cnt = popcnt(mob);
    endgame -= cnt * 37;
    opening -= cnt * 54;
    if ((square & 0xFFFFFFF8) == 8) {
      if ((Board->mp[WhitePawn] & 0x000000000000FF00) || (Board->mp[WhiteKing] & 0x00000000000000FF)) {
        endgame -= 1420;
      }
    }
  }
  mob_b |= bking_moves;
  if (flags & 2) {
    opening -= (KingAttackWeight[king_attack_nb] * king_safety_value) / 32;
    i = FileWing[wking_square];
    k = entry->sheet_white_king[i];
    if (((Board->flags) & 1) && entry->sheet_white_king[2] < k) k = entry->sheet_white_king[2];
    if (((Board->flags) & 2) && entry->sheet_white_king[0] < k) k = entry->sheet_white_king[0];
    opening -= (k + entry->sheet_white_king[i]) / 2;
  }
  // Отдельно считаем оценку проходных пешек - по соответствующим вертикалям
  for (files = entry->pass_file[0]; files != 0; files &= (files - 1)) {
    mask = Board->mp[WhitePawn] & MaskFile[BsfByte[files]];
    square = last_one(mask);   // если пешка не одна - берем самую продвинутую
    mob = MaskPawnOpenFileW[square];
    opening += PassedPawnValue1[square >> 3];  // оценка в зависимости от горизонтали
    endgame += PassedPawnValue2[square >> 3];
    // В случае отсутствия чужих фигур - у соперника только пешки
    if ((Board->mp[BlackQueen]  | Board->mp[BlackRook] |
         Board->mp[BlackBishop] | Board->mp[BlackKnight]) == 0) {
      if ((mob & Board->mp[White]) == 0 && (QuadBlackKing[square][Board->turn] & (Board->mp[BlackKing])) != 0) endgame += 25600;
      else if ((WhiteKingOpp[square] & Board->mp[WhiteKing]) != 0) endgame += 25600;
    }
    else {    // есть фигуры соперника
      if ((mob & Board->mp[White]) == 0) endgame += PassedPawnValue3[square >> 3];
      if ((mob & Board->mp[Black]) == 0) endgame += PassedPawnValue4[square >> 3];
      if (((~mob_w) & mob & mob_b) == 0) endgame += PassedPawnValue5[square >> 3];
    }
    endgame += Distance[square+8][bking_square] * PassedPawnValue7[square>>3] -
               Distance[square+8][wking_square] * PassedPawnValue6[square>>3];
  }
  // То же самое для черных проходных пешек
  for (files = entry->pass_file[1]; files != 0; files &= (files - 1)) {
    mask = Board->mp[BlackPawn] & MaskFile[BsfByte[files]];
    square = first_one(mask);
    mob = MaskPawnOpenFileB[square];
    opening -= PassedPawnValue1[7 - (square >> 3)];
    endgame -= PassedPawnValue2[7 - (square >> 3)];
    if ((Board->mp[WhiteQueen] | Board->mp[WhiteRook] |
         Board->mp[WhiteBishop] | Board->mp[WhiteKnight]) == 0) {
      if ((mob & Board->mp[Black]) == 0 && (QuadWhiteKing[square][Board->turn] & Board->mp[WhiteKing]) != 0) endgame -= 25600;
      else if ((BlackKingOpp[square] & Board->mp[BlackKing]) != 0) endgame -= 25600;
    }
    else {
      if ((mob & Board->mp[Black]) == 0) endgame -= PassedPawnValue3[7 - (square >> 3)];
      if ((mob & Board->mp[White]) == 0) endgame -= PassedPawnValue4[7 - (square >> 3)];
      if (((~mob_b) & mob & mob_w) == 0) endgame -= PassedPawnValue5[7 - (square >> 3)];
    }
    endgame += Distance[square-8][bking_square] * PassedPawnValue6[7-(square>>3)] -
               Distance[square-8][wking_square] * PassedPawnValue7[7-(square>>3)];
  }
  // Некоторые особые случаи паттернов
  if ((((Board->mp[WhiteBishop]) >> 7) & (Board->mp[BlackPawn]) & 0x0004020200000000) ||
      (((Board->mp[WhiteBishop]) >> 9) & (Board->mp[BlackPawn]) & 0x0020404000000000)) {
    opening -= 1802; endgame -= 1802;
  }
  if (((((Board->mp[BlackBishop]) & 0x0000000000010102) << 9) & Board->mp[WhitePawn]) ||
      ((((Board->mp[BlackBishop]) & 0x0000000000808040) << 7) & Board->mp[WhitePawn])) {
    opening += 1802; endgame += 1802;
  }
  if ((Board->mp[WhiteBishop]  & 0x0000000000000004) &&
      (Board->mp[WhitePawn]    & 0x0000000000000800) &&
      (Board->md1 & 0x0000000000080000)) opening -= 1920;
  if ((Board->mp[WhiteBishop]  & 0x0000000000000020) &&
      (Board->mp[WhitePawn]    & 0x0000000000001000) &&
      (Board->md1 & 0x0000000000100000)) opening -= 1920;
  if ((Board->mp[BlackBishop]  & 0x0400000000000000) &&
      (Board->mp[BlackPawn]    & 0x0008000000000000) &&
      (Board->md1 & 0x0000080000000000)) opening += 1920;
  if ((Board->mp[BlackBishop]  & 0x2000000000000000) &&
      (Board->mp[BlackPawn]    & 0x0010000000000000) &&
      (Board->md1 & 0x0000100000000000)) opening += 1920;
  if ((Board->mp[WhiteRook] & 0x0000000000000103) &&
      (Board->mp[WhiteKing] & 0x0000000000000006)) opening -= 1920;
  if ((Board->mp[WhiteRook] & 0x00000000000080C0) &&
      (Board->mp[WhiteKing] & 0x0000000000000060)) opening -= 1920;
  if ((Board->mp[BlackRook] & 0x0301000000000000) &&
      (Board->mp[BlackKing] & 0x0600000000000000)) opening += 1920;
  if ((Board->mp[BlackRook] & 0xC080000000000000) &&
      (Board->mp[BlackKing] & 0x6000000000000000)) opening += 1920;

  if (flags & 1) {    // слоновый эндшпиль
    // Смотрим - разноцветные ли слоны
    mask = (Board->mp[BlackBishop]) | (Board->mp[WhiteBishop]);
    if ((mask & 0x55AA55AA55AA55AA) && (mask & 0xAA55AA55AA55AA55)) {
      opening = opening / 2; endgame = endgame / 2;
    }
  }

  // Считаем интегральную оценку и заносим ее в структуру pos_info,
  // а также битовые маски подвижности фигур обоих цветов

  phase = (unsigned int)(flags) >> 4;   // стадия игры - число от 0 до 24

  // В оригинальной Стрелке здесь вместо phase были нелинейные коэффициенты:
  // value = ((MatPhase[phase][1] * endgame) + (MatPhase[phase][0] * opening)) >> 13;
  // В Белке 1.8.7 заменено на линейные коэффициенты, как у Фрукта (+30 пунктов !!!)

  value = ((phase * endgame + (24 - phase) * opening) / 24) / 32;

  if (Board->turn == 0) {
    pos_info->value = value + 3;
    pos_info->mob[0] = mob_w;
    pos_info->mob[1] = mob_b;
    mask = (Board->mp[WhiteKing]) & mob_b;   // маска атак чужих фигур на своего короля
  }
  else {
    pos_info->value = 3 - value;
    pos_info->mob[0] = mob_b;
    pos_info->mob[1] = mob_w;
    mask = (Board->mp[BlackKing]) & mob_w;
  }
  // Возвращаем признак шаха в позиции по наличию в маске подвижности атак на короля
  if (mask == 0) return 0;
            else return 1;
}
///////////////////////////////////////////////////////////////////////////
// Определение битовых масок подвижности фигур в случае ленивой оценки
int eval_mob(struct pos_info_t * pos_info)
{ unsigned __int64 mob_w, mob_b, mask;
  int square;

  mob_w = Board->mp[WhitePawn];
  mob_w = (((mob_w & 0xFFFF7F7F7F7F7F7F) << 2) | (mob_w & 0x00FEFEFEFEFEFEFE)) << 7;
  for (mask = Board->mp[WhiteKnight]; mask != 0; mask &= (mask-1)) {
    mob_w |= MaskKnightMoves[first_one(mask)];
  }
  for (mask = Board->mp[WhiteBishop]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob_w |= LINE1(square) | LINE2(square);
  }
  for (mask = Board->mp[WhiteRook]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob_w |= LINE3(square) | LINE4(square);
  }
  for (mask = Board->mp[WhiteQueen]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob_w |= LINE1(square) | LINE2(square) | LINE3(square) | LINE4(square);
  }
  mob_w |= MaskKingMoves[first_one(Board->mp[WhiteKing])];

  mob_b = Board->mp[BlackPawn];
  mob_b = (((mob_b >> 2) & 0x3FBFBFBFBFBFBF80) | (mob_b & 0x7F7F7F7F7F7F7F00)) >> 7;
  for (mask = Board->mp[BlackKnight]; mask != 0; mask &= (mask-1)) {
    mob_b |= MaskKnightMoves[first_one(mask)];
  }
  for (mask = Board->mp[BlackBishop]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob_b |= LINE1(square) | LINE2(square);
  }
  for (mask = Board->mp[BlackRook]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob_b |= LINE3(square) | LINE4(square);
  }
  for (mask = Board->mp[BlackQueen]; mask != 0; mask &= (mask-1)) {
    square = first_one(mask);
    mob_b |= LINE1(square) | LINE2(square) | LINE3(square) | LINE4(square);
  }
  mob_b |= MaskKingMoves[first_one(Board->mp[BlackKing])];

  if (Board->turn == 0) {
    pos_info->mob[0] = mob_w;
    pos_info->mob[1] = mob_b;
    mask = (Board->mp[WhiteKing]) & mob_b;
  }
  else {
    pos_info->mob[0] = mob_b;
    pos_info->mob[1] = mob_w;
    mask = (Board->mp[BlackKing]) & mob_w;
  }
  if (mask == 0) return 0;
            else return 1;
}
//////////////////////////////////////////////////////////////////////////////
// Оценка пешечной структуры
struct pawn_info_t * pawn_get_info()
{ struct pawn_info_t * entry;
  unsigned int lock;
  char open_file;
  int square;
  unsigned __int64 mask, mask1, mask2;
  __int16 opening, endgame;
  unsigned char wp_pass_file, bp_pass_file;

  // Сначала ищем сохраненную информацию в таблице Pawn
  entry = pawn_entry + ((unsigned int)(Board->pawn_key) & pawn_mask);
  lock = (unsigned int)((Board->pawn_key) >> 32);
  if (entry->lock == lock) return entry;    // Нашли
  // Не нашли в таблице - считаем
  opening = endgame = 0;             // Общая оценка пешечной структуры
  wp_pass_file = bp_pass_file = 0;   // Здесь будем запоминать открытые вертикали
  for (mask = Board->mp[WhitePawn]; mask != 0; mask &= (mask-1)) {    // По белым пешкам
    square = first_one(mask);
    if (MaskPawnDoubled[square] & Board->mp[WhitePawn]) endgame -= 158;
    open_file = 1;
    if (MaskPawnOpenFileW[square] & (Board->mp[WhitePawn] | Board->mp[BlackPawn])) open_file = 0;
    if ((MaskPawnIsolated[square] & Board->mp[WhitePawn]) == 0) {
      if (open_file) { opening -= 923; endgame -= 324; }
                else { opening -= 79;  endgame -= 324; }
    }
    else if ((MaskPawnProtectedW[square] & Board->mp[WhitePawn]) == 0) {
      if ((MaskPawnAttacksW1[square] & Board->mp[BlackPawn]) ||
          ((MaskPawnAttacksW2[square] & Board->mp[BlackPawn]) &&
          ((MaskPawnAttacks[0][square] & Board->mp[WhitePawn])==0))) {
        if (open_file) opening -= 872; else opening -= 406;
        endgame -= 297;
      }
    }
    if ((MaskPawnPassedW[square] & Board->mp[BlackPawn]) == 0) wp_pass_file |= PawnPassedFile[square & 7];
    else if (open_file) {
      mask1 = MaskPawnProtectedW[square] & Board->mp[WhitePawn];
      mask2 = MaskPawnPassedW[square] & Board->mp[BlackPawn];
      if (popcnt(mask1) >= popcnt(mask2)) {
        opening += PawnPassedScoreOpening[square >> 3];
        endgame += PawnPassedScoreEndgame[square >> 3];
      }
    }
  }
  for (mask = Board->mp[BlackPawn]; mask != 0; mask &= (mask-1)) {    // По черным
    square = first_one(mask);
    if (MaskPawnDoubled[square] & Board->mp[BlackPawn]) endgame += 158;
    open_file = 1;
    if (MaskPawnOpenFileB[square] & (Board->mp[WhitePawn] | Board->mp[BlackPawn])) open_file = 0;
    if ((MaskPawnIsolated[square] & Board->mp[BlackPawn]) == 0) {
      if (open_file) { opening += 923; endgame += 324; }
                else { opening += 79;  endgame += 324; }
    }
    else if ((MaskPawnProtectedB[square] & Board->mp[BlackPawn]) == 0) {
      if ((MaskPawnAttacksB1[square] & Board->mp[WhitePawn]) ||
          ((MaskPawnAttacksB2[square] & Board->mp[WhitePawn]) &&
          ((MaskPawnAttacks[1][square] & Board->mp[BlackPawn])==0))) {
        if (open_file) opening += 872; else opening += 406;
        endgame += 297;
      }
    }
    if ((MaskPawnPassedB[square] & Board->mp[WhitePawn]) == 0) bp_pass_file |= PawnPassedFile[square & 7];
    else if (open_file) {
      mask1 = MaskPawnProtectedB[square] & Board->mp[BlackPawn];
      mask2 = MaskPawnPassedB[square] & Board->mp[WhitePawn];
      if (popcnt(mask1) >= popcnt(mask2)) {
        opening -= PawnPassedScoreOpening[7 - (square >> 3)];
        endgame -= PawnPassedScoreEndgame[7 - (square >> 3)];
      }
    }
  }
  // Заносим в таблицу
  entry->pass_file[0] = wp_pass_file;
  entry->opening = opening;
  entry->pass_file[1] = bp_pass_file;
  entry->endgame = endgame;
  // Оцениваем пешечное прикрытие короля по флангам и/или в центре
  mask = Board->mp[BlackPawn];
  mask1 = ((mask >> 23) & 0x0E00) | ((mask >> 18) & 0x01C0) | ((mask >> 13) & 0x0038) | ((mask >> 8) & 0x0007);
  mask = Board->mp[WhitePawn];
  mask2 = ((mask >> 23) & 0x0E00) | ((mask >> 18) & 0x01C0) | ((mask >> 13) & 0x0038) | ((mask >> 8) & 0x0007);
  entry->sheet_white_king[0] = PawnStruScore1[mask1] + PawnStruScore0[mask2];
  mask = Board->mp[BlackPawn];
  mask1 = ((mask >> 26) & 0x0E00) | ((mask >> 21) & 0x01C0) | ((mask >> 16) & 0x0038) | ((mask >> 11) & 0x0007);
  mask = Board->mp[WhitePawn];
  mask2 = ((mask >> 26) & 0x0E00) | ((mask >> 21) & 0x01C0) | ((mask >> 16) & 0x0038) | ((mask >> 11) & 0x0007);
  entry->sheet_white_king[1] = PawnStruScore1[mask1] + PawnStruScore0[mask2];
  mask = Board->mp[BlackPawn];
  mask1 = ((mask >> 28) & 0x0E00) | ((mask >> 23) & 0x01C0) | ((mask >> 18) & 0x0038) | ((mask >> 13) & 0x0007);
  mask = Board->mp[WhitePawn];
  mask2 = ((mask >> 28) & 0x0E00) | ((mask >> 23) & 0x01C0) | ((mask >> 18) & 0x0038) | ((mask >> 13) & 0x0007);
  entry->sheet_white_king[2] = PawnStruScore1[mask1] + PawnStruScore0[mask2];
  mask = Board->mp[BlackPawn];
  mask1 = ((mask >> 15) & 0x0E00) | ((mask >> 26) & 0x01C0) | ((mask >> 37) & 0x0038) | ((mask >> 48) & 0x0007);
  mask = Board->mp[WhitePawn];
  mask2 = ((mask >> 15) & 0x0E00) | ((mask >> 26) & 0x01C0) | ((mask >> 37) & 0x0038) | ((mask >> 48) & 0x0007);
  entry->sheet_black_king[0] = PawnStruScore0[mask1] + PawnStruScore1[mask2];
  mask = Board->mp[BlackPawn];
  mask1 = ((mask >> 18) & 0x0E00) | ((mask >> 29) & 0x01C0) | ((mask >> 40) & 0x0038) | ((mask >> 51) & 0x0007);
  mask = Board->mp[WhitePawn];
  mask2 = ((mask >> 18) & 0x0E00) | ((mask >> 29) & 0x01C0) | ((mask >> 40) & 0x0038) | ((mask >> 51) & 0x0007);
  entry->sheet_black_king[1] = PawnStruScore0[mask1] + PawnStruScore1[mask2];
  mask = Board->mp[BlackPawn];
  mask1 = ((mask >> 20) & 0x0E00) | ((mask >> 31) & 0x01C0) | ((mask >> 42) & 0x0038) | ((mask >> 53) & 0x0007);
  mask = Board->mp[WhitePawn];
  mask2 = ((mask >> 20) & 0x0E00) | ((mask >> 31) & 0x01C0) | ((mask >> 42) & 0x0038) | ((mask >> 53) & 0x0007);
  entry->sheet_black_king[2] = PawnStruScore0[mask1] + PawnStruScore1[mask2];
  entry->lock = lock;
  return entry;
}
