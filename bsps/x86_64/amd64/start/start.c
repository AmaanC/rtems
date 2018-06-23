// XXX: _start can pass boot arguments to boot_card
// https://lists.rtems.org/pipermail/devel/2018-June/022123.html
void _start(void);

void _start(void)
{
  boot_card("");
}
