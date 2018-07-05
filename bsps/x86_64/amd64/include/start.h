/*
 *  COPYRIGHT (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_AMD64_START_H
#define LIBBSP_AMD64_START_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Entry point for generated ELF.
 *
 * The linkcmds script sets this function as the entry point, to be jumped into
 * the bootloader. It calls boot_card and kicks the whole RTEMS initialization
 * process off.
 */
void _start(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_AMD64_START_H */
