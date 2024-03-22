#include "sensor.hpp"
#include <Arduino.h>

#include "Adafruit_MS8607.h"
#include "Adafruit_SI1145.h"

using sensor::MS8607_t;

Adafruit_SI1145 si1145;
Adafruit_MS8607 ms8607;
bool is_ms8607_enabled = false;
bool is_si1145_enabled = false;

MS8607_t sensor::get_ms8607() {
  if (!is_ms8607_enabled) {
    return {0};
  }
  sensors_event_t temp, pressure, humidity;
  ms8607.getEvent(&pressure, &temp, &humidity);

  MS8607_t event;
  event.pressure = pressure.pressure;
  event.temperature = temp.temperature;
  event.relative_humidity = humidity.relative_humidity;
  return event;
}

float sensor::get_si1145() { return si1145.readUV(); }

bool sensor::is_ms8607_working() { return is_ms8607_enabled; }
bool sensor::is_si1145_working() { return is_si1145_enabled; }

void sensor::setup() {
  if (ms8607.begin()) {
    is_ms8607_enabled = true;

    ms8607.setHumidityResolution(MS8607_HUMIDITY_RESOLUTION_OSR_8b);
    ms8607.setPressureResolution(MS8607_PRESSURE_RESOLUTION_OSR_4096);
  }

  if (si1145.begin()) {
    is_si1145_enabled = true;
  }
}
