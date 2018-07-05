/**
 *  @file
 *
 *  @brief x86_64 Dependent Source
 */

/*
 *  COPYRIGHT (c) 2018.
 *  Amaan Cheval <amaan.cheval@gmail.com>.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/tls.h>

Context_Control_fp _CPU_Null_fp_context;
void _CPU_Exception_frame_print (const CPU_Exception_frame *ctx) {}

void _CPU_Initialize(void)
{
}

uint32_t   _CPU_ISR_Get_level( void )
{
  return 0;
}


void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
}

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
}


void _CPU_Install_interrupt_stack( void )
{
}


void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  for( ; ; ) { }
}
