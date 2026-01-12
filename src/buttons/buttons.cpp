#include "buttons.hpp"

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>

static const gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const gpio_dt_spec sw1 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
static const gpio_dt_spec sw2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);

static atomic_t b0;
static atomic_t b1;
static atomic_t b2;

static gpio_callback cb0;
static gpio_callback cb1;
static gpio_callback cb2;

static inline void toggle01(atomic_t *v) { atomic_xor(v, 1); }

static void isr0(const device *dev, gpio_callback *cb, uint32_t pins) {
  (void)dev;
  (void)cb;
  (void)pins;
  toggle01(&b0);
}

static void isr1(const device *dev, gpio_callback *cb, uint32_t pins) {
  (void)dev;
  (void)cb;
  (void)pins;
  toggle01(&b1);
}

static void isr2(const device *dev, gpio_callback *cb, uint32_t pins) {
  (void)dev;
  (void)cb;
  (void)pins;
  toggle01(&b2);
}

void buttons_init_irq(void) {
  gpio_pin_configure_dt(&sw0, GPIO_INPUT);
  gpio_pin_interrupt_configure_dt(&sw0, GPIO_INT_EDGE_TO_ACTIVE);
  gpio_init_callback(&cb0, isr0, BIT(sw0.pin));
  gpio_add_callback(sw0.port, &cb0);

  gpio_pin_configure(sw1.port, sw1.pin, GPIO_INPUT | GPIO_PULL_DOWN);
  gpio_pin_interrupt_configure(sw1.port, sw1.pin, GPIO_INT_EDGE_TO_ACTIVE);
  gpio_init_callback(&cb1, isr1, BIT(sw1.pin));
  gpio_add_callback(sw1.port, &cb1);

  gpio_pin_configure(sw2.port, sw2.pin, GPIO_INPUT | GPIO_PULL_DOWN);
  gpio_pin_interrupt_configure(sw2.port, sw2.pin, GPIO_INT_EDGE_TO_ACTIVE);
  gpio_init_callback(&cb2, isr2, BIT(sw2.pin));
  gpio_add_callback(sw2.port, &cb2);
}

uint8_t button0_get(void) { return (uint8_t)(atomic_get(&b0) & 1); }
uint8_t button1_get(void) { return (uint8_t)(atomic_get(&b1) & 1); }
uint8_t button2_get(void) { return (uint8_t)(atomic_get(&b2) & 1); }
