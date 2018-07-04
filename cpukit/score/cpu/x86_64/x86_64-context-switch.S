/*
 *  COPYRIGHT (c) 2018.
 *  Amaan Cheval <amaan.cheval@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT not defined"
#endif

BEGIN_CODE

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */

.p2align  1
PUBLIC(_CPU_Context_switch)

.set RUNCONTEXT_ARG,   rdi                   /* save context argument */
.set HEIRCONTEXT_ARG,  rsi                   /* restore context argument */

SYM(_CPU_Context_switch):
  movq    RUNCONTEXT_ARG, rax  /* rax = running threads context */

  /* Fill up Context_Control struct */
  pushf
  popq               (0 * CPU_SIZEOF_POINTER)(rax) /* pop rflags into context */
  movq    rbx,       (1 * CPU_SIZEOF_POINTER)(rax)
  movq    rsp,       (2 * CPU_SIZEOF_POINTER)(rax)
  movq    rbp,       (3 * CPU_SIZEOF_POINTER)(rax)
  movq    r12,       (4 * CPU_SIZEOF_POINTER)(rax)
  movq    r13,       (5 * CPU_SIZEOF_POINTER)(rax)
  movq    r14,       (6 * CPU_SIZEOF_POINTER)(rax)
  movq    r15,       (7 * CPU_SIZEOF_POINTER)(rax)

  movq    HEIRCONTEXT_ARG, rax /* rax = heir threads context */

restore:
  pushq (0 * CPU_SIZEOF_POINTER)(rax)       /* push rflags */
  popf                                      /* restore rflags */
  movq  (1 * CPU_SIZEOF_POINTER)(rax), rbx
  movq  (2 * CPU_SIZEOF_POINTER)(rax), rsp
  movq  (3 * CPU_SIZEOF_POINTER)(rax), rbp
  movq  (4 * CPU_SIZEOF_POINTER)(rax), r12
  movq  (5 * CPU_SIZEOF_POINTER)(rax), r13
  movq  (6 * CPU_SIZEOF_POINTER)(rax), r14
  movq  (7 * CPU_SIZEOF_POINTER)(rax), r15

  /* XXX: TLS - load GDT and refresh FS segment selector */

  ret

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context restore.
 */

PUBLIC(_CPU_Context_restore)

.set NEWCONTEXT_ARG,   rdi       /* context to restore argument */

SYM(_CPU_Context_restore):
  movq      NEWCONTEXT_ARG, rax  /* rax = running threads context */
  jmp       restore

END_CODE
END
