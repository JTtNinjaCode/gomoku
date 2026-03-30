#include "configuration.h"

Configuration::Configuration() {}
void Configuration::Save() const {}
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
