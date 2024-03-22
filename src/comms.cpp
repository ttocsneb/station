#include "comms.hpp"

#include <Arduino.h>

#include "debug.hpp"

CodeStream cs(16);

void comms::setup() {
  COMMS_SERIAL.begin(115200);
  while (!COMMS_SERIAL)
    delay(10);
}

int comms::read(code_t *code) {
  while (COMMS_SERIAL.available() > 0) {
    char c = COMMS_SERIAL.read();
    PRINT(c);
    cs.update(&c, 1);
  }

  int res = SCODE_ERROR_EMPTY;
  while (res == SCODE_ERROR_EMPTY) {
    free_code(code);
    res = cs.pop(code);
  }
  return res;
}

void comms::write(const code_t &code, bool binary) {
  char buf[64];
  int res;
  if (!binary) {
    res = code_dump_human(&code, buf, sizeof buf);
  } else {
    res = code_dump_binary(&code, buf, sizeof buf);
  }
  if (res < 0) {
    return;
  }
  COMMS_SERIAL.write(buf, res);
#ifdef DEBUG
  Serial.write(buf, res);
#endif
}

void comms::ok(param_t &&code, bool binary) {
  code_t ok_code = init_code('O', 1, 1);
  ok_code.params[0] = code;
  write(ok_code, binary);
  free_code(&ok_code);
}

void comms::err(uint8_t error, param_t &&code, bool binary) {
  code_t err_code = init_code('E', error, 1);
  err_code.params[0] = code;
  write(err_code, binary);
  free_code(&err_code);
}
