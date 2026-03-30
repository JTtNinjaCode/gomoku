#include "app.h"

App::App() {}

void App::Run() {}

App::State App::RunMainMenu() { return State::kQuit; }
App::State App::RunLocalTwoPlayer() { return State::kMainMenu; }
App::State App::RunReplay() { return State::kMainMenu; }
App::State App::RunSettings() { return State::kMainMenu; }
void App::PromptSaveGame(const GameRecord& record) {}
void App::PromptReturnToMenu() {}
