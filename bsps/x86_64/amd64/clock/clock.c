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

#include <bsp.h>
#include <rtems.h>
#include <rtems/score/interrupts.h>
#include <rtems/timecounter.h>
#include <rtems/score/cpuimpl.h>
#include <bsp/irq-generic.h>

#define CLOCK_VECTOR 0

static struct timecounter amd64_clock_tc;

static uint32_t amd64_clock_get_timecount(struct timecounter *tc)
{
  // XXX:
}

/*
 * When the CPUID instruction is executed with a source operand of 1 in the EAX
 * register, bit 9 of the CPUID feature flags returned in the EDX register
 * indicates the presence (set) or absence (clear) of a local APIC.
 */
bool has_apic_support() {
  uint32_t eax, ebx, ecx, edx;
  cpuid(1, &eax, &ebx, &ecx, &edx);
  return (edx >> 9) & 1;
}

// Space reserved for APIC in linker script
extern char APICBase[];

void relocate_apic()
{
  // XXX: FreeBSD maps the first 1 GiB of physical memory to all 1 GiB of
  // virtual memory. This means that we need to move the APIC base to the first
  // 1 GiB of physical memory.

  // The linker script's virtual address may not be good enough for us, since
  // what we write to the MSR is the physical address.
  // So let's make sure its within the first 1 GiB.
  printf("Linker APICBase=%x\n", (uintptr_t) APICBase);
  // Page-aligned and within first 1 GiB
  uint32_t new_apic_base = (uintptr_t) APICBase & 0x3ffff000;
  uint32_t low = (new_apic_base & 0xfffff000) | APIC_BASE_MSR_ENABLE;
  // uint32_t high = (new_apic_base >> 32) & 0xf0;
  uint32_t high = 0;

  // Relocate APIC
  wrmsr(APIC_BASE_MSR, low, high);
}

void apic_initialize(void)
{
  printf("Initializing clock\n");
  /* Use the apic_base as a pointer into an array of 32-bit APIC registers */
  uint32_t *apic_base;
  /*
   * APIC base is a 36-bit physical address.
   *
   * Bits 0-11 (inclusive) are 0, making the address page (4KiB) aligned.
   * Bits 12-35 (inclusive) of the MSR point to the rest of the address.
   */
  apic_base = (uint32_t*) rdmsr(APIC_BASE_MSR);
  apic_base = (uintptr_t) apic_base & 0x0ffffff000;
  printf("APIC was at %x\n", (uintptr_t) apic_base);

  relocate_apic();
  // XXX: Set bit 11 of apic_base_msr to hardware enable?

  apic_base = (uint32_t*) rdmsr(APIC_BASE_MSR);
  apic_base = (uintptr_t) apic_base & 0x0ffffff000;
  printf("APIC is now at %x\n", (uintptr_t) apic_base);

  printf("APIC ID at *%x=%x\n", &apic_base[APIC_ID], apic_base[APIC_ID]);

  printf("APIC spurious vector register *%x=%x\n", &apic_base[APIC_SPURIOUS], apic_base[APIC_SPURIOUS]);
  // Enable APIC through spurious vector register and map spurious vector number
  apic_base[APIC_SPURIOUS] = APIC_SPURIOUS_ENABLE | 0xFF;
  printf("APIC spurious vector register *%x=%x\n", &apic_base[APIC_SPURIOUS], apic_base[APIC_SPURIOUS]);

  pic_disable();
  // XXX: Remap IRQs to appropriate interrupt vectors too? Especially spurious
}

// XXXXXX
void exception_handler(void* xxx)
{
  printf("Exception handler called!");
}

void test_irq()
{
  rtems_status_code sc = rtems_interrupt_handler_install(0, "#de", RTEMS_INTERRUPT_UNIQUE, exception_handler, 0);
  printf("Handler status=%x\nException triggered now!", sc);
  printf("%d", 1/0);
}

// static
void amd64_clock_initialize(void)
{
  test_irq();
  apic_initialize();

  amd64_clock_tc.tc_get_timecount = amd64_clock_get_timecount;
  amd64_clock_tc.tc_counter_mask = 0xffffffff;
  amd64_clock_tc.tc_frequency = 0; //XXXXXX;
  amd64_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&amd64_clock_tc);
}

#define Clock_driver_support_initialize_hardware() amd64_clock_initialize()

#include "../../../shared/dev/clock/clockimpl.h"
