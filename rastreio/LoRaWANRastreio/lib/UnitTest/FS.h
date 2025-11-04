#pragma once

// Minimal stub for FS.h so PositionQueueLittleFS can include it in native tests.
// The real File/FS APIs are provided by LittleFS mock (MockLittleFS.h).
#include <stdlib.h>


#define FILE_READ   "r"
#define FILE_WRITE  "w"
#define FILE_APPEND "a"

enum SeekMode {
  SeekSet = 0,
  SeekCur = 1,
  SeekEnd = 2
};