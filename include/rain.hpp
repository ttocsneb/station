#pragma once

namespace rain {

void setup();

void update();

/**
 * Get the rainfall in the past hour
 *
 * @return rain inches
 */
float get_hour();
/**
 * Get the rainfall for the day
 *
 * @return rain inches
 */
float get_day();
/**
 * Reset the daily rainfall meter
 */
void reset_day();

} // namespace rain

#define RAIN 2
