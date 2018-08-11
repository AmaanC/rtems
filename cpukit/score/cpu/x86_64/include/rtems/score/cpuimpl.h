/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
 *
 * Copyright (c) 2013, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#define CPU_PER_CPU_CONTROL_SIZE 0

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

static inline uint8_t inport_byte(uint16_t port)
{
  uint8_t ret;
  __asm__ volatile ( "inb %1, %0"
                     : "=a" (ret)
                     : "Nd" (port) );
  return ret;
}

static inline void outport_byte(uint16_t port, uint8_t val)
{
  __asm__ volatile ( "outb %0, %1" : : "a" (val), "Nd" (port) );
}

RTEMS_INLINE_ROUTINE void _CPU_Context_volatile_clobber( uintptr_t pattern )
{
  /* TODO */
}

RTEMS_INLINE_ROUTINE void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( ".word 0" );
}

RTEMS_INLINE_ROUTINE void _CPU_Context_validate( uintptr_t pattern )
{
  while (1) {
    /* TODO */
  }
}

RTEMS_INLINE_ROUTINE void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

static inline uint16_t amd64_get_cs(void)
{
  uint16_t segment = 0;

  __asm__ volatile ( "movw %%cs, %0" : "=r" (segment) : "0" (segment) );

  return segment;
}

static inline void amd64_set_cr3(uint64_t segment)
{
  __asm__ volatile ( "movq %0, %%cr3" : "=r" (segment) : "0" (segment) );
}

static inline void cpuid(int code, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
  __asm__ volatile ( "cpuid"
                     : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                     : "a" (code) );
}

static inline uint64_t rdmsr(uint32_t msr)
{
  uint32_t low, high;
  __asm__ volatile ( "rdmsr" :
                     "=a" (low), "=d" (high) :
                     "c" (msr) );
   return low | (uint64_t) high << 32;
}

static inline void wrmsr(uint32_t msr, uint32_t low, uint32_t high)
{
  __asm__ volatile ( "wrmsr" : :
                     "a" (low), "d" (high), "c" (msr) );
}

static inline void amd64_enable_interrupts(void)
{
  __asm__ volatile ( "sti" );
}

static inline void amd64_disable_interrupts(void)
{
  __asm__ volatile ( "cli" );
}

static inline void stub_io_wait(void)
{
  /* XXX: This likely won't be required on any modern boards, but this function
   * exists so it's easier to find all the places it may be used.
   */
}
#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
