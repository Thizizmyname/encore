#ifndef ENCORE_H_6Q243YHL
#define ENCORE_H_6Q243YHL

#define _XOPEN_SOURCE 800
#include <ucontext.h>

#define LAZY_IMPL

#define Stack_Size 64*1024
typedef struct ctx_wrapper {
  ucontext_t* ctx;
  void* uc_link;
} ctx_wrapper;

#include <pony/pony.h>

static pony_type_t *ENCORE_ACTIVE    = (pony_type_t *)1;
static pony_type_t *ENCORE_PRIMITIVE = (pony_type_t *)NULL;

typedef struct encore_actor encore_actor_t;
typedef struct encore_oneway_msg encore_oneway_msg_t;
typedef struct encore_fut_msg encore_fut_msg_t;

typedef union
{
  void* p;
  intptr_t i;
  double d;
} encore_arg_t;

typedef enum {
  ID_CLOSURE = 0,
  ID_FUTURE,
  ID_SCONS,
  ID_ARRAY,
} encore_type_id;

typedef enum {
  _ENC__MSG_RESUME_GET, /// This should probably go
  _ENC__MSG_RESUME_SUSPEND,
  _ENC__MSG_RESUME_AWAIT,
  _ENC__MSG_RUN_CLOSURE,
  _ENC__MSG_MAIN,
} encore_msg_id;

struct encore_oneway_msg
{
  struct pony_msg_t;
};

#include "future.h"

struct encore_fut_msg
{
  encore_oneway_msg_t;
  future_t    *_fut;
};

typedef struct stack_page {
  void *stack;
  struct stack_page *next;
} stack_page;

void *get_local_page_stack();
struct encore_actor
{
  pony_actor_pad_t;
  // Everything else that goes into an encore_actor that's not part of PonyRT
  ucontext_t ctx;
  ucontext_t home_ctx;
  bool resume;
  bool run_to_completion;
  stack_page *page;
  ucontext_t *saved;
};

/// Create a new Encore actor
encore_actor_t *encore_create(pony_type_t *type);

/// Create a new Encore actor in another work pool
encore_actor_t *encore_peer_create(pony_type_t *type);

/// Allocate s bytes of memory, zeroed out
void *encore_alloc(size_t s);

/// The starting point of all Encore programs
int encore_start(int argc, char** argv, pony_type_t *type);

bool encore_actor_run_hook(encore_actor_t *actor);
bool encore_actor_handle_message_hook(encore_actor_t *actor, pony_msg_t* msg);
void actor_block(encore_actor_t *actor);
void actor_set_resume(encore_actor_t *actor);
#ifdef LAZY_IMPL
void actor_resume(encore_actor_t *actor) __attribute__ ((noreturn));
#endif

/// calls the pony's respond with the current object's scheduler
void call_respond_with_current_scheduler();

#endif /* end of include guard: ENCORE_H_6Q243YHL */