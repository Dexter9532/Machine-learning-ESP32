#include "display/display.hpp"
#include <zephyr/kernel.h>

extern "C" int main(void) {
  display_init();

  int v = 3;

  while (1) {
    display_set_value(v);
    k_msleep(1000);
  }
}
