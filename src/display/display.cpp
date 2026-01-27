/**
 *@brief Main file for display functions
 *
 */
#include "display.hpp"
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

static int current_value;
static int last_value;

/**
 *@brief update display safely
 *
 *@note Display needs to be rotated 1800 degrece to fit needs.
 */
static void lvgl_update(void) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%d", current_value);

  lv_label_set_text(label, buf);
  lv_obj_update_layout(box);

  int w = lv_obj_get_width(box);
  int h = lv_obj_get_height(box);

  lv_obj_set_style_transform_angle(top_box, 1800, 0);
  lv_obj_set_style_transform_pivot_x(top_box, lv_obj_get_width(top_box) / 2, 0);
  lv_obj_set_style_transform_pivot_y(top_box, lv_obj_get_height(top_box) / 2,
                                     0);

  lv_obj_set_style_transform_angle(box, 1800, 0);
  lv_obj_set_style_transform_pivot_x(box, w / 2, 0);
  lv_obj_set_style_transform_pivot_y(box, h / 2, 0);
}

/**
 *@brief Initialize the display safely and set "JOBO" label at the top.
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

  top_box = lv_obj_create(lv_scr_act());
  lv_obj_remove_style_all(top_box);
  lv_obj_set_size(top_box, LV_PCT(100), 30);
  lv_obj_align(top_box, LV_ALIGN_BOTTOM_MID, 0, 0);

  top_label = lv_label_create(top_box);
  lv_label_set_text(top_label, "JOBO");
  lv_obj_center(top_label);

  box = lv_obj_create(lv_scr_act());
  lv_obj_remove_style_all(box);
  lv_obj_set_size(box, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_center(box);

  label = lv_label_create(box);
  lv_obj_center(label);

  lv_unlock();

  last_value = 0x7fffffff;
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
