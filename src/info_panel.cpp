#include "info_panel.h"

void InfoPanel::Update(int move_number, Player current_player,
                       bool time_enabled, int seconds_remaining) {
  move_number_ = move_number;
  current_player_ = current_player;
  time_enabled_ = time_enabled;
  seconds_remaining_ = seconds_remaining;
}

int InfoPanel::move_number() const { return move_number_; }
Player InfoPanel::current_player() const { return current_player_; }
bool InfoPanel::time_enabled() const { return time_enabled_; }
int InfoPanel::seconds_remaining() const { return seconds_remaining_; }
