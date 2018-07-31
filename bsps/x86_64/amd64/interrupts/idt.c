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
#include <idt.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/x86_64.h>
#include <rtems/score/cpuimpl.h>

/*
 * The IDT maps every interrupt vector to an interrupt_descriptor based on the vector
 * number
 */
interrupt_descriptor amd64_idt[IDT_SIZE] RTEMS_ALIGNED(8) = { { 0 } };

struct idt_record idtr = {
  .limit = (IDT_SIZE * 16) - 1,
  .base = (uintptr_t) amd64_idt
};

void lidt(struct idt_record *ptr)
{
  __asm__ volatile ("lidt %0" :: "m"(*ptr));
}

void sidt(void)
{
  // 2+8 bytes
  uint8_t idtr[10];
  __asm__ volatile ("sidt %0" :: "m"(idtr));
  printf("sidt = ");
  for(int i = 0; i < 10; i++) {
    printf("%x ", idtr[i]);
  }
  printf("\n");
}

interrupt_descriptor amd64_create_interrupt_descriptor(uintptr_t handler, uint8_t types_and_attributes)
{
  interrupt_descriptor entry = {
    .offset_0 = handler & 0xffff,
    .segment_selector = amd64_get_cs(),
    .interrupt_stack_table = 0,
    .type_and_attributes = types_and_attributes,
    .offset_1 = (handler >> 16) & 0xffff,
    .offset_2 = handler >> 32,
    .reserved_zero = 0,
  };
  return entry;
}

uintptr_t amd64_get_handler_from_idt(uint32_t vector)
{
  interrupt_descriptor entry = amd64_idt[vector];
  uintptr_t handler = entry.offset_0 | (entry.offset_1 << 16) | ((uint64_t) entry.offset_2 << 32);
  return handler;
}

struct interrupt_frame {
  uint64_t xxx_unknown;
};

void __attribute__((interrupt)) exception_de_handler(struct interrupt_frame *frame)
{
  printf("!!! Divide by 0 exception !!!\n");
}

void __attribute__((interrupt)) exception_generic_handler(struct interrupt_frame *frame, uint64_t error)
{
  printf("!!! Generic exception %x !!!\n", error);
}

void disable_pic(void)
{
  // Mask all lines on both master and slave PIC to disable
  outport_byte(PIC1_DATA, 0xff);
  outport_byte(PIC2_DATA, 0xff);
}

void amd64_install_interrupt(uint32_t vector, uintptr_t new_handler, uintptr_t *old_handler)
{
  // XXX: Locks or ISR disabling?
  *old_handler = amd64_get_handler_from_idt(vector);
  interrupt_descriptor new_desc = amd64_create_interrupt_descriptor(
    new_handler,
    IDT_INTERRUPT_GATE | IDT_PRESENT
  );
  amd64_idt[vector] = new_desc;
}

void init_idt(void)
{
  // XXX: Test!
  uintptr_t old;
  amd64_install_interrupt(0, exception_de_handler, &old);
  printf("exception_de_handler=%x, idt[0]=%x\n", &exception_de_handler, amd64_get_handler_from_idt(0));

  amd64_install_interrupt(8, exception_generic_handler, &old);

  lidt(&idtr);
  sidt();

  printf("Exception triggered now!");
  printf("%d", 1/0);
}
