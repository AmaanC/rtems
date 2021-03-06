/*
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 * Copyright (c) 2013, The Regents of the University of California (Regents).
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <rtems/asm.h>
#include <rtems/score/percpu.h>
#include <rtems/score/riscv-utility.h>
#include <bsp/linker-symbols.h>
#include <bspopts.h>

EXTERN(bsp_section_bss_begin)
EXTERN(bsp_section_bss_end)
EXTERN(ISR_Handler)
EXTERN(bsp_section_stack_begin)

PUBLIC(bsp_start_vector_table_begin)
PUBLIC(bsp_start_vector_table_end)
PUBLIC(_start)

	.section	.bsp_start_text, "wax", @progbits
	.align	2

TYPE_FUNC(_start)
SYM(_start):
	/* Load global pointer */
	.option	push
	.option	norelax
	LADDR	gp, __global_pointer$
	.option	pop

#ifdef RTEMS_SMP
	csrr	s0, mhartid
	LADDR	t0, _Per_CPU_Information
	slli	t1, s0, PER_CPU_CONTROL_SIZE_LOG2
	add	t0, t0, t1
	csrw	mscratch, t0
	bnez	s0, .Lwait_for_go
#endif

	/* load stack and frame pointers */
	LADDR	sp, _Configuration_Interrupt_stack_area_end

#ifdef BSP_START_COPY_FDT_FROM_U_BOOT
	mv	a0, a1
	call	bsp_fdt_copy
#endif

	LADDR	t0, ISR_Handler
	csrw	mtvec, t0

	/* Clear .bss */
	LADDR	a0, bsp_section_bss_begin
	li	a1, 0
	LADDR	a2, bsp_section_bss_size
	call	memset

#ifdef RTEMS_SMP
	/* Give go to secondary processors */
	LADDR	t0, .Lsecondary_processor_go
	fence	iorw,ow
	amoswap.w	zero, zero, 0(t0)
#endif

	/* Init FPU unit if it's there */
	li	t0, MSTATUS_FS
	csrs	mstatus, t0

	j	boot_card

#ifdef RTEMS_SMP
	/* Wait for go issued by the boot processor (mhartid == 0) */
.Lwait_for_go:
	LADDR	t0, .Lsecondary_processor_go
.Lwait_for_go_again:
	lw	t1, 0(t0)
	fence	iorw, iorw
	sext.w	t1, t1
	bnez	t1, .Lwait_for_go_again
.Lloop_forever:
	j	.Lloop_forever

.Lsecondary_processor_go:
	.word	0xdeadbeef
#endif

#if __riscv_xlen == 32
#define ADDR .word
#elif __riscv_xlen == 64
#define ADDR .quad
#endif

	.align	4
bsp_start_vector_table_begin:
	ADDR	_RISCV_Exception_default /* User int */
	ADDR	_RISCV_Exception_default /* Supervisor int */
	ADDR	_RISCV_Exception_default /* Reserved */
	ADDR	_RISCV_Exception_default /* Machine int */
	ADDR	_RISCV_Exception_default /* User timer int */
	ADDR	_RISCV_Exception_default /* Supervisor Timer int */
	ADDR	_RISCV_Exception_default /* Reserved */
	ADDR	_RISCV_Exception_default /* Machine Timer int */
	ADDR	_RISCV_Exception_default /* User external int */
	ADDR	_RISCV_Exception_default /* Supervisor external int */
	ADDR	_RISCV_Exception_default /* Reserved */
	ADDR	_RISCV_Exception_default /* Machine external int */
	ADDR	_RISCV_Exception_default
	ADDR	_RISCV_Exception_default
	ADDR	_RISCV_Exception_default
	ADDR	_RISCV_Exception_default
bsp_start_vector_table_end:
