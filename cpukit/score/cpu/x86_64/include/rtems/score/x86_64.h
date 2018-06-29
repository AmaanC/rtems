#ifndef _RTEMS_SCORE_X86_64_H
#define _RTEMS_SCORE_X86_64_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the NO CPU family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "x86-64"
#define CPU_MODEL_NAME "XXX: x86-64 generic"

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

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_X86_64_H */
