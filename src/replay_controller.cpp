#include "replay_controller.h"

#include "file_manager.h"

bool ReplayController::Load(const std::string& path) {
  GameRecord tmp;
  if (!FileManager::Load(path, &tmp)) return false;
  record_ = tmp;
  return true;
}

Board ReplayController::BoardAtStep(int step) const {
  Board b;
  const auto& moves = record_.moves();
  for (int i = 0; i < step && i < static_cast<int>(moves.size()); ++i) {
    b.PlaceStone(moves[i].row, moves[i].col, moves[i].player);
  }
  return b;
}
