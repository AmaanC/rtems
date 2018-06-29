/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <tmacros.h>

const char rtems_test_name[] = "HELLO WORLD";
void init_serial();
void write_serial(char a);
static rtems_task Init(
  rtems_task_argument ignored
)
{
  /*
   * rtems_print_printer_fprintf_putc(&rtems_test_printer);
   * TEST_BEGIN();
   * printf( "Hello World\n" );
   * TEST_END();
   * rtems_test_exit( 0 );
   */
  init_serial();
  write_serial('h');
  write_serial('e');
  write_serial('l');
  write_serial('l');
  write_serial('o');
  write_serial(' ');
  write_serial('w');
  write_serial('o');
  write_serial('r');
  write_serial('l');
  write_serial('d');
  write_serial(' ');
  write_serial('o');
  write_serial('n');
  write_serial(' ');
  write_serial('x');
  write_serial('6');
  write_serial('4');
  write_serial('!');

}

// XXX
//#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
//#define CONFIGURE_HAS_OWN_CONFIGURATION_TABLE

/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
