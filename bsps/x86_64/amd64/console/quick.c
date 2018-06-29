// XXX: Quick test!

#define inport_byte( _port, _value )                \
  do { unsigned short __port  = _port;              \
    unsigned char  __value = 0;                     \
                                                    \
    __asm__ volatile ( "inb %1,%0" : "=a" (__value) \
                       : "d"  (__port)              \
      );                                            \
    _value = __value;                               \
  } while (0)

#define outport_byte( _port, _value )                                   \
  do { unsigned short __port  = _port;                                  \
    unsigned char  __value = _value;                                    \
                                                                        \
    __asm__ volatile ( "outb %0,%1" : : "a" (__value), "d" (__port) );  \
  } while (0)

#define PORT 0x3f8   /* COM1 */

void init_serial() {
  outport_byte(PORT + 1, 0x00);    // Disable all interrupts
  outport_byte(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outport_byte(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outport_byte(PORT + 1, 0x00);    //                  (hi byte)
  outport_byte(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
  outport_byte(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outport_byte(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int is_transmit_empty() {
  int in = 0;
  inport_byte(PORT + 5, in);
  return in & 0x20;
}

void write_serial(char a) {
  while (is_transmit_empty() == 0);

  outport_byte(PORT,a);
}
