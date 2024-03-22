#include "rain.hpp"
#include <Arduino.h>

#include "commands.hpp"
#include "date.hpp"
#include "settings.hpp"

#define SAMPLES 60

volatile uint32_t rain_ticks = 0;
volatile uint8_t rain_hour_samples[SAMPLES] = {0};

void onRainTick() {
  static uint32_t last_tick(0);
  uint32_t tick = micros();

  if (tick - last_tick < 50'000) {
    return;
  }
  last_tick = tick;
  rain_ticks += 1;
  rain_hour_samples[0] += 1;

  commands::broadcast_notify(ID_RAIN_DAY);
  commands::broadcast_notify(ID_RAIN_HOUR);
}

void onRainUpdate() {
  for (int i = SAMPLES - 1; i > 0; --i) {
    rain_hour_samples[i] = rain_hour_samples[i - 1];
  }
  rain_hour_samples[0] = 0;

  settings::data_t &dat = settings::data();
  if (dat.rain_day != rain_ticks) {
    dat.rain_day = rain_ticks;
    settings::save_data();
  }
}

void rain::setup() {
  pinMode(RAIN, INPUT_PULLUP);

  attachInterrupt(RAIN, onRainTick, FALLING);

  settings::data_t &dat = settings::data();

  rain_ticks = dat.rain_day;
}

void rain::update() {
  static uint32_t last_update(0);
  static uint32_t last_check(0);
  static uint32_t last_day(0);

  uint32_t tick = millis();
  if (tick - last_update > 60000) {
    last_update = tick;
    onRainUpdate();
  }
  if (tick - last_check > 1000) {
    last_check = tick;
    uint32_t day = date::get_day();
    if (day != last_day) {
      last_day = day;
      reset_day();
      settings::data_t &dat = settings::data();
      dat.rain_day = 0;
      dat.day = day;
      settings::save_data();
    }
  }
}

float rain::get_hour() {
  uint32_t sum = 0;
  for (int i = 0; i < SAMPLES; ++i) {
    sum += rain_hour_samples[i];
  }
  return ((float)sum) / 100;
}

float rain::get_day() { return ((float)rain_ticks) / 100; }

void rain::reset_day() { rain_ticks = 0; }
