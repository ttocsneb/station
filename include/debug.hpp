#pragma once

/* #define DEBUG */

#ifdef DEBUG
#define DEBUG_BEGIN()                                                          \
  if (!Serial) {                                                               \
    Serial.begin(115200);                                                      \
  }
#define PRINTLN(...) Serial.println(__VA_ARGS__)
#define PRINT(...) Serial.print(__VA_ARGS__)
#else
#define PRINTLN(...)
#define PRINT(...)
#define DEBUG_BEGIN()
#endif
