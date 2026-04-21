#include "mcts_state.h"

#include <algorithm>
#include <cstring>

MCTSState MCTSState::FromGame(const Board& board, Player player,
                               const GameRecord& record) {
  MCTSState s;
  s.current_player_ = (player == Player::kBlack) ? 1 : 2;

  for (int r = 0; r < kSize; ++r) {
    for (int c = 0; c < kSize; ++c) {
      Cell cell = board.GetCell(r, c);
      if (cell == Cell::kBlack)
        s.board_[r][c] = 1;
      else if (cell == Cell::kWhite)
        s.board_[r][c] = 2;
    }
  }

  for (const MoveRecord& mv : record.moves()) {
    int pl = (mv.player == Player::kBlack) ? 1 : 2;
    s.states_.push_back({mv.row * kSize + mv.col, pl});
  }

  return s;
}

void MCTSState::DoMove(int action) {
  int row = action / kSize;
  int col = action % kSize;
  board_[row][col] = current_player_;
  states_.push_back({action, current_player_});
  current_player_ = 3 - current_player_;
}

std::vector<int> MCTSState::Availables() const {
  std::vector<int> avail;
  avail.reserve(kSize * kSize);
  for (int r = 0; r < kSize; ++r)
    for (int c = 0; c < kSize; ++c)
      if (board_[r][c] == 0)
        avail.push_back(r * kSize + c);
  return avail;
}

std::pair<bool, int> MCTSState::GameEnd() const {
  if (states_.empty()) return {false, 0};

  // The last mover is the player who just played (before current_player_ flip).
  int last_mover = 3 - current_player_;
  int last_pos   = states_.back().first;
  int last_row   = last_pos / kSize;
  int last_col   = last_pos % kSize;

  if (CheckWin(last_row, last_col, last_mover))
    return {true, last_mover};

  if (Availables().empty())
    return {true, -1};  // draw

  return {false, 0};
}

int MCTSState::CountDir(int row, int col, int player,
                         int dr, int dc) const {
  int count = 0;
  int r = row + dr, c = col + dc;
  while (r >= 0 && r < kSize && c >= 0 && c < kSize &&
         board_[r][c] == player) {
    ++count;
    r += dr;
    c += dc;
  }
  return count;
}

bool MCTSState::CheckWin(int row, int col, int player) const {
  const int dirs[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
  for (auto& d : dirs) {
    int cnt = 1 + CountDir(row, col, player, d[0], d[1]) +
              CountDir(row, col, player, -d[0], -d[1]);
    if (cnt >= 5) return true;
  }
  return false;
}

std::vector<float> MCTSState::GetFeature() const {
  const int HW = kSize * kSize;
  std::vector<float> sq(kChannels * HW, 0.f);

  if (!states_.empty()) {
    std::vector<int> move_curr, move_oppo;
    for (auto& [pos, pl] : states_) {
      if (pl == current_player_)
        move_curr.push_back(pos);
      else
        move_oppo.push_back(pos);
    }

    // Even channels = opponent history, odd channels = self history.
    for (int i = 0; i < kFeaturePlanes; ++i) {
      const auto& mvs = (i % 2 == 0) ? move_oppo : move_curr;
      for (int m : mvs)
        sq[i * HW + m] = 1.f;
    }

    // Build states_sequence: most-recent-first slice of states_.
    struct Entry { int pos = -1; int player = -1; };
    Entry states_sequence[kFeaturePlanes];
    int n = std::min((int)states_.size(), kFeaturePlanes);
    for (int i = 0; i < n; ++i)
      states_sequence[i] = {states_[(int)states_.size() - 1 - i].first,
                             states_[(int)states_.size() - 1 - i].second};

    // Erasure for even planes (opponent history).
    for (int i = 0; i < kFeaturePlanes - 2; i += 2) {
      if (states_sequence[i].player == -1) continue;
      int m = states_sequence[i].pos;
      for (int j = i + 2; j < kFeaturePlanes; j += 2)
        sq[j * HW + m] = 0.f;
    }

    // Erasure for odd planes (self history).
    for (int i = 1; i < kFeaturePlanes - 2; i += 2) {
      if (states_sequence[i].player == -1) continue;
      int m = states_sequence[i].pos;
      for (int j = i + 2; j < kFeaturePlanes; j += 2)
        sq[j * HW + m] = 0.f;
    }
  }

  // Color plane: all 1s when it is Black's turn (current_player_ == 1).
  if (current_player_ == 1)
    std::fill(sq.begin() + kFeaturePlanes * HW, sq.end(), 1.f);

  // Vertical flip to match training orientation.
  std::vector<float> flipped(kChannels * HW);
  for (int c = 0; c < kChannels; ++c) {
    for (int h = 0; h < kSize; ++h) {
      int src_h = kSize - 1 - h;
      for (int w = 0; w < kSize; ++w)
        flipped[c * HW + h * kSize + w] = sq[c * HW + src_h * kSize + w];
    }
  }
  return flipped;
}
