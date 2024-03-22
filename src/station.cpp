#include <Arduino.h>

#include "commands.hpp"
#include "comms.hpp"
#include "date.hpp"
#include "debug.hpp"
#include "rain.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "wind.hpp"

using sensor::MS8607_t;
using wind::wind_t;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  date::set_day(settings::data().day);

  comms::setup();
  wind::setup();
  rain::setup();
  sensor::setup();

  DEBUG_BEGIN();

  digitalWrite(LED_BUILTIN, LOW);
}

code_t code = {0};

void loop() {
  static uint32_t last_broadcast(0);
  static settings::settings_t &sett(settings::settings());

  rain::update();
  wind::update();

  uint32_t tick = millis();

  int res;
  while ((res = comms::read(&code)) != SCODE_ERROR_BUFFER) {
    if (res != 0) {
      param_t p = init_param_i8('C', static_cast<int8_t>(res));
      comms::err(1, std::move(p), sett.default_binary);
      continue;
    }

    switch (code_letter(&code)) {
    case 'S':
      commands::sensor(code);
      break;
    case 'M':
      commands::options(code);
      break;
    default: {
      param_t p = init_param_u8(code_letter(&code), code.number);
      comms::err(2, std::move(p), code_is_binary(&code));
    }
    }
  }

  if (tick - last_broadcast > 1000) {
    last_broadcast = tick;

    commands::broadcast_notify(ID_TEMP);
    commands::broadcast_notify(ID_PRESSURE);
    commands::broadcast_notify(ID_HUMIDITY);
    commands::broadcast_notify(ID_UV);
  }

  // TODO: save the rain counter into EEPROM once an hour
  // TODO: restore rain counter after restart

  commands::broadcast();
  delay(25);
}
