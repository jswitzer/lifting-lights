#include "base.h"
#include "shared.h"
#include <Arduino.h>

b_context_t b_ctx;

void setup() {
  b_init(&b_ctx, S_POS_BASE);
  b_setup(&b_ctx);
}
void loop() {
  b_loop(&b_ctx);
}

