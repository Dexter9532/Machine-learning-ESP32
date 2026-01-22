#include "buttons.hpp"
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#define DEBOUNCE_MS 5

static inline bool time_ok(atomic_t *t_last) {
  uint32_t now = k_uptime_get_32();
  uint32_t last = (uint32_t)atomic_get(t_last);

  if ((uint32_t)(now - last) < DEBOUNCE_MS) {
    return false;
  }

  atomic_set(t_last, (atomic_val_t)now);
  return true;
}

static const gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const gpio_dt_spec sw1 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
static const gpio_dt_spec sw2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);

static atomic_t t0;
static atomic_t t1;
static atomic_t t2;

static atomic_t b0;
static atomic_t b1;
static atomic_t b2;

static gpio_callback cb0;
static gpio_callback cb1;
static gpio_callback cb2;

static atomic_t armed0 = 1;
static atomic_t armed1 = 1;
static atomic_t armed2 = 1;

static inline void toggle01(atomic_t *v) { atomic_xor(v, 1); }

static void isr0(const device *dev, gpio_callback *cb, uint32_t pins) {
  (void)dev;
  (void)cb;
  (void)pins;

  if (!time_ok(&t0))
    return;

  int level = gpio_pin_get_dt(&sw0);

  if (level == 1) {
    if (atomic_get(&armed0)) {
      toggle01(&b0);
      atomic_set(&armed0, 0);
    }
  } else {
    atomic_set(&armed0, 1);
  }
}

static void isr1(const device *dev, gpio_callback *cb, uint32_t pins) {
  (void)dev;
  (void)cb;
  (void)pins;

  if (!time_ok(&t1)) {
    return;
  }

  int level = gpio_pin_get_dt(&sw1);

  if (level == 1) {
    if (atomic_get(&armed1)) {
      toggle01(&b1);
      atomic_set(&armed1, 0);
    }
  } else {
    atomic_set(&armed1, 1);
  }
}

static void isr2(const device *dev, gpio_callback *cb, uint32_t pins) {
  (void)dev;
  (void)cb;
  (void)pins;

  int level = gpio_pin_get_dt(&sw2);

  if (level == 1) {
    if (atomic_get(&armed2)) {
      toggle01(&b2);
      atomic_set(&armed2, 0);
    }
  } else {
    atomic_set(&armed2, 1);
  }
}

void buttons_init_irq(void) {

  atomic_set(&t0, 0);
  atomic_set(&t1, 0);
  atomic_set(&t2, 0);

  atomic_set(&armed0, 1);
  atomic_set(&armed1, 1);
  atomic_set(&armed2, 1);

  gpio_pin_configure_dt(&sw0, GPIO_INPUT);
  gpio_pin_interrupt_configure_dt(&sw0, GPIO_INT_EDGE_BOTH);
  gpio_init_callback(&cb0, isr0, BIT(sw0.pin));
  gpio_add_callback(sw0.port, &cb0);

  gpio_pin_configure(sw1.port, sw1.pin, GPIO_INPUT | GPIO_PULL_DOWN);
  gpio_pin_interrupt_configure(sw1.port, sw1.pin, GPIO_INT_EDGE_BOTH);
  gpio_init_callback(&cb1, isr1, BIT(sw1.pin));
  gpio_add_callback(sw1.port, &cb1);

  gpio_pin_configure(sw2.port, sw2.pin, GPIO_INPUT | GPIO_PULL_DOWN);
  gpio_pin_interrupt_configure(sw2.port, sw2.pin, GPIO_INT_EDGE_BOTH);
  gpio_init_callback(&cb2, isr2, BIT(sw2.pin));
  gpio_add_callback(sw2.port, &cb2);
}

uint8_t button0_get(void) { return (uint8_t)(atomic_get(&b0) & 1); }
uint8_t button1_get(void) { return (uint8_t)(atomic_get(&b1) & 1); }
uint8_t button2_get(void) { return (uint8_t)(atomic_get(&b2) & 1); }
