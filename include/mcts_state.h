#pragma once

#include <utility>
#include <vector>

#include "board.h"
#include "game_record.h"
#include "move_record.h"

// Self-contained copyable board state for MCTS simulation.
// Independent of the game's Board/Game classes.
// Uses int encoding: 0=empty, 1=black, 2=white.
class MCTSState {
 public:
  static constexpr int kSize          = 15;
  static constexpr int kFeaturePlanes = 8;
  static constexpr int kChannels      = 9;  // kFeaturePlanes + 1 color plane

  MCTSState() = default;

  // Build from the live game state before handing off to the AI thread.
  static MCTSState FromGame(const Board& board, Player player,
                            const GameRecord& record);

  // Execute action (flat index = row*15 + col).
  void DoMove(int action);

  // Returns flat indices of all empty cells.
  std::vector<int> Availables() const;

  // {game_ended, winner}: winner 1=black, 2=white, -1=draw, 0=ongoing
  std::pair<bool, int> GameEnd() const;

  // Returns kChannels * kSize * kSize = 2025-element feature vector.
  std::vector<float> GetFeature() const;

  int current_player() const { return current_player_; }

 private:
  int board_[kSize][kSize] = {};
  int current_player_ = 1;  // 1=black, 2=white

  // Ordered move history: (flat_pos, player_int), oldest first.
  std::vector<std::pair<int, int>> states_;

  // Returns true if the stone at (row, col) forms 5-in-a-row for `player`.
  bool CheckWin(int row, int col, int player) const;
  int  CountDir(int row, int col, int player, int dr, int dc) const;
};
