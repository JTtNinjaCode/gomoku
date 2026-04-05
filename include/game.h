#pragma once

#include <chrono>
#include <optional>

#include "board.h"
#include "configuration.h"
#include "game_record.h"
#include "move_record.h"

// Non-blocking game state machine for use with an ImGui immediate-mode loop.
// Callers must call Update() every frame and check state() for the outcome.
class Game {
 public:
  enum class State { kPlaying, kWin, kDraw, kTimeout };

  explicit Game(const Configuration& config);

  // Resets the board and starts a fresh game; Black moves first.
  void Start();

  // Must be called every frame. Checks the turn timer and transitions
  // to kTimeout automatically when time runs out.
  void Update();

  // Attempts to place a stone at (row, col) for the current player.
  // Returns true and advances the turn on success.
  // No-op (returns false) if the game is not in kPlaying state or the
  // cell is invalid / occupied.
  bool TryPlaceStone(int row, int col);

  // Undoes the last two half-moves so the same player moves again.
  // Requires undo to be enabled and at least two moves on record.
  void Undo();

  // Builds and returns the completed GameRecord.
  // Must only be called after the game is over (state != kPlaying).
  GameRecord FinalizeRecord();

  // ── Accessors ────────────────────────────────────────────────────────────
  State state() const { return state_; }
  Player current_player() const { return current_player_; }
  const Board& board() const { return board_; }
  int move_number() const { return move_number_; }
  std::optional<Player> winner() const { return winner_; }

  // Returns fractional seconds remaining this turn, or -1.0f if disabled.
  float seconds_remaining() const;

 private:
  const Configuration& config_;
  Board board_;
  GameRecord record_;

  State state_ = State::kPlaying;
  Player current_player_ = Player::kBlack;
  int move_number_ = 1;
  std::optional<Player> winner_;

  std::chrono::steady_clock::time_point turn_start_;
  std::chrono::steady_clock::time_point game_start_;
};
