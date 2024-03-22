#pragma once

namespace sensor {

typedef struct {
  // temperature is in degrees centigrade (Celsius)
  float temperature;
  // pressure in hectopascal (hPa)
  float pressure;
  // relative humidity in percent
  float relative_humidity;
} MS8607_t;

/**
 * Get the temperature, pressure, and humidity from the MS8607 sensor
 */
MS8607_t get_ms8607();
/**
 * Get the UV index from the SI1145 sensor
 */
float get_si1145();

/**
 * Check whether the ms8607 sensor is working.
 */
bool is_ms8607_working();

/**
 * Check whether the SI1145 sensor is working.
 */
bool is_si1145_working();

/**
 * Setup the sensors
 */
void setup();

} // namespace sensor
