#include "game.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

Game::Game(const Configuration& config) : config_(config) {}

// ---------------------------------------------------------------------------
// Start / reset
// ---------------------------------------------------------------------------

void Game::Start() {
  board_.Reset();
  record_ = GameRecord();
  state_ = State::kPlaying;
  current_player_ = Player::kBlack;
  move_number_ = 1;
  winner_ = std::nullopt;

  auto now = std::chrono::steady_clock::now();
  turn_start_ = now;
  game_start_ = now;
}

// ---------------------------------------------------------------------------
// Per-frame update — checks the timer
// ---------------------------------------------------------------------------

void Game::Update() {
  if (state_ != State::kPlaying) return;
  if (config_.time_limit_enabled() && seconds_remaining() <= 0.0f) {
    winner_ =
        (current_player_ == Player::kBlack) ? Player::kWhite : Player::kBlack;
    state_ = State::kTimeout;
  }
}

// ---------------------------------------------------------------------------
// Timer query
// ---------------------------------------------------------------------------

float Game::seconds_remaining() const {
  if (!config_.time_limit_enabled()) return -1.0f;
  using namespace std::chrono;
  float elapsed = duration<float>(steady_clock::now() - turn_start_).count();
  float remaining = static_cast<float>(config_.time_limit_seconds()) - elapsed;
  return remaining > 0.0f ? remaining : 0.0f;
}

// ---------------------------------------------------------------------------
// Stone placement
// ---------------------------------------------------------------------------

bool Game::TryPlaceStone(int row, int col) {
  if (state_ != State::kPlaying) return false;
  if (!board_.PlaceStone(row, col, current_player_)) return false;

  record_.AddMove({move_number_, current_player_, row, col});

  if (board_.CheckWin(row, col, current_player_)) {
    winner_ = current_player_;
    state_ = State::kWin;
    return true;
  }
  if (board_.IsFull()) {
    state_ = State::kDraw;
    return true;
  }

  current_player_ =
      (current_player_ == Player::kBlack) ? Player::kWhite : Player::kBlack;
  ++move_number_;
  turn_start_ = std::chrono::steady_clock::now();
  return true;
}

// ---------------------------------------------------------------------------
// Undo
// ---------------------------------------------------------------------------

void Game::Undo() {
  if (state_ != State::kPlaying) return;
  if (!config_.undo_enabled()) return;
  if (record_.size() < 2) return;

  const auto& m1 = record_.moves()[record_.size() - 1];
  const auto& m2 = record_.moves()[record_.size() - 2];
  board_.RemoveStone(m1.row, m1.col);
  board_.RemoveStone(m2.row, m2.col);
  record_.RemoveLastMove();
  record_.RemoveLastMove();
  move_number_ = std::max(1, move_number_ - 2);
  turn_start_ = std::chrono::steady_clock::now();
}

// ---------------------------------------------------------------------------
// Finalise record
// ---------------------------------------------------------------------------

GameRecord Game::FinalizeRecord() {
  using namespace std::chrono;
  int duration_sec = static_cast<int>(
      duration_cast<seconds>(steady_clock::now() - game_start_).count());

  std::time_t t = std::time(nullptr);
  std::tm* tm_buf = std::localtime(&t);
  std::ostringstream date_ss;
  date_ss << std::put_time(tm_buf, "%Y-%m-%d");

  GameMetadata meta;
  meta.date = date_ss.str();
  meta.duration_sec = duration_sec;
  meta.total_moves = record_.size();
  meta.winner = winner_;
  meta.undo_enabled = config_.undo_enabled();
  meta.time_limit_enabled = config_.time_limit_enabled();
  meta.time_limit_seconds = config_.time_limit_seconds();

  record_.Finalize(meta);
  return record_;
}
