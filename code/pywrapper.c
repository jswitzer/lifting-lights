#include "Python.h"
#include "shared.h"
#include "base.h"
#include "remote.h"

#define MAX_REMOTE_CONTEXTS 10
#define MAX_BASE_CONTEXTS 10
#define MAX_PINS 40

r_context_t g_remote_contexts[MAX_REMOTE_CONTEXTS];
b_context_t g_base_contexts[MAX_BASE_CONTEXTS];
int         g_base_context_next = 0;
int         g_remote_context_next = 0;

int g_base_pins[MAX_BASE_CONTEXTS][MAX_PINS];
int g_remote_pins[MAX_REMOTE_CONTEXTS][MAX_PINS];

s_event_t g_base_inbox[MAX_BASE_CONTEXTS];
s_event_t g_remote_inbox[MAX_REMOTE_CONTEXTS];

static r_context_t * get_rctxt(int ctxid) {
  return (&(g_remote_contexts[ctxid]));
}

static b_context_t * get_bctxt(int ctxid) {
  return (&(g_base_contexts[ctxid]));
}

static int sim_lookup_remote(s_context_t *sctx, unsigned char position) {
  int r_idx=0;
  r_context_t * rctx;
  for (r_idx = 0; r_idx < g_remote_context_next; r_idx ++) {
    rctx = get_rctxt(r_idx);
    if (sctx) {
      if (&(rctx->r_ctx_shared) == sctx) {
        return r_idx;
      }
    } else {
      if (position == rctx->r_ctx_position) {
        return r_idx;
      }
    }
  }
  return -1;
}

static int sim_lookup_base(s_context_t *sctx, unsigned char position) {
  int b_idx=0;
  b_context_t * bctx;

  for (b_idx = 0; b_idx < g_base_context_next; b_idx ++) {
    bctx = get_bctxt(b_idx);
    if (sctx) {
      if (&(bctx->b_ctx_shared) == sctx) {
        return b_idx;
      }
    } else {
      if (position == bctx->b_ctx_position) {
        return b_idx;
      }
    }
  }
  return -1;
}

int sim_hookup(int type, unsigned char pin, s_context_t * sctx, s_event_t *evt) {
  int ctx_id = 0;
    if (type == S_SIM_XBEE_TX) {
      s_event_t * tail=NULL, *new;
      new = (s_event_t *)malloc(sizeof(s_event_t));
      new->s_evt_src = evt->s_evt_src;
      new->s_evt_dest = evt->s_evt_dest;
      new->s_evt_type = evt->s_evt_type;
      new->s_evt_data1 = evt->s_evt_data1;
      new->s_evt_data2 = evt->s_evt_data2;
      new->s_evt_next = NULL;

      ctx_id = sim_lookup_remote(NULL, evt->s_evt_dest);
      if (ctx_id >= 0) {
        tail = &(g_remote_inbox[ctx_id]);
      } else {
      ctx_id = sim_lookup_base(NULL, evt->s_evt_dest);
      if (ctx_id >= 0) {
        tail = &(g_base_inbox[ctx_id]);
      }
      }
      if (tail) {
      while (tail->s_evt_next) {
        tail = tail->s_evt_next;
      }
      tail->s_evt_next = new;
      } else {
        DEBUG_PRINT("Couldn't find anyone at position %d\n", evt->s_evt_dest);
      }

    }
    if (type == S_SIM_XBEE_RX) {
      s_event_t * head=NULL;
      ctx_id = sim_lookup_remote(sctx, 0);
      if (ctx_id >= 0) {
        head = g_remote_inbox[ctx_id].s_evt_next;
        if (head) {
          g_remote_inbox[ctx_id].s_evt_next = head->s_evt_next;
        }
      } else {
        ctx_id = sim_lookup_base(sctx, 0);
        if (ctx_id >= 0) {
          head = g_base_inbox[ctx_id].s_evt_next;
          if (head) {
            g_base_inbox[ctx_id].s_evt_next = head->s_evt_next;
          }
        }
      }
      if (head) {
        evt->s_evt_type = head->s_evt_type;
        evt->s_evt_src = head->s_evt_src;
        evt->s_evt_dest = head->s_evt_dest;
        evt->s_evt_data1 = head->s_evt_data1;
        evt->s_evt_data2 = head->s_evt_data2;
        free(head);
        return 0;
      } else {
        return 1;
      }
    }

    if (type == S_SIM_SET_PIN_ON) {
      ctx_id = sim_lookup_remote(sctx, 0);
      if (ctx_id >= 0) {
        g_remote_pins[ctx_id][pin] = 1;
      } else {
        ctx_id = sim_lookup_base(sctx, 0);
        if (ctx_id >= 0) {
          g_base_pins[ctx_id][pin] = 1;
        }
      }
    }

    if (type == S_SIM_SET_PIN_OFF) {
      ctx_id = sim_lookup_remote(sctx, 0);
      if (ctx_id >= 0) {
        g_remote_pins[ctx_id][pin] = 0;
      } else {
        ctx_id = sim_lookup_base(sctx, 0);
        if (ctx_id >= 0) {
          g_base_pins[ctx_id][pin] = 0;
        }
      }
    }

    if (type == S_SIM_GET_PIN_STATE) {
      int res = 0;

      ctx_id = sim_lookup_remote(sctx, 0);
      if (ctx_id >= 0) {
        res = g_remote_pins[ctx_id][pin];
        g_remote_pins[ctx_id][pin] = 0;
      } else {
        ctx_id = sim_lookup_base(sctx, 0);
        if (ctx_id >= 0) {
          res = g_base_pins[ctx_id][pin];
          g_base_pins[ctx_id][pin] = 0;
        }
      }
      return res;
    }

    return 0;
}


static PyObject *
py_pywrapper_test(PyObject *self, PyObject *args) {
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
py_s_test(PyObject *self, PyObject *args) {
  int rv = s_test();
  return PyInt_FromLong((long)rv);
}

static PyObject *
py_b_test(PyObject *self, PyObject *args) {
  int rv = b_test();
  return PyInt_FromLong((long)rv);
}

static PyObject *
py_r_test(PyObject *self, PyObject *args) {
  int rv = r_test();
  return PyInt_FromLong((long)rv);
}

static PyObject *
py_b_ctx_new(PyObject *self, PyObject *args) {
  int context_idx = g_base_context_next ++;
  b_init(get_bctxt(context_idx), S_POS_BASE);
  return PyInt_FromLong((long)context_idx);
}

static PyObject *
py_r_ctx_new(PyObject *self, PyObject *args) {
  char * pos;
  char position = S_POS_HEAD;
  int context_idx = g_remote_context_next ++;
  PyArg_ParseTuple(args, "s", &pos);
  if (!strcmp(pos, "LEFT")) {
    position = S_POS_LEFT;
  }
  if (!strcmp(pos, "RIGHT")) {
    position = S_POS_RIGHT;
  }

  r_init(get_rctxt(context_idx), position);
  return PyInt_FromLong((long)context_idx);
}

static PyObject *
py_loop(PyObject *self, PyObject *args) {
  int r_idx=0, b_idx=0;
  r_context_t * rctx;
  b_context_t * bctx;
  int ms_delta;

  PyArg_ParseTuple(args, "i", &ms_delta);

  // loop all remotes
  for (r_idx = 0; r_idx < g_remote_context_next; r_idx ++) {
    rctx = get_rctxt(r_idx);
    r_loop(rctx);
  }
    
  // loop all bases
  for (b_idx = 0; b_idx < g_base_context_next; b_idx ++) {
    bctx = get_bctxt(b_idx);
    b_loop(bctx);
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
py_r_keypress(PyObject *self, PyObject *args) {
  int r_idx;
  char * btn;
  PyArg_ParseTuple(args, "is", &r_idx, &btn);

  if (r_idx >= 0) {
    if (!strcmp(btn, "RESET")) {
      g_remote_pins[r_idx][PIN_R_RESET] = 1;
      DEBUG_PRINT("SET RESET PIN for %d to 1\n", r_idx);
    }
    if (!strcmp(btn, "GOOD")) {
      g_remote_pins[r_idx][PIN_R_GOOD] = 1;
      DEBUG_PRINT("SET GOOD PIN for %d to 1\n", r_idx);
    }
    if (!strcmp(btn, "BAD")) {
      g_remote_pins[r_idx][PIN_R_BAD] = 1;
      DEBUG_PRINT("SET BAD PIN for %d to 1\n", r_idx);
    }
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
py_b_keypress(PyObject *self, PyObject *args) {
  char * btn;
  int b_idx;
  PyArg_ParseTuple(args, "is", &b_idx, &btn);
  //b_idx = sim_lookup_base(NULL, position);
  Py_INCREF(Py_None);
  return Py_None;
}
 
static PyObject *
py_r_get_lights(PyObject *self, PyObject *args) {
  PyObject *d = PyDict_New();
  int r_idx;
  PyArg_ParseTuple(args, "i", &r_idx);
  //r_idx = sim_lookup_base(NULL, position);
  return d;
}


static PyObject *
py_b_get_lights(PyObject *self, PyObject *args) {
  int b_idx;
  PyObject *d = PyDict_New();
  PyArg_ParseTuple(args, "i", &b_idx);

  if (g_base_pins[b_idx][PIN_B_LEFT_GOOD]) {
    PyDict_SetItem(d, Py_BuildValue("s", "b_left_good"), Py_BuildValue("s", "white"));
  } else {
    PyDict_SetItem(d, Py_BuildValue("s", "b_left_good"), Py_BuildValue("s", "black"));
  }
  if (g_base_pins[b_idx][PIN_B_LEFT_BAD]) {
    PyDict_SetItem(d, Py_BuildValue("s", "b_left_bad"), Py_BuildValue("s", "red"));
  } else {
    PyDict_SetItem(d, Py_BuildValue("s", "b_left_bad"), Py_BuildValue("s", "black"));
  }

  if (g_base_pins[b_idx][PIN_B_HEAD_GOOD]) {
    PyDict_SetItem(d, Py_BuildValue("s", "b_head_good"), Py_BuildValue("s", "white"));
  } else {
    PyDict_SetItem(d, Py_BuildValue("s", "b_head_good"), Py_BuildValue("s", "black"));
  }
  if (g_base_pins[b_idx][PIN_B_HEAD_BAD]) {
    PyDict_SetItem(d, Py_BuildValue("s", "b_head_bad"), Py_BuildValue("s", "red"));
  } else {
    PyDict_SetItem(d, Py_BuildValue("s", "b_head_bad"), Py_BuildValue("s", "black"));
  }

  if (g_base_pins[b_idx][PIN_B_RIGHT_GOOD]) {
    PyDict_SetItem(d, Py_BuildValue("s", "b_right_good"), Py_BuildValue("s", "white"));
  } else {
    PyDict_SetItem(d, Py_BuildValue("s", "b_right_good"), Py_BuildValue("s", "black"));
  }

  if (g_base_pins[b_idx][PIN_B_RIGHT_BAD]) {
    PyDict_SetItem(d, Py_BuildValue("s", "b_right_bad"), Py_BuildValue("s", "red"));
  } else {
    PyDict_SetItem(d, Py_BuildValue("s", "b_right_bad"), Py_BuildValue("s", "black"));
  }
  return d;
}
 
static PyMethodDef c_methods[] = {
    {"pywrapper_test", py_pywrapper_test, METH_VARARGS, "pywrapper_test()"},
    {"s_test", py_s_test, METH_VARARGS, "s_test()"},
    {"b_test", py_b_test, METH_VARARGS, "b_test()"},
    {"r_test", py_r_test, METH_VARARGS, "r_test()"},
    {"b_ctx_new", py_b_ctx_new, METH_VARARGS, "b_ctx_new()"},
    {"r_ctx_new", py_r_ctx_new, METH_VARARGS, "r_ctx_new()"},
    {"loop", py_loop, METH_VARARGS, "loop()"}, 
    {"b_get_lights", py_b_get_lights, METH_VARARGS, "b_get_lights()"},
    {"r_get_lights", py_r_get_lights, METH_VARARGS, "r_get_lights()"},
    {"b_keypress", py_b_keypress, METH_VARARGS, "b_keypress()"},
    {"r_keypress", py_r_keypress, METH_VARARGS, "r_keypress()"},
    {NULL, NULL}
};

PyMODINIT_FUNC
initlifting_lights_c(void) { 
    int x ,y;
    memset(g_remote_contexts, 0, sizeof(MAX_REMOTE_CONTEXTS)*sizeof(r_context_t));
    memset(g_base_contexts, 0, sizeof(MAX_BASE_CONTEXTS)*sizeof(b_context_t));
    Py_InitModule("lifting_lights_c", c_methods);
    for (x=0;x < MAX_BASE_CONTEXTS; x++){
      for (y=0;y < MAX_PINS; y++){
        g_base_pins[x][y] = 0;
      }
      g_base_inbox[x].s_evt_next = NULL;
    }
    for (x=0;x < MAX_REMOTE_CONTEXTS; x++){
      for (y=0;y < MAX_PINS; y++){
        g_remote_pins[x][y] = 0;
      }
      g_remote_inbox[x].s_evt_next = NULL;
    }

    s_register_sim_hookup(*sim_hookup);
}
