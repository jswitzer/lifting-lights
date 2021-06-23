#ifndef __APLREMOTE_H__
#define __APLREMOTE_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "shared.h"


#define R_BTN_OFF 0
#define R_BTN_GOOD 1
#define R_BTN_BAD 2
#define R_BTN_RESET 3
#define R_NBUTTONS 4

#define R_LGT_PWR 0
#define R_NLIGHTS 1

typedef struct r_state_s {
  s_light_t rs_light[R_NLIGHTS];
  s_button_t rs_buttons[R_NBUTTONS];
} r_state_t;

typedef struct r_context_s {
  char r_ctx_position;
  r_state_t r_ctx_state;
  s_context_t r_ctx_shared;
} r_context_t;

int r_test(void);
int r_setup(r_context_t * ctx);
int r_loop(r_context_t * ctx);
int r_init(r_context_t * ctx, unsigned char position);

#ifdef __cplusplus
}
#endif
#endif
