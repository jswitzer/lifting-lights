#include "remote.h"
#include "shared.h"
#include <Arduino.h>

r_context_t r_ctx;

void setup() {
  r_init(&r_ctx, S_POS_HEAD);
  r_setup(&r_ctx);
}
void loop() {
  r_loop(&r_ctx);
}

