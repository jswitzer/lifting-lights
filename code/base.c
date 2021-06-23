#ifdef __cplusplus
#include <Arduino.h>
extern "C" {
#endif

#include "shared.h"
#include "base.h"

int b_init(b_context_t * ctx, unsigned char position) {
  ctx->b_ctx_position = position;
  b_state_t * state = &(ctx->b_ctx_state);
  s_ctx_init(&(ctx->b_ctx_shared), position);

  state->bs_lights[B_LGT_LEFT_BAD] = (s_light_t) {S_LGTTYPE_LED, PIN_B_LEFT_BAD};
  state->bs_lights[B_LGT_LEFT_GOOD] = (s_light_t) {S_LGTTYPE_LED, PIN_B_LEFT_GOOD};
  state->bs_lights[B_LGT_HEAD_BAD] = (s_light_t) {S_LGTTYPE_LED, PIN_B_HEAD_BAD};
  state->bs_lights[B_LGT_HEAD_GOOD] = (s_light_t) {S_LGTTYPE_LED, PIN_B_HEAD_GOOD};
  state->bs_lights[B_LGT_RIGHT_BAD] = (s_light_t) {S_LGTTYPE_LED, PIN_B_RIGHT_BAD};
  state->bs_lights[B_LGT_RIGHT_GOOD] = (s_light_t) {S_LGTTYPE_LED, PIN_B_RIGHT_GOOD};

  return 0;
}

int b_setup(b_context_t * ctx) {
  // setup networking lib
  return 0;
}

static void do_reset(b_context_t * ctx) {
  b_state_t * state = &(ctx->b_ctx_state);
  s_context_t *sctx = &(ctx->b_ctx_shared);

  s_set_light(sctx, &(state->bs_lights[B_LGT_LEFT_GOOD]), S_COLOR_OFF);
  s_set_light(sctx, &(state->bs_lights[B_LGT_LEFT_BAD]), S_COLOR_OFF);
  s_set_light(sctx, &(state->bs_lights[B_LGT_RIGHT_GOOD]), S_COLOR_OFF);
  s_set_light(sctx, &(state->bs_lights[B_LGT_RIGHT_BAD]), S_COLOR_OFF);
  s_set_light(sctx, &(state->bs_lights[B_LGT_HEAD_GOOD]), S_COLOR_OFF);
  s_set_light(sctx, &(state->bs_lights[B_LGT_HEAD_BAD]), S_COLOR_OFF);

  s_ctx_timer_cancel(sctx, S_TIMER_RESET);

}

static void process_event(b_context_t * ctx, s_event_t* evt) {
  b_state_t * state = &(ctx->b_ctx_state);
  unsigned char from = evt->s_evt_src;
  unsigned char type = evt->s_evt_type;
  s_context_t *sctx = &(ctx->b_ctx_shared);

  if (type == S_EVT_GOOD) {
    if (from == S_POS_LEFT) {
      state->bs_left = S_EVT_GOOD;
      s_set_light(sctx, &(state->bs_lights[B_LGT_LEFT_GOOD]), S_COLOR_ON);
      s_ctx_timer_set(sctx, S_TIMER_RESET, 10000, S_EVT_RESET);
    }
    if (from == S_POS_RIGHT) {
      state->bs_right = S_EVT_GOOD;
      s_set_light(sctx, &(state->bs_lights[B_LGT_RIGHT_GOOD]), S_COLOR_ON);
      s_ctx_timer_set(sctx, S_TIMER_RESET, 10000, S_EVT_RESET);
    }
    if (from == S_POS_HEAD) {
      state->bs_head = S_EVT_GOOD;
      s_set_light(sctx, &(state->bs_lights[B_LGT_HEAD_GOOD]), S_COLOR_ON);
      s_ctx_timer_set(sctx, S_TIMER_RESET, 10000, S_EVT_RESET);
    }
  }

  if (type == S_EVT_BAD) {
    if (from == S_POS_LEFT) {
      state->bs_left = S_EVT_BAD;
      s_set_light(sctx, &(state->bs_lights[B_LGT_LEFT_BAD]), S_COLOR_ON);
      s_ctx_timer_set(sctx, S_TIMER_RESET, 10000, S_EVT_RESET);
    }
    if (from == S_POS_RIGHT) {
      state->bs_right = S_EVT_BAD;
      s_set_light(sctx, &(state->bs_lights[B_LGT_RIGHT_BAD]), S_COLOR_ON);
      s_ctx_timer_set(sctx, S_TIMER_RESET, 10000, S_EVT_RESET);
    }
    if (from == S_POS_HEAD) {
      state->bs_head = S_EVT_BAD;
      s_set_light(sctx, &(state->bs_lights[B_LGT_HEAD_BAD]), S_COLOR_ON);
      s_ctx_timer_set(sctx, S_TIMER_RESET, 10000, S_EVT_RESET);
    }
  }

  if (type == S_EVT_TIMER) {
    if (evt->s_evt_data1 == S_EVT_RESET) {
      do_reset(ctx);
    }
  }

  if (type == S_EVT_RESET) 
  {
    if (from == S_POS_HEAD) 
    {
      do_reset(ctx);
    }
    else {
      //printf("WARNING: base got reset cmd from non-head - disregarding\n");
    }
  }
}

int b_test(void) {
  b_context_t ctx;
  b_init(&ctx, S_POS_BASE);
  //s_command_t cmd = {0};
  //process_rx_command(&(ctx.b_ctx_state), &cmd);
  return 0;
}

int b_loop(b_context_t * ctx) {
  s_context_t *sctx = &(ctx->b_ctx_shared);
  //b_state_t  *bstat = &(ctx->b_ctx_state);
  // detect button presses and produce button press events


  // receive and parse things from the xbee into incoming network events
  s_ctx_receive(sctx);

  // service the timers
  s_ctx_timers(sctx);

  while (sctx->s_ctx_oldest) {
    s_event_t * evt = sctx->s_ctx_oldest;
    process_event(ctx, evt);
    s_ctx_remove_event(sctx, evt);
  }

  return 0;
}

#ifdef __cplusplus
}
#endif
