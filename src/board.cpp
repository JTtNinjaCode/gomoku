#include "board.h"

Board::Board() { Reset(); }

void Board::Reset() {
  for (int r = 0; r < kSize; ++r)
    for (int c = 0; c < kSize; ++c) grid_[r][c] = Cell::kEmpty;
}

bool Board::PlaceStone(int row, int col, Player player) { return false; }
void Board::RemoveStone(int row, int col) {}
Cell Board::GetCell(int row, int col) const { return Cell::kEmpty; }
bool Board::CheckWin(int row, int col, Player player) const { return false; }
bool Board::IsFull() const { return false; }
int Board::CountInDirection(int row, int col, Player player, int dr,
                            int dc) const {
  return 0;
}
