/**
 *  @file
 *
 *  @brief No CPU Dependent Source
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/tls.h>

// XXX:
Context_Control_fp _CPU_Null_fp_context;
void _CPU_Exception_frame_print (const CPU_Exception_frame *ctx) {}

void _CPU_Initialize(void)
{
  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */
}

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
  uint32_t _stack;
  uint32_t tcb;

  (void) is_fp; /* avoid warning for being unused */

  // XXX: Leaving interrupts off for now
  the_context->rflags = 0x0000000000003002;

  _stack  = ((uint32_t)(stack_area_begin)) + (stack_area_size);
  _stack &= ~ (CPU_STACK_ALIGNMENT - 1);
  _stack -= 2*sizeof(proc_ptr*); /* see above for why we need to do this */
  *((proc_ptr *)(_stack)) = (entry_point);
  the_context->rbp     = (void *) 0;
  the_context->rsp     = (void *) _stack;

  if ( tls_area != NULL ) {
    tcb = (uint32_t) _TLS_TCB_after_TLS_block_initialize( tls_area );
  } else {
    tcb = 0;
  }

  // XXX: Use FS for tcb - can leave out for now
}

uint32_t   _CPU_ISR_Get_level( void )
{
  /*
   *  This routine returns the current interrupt level.
   */

  return 0;
}

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   */
}

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  // XXX
}

/*
 *  _CPU_Install_interrupt_stack
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void _CPU_Install_interrupt_stack( void )
{
}

/*
 *  _CPU_Thread_Idle_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{

  for( ; ; )
    /* insert your "halt" instruction here */ ;
}
