#pragma once

#include "move_record.h"

// Pure data container representing the status bar shown below the board.
// Updated by Game; read by Grid for rendering.
class InfoPanel {
 public:
  void Update(int move_number, Player current_player, bool time_enabled,
              int seconds_remaining);

  int move_number() const;
  Player current_player() const;
  bool time_enabled() const;
  int seconds_remaining() const;

 private:
  int move_number_ = 0;
  Player current_player_ = Player::kBlack;
  bool time_enabled_ = false;
  int seconds_remaining_ = 0;
};
