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

/*
 * The IDT maps every interrupt vector to an interrupt_descriptor based on the vector
 * number
 */
interrupt_descriptor amd64_idt[IDT_SIZE] RTEMS_ALIGNED(8) = { { 0 } };

struct idt_record idtr = {
  .limit = (IDT_SIZE * 16) - 1,
  .base = (uintptr_t) amd64_idt
};

/**
 * IRQs that the RTEMS Interrupt Manager will manage
 * @see DISTINCT_INTERRUPT_ENTRY
 */
static uintptr_t rtemsIRQs[BSP_IRQ_VECTOR_NUMBER] = {
  rtems_irq_prologue_0,
  rtems_irq_prologue_1,
  rtems_irq_prologue_2,
  rtems_irq_prologue_3,
  rtems_irq_prologue_4,
  rtems_irq_prologue_5,
  rtems_irq_prologue_6,
  rtems_irq_prologue_7,
  rtems_irq_prologue_8,
  rtems_irq_prologue_9,
  rtems_irq_prologue_10,
  rtems_irq_prologue_11,
  rtems_irq_prologue_12,
  rtems_irq_prologue_13,
  rtems_irq_prologue_14,
  rtems_irq_prologue_15,
  rtems_irq_prologue_16,
};

void lidt(struct idt_record *ptr)
{
  __asm__ volatile ("lidt %0" :: "m"(*ptr));
}

void print_idtr(void)
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

void amd64_dispatch_isr(rtems_vector_number vector)
{
  printf("Dispatching ISR %x\n", vector);
  // XXX: Do more processing? Mask?
  bsp_interrupt_handler_dispatch(vector);
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  // XXX: disable+remap PIC?
  uintptr_t old;
  for(int i = 0; i <= 16; i++) {
    amd64_install_interrupt(i, rtemsIRQs[i], &old);
  }

  lidt(&idtr);
  print_idtr();

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_vector_disable(rtems_vector_number vector) {}
void bsp_interrupt_vector_enable(rtems_vector_number vector) {}
