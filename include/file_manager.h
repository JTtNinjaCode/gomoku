#pragma once

#include <string>

#include "game_record.h"

// Static utility class for serialising and deserialising .gom game records.
// Not instantiable.
class FileManager {
 public:
  FileManager() = delete;

  // Writes record to path. Returns true on success.
  static bool Save(const GameRecord& record, const std::string& path);

  // Reads path into *record. Returns true on success.
  static bool Load(const std::string& path, GameRecord* record);
};
