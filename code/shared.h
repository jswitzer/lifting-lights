#ifndef __APLSHARED_H__
#define __APLSHARED_H__
#ifdef __cplusplus
extern "C" {
#define DEBUG_PRINT(...)
#else
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#endif

#define S_POS_INVALID   0x00
#define S_POS_LEFT      0x01
#define S_POS_RIGHT     0x02
#define S_POS_HEAD      0x03
#define S_POS_BASE      0x04
#define S_POS_REMOTE    0x05
#define S_POS_ALL       0xFF

#define S_EVT_INVALID   0x00
#define S_EVT_BUTTON    0x01
#define S_EVT_GOOD      0x02
#define S_EVT_BAD       0x03
#define S_EVT_RESET     0x04
#define S_EVT_TIMER     0x05

#if 0
#define S_LED_LEFT_GOOD  0x01
#define S_LED_LEFT_BAD   0x02
#define S_LED_HEAD_GOOD  0x03
#define S_LED_HEAD_BAD   0x04
#define S_LED_RIGHT_GOOD 0x05
#define S_LED_RIGHT_BAD  0x06
#endif

#if 0
#define S_BTN_GOODLIFT  0x01
#define S_BTN_BADLIFT   0x02
#define S_BTN_RESET     0x03
#endif

#define S_COLOR_INVALID 0x00
#define S_COLOR_OFF     0x01
#define S_COLOR_ON      0x02
#define S_COLOR_RED     0x03
#define S_COLOR_GREEN   0x04
#define S_COLOR_BLUE    0x05
#define S_COLOR_YELLOW  0x06

#define S_BTNTYPE_POSEDGE 0x01

#define S_LGTTYPE_LED 0x01

#define PIN_R_RESET 1
#define PIN_R_GOOD 2
#define PIN_R_BAD 3
#define PIN_R_PWR 4

#define PIN_B_LEFT_GOOD 1
#define PIN_B_LEFT_BAD 2
#define PIN_B_RIGHT_GOOD 3
#define PIN_B_RIGHT_BAD 4
#define PIN_B_HEAD_GOOD 5
#define PIN_B_HEAD_BAD 6

#define S_TIMER_ONE 0
#define S_TIMER_RESET 1
#define S_NTIMERS 2

typedef struct s_event_s  {
  unsigned char s_evt_type; 
  unsigned char s_evt_src;
  unsigned char s_evt_dest;
  unsigned char s_evt_data1;
  unsigned char s_evt_data2;
  struct s_event_s * s_evt_prev;
  struct s_event_s * s_evt_next;
} s_event_t;

typedef struct s_button_s {
  unsigned char s_btn_id;
  unsigned char s_btn_type;
  unsigned char s_btn_pin;
  unsigned char s_btn_last;
} s_button_t;

typedef struct s_light_s {
  unsigned char s_lgt_type;
  unsigned char s_lgt_pin;
} s_light_t;

typedef struct s_timer_s {
  long s_tmr_when;
  unsigned char s_tmr_data;
  unsigned char s_tmr_set;
} s_timer_t;

typedef struct s_context_s {
  unsigned char s_ctx_position;
  s_event_t * s_ctx_free;
  s_event_t * s_ctx_events;
  s_event_t * s_ctx_oldest;
  s_timer_t   s_ctx_timers[S_NTIMERS];
  long        s_ctx_timers_last;
} s_context_t;



int s_test(void);

#if 0
void s_register_interrupts(int(*interrupt0)(int), int(*interrupt1)(int));

int s_fire_interrupt0(void);

int s_fire_interrupt1(void);
#endif

int s_ctx_init(s_context_t * sctx, unsigned char position);
int s_ctx_remove_event(s_context_t * sctx, s_event_t * evt);
int s_ctx_transmit_one(s_context_t * sctx, s_event_t * evt);
int s_ctx_receive(s_context_t * sctx);
int s_ctx_scan_button(s_context_t * sctx, s_button_t * button);
int s_set_light(s_context_t * sctx, s_light_t * light, unsigned char color);
int s_ctx_add_event(s_context_t * sctx, \
                        unsigned char type, \
                        unsigned char src, \
                        unsigned char dest, \
                        unsigned char data1, \
                        unsigned char data2);

int s_ctx_timer_set(s_context_t * sctx, unsigned char id, long n_milliseconds, unsigned char data);
int s_ctx_timer_cancel(s_context_t * sctx, unsigned char id);
void s_ctx_timers(s_context_t * sctx);

#ifdef __cplusplus
}
#else
void s_register_sim_hookup(int(*hookup)(int,unsigned char, s_context_t *, s_event_t *));
void s_sim_timer_inc(long ms_delta);
#define S_SIM_SET_PIN_OFF 1
#define S_SIM_SET_PIN_ON 2
#define S_SIM_GET_PIN_STATE 3
#define S_SIM_XBEE_RX 4
#define S_SIM_XBEE_TX 5
#endif
#endif
