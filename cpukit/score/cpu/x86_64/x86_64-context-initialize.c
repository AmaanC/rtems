/*
 *  COPYRIGHT (c) 2018.
 *  Amaan Cheval <amaan.cheval@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>
#include <rtems/score/tls.h>

/*
 * Stack alignment note:
 *
 * Per the x86-64 SysV ABI, the stack frame layout is as follows:
 *       optional args
 *       ------        (16-byte alignment boundary)
 * RSP-> return_addr   (RSP is moved lower as needed for this frame)
 *
 * Per the ABI:
 *
 *  > The end of the input argument area shall be aligned on a 16 (32 or 64, if
 *    __m256 or __m512 is passed on stack) byte boundary.
 *
 *  > In other words, the value (%rsp + 8) is always a multiple of 16 (32 or 64)
 *    when control is transferred to the function entry point.
 *
 * We want the stack to look to the '_entry_point' routine
 * like an ordinary stack frame as if '_entry_point' was
 * called from C-code.
 * Note that '_entry_point' is jumped-to by the 'ret'
 * instruction returning from _CPU_Context_switch() or
 * _CPU_Context_restore() thus popping the _entry_point
 * from the stack.
 *
 * Hence we must initialize the stack as follows
 *
 *         [arg0 (aligned)]:  n/a
 *         [ret. addr     ]:  NULL
 * RSP->   [jump-target   ]:  _entry_point
 *
 * When Context_switch returns it pops the _entry_point from
 * the stack which then finds a standard layout.
 */
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
  uintptr_t _stack;

  /* avoid warning for being unused */
  (void) is_fp;

  // XXX: Should be used in the future
  (void) new_level;
  (void) tls_area;

  // XXX: Leaving interrupts off regardless of `new_level` for now
  the_context->rflags = CPU_EFLAGS_INTERRUPTS_OFF;

  _stack  = ((uintptr_t)(stack_area_begin)) + (stack_area_size);
  _stack &= ~(CPU_STACK_ALIGNMENT - 1);
  _stack -= sizeof(uintptr_t); /* fake return address for entry_point's frame;
                                * this allows rsp+8 to be an aligned boundary */
  *((proc_ptr *) _stack) = entry_point;

  the_context->rbp     = (void *) 0;
  the_context->rsp     = (void *) _stack;

  // XXX: Initialize thread-local storage area (TLS / TCB)
}
