#include "configuration.h"

#include <fstream>
#include <sstream>
#include <string>

Configuration::Configuration() {
  std::ifstream file(kConfigPath);
  if (!file.is_open()) return;

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;
    auto eq = line.find('=');
    if (eq == std::string::npos) continue;
    std::string key = line.substr(0, eq);
    std::string val = line.substr(eq + 1);
    // Trim whitespace.
    auto trim = [](std::string& s) {
      while (!s.empty() && (s.front() == ' ' || s.front() == '\t'))
        s.erase(s.begin());
      while (!s.empty() &&
             (s.back() == ' ' || s.back() == '\t' || s.back() == '\r'))
        s.pop_back();
    };
    trim(key);
    trim(val);
    if (key == "undo_enabled") {
      undo_enabled_ = (val == "true");
    } else if (key == "time_limit_enabled") {
      time_limit_enabled_ = (val == "true");
    } else if (key == "time_limit_seconds") {
      try {
        time_limit_seconds_ = std::stoi(val);
      } catch (...) {
      }
    }
  }
}

void Configuration::Save() const {
  std::ofstream file(kConfigPath);
  if (!file.is_open()) return;
  file << "# Gomoku configuration\n";
  file << "undo_enabled = " << (undo_enabled_ ? "true" : "false") << "\n";
  file << "time_limit_enabled = " << (time_limit_enabled_ ? "true" : "false")
       << "\n";
  file << "time_limit_seconds = " << time_limit_seconds_ << "\n";
}

bool Configuration::undo_enabled() const { return undo_enabled_; }
bool Configuration::time_limit_enabled() const { return time_limit_enabled_; }
int Configuration::time_limit_seconds() const { return time_limit_seconds_; }
void Configuration::set_undo_enabled(bool value) { undo_enabled_ = value; }
void Configuration::set_time_limit_enabled(bool value) {
  time_limit_enabled_ = value;
}
void Configuration::set_time_limit_seconds(int seconds) {
  time_limit_seconds_ = seconds;
}
