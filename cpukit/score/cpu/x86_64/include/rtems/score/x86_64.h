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

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_X86_64_H */
