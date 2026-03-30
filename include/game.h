#pragma once

#include <chrono>
#include <string>

#include "board.h"
#include "configuration.h"
#include "game_record.h"
#include "grid.h"
#include "info_panel.h"

// Runs one complete game of local two-player Gomoku.
// Returns the completed GameRecord when the game ends.
class Game {
 public:
  explicit Game(const Configuration& config);

  GameRecord Run();

 private:
  enum class TurnResult { kContinue, kWin, kDraw, kUndo, kTimeout };

  // Executes one player's turn. Reads input, updates board, detects outcomes.
  TurnResult PlayTurn(Player current_player, int move_number);

  // Parses "H8" / "h8" / "8 8" style input into 0-indexed (row, col).
  // Returns false on malformed input.
  bool ParseInput(const std::string& input, int* row, int* col) const;

  // Returns seconds remaining for the current turn, or -1 if disabled.
  int ComputeSecondsRemaining() const;

  const Configuration& config_;
  Board board_;
  InfoPanel panel_;
  GameRecord record_;
  Grid grid_;

  std::chrono::steady_clock::time_point turn_start_;
};
