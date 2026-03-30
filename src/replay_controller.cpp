#include "replay_controller.h"

#include <iostream>
#include <string>

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

void ReplayController::Run(const Grid& grid) const {
  int total = record_.size();
  int cursor = 0;

  while (true) {
    Board b = BoardAtStep(cursor);
    grid.RenderReplayFrame(b, cursor, total, record_.metadata());

    std::string input;
    if (!std::getline(std::cin, input)) break;

    if (input.empty()) continue;

    char ch = input[0];
    if (ch == 'n' || ch == 'N') {
      if (cursor < total) ++cursor;
    } else if (ch == 'p' || ch == 'P') {
      if (cursor > 0) --cursor;
    } else if (ch == 'q' || ch == 'Q') {
      break;
    }
  }
}
