#include "board.h"

Board::Board() { Reset(); }

void Board::Reset() {
  for (int r = 0; r < kSize; ++r)
    for (int c = 0; c < kSize; ++c) grid_[r][c] = Cell::kEmpty;
}

bool Board::PlaceStone(int row, int col, Player player) {
  if (row < 0 || row >= kSize || col < 0 || col >= kSize) return false;
  if (grid_[row][col] != Cell::kEmpty) return false;
  grid_[row][col] = (player == Player::kBlack) ? Cell::kBlack : Cell::kWhite;
  return true;
}

void Board::RemoveStone(int row, int col) {
  if (row >= 0 && row < kSize && col >= 0 && col < kSize)
    grid_[row][col] = Cell::kEmpty;
}

Cell Board::GetCell(int row, int col) const {
  if (row < 0 || row >= kSize || col < 0 || col >= kSize) return Cell::kEmpty;
  return grid_[row][col];
}

bool Board::CheckWin(int row, int col, Player player) const {
  // Check all four axes: horizontal, vertical, diagonal, anti-diagonal.
  const int dr[] = {0, 1, 1, 1};
  const int dc[] = {1, 0, 1, -1};
  for (int i = 0; i < 4; ++i) {
    int count = 1;
    count += CountInDirection(row, col, player, dr[i], dc[i]);
    count += CountInDirection(row, col, player, -dr[i], -dc[i]);
    if (count >= 5) return true;
  }
  return false;
}

bool Board::IsFull() const {
  for (int r = 0; r < kSize; ++r)
    for (int c = 0; c < kSize; ++c)
      if (grid_[r][c] == Cell::kEmpty) return false;
  return true;
}

int Board::CountInDirection(int row, int col, Player player, int dr,
                            int dc) const {
  Cell target = (player == Player::kBlack) ? Cell::kBlack : Cell::kWhite;
  int count = 0;
  int r = row + dr;
  int c = col + dc;
  while (r >= 0 && r < kSize && c >= 0 && c < kSize && grid_[r][c] == target) {
    ++count;
    r += dr;
    c += dc;
  }
  return count;
}
