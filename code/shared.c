#include "shared.h"

#define EVENT_POOL_SIZE 50

#ifdef __cplusplus
#define USE_EVENT_POOL 1
#include <Arduino.h>
#include <XBee.h>
XBee xbee = XBee();
extern "C" int xbee_init(s_context_t *);
extern "C" int xbee_tx(s_context_t *, s_event_t *);
extern "C" int xbee_rx(s_context_t *);
s_event_t g_event_pool[EVENT_POOL_SIZE];

static int xbee_init(s_context_t * ctx) {
  return 0;
}

static int xbee_tx(s_context_t* ctx, s_event_t * evt) {
  return 0;
}

static int xbee_rx(s_context_t *ctx) {
  return 0;
}

static int get_pin_state(s_context_t * sctx, unsigned char pin_id) {
  return 0;
}

static int set_pin_on(s_context_t * sctx, unsigned char pin_id) {
  return 0;
}

static int set_pin_off(s_context_t * sctx, unsigned char pin_id) {
  return 0;
}

static long timer_get(s_context_t * sctx) {
  return 0;
}

static void timer_reset(s_context_t * sctx) {
}

extern "C" {
#else
#define USE_EVENT_POOL 0
#include "stdlib.h"
#include "string.h"


int null_sim_hookup(int type, unsigned char pin, s_context_t * sctx, s_event_t *evt) {
  return 0;
}

int (*g_sim_hookup)(int, unsigned char, s_context_t*, s_event_t*) = *null_sim_hookup;

void s_register_sim_hookup(int(*hookup)(int,unsigned char, s_context_t *, s_event_t *)) {
  g_sim_hookup = hookup;
}

static int xbee_init(s_context_t * ctx) {
  return 0;
}

static int xbee_tx(s_context_t* sctx, s_event_t * evt) {
  g_sim_hookup(S_SIM_XBEE_TX, 0, sctx, evt);
  return 0;
}

static int xbee_rx(s_context_t *sctx) {
  s_event_t evt;
  if (!(g_sim_hookup(S_SIM_XBEE_RX, 0, sctx, &evt))) {
    s_ctx_add_event(sctx, evt.s_evt_type, evt.s_evt_src, evt.s_evt_dest, evt.s_evt_data1, evt.s_evt_data2);
  }
  return 0;
}

static int get_pin_state(s_context_t *sctx, unsigned char pin_id) {
  return g_sim_hookup(S_SIM_GET_PIN_STATE, pin_id, sctx, NULL);
}

static int set_pin_on(s_context_t * sctx, unsigned char pin_id) {
  return g_sim_hookup(S_SIM_SET_PIN_ON, pin_id, sctx, NULL);
}

static int set_pin_off(s_context_t * sctx, unsigned char pin_id) {
  return g_sim_hookup(S_SIM_SET_PIN_OFF, pin_id, sctx, NULL);
}

long g_sim_timer = 0;

void s_sim_timer_inc(long ms_delta) {
  g_sim_timer += ms_delta;
}

static long timer_get(s_context_t * sctx) {
  return g_sim_timer;
}

static void timer_reset(s_context_t * sctx) {
  g_sim_timer = 0;
}

#endif

int null_sim_set_pin_on(int input) {
  return 0;
}


int s_test(void) {
  xbee_init(0);
  return 0;
}

int s_ctx_timer_cancel(s_context_t * sctx, unsigned char id) {
  sctx->s_ctx_timers[id].s_tmr_set = 1;
  return 0;
}

int s_ctx_timer_set(s_context_t * sctx, unsigned char id, long n_milliseconds, unsigned char data) {
  if (id < S_NTIMERS) {
    sctx->s_ctx_timers[id].s_tmr_set = 1;
    sctx->s_ctx_timers[id].s_tmr_when = n_milliseconds;
    sctx->s_ctx_timers[id].s_tmr_data = data;
  }
  return 0;
}

static long get_uptime_ms(s_context_t * sctx) {
  long result = timer_get(sctx);
  if (result < 0) {
    timer_reset(sctx);
    return 0;
  }
  return result;
}

void s_ctx_timers(s_context_t * sctx) { 
  long uptime_ms = get_uptime_ms(sctx);
  long mdelta = 0;
  unsigned char x = 0;
  if (uptime_ms == 0) {
    // underlying timer reset - treat as 0 seconds
    sctx->s_ctx_timers_last = 0;
    return;
  }
  mdelta = uptime_ms - sctx->s_ctx_timers_last;
  sctx->s_ctx_timers_last = uptime_ms;

  while (x < S_NTIMERS) {
    s_timer_t *tmr = &(sctx->s_ctx_timers[x]);
    if (tmr->s_tmr_set) {
      tmr->s_tmr_when -= mdelta;
      if (tmr->s_tmr_when <= 0) {
        s_ctx_add_event(sctx, S_EVT_TIMER, sctx->s_ctx_position, sctx->s_ctx_position, x, tmr->s_tmr_data);
        tmr->s_tmr_set = 0;
      }
    }
    x++;
  }
}

int s_ctx_init(s_context_t * sctx, unsigned char position) {
  int x=0;
  memset(sctx, 0, sizeof(s_context_t));
  while (x<EVENT_POOL_SIZE) {
#if USE_EVENT_POOL
    s_event_t * evt = g_event_pool[x];
#else
    s_event_t * evt = (s_event_t*)malloc(sizeof(s_event_t));
#endif
    evt->s_evt_next = sctx->s_ctx_free;
    sctx->s_ctx_free = evt;
    x++;
  }
  sctx->s_ctx_timers_last = get_uptime_ms(sctx);
  sctx->s_ctx_position = position;
  return 0;
}

int s_ctx_add_event(s_context_t * sctx, unsigned char type, unsigned char src, unsigned char dest, unsigned char data1, unsigned char data2) {
  s_event_t * evt, *next;
  if (!sctx->s_ctx_free) {
    return 1;
  }
  evt = sctx->s_ctx_free;
  sctx->s_ctx_free = evt->s_evt_next;

  next = sctx->s_ctx_events;

  if (next) {
    next->s_evt_prev = evt;
  } else {
    sctx->s_ctx_oldest = evt;
  }
  sctx->s_ctx_events = evt;

  evt->s_evt_next = next;
  evt->s_evt_prev = NULL;


  evt->s_evt_src = src;
  evt->s_evt_dest = dest;
  evt->s_evt_type = type;
  evt->s_evt_data1 = data1;
  evt->s_evt_data2 = data2;

  return 0;
}

int s_ctx_remove_event(s_context_t * sctx, s_event_t * evt) {
  s_event_t * prev = evt->s_evt_prev;
  s_event_t * next = evt->s_evt_next;
  if (prev) {
    prev->s_evt_next = next;
  } else {
    sctx->s_ctx_events = prev;
  }
  if (next) {
    next->s_evt_prev = prev;
  } else {
    sctx->s_ctx_oldest = prev;
  }

  evt->s_evt_next = sctx->s_ctx_free;
  sctx->s_ctx_free = evt;
  return 0;
}


int s_ctx_scan_button(s_context_t * sctx, s_button_t * button) {
  if (button->s_btn_type == S_BTNTYPE_POSEDGE) {
    int pin_state = get_pin_state(sctx, button->s_btn_pin);
    if (pin_state) {
      if (!button->s_btn_last) {
        s_ctx_add_event(sctx, S_EVT_BUTTON, sctx->s_ctx_position, sctx->s_ctx_position, button->s_btn_id, 0);
      }
    }
    button->s_btn_last = pin_state;
  }
  return 0;
}

int s_ctx_receive(s_context_t * sctx) {
  xbee_rx(sctx);
  return 0;
}

int s_ctx_transmit_one(s_context_t * sctx, s_event_t * evt) {
  xbee_tx(sctx, evt);
  return 0;
}

int s_ctx_transmit(s_context_t * sctx) {
  s_event_t * evt;
  evt = sctx->s_ctx_oldest;
  while (evt != NULL) {
    if (evt->s_evt_dest != sctx->s_ctx_position) {
      xbee_tx(sctx, evt);
      s_ctx_remove_event(sctx, evt);
    }
    evt = evt->s_evt_prev;
  }
  return 0;
}

int s_set_light(s_context_t * sctx, s_light_t * light, unsigned char color) {
  if (light->s_lgt_type == S_LGTTYPE_LED) {
    if (color == S_COLOR_ON) {
      set_pin_on(sctx, light->s_lgt_pin);
    }
    if (color == S_COLOR_OFF) {
      set_pin_off(sctx, light->s_lgt_pin);
    }
  }
  return 0;
}
//#endif

#ifdef __cplusplus
}
#endif
