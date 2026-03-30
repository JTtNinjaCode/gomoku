#pragma once

#include "configuration.h"
#include "game_record.h"
#include "grid.h"

// Top-level application state machine.
// Owns configuration and the terminal renderer; dispatches into sub-modes.
class App {
 public:
  App();

  // Enters the main loop and runs until the user chooses Quit.
  void Run();

 private:
  enum class State {
    kMainMenu,
    kLocalTwoPlayer,
    kLocalSinglePlayer,  // placeholder: not yet implemented
    kNetwork,            // placeholder: not yet implemented
    kReplay,
    kSettings,
    kQuit,
  };

  State RunMainMenu();
  State RunLocalTwoPlayer();
  State RunReplay();
  State RunSettings();

  // Asks the user whether to save, and if yes, prompts for a file path.
  void PromptSaveGame(const GameRecord& record);

  // Prints a "Press Enter to return to main menu" prompt and waits.
  void PromptReturnToMenu();

  Configuration config_;
  Grid grid_;
};
