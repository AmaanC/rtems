/*
 *  COPYRIGHT (c) 2018.
 *  Amaan Cheval <amaan.cheval@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <libchip/ns16550.h>
#include <rtems/bspIo.h>
#include <bsp.h>
#include <bsp/console-termios.h>
#include <rtems/score/cpuimpl.h>

static uint8_t amd64_uart_get_register(uintptr_t addr, uint8_t i)
{
  return inport_byte(addr + i);
}

static void amd64_uart_set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  outport_byte(addr + i, val);
}

static ns16550_context amd64_uart_context = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART"),
  .get_reg = amd64_uart_get_register,
  .set_reg = amd64_uart_set_register,
  .port = (uintptr_t) COM1_BASE_IO,
  .initial_baud = COM1_CLOCK_RATE
};

/*
 * XXX: We should use the interrupt based handler once interrupts are supported
 */
const console_device console_device_table[] = {
    {
      .device_file = "/dev/console",
      .probe = console_device_probe_default,
      .handler = &ns16550_handler_polled,
      .context = &amd64_uart_context.base
    }
};
const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);

static void output_char(char c)
{
  rtems_termios_device_context *ctx = console_device_table[0].context;

  ns16550_polled_putchar(ctx, c);
}

BSP_output_char_function_type BSP_output_char = output_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;
