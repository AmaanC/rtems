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

#include <stdint.h>
#include <rtems.h>
#include <rtems/score/interrupts.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/x86_64.h>
#include <rtems/score/cpuimpl.h>
#include <bsp/irq-generic.h>

#define PIC1          0x20  /* IO base address for master PIC */
#define PIC2          0xA0  /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA    (PIC2+1)

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4       0x01  /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02  /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04  /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08  /* Level triggered (edge) mode */
#define ICW1_INIT       0x10  /* Initialization - required! */

#define ICW4_8086       0x01  /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02  /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C  /* Buffered mode/master */
#define ICW4_SFNM       0x10  /* Special fully nested (not) */

void io_wait(void)
{
  /* XXX: This likely won't be required on any modern boards */
}

void pic_remap(int offset1, int offset2)
{
  uint8_t a1, a2;

  /* save masks */
  a1 = inport_byte(PIC1_DATA);
  a2 = inport_byte(PIC2_DATA);

  /* starts the initialization sequence (in cascade mode) */
  outport_byte(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);
  io_wait();
  outport_byte(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
  io_wait();
  /* ICW2: Master PIC vector offset */
  outport_byte(PIC1_DATA, offset1);
  io_wait();
  /* ICW2: Slave PIC vector offset */
  outport_byte(PIC2_DATA, offset2);
  io_wait();
  /* ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) */
  outport_byte(PIC1_DATA, 4);
  io_wait();
  /* ICW3: tell Slave PIC its cascade identity (0000 0010) */
  outport_byte(PIC2_DATA, 2);
  io_wait();

  outport_byte(PIC1_DATA, ICW4_8086);
  io_wait();
  outport_byte(PIC2_DATA, ICW4_8086);
  io_wait();

  /* restore saved masks. */
  outport_byte(PIC1_DATA, a1);
  outport_byte(PIC2_DATA, a2);
}

void pic_disable(void)
{
  /* Mask all lines on both master and slave PIC to disable */
  outport_byte(PIC1_DATA, 0xff);
  outport_byte(PIC2_DATA, 0xff);
}
