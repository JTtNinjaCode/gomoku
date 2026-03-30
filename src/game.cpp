#include "game.h"

Game::Game(const Configuration& config) : config_(config) {}

GameRecord Game::Run() { return record_; }

Game::TurnResult Game::PlayTurn(Player current_player, int move_number) {
  return TurnResult::kContinue;
}

bool Game::ParseInput(const std::string& input, int* row, int* col) const {
  return false;
}

int Game::ComputeSecondsRemaining() const { return -1; }
