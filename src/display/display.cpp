/**
 *@brief Main file for display functions
 *
 */
//! @note Samma kommentarer gällande sortering av headerfiler, anonyma namnrymder vs static som 
//! i buttons.cpp gäller här.

#include "display.hpp"
#include "buttons/buttons.hpp"
#include "misc/lv_area.h"

#include <lvgl.h>
#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

static const struct gpio_dt_spec tft_pwr =
    GPIO_DT_SPEC_GET(DT_NODELABEL(i2c_reg), enable_gpios);

static const struct gpio_dt_spec bl =
    GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios);

static lv_obj_t *box;
static lv_obj_t *label;

static lv_obj_t *top_box;
static lv_obj_t *top_label;

static lv_obj_t *bottom_box;
static lv_obj_t *bottom_label;

static int current_value;
static int last_value;

static constexpr int DISPLAY_VALUE_INVALID = -1;

/**
 *@brief update display safely
 *
 *@note Display needs to be rotated 1800 degrece to fit needs.
 */
static void lvgl_update(void) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%d", current_value);
  lv_label_set_text(label, buf);

  uint8_t b0 = button0_get() & 1; // MSB
  uint8_t b1 = button1_get() & 1;
  uint8_t b2 = button2_get() & 1; // LSB

  char bin[4];
  snprintf(bin, sizeof(bin), "%d%d%d", b0, b1, b2);

  lv_label_set_text(bottom_label, bin);
}

/**
 *@brief Initialize the display safely and set labels and boxes.
 */
void display_init(void) {
  const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

  if (device_is_ready(tft_pwr.port)) {
    gpio_pin_configure_dt(&tft_pwr, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set_dt(&tft_pwr, 1);
  }

  if (device_is_ready(bl.port)) {
    gpio_pin_configure_dt(&bl, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set_dt(&bl, 1);
  }

  if (device_is_ready(display)) {
    display_blanking_off(display);
  }

  while (lv_disp_get_default() == NULL) {
    k_msleep(10);
  }

  lv_lock();

  /* ---------------- TOP BOX (JOBO) ---------------- */
  top_box = lv_obj_create(lv_scr_act());
  lv_obj_remove_style_all(top_box);
  lv_obj_set_size(top_box, LV_PCT(100), 30);
  lv_obj_align(top_box, LV_ALIGN_BOTTOM_MID, 0, 0);

  top_label = lv_label_create(top_box);
  lv_label_set_text(top_label, "JOBO");
  lv_obj_center(top_label);

  lv_obj_update_layout(top_box);

  lv_obj_set_style_transform_angle(top_box, 1800, 0);
  lv_obj_set_style_transform_pivot_x(top_box, lv_obj_get_width(top_box) / 2, 0);
  lv_obj_set_style_transform_pivot_y(top_box, lv_obj_get_height(top_box) / 2,
                                     0);

  /* ---------------- CENTER BOX (prediction) ---------------- */
  box = lv_obj_create(lv_scr_act());
  lv_obj_remove_style_all(box);
  lv_obj_set_size(box, LV_PCT(100), LV_SIZE_CONTENT);
  lv_obj_center(box);

  label = lv_label_create(box);
  lv_obj_center(label);

  lv_obj_update_layout(box);

  lv_obj_set_style_transform_angle(box, 1800, 0);
  lv_obj_set_style_transform_pivot_x(box, lv_obj_get_width(box) / 2, 0);
  lv_obj_set_style_transform_pivot_y(box, lv_obj_get_height(box) / 2, 0);

  /* ---------------- BOTTOM BINARY BOX ---------------- */
  bottom_box = lv_obj_create(lv_scr_act());
  lv_obj_remove_style_all(bottom_box);
  lv_obj_set_size(bottom_box, LV_PCT(100), LV_SIZE_CONTENT);
  lv_obj_align(bottom_box, LV_ALIGN_TOP_MID, 0, 0);

  bottom_label = lv_label_create(bottom_box);
  lv_obj_set_width(bottom_label, 80);
  lv_obj_set_style_text_align(bottom_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(bottom_label, "000");
  lv_obj_center(bottom_label);

  lv_obj_update_layout(bottom_box);

  lv_obj_set_style_transform_angle(bottom_box, 1800, 0);
  lv_obj_set_style_transform_pivot_x(bottom_box,
                                     lv_obj_get_width(bottom_box) / 2, 0);
  lv_obj_set_style_transform_pivot_y(bottom_box,
                                     lv_obj_get_height(bottom_box) / 2, 0);

  lv_unlock();

  last_value = DISPLAY_VALUE_INVALID;
}

/**
 *@brief Set display value.
 *
 *@return if value is the same as before.
 */
void display_set_value(int value) {
  current_value = value;

  if (current_value == last_value) {
    return;
  }

  last_value = current_value;

  lv_lock();
  lvgl_update();
  lv_timer_handler();
  lv_unlock();
}
