#include "app.h"

#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "file_manager.h"
#include "grid.h"
#include "imgui.h"

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <cstdio>

// ---------------------------------------------------------------------------
// GLFW error callback
// ---------------------------------------------------------------------------

static void GlfwErrorCallback(int error, const char* description) {
  std::fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

App::App() = default;

App::~App() { Shutdown(); }

// ---------------------------------------------------------------------------
// Init / Shutdown
// ---------------------------------------------------------------------------

bool App::Init() {
  glfwSetErrorCallback(GlfwErrorCallback);
  if (!glfwInit()) return false;

  // GL 3.0 + GLSL 130 (Linux/Windows).
#if defined(__APPLE__)
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

  window_ = glfwCreateWindow(760, 800, "Gomoku", nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);  // vsync

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Disable .ini persistence for a self-contained executable.
  io.IniFilename = nullptr;

  ImGui::StyleColorsDark();
  // Tweak style for a cleaner game UI.
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 6.0f;
  style.FrameRounding = 4.0f;
  style.ItemSpacing = {10, 8};

  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  return true;
}

void App::Shutdown() {
  if (!window_) return;
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window_);
  glfwTerminate();
  window_ = nullptr;
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void App::Run() {
  if (!Init()) return;

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    RenderFrame();

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window_);
  }
}

// ---------------------------------------------------------------------------
// Per-frame dispatch
// ---------------------------------------------------------------------------

void App::RenderFrame() {
  // Full-screen borderless window that fills the GLFW framebuffer.
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowBgAlpha(0.0f);
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus;
  ImGui::Begin("##root", nullptr, flags);

  switch (state_) {
    case AppState::kMainMenu:  DrawMainMenu();  break;
    case AppState::kPlaying:   DrawGame();      break;
    case AppState::kGameOver:  DrawGameOver();  break;
    case AppState::kReplay:    DrawReplay();    break;
    case AppState::kSettings:  DrawSettings();  break;
  }

  ImGui::End();
}

// ---------------------------------------------------------------------------
// Main menu
// ---------------------------------------------------------------------------

void App::DrawMainMenu() {
  const float panel_w = 340.0f;
  const float panel_h = 300.0f;
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowPos(
      {(io.DisplaySize.x - panel_w) * 0.5f,
       (io.DisplaySize.y - panel_h) * 0.5f},
      ImGuiCond_Always);
  ImGui::SetNextWindowSize({panel_w, panel_h});
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                           ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoScrollbar;
  ImGui::Begin("##menu", nullptr, flags);

  // Title.
  ImGui::SetWindowFontScale(1.8f);
  float title_w = ImGui::CalcTextSize("GOMOKU").x;
  ImGui::SetCursorPosX((panel_w - title_w) * 0.5f);
  ImGui::TextUnformatted("GOMOKU");
  ImGui::SetWindowFontScale(1.0f);
  ImGui::Separator();
  ImGui::Spacing();

  const float btn_w = panel_w - 40.0f;
  auto centred_btn = [&](const char* label) -> bool {
    ImGui::SetCursorPosX(20.0f);
    return ImGui::Button(label, {btn_w, 36.0f});
  };

  if (centred_btn("Local Two Player")) {
    game_ = std::make_unique<Game>(config_);
    game_->Start();
    last_row_ = last_col_ = -1;
    finished_record_.reset();
    save_attempted_ = false;
    save_status_msg_.clear();
    state_ = AppState::kPlaying;
  }
  ImGui::Spacing();
  if (centred_btn("Replay")) {
    replay_path_buf_[0] = '\0';
    replay_error_msg_.clear();
    state_ = AppState::kReplay;
  }
  ImGui::Spacing();
  if (centred_btn("Settings")) {
    settings_undo_enabled_ = config_.undo_enabled();
    settings_time_enabled_ = config_.time_limit_enabled();
    settings_time_seconds_ = config_.time_limit_seconds();
    state_ = AppState::kSettings;
  }
  ImGui::Spacing();
  if (centred_btn("Quit")) {
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
  }

  ImGui::End();
}

// ---------------------------------------------------------------------------
// In-game
// ---------------------------------------------------------------------------

void App::DrawGame() {
  // Update timer every frame — may auto-transition to kTimeout.
  game_->Update();
  if (game_->state() != Game::State::kPlaying) {
    finished_record_ = game_->FinalizeRecord();
    state_ = AppState::kGameOver;
    return;
  }

  // ── Info bar ──────────────────────────────────────────────────────────
  const char* player_name =
      (game_->current_player() == Player::kBlack) ? "Black" : "White";
  ImGui::SetWindowFontScale(1.15f);
  ImGui::Text("Move %d  |  %s to play", game_->move_number(), player_name);
  ImGui::SetWindowFontScale(1.0f);

  if (config_.time_limit_enabled()) {
    float secs = game_->seconds_remaining();
    ImGui::SameLine();
    // Colour the timer red when under 10 s.
    if (secs <= 10.0f)
      ImGui::TextColored({1.0f, 0.25f, 0.25f, 1.0f}, "  |  Time: %.1fs", secs);
    else
      ImGui::Text("  |  Time: %.1fs", secs);
  }

  ImGui::Spacing();

  // ── Board ──────────────────────────────────────────────────────────────
  // Centre the board horizontally.
  float avail_x = ImGui::GetContentRegionAvail().x;
  float offset_x = (avail_x - Grid::kCanvasSize) * 0.5f;
  if (offset_x > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);

  auto [clicked_row, clicked_col] =
      Grid::DrawBoard(game_->board(), last_row_, last_col_);

  if (clicked_row >= 0) {
    if (game_->TryPlaceStone(clicked_row, clicked_col)) {
      last_row_ = clicked_row;
      last_col_ = clicked_col;
      // Check again: the move may have ended the game.
      if (game_->state() != Game::State::kPlaying) {
        finished_record_ = game_->FinalizeRecord();
        state_ = AppState::kGameOver;
        return;
      }
    }
  }

  ImGui::Spacing();

  // ── Buttons ────────────────────────────────────────────────────────────
  if (config_.undo_enabled()) {
    if (ImGui::Button("Undo", {100, 32})) {
      game_->Undo();
      // After undo, re-derive the last move from the record.
      const auto& moves = game_->board();
      (void)moves;
      if (!game_->board().IsFull()) {
        // Re-read last move from the internal record is not exposed, so
        // we just clear the indicator on undo.
        last_row_ = last_col_ = -1;
      }
    }
    ImGui::SameLine();
  }
  if (ImGui::Button("Forfeit", {100, 32})) {
    // Current player forfeits; opponent wins.
    // Force a timeout to trigger the same game-over path.
    finished_record_ = game_->FinalizeRecord();
    state_ = AppState::kGameOver;
  }
}

// ---------------------------------------------------------------------------
// Game over
// ---------------------------------------------------------------------------

void App::DrawGameOver() {
  // Show the final board (read-only).
  ImGui::SetWindowFontScale(1.15f);

  // Result banner.
  if (game_->winner().has_value()) {
    const char* name =
        (*game_->winner() == Player::kBlack) ? "Black" : "White";
    if (game_->state() == Game::State::kTimeout)
      ImGui::Text("%s wins by timeout!", name);
    else
      ImGui::Text("%s wins!", name);
  } else {
    ImGui::TextUnformatted("Draw — the board is full!");
  }
  ImGui::SetWindowFontScale(1.0f);
  ImGui::Spacing();

  // Board.
  float avail_x = ImGui::GetContentRegionAvail().x;
  float offset_x = (avail_x - Grid::kCanvasSize) * 0.5f;
  if (offset_x > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
  Grid::DrawBoardReadOnly(game_->board(), last_row_, last_col_);

  ImGui::Spacing();

  // ── Save dialog ────────────────────────────────────────────────────────
  if (!save_attempted_) {
    ImGui::TextUnformatted("Save game record?");
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
      save_attempted_ = true;
      // Build default path.
      std::time_t t = std::time(nullptr);
      std::tm tm_buf;
      localtime_r(&t, &tm_buf);
      std::ostringstream ss;
      ss << "saves/" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S") << ".gom";
      std::string default_path = ss.str();
      std::snprintf(save_path_buf_, sizeof(save_path_buf_), "%s",
                    default_path.c_str());
    }
    ImGui::SameLine();
    if (ImGui::Button("Skip")) {
      save_attempted_ = true;
      save_path_buf_[0] = '\0';
      save_status_msg_ = "Not saved.";
    }
  } else if (save_path_buf_[0] != '\0' && save_status_msg_.empty()) {
    ImGui::SetNextItemWidth(400.0f);
    ImGui::InputText("File path", save_path_buf_, sizeof(save_path_buf_));
    ImGui::SameLine();
    if (ImGui::Button("Confirm")) {
      std::string path(save_path_buf_);
      // Ensure parent directory exists.
      {
        std::filesystem::path p(path);
        if (p.has_parent_path()) {
          std::error_code ec;
          std::filesystem::create_directories(p.parent_path(), ec);
        }
      }
      if (finished_record_.has_value() &&
          FileManager::Save(*finished_record_, path)) {
        save_status_msg_ = "Saved to: " + path;
      } else {
        save_status_msg_ = "Failed to save!";
      }
      save_path_buf_[0] = '\0';
    }
  } else if (!save_status_msg_.empty()) {
    ImGui::TextUnformatted(save_status_msg_.c_str());
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button("New Game", {130, 34})) {
    game_->Start();
    last_row_ = last_col_ = -1;
    finished_record_.reset();
    save_attempted_ = false;
    save_status_msg_.clear();
    state_ = AppState::kPlaying;
  }
  ImGui::SameLine();
  if (ImGui::Button("Main Menu", {130, 34})) {
    game_.reset();
    finished_record_.reset();
    state_ = AppState::kMainMenu;
  }
}

// ---------------------------------------------------------------------------
// Replay
// ---------------------------------------------------------------------------

void App::DrawReplay() {
  ImGui::TextUnformatted("Replay — enter path to a .gom file:");
  ImGui::SetNextItemWidth(500.0f);
  bool enter_pressed =
      ImGui::InputText("##path", replay_path_buf_, sizeof(replay_path_buf_),
                       ImGuiInputTextFlags_EnterReturnsTrue);

  ImGui::SameLine();
  bool load_clicked = ImGui::Button("Load");

  if (!replay_error_msg_.empty()) {
    ImGui::TextColored({1.0f, 0.4f, 0.4f, 1.0f}, "%s",
                       replay_error_msg_.c_str());
  }

  if (enter_pressed || load_clicked) {
    replay_error_msg_.clear();
    if (replay_.Load(replay_path_buf_)) {
      replay_cursor_ = 0;
    } else {
      replay_error_msg_ =
          std::string("Failed to load '") + replay_path_buf_ + "'.";
    }
  }

  if (replay_.total_moves() > 0) {
    ImGui::Spacing();
    // Navigation controls.
    bool prev =
        ImGui::Button("< Prev", {90, 30}) ||
        ImGui::IsKeyPressed(ImGuiKey_LeftArrow) ||
        ImGui::IsKeyPressed(ImGuiKey_P);
    ImGui::SameLine();
    bool next =
        ImGui::Button("Next >", {90, 30}) ||
        ImGui::IsKeyPressed(ImGuiKey_RightArrow) ||
        ImGui::IsKeyPressed(ImGuiKey_N);
    ImGui::SameLine();
    ImGui::Text("Step %d / %d", replay_cursor_, replay_.total_moves());

    if (prev && replay_cursor_ > 0) --replay_cursor_;
    if (next && replay_cursor_ < replay_.total_moves()) ++replay_cursor_;

    // Metadata.
    const GameMetadata& meta = replay_.record().metadata();
    ImGui::Text("Date: %s  |  Moves: %d  |  Winner: %s", meta.date.c_str(),
                meta.total_moves,
                meta.winner.has_value()
                    ? (*meta.winner == Player::kBlack ? "Black" : "White")
                    : "Draw");

    ImGui::Spacing();

    // Board at current step.
    Board b = replay_.BoardAtStep(replay_cursor_);
    // Last move indicator: the stone placed at cursor step.
    int lr = -1, lc = -1;
    if (replay_cursor_ > 0) {
      const auto& mv = replay_.record().moves()[replay_cursor_ - 1];
      lr = mv.row;
      lc = mv.col;
    }

    float avail_x = ImGui::GetContentRegionAvail().x;
    float offset_x = (avail_x - Grid::kCanvasSize) * 0.5f;
    if (offset_x > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
    Grid::DrawBoardReadOnly(b, lr, lc);
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button("Back to Menu", {140, 32})) {
    replay_ = ReplayController();
    replay_cursor_ = 0;
    replay_path_buf_[0] = '\0';
    replay_error_msg_.clear();
    state_ = AppState::kMainMenu;
  }
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

void App::DrawSettings() {
  ImGui::SetWindowFontScale(1.2f);
  ImGui::TextUnformatted("Settings");
  ImGui::SetWindowFontScale(1.0f);
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::Checkbox("Enable undo", &settings_undo_enabled_);
  ImGui::Spacing();
  ImGui::Checkbox("Enable per-move time limit", &settings_time_enabled_);
  if (settings_time_enabled_) {
    ImGui::SetNextItemWidth(120.0f);
    ImGui::InputInt("Time limit (seconds)", &settings_time_seconds_);
    if (settings_time_seconds_ < 5) settings_time_seconds_ = 5;
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::Button("Save & Back", {130, 34})) {
    config_.set_undo_enabled(settings_undo_enabled_);
    config_.set_time_limit_enabled(settings_time_enabled_);
    config_.set_time_limit_seconds(settings_time_seconds_);
    config_.Save();
    state_ = AppState::kMainMenu;
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel", {100, 34})) {
    state_ = AppState::kMainMenu;
  }
}
