/*
 *  COPYRIGHT (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_start(void)
{
}

/*
 * XXX: We may want a custom bsp_work_area_initialize
 *
 * FreeBSD's bootloader may leave a bootinfo structure for the kernel to find later:
 * http://fxr.watson.org/fxr/source/i386/include/bootinfo.h?v=FREEBSD11#L48
 */
