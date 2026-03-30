#pragma once

#include <string>

#include "board.h"
#include "game_record.h"
#include "grid.h"

// Loads a .gom file and provides interactive step-through replay.
class ReplayController {
 public:
  // Loads the .gom file at path. Returns false on failure.
  bool Load(const std::string& path);

  // Runs the interactive replay loop.
  // Controls: n = next, p = previous, q = quit.
  void Run(const Grid& grid) const;

 private:
  // Reconstructs a Board by replaying moves[0..step-1].
  Board BoardAtStep(int step) const;

  GameRecord record_;
};
