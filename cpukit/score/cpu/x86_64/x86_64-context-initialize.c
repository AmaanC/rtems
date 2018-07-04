#include <rtems/score/cpu.h>
#include <rtems/score/tls.h>

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  uint64_t _stack;

  /* avoid warning for being unused */
  (void) is_fp;

  // XXX: Should be used in the future
  (void) new_level;
  (void) tls_area;

  // XXX: Leaving interrupts off regardless of `new_level` for now
  the_context->rflags = CPU_EFLAGS_INTERRUPTS_OFF;

  _stack  = ((uint64_t)(stack_area_begin)) + (stack_area_size);
  _stack &= ~ (CPU_STACK_ALIGNMENT - 1);
  _stack -= 2*sizeof(proc_ptr*); /* see above for why we need to do this */
  *((proc_ptr *)(_stack)) = (entry_point);
  the_context->rbp     = (void *) 0;
  the_context->rsp     = (void *) _stack;

  // XXX: Initialize thread-local storage area (TLS / TCB)
  // XXXXXXX: If it fails, the missing TLS calls may be responsible
}
