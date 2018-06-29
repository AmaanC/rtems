/*  cpu_asm.S
 *
 *  This file contains all assembly code for the amd64 implementation
 *  of RTEMS.
 *
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
        PUBLIC (_CPU_Context_switch)

.set RUNCONTEXT_ARG,   rdi                   /* save context argument #1 */
.set HEIRCONTEXT_ARG,  rsi                   /* restore context argument #2 */

SYM (_CPU_Context_switch):
        movq      RUNCONTEXT_ARG, rax  /* rax = running threads context */
        pushf                              /* push rflags */
        popq      (0  * CPU_SIZEOF_POINTER)(rax)          /* save rflags */
   	    movq rbx, (1  * CPU_SIZEOF_POINTER)(rax)
   	    movq rcx, (2  * CPU_SIZEOF_POINTER)(rax)
   	    movq rdx, (3  * CPU_SIZEOF_POINTER)(rax)
   	    movq rdi, (4  * CPU_SIZEOF_POINTER)(rax)
   	    movq rsi, (5  * CPU_SIZEOF_POINTER)(rax)
   	    movq rbp, (6  * CPU_SIZEOF_POINTER)(rax)
   	    movq rsp, (7  * CPU_SIZEOF_POINTER)(rax)
   	    movq r8,  (8  * CPU_SIZEOF_POINTER)(rax)
   	    movq r9,  (9  * CPU_SIZEOF_POINTER)(rax)
   	    movq r10, (10 * CPU_SIZEOF_POINTER)(rax)
   	    movq r11, (11 * CPU_SIZEOF_POINTER)(rax)
   	    movq r12, (12 * CPU_SIZEOF_POINTER)(rax)
   	    movq r13, (13 * CPU_SIZEOF_POINTER)(rax)
   	    movq r14, (14 * CPU_SIZEOF_POINTER)(rax)
   	    movq r15, (15 * CPU_SIZEOF_POINTER)(rax)

    // XXX: FS, RAX?

    // XXX: RTEMS_SMP considerations?

        movq      HEIRCONTEXT_ARG, rax /* rax = heir threads context */

restore:
        pushq     (0  * CPU_SIZEOF_POINTER)(rax)          /* push rflags */
        popf                               /* restore rflags */
   	    movq (1  * CPU_SIZEOF_POINTER)(rax), rbx
   	    movq (2  * CPU_SIZEOF_POINTER)(rax), rcx
   	    movq (3  * CPU_SIZEOF_POINTER)(rax), rdx
	// XXX: Should registers part of the calling convention be saved at all? Why?
   	    movq (4  * CPU_SIZEOF_POINTER)(rax), rdi
   	    movq (5  * CPU_SIZEOF_POINTER)(rax), rsi
   	    movq (6  * CPU_SIZEOF_POINTER)(rax), rbp
   	    movq (7  * CPU_SIZEOF_POINTER)(rax), rsp
   	    movq (8  * CPU_SIZEOF_POINTER)(rax), r8
   	    movq (9  * CPU_SIZEOF_POINTER)(rax), r9
   	    movq (10 * CPU_SIZEOF_POINTER)(rax), r10
   	    movq (11 * CPU_SIZEOF_POINTER)(rax), r11
   	    movq (12 * CPU_SIZEOF_POINTER)(rax), r12
   	    movq (13 * CPU_SIZEOF_POINTER)(rax), r13
   	    movq (14 * CPU_SIZEOF_POINTER)(rax), r14
   	    movq (15 * CPU_SIZEOF_POINTER)(rax), r15
	// XXX: FS into GDT, EAX
    // XXX: Loads the selector to "refresh" the segment

        ret

/*
 *  NOTE: May be unnecessary to reload some registers.
 */

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context.
 */

        PUBLIC (_CPU_Context_restore)

.set NEWCONTEXT_ARG,   rdi                   /* context to restore argument */

SYM (_CPU_Context_restore):
        movq      NEWCONTEXT_ARG, rax  /* rax = running threads context */
        jmp       restore

END_CODE
END
