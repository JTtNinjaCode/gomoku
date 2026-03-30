#include "game_record.h"

void GameRecord::AddMove(const MoveRecord& move) { moves_.push_back(move); }
void GameRecord::RemoveLastMove() {
  if (!moves_.empty()) moves_.pop_back();
}
void GameRecord::Finalize(const GameMetadata& metadata) {
  metadata_ = metadata;
}
const GameMetadata& GameRecord::metadata() const { return metadata_; }
const std::vector<MoveRecord>& GameRecord::moves() const { return moves_; }
int GameRecord::size() const { return static_cast<int>(moves_.size()); }
