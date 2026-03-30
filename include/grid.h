#pragma once

#include "board.h"
#include "configuration.h"
#include "game_record.h"
#include "info_panel.h"
#include "move_record.h"

// MVC view layer. Reads Board and InfoPanel; writes to stdout.
// Never mutates any game state.
class Grid {
 public:
  // Clears the terminal and redraws the board and info bar.
  void Render(const Board& board, const InfoPanel& panel) const;

  // Displays the win banner.
  void RenderWin(std::optional<Player> winner) const;

  // Displays the main menu.
  void RenderMainMenu() const;

  // Displays the settings screen with the current configuration.
  void RenderSettings(const Configuration& config) const;

  // Displays the board in replay mode with step counter.
  void RenderReplayFrame(const Board& board, int step, int total,
                         const GameMetadata& meta) const;

 private:
  void DrawBoard(const Board& board) const;
  void DrawInfoBar(const InfoPanel& panel) const;

  // Returns 'X' for Black, 'O' for White.
  static char PlayerChar(Player p);
};
