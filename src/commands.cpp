#include "commands.hpp"
#include <Arduino.h>

#include "comms.hpp"
#include "date.hpp"
#include "rain.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "wind.hpp"

#include "debug.hpp"

volatile uint64_t updated = 0;

class ISensor {
public:
  const uint8_t number;
  float last_value = 0;
  uint32_t last_broadcast = 0;

  ISensor(uint8_t number) : number(number) {}

  virtual void handle_command(const code_t &code) {

    // Find the request string
    String request;
    if (code.params != NULL) {
      for (size_t i = 0; code.params[i].param != 0; ++i) {
        char letter = param_letter(&code.params[i]);
        if (letter == 'R') {
          uint8_t type = param_type(&code.params[i]);
          if (type == PARAM_T_STR) {
            request = String(code.params[i].str).toUpperCase();
            break;
          }
        }
      }
    }

    code_t broadcast = init_code('S', number, 3);
    size_t pos = 0;
    if (request.length() == 0 || request.indexOf('V') != -1) {
      broadcast.params[pos++] = init_param_f32('V', this->value());
    }
    if (request.length() == 0 || request.indexOf('N') != -1) {
      broadcast.params[pos++] = init_param_str('N', this->name());
    }
    if (request.length() == 0 || request.indexOf('U') != -1) {
      broadcast.params[pos++] = init_param_str('U', this->unit());
    }
    broadcast.params[pos] = {0};

    comms::write(broadcast, code_is_binary(&code));

    free_code(&broadcast);
  };

  virtual void broadcast() {
    settings::settings_t &sett = settings::settings();
    if ((SENSOR_BIT(number) & sett.sensor_broadcast_bitmap) == 0) {
      return;
    }
    uint32_t now = millis();
    if (now - last_broadcast < sett.min_broadcast_interval) {
      return;
    }
    updated &= ~(SENSOR_BIT(number));
    float v = value();
    if (v == last_value) {
      return;
    }
    last_value = v;
    code_t broadcast = init_code('S', number, 1);
    broadcast.params[0] = init_param_f32('V', v);

    comms::write(broadcast, sett.default_binary);

    free_code(&broadcast);
  }

  virtual float value() = 0;
  virtual const char *name() const = 0;
  virtual const char *unit() const = 0;
};

class TempSensor : public ISensor {
public:
  TempSensor(uint8_t number) : ISensor(number) {}

  float value() { return sensor::get_ms8607().temperature; }
  const char *name() const { return "temperature"; }
  const char *unit() const { return "C"; }
};
TempSensor temp_sensor(ID_TEMP);

class PressureSensor : public ISensor {
public:
  PressureSensor(uint8_t number) : ISensor(number) {}

  float value() { return sensor::get_ms8607().pressure; }
  const char *name() const { return "pressure"; }
  const char *unit() const { return "hPa"; }
};
PressureSensor pressure_sensor(ID_PRESSURE);

class HumiditySensor : public ISensor {
public:
  HumiditySensor(uint8_t number) : ISensor(number) {}

  float value() { return sensor::get_ms8607().relative_humidity; }
  const char *name() const { return "humidity"; }
  const char *unit() const { return "%"; }
};
HumiditySensor humidity_sensor(ID_HUMIDITY);

class UVSensor : public ISensor {
public:
  UVSensor(uint8_t number) : ISensor(number) {}

  float value() {
    if (sensor::is_si1145_working()) {
      return sensor::get_si1145();
    }
    return 0;
  }
  const char *name() const { return "uv"; }
  const char *unit() const { return "UV Index"; }
};
UVSensor uv_sensor(ID_UV);

class WindspeedSensor : public ISensor {
public:
  WindspeedSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_speed(); }
  const char *name() const { return "wind speed"; }
  const char *unit() const { return "m/s"; }
};
WindspeedSensor windspeed_sensor(ID_WIND_SPEED);
class WinddirSensor : public ISensor {
public:
  WinddirSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_heading(); }
  const char *name() const { return "wind heading"; }
  const char *unit() const { return "deg"; }
};
WinddirSensor winddir_sensor(ID_WIND_DIR);

class WindAvg2MspeedSensor : public ISensor {
public:
  WindAvg2MspeedSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_avg_2m().speed; }
  const char *name() const { return "avg 2m wind speed"; }
  const char *unit() const { return "m/s"; }
};
WindAvg2MspeedSensor windavg2mspeed_sensor(ID_AVG_2M_WIND_SPEED);
class WindAvg2MdirSensor : public ISensor {
public:
  WindAvg2MdirSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_avg_2m().direction; }
  const char *name() const { return "avg 2m wind heading"; }
  const char *unit() const { return "deg"; }
};
WindAvg2MdirSensor windavg2mdir_sensor(ID_AVG_2M_WIND_DIR);

class WindGust2MspeedSensor : public ISensor {
public:
  WindGust2MspeedSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_gust_2m().speed; }
  const char *name() const { return "gust 2m wind speed"; }
  const char *unit() const { return "m/s"; }
};
WindGust2MspeedSensor windgust2mspeed_sensor(ID_GUST_2M_WIND_SPEED);
class WindGust2MdirSensor : public ISensor {
public:
  WindGust2MdirSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_gust_2m().direction; }
  const char *name() const { return "gust 2m wind heading"; }
  const char *unit() const { return "deg"; }
};
WindGust2MdirSensor windgust2mdir_sensor(ID_GUST_2M_WIND_DIR);

class WindAvg10MspeedSensor : public ISensor {
public:
  WindAvg10MspeedSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_avg_10m().speed; }
  const char *name() const { return "avg 10m wind speed"; }
  const char *unit() const { return "m/s"; }
};
WindAvg10MspeedSensor windavg10mspeed_sensor(ID_AVG_10M_WIND_SPEED);
class WindAvg10MdirSensor : public ISensor {
public:
  WindAvg10MdirSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_avg_10m().direction; }
  const char *name() const { return "avg 10m wind heading"; }
  const char *unit() const { return "deg"; }
};
WindAvg10MdirSensor windavg10mdir_sensor(ID_AVG_10M_WIND_DIR);

class WindGust10MspeedSensor : public ISensor {
public:
  WindGust10MspeedSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_gust_10m().speed; }
  const char *name() const { return "gust 10m wind speed"; }
  const char *unit() const { return "m/s"; }
};
WindGust10MspeedSensor windgust10mspeed_sensor(ID_GUST_10M_WIND_SPEED);
class WindGust10MdirSensor : public ISensor {
public:
  WindGust10MdirSensor(uint8_t number) : ISensor(number) {}

  float value() { return wind::get_gust_10m().direction; }
  const char *name() const { return "gust 10m wind heading"; }
  const char *unit() const { return "deg"; }
};
WindGust10MdirSensor windgust10mdir_sensor(ID_GUST_10M_WIND_DIR);

class RainHourSensor : public ISensor {
public:
  RainHourSensor(uint8_t number) : ISensor(number) {}

  float value() { return rain::get_hour(); }
  const char *name() const { return "rain hour"; }
  const char *unit() const { return "in"; }
};
RainHourSensor rainhour_sensor(ID_RAIN_HOUR);

class RainDaySensor : public ISensor {
public:
  RainDaySensor(uint8_t number) : ISensor(number) {}

  float value() { return rain::get_day(); }
  const char *name() const { return "rain day"; }
  const char *unit() const { return "in"; }
};
RainDaySensor rainday_sensor(ID_RAIN_DAY);

ISensor *sensors[] = {&temp_sensor,
                      &pressure_sensor,
                      &humidity_sensor,
                      &uv_sensor,
                      &windspeed_sensor,
                      &winddir_sensor,
                      &windavg2mspeed_sensor,
                      &windavg2mdir_sensor,
                      &windgust2mspeed_sensor,
                      &windgust2mdir_sensor,
                      &windavg10mspeed_sensor,
                      &windavg10mdir_sensor,
                      &windgust10mspeed_sensor,
                      &windgust10mdir_sensor,
                      &rainhour_sensor,
                      &rainday_sensor};

template <typename T, size_t N> char (&ArraySizeHelper(T (&array)[N]))[N];

#define countof(array) (sizeof(ArraySizeHelper(array)))

void commands::sensor(const code_t &code) {
  param_t p = init_param_u8(code_letter(&code), code.number);
  if (code.number >= countof(sensors)) {
    comms::err(2, std::move(p), code_is_binary(&code));
    return;
  }
  sensors[code.number]->handle_command(code);
  comms::ok(std::move(p), code_is_binary(&code));
}

void broadcast(const code_t &code) {
  for (ISensor *sensor : sensors) {
    sensor->handle_command(code);
  }
}

void reset(const code_t &code) {
  settings::save_data();
  SCB_AIRCR = 0x05FA0004;
}

void clock(const code_t &code) {
  int32_t days = -1;
  int32_t ms = -1;

  if (code.params) {
    for (size_t i = 0; code.params[i].param != 0; ++i) {
      char letter = param_letter(&code.params[i]);
      switch (letter) {
      case 'D':
        days = param_cast_i32(&code.params[i]);
        break;
      case 'T':
        ms = param_cast_i32(&code.params[i]);
        break;
      }
    }
  }

  if (days != -1) {
    date::set_day(days);
  }
  if (ms != -1) {
    date::set_time(ms);
  }

  code_t c = init_code('M', 10, 2);
  c.params[0] = init_param_i32('D', static_cast<int32_t>(date::get_day()));
  c.params[1] = init_param_i32('T', static_cast<int32_t>(date::get_time()));

  comms::write(c, code_is_binary(&code));

  free_code(&c);
}

void uptime(const code_t &code) {
  code_t c = init_code('M', 11, 1);
  c.params[0] = init_param_i32('S', static_cast<int32_t>(date::get_uptime()));

  comms::write(c, code_is_binary(&code));

  free_code(&c);
}
void revert_settings(const code_t &code) { settings::load_settings(); }
void reset_settings(const code_t &code) { settings::reset_settings(); }
void save_settings(const code_t &code) { settings::save_settings(); }

void min_broadcast_interval(const code_t &code) {
  uint32_t &mbi = settings::settings().min_broadcast_interval;

  if (code.params != NULL) {
    for (size_t i = 0; code.params[i].param != 0; ++i) {
      char letter = param_letter(&code.params[i]);
      if (letter == 'V') {
        mbi = param_cast_i32(&code.params[i]);

        break;
      }
    }
  }

  code_t c = init_code('M', 101, 1);
  c.params[0] = init_param_i32('V', mbi);

  comms::write(c, code_is_binary(&code));

  free_code(&c);
}

void sensor_broadcast_bitmap(const code_t &code) {
  uint64_t &value = settings::settings().sensor_broadcast_bitmap;
  if (code.params != NULL) {
    for (size_t i = 0; code.params[i].param != 0; ++i) {
      char letter = param_letter(&code.params[i]);
      switch (letter) {
      case 'V':
        value = param_cast_i64(&code.params[i]);
        break;
      case 'E':
        value |= SENSOR_BIT(param_cast_u8(&code.params[i]));
        break;
      case 'D':
        value &= ~SENSOR_BIT(param_cast_u8(&code.params[i]));
        break;
      }
    }
  }

  uint8_t count = 0;
  for (int i = 0; i < 64; ++i) {
    if (SENSOR_BIT(i) & value) {
      count++;
    }
  }

  code_t c = init_code('M', 102, count);
  uint8_t pos = 0;
  for (int i = 0; i < 64; ++i) {
    if (SENSOR_BIT(i) & value) {
      c.params[pos++] = init_param_u8('E', i);
    }
  }

  comms::write(c, code_is_binary(&code));

  free_code(&c);
}

void anemometer_k(const code_t &code) {
  float &k = settings::settings().anemometer_k;

  if (code.params != NULL) {
    for (size_t i = 0; code.params[i].param != 0; ++i) {
      char letter = param_letter(&code.params[i]);
      if (letter == 'V') {
        k = param_cast_f32(&code.params[i]);

        break;
      }
    }
  }

  code_t c = init_code('M', 103, 1);
  c.params[0] = init_param_f32('V', k);

  comms::write(c, code_is_binary(&code));

  free_code(&c);
}

void default_binary(const code_t &code) {
  bool &k = settings::settings().default_binary;

  if (code.params != NULL) {
    for (size_t i = 0; code.params[i].param != 0; ++i) {
      char letter = param_letter(&code.params[i]);
      if (letter == 'V') {
        k = param_cast_u8(&code.params[i]) != 0;

        break;
      }
    }
  }

  code_t c = init_code('M', 104, 1);
  c.params[0] = init_param_u8('V', k);

  comms::write(c, code_is_binary(&code));

  free_code(&c);
}

void commands::options(const code_t &code) {
  param_t p = init_param_u8(code_letter(&code), code.number);
  switch (code.number) {
  case 1:
    broadcast(code);
    break;
  case 10:
    clock(code);
    break;
  case 11:
    uptime(code);
    break;
  case 20:
    reset(code);
    break;
  case 98:
    revert_settings(code);
    break;
  case 99:
    reset_settings(code);
    break;
  case 100:
    save_settings(code);
    break;
  case 101:
    min_broadcast_interval(code);
    break;
  case 102:
    sensor_broadcast_bitmap(code);
    break;
  case 103:
    anemometer_k(code);
    break;
  case 104:
    default_binary(code);
    break;
  default: {
    comms::err(2, std::move(p), code_is_binary(&code));
    return;
  }
  }
  comms::ok(std::move(p), code_is_binary(&code));
}

void commands::broadcast() {
  for (size_t i = 0; i < countof(sensors); ++i) {
    if (updated & SENSOR_BIT(i)) {
      sensors[i]->broadcast();
    }
  }
}

void commands::broadcast_notify(uint8_t sensor) {
  updated |= SENSOR_BIT(sensor);
}
