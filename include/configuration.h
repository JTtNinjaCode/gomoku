#pragma once

#include <string>

// Holds all user-configurable game settings.
// Loaded from and persisted to gomoku.cfg on disk.
class Configuration {
 public:
  static constexpr int kDefaultTimeLimitSeconds = 60;

  // Loads settings from kConfigPath if the file exists; otherwise uses
  // defaults.
  Configuration();

  // Writes current settings to kConfigPath.
  void Save() const;

  bool undo_enabled() const;
  bool time_limit_enabled() const;
  int time_limit_seconds() const;

  void set_undo_enabled(bool value);
  void set_time_limit_enabled(bool value);
  void set_time_limit_seconds(int seconds);

 private:
  static constexpr const char* kConfigPath = "gomoku.cfg";

  bool undo_enabled_ = false;
  bool time_limit_enabled_ = false;
  int time_limit_seconds_ = kDefaultTimeLimitSeconds;
};
