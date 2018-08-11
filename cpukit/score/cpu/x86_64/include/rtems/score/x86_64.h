/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

#ifndef _RTEMS_SCORE_X86_64_H
#define _RTEMS_SCORE_X86_64_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_NAME "x86-64"
#define CPU_MODEL_NAME "amd64"

#define COM1_BASE_IO    0x3F8
#define COM1_CLOCK_RATE (115200 * 16)

#define PIC1_DATA                 0xa1
#define PIC2_DATA                 0x21

/* This remaps IRQ0 to vector number 0x20 and so on (i.e. IDT[32]) */
#define PIC1_REMAP_DEST           0x20
#define PIC2_REMAP_DEST           0x28

#define PIT_FREQUENCY             1193180
/*
 * The PIT_FREQUENCY determines how many times the PIT counter is decremented
 * per second - therefore, we can calculate how many ticks we set based on what
 * fraction of a second we're okay with spending on calibration
 */
#define PIT_CALIBRATE_DIVIDER     100
#define PIT_CALIBRATE_TICKS       (PIT_FREQUENCY/PIT_CALIBRATE_DIVIDER)

/* I/O ports for the PIT */
#define PIT_PORT_CHAN0            0x40
#define PIT_PORT_CHAN1            0x41
#define PIT_PORT_CHAN2            0x42
/* The input to channel 2 can be gated through software, using bit 0 of port 0x61 */
#define PIT_PORT_CHAN2_GATE       0x61
#define PIT_CHAN2_TIMER_BIT       1
#define PIT_CHAN2_SPEAKER_BIT     2
/* The PIT mode/command register */
#define PIT_PORT_MCR              0x43

/* PIT values to select channels, access, and operating modes */
#define PIT_SELECT_CHAN0          0b00000000
#define PIT_SELECT_CHAN1          0b01000000
#define PIT_SELECT_CHAN2          0b10000000
/*
 * In the lo/hi mode, the low-byte is sent to the data port, followed by the
 * high-byte; this makes it important that this be an atomic operation.
 */
#define PIT_SELECT_ACCESS_LOHI    0b00110000
#define PIT_SELECT_ONE_SHOT_MODE  0b00000010
#define PIT_SELECT_BINARY_MODE    0

#define APIC_BASE_MSR             0x1B
#define APIC_BASE_MSR_ENABLE      0x800

#ifndef ASM
  extern uint32_t *amd64_apic_base;
#endif

/*
 * Since amd64_apic_base is an array of 32-bit elements, these byte-offsets
 * need to be divided by 4 to index the array.
 */
#define APIC_OFFSET(val) (val >> 2)

#define APIC_REGISTER_APICID        APIC_OFFSET(0x20)
#define APIC_REGISTER_EOI           APIC_OFFSET(0x0B0)
#define APIC_REGISTER_SPURIOUS      APIC_OFFSET(0x0F0)
#define APIC_REGISTER_LVT_TIMER     APIC_OFFSET(0x320)
#define APIC_REGISTER_TIMER_INITCNT APIC_OFFSET(0x380)
#define APIC_REGISTER_TIMER_CURRCNT APIC_OFFSET(0x390)
#define APIC_REGISTER_TIMER_DIV     APIC_OFFSET(0x3E0)

#define APIC_DISABLE                0x10000
#define APIC_EOI_ACK                0
#define APIC_SELECT_TMR_PERIODIC    0x20000
#define APIC_SPURIOUS_ENABLE        0x100

/* Default divide value used by APIC timer */
#define APIC_TIMER_DIVIDE_VALUE     16
/* Value to set in register to pick the divide value above */
#define APIC_TIMER_SELECT_DIVIDER   3

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_X86_64_H */
