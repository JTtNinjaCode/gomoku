#pragma once

#include "move_record.h"

enum class Cell { kEmpty, kBlack, kWhite };

// Owns the 15x15 board state and implements win/draw detection.
class Board {
 public:
  static constexpr int kSize = 15;

  Board();

  void Reset();

  // Returns false if (row, col) is out of bounds or already occupied.
  bool PlaceStone(int row, int col, Player player);

  // Removes the stone at (row, col). Used for undo.
  void RemoveStone(int row, int col);

  Cell GetCell(int row, int col) const;

  // Returns true if the last placed stone at (row, col) forms five in a row.
  // Must be called immediately after a successful PlaceStone.
  bool CheckWin(int row, int col, Player player) const;

  // Returns true when every cell is occupied (draw condition).
  bool IsFull() const;

 private:
  // Counts consecutive matching stones from (row, col) in direction (dr, dc).
  int CountInDirection(int row, int col, Player player, int dr, int dc) const;

  Cell grid_[kSize][kSize];
};
