#pragma once

#include <cstdint>

namespace settings {

struct settings_t {
  uint32_t min_broadcast_interval;
  uint64_t sensor_broadcast_bitmap;
  float anemometer_k;
  bool default_binary;
};

struct data_t {
  uint32_t day;
  uint32_t rain_day;
};

/**
 * Reset settings to defaults
 */
void reset_settings();

/**
 * Save the settings into EEPROM.
 */
void save_settings();

/**
 * Save the data into EEPROM.
 */
void save_data();

/**
 * Load the settings from EEPROM
 */
void load_settings();

/**
 * Load the settings from EEPROM.
 *
 * This will return a cached global settings struct, so any changes are
 * effective instantly.
 *
 * @return settings
 */
settings_t &settings();
/**
 * Load the data from EEPROM.
 *
 * This will return a cached global data struct, so any changes are
 * effective instantly.
 *
 * @return data
 */
data_t &data();

} // namespace settings

#define SENSOR_BIT(x) (1 << (x))
