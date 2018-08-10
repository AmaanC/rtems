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

#include <assert.h>
#include <bsp.h>
#include <rtems.h>
#include <rtems/score/interrupts.h>
#include <rtems/timecounter.h>
#include <rtems/score/cpu.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/x86_64.h>
#include <bsp/irq-generic.h>

#ifndef CPU_NAME
#error "Missing x86_64.h"
#endif

/* Use the amd64_apic_base as a pointer into an array of 32-bit APIC registers */
uint32_t *amd64_apic_base;

static struct timecounter amd64_clock_tc;
extern volatile uint32_t Clock_driver_ticks;

static uint32_t amd64_clock_get_timecount(struct timecounter *tc)
{
  /*
   * uint32_t irqs = Clock_driver_ticks;
   * uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
   *
   * uint32_t completed_already = irqs * us_per_tick;
   * uint32_t currently_running =
   *   (double) amd64_apic_base[APIC_REGISTER_TIMER_CURRCNT] /
   *   amd64_apic_base[APIC_REGISTER_TIMER_INITCNT] *
   *   us_per_tick;
   *
   * return completed_already + currently_running;
   */
  // XXX: Return how many IRQs have occurred
  return Clock_driver_ticks;
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

void apic_initialize(void)
{
  /*
   * The APIC base address is a 36-bit physical address.
   * We have identity-paging setup at the moment, which makes this simpler, but
   * that's something to note since the variables below use virtual addresses.
   *
   * Bits 0-11 (inclusive) are 0, making the address page (4KiB) aligned.
   * Bits 12-35 (inclusive) of the MSR point to the rest of the address.
   */
  uint64_t apic_base_msr = rdmsr(APIC_BASE_MSR);
  // XXX: Needs to be global?
  amd64_apic_base = (uint32_t*) apic_base_msr;
  amd64_apic_base = (uintptr_t) amd64_apic_base & 0x0ffffff000;

  /* Hardware enable the APIC just to be sure */
  wrmsr(
    APIC_BASE_MSR,
    apic_base_msr | APIC_BASE_MSR_ENABLE,
    apic_base_msr >> 32
  );

#if 1
  printf("APIC is now at %x\n", (uintptr_t) amd64_apic_base);
  printf("APIC ID at *%x=%x\n", &amd64_apic_base[APIC_APICID], amd64_apic_base[APIC_APICID]);
  printf("APIC spurious vector register *%x=%x\n", &amd64_apic_base[APIC_SPURIOUS], amd64_apic_base[APIC_SPURIOUS]);
#endif

  /* Enable APIC through spurious vector register and map spurious vector
   * number */
  amd64_apic_base[APIC_SPURIOUS] = APIC_SPURIOUS_ENABLE | BSP_VECTOR_SPURIOUS;

  // XXX: This won't work since raw handlers need to end in iretq - use
  // attribute(interrupt)?
  /*
   * uintptr_t old;
   * amd64_install_raw_interrupt(BSP_VECTOR_SPURIOUS, apic_spurious_handler, &old);
   */

#if 1
  printf("APIC spurious vector register *%x=%x\n", &amd64_apic_base[APIC_SPURIOUS], amd64_apic_base[APIC_SPURIOUS]);
#endif

  /*
   * The PIC may send spurious IRQs even when disabled, and without remapping
   * IRQ7 would look like an exception.
   */
  pic_remap(0x20, 0x28);
  pic_disable();
}

void apic_spurious_handler(void);
void apic_dummy_timer(void);
void Clock_isr(void *param);

void xxx_apic_isr(void)
{
  Clock_isr(NULL);
  // XXX: Should be in ISR_Handler?
  amd64_apic_base[APIC_EOI] = 0;
}

uint64_t apic_timer_initialize(uint64_t irq_ticks_per_sec)
{
  uintptr_t old;
  // XXX: Enable spurious register here too?
  amd64_install_raw_interrupt(BSP_VECTOR_SPURIOUS, apic_spurious_handler, &old);

  // XXX: Should be Clock_driver_support_install_isr?
  rtems_status_code sc = rtems_interrupt_handler_install(
    BSP_VECTOR_APIC_TIMER,
    "ckinit",
    RTEMS_INTERRUPT_UNIQUE,
    xxx_apic_isr,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  amd64_apic_base[APIC_REGISTER_LVT_TIMER] = BSP_VECTOR_APIC_TIMER;
  amd64_apic_base[APIC_REGISTER_TIMER_DIV] = APIC_TIMER_SELECT_DIVIDER;

  /* Enable the channel 2 timer gate and disable the speaker output */
  uint8_t chan2_value = (inport_byte(PIT_PORT_CHAN2_GATE) | PIT_CHAN2_TIMER_BIT) & ~PIT_CHAN2_SPEAKER_BIT;
  outport_byte(PIT_PORT_CHAN2_GATE, chan2_value);

  /* Initialize PIT in one-shot mode on Channel 2 */
  outport_byte(
    PIT_PORT_MCR,
    PIT_SELECT_CHAN2 | PIT_SELECT_ACCESS_LOHI |
      PIT_SELECT_ONE_SHOT_MODE | PIT_SELECT_BINARY_MODE
  );

  amd64_disable_interrupts();
  /* Set PIT reload value */
  uint32_t pit_ticks = PIT_CALIBRATE_TICKS;
  uint8_t low_tick = ((uint16_t) pit_ticks) & 0xff;
  uint8_t high_tick = ((uint16_t) pit_ticks) >> 8;
  outport_byte(PIT_PORT_CHAN2, low_tick);
  // XXX: io_wait()?
  outport_byte(PIT_PORT_CHAN2, high_tick);
  amd64_enable_interrupts();

  /* Restart PIT by disabling the gated input and then re-enabling it */
  chan2_value &= ~PIT_CHAN2_TIMER_BIT;
  outport_byte(PIT_PORT_CHAN2, chan2_value);
  chan2_value |= PIT_CHAN2_TIMER_BIT;
  outport_byte(PIT_PORT_CHAN2, chan2_value);

  /* Start APIC timer's countdown */
  const uint32_t apic_calibrate_init_count = 0xffffffff;
  amd64_apic_base[APIC_REGISTER_TIMER_INITCNT] = apic_calibrate_init_count;

  uint32_t old_pit_ticks = PIT_CALIBRATE_TICKS;
  amd64_disable_interrupts();
  while(pit_ticks > 0 && pit_ticks < PIT_CALIBRATE_TICKS)
  {
    old_pit_ticks = pit_ticks;
    /* Send latch command to read multi-byte value atomically */
    outport_byte(PIT_PORT_MCR, PIT_SELECT_CHAN2);
    pit_ticks = inport_byte(PIT_PORT_CHAN2);
    pit_ticks |= inport_byte(PIT_PORT_CHAN2) << 8;

    /* Make sure they're strictly decreasing */
    assert(pit_ticks <= old_pit_ticks);
  }
  amd64_enable_interrupts();

  /* Stop APIC timer to calculate ticks to time ratio */
  amd64_apic_base[APIC_REGISTER_LVT_TIMER] = APIC_DISABLE;

  /* Get counts passed since we started counting */
  uint64_t amd64_apic_ticks_per_sec = apic_calibrate_init_count - amd64_apic_base[APIC_REGISTER_TIMER_CURRCNT];
  /* We ran the PIT for a fraction of a second */
  amd64_apic_ticks_per_sec = amd64_apic_ticks_per_sec * PIT_CALIBRATE_DIVIDER;

  assert(amd64_apic_ticks_per_sec != 0 && amd64_apic_ticks_per_sec != apic_calibrate_init_count);

  /* Multiply to undo effects of divider */
  uint64_t cpu_bus_frequency = amd64_apic_ticks_per_sec * APIC_TIMER_DIVIDE_VALUE;

  printf("CPU frequency: 0x%llx\nAPIC ticks/sec: 0x%llx", cpu_bus_frequency, amd64_apic_ticks_per_sec);

  /*
   * The APIC timer counter is decremented at the speed of the CPU bus
   * frequency.
   *
   * This means:
   *   cpu_time_per_tick = 1 / (cpu_bus_frequency / timer_divide_value)
   *
   * Therefore:
   *   reload_value * cpu_timer_per_tick = (1/apic_timer_frequency)
   */
  uint32_t apic_timer_reload_value = (cpu_bus_frequency / APIC_TIMER_DIVIDE_VALUE) / irq_ticks_per_sec;

  amd64_apic_base[APIC_REGISTER_LVT_TIMER] = BSP_VECTOR_APIC_TIMER | APIC_SELECT_TMR_PERIODIC;
  amd64_apic_base[APIC_REGISTER_TIMER_DIV] = APIC_TIMER_SELECT_DIVIDER;
  amd64_apic_base[APIC_REGISTER_TIMER_INITCNT] = apic_timer_reload_value;

  return amd64_apic_ticks_per_sec;
}

void amd64_clock_initialize(void)
{
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint64_t irq_ticks_per_sec = 1000000 / us_per_tick;
  printf("us_per_tick = %d\nDesired frequency = %d irqs/sec\n", us_per_tick, irq_ticks_per_sec);

  /* Setup and enable the APIC itself */
  apic_initialize();
  /* Setup and initialize the APIC timer */
  uint64_t apic_freq = apic_timer_initialize(irq_ticks_per_sec);

  amd64_clock_tc.tc_get_timecount = amd64_clock_get_timecount;
  amd64_clock_tc.tc_counter_mask = 0xffffffff;
  amd64_clock_tc.tc_frequency = irq_ticks_per_sec;
  amd64_clock_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&amd64_clock_tc);
}

#define Clock_driver_support_initialize_hardware() amd64_clock_initialize()

#include "../../../shared/dev/clock/clockimpl.h"
