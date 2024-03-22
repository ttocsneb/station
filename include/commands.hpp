#pragma once

#include <scode.h>

namespace commands {

/**
 * Handle a sensor command (S*)
 *
 * @param code command to process
 */
void sensor(const code_t &code);

/**
 * Handle an options command (M*)
 *
 * @param code command to process
 */
void options(const code_t &code);

/**
 * Broadcast any pending sensor updates
 */
void broadcast();

/**
 * Notify that the provided sensor should be broadcasted
 *
 * This is designed to be a very light function that can be called in an
 * interrupt.
 *
 * This will not broadcast the sensor until broadcast() is called.
 *
 * @param sensor sensor to notify a change has occurred.
 */
void broadcast_notify(uint8_t sensor);

} // namespace commands

#define ID_TEMP 0
#define ID_PRESSURE 1
#define ID_HUMIDITY 2
#define ID_UV 3
#define ID_WIND_SPEED 4
#define ID_WIND_DIR 5
#define ID_AVG_2M_WIND_SPEED 6
#define ID_AVG_2M_WIND_DIR 7
#define ID_GUST_2M_WIND_SPEED 8
#define ID_GUST_2M_WIND_DIR 9
#define ID_AVG_10M_WIND_SPEED 10
#define ID_AVG_10M_WIND_DIR 11
#define ID_GUST_10M_WIND_SPEED 12
#define ID_GUST_10M_WIND_DIR 13
#define ID_RAIN_HOUR 14
#define ID_RAIN_DAY 15
