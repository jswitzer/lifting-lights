#ifdef __cplusplus
#include <Arduino.h>
extern "C" {
#endif

#include "shared.h"
#include "remote.h"

int r_test(void) {
  return 1;
}

int r_setup(r_context_t * ctx) {
  // figure out which thing I am
  // initalize libraries
  return 0;
}

static void process_event(r_context_t * ctx, s_event_t* evt) {
  unsigned char from = evt->s_evt_src;
  unsigned char type = evt->s_evt_type;
  s_context_t * sctx = &(ctx->r_ctx_shared);
  s_event_t txevt;
  txevt.s_evt_src = sctx->s_ctx_position;
  txevt.s_evt_dest = S_POS_BASE;

#ifndef __cplusplus
  printf("Processing event from:%d type:%d\n", from, type);
#endif
  if (type == S_EVT_BUTTON) {
    if (evt->s_evt_data1 == R_BTN_GOOD) {
      txevt.s_evt_type = S_EVT_GOOD;
      s_ctx_transmit_one(sctx, &txevt);
    }
    if (evt->s_evt_data1 == R_BTN_BAD) {
      txevt.s_evt_type = S_EVT_BAD;
      s_ctx_transmit_one(sctx, &txevt);
    }
    if ((evt->s_evt_data1 == R_BTN_RESET)) {
      txevt.s_evt_type = S_EVT_RESET;
      s_ctx_transmit_one(sctx, &txevt);
    }
  }
}
 
int r_loop(r_context_t * ctx) {

  int i = 0;
  s_context_t * sctx = &(ctx->r_ctx_shared);
  r_state_t * rstat = &(ctx->r_ctx_state);
  while (i < R_NBUTTONS) {
#ifndef __cplusplus
  //printf("Scanning button %d\n", i);
#endif

    s_ctx_scan_button(sctx, &(rstat->rs_buttons[i]));
    i++;
  }

  s_ctx_receive(sctx);
  while (sctx->s_ctx_oldest) {
    s_event_t * evt = sctx->s_ctx_oldest;
    process_event(ctx, evt);
    s_ctx_remove_event(sctx, evt);
  }
  //process_rx_command(&(ctx.b_ctx_state), &cmd);

  // detect button presses and produce button press events
  // receive and parse things from the xbee into incoming network events

  // process events 
  // - process those events are destined for us (Which might produce more events)
  // - send those events that are destined for others
  // - clear all remaining events
  
  // set the output pins so that they match (AKA turn the lights on)


  return 0;
}

int r_init(r_context_t * ctx, unsigned char position) {
  s_context_t * sctx = &(ctx->r_ctx_shared);
  r_state_t * rstat = &(ctx->r_ctx_state);
  ctx->r_ctx_position = position;
  s_ctx_init(sctx, position);
  rstat->rs_buttons[R_BTN_OFF] = (s_button_t){R_BTN_OFF, S_BTNTYPE_POSEDGE, PIN_R_PWR, 0};
  rstat->rs_buttons[R_BTN_GOOD] = (s_button_t){R_BTN_GOOD, S_BTNTYPE_POSEDGE, PIN_R_GOOD, 0};
  rstat->rs_buttons[R_BTN_BAD] = (s_button_t){R_BTN_BAD, S_BTNTYPE_POSEDGE, PIN_R_BAD, 0};
  rstat->rs_buttons[R_BTN_RESET] = (s_button_t){R_BTN_RESET, S_BTNTYPE_POSEDGE, PIN_R_RESET, 0};



  return 0;
}

#ifdef __cplusplus
}
#endif
