#pragma once

#include <scode.h>

namespace comms {

/**
 * Setup the Communications system
 */
void setup();

/**
 * Read the next code from the stream
 *
 * @param code code to populate
 *
 * @return any errors (SCODE_ERROR_X) or 0
 */
int read(code_t *code);
/**
 * Write a code to the stream
 *
 * @param code code to write
 * @param binary whether to write the code in the binary format
 */
void write(const code_t &code, bool binary = true);

/**
 * Write a finishing code
 *
 * @param binary whether to write the code in binary format
 */
void ok(param_t &&code, bool binary = true);

void err(uint8_t error, param_t &&code, bool binary = true);

} // namespace comms

#define COMMS_SERIAL Serial1
