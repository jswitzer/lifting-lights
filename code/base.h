#ifndef __APLBASE_H__
#define __APLBASE_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "shared.h"

#define B_BTN_OFF 0
#define B_NBUTTONS 1

#define B_LGT_PWR 0
#define B_LGT_LEFT_BAD 1
#define B_LGT_LEFT_GOOD 2
#define B_LGT_RIGHT_BAD 3
#define B_LGT_RIGHT_GOOD 4
#define B_LGT_HEAD_BAD 5
#define B_LGT_HEAD_GOOD 6
#define B_NLIGHTS 7

typedef struct b_state_s {
  s_light_t bs_lights[B_NLIGHTS];
  s_button_t bs_buttons[B_NBUTTONS];
  unsigned char bs_left;
  unsigned char bs_right;
  unsigned char bs_head;
} b_state_t;

typedef struct b_context_s {
  char b_ctx_position;
  b_state_t b_ctx_state;
  s_context_t b_ctx_shared;
} b_context_t;

int b_test(void);
int b_setup(b_context_t * ctx);
int b_loop(b_context_t * ctx);
int b_init(b_context_t * ctx, unsigned char position);

#ifdef __cplusplus
}
#endif
#endif
