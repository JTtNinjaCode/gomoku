#include "grid.h"

#include <iostream>
#include <string>

namespace {

void ClearScreen() { std::cout << "\033[2J\033[H"; }

void PrintSeparator(int width = 60) {
  std::cout << std::string(width, '-') << "\n";
}

}  // namespace

// ---------------------------------------------------------------------------
// Board rendering
// ---------------------------------------------------------------------------

char Grid::PlayerChar(Player p) {
  return (p == Player::kBlack) ? 'X' : 'O';
}

void Grid::DrawBoard(const Board& board) const {
  // Column header: A through O.
  std::cout << "    ";
  for (int c = 0; c < Board::kSize; ++c) {
    std::cout << static_cast<char>('A' + c);
    if (c < Board::kSize - 1) std::cout << ' ';
  }
  std::cout << "\n";

  for (int r = 0; r < Board::kSize; ++r) {
    // Row number right-aligned in 2 characters.
    std::cout << (r + 1 < 10 ? " " : "") << (r + 1) << "  ";
    for (int c = 0; c < Board::kSize; ++c) {
      Cell cell = board.GetCell(r, c);
      if (cell == Cell::kEmpty) {
        std::cout << '.';
      } else if (cell == Cell::kBlack) {
        std::cout << 'X';
      } else {
        std::cout << 'O';
      }
      if (c < Board::kSize - 1) std::cout << ' ';
    }
    std::cout << "\n";
  }
}

void Grid::DrawInfoBar(const InfoPanel& panel) const {
  PrintSeparator();
  std::cout << "Move " << panel.move_number() << " | ";
  if (panel.current_player() == Player::kBlack) {
    std::cout << "Black (X) to play";
  } else {
    std::cout << "White (O) to play";
  }
  if (panel.time_enabled()) {
    std::cout << " | Time: " << panel.seconds_remaining() << "s";
  }
  std::cout << "\n";
}

// ---------------------------------------------------------------------------
// Public render methods
// ---------------------------------------------------------------------------

void Grid::Render(const Board& board, const InfoPanel& panel) const {
  ClearScreen();
  DrawBoard(board);
  DrawInfoBar(panel);
}

void Grid::RenderWin(std::optional<Player> winner) const {
  PrintSeparator();
  if (!winner.has_value()) {
    std::cout << "  ** DRAW — the board is full! **\n";
  } else if (*winner == Player::kBlack) {
    std::cout << "  ** BLACK (X) WINS! **\n";
  } else {
    std::cout << "  ** WHITE (O) WINS! **\n";
  }
  PrintSeparator();
}

void Grid::RenderMainMenu() const {
  ClearScreen();
  PrintSeparator();
  std::cout << "  GOMOKU\n";
  PrintSeparator();
  std::cout << "  1. Single Player vs AI  (not implemented)\n";
  std::cout << "  2. Local Two Player\n";
  std::cout << "  3. Network Multiplayer  (not implemented)\n";
  std::cout << "  4. Replay\n";
  std::cout << "  5. Settings\n";
  std::cout << "  0. Quit\n";
  PrintSeparator();
  std::cout << "Select: ";
}

void Grid::RenderSettings(const Configuration& config) const {
  ClearScreen();
  PrintSeparator();
  std::cout << "  SETTINGS\n";
  PrintSeparator();
  std::cout << "  1. Undo feature       : "
            << (config.undo_enabled() ? "ON " : "OFF") << "\n";
  std::cout << "  2. Move time limit    : "
            << (config.time_limit_enabled() ? "ON " : "OFF") << "\n";
  if (config.time_limit_enabled()) {
    std::cout << "  3. Time limit (sec)   : " << config.time_limit_seconds()
              << "\n";
  }
  std::cout << "  0. Back\n";
  PrintSeparator();
  std::cout << "Select: ";
}

void Grid::RenderReplayFrame(const Board& board, int step, int total,
                             const GameMetadata& meta) const {
  ClearScreen();
  DrawBoard(board);
  PrintSeparator();
  std::cout << "Replay  [" << step << "/" << total << "]";
  if (step > 0) {
    // Show whose turn led to this board state.
    // The last move placed was by the player who moved at step.
  }
  std::cout << "\n";

  // Show metadata summary.
  std::cout << "Date: " << meta.date << "  |  Moves: " << meta.total_moves;
  if (meta.winner.has_value()) {
    std::cout << "  |  Winner: "
              << (*meta.winner == Player::kBlack ? "Black (X)" : "White (O)");
  } else {
    std::cout << "  |  Draw";
  }
  std::cout << "\n";
  PrintSeparator();
  std::cout << "n = next  |  p = previous  |  q = quit\n";
}
