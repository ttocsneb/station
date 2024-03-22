#include "date.hpp"
#include <Arduino.h>

#define MS_PER_DAY (1000 * 3600 * 24)

uint32_t d;
uint32_t t;
uint32_t ms;
uint64_t s;

void date::set_day(uint32_t day) { d = day; }
void date::set_time(uint32_t time) {
  t = time;
  ms = millis();
}

void update_time() {
  uint32_t now = millis();
  uint32_t elapsed;
  if (ms > now) {
    // Overflow occurred
    elapsed = (UINT32_MAX - ms) + now;
  } else {
    elapsed = (now - ms);
  }
  ms = now;
  t += elapsed;
  s += elapsed;
  while (t > MS_PER_DAY) {
    d += 1;
    t -= MS_PER_DAY;
  }
}

uint32_t date::get_day() {
  update_time();
  return d;
}
uint32_t date::get_time() {
  update_time();
  return t;
}

uint32_t date::get_uptime() {
  update_time();
  return s / 1000;
}
