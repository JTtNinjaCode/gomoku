#include "game.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

Game::Game(const Configuration& config) : config_(config) {}

// ---------------------------------------------------------------------------
// Input parsing
// ---------------------------------------------------------------------------

bool Game::ParseInput(const std::string& input, int* row, int* col) const {
  std::string s = input;
  // Trim leading/trailing whitespace.
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
    s.erase(s.begin());
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
    s.pop_back();
  if (s.empty()) return false;

  // Format 1: letter + number, e.g. "H8" or "h8".
  if (std::isalpha(static_cast<unsigned char>(s[0]))) {
    char letter =
        static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
    if (letter < 'A' || letter > 'O') return false;
    std::string num_str = s.substr(1);
    // Trim spaces between letter and number.
    while (!num_str.empty() &&
           std::isspace(static_cast<unsigned char>(num_str.front())))
      num_str.erase(num_str.begin());
    try {
      int r = std::stoi(num_str);
      if (r < 1 || r > Board::kSize) return false;
      *col = letter - 'A';  // 0-indexed
      *row = r - 1;         // 0-indexed
    } catch (...) {
      return false;
    }
    return true;
  }

  // Format 2: two integers, e.g. "8 8" (row col, 1-indexed).
  std::istringstream ss(s);
  int r, c;
  if (!(ss >> r >> c)) return false;
  if (r < 1 || r > Board::kSize || c < 1 || c > Board::kSize) return false;
  *row = r - 1;
  *col = c - 1;
  return true;
}

// ---------------------------------------------------------------------------
// Timer
// ---------------------------------------------------------------------------

int Game::ComputeSecondsRemaining() const {
  if (!config_.time_limit_enabled()) return -1;
  auto elapsed = std::chrono::steady_clock::now() - turn_start_;
  int elapsed_sec = static_cast<int>(
      std::chrono::duration_cast<std::chrono::seconds>(elapsed).count());
  int remaining = config_.time_limit_seconds() - elapsed_sec;
  return (remaining > 0) ? remaining : 0;
}

// ---------------------------------------------------------------------------
// Single turn
// ---------------------------------------------------------------------------

Game::TurnResult Game::PlayTurn(Player current_player, int move_number) {
  turn_start_ = std::chrono::steady_clock::now();

  while (true) {
    int secs = ComputeSecondsRemaining();

    panel_.Update(move_number, current_player, config_.time_limit_enabled(),
                  (secs >= 0) ? secs : 0);
    grid_.Render(board_, panel_);

    // Check timeout (only detectable after input returns).
    if (config_.time_limit_enabled() && secs == 0) {
      std::cout << "  Time's up! "
                << (current_player == Player::kBlack ? "Black" : "White")
                << " forfeits.\n";
      return TurnResult::kTimeout;
    }

    // Build prompt.
    std::cout << "Enter move (e.g. H8 or 8 8)";
    if (config_.undo_enabled()) std::cout << " [or 'undo']";
    std::cout << ": ";

    std::string input;
    if (!std::getline(std::cin, input)) {
      // EOF / stream error — treat as quit.
      return TurnResult::kContinue;
    }

    // Handle undo.
    {
      std::string lower = input;
      std::transform(lower.begin(), lower.end(), lower.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      if (lower == "undo") {
        if (!config_.undo_enabled()) {
          std::cout << "  Undo is disabled. Press Enter to continue.\n";
          std::getline(std::cin, input);
          continue;
        }
        if (record_.size() < 2) {
          std::cout << "  Nothing to undo. Press Enter to continue.\n";
          std::getline(std::cin, input);
          continue;
        }
        // Undo two half-moves so the same player moves again.
        const auto& last2 = record_.moves()[record_.size() - 2];
        const auto& last1 = record_.moves()[record_.size() - 1];
        board_.RemoveStone(last1.row, last1.col);
        board_.RemoveStone(last2.row, last2.col);
        record_.RemoveLastMove();
        record_.RemoveLastMove();
        return TurnResult::kUndo;
      }
    }

    int row, col;
    if (!ParseInput(input, &row, &col)) {
      std::cout << "  Invalid input. Use a letter + number (e.g. H8) or two "
                   "numbers (e.g. 8 8).\n";
      std::cout << "  Press Enter to try again.";
      std::getline(std::cin, input);
      continue;
    }

    if (!board_.PlaceStone(row, col, current_player)) {
      std::cout << "  That cell is already occupied or out of bounds.\n";
      std::cout << "  Press Enter to try again.";
      std::getline(std::cin, input);
      continue;
    }

    // Record the move.
    record_.AddMove({move_number, current_player, row, col});

    // Check win / draw.
    if (board_.CheckWin(row, col, current_player)) return TurnResult::kWin;
    if (board_.IsFull()) return TurnResult::kDraw;
    return TurnResult::kContinue;
  }
}

// ---------------------------------------------------------------------------
// Main game loop
// ---------------------------------------------------------------------------

GameRecord Game::Run() {
  board_.Reset();

  Player current = Player::kBlack;
  int move_number = 1;

  auto game_start = std::chrono::steady_clock::now();

  std::optional<Player> winner;

  while (true) {
    TurnResult result = PlayTurn(current, move_number);

    if (result == TurnResult::kUndo) {
      // Two half-moves were undone; back up the move counter by 2.
      move_number = std::max(1, move_number - 2);
      // Keep the same player (undo restores it to that player's turn).
      continue;
    }

    if (result == TurnResult::kWin) {
      winner = current;
      break;
    }
    if (result == TurnResult::kDraw) {
      break;
    }
    if (result == TurnResult::kTimeout) {
      // The other player wins by forfeit.
      winner = (current == Player::kBlack) ? Player::kWhite : Player::kBlack;
      break;
    }

    // Advance turn.
    current = (current == Player::kBlack) ? Player::kWhite : Player::kBlack;
    ++move_number;
  }

  // Re-render the final board state.
  panel_.Update(move_number, current, false, 0);
  grid_.Render(board_, panel_);
  grid_.RenderWin(winner);

  // Build game duration.
  auto game_end = std::chrono::steady_clock::now();
  int duration_sec = static_cast<int>(
      std::chrono::duration_cast<std::chrono::seconds>(game_end - game_start)
          .count());

  // Build ISO date string.
  std::time_t t = std::time(nullptr);
  std::tm tm_buf;
  localtime_r(&t, &tm_buf);
  std::ostringstream date_ss;
  date_ss << std::put_time(&tm_buf, "%Y-%m-%d");

  GameMetadata meta;
  meta.date = date_ss.str();
  meta.duration_sec = duration_sec;
  meta.total_moves = record_.size();
  meta.winner = winner;
  meta.undo_enabled = config_.undo_enabled();
  meta.time_limit_enabled = config_.time_limit_enabled();
  meta.time_limit_seconds = config_.time_limit_seconds();

  record_.Finalize(meta);
  return record_;
}
