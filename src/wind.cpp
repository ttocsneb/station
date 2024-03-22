#include "wind.hpp"
#include <Arduino.h>

#include "commands.hpp"
#include "settings.hpp"

using wind::wind_t;

#define SECONDS 2
#define SAMPLES_PER_SECOND 10
#define SAMPLES (SECONDS * SAMPLES_PER_SECOND)

#define SAMPLES_PER_MINUTE 60
#define AVG_SAMPLES (10 * SAMPLES_PER_MINUTE)

volatile uint16_t wind_avg_samples[AVG_SAMPLES] = {0};
volatile float wind_gust_samples[AVG_SAMPLES] = {0.0};
volatile uint8_t wind_dir_samples[AVG_SAMPLES] = {0};

volatile uint32_t wind_ticks = 0;
volatile uint32_t wind_samples[SAMPLES] = {0};

/**
 * Called once a second to keep a log of the wind average/gust
 */
void onWindAvgUpdate() {
  for (int i = AVG_SAMPLES - 1; i > 0; --i) {
    wind_avg_samples[i] = wind_avg_samples[i - 1];
    wind_gust_samples[i] = wind_gust_samples[i - 1];
    wind_dir_samples[i] = wind_dir_samples[i - 1];
  }
  wind_avg_samples[0] = 0;
  wind_gust_samples[0] = wind::get_speed();
  wind_dir_samples[0] = wind::get_heading() / 45;

  commands::broadcast_notify(ID_AVG_2M_WIND_SPEED);
  commands::broadcast_notify(ID_AVG_2M_WIND_DIR);
  commands::broadcast_notify(ID_GUST_2M_WIND_SPEED);
  commands::broadcast_notify(ID_GUST_2M_WIND_DIR);
  commands::broadcast_notify(ID_AVG_10M_WIND_SPEED);
  commands::broadcast_notify(ID_AVG_10M_WIND_DIR);
  commands::broadcast_notify(ID_GUST_10M_WIND_SPEED);
  commands::broadcast_notify(ID_GUST_10M_WIND_DIR);
}

/**
 * Called 10 times per second to keep an acurate log of the last second of wind.
 */
void onWindUpdate() {
  static uint8_t count(0);

  for (int i = SAMPLES - 1; i > 0; --i) {
    wind_samples[i] = wind_samples[i - 1];
  }
  wind_samples[0] = wind_ticks;
  wind_ticks = 0;

  count += 1;
  if (count >= (60 * SAMPLES_PER_SECOND / SAMPLES_PER_MINUTE)) {
    onWindAvgUpdate();
    count = 0;
  }

  commands::broadcast_notify(ID_WIND_DIR);
  commands::broadcast_notify(ID_WIND_SPEED);
}

/**
 * The anemometer update function gets called on every 180°. This will increment
 * the `ticks` variable accordingly.
 */
void onAnemUpdate() {
  static uint32_t last_tick(0);
  uint32_t tick = micros();

  if (tick - last_tick < 1000) {
    return;
  }
  last_tick = tick;
  wind_ticks += 1;
  wind_avg_samples[0] += 1;
}

void wind::setup() {
  pinMode(ANEM, INPUT_PULLUP);
  pinMode(VANE, INPUT);

  attachInterrupt(ANEM, onAnemUpdate, FALLING);

  int heading = get_heading() / 45;

  for (int i = 0; i < AVG_SAMPLES; ++i) {
    wind_dir_samples[i] = heading;
  }
}

void wind::update() {
  static uint32_t last_update(0);
  uint32_t tick = millis();

  if (tick - last_update > (1000 / SAMPLES_PER_SECOND)) {
    last_update = tick;
    onWindUpdate();
  }
}

float tps_to_speed(float tps) {
  float k = settings::settings().anemometer_k;
  const float MPS_TO_KMPH = 3.6;

  // 2 ticks per rotation
  float val = tps / 2.0;       // Convert to rps
  val *= 0.0889 * 2 * 3.14159; // Circumference of anemometer
  val *= MPS_TO_KMPH * k;      // Convert to kmph and multiply by a constant
  /* val *= 0.6213712;            // Convert to mph */
  return val;
}

float wind::get_speed() {
  uint32_t sum = 0;
  for (int i = 0; i < SAMPLES; ++i) {
    sum += wind_samples[i];
  }

  return tps_to_speed(((float)sum) / SECONDS);
}

wind_t wind::get_avg_2m() {
  uint32_t wind_sum = 0;
  uint32_t dir_sum = 0;
  for (int i = 0; i < 2 * SAMPLES_PER_MINUTE; ++i) {
    wind_sum += wind_avg_samples[i];
    dir_sum += wind_dir_samples[i];
  }

  wind_t wind;
  wind.speed = tps_to_speed(((float)wind_sum) / (2 * SAMPLES_PER_MINUTE));
  wind.direction = dir_sum * 45 / (2 * SAMPLES_PER_MINUTE);

  return wind;
}

wind_t wind::get_avg_10m() {
  uint32_t wind_sum = 0;
  uint32_t dir_sum = 0;
  for (int i = 0; i < 10 * SAMPLES_PER_MINUTE; ++i) {
    wind_sum += wind_avg_samples[i];
    dir_sum += wind_dir_samples[i];
  }

  wind_t wind;
  wind.speed = tps_to_speed(((float)wind_sum) / (10 * SAMPLES_PER_MINUTE));
  wind.direction = dir_sum * 45 / (10 * SAMPLES_PER_MINUTE);

  return wind;
}

wind_t wind::get_gust_2m() {
  float max = 0;
  int dir = 0;
  for (int i = 0; i < 2 * SAMPLES_PER_MINUTE; ++i) {
    if (max < wind_gust_samples[i]) {
      max = wind_gust_samples[i];
      dir = wind_dir_samples[i];
    }
  }

  wind_t wind;
  wind.speed = max;
  wind.direction = dir * 45;
  return wind;
}

wind_t wind::get_gust_10m() {
  float max = 0;
  int dir = 0;
  for (int i = 0; i < 10 * SAMPLES_PER_MINUTE; ++i) {
    if (max < wind_gust_samples[i]) {
      max = wind_gust_samples[i];
      dir = wind_dir_samples[i];
    }
  }

  wind_t wind;
  wind.speed = max;
  wind.direction = dir * 45;
  return wind;
}

int wind::get_heading() {
  static int last_heading(0);

  // MAX * R2 / (R1 + R2) = value
  // R2 = 5600
  // MAX = 1023
  int value = analogRead(VANE);

  // There are 8 directions that this sensor can sense. If none of the sensors
  // are detected, then the last heading that was recorded should be good enough

  // Unknown (0)
  if (value < 23)
    return last_heading;

  // West 120k (45)
  if (value < 63)
    last_heading = 270;
  // North West 65k (81)
  else if (value < 115)
    last_heading = 315;
  // North 33k (148)
  else if (value < 207)
    last_heading = 0;
  // South West 16k (265)
  else if (value < 342)
    last_heading = 225;
  // North East 8k1 (418)
  else if (value < 511)
    last_heading = 45;
  // South 3k9 (603)
  else if (value < 651)
    last_heading = 180;
  // South East 2k6 (698)
  else if (value < 778)
    last_heading = 135;
  // East 1k (858)
  else
    last_heading = 90;

  return last_heading;
}
