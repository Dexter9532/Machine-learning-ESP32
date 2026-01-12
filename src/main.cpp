#include "buttons/buttons.hpp"
#include "display/display.hpp"
#include <zephyr/kernel.h>

extern "C" int main(void) {
  display_init();
  buttons_init_irq();

  uint8_t last_v = 0;

  while (1) {
    uint8_t b0 = button0_get();
    uint8_t b1 = button1_get();
    uint8_t b2 = button2_get();

    uint8_t v = 5;

    if (v != last_v) {
      last_v = v;
      display_set_value(v);
    }

    k_msleep(1000);
  }
}
