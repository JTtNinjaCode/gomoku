#include "app.h"

#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "file_manager.h"
#include "game.h"
#include "replay_controller.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

App::App() {}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void App::Run() {
  State state = State::kMainMenu;

  while (state != State::kQuit) {
    switch (state) {
      case State::kMainMenu:
        state = RunMainMenu();
        break;
      case State::kLocalTwoPlayer:
        state = RunLocalTwoPlayer();
        break;
      case State::kLocalSinglePlayer:
        std::cout << "\n  Single Player vs AI is not yet implemented.\n";
        PromptReturnToMenu();
        state = State::kMainMenu;
        break;
      case State::kNetwork:
        std::cout << "\n  Network Multiplayer is not yet implemented.\n";
        PromptReturnToMenu();
        state = State::kMainMenu;
        break;
      case State::kReplay:
        state = RunReplay();
        break;
      case State::kSettings:
        state = RunSettings();
        break;
      case State::kQuit:
        break;
    }
  }

  std::cout << "Goodbye!\n";
}

// ---------------------------------------------------------------------------
// Main menu
// ---------------------------------------------------------------------------

App::State App::RunMainMenu() {
  grid_.RenderMainMenu();

  std::string input;
  if (!std::getline(std::cin, input)) return State::kQuit;
  if (input.empty()) return State::kMainMenu;

  switch (input[0]) {
    case '1':
      return State::kLocalSinglePlayer;
    case '2':
      return State::kLocalTwoPlayer;
    case '3':
      return State::kNetwork;
    case '4':
      return State::kReplay;
    case '5':
      return State::kSettings;
    case '0':
      return State::kQuit;
    default:
      return State::kMainMenu;
  }
}

// ---------------------------------------------------------------------------
// Local two-player game
// ---------------------------------------------------------------------------

App::State App::RunLocalTwoPlayer() {
  Game game(config_);
  GameRecord record = game.Run();
  PromptSaveGame(record);
  PromptReturnToMenu();
  return State::kMainMenu;
}

// ---------------------------------------------------------------------------
// Replay
// ---------------------------------------------------------------------------

App::State App::RunReplay() {
  std::cout << "\033[2J\033[H";
  std::cout << "Enter path to .gom file: ";
  std::string path;
  if (!std::getline(std::cin, path)) return State::kMainMenu;

  // Trim whitespace.
  while (!path.empty() && (path.front() == ' ' || path.front() == '\t'))
    path.erase(path.begin());
  while (!path.empty() && (path.back() == ' ' || path.back() == '\t' ||
                            path.back() == '\r'))
    path.pop_back();

  ReplayController rc;
  if (!rc.Load(path)) {
    std::cout << "  Failed to load '" << path
              << "'. Check the path and file format.\n";
    PromptReturnToMenu();
    return State::kMainMenu;
  }

  rc.Run(grid_);
  return State::kMainMenu;
}

// ---------------------------------------------------------------------------
// Settings  (implemented in stage 9)
// ---------------------------------------------------------------------------

App::State App::RunSettings() {
  while (true) {
    grid_.RenderSettings(config_);

    std::string input;
    if (!std::getline(std::cin, input)) break;
    if (input.empty()) continue;

    switch (input[0]) {
      case '1':
        config_.set_undo_enabled(!config_.undo_enabled());
        break;
      case '2':
        config_.set_time_limit_enabled(!config_.time_limit_enabled());
        break;
      case '3':
        if (config_.time_limit_enabled()) {
          std::cout << "  Enter new time limit in seconds: ";
          std::string val;
          if (std::getline(std::cin, val)) {
            try {
              int secs = std::stoi(val);
              if (secs > 0) config_.set_time_limit_seconds(secs);
            } catch (...) {
            }
          }
        }
        break;
      case '0':
        config_.Save();
        return State::kMainMenu;
      default:
        break;
    }
  }

  config_.Save();
  return State::kMainMenu;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void App::PromptSaveGame(const GameRecord& record) {
  std::cout << "\nSave game record? (y/n): ";
  std::string answer;
  if (!std::getline(std::cin, answer)) return;
  if (answer.empty() || (answer[0] != 'y' && answer[0] != 'Y')) return;

  // Build default filename: saves/YYYYMMDD_HHMMSS.gom
  std::time_t t = std::time(nullptr);
  std::tm tm_buf;
  localtime_r(&t, &tm_buf);
  std::ostringstream default_name;
  default_name << "saves/" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S") << ".gom";

  std::cout << "File path [" << default_name.str() << "]: ";
  std::string path;
  std::getline(std::cin, path);

  // Trim.
  while (!path.empty() && (path.front() == ' ' || path.front() == '\t'))
    path.erase(path.begin());
  while (!path.empty() && (path.back() == ' ' || path.back() == '\t' ||
                            path.back() == '\r'))
    path.pop_back();

  if (path.empty()) path = default_name.str();

  // Ensure parent directory exists.
  {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
      std::error_code ec;
      std::filesystem::create_directories(p.parent_path(), ec);
    }
  }

  if (FileManager::Save(record, path)) {
    std::cout << "  Saved to: " << path << "\n";
  } else {
    std::cout << "  Failed to save to: " << path << "\n";
  }
}

void App::PromptReturnToMenu() {
  std::cout << "\nPress Enter to return to main menu...";
  std::string dummy;
  std::getline(std::cin, dummy);
}
