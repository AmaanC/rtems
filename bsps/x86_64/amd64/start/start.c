/*
 *  COPYRIGHT (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <start.h>
#include <bsp/bootcard.h>

// XXX: _start can pass boot arguments to boot_card
// https://lists.rtems.org/pipermail/devel/2018-June/022123.html
void _start(void)
{
  boot_card("");
}
