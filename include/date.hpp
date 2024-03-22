#pragma once

#include <cstdint>

namespace date {

/**
 * Set the current day as days after January 1, 1984
 *
 * @param day days after 1984-1-1
 */
void set_day(uint32_t day);
/**
 * Set the current time of day as milliseconds after midnight
 *
 * @param time ms after midnight
 */
void set_time(uint32_t time);

/**
 * Get the current day as days after January 1, 1984.
 *
 * This will automatically update with millis()
 *
 * @return days after 1984-1-1
 */
uint32_t get_day();
/**
 * Get the current time of day as milliseconds after midnight
 *
 * This will automatically update with millis()
 *
 * @return ms after midnight
 */
uint32_t get_time();

/**
 * Get the number of seconds the device has been online
 *
 * @return seconds after boot
 */
uint32_t get_uptime();

} // namespace date
