#pragma once

// move_record.h — plain data types shared across the project.

enum class Player { kBlack, kWhite };

struct MoveRecord {
  int move_number;  // 1-indexed
  Player player;
  int row;  // 0-indexed [0, 14]
  int col;  // 0-indexed [0, 14]
};
