#include "settings.hpp"
#include <Arduino.h>

#include "commands.hpp"

#include <EEPROM.h>

using settings::data_t;
using settings::settings_t;

#define VERSION_ADDR 0x100
#define SETTINGS_ADDR (VERSION_ADDR + 1)
#define DATA_ADDR (SETTINGS_ADDR + sizeof(settings_t))
// This should only change if the structure of the Settings_t changes
#define SETTINGS_VERSION 4

settings_t sett = {0};
data_t dat = {0};
uint8_t version = 0;

void settings::reset_settings() {
  version = SETTINGS_VERSION;
  sett.min_broadcast_interval = 100;
  sett.sensor_broadcast_bitmap =
      SENSOR_BIT(ID_WIND_DIR) | SENSOR_BIT(ID_AVG_2M_WIND_DIR) |
      SENSOR_BIT(ID_GUST_2M_WIND_DIR) | SENSOR_BIT(ID_AVG_10M_WIND_DIR) |
      SENSOR_BIT(ID_GUST_10M_WIND_DIR) | SENSOR_BIT(ID_RAIN_HOUR) |
      SENSOR_BIT(ID_RAIN_DAY);
  sett.anemometer_k = 2.0;
  sett.default_binary = true;

  dat.day = 0;
  dat.rain_day = 0;
}

void settings::save_settings() {
  EEPROM.update(VERSION_ADDR, SETTINGS_VERSION);
  EEPROM.put(SETTINGS_ADDR, sett);
}

void settings::save_data() {
  EEPROM.update(VERSION_ADDR, SETTINGS_VERSION);
  EEPROM.put(DATA_ADDR, dat);
}

void settings::load_settings() {
  version = EEPROM.read(VERSION_ADDR);
  if (version != SETTINGS_VERSION) {
    settings::reset_settings();
  } else {
    EEPROM.get(SETTINGS_ADDR, sett);
  }
}

void load() {
  if (version == SETTINGS_VERSION) {
    return;
  }

  version = EEPROM.read(VERSION_ADDR);
  if (version != SETTINGS_VERSION) {
    settings::reset_settings();
  } else {
    EEPROM.get(SETTINGS_ADDR, sett);
    EEPROM.get(DATA_ADDR, dat);
  }
}

settings_t &settings::settings() {
  load();
  return sett;
}

data_t &settings::data() {
  load();
  return dat;
}
