#include "grid.h"

#include <iostream>

void Grid::Render(const Board& board, const InfoPanel& panel) const {}
void Grid::RenderWin(std::optional<Player> winner) const {}
void Grid::RenderMainMenu() const {}
void Grid::RenderSettings(const Configuration& config) const {}
void Grid::RenderReplayFrame(const Board& board, int step, int total,
                             const GameMetadata& meta) const {}
void Grid::DrawBoard(const Board& board) const {}
void Grid::DrawInfoBar(const InfoPanel& panel) const {}
char Grid::PlayerChar(Player p) {
  return p == Player::kBlack ? 'X' : 'O';
}
