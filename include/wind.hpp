#pragma once

namespace wind {

typedef struct {
  float speed;
  int direction;
} wind_t;

/**
 * Setup the wind sensor
 */
void setup();

/**
 * Update the wind sensors
 */
void update();

/**
 * Get the current wind speed
 *
 * @return wind sensor rpm
 */
float get_speed();
wind_t get_avg_2m();
wind_t get_avg_10m();
wind_t get_gust_2m();
wind_t get_gust_10m();

/**
 * Get the heading of the wind.
 *
 * This is the direction from where the wind comes from
 *
 * @return wind heading in degrees
 */
int get_heading();

} // namespace wind

#define VANE A0
#define ANEM 3
